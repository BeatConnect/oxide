#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <random>

#if HAS_PROJECT_DATA
#include "ProjectData.h"
#endif

#if BEATCONNECT_ACTIVATION_ENABLED
#include <beatconnect/Activation.h>
#endif

class OxideAudioProcessor : public juce::AudioProcessor
{
public:
    OxideAudioProcessor();
    ~OxideAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.1; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Visualizer data access (thread-safe)
    float getCurrentRMS() const { return currentRMS.load(); }
    float getCurrentPeak() const { return currentPeak.load(); }
    float getWobblePhase() const { return wobblePhaseVis.load(); }
    float getCrackleActivity() const { return crackleActivity.load(); }
    int getCurrentMode() const { return currentMode.load(); }
    bool isBypassed() const { return bypassed.load(); }
    float getDegradationAmount() const { return degradationAmount.load(); }

    // BeatConnect integration
    bool hasActivationEnabled() const;
    juce::String getPluginId() const { return pluginId; }
    juce::String getApiBaseUrl() const { return apiBaseUrl; }
    juce::String getSupabaseKey() const { return supabaseKey; }

#if BEATCONNECT_ACTIVATION_ENABLED
    beatconnect::Activation* getActivation() { return activation.get(); }
#endif

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void loadProjectData();
    void updateModeCharacteristics(int mode);

    juce::AudioProcessorValueTreeState apvts;

    // === DSP Components ===

    // Bitcrusher state
    float bitcrushHold[2] = { 0.0f, 0.0f };
    int bitcrushCounter[2] = { 0, 0 };

    // Sample rate reduction
    float lastSample[2] = { 0.0f, 0.0f };
    float sampleHoldCounter[2] = { 0.0f, 0.0f };

    // Noise generator
    std::mt19937 noiseGen;
    std::uniform_real_distribution<float> noiseDist { -1.0f, 1.0f };
    std::uniform_real_distribution<float> crackleChance { 0.0f, 1.0f };

    // Wow & flutter LFOs
    float wowPhase = 0.0f;
    float flutterPhase = 0.0f;
    float driftPhase = 0.0f;

    // Pitch shifting delay line for wow/flutter
    static constexpr int kMaxDelaySize = 4096;
    float delayLine[2][kMaxDelaySize] = {};
    int delayWritePos = 0;
    float modulatedDelay[2] = { 0.0f, 0.0f };

    // Dropout simulation
    float dropoutEnvelope = 1.0f;
    float dropoutTimer = 0.0f;
    bool inDropout = false;

    // Crackle state
    float crackleEnvelope[2] = { 0.0f, 0.0f };
    int crackleTimer = 0;

    // Filters
    juce::dsp::StateVariableTPTFilter<float> lowpassFilter;
    juce::dsp::StateVariableTPTFilter<float> highpassFilter;  // For radio mode
    juce::dsp::StateVariableTPTFilter<float> bandpassFilter;  // For radio mode
    juce::dsp::StateVariableTPTFilter<float> tapeHeadFilter;  // Tape head bump

    // DC blocker
    float dcState[2] = { 0.0f, 0.0f };
    float dcCoeff = 0.995f;

    // Smoothed parameters
    juce::SmoothedValue<float> bitcrushSmoothed;
    juce::SmoothedValue<float> downsampleSmoothed;
    juce::SmoothedValue<float> noiseSmoothed;
    juce::SmoothedValue<float> wobbleSmoothed;
    juce::SmoothedValue<float> saturationSmoothed;
    juce::SmoothedValue<float> filterCutoffSmoothed;
    juce::SmoothedValue<float> mixSmoothed;

    // Mode-specific characteristics
    struct ModeChar {
        float noiseColor = 1.0f;      // 0=pink, 1=white, 0.5=tape
        float wobbleDepth = 1.0f;     // Pitch modulation depth
        float wobbleRate = 1.0f;      // Pitch modulation rate
        float satCurve = 0.5f;        // Saturation curve type
        float hpFreq = 20.0f;         // High-pass frequency
        float lpFreq = 20000.0f;      // Low-pass frequency
        float hissAmount = 1.0f;      // Noise character
    };
    ModeChar modeChars[4];  // Cassette, Vinyl, VHS, Radio
    int lastMode = -1;

    // Visualizer data (atomic for thread safety)
    std::atomic<float> currentRMS { 0.0f };
    std::atomic<float> currentPeak { 0.0f };
    std::atomic<float> wobblePhaseVis { 0.0f };
    std::atomic<float> crackleActivity { 0.0f };
    std::atomic<int> currentMode { 0 };
    std::atomic<bool> bypassed { false };
    std::atomic<float> degradationAmount { 0.0f };

    double currentSampleRate = 44100.0;

    // BeatConnect data
    juce::String pluginId;
    juce::String apiBaseUrl;
    juce::String supabaseKey;
    juce::var buildFlags;

#if BEATCONNECT_ACTIVATION_ENABLED
    std::unique_ptr<beatconnect::Activation> activation;
#endif

    // State version for backwards compatibility
    static constexpr int kStateVersion = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OxideAudioProcessor)
};
