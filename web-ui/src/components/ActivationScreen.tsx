import React, { useState, useEffect, useCallback } from 'react';
import { addEventListener, removeEventListener, emitEvent, isInJuceWebView } from '../lib/juce-bridge';

interface ActivationScreenProps {
  onActivated: () => void;
}

export function ActivationScreen({ onActivated }: ActivationScreenProps) {
  const [licenseKey, setLicenseKey] = useState('');
  const [status, setStatus] = useState<'idle' | 'loading' | 'error' | 'success'>('idle');
  const [errorMessage, setErrorMessage] = useState('');

  useEffect(() => {
    if (!isInJuceWebView()) {
      // Dev mode - auto activate
      setTimeout(onActivated, 500);
      return;
    }

    // Check initial activation status
    emitEvent('getActivationStatus');

    const handleActivationState = (data: any) => {
      if (data.isActivated) {
        onActivated();
      }
    };

    const handleActivationResult = (data: any) => {
      if (data.status === 'Valid') {
        setStatus('success');
        setTimeout(onActivated, 1000);
      } else {
        setStatus('error');
        setErrorMessage(data.status || 'Activation failed');
      }
    };

    addEventListener('activationState', handleActivationState);
    addEventListener('activationResult', handleActivationResult);

    return () => {
      removeEventListener('activationState', handleActivationState);
      removeEventListener('activationResult', handleActivationResult);
    };
  }, [onActivated]);

  const handleSubmit = useCallback((e: React.FormEvent) => {
    e.preventDefault();
    if (!licenseKey.trim()) return;

    setStatus('loading');
    setErrorMessage('');
    emitEvent('activateLicense', { code: licenseKey.trim() });
  }, [licenseKey]);

  return (
    <div className="activation-screen">
      <div className="activation-card">
        <div className="activation-logo">
          <span className="logo-text">OXIDE</span>
          <span className="logo-sub">Lo-Fi Texture Processor</span>
        </div>

        <form onSubmit={handleSubmit} className="activation-form">
          <div className="input-group">
            <label>License Key</label>
            <input
              type="text"
              value={licenseKey}
              onChange={(e) => setLicenseKey(e.target.value)}
              placeholder="XXXX-XXXX-XXXX-XXXX"
              disabled={status === 'loading'}
              autoFocus
            />
          </div>

          {errorMessage && (
            <div className="error-message">{errorMessage}</div>
          )}

          <button
            type="submit"
            disabled={status === 'loading' || !licenseKey.trim()}
            className={status === 'success' ? 'success' : ''}
          >
            {status === 'loading' ? 'Activating...' :
             status === 'success' ? 'Activated!' : 'Activate'}
          </button>
        </form>

        <div className="activation-footer">
          <a href="https://beatconnect.io" target="_blank" rel="noopener noreferrer">
            Get a license
          </a>
        </div>
      </div>

      <style>{`
        .activation-screen {
          position: fixed;
          inset: 0;
          background: linear-gradient(135deg, #0a0a0c 0%, #141418 50%, #0a0a0c 100%);
          display: flex;
          align-items: center;
          justify-content: center;
          z-index: 1000;
        }

        .activation-card {
          width: 380px;
          background: rgba(20, 20, 24, 0.95);
          border: 1px solid rgba(255,255,255,0.08);
          border-radius: 16px;
          padding: 40px;
          box-shadow: 0 20px 60px rgba(0,0,0,0.5);
        }

        .activation-logo {
          text-align: center;
          margin-bottom: 32px;
        }

        .logo-text {
          display: block;
          font-family: 'Rajdhani', sans-serif;
          font-size: 42px;
          font-weight: 300;
          letter-spacing: 12px;
          color: #ff6b35;
          text-shadow: 0 0 30px rgba(255,107,53,0.4);
        }

        .logo-sub {
          display: block;
          font-size: 12px;
          letter-spacing: 3px;
          color: rgba(255,255,255,0.4);
          margin-top: 4px;
        }

        .activation-form {
          display: flex;
          flex-direction: column;
          gap: 20px;
        }

        .input-group {
          display: flex;
          flex-direction: column;
          gap: 8px;
        }

        .input-group label {
          font-size: 11px;
          font-weight: 600;
          letter-spacing: 1px;
          color: rgba(255,255,255,0.5);
          text-transform: uppercase;
        }

        .input-group input {
          padding: 14px 16px;
          background: rgba(0,0,0,0.3);
          border: 1px solid rgba(255,255,255,0.1);
          border-radius: 8px;
          color: white;
          font-size: 16px;
          font-family: 'JetBrains Mono', monospace;
          letter-spacing: 2px;
          text-align: center;
          outline: none;
          transition: border-color 0.2s;
        }

        .input-group input:focus {
          border-color: #ff6b35;
        }

        .input-group input::placeholder {
          color: rgba(255,255,255,0.2);
        }

        .error-message {
          padding: 12px;
          background: rgba(239,68,68,0.1);
          border: 1px solid rgba(239,68,68,0.3);
          border-radius: 8px;
          color: #ef4444;
          font-size: 13px;
          text-align: center;
        }

        button {
          padding: 14px 24px;
          background: #ff6b35;
          border: none;
          border-radius: 8px;
          color: white;
          font-size: 14px;
          font-weight: 600;
          letter-spacing: 1px;
          cursor: pointer;
          transition: all 0.2s;
        }

        button:hover:not(:disabled) {
          background: #ff8555;
          transform: translateY(-1px);
        }

        button:disabled {
          opacity: 0.5;
          cursor: not-allowed;
        }

        button.success {
          background: #22c55e;
        }

        .activation-footer {
          margin-top: 24px;
          text-align: center;
        }

        .activation-footer a {
          color: rgba(255,255,255,0.4);
          font-size: 12px;
          text-decoration: none;
          transition: color 0.2s;
        }

        .activation-footer a:hover {
          color: #ff6b35;
        }
      `}</style>
    </div>
  );
}
