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

  splitPanDiffLabel.setFont(KanitMini);
  splitPanDiffLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(splitPanDiffLabel);

  splitPanCenterLabel.setFont(KanitMini);
  splitPanCenterLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(splitPanCenterLabel);

  splitPanAngleDiffSlider.addListener(&CPSListener);
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

  splitPanCenterPosSlider_TextBox.setSliderStyle(
      juce::Slider::SliderStyle::LinearBar);
  splitPanCenterPosSlider_TextBox.setSliderSnapsToMousePosition(false);
  splitPanCenterPosSlider_TextBox.setColour(juce::Slider::trackColourId,
                                            juce::Colours::transparentBlack);
  splitPanCenterPosSlider_TextBox.setSize(70, 30);
  addAndMakeVisible(splitPanCenterPosSlider_TextBox);

  splitPanSlider_L.setTextBoxStyle(
      juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
  splitPanSlider_L.setLookAndFeel(&splitLSliderLookAndFeel);
  addAndMakeVisible(splitPanSlider_L);

  splitPanSlider_R.setTextBoxStyle(
      juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
  splitPanSlider_R.setLookAndFeel(&splitRSliderLookAndFeel);
  addAndMakeVisible(splitPanSlider_R);

  splitPanSlider_L_Mask.setTextBoxStyle(
      juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
  splitPanSlider_L_Mask.setLookAndFeel(&plus_Mask);
  addAndMakeVisible(splitPanSlider_L_Mask);

  splitPanSlider_R_Mask.setTextBoxStyle(
      juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
  splitPanSlider_R_Mask.setLookAndFeel(&minus_Mask);
  addAndMakeVisible(splitPanSlider_R_Mask);

  splitPanCenterPosSlider.setDoubleClickReturnValue(true, 0);
  splitPanCenterPosSlider.setTextBoxStyle(
      juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
  splitPanCenterPosSlider.setLookAndFeel(&centerPosSliderLookAndFeel);
  addAndMakeVisible(splitPanCenterPosSlider);

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
  splitPanSliderAttachment_L_Mask.reset(new SliderAttachment(
      valueTreeState, "splitpanlangle", splitPanSlider_L_Mask));
  splitPanSliderAttachment_R_Mask.reset(new SliderAttachment(
      valueTreeState, "splitpanrangle", splitPanSlider_R_Mask));
  splitPanBypassAttachment.reset(
      new ButtonAttachment(valueTreeState, "issplitpanbypass", splitPanBypass));
  splitPanRuleAttachment.reset(
      new ComboBoxAttachment(valueTreeState, "splitpanrule", splitPanRule));

  // --------------------------------------------------------------

  setSize(800, 790);
}

CyclingPannerAudioProcessorEditor::~CyclingPannerAudioProcessorEditor() {}
//==============================================================================

void CPSListener::sliderValueChanged(juce::Slider* slider) {
  if (0 <= *editor.audioProcessor.splitPanAngleDiff) {
    editor.splitPanSlider_L_Mask.setLookAndFeel(&editor.plus_Mask);
    editor.splitPanSlider_R_Mask.setLookAndFeel(&editor.minus_Mask);
  } else {
    editor.splitPanSlider_L_Mask.setLookAndFeel(&editor.minus_Mask);
    editor.splitPanSlider_R_Mask.setLookAndFeel(&editor.plus_Mask);
  }
}

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

void CenterPosSliderLookAndFeel::drawLinearSlider(
    juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider&) {
  g.setColour(juce::Colour::Colour(142, 149, 166));
  g.fillRect((float)sliderPos - 6.f, (float)height / 2 + y - 25.f, 12.f,
             50.f);  // thumb
  g.setColour(juce::Colour::Colour(234, 235, 238));
  g.fillRect((float)sliderPos - 6.f + 3.f, (float)height / 2 + y - 25.f + 3.f,
             6.f,
             44.f);  // thumb
}

void SplitLSliderLookAndFeel::drawLinearSlider(
    juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider&) {
  g.setColour(juce::Colour::Colour(6, 5, 20));
  g.fillRect((float)x, (float)height / 2 + y - 7.f, (float)width,
             14.f);                      // background
  if ((sliderPos - x) >= (width / 2)) {  // プラス方向
    g.setColour(juce::Colour::Colour(225, 129, 12));
    g.fillRect((float)width / 2 + x, (float)height / 2 + y - 7.f,
               (float)sliderPos - x - (width / 2),
               14.f);  // fill
  } else {
    g.setColour(juce::Colour::Colour(12, 134, 225));
    g.fillRect((float)sliderPos, (float)height / 2 + y - 7.f,
               (float)((width / 2 + x) - sliderPos), 14.f);
  }
}

void SplitRSliderLookAndFeel::drawLinearSlider(
    juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider&) {
  if ((sliderPos - x) >= (width / 2)) {  // プラス方向
    g.setColour(juce::Colour::Colour(12, 134, 225));
    g.fillRect((float)width / 2 + x, (float)height / 2 + y - 7.f,
               (float)sliderPos - x - (width / 2),
               14.f);  // fill
  } else {
    g.setColour(juce::Colour::Colour(225, 129, 12));
    g.fillRect((float)sliderPos, (float)height / 2 + y - 7.f,
               (float)((width / 2 + x) - sliderPos), 14.f);
  }
}

void Plus_Mask::drawLinearSlider(juce::Graphics& g, int x, int y, int width,
                                 int height, float sliderPos,
                                 float minSliderPos, float maxSliderPos,
                                 const juce::Slider::SliderStyle style,
                                 juce::Slider&) {
  g.setColour(juce::Colour::Colour(6, 5, 20));
  if ((sliderPos - x) >= (width / 2)) {  // プラス方向
    g.fillRect((float)width / 2 + x, (float)height / 2 + y - 7.f,
               (float)sliderPos - x - (width / 2),
               14.f);  // fill
  }
}

void Minus_Mask::drawLinearSlider(juce::Graphics& g, int x, int y, int width,
                                  int height, float sliderPos,
                                  float minSliderPos, float maxSliderPos,
                                  const juce::Slider::SliderStyle style,
                                  juce::Slider&) {
  g.setColour(juce::Colour::Colour(6, 5, 20));
  if (!((sliderPos - x) >= (width / 2))) {
    g.fillRect((float)sliderPos, (float)height / 2 + y - 7.f,
               (float)((width / 2 + x) - sliderPos), 14.f);
  }
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
  g.fillRoundedRectangle(rotatePanAria.toFloat(), 10.f);
  g.fillRoundedRectangle(panAria.toFloat(), 10.f);
  g.fillRoundedRectangle(splitPanAria.toFloat(), 10.f);
}

void CyclingPannerAudioProcessorEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  rotatePanLabel.setSize(rotatePanAria.getWidth(), 50);
  rotatePanLabel.setCentrePosition(rotatePanAria.getCentreX(),
                                   rotatePanAria.getY() + 20);
  angleSlider.setSize(550, 60);
  angleSlider.setCentrePosition(rotatePanAria.getCentreX(),
                                rotatePanAria.getCentreY());
  angleSlider_TextBox.setSize(70, 20);
  angleSlider_TextBox.setCentrePosition(rotatePanAria.getCentreX(),
                                        rotatePanAria.getBottom() - 22);
  rotatePanBypass.setSize(25, 25);
  rotatePanBypass.setCentrePosition(93, rotatePanAria.getCentreY() - 2);

  // --------------------------------------------------------------

  panLabel.setSize(panAria.getWidth(), 50);
  panLabel.setCentrePosition(panAria.getCentreX(), panAria.getY() + 20);
  panSlider.setSize(550, 60);
  panSlider.setCentrePosition(panAria.getCentreX(), panAria.getCentreY());
  panSlider_TextBox.setSize(70, 20);
  panSlider_TextBox.setCentrePosition(panAria.getCentreX(),
                                      panAria.getBottom() - 22);
  panBypass.setSize(25, 25);
  panBypass.setCentrePosition(93, panAria.getCentreY() - 2);
  panRule.setSize(70, 25);
  panRule.setCentrePosition(panAria.getRight() - 44, panAria.getCentreY());

  // --------------------------------------------------------------

  splitPanLabel.setSize(splitPanAria.getWidth(), 50);
  splitPanLabel.setCentrePosition(splitPanAria.getCentreX(),
                                  splitPanAria.getY() + 20);
  splitPanBypass.setSize(25, 25);
  splitPanBypass.setCentrePosition(93, splitPanAria.getCentreY() - 2);
  splitPanRule.setSize(70, 25);
  splitPanRule.setCentrePosition(splitPanAria.getRight() - 44,
                                 splitPanAria.getCentreY());

  diffAria.setY(splitPanAria.getY() + 25);
  splitPanDiffLabel.setSize(diffAria.getWidth(), 50);
  splitPanDiffLabel.setCentrePosition(diffAria.getCentreX(),
                                      diffAria.getY() + 30);
  splitPanAngleDiffSlider.setSize(550, 60);
  splitPanAngleDiffSlider.setCentrePosition(diffAria.getCentreX(),
                                            diffAria.getCentreY());
  splitPanAngleDiffSlider_TextBox.setSize(70, 20);
  splitPanAngleDiffSlider_TextBox.setCentrePosition(diffAria.getCentreX(),
                                                    diffAria.getBottom() - 28);

  centerAria.setY(splitPanAria.getBottom() - 10 - 150);
  splitPanCenterLabel.setSize(centerAria.getWidth(), 50);
  splitPanCenterLabel.setCentrePosition(centerAria.getCentreX(),
                                        centerAria.getY() + 30);
  splitPanSlider_L.setSize(550, 60);
  splitPanSlider_L.setCentrePosition(centerAria.getCentreX(),
                                     centerAria.getCentreY());
  splitPanSlider_R.setSize(550, 60);
  splitPanSlider_R.setCentrePosition(centerAria.getCentreX(),
                                     centerAria.getCentreY());
  splitPanSlider_L_Mask.setSize(550, 60);
  splitPanSlider_L_Mask.setCentrePosition(centerAria.getCentreX(),
                                          centerAria.getCentreY());
  splitPanSlider_R_Mask.setSize(550, 60);
  splitPanSlider_R_Mask.setCentrePosition(centerAria.getCentreX(),
                                          centerAria.getCentreY());
  splitPanCenterPosSlider.setSize(550, 60);
  splitPanCenterPosSlider.setCentrePosition(centerAria.getCentreX(),
                                            centerAria.getCentreY());
  splitPanCenterPosSlider_TextBox.setSize(70, 20);
  splitPanCenterPosSlider_TextBox.setCentrePosition(
      centerAria.getCentreX(), centerAria.getBottom() - 28);
}
