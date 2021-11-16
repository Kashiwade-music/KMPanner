/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <float.h>

//#include "./JuceLibraryCode/JucePluginDefines.h"

//==============================================================================
/**
 */
class CyclingPannerAudioProcessor;
template <typename FloatType>
class Engine {
 public:
  Engine(CyclingPannerAudioProcessor& p) : myProcessor(p){};
  void process(juce::AudioBuffer<FloatType>& buffer,
               juce::MidiBuffer& midiMessages);
  void prepare(double sampleRate, int numChannels, int maximumBlockSize) {
    spec.maximumBlockSize = maximumBlockSize;
    spec.numChannels = numChannels;
    spec.sampleRate = sampleRate;
    pan.prepare(spec);
    splitPan_L.prepare(spec);
    splitPan_R.prepare(spec);
    splitPanGain_L.prepare(spec);
    splitPanGain_R.prepare(spec);
  };

 private:
  CyclingPannerAudioProcessor& myProcessor;
  juce::dsp::Panner<FloatType> pan;
  juce::dsp::Panner<FloatType> splitPan_L;
  juce::dsp::Panner<FloatType> splitPan_R;
  juce::dsp::Gain<FloatType> splitPanGain_L;
  juce::dsp::Gain<FloatType> splitPanGain_R;

  juce::dsp::ProcessSpec spec;
};

class splitPanAngleDiffListener
    : public juce::AudioProcessorValueTreeState::Listener {
 public:
  splitPanAngleDiffListener(CyclingPannerAudioProcessor& p,
                            juce::AudioProcessorValueTreeState& param)
      : myProcessor(p), parameters(param){};
  void parameterChanged(const juce::String& parameterID,
                        float newValue) override;

 private:
  CyclingPannerAudioProcessor& myProcessor;
  juce::AudioProcessorValueTreeState& parameters;
};

class splitPanCenterPosListener
    : public juce::AudioProcessorValueTreeState::Listener {
 public:
  splitPanCenterPosListener(CyclingPannerAudioProcessor& p,
                            juce::AudioProcessorValueTreeState& param)
      : myProcessor(p), parameters(param){};
  void parameterChanged(const juce::String& parameterID,
                        float newValue) override;

 private:
  CyclingPannerAudioProcessor& myProcessor;
  juce::AudioProcessorValueTreeState& parameters;
};

class splitPanAngleListener
    : public juce::AudioProcessorValueTreeState::Listener {
 public:
  splitPanAngleListener(CyclingPannerAudioProcessor& p,
                        juce::AudioProcessorValueTreeState& param,
                        juce::StringRef pID)
      : myProcessor(p), parameters(param), parameterID(pID){};
  void parameterChanged(const juce::String& parameterID,
                        float newValue) override;
  bool isChangedByParameters = false;
  float oldValue = 0.0f;

 private:
  juce::StringRef parameterID;
  CyclingPannerAudioProcessor& myProcessor;
  juce::AudioProcessorValueTreeState& parameters;
};

class CyclingPannerAudioProcessor : public juce::AudioProcessor {
 public:
  //==============================================================================
  CyclingPannerAudioProcessor();
  ~CyclingPannerAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  int processingPrecision;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
  void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;

  //==============================================================================
  bool supportsDoublePrecisionProcessing() const override;

  //==============================================================================
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  //==============================================================================
  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  Engine<float> floatEngine{*this};
  Engine<double> doubleEngine{*this};
  //==============================================================================
  std::atomic<float>* angle = nullptr;
  std::atomic<float>* isRotateBypass = nullptr;

  std::atomic<float>* panRule = nullptr;
  std::atomic<float>* panAngle = nullptr;
  std::atomic<float>* isPanBypass = nullptr;

  std::atomic<float>* splitPanRule = nullptr;
  std::atomic<float>* splitPanAngleDiff = nullptr;
  std::atomic<float>* splitPanCenterPos = nullptr;
  std::atomic<float>* isSplitPanBypass = nullptr;
  std::atomic<float>* splitPanLAngle = nullptr;
  std::atomic<float>* splitPanRAngle = nullptr;
  //==============================================================================
  float calcSplitPanL(float, float);
  float calcSplitPanR(float, float);
  splitPanAngleListener splitPanAngleListener_L{*this, parameters,
                                                "splitpanlangle"};
  splitPanAngleListener splitPanAngleListener_R{*this, parameters,
                                                "splitpanrangle"};

 private:
  //==============================================================================
  juce::AudioProcessorValueTreeState parameters;
  splitPanAngleDiffListener splitPanAngleDiffListener{*this, parameters};
  splitPanCenterPosListener splitPanCenterPosListener{*this, parameters};

  //==============================================================================

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CyclingPannerAudioProcessor)
};
