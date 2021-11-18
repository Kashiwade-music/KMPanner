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
  void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, float minSliderPos, float maxSliderPos,
                        const juce::Slider::SliderStyle style,
                        juce::Slider&) override;
};

class CenterPosSliderLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, float minSliderPos, float maxSliderPos,
                        const juce::Slider::SliderStyle style,
                        juce::Slider&) override;
};

class SplitLSliderLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, float minSliderPos, float maxSliderPos,
                        const juce::Slider::SliderStyle style,
                        juce::Slider&) override;
};

class SplitRSliderLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, float minSliderPos, float maxSliderPos,
                        const juce::Slider::SliderStyle style,
                        juce::Slider&) override;
};

class Plus_Mask : public juce::LookAndFeel_V4 {
 public:
  void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, float minSliderPos, float maxSliderPos,
                        const juce::Slider::SliderStyle style,
                        juce::Slider&) override;
};

class Minus_Mask : public juce::LookAndFeel_V4 {
 public:
  void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, float minSliderPos, float maxSliderPos,
                        const juce::Slider::SliderStyle style,
                        juce::Slider&) override;
};

class CyclingPannerAudioProcessorEditor;
class CPSListener : public juce::Slider::Listener {
 public:
  CPSListener(CyclingPannerAudioProcessorEditor& p) : editor(p){};
  void sliderValueChanged(juce::Slider* slider) override;

 private:
  CyclingPannerAudioProcessorEditor& editor;
};

class ComboBoxLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  void drawComboBox(juce::Graphics& g, int width, int height, bool, int, int,
                    int, int, juce::ComboBox& box) override;
  void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override;
};

class CyclingPannerAudioProcessorEditor : public juce::AudioProcessorEditor {
 public:
  CyclingPannerAudioProcessorEditor(CyclingPannerAudioProcessor&,
                                    juce::AudioProcessorValueTreeState& vts);
  ~CyclingPannerAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics&) override;
  void resized() override;

  juce::Slider splitPanSlider_L_Mask;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      splitPanSliderAttachment_L_Mask;
  juce::Slider splitPanSlider_R_Mask;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      splitPanSliderAttachment_R_Mask;
  CPSListener CPSListener{*this};
  CyclingPannerAudioProcessor& audioProcessor;
  Plus_Mask plus_Mask;
  Minus_Mask minus_Mask;

 private:
  juce::Font Kanit{"Kanit", "ExtraLight", 30.f};
  juce::Font KanitMini{"Kanit", "ExtraLight", 25.f};
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.

  SliderLookAndFeel sliderLookAndFeel;
  CenterPosSliderLookAndFeel centerPosSliderLookAndFeel;
  SplitLSliderLookAndFeel splitLSliderLookAndFeel;
  SplitRSliderLookAndFeel splitRSliderLookAndFeel;

  ComboBoxLookAndFeel comboBoxLookAndFeel;
  juce::LookAndFeel_V4::ColourScheme comboBoxColourScheme{
      juce::LookAndFeel_V4::getDarkColourScheme()};

  typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
  typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
  typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment
      ComboBoxAttachment;
  juce::AudioProcessorValueTreeState& valueTreeState;

  juce::Rectangle<int> rotatePanAria{50, 30, 700, 160};
  juce::Label rotatePanLabel{"rotatePan", "Rotate Panner"};
  juce::Slider angleSlider;
  std::unique_ptr<SliderAttachment> angleAttachment;
  juce::Slider angleSlider_TextBox;
  std::unique_ptr<SliderAttachment> angleAttachment_TextBox;
  juce::ImageButton rotatePanBypass;
  std::unique_ptr<ButtonAttachment> rotatePanBypassAttachment;

  juce::Rectangle<int> panAria{50, 220, 700, 160};
  juce::Label panLabel{"pan", "Balance Panner"};
  juce::Slider panSlider;
  std::unique_ptr<SliderAttachment> panAttachment;
  juce::Slider panSlider_TextBox;
  std::unique_ptr<SliderAttachment> panAttachment_TextBox;
  juce::ComboBox panRule;
  std::unique_ptr<ComboBoxAttachment> panRuleAttachment;
  juce::ImageButton panBypass;
  std::unique_ptr<ButtonAttachment> panBypassAttachment;

  juce::Rectangle<int> splitPanAria{50, 410, 700, 350};
  juce::Rectangle<int> diffAria{50, 0, 700, 160};
  juce::Rectangle<int> centerAria{50, 0, 700, 160};
  juce::Label splitPanLabel{"pan", "Split Panner"};
  juce::Label splitPanDiffLabel{"pan", "R-L Diff"};
  juce::Label splitPanCenterLabel{"pan", "Center Position"};
  juce::Slider splitPanAngleDiffSlider;
  std::unique_ptr<SliderAttachment> splitPanAngleDiffAttachment;
  juce::Slider splitPanAngleDiffSlider_TextBox;
  std::unique_ptr<SliderAttachment> splitPanAngleDiffAttachment_TextBox;
  juce::Slider splitPanCenterPosSlider;
  std::unique_ptr<SliderAttachment> splitPanCenterPosAttachment;
  juce::Slider splitPanCenterPosSlider_TextBox;
  std::unique_ptr<SliderAttachment> splitPanCenterPosAttachment_TextBox;
  juce::Slider splitPanSlider_L;
  std::unique_ptr<SliderAttachment> splitPanSliderAttachment_L;
  juce::Slider splitPanSlider_R;
  std::unique_ptr<SliderAttachment> splitPanSliderAttachment_R;
  juce::ComboBox splitPanRule;
  std::unique_ptr<ComboBoxAttachment> splitPanRuleAttachment;
  juce::ImageButton splitPanBypass;
  std::unique_ptr<ButtonAttachment> splitPanBypassAttachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
      CyclingPannerAudioProcessorEditor)
};
