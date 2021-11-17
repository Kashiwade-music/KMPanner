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
      rotatePanBypass() {
  // --------------------------------------------------------------
  addAndMakeVisible(rotatePanLabel);
  rotatePanLabel.setFont(Kanit);
  rotatePanLabel.setJustificationType(juce::Justification::centred);
  angleSlider.setDoubleClickReturnValue(true, 0);
  angleSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox,
                              false, 0, 0);
  angleSlider.setLookAndFeel(&sliderLookAndFeel);
  addAndMakeVisible(angleSlider);
  angleSlider_TextBox.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
  angleSlider_TextBox.setSliderSnapsToMousePosition(false);
  angleSlider_TextBox.setColour(juce::Slider::trackColourId,
                                juce::Colours::transparentBlack);
  angleSlider_TextBox.setSize(70, 30);
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
  angleAttachment.reset(
      new SliderAttachment(valueTreeState, "angle", angleSlider));
  angleAttachment_TextBox.reset(
      new SliderAttachment(valueTreeState, "angle", angleSlider_TextBox));
  rotatePanBypassAttachment.reset(
      new ButtonAttachment(valueTreeState, "isrotatebypass", rotatePanBypass));

  // --------------------------------------------------------------

  panLabel.setFont(Kanit);
  panLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(panLabel);
  panSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox,
                            false, 0, 0);
  panSlider.setLookAndFeel(&sliderLookAndFeel);
  panSlider.setDoubleClickReturnValue(true, 0);
  addAndMakeVisible(panSlider);
  panSlider_TextBox.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
  panSlider_TextBox.setSliderSnapsToMousePosition(false);
  panSlider_TextBox.setColour(juce::Slider::trackColourId,
                              juce::Colours::transparentBlack);
  panSlider_TextBox.setSize(70, 30);
  addAndMakeVisible(panSlider_TextBox);
  panBypass.setImages(
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
  panBypass.setClickingTogglesState(true);
  addAndMakeVisible(panBypass);
  panRule.setJustificationType(juce::Justification::Justification::centred);
  panRule.setScrollWheelEnabled(true);
  panRule.addItemList(
      juce::StringArray("linear", "balanced", "sin3dB", "sin4.5dB", "sin6dB",
                        "sqrt3dB", "sqrt4.5dB"),
      1);
  comboBoxColourScheme.setUIColour(
      juce::LookAndFeel_V4::ColourScheme::UIColour::menuBackground,
      juce::Colour::Colour(35, 35, 46));
  comboBoxColourScheme.setUIColour(
      juce::LookAndFeel_V4::ColourScheme::UIColour::highlightedFill,
      juce::Colour::Colour(23, 23, 32));
  comboBoxLookAndFeel.setColourScheme(comboBoxColourScheme);
  panRule.setLookAndFeel(&comboBoxLookAndFeel);
  addAndMakeVisible(panRule);

  panAttachment.reset(
      new SliderAttachment(valueTreeState, "panangle", panSlider));
  panAttachment_TextBox.reset(
      new SliderAttachment(valueTreeState, "panangle", panSlider_TextBox));
  panBypassAttachment.reset(
      new ButtonAttachment(valueTreeState, "ispanbypass", panBypass));
  panRuleAttachment.reset(
      new ComboBoxAttachment(valueTreeState, "panrule", panRule));

  // --------------------------------------------------------------

  splitPanLabel.setFont(Kanit);
  splitPanLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(splitPanLabel);

  splitPanAngleDiffSlider.setDoubleClickReturnValue(true, 0);
  splitPanAngleDiffSlider.setTextBoxStyle(
      juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
  splitPanAngleDiffSlider.setLookAndFeel(&sliderLookAndFeel);
  addAndMakeVisible(splitPanAngleDiffSlider);

  splitPanAngleDiffSlider_TextBox.setSliderStyle(
      juce::Slider::SliderStyle::LinearBar);
  splitPanAngleDiffSlider_TextBox.setSliderSnapsToMousePosition(false);
  splitPanAngleDiffSlider_TextBox.setColour(juce::Slider::trackColourId,
                                            juce::Colours::transparentBlack);
  splitPanAngleDiffSlider_TextBox.setSize(70, 30);
  addAndMakeVisible(splitPanAngleDiffSlider_TextBox);

  splitPanCenterPosSlider.setDoubleClickReturnValue(true, 0);
  splitPanCenterPosSlider.setTextBoxStyle(
      juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
  splitPanCenterPosSlider.setLookAndFeel(&sliderLookAndFeel);
  addAndMakeVisible(splitPanCenterPosSlider);

  splitPanCenterPosSlider_TextBox.setSliderStyle(
      juce::Slider::SliderStyle::LinearBar);
  splitPanCenterPosSlider_TextBox.setSliderSnapsToMousePosition(false);
  splitPanCenterPosSlider_TextBox.setColour(juce::Slider::trackColourId,
                                            juce::Colours::transparentBlack);
  splitPanCenterPosSlider_TextBox.setSize(70, 30);
  addAndMakeVisible(splitPanCenterPosSlider_TextBox);

  splitPanSlider_L.setTextBoxStyle(
      juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
  addAndMakeVisible(splitPanSlider_L);

  splitPanSlider_R.setTextBoxStyle(
      juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
  addAndMakeVisible(splitPanSlider_R);

  splitPanRule.setJustificationType(
      juce::Justification::Justification::centred);
  splitPanRule.setScrollWheelEnabled(true);
  splitPanRule.addItemList(
      juce::StringArray("linear", "balanced", "sin3dB", "sin4.5dB", "sin6dB",
                        "sqrt3dB", "sqrt4.5dB"),
      1);
  splitPanRule.setLookAndFeel(&comboBoxLookAndFeel);
  addAndMakeVisible(splitPanRule);

  splitPanBypass.setImages(
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
  splitPanBypass.setClickingTogglesState(true);
  addAndMakeVisible(splitPanBypass);

  splitPanAngleDiffAttachment.reset(new SliderAttachment(
      valueTreeState, "splitpananglediff", splitPanAngleDiffSlider));
  splitPanAngleDiffAttachment_TextBox.reset(new SliderAttachment(
      valueTreeState, "splitpananglediff", splitPanAngleDiffSlider_TextBox));
  splitPanCenterPosAttachment.reset(new SliderAttachment(
      valueTreeState, "splitpancenterpos", splitPanCenterPosSlider));
  splitPanCenterPosAttachment_TextBox.reset(new SliderAttachment(
      valueTreeState, "splitpancenterpos", splitPanCenterPosSlider_TextBox));
  splitPanSliderAttachment_L.reset(
      new SliderAttachment(valueTreeState, "splitpanlangle", splitPanSlider_L));
  splitPanSliderAttachment_R.reset(
      new SliderAttachment(valueTreeState, "splitpanrangle", splitPanSlider_R));
  splitPanBypassAttachment.reset(
      new ButtonAttachment(valueTreeState, "issplitpanbypass", splitPanBypass));
  splitPanRuleAttachment.reset(
      new ComboBoxAttachment(valueTreeState, "splitpanrule", splitPanRule));

  // --------------------------------------------------------------

  setSize(800, 800);
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

void ComboBoxLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height,
                                       bool, int, int, int, int,
                                       juce::ComboBox& box) {
  juce::Rectangle<int> boxBounds(0, 0, width, height);
  g.setColour(juce::Colours::transparentBlack);
  g.fillRect(boxBounds.toFloat());

  g.setColour(box.findColour(juce::ComboBox::outlineColourId));
  g.drawRect(boxBounds.toFloat().reduced(0.5f, 0.5f), 1.0f);

  juce::Rectangle<int> arrowZone(width - 15, 0, 10, height);
  juce::Path path;
  path.startNewSubPath((float)arrowZone.getX() + 2.0f,
                       (float)arrowZone.getCentreY() - 1.0f);
  path.lineTo((float)arrowZone.getCentreX(),
              (float)arrowZone.getCentreY() + 2.0f);
  path.lineTo((float)arrowZone.getRight() - 2.0f,
              (float)arrowZone.getCentreY() - 1.0f);

  g.setColour(box.findColour(juce::ComboBox::arrowColourId)
                  .withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
  g.strokePath(path, juce::PathStrokeType(2.0f));
}

void ComboBoxLookAndFeel::positionComboBoxText(juce::ComboBox& box,
                                               juce::Label& label) {
  label.setBounds(1, 1, box.getWidth() - 15, box.getHeight() - 2);

  label.setFont(getComboBoxFont(box));
}
//==============================================================================
void CyclingPannerAudioProcessorEditor::paint(juce::Graphics& g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(juce::Colour::Colour(23, 23, 32));
  g.setColour(juce::Colour::Colour(35, 35, 46));
  g.fillRoundedRectangle(50.f, 30.f, 700.f, 160.f, 10.f);
  g.fillRoundedRectangle(50.f, 220.f, 700.f, 160.f, 10.f);
  g.fillRoundedRectangle(50.f, 410.f, 700.f, 220.f, 10.f);
}

void CyclingPannerAudioProcessorEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  rotatePanLabel.setBounds(125, 25, 550, 60);
  angleSlider.setBounds(125, 75, 550, 60);
  angleSlider_TextBox.setBounds(400 - 35, 145, 70, 30);
  rotatePanBypass.setBounds(81, 92, 25, 25);

  panLabel.setBounds(125, 215, 550, 60);
  panSlider.setBounds(125, 265, 550, 60);
  panSlider_TextBox.setBounds(400 - 35, 335, 70, 30);
  panBypass.setBounds(81, 282, 25, 25);
  panRule.setBounds(672, 282, 70, 25);

  splitPanLabel.setBounds(125, 405, 550, 60);
  splitPanAngleDiffSlider.setBounds(125, 455, 550, 60);
  splitPanAngleDiffSlider_TextBox.setBounds(400 - 35, 525, 70, 30);
  splitPanBypass.setBounds(81, 472, 25, 25);
  splitPanRule.setBounds(672, 472, 70, 25);
  splitPanCenterPosSlider.setBounds(125, 550, 550, 60);
}
