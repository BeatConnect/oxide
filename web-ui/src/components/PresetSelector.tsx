import React, { useState } from 'react';

const presets = [
  { name: 'Init', category: 'Default' },
  { name: 'Dusty Vinyl', category: 'Vinyl' },
  { name: 'Worn Cassette', category: 'Cassette' },
  { name: 'VHS Memories', category: 'VHS' },
  { name: 'AM Radio', category: 'Radio' },
  { name: 'Lo-Fi Beats', category: 'Cassette' },
  { name: 'Old Record', category: 'Vinyl' },
  { name: 'Tape Warble', category: 'Cassette' },
  { name: 'Broken TV', category: 'VHS' },
  { name: 'Subtle Warmth', category: 'Cassette' },
];

export function PresetSelector() {
  const [isOpen, setIsOpen] = useState(false);
  const [currentPreset, setCurrentPreset] = useState('Init');

  const handleSelect = (preset: string) => {
    setCurrentPreset(preset);
    setIsOpen(false);
    // TODO: Load preset values
  };

  return (
    <div className="preset-selector">
      <button
        className="preset-button"
        onClick={() => setIsOpen(!isOpen)}
      >
        <span className="preset-label">PRESET</span>
        <span className="preset-name">{currentPreset}</span>
        <span className="preset-arrow">{isOpen ? '▲' : '▼'}</span>
      </button>

      {isOpen && (
        <div className="preset-dropdown">
          {presets.map((preset) => (
            <button
              key={preset.name}
              className={`preset-item ${currentPreset === preset.name ? 'active' : ''}`}
              onClick={() => handleSelect(preset.name)}
            >
              <span className="preset-item-name">{preset.name}</span>
              <span className="preset-item-category">{preset.category}</span>
            </button>
          ))}
        </div>
      )}

      <style>{`
        .preset-selector {
          position: relative;
        }

        .preset-button {
          display: flex;
          align-items: center;
          gap: 8px;
          padding: 8px 12px;
          background: rgba(0,0,0,0.3);
          border: 1px solid rgba(255,255,255,0.08);
          border-radius: 6px;
          cursor: pointer;
          transition: all 0.2s;
        }

        .preset-button:hover {
          border-color: rgba(255,255,255,0.15);
          background: rgba(0,0,0,0.4);
        }

        .preset-label {
          font-size: 9px;
          font-weight: 600;
          letter-spacing: 1px;
          color: rgba(255,255,255,0.4);
        }

        .preset-name {
          font-size: 12px;
          font-weight: 500;
          color: rgba(255,255,255,0.8);
          min-width: 100px;
        }

        .preset-arrow {
          font-size: 8px;
          color: rgba(255,255,255,0.4);
        }

        .preset-dropdown {
          position: absolute;
          top: 100%;
          left: 0;
          margin-top: 4px;
          min-width: 200px;
          max-height: 300px;
          overflow-y: auto;
          background: #1a1a1e;
          border: 1px solid rgba(255,255,255,0.1);
          border-radius: 8px;
          box-shadow: 0 10px 40px rgba(0,0,0,0.5);
          z-index: 100;
        }

        .preset-item {
          display: flex;
          justify-content: space-between;
          align-items: center;
          width: 100%;
          padding: 10px 14px;
          background: transparent;
          border: none;
          cursor: pointer;
          transition: background 0.15s;
        }

        .preset-item:hover {
          background: rgba(255,107,53,0.1);
        }

        .preset-item.active {
          background: rgba(255,107,53,0.15);
        }

        .preset-item-name {
          font-size: 13px;
          color: rgba(255,255,255,0.9);
        }

        .preset-item.active .preset-item-name {
          color: #ff6b35;
        }

        .preset-item-category {
          font-size: 10px;
          color: rgba(255,255,255,0.3);
        }
      `}</style>
    </div>
  );
}
