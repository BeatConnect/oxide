import React, { useRef, useEffect } from 'react';
import { useVisualizerData } from '../hooks/useVisualizerData';

interface OxideVisualizerProps {
  mode: number;
  degradation: number;
}

const modeColors = [
  { primary: '#ff6b35', secondary: '#ff9f1c' },  // Cassette - warm orange
  { primary: '#8b5cf6', secondary: '#a78bfa' },  // Vinyl - purple
  { primary: '#06b6d4', secondary: '#22d3ee' },  // VHS - cyan
  { primary: '#22c55e', secondary: '#4ade80' }   // Radio - green
];

export function OxideVisualizer({ mode, degradation }: OxideVisualizerProps) {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const visualizerData = useVisualizerData();
  const animationRef = useRef<number>();
  const timeRef = useRef(0);
  const noiseDataRef = useRef<ImageData | null>(null);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    const width = canvas.width;
    const height = canvas.height;

    // Pre-generate noise texture
    if (!noiseDataRef.current) {
      noiseDataRef.current = ctx.createImageData(width, height);
    }

    const colors = modeColors[mode] || modeColors[0];

    const animate = () => {
      timeRef.current += 0.016;
      const t = timeRef.current;

      // Clear with dark gradient
      const gradient = ctx.createRadialGradient(
        width / 2, height / 2, 0,
        width / 2, height / 2, width / 2
      );
      gradient.addColorStop(0, '#0f0f12');
      gradient.addColorStop(1, '#08080a');
      ctx.fillStyle = gradient;
      ctx.fillRect(0, 0, width, height);

      // Draw based on mode
      const rms = visualizerData.rms;
      const peak = visualizerData.peak;
      const wobble = visualizerData.wobblePhase;
      const crackle = visualizerData.crackleActivity;
      const deg = degradation;

      // Central waveform visualization
      ctx.save();
      ctx.translate(width / 2, height / 2);

      // Wobble effect on entire visualization
      const wobbleOffset = Math.sin(wobble * Math.PI * 2) * deg * 5;
      ctx.translate(wobbleOffset, 0);

      // Draw degraded waveform bars
      const numBars = 48;
      const barWidth = 4;
      const maxHeight = height * 0.35;

      for (let i = 0; i < numBars; i++) {
        const angle = (i / numBars) * Math.PI * 2 - Math.PI / 2;
        const x = Math.cos(angle);
        const y = Math.sin(angle);

        // Base height from audio
        let barHeight = maxHeight * (0.3 + rms * 0.7);

        // Add per-bar variation
        const variation = Math.sin(t * 2 + i * 0.3) * 0.2 + 0.8;
        barHeight *= variation;

        // Degradation effect - some bars are "damaged"
        if (deg > 0.3 && Math.sin(i * 7 + t * 0.5) > 1 - deg * 0.5) {
          barHeight *= 0.3 + Math.random() * 0.3;
        }

        // Calculate bar position
        const innerRadius = 50 + peak * 20;
        const outerRadius = innerRadius + barHeight;

        const x1 = x * innerRadius;
        const y1 = y * innerRadius;
        const x2 = x * outerRadius;
        const y2 = y * outerRadius;

        // Bar gradient
        const barGradient = ctx.createLinearGradient(x1, y1, x2, y2);
        barGradient.addColorStop(0, colors.secondary + '40');
        barGradient.addColorStop(0.5, colors.primary);
        barGradient.addColorStop(1, colors.primary + '80');

        ctx.beginPath();
        ctx.moveTo(x1, y1);
        ctx.lineTo(x2, y2);
        ctx.strokeStyle = barGradient;
        ctx.lineWidth = barWidth;
        ctx.lineCap = 'round';
        ctx.stroke();
      }

      // Center glow
      const glowGradient = ctx.createRadialGradient(0, 0, 0, 0, 0, 60 + rms * 30);
      glowGradient.addColorStop(0, colors.primary + '30');
      glowGradient.addColorStop(0.5, colors.primary + '10');
      glowGradient.addColorStop(1, 'transparent');
      ctx.fillStyle = glowGradient;
      ctx.fillRect(-100, -100, 200, 200);

      // Center circle
      ctx.beginPath();
      ctx.arc(0, 0, 40 + peak * 10, 0, Math.PI * 2);
      ctx.fillStyle = '#0a0a0c';
      ctx.fill();
      ctx.strokeStyle = colors.primary + '60';
      ctx.lineWidth = 2;
      ctx.stroke();

      // Inner ring
      ctx.beginPath();
      ctx.arc(0, 0, 30 + rms * 5, 0, Math.PI * 2);
      ctx.strokeStyle = colors.primary;
      ctx.lineWidth = 1.5;
      ctx.stroke();

      ctx.restore();

      // Scanlines effect (VHS mode emphasized)
      if (mode === 2 || deg > 0.5) {
        const scanlineIntensity = mode === 2 ? 0.08 : deg * 0.05;
        ctx.fillStyle = `rgba(0,0,0,${scanlineIntensity})`;
        for (let y = 0; y < height; y += 3) {
          ctx.fillRect(0, y, width, 1);
        }
      }

      // Static noise overlay
      if (deg > 0.2) {
        const noiseData = noiseDataRef.current!;
        const pixels = noiseData.data;
        const noiseIntensity = Math.floor(deg * 40);

        for (let i = 0; i < pixels.length; i += 4) {
          if (Math.random() > 0.97) {
            const noise = Math.random() * noiseIntensity;
            pixels[i] = pixels[i + 1] = pixels[i + 2] = noise;
            pixels[i + 3] = noise;
          } else {
            pixels[i + 3] = 0;
          }
        }
        ctx.putImageData(noiseData, 0, 0);
      }

      // Crackle flash effect
      if (crackle > 0.1) {
        ctx.fillStyle = `rgba(255,255,255,${crackle * 0.3})`;
        ctx.fillRect(0, 0, width, height);
      }

      // Vignette
      const vignette = ctx.createRadialGradient(
        width / 2, height / 2, width * 0.3,
        width / 2, height / 2, width * 0.7
      );
      vignette.addColorStop(0, 'transparent');
      vignette.addColorStop(1, 'rgba(0,0,0,0.5)');
      ctx.fillStyle = vignette;
      ctx.fillRect(0, 0, width, height);

      animationRef.current = requestAnimationFrame(animate);
    };

    animate();

    return () => {
      if (animationRef.current) {
        cancelAnimationFrame(animationRef.current);
      }
    };
  }, [mode, degradation, visualizerData]);

  return (
    <div className="oxide-visualizer">
      <canvas
        ref={canvasRef}
        width={280}
        height={280}
        style={{
          width: '100%',
          height: '100%',
          borderRadius: 12
        }}
      />

      <style>{`
        .oxide-visualizer {
          width: 280px;
          height: 280px;
          background: #0a0a0c;
          border-radius: 16px;
          border: 1px solid rgba(255,255,255,0.06);
          box-shadow:
            inset 0 2px 10px rgba(0,0,0,0.5),
            0 8px 32px rgba(0,0,0,0.3);
          overflow: hidden;
        }
      `}</style>
    </div>
  );
}
