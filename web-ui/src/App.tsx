import React, { useState, useCallback } from 'react';
import { Knob } from './components/Knob';
import { Slider } from './components/Slider';
import { ModeSelector } from './components/ModeSelector';
import { OxideVisualizer } from './components/OxideVisualizer';
import { PresetSelector } from './components/PresetSelector';
import { ActivationScreen } from './components/ActivationScreen';
import { useSliderParam, useToggleParam, useChoiceParam } from './hooks/useJuceParam';
import { useVisualizerData } from './hooks/useVisualizerData';

function PluginUI() {
  // Parameters
  const bitcrush = useSliderParam('bitcrush', 0);
  const downsample = useSliderParam('downsample', 0);
  const noise = useSliderParam('noise', 15);
  const crackle = useSliderParam('crackle', 0);
  const wobble = useSliderParam('wobble', 20);
  const dropout = useSliderParam('dropout', 0);
  const saturation = useSliderParam('saturation', 30);
  const age = useSliderParam('age', 25);
  const filterCutoff = useSliderParam('filterCutoff', 80);
  const filterRes = useSliderParam('filterRes', 0);
  const filterDrive = useSliderParam('filterDrive', 0);
  const mode = useChoiceParam('mode', 4, 0);
  const mix = useSliderParam('mix', 100);
  const output = useSliderParam('output', 0);
  const bypass = useToggleParam('bypass', false);

  const visualizerData = useVisualizerData();

  // Mode colors
  const modeColors = ['#ff6b35', '#8b5cf6', '#06b6d4', '#22c55e'];
  const currentColor = modeColors[mode.value] || modeColors[0];

  return (
    <div
      className="plugin-container"
      style={{ '--accent-color': currentColor } as React.CSSProperties}
    >
      {/* Background gradient */}
      <div className="background-gradient" />

      {/* Noise texture overlay */}
      <div className="noise-overlay" style={{ opacity: age.value / 500 }} />

      {/* Header */}
      <header className="plugin-header">
        <PresetSelector />
        <h1
          className={`plugin-title ${bypass.value ? 'bypassed' : ''}`}
          onClick={bypass.toggle}
        >
          OXIDE
        </h1>
        <div className="header-spacer" />
      </header>

      {/* Main content */}
      <main className="plugin-main">
        {/* Left Panel - Degradation */}
        <section className="panel degradation-panel">
          <h2 className="panel-title">Degradation</h2>
          <div className="knob-grid">
            <Knob
              value={bitcrush.value}
              label="BITCRUSH"
              color={currentColor}
              onChange={bitcrush.setValue}
              onDragStart={bitcrush.dragStart}
              onDragEnd={bitcrush.dragEnd}
            />
            <Knob
              value={downsample.value}
              label="DOWNSAMPLE"
              color={currentColor}
              onChange={downsample.setValue}
              onDragStart={downsample.dragStart}
              onDragEnd={downsample.dragEnd}
            />
            <Knob
              value={noise.value}
              label="NOISE"
              color={currentColor}
              onChange={noise.setValue}
              onDragStart={noise.dragStart}
              onDragEnd={noise.dragEnd}
            />
            <Knob
              value={crackle.value}
              label="CRACKLE"
              color={currentColor}
              onChange={crackle.setValue}
              onDragStart={crackle.dragStart}
              onDragEnd={crackle.dragEnd}
            />
          </div>
        </section>

        {/* Center - Visualizer */}
        <section className="visualizer-section">
          <OxideVisualizer
            mode={mode.value}
            degradation={visualizerData.degradation}
          />
          <ModeSelector
            value={mode.value}
            onChange={mode.setChoice}
          />
        </section>

        {/* Right Panel - Character */}
        <section className="panel character-panel">
          <h2 className="panel-title">Character</h2>
          <div className="knob-grid">
            <Knob
              value={wobble.value}
              label="WOBBLE"
              color={currentColor}
              onChange={wobble.setValue}
              onDragStart={wobble.dragStart}
              onDragEnd={wobble.dragEnd}
            />
            <Knob
              value={dropout.value}
              label="DROPOUT"
              color={currentColor}
              onChange={dropout.setValue}
              onDragStart={dropout.dragStart}
              onDragEnd={dropout.dragEnd}
            />
            <Knob
              value={saturation.value}
              label="SATURATION"
              color={currentColor}
              onChange={saturation.setValue}
              onDragStart={saturation.dragStart}
              onDragEnd={saturation.dragEnd}
            />
            <Knob
              value={age.value}
              label="AGE"
              color={currentColor}
              size="lg"
              onChange={age.setValue}
              onDragStart={age.dragStart}
              onDragEnd={age.dragEnd}
            />
          </div>
        </section>
      </main>

      {/* Footer - Filter & Output */}
      <footer className="plugin-footer">
        <div className="footer-section filter-section">
          <h3 className="footer-title">Filter</h3>
          <div className="slider-group">
            <Slider
              value={filterCutoff.value}
              label="CUTOFF"
              color={currentColor}
              onChange={filterCutoff.setValue}
              onDragStart={filterCutoff.dragStart}
              onDragEnd={filterCutoff.dragEnd}
            />
            <Slider
              value={filterRes.value}
              label="RESONANCE"
              color={currentColor}
              onChange={filterRes.setValue}
              onDragStart={filterRes.dragStart}
              onDragEnd={filterRes.dragEnd}
            />
            <Slider
              value={filterDrive.value}
              label="DRIVE"
              color={currentColor}
              onChange={filterDrive.setValue}
              onDragStart={filterDrive.dragStart}
              onDragEnd={filterDrive.dragEnd}
            />
          </div>
        </div>

        <div className="footer-divider" />

        <div className="footer-section output-section">
          <h3 className="footer-title">Output</h3>
          <div className="output-knobs">
            <Knob
              value={mix.value}
              label="MIX"
              color={currentColor}
              onChange={mix.setValue}
              onDragStart={mix.dragStart}
              onDragEnd={mix.dragEnd}
            />
            <Knob
              value={output.value}
              min={-24}
              max={12}
              label="OUTPUT"
              unit="dB"
              color={currentColor}
              bipolar
              onChange={output.setValue}
              onDragStart={output.dragStart}
              onDragEnd={output.dragEnd}
            />
          </div>
        </div>
      </footer>
    </div>
  );
}

function App() {
  const [isActivated, setIsActivated] = useState(false);

  const handleActivated = useCallback(() => {
    setIsActivated(true);
  }, []);

  if (!isActivated) {
    return <ActivationScreen onActivated={handleActivated} />;
  }

  return <PluginUI />;
}

export default App;
