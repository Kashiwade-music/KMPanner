/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"

//==============================================================================
/**
 */
class SliderLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  SliderLookAndFeel() {
    setColour(juce::Slider::thumbColourId, juce::Colours::red);
  }

  void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, float minSliderPos, float maxSliderPos,
                        const juce::Slider::SliderStyle style,
                        juce::Slider&) override;
};

class CyclingPannerAudioProcessorEditor : public juce::AudioProcessorEditor {
 public:
  CyclingPannerAudioProcessorEditor(CyclingPannerAudioProcessor&,
                                    juce::AudioProcessorValueTreeState& vts);
  ~CyclingPannerAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics&) override;
  void resized() override;

 private:
  juce::Font Kanit;
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  CyclingPannerAudioProcessor& audioProcessor;
  SliderLookAndFeel sliderLookAndFeel;

  typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
  typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
  juce::AudioProcessorValueTreeState& valueTreeState;

  juce::Label rotatePanLabel;
  juce::Slider angleSlider;
  std::unique_ptr<SliderAttachment> angleAttachment;
  juce::Slider angleSlider_TextBox;
  std::unique_ptr<SliderAttachment> angleAttachment_TextBox;
  juce::ImageButton rotatePanBypass;
  std::unique_ptr<ButtonAttachment> rotatePanBypassAttachment;

  juce::Slider splitPanAngleDiffSlider;
  std::unique_ptr<SliderAttachment> splitPanAngleDiffAttachment;
  juce::Slider splitPanCenterPosSlider;
  std::unique_ptr<SliderAttachment> splitPanCenterPosAttachment;
  juce::Label splitPanLAngle;
  juce::Label splitPanRAngle;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
      CyclingPannerAudioProcessorEditor)
};
