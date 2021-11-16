/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

#include "PluginProcessor.h"

//==============================================================================
CyclingPannerAudioProcessorEditor::CyclingPannerAudioProcessorEditor(
    CyclingPannerAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p),
      valueTreeState(vts),
      audioProcessor(p),
      rotatePanLabel("rotatePan", "Rotate Pan"),
      Kanit("Kanit", "ExtraLight", 30.f),
      rotatePanBypass() {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  addAndMakeVisible(rotatePanLabel);
  angleSlider.setDoubleClickReturnValue(true, 0);
  addAndMakeVisible(angleSlider);
  addAndMakeVisible(angleSlider_TextBox);
  rotatePanBypass.setImages(
      false, true, true,
      juce::ImageCache::getFromMemory(BinaryData::poweron_png,
                                      BinaryData::poweron_pngSize),
      1.0f, juce::Colour::Colour(0.f, 0.f, 0.f, 0.f),
      juce::ImageCache::getFromMemory(BinaryData::poweron_png,
                                      BinaryData::poweron_pngSize),
      1.f, juce::Colour::Colour(0.f, 0.f, 0.f, 0.f),
      juce::ImageCache::getFromMemory(BinaryData::poweroff_png,
                                      BinaryData::poweroff_pngSize),
      1.f, juce::Colour::Colour(0.f, 0.f, 0.f, 0.f), 0.f);
  rotatePanBypass.setClickingTogglesState(true);
  addAndMakeVisible(rotatePanBypass);

  addAndMakeVisible(splitPanAngleDiffSlider);
  addAndMakeVisible(splitPanCenterPosSlider);

  angleAttachment.reset(
      new SliderAttachment(valueTreeState, "angle", angleSlider));
  angleAttachment_TextBox.reset(
      new SliderAttachment(valueTreeState, "angle", angleSlider_TextBox));
  rotatePanBypassAttachment.reset(
      new ButtonAttachment(valueTreeState, "isrotatebypass", rotatePanBypass));

  splitPanAngleDiffAttachment.reset(new SliderAttachment(
      valueTreeState, "splitpananglediff", splitPanAngleDiffSlider));
  splitPanCenterPosAttachment.reset(new SliderAttachment(
      valueTreeState, "splitpancenterpos", splitPanCenterPosSlider));
  setSize(800, 600);
}

CyclingPannerAudioProcessorEditor::~CyclingPannerAudioProcessorEditor() {}

//==============================================================================
void SliderLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y,
                                         int width, int height, float sliderPos,
                                         float minSliderPos, float maxSliderPos,
                                         const juce::Slider::SliderStyle style,
                                         juce::Slider&) {
  g.setColour(juce::Colour::Colour(6, 5, 20));
  g.fillRect((float)x, (float)height / 2 + y - 7.f, (float)width,
             14.f);  // background
  g.setColour(juce::Colour::Colour(12, 134, 225));
  if ((sliderPos - x) >= (width / 2)) {  // プラス方向
    g.fillRect((float)width / 2 + x, (float)height / 2 + y - 7.f,
               (float)sliderPos - x - (width / 2),
               14.f);  // fill
  } else {
    g.fillRect((float)sliderPos, (float)height / 2 + y - 7.f,
               (float)((width / 2 + x) - sliderPos), 14.f);
  }
  g.setColour(juce::Colour::Colour(142, 149, 166));
  g.fillRect((float)sliderPos - 6.f, (float)height / 2 + y - 25.f, 12.f,
             50.f);  // thumb
  g.setColour(juce::Colour::Colour(234, 235, 238));
  g.fillRect((float)sliderPos - 6.f + 3.f, (float)height / 2 + y - 25.f + 3.f,
             6.f,
             44.f);  // thumb
}

//==============================================================================
void CyclingPannerAudioProcessorEditor::paint(juce::Graphics& g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(juce::Colour::Colour(23, 23, 32));
  g.setColour(juce::Colour::Colour(35, 35, 46));
  g.fillRoundedRectangle(50.f, 30.f, 700.f, 160.f, 10.f);

  rotatePanLabel.setFont(Kanit);
  rotatePanLabel.setJustificationType(juce::Justification::centred);
  angleSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox,
                              false, 0, 0);
  angleSlider.setLookAndFeel(&sliderLookAndFeel);

  angleSlider_TextBox.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
  angleSlider_TextBox.setSliderSnapsToMousePosition(false);
  angleSlider_TextBox.setColour(juce::Slider::trackColourId,
                                juce::Colours::transparentBlack);
  angleSlider_TextBox.setSize(70, 30);
}

void CyclingPannerAudioProcessorEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  rotatePanLabel.setBounds(125, 25, 550, 60);
  angleSlider.setBounds(125, 75, 550, 60);
  angleSlider_TextBox.setBounds(400 - 35, 145, 80, 60);
  rotatePanBypass.setBounds(125, 400, 550, 60);

  splitPanAngleDiffSlider.setBounds(125, 200, 550, 60);
  splitPanCenterPosSlider.setBounds(125, 300, 550, 60);
}
