/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "PluginEditor.h"

//==============================================================================
CyclingPannerAudioProcessor::CyclingPannerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              )
#endif
      ,
      parameters(
          *this, nullptr, juce::Identifier("CyclingPanner"),
          {
              std::make_unique<juce::AudioParameterFloat>(
                  "angle", "Angle",
                  juce::NormalisableRange<float>(-100.0f, 100.0f), 0.0f),
              std::make_unique<juce::AudioParameterBool>("isrotatebypass",
                                                         "Bypass", false),
              std::make_unique<juce::AudioParameterChoice>(
                  "panrule", "Pan Rule",
                  juce::StringArray("linear", "balanced", "sin3dB", "sin4p5dB",
                                    "sin6dB", "squareRoot3dB",
                                    "squareRoot4p5dB"),
                  1),
              std::make_unique<juce::AudioParameterFloat>(
                  "panangle", "Pan Angle",
                  juce::NormalisableRange<float>(-100.0f, 100.0f), 0.0f),
              std::make_unique<juce::AudioParameterBool>("ispanbypass",
                                                         "Bypass", false),
              std::make_unique<juce::AudioParameterChoice>(
                  "splitpanrule", "Split Pan Rule",
                  juce::StringArray("linear", "balanced", "sin3dB", "sin4p5dB",
                                    "sin6dB", "squareRoot3dB",
                                    "squareRoot4p5dB"),
                  1),
              std::make_unique<juce::AudioParameterFloat>(
                  "splitpananglediff", "Split Pan Angle Diff",
                  juce::NormalisableRange<float>(-200.0f, 200.0f), 200.0f),
              std::make_unique<juce::AudioParameterFloat>(
                  "splitpancenterpos", "Split Pan Center Pos",
                  juce::NormalisableRange<float>(-100.0f, 100.0f), 0.0f),
              std::make_unique<juce::AudioParameterBool>("issplitpanbypass",
                                                         "Bypass", false),
              std::make_unique<juce::AudioParameterFloat>(
                  "splitpanlangle", "Split Pan L Angle (Read only)",
                  juce::NormalisableRange<float>(-100.0f, 100.0f), -100.0f,
                  juce::String(), juce::AudioProcessorParameter::analysisMeter),
              std::make_unique<juce::AudioParameterFloat>(
                  "splitpanrangle", "Split Pan R Angle (Read only)",
                  juce::NormalisableRange<float>(-100.0f, 100.0f), 100.0f,
                  juce::String(), juce::AudioProcessorParameter::analysisMeter),
          }) {

  angle = parameters.getRawParameterValue("angle");
  isRotateBypass = parameters.getRawParameterValue("isrotatebypass");
  panRule = parameters.getRawParameterValue("panrule");
  panAngle = parameters.getRawParameterValue("panangle");
  isPanBypass = parameters.getRawParameterValue("ispanbypass");
  splitPanRule = parameters.getRawParameterValue("splitpanrule");
  splitPanAngleDiff = parameters.getRawParameterValue("splitpananglediff");
  splitPanCenterPos = parameters.getRawParameterValue("splitpancenterpos");
  isSplitPanBypass = parameters.getRawParameterValue("issplitpanbypass");
  splitPanLAngle = parameters.getRawParameterValue("splitpanlangle");
  splitPanRAngle = parameters.getRawParameterValue("splitpanrangle");

  parameters.addParameterListener("splitpananglediff",
                                  &splitPanAngleDiffListener);
  parameters.addParameterListener("splitpancenterpos",
                                  &splitPanCenterPosListener);
  parameters.addParameterListener("splitpanlangle", &splitPanAngleListener_L);
  parameters.addParameterListener("splitpanrangle", &splitPanAngleListener_R);
}

