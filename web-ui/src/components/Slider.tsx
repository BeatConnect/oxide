import React, { useRef, useCallback, useEffect, useState } from 'react';

interface SliderProps {
  value: number;
  min?: number;
  max?: number;
  label: string;
  unit?: string;
  color?: string;
  onChange: (value: number) => void;
  onDragStart?: () => void;
  onDragEnd?: () => void;
}

export function Slider({
  value,
  min = 0,
  max = 100,
  label,
  unit = '%',
  color = '#ff6b35',
  onChange,
  onDragStart,
  onDragEnd
}: SliderProps) {
  const sliderRef = useRef<HTMLDivElement>(null);
  const isDragging = useRef(false);
  const [isHovered, setIsHovered] = useState(false);

  const normalizedValue = (value - min) / (max - min);

  const handleMouseDown = useCallback((e: React.MouseEvent) => {
    isDragging.current = true;
    onDragStart?.();
    document.body.style.cursor = 'grabbing';

    // Immediately update value based on click position
    if (sliderRef.current) {
      const rect = sliderRef.current.getBoundingClientRect();
      const x = (e.clientX - rect.left) / rect.width;
      const newValue = min + x * (max - min);
      onChange(Math.max(min, Math.min(max, newValue)));
    }
  }, [min, max, onChange, onDragStart]);

  useEffect(() => {
    const handleMouseMove = (e: MouseEvent) => {
      if (!isDragging.current || !sliderRef.current) return;

      const rect = sliderRef.current.getBoundingClientRect();
      const x = (e.clientX - rect.left) / rect.width;
      const newValue = min + x * (max - min);
      onChange(Math.max(min, Math.min(max, newValue)));
    };

    const handleMouseUp = () => {
      if (isDragging.current) {
        isDragging.current = false;
        onDragEnd?.();
        document.body.style.cursor = '';
      }
    };

    document.addEventListener('mousemove', handleMouseMove);
    document.addEventListener('mouseup', handleMouseUp);
    return () => {
      document.removeEventListener('mousemove', handleMouseMove);
      document.removeEventListener('mouseup', handleMouseUp);
    };
  }, [min, max, onChange, onDragEnd]);

  const handleDoubleClick = () => {
    onChange(min);
  };

  const displayValue = Math.round(value);

  return (
    <div
      className="slider-container"
      onMouseEnter={() => setIsHovered(true)}
      onMouseLeave={() => setIsHovered(false)}
    >
      <div className="slider-header">
        <span className="slider-label">{label}</span>
        <span
          className="slider-value"
          style={{ color: isHovered ? color : 'rgba(255,255,255,0.6)' }}
        >
          {displayValue}{unit}
        </span>
      </div>

      <div
        ref={sliderRef}
        className="slider-track"
        onMouseDown={handleMouseDown}
        onDoubleClick={handleDoubleClick}
      >
        <div
          className="slider-fill"
          style={{
            width: `${normalizedValue * 100}%`,
            background: `linear-gradient(90deg, ${color}80, ${color})`,
            boxShadow: isHovered ? `0 0 12px ${color}60` : 'none'
          }}
        />
        <div
          className="slider-thumb"
          style={{
            left: `${normalizedValue * 100}%`,
            borderColor: color,
            boxShadow: isHovered ? `0 0 10px ${color}` : `0 2px 6px rgba(0,0,0,0.4)`
          }}
        />
      </div>

      <style>{`
        .slider-container {
          width: 100%;
        }

        .slider-header {
          display: flex;
          justify-content: space-between;
          margin-bottom: 6px;
        }

        .slider-label {
          font-size: 10px;
          font-weight: 600;
          letter-spacing: 1px;
          color: rgba(255,255,255,0.5);
          text-transform: uppercase;
        }

        .slider-value {
          font-size: 11px;
          font-weight: 500;
          transition: color 0.2s;
        }

        .slider-track {
          position: relative;
          height: 6px;
          background: rgba(255,255,255,0.08);
          border-radius: 3px;
          cursor: pointer;
          overflow: visible;
        }

        .slider-fill {
          position: absolute;
          top: 0;
          left: 0;
          height: 100%;
          border-radius: 3px;
          transition: box-shadow 0.2s;
        }

        .slider-thumb {
          position: absolute;
          top: 50%;
          width: 14px;
          height: 14px;
          background: #1a1a1e;
          border: 2px solid;
          border-radius: 50%;
          transform: translate(-50%, -50%);
          transition: box-shadow 0.2s;
          pointer-events: none;
        }
      `}</style>
    </div>
  );
}
