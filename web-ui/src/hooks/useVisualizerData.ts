import { useState, useEffect } from 'react';
import { addEventListener, removeEventListener, isInJuceWebView } from '../lib/juce-bridge';

export interface VisualizerData {
  rms: number;
  peak: number;
  wobblePhase: number;
  crackleActivity: number;
  mode: number;
  bypassed: boolean;
  degradation: number;
}

const defaultData: VisualizerData = {
  rms: 0,
  peak: 0,
  wobblePhase: 0,
  crackleActivity: 0,
  mode: 0,
  bypassed: false,
  degradation: 0
};

export function useVisualizerData(): VisualizerData {
  const [data, setData] = useState<VisualizerData>(defaultData);

  useEffect(() => {
    if (!isInJuceWebView()) {
      // Demo mode for development
      const interval = setInterval(() => {
        setData({
          rms: 0.3 + Math.random() * 0.3,
          peak: 0.5 + Math.random() * 0.4,
          wobblePhase: (Date.now() / 2000) % 1,
          crackleActivity: Math.random() > 0.95 ? Math.random() : 0,
          mode: 0,
          bypassed: false,
          degradation: 0.5
        });
      }, 16);
      return () => clearInterval(interval);
    }

    const handleData = (event: any) => {
      setData({
        rms: event.rms ?? 0,
        peak: event.peak ?? 0,
        wobblePhase: event.wobblePhase ?? 0,
        crackleActivity: event.crackleActivity ?? 0,
        mode: event.mode ?? 0,
        bypassed: event.bypassed ?? false,
        degradation: event.degradation ?? 0
      });
    };

    addEventListener('visualizerData', handleData);
    return () => removeEventListener('visualizerData', handleData);
  }, []);

  return data;
}