CyclingPannerAudioProcessor::~CyclingPannerAudioProcessor() {}
//==============================================================================
void splitPanAngleDiffListener::parameterChanged(
    const juce::String& parameterID, float newValue) {
  juce::Value panValue_L = parameters.getParameterAsValue("splitpanlangle");
  juce::Value panValue_R = parameters.getParameterAsValue("splitpanrangle");

  myProcessor.splitPanAngleListener_L.isChangedByParameters = true;
  myProcessor.splitPanAngleListener_R.isChangedByParameters = true;
  float newPanValue_L = myProcessor.calcSplitPanL(
      newValue, parameters.getParameterAsValue("splitpancenterpos").getValue());
  float newPanValue_R = myProcessor.calcSplitPanR(
      newValue, parameters.getParameterAsValue("splitpancenterpos").getValue());
  panValue_L.setValue(newPanValue_L);
  panValue_R.setValue(newPanValue_R);
  myProcessor.splitPanAngleListener_L.oldValue = newPanValue_L;
  myProcessor.splitPanAngleListener_R.oldValue = newPanValue_R;

  myProcessor.splitPanAngleListener_L.isChangedByParameters = false;
  myProcessor.splitPanAngleListener_R.isChangedByParameters = false;
}

void splitPanCenterPosListener::parameterChanged(
    const juce::String& parameterID, float newValue) {
  juce::Value panValue_L = parameters.getParameterAsValue("splitpanlangle");
  juce::Value panValue_R = parameters.getParameterAsValue("splitpanrangle");

  myProcessor.splitPanAngleListener_L.isChangedByParameters = true;
  myProcessor.splitPanAngleListener_R.isChangedByParameters = true;
  float newPanValue_L = myProcessor.calcSplitPanL(
      parameters.getParameterAsValue("splitpananglediff").getValue(), newValue);
  float newPanValue_R = myProcessor.calcSplitPanR(
      parameters.getParameterAsValue("splitpananglediff").getValue(), newValue);
  panValue_L.setValue(newPanValue_L);
  panValue_R.setValue(newPanValue_R);
  myProcessor.splitPanAngleListener_L.oldValue = newPanValue_L;
  myProcessor.splitPanAngleListener_R.oldValue = newPanValue_R;
  myProcessor.splitPanAngleListener_L.isChangedByParameters = false;
  myProcessor.splitPanAngleListener_R.isChangedByParameters = false;
}

void splitPanAngleListener::parameterChanged(const juce::String& parameterID,
                                             float newValue) {
  if (!isChangedByParameters) {
    parameters.getParameterAsValue(parameterID).setValue(oldValue);
  }
}
//==============================================================================
float CyclingPannerAudioProcessor::calcSplitPanL(float splitPanAngleDiff,
                                                 float splitPanCenterPos) {
  float edgeToCenterPosValue = 100 - fabs(splitPanCenterPos);
  float diffHalf = fabs(splitPanAngleDiff / 2);

  float value = 0.0f;

  if (edgeToCenterPosValue <= diffHalf) {
    value = splitPanCenterPos -
            (int)(splitPanAngleDiff / fabsf(splitPanAngleDiff)) *
                edgeToCenterPosValue;
  } else {
    value = splitPanCenterPos - (float)(splitPanAngleDiff / 2);
  }

  if (100.0f < fabsf(value) || fabs(100.0f - fabsf(value)) < FLT_EPSILON) {
    value = (int)(value / fabsf(value)) * 100.0f;
  }
  return value;
}

float CyclingPannerAudioProcessor::calcSplitPanR(float splitPanAngleDiff,
                                                 float splitPanCenterPos) {
  float edgeToCenterPosValue = 100 - fabs(splitPanCenterPos);
  float diffHalf = fabs(splitPanAngleDiff / 2);

  float value = 0.0f;

  if (edgeToCenterPosValue <= diffHalf) {
    value = splitPanCenterPos +
            (int)(splitPanAngleDiff / fabsf(splitPanAngleDiff)) *
                edgeToCenterPosValue;
  } else {
    value = splitPanCenterPos + (float)(splitPanAngleDiff / 2);
  }

  if (100.0f < fabsf(value) || fabs(100.0f - fabsf(value)) < FLT_EPSILON) {
    value = (int)(value / fabsf(value)) * 100.0f;
  }
  return value;
}
//==============================================================================
const juce::String CyclingPannerAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool CyclingPannerAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool CyclingPannerAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool CyclingPannerAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double CyclingPannerAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int CyclingPannerAudioProcessor::getNumPrograms() {
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int CyclingPannerAudioProcessor::getCurrentProgram() { return 0; }

void CyclingPannerAudioProcessor::setCurrentProgram(int index) {}

const juce::String CyclingPannerAudioProcessor::getProgramName(int index) {
  return {};
}

void CyclingPannerAudioProcessor::changeProgramName(
    int index, const juce::String& newName) {}

//==============================================================================
void CyclingPannerAudioProcessor::prepareToPlay(double sampleRate,
                                                int samplesPerBlock) {
  floatEngine.prepare(sampleRate, 2, samplesPerBlock);
  doubleEngine.prepare(sampleRate, 2, samplesPerBlock);
}

void CyclingPannerAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CyclingPannerAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void CyclingPannerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                               juce::MidiBuffer& midiMessages) {
  floatEngine.process(buffer, midiMessages);
}
void CyclingPannerAudioProcessor::processBlock(
    juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) {
  doubleEngine.process(buffer, midiMessages);
}

