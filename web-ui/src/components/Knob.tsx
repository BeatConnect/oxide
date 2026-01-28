import React, { useRef, useCallback, useEffect, useState } from 'react';

interface KnobProps {
  value: number;
  min?: number;
  max?: number;
  label: string;
  unit?: string;
  color?: string;
  size?: 'sm' | 'md' | 'lg';
  bipolar?: boolean;
  onChange: (value: number) => void;
  onDragStart?: () => void;
  onDragEnd?: () => void;
}

export function Knob({
  value,
  min = 0,
  max = 100,
  label,
  unit = '%',
  color = '#ff6b35',
  size = 'md',
  bipolar = false,
  onChange,
  onDragStart,
  onDragEnd
}: KnobProps) {
  const knobRef = useRef<HTMLDivElement>(null);
  const isDragging = useRef(false);
  const startY = useRef(0);
  const startValue = useRef(0);
  const [isHovered, setIsHovered] = useState(false);

  const sizes = {
    sm: { outer: 52, inner: 40, stroke: 3 },
    md: { outer: 68, inner: 54, stroke: 4 },
    lg: { outer: 88, inner: 72, stroke: 5 }
  };

  const s = sizes[size];
  const radius = (s.inner - s.stroke) / 2;
  const circumference = 2 * Math.PI * radius;
  const arcLength = 0.75; // 270 degrees

  const normalizedValue = (value - min) / (max - min);
  const rotation = -135 + normalizedValue * 270;

  // Arc calculation
  const arcStart = bipolar ? 0.5 : 0;
  const arcEnd = normalizedValue;
  const arcProgress = bipolar
    ? Math.abs(arcEnd - arcStart) * arcLength
    : normalizedValue * arcLength;

  const handleMouseDown = useCallback((e: React.MouseEvent) => {
    isDragging.current = true;
    startY.current = e.clientY;
    startValue.current = value;
    onDragStart?.();
    document.body.style.cursor = 'grabbing';
  }, [value, onDragStart]);

  useEffect(() => {
    const handleMouseMove = (e: MouseEvent) => {
      if (!isDragging.current) return;

      const deltaY = startY.current - e.clientY;
      const sensitivity = e.shiftKey ? 0.1 : 0.5;
      const range = max - min;
      const delta = (deltaY * sensitivity * range) / 150;
      const newValue = Math.max(min, Math.min(max, startValue.current + delta));
      onChange(newValue);
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
    onChange(bipolar ? (min + max) / 2 : min);
  };

  const displayValue = Math.round(value);

  return (
    <div
      className="knob-container"
      style={{ width: s.outer, textAlign: 'center' }}
      onMouseEnter={() => setIsHovered(true)}
      onMouseLeave={() => setIsHovered(false)}
    >
      <div
        ref={knobRef}
        className="knob"
        onMouseDown={handleMouseDown}
        onDoubleClick={handleDoubleClick}
        style={{
          width: s.outer,
          height: s.outer,
          position: 'relative',
          cursor: 'grab',
        }}
      >
        {/* Background track */}
        <svg
          width={s.outer}
          height={s.outer}
          style={{ position: 'absolute', top: 0, left: 0 }}
        >
          <circle
            cx={s.outer / 2}
            cy={s.outer / 2}
            r={radius}
            fill="none"
            stroke="rgba(255,255,255,0.08)"
            strokeWidth={s.stroke}
            strokeDasharray={`${circumference * arcLength} ${circumference}`}
            strokeDashoffset={0}
            strokeLinecap="round"
            transform={`rotate(135, ${s.outer / 2}, ${s.outer / 2})`}
          />
          {/* Value arc */}
          <circle
            cx={s.outer / 2}
            cy={s.outer / 2}
            r={radius}
            fill="none"
            stroke={color}
            strokeWidth={s.stroke}
            strokeDasharray={`${circumference * arcProgress} ${circumference}`}
            strokeDashoffset={0}
            strokeLinecap="round"
            transform={`rotate(${bipolar ? 0 : 135}, ${s.outer / 2}, ${s.outer / 2})`}
            style={{
              filter: `drop-shadow(0 0 ${isHovered ? 8 : 4}px ${color})`,
              transition: 'filter 0.2s'
            }}
          />
        </svg>

        {/* Inner knob */}
        <div
          style={{
            position: 'absolute',
            top: (s.outer - s.inner + 8) / 2,
            left: (s.outer - s.inner + 8) / 2,
            width: s.inner - 8,
            height: s.inner - 8,
            borderRadius: '50%',
            background: `radial-gradient(circle at 30% 30%, #2a2a2e, #18181c)`,
            boxShadow: `
              inset 0 2px 4px rgba(255,255,255,0.05),
              inset 0 -2px 4px rgba(0,0,0,0.3),
              0 4px 12px rgba(0,0,0,0.5)
            `,
            transform: `rotate(${rotation}deg)`,
            transition: 'transform 0.05s ease-out'
          }}
        >
          {/* Indicator dot */}
          <div
            style={{
              position: 'absolute',
              top: 6,
              left: '50%',
              transform: 'translateX(-50%)',
              width: 4,
              height: 4,
              borderRadius: '50%',
              background: color,
              boxShadow: `0 0 6px ${color}`
            }}
          />
        </div>
      </div>

      {/* Label */}
      <div style={{
        marginTop: 6,
        fontSize: size === 'sm' ? 9 : 10,
        fontWeight: 600,
        letterSpacing: '1.5px',
        color: 'rgba(255,255,255,0.5)',
        textTransform: 'uppercase'
      }}>
        {label}
      </div>

      {/* Value */}
      <div style={{
        fontSize: size === 'sm' ? 10 : 11,
        fontWeight: 500,
        color: isHovered ? color : 'rgba(255,255,255,0.7)',
        transition: 'color 0.2s'
      }}>
        {displayValue}{unit}
      </div>
    </div>
  );
}
