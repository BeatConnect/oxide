import React from 'react';

interface ModeSelectorProps {
  value: number;
  onChange: (mode: number) => void;
}

const modes = [
  { name: 'CASSETTE', icon: '📼', color: '#ff6b35' },
  { name: 'VINYL', icon: '💿', color: '#8b5cf6' },
  { name: 'VHS', icon: '📹', color: '#06b6d4' },
  { name: 'RADIO', icon: '📻', color: '#22c55e' }
];

export function ModeSelector({ value, onChange }: ModeSelectorProps) {
  return (
    <div className="mode-selector">
      <div className="mode-label">CHARACTER</div>
      <div className="mode-buttons">
        {modes.map((mode, index) => (
          <button
            key={mode.name}
            className={`mode-btn ${value === index ? 'active' : ''}`}
            onClick={() => onChange(index)}
            style={{
              '--mode-color': mode.color
            } as React.CSSProperties}
          >
            <span className="mode-icon">{mode.icon}</span>
            <span className="mode-name">{mode.name}</span>
          </button>
        ))}
      </div>

      <style>{`
        .mode-selector {
          display: flex;
          flex-direction: column;
          align-items: center;
          gap: 8px;
        }

        .mode-label {
          font-size: 10px;
          font-weight: 600;
          letter-spacing: 2px;
          color: rgba(255,255,255,0.4);
          text-transform: uppercase;
        }

        .mode-buttons {
          display: flex;
          gap: 4px;
          background: rgba(0,0,0,0.3);
          padding: 4px;
          border-radius: 8px;
          border: 1px solid rgba(255,255,255,0.05);
        }

        .mode-btn {
          display: flex;
          flex-direction: column;
          align-items: center;
          gap: 2px;
          padding: 8px 12px;
          border: none;
          border-radius: 6px;
          background: transparent;
          cursor: pointer;
          transition: all 0.2s;
          min-width: 70px;
        }

        .mode-btn:hover {
          background: rgba(255,255,255,0.05);
        }

        .mode-btn.active {
          background: rgba(var(--mode-color-rgb, 255,107,53), 0.15);
          box-shadow: 0 0 20px rgba(var(--mode-color-rgb, 255,107,53), 0.2);
        }

        .mode-btn.active .mode-name {
          color: var(--mode-color);
          text-shadow: 0 0 10px var(--mode-color);
        }

        .mode-icon {
          font-size: 18px;
          filter: grayscale(0.5);
          transition: filter 0.2s;
        }

        .mode-btn.active .mode-icon {
          filter: grayscale(0);
        }

        .mode-name {
          font-size: 9px;
          font-weight: 600;
          letter-spacing: 1px;
          color: rgba(255,255,255,0.5);
          transition: all 0.2s;
        }
      `}</style>
    </div>
  );
}