template <typename FloatType>
void Engine<FloatType>::process(juce::AudioBuffer<FloatType>& buffer,
                                juce::MidiBuffer& midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = 2;
  auto totalNumOutputChannels = 2;

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // This is the place where you'd normally do the guts of your plugin's
  // audio processing...
  // Make sure to reset the state if your inner loop is processing
  // the samples and the outer loop is handling the channels.
  // Alternatively, you can process the samples with the channels
  // interleaved by keeping the same state.
  int buffLength = buffer.getNumSamples();
  if (!*myProcessor.isRotateBypass) {
    auto* leftAudioBuff = buffer.getWritePointer(0);
    auto* rightAudioBuff = buffer.getWritePointer(1);
    double cacheR, cacheL;
    for (int samples = 0; samples < buffLength; samples++) {
      cacheL = *leftAudioBuff;
      cacheR = *rightAudioBuff;
      *leftAudioBuff = (FloatType)(cacheL * cos(-*myProcessor.angle * 90.0 /
                                                100.0 * M_PI / 180.0) +
                                   cacheR * sin(-*myProcessor.angle * 90.0 /
                                                100.0 * M_PI / 180.0));
      *rightAudioBuff = (FloatType)(cacheR * cos(-*myProcessor.angle * 90.0 /
                                                 100.0 * M_PI / 180.0) -
                                    cacheL * sin(-*myProcessor.angle * 90.0 /
                                                 100.0 * M_PI / 180.0));
      leftAudioBuff++;
      rightAudioBuff++;
    }
  }

  if (!*myProcessor.isPanBypass) {
    juce::dsp::AudioBlock<FloatType> audioBlock(buffer);
    juce::dsp::ProcessContextReplacing<FloatType> context(audioBlock);

    switch ((int)*myProcessor.panRule) {
      case 0:
        pan.setRule(juce::dsp::PannerRule::linear);
        break;
      case 1:
        pan.setRule(juce::dsp::PannerRule::balanced);
        break;
      case 2:
        pan.setRule(juce::dsp::PannerRule::sin3dB);
        break;
      case 3:
        pan.setRule(juce::dsp::PannerRule::sin4p5dB);
        break;
      case 4:
        pan.setRule(juce::dsp::PannerRule::sin6dB);
        break;
      case 5:
        pan.setRule(juce::dsp::PannerRule::squareRoot3dB);
        break;
      case 6:
        pan.setRule(juce::dsp::PannerRule::squareRoot4p5dB);
        break;
    }
    pan.setPan((float)(*myProcessor.panAngle / 100.0));
    pan.process(context);
  }

  if (!*myProcessor.isSplitPanBypass) {
    juce::AudioBuffer<FloatType> cacheR(2, buffLength);
    cacheR.copyFrom(0, 0, buffer, 1, 0, buffLength);
    cacheR.copyFrom(1, 0, buffer, 1, 0, buffLength);
    juce::AudioBuffer<FloatType> cacheL(2, buffLength);
    cacheL.copyFrom(0, 0, buffer, 0, 0, buffLength);
    cacheL.copyFrom(1, 0, buffer, 0, 0, buffLength);
    // -------------------------Edit L channel-------------------------

    {
      juce::dsp::AudioBlock<FloatType> audioBlock(cacheL);
      juce::dsp::ProcessContextReplacing<FloatType> context(audioBlock);

      switch ((int)*myProcessor.splitPanRule) {
        case 0:
          splitPan_L.setRule(juce::dsp::PannerRule::linear);
          splitPanGain_L.setGainDecibels(-6.0f);
          break;
        case 1:
          splitPan_L.setRule(juce::dsp::PannerRule::balanced);
          splitPanGain_L.setGainDecibels(0.0f);
          break;
        case 2:
          splitPan_L.setRule(juce::dsp::PannerRule::sin3dB);
          splitPanGain_L.setGainDecibels(-3.0f);
          break;
        case 3:
          splitPan_L.setRule(juce::dsp::PannerRule::sin4p5dB);
          splitPanGain_L.setGainDecibels(-4.5f);
          break;
        case 4:
          splitPan_L.setRule(juce::dsp::PannerRule::sin6dB);
          splitPanGain_L.setGainDecibels(-6.0f);
          break;
        case 5:
          splitPan_L.setRule(juce::dsp::PannerRule::squareRoot3dB);
          splitPanGain_L.setGainDecibels(-3.0f);
          break;
        case 6:
          splitPan_L.setRule(juce::dsp::PannerRule::squareRoot4p5dB);
          splitPanGain_L.setGainDecibels(-4.5f);
          break;
      }
      splitPanGain_L.process(context);
      splitPan_L.setPan((float)(*myProcessor.splitPanLAngle / 100.0));
      splitPan_L.process(context);
    }
    // -------------------------Edit R channel-------------------------
    {
      juce::dsp::AudioBlock<FloatType> audioBlock(cacheR);
      juce::dsp::ProcessContextReplacing<FloatType> context(audioBlock);

      switch ((int)*myProcessor.splitPanRule) {
        case 0:
          splitPan_R.setRule(juce::dsp::PannerRule::linear);
          splitPanGain_R.setGainDecibels(-6.0f);
          break;
        case 1:
          splitPan_R.setRule(juce::dsp::PannerRule::balanced);
          splitPanGain_R.setGainDecibels(0.0f);
          break;
        case 2:
          splitPan_R.setRule(juce::dsp::PannerRule::sin3dB);
          splitPanGain_R.setGainDecibels(-3.0f);
          break;
        case 3:
          splitPan_R.setRule(juce::dsp::PannerRule::sin4p5dB);
          splitPanGain_R.setGainDecibels(-4.5f);
          break;
        case 4:
          splitPan_R.setRule(juce::dsp::PannerRule::sin6dB);
          splitPanGain_R.setGainDecibels(-6.0f);
          break;
        case 5:
          splitPan_R.setRule(juce::dsp::PannerRule::squareRoot3dB);
          splitPanGain_R.setGainDecibels(-3.0f);
          break;
        case 6:
          splitPan_R.setRule(juce::dsp::PannerRule::squareRoot4p5dB);
          splitPanGain_R.setGainDecibels(-4.5f);
          break;
      }
      splitPanGain_R.process(context);
      splitPan_R.setPan((float)(*myProcessor.splitPanRAngle / 100.0));
      splitPan_R.process(context);
    }
    // -------------------------Sum-------------------------
    auto* leftAudioBuff = buffer.getWritePointer(0);
    auto* rightAudioBuff = buffer.getWritePointer(1);
    auto* cacheL_LeftAudioBuff = cacheL.getWritePointer(0);
    auto* cacheL_RightAudioBuff = cacheL.getWritePointer(1);
    auto* cacheR_LeftAudioBuff = cacheR.getWritePointer(0);
    auto* cacheR_RightAudioBuff = cacheR.getWritePointer(1);
    for (int count = 0; count < buffLength; count++) {
      *leftAudioBuff = *cacheL_LeftAudioBuff + *cacheR_LeftAudioBuff;
      *rightAudioBuff = *cacheL_RightAudioBuff + *cacheR_RightAudioBuff;
      leftAudioBuff++;
      rightAudioBuff++;
      cacheL_LeftAudioBuff++;
      cacheL_RightAudioBuff++;
      cacheR_LeftAudioBuff++;
      cacheR_RightAudioBuff++;
    }
  }
}

bool CyclingPannerAudioProcessor::supportsDoublePrecisionProcessing() const {
  return true;
};
//==============================================================================
bool CyclingPannerAudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CyclingPannerAudioProcessor::createEditor() {
  return new CyclingPannerAudioProcessorEditor(*this, parameters);
}

//==============================================================================
void CyclingPannerAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData) {
  auto state = parameters.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void CyclingPannerAudioProcessor::setStateInformation(const void* data,
                                                      int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));

  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(parameters.state.getType()))
      parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new CyclingPannerAudioProcessor();
}
