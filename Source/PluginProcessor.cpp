/*
  ==============================================================================
    OXIDE - Lo-Fi Texture Processor
    A premium degradation effect with character modes
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIDs.h"

OxideAudioProcessor::OxideAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()),
      noiseGen(std::random_device{}())
{
    loadProjectData();

    // Initialize mode characteristics
    // Cassette: warm, wobbly, hissy
    modeChars[0] = { 0.6f, 1.0f, 1.0f, 0.6f, 40.0f, 14000.0f, 1.0f };
    // Vinyl: crackly, warm, slight wobble
    modeChars[1] = { 0.4f, 0.3f, 0.5f, 0.4f, 30.0f, 16000.0f, 0.5f };
    // VHS: heavy wobble, muffled, artifacts
    modeChars[2] = { 0.8f, 2.0f, 0.7f, 0.7f, 60.0f, 10000.0f, 1.5f };
    // Radio: bandpass, noise, compression
    modeChars[3] = { 1.0f, 0.1f, 2.0f, 0.8f, 300.0f, 5000.0f, 2.0f };
}

OxideAudioProcessor::~OxideAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout OxideAudioProcessor::createParameterLayout()
{
    using namespace ParameterIDs;
    using namespace ParameterIDs::Ranges;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Degradation section
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { bitcrush, 1 }, "Bitcrush",
        juce::NormalisableRange<float>(bitcrushMin, bitcrushMax, 0.1f),
        bitcrushDefault, juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { downsample, 1 }, "Downsample",
        juce::NormalisableRange<float>(downsampleMin, downsampleMax, 0.1f),
        downsampleDefault, juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { noise, 1 }, "Noise",
        juce::NormalisableRange<float>(noiseMin, noiseMax, 0.1f),
        noiseDefault, juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { crackle, 1 }, "Crackle",
        juce::NormalisableRange<float>(crackleMin, crackleMax, 0.1f),
        crackleDefault, juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    // Tape/Vinyl section
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { wobble, 1 }, "Wobble",
        juce::NormalisableRange<float>(wobbleMin, wobbleMax, 0.1f),
        wobbleDefault, juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { dropout, 1 }, "Dropout",
        juce::NormalisableRange<float>(dropoutMin, dropoutMax, 0.1f),
        dropoutDefault, juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { saturation, 1 }, "Saturation",
        juce::NormalisableRange<float>(saturationMin, saturationMax, 0.1f),
        saturationDefault, juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { age, 1 }, "Age",
        juce::NormalisableRange<float>(ageMin, ageMax, 0.1f),
        ageDefault, juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    // Filter section
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { filterCutoff, 1 }, "Filter",
        juce::NormalisableRange<float>(filterCutoffMin, filterCutoffMax, 0.1f),
        filterCutoffDefault, juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { filterRes, 1 }, "Resonance",
        juce::NormalisableRange<float>(filterResMin, filterResMax, 0.1f),
        filterResDefault, juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { filterDrive, 1 }, "Filter Drive",
        juce::NormalisableRange<float>(filterDriveMin, filterDriveMax, 0.1f),
        filterDriveDefault, juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    // Mode selector
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { mode, 1 }, "Mode",
        juce::StringArray { "Cassette", "Vinyl", "VHS", "Radio" },
        modeDefault
    ));

    // Output section
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { mix, 1 }, "Mix",
        juce::NormalisableRange<float>(mixMin, mixMax, 0.1f),
        mixDefault, juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { output, 1 }, "Output",
        juce::NormalisableRange<float>(outputMin, outputMax, 0.1f),
        outputDefault, juce::AudioParameterFloatAttributes().withLabel("dB")
    ));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { bypass, 1 }, "Bypass", false
    ));

    return { params.begin(), params.end() };
}

void OxideAudioProcessor::loadProjectData()
{
#if HAS_PROJECT_DATA
    int dataSize = 0;
    const char* data = ProjectData::getNamedResource("project_data_json", dataSize);

    if (data == nullptr || dataSize == 0)
    {
        DBG("No project_data.json found in binary data");
        return;
    }

    auto parsed = juce::JSON::parse(juce::String::fromUTF8(data, dataSize));
    if (parsed.isVoid())
    {
        DBG("Failed to parse project_data.json");
        return;
    }

    pluginId = parsed.getProperty("pluginId", "").toString();
    apiBaseUrl = parsed.getProperty("apiBaseUrl", "").toString();
    supabaseKey = parsed.getProperty("supabasePublishableKey", "").toString();
    buildFlags = parsed.getProperty("flags", juce::var());

    DBG("Loaded project data - pluginId: " + pluginId);

#if BEATCONNECT_ACTIVATION_ENABLED
    bool enableActivation = static_cast<bool>(buildFlags.getProperty("enableActivationKeys", false));

    if (enableActivation && pluginId.isNotEmpty())
    {
        beatconnect::ActivationConfig config;
        config.apiBaseUrl = apiBaseUrl.toStdString();
        config.pluginId = pluginId.toStdString();
        config.supabaseKey = supabaseKey.toStdString();
        config.validateOnStartup = true;
        config.revalidateIntervalSeconds = 86400;

        activation = beatconnect::Activation::create(config);
        DBG("Activation system configured");
    }
#endif
#endif
}

bool OxideAudioProcessor::hasActivationEnabled() const
{
#if HAS_PROJECT_DATA && BEATCONNECT_ACTIVATION_ENABLED
    return static_cast<bool>(buildFlags.getProperty("enableActivationKeys", false));
#else
    return false;
#endif
}

void OxideAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock * 2);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    // Prepare filters
    lowpassFilter.prepare(spec);
    lowpassFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

    highpassFilter.prepare(spec);
    highpassFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);

    bandpassFilter.prepare(spec);
    bandpassFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);

    tapeHeadFilter.prepare(spec);
    tapeHeadFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    tapeHeadFilter.setCutoffFrequency(80.0f);  // Tape head bump

    // Reset delay lines
    std::memset(delayLine, 0, sizeof(delayLine));
    delayWritePos = 0;

    // Reset envelopes and states
    for (int i = 0; i < 2; ++i)
    {
        bitcrushHold[i] = 0.0f;
        bitcrushCounter[i] = 0;
        lastSample[i] = 0.0f;
        sampleHoldCounter[i] = 0.0f;
        crackleEnvelope[i] = 0.0f;
        dcState[i] = 0.0f;
        modulatedDelay[i] = 512.0f;
    }

    dropoutEnvelope = 1.0f;
    dropoutTimer = 0.0f;
    inDropout = false;
    wowPhase = 0.0f;
    flutterPhase = 0.0f;
    driftPhase = 0.0f;

    // Parameter smoothing
    const double smoothTime = 0.02;
    bitcrushSmoothed.reset(sampleRate, smoothTime);
    downsampleSmoothed.reset(sampleRate, smoothTime);
    noiseSmoothed.reset(sampleRate, smoothTime);
    wobbleSmoothed.reset(sampleRate, smoothTime);
    saturationSmoothed.reset(sampleRate, smoothTime);
    filterCutoffSmoothed.reset(sampleRate, 0.05);
    mixSmoothed.reset(sampleRate, smoothTime);

    // Initialize to current values
    bitcrushSmoothed.setCurrentAndTargetValue(apvts.getRawParameterValue(ParameterIDs::bitcrush)->load());
    downsampleSmoothed.setCurrentAndTargetValue(apvts.getRawParameterValue(ParameterIDs::downsample)->load());
    noiseSmoothed.setCurrentAndTargetValue(apvts.getRawParameterValue(ParameterIDs::noise)->load());
    wobbleSmoothed.setCurrentAndTargetValue(apvts.getRawParameterValue(ParameterIDs::wobble)->load());
    saturationSmoothed.setCurrentAndTargetValue(apvts.getRawParameterValue(ParameterIDs::saturation)->load());
    filterCutoffSmoothed.setCurrentAndTargetValue(apvts.getRawParameterValue(ParameterIDs::filterCutoff)->load());
    mixSmoothed.setCurrentAndTargetValue(apvts.getRawParameterValue(ParameterIDs::mix)->load());

    // DC blocker coefficient
    dcCoeff = 1.0f - (20.0f / static_cast<float>(sampleRate));

    DBG("prepareToPlay - sampleRate: " + juce::String(sampleRate) + ", blockSize: " + juce::String(samplesPerBlock));
}

void OxideAudioProcessor::releaseResources()
{
    lowpassFilter.reset();
    highpassFilter.reset();
    bandpassFilter.reset();
    tapeHeadFilter.reset();
}

bool OxideAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void OxideAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    const float sampleRate = static_cast<float>(currentSampleRate);

    // Clear unused channels
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, numSamples);

    // =========================================================================
    // GET PARAMETERS
    // =========================================================================
    const float bitcrushVal = apvts.getRawParameterValue(ParameterIDs::bitcrush)->load();
    const float downsampleVal = apvts.getRawParameterValue(ParameterIDs::downsample)->load();
    const float noiseVal = apvts.getRawParameterValue(ParameterIDs::noise)->load();
    const float crackleVal = apvts.getRawParameterValue(ParameterIDs::crackle)->load();
    const float wobbleVal = apvts.getRawParameterValue(ParameterIDs::wobble)->load();
    const float dropoutVal = apvts.getRawParameterValue(ParameterIDs::dropout)->load();
    const float saturationVal = apvts.getRawParameterValue(ParameterIDs::saturation)->load();
    const float ageVal = apvts.getRawParameterValue(ParameterIDs::age)->load();
    const float filterCutoffVal = apvts.getRawParameterValue(ParameterIDs::filterCutoff)->load();
    const float filterResVal = apvts.getRawParameterValue(ParameterIDs::filterRes)->load();
    const float filterDriveVal = apvts.getRawParameterValue(ParameterIDs::filterDrive)->load();
    const int modeVal = static_cast<int>(apvts.getRawParameterValue(ParameterIDs::mode)->load());
    const float mixVal = apvts.getRawParameterValue(ParameterIDs::mix)->load();
    const float outputVal = apvts.getRawParameterValue(ParameterIDs::output)->load();
    const bool bypassVal = apvts.getRawParameterValue(ParameterIDs::bypass)->load() > 0.5f;

    // Store for UI
    currentMode.store(modeVal);
    bypassed.store(bypassVal);

    // Update smoothed parameters
    bitcrushSmoothed.setTargetValue(bitcrushVal);
    downsampleSmoothed.setTargetValue(downsampleVal);
    noiseSmoothed.setTargetValue(noiseVal);
    wobbleSmoothed.setTargetValue(wobbleVal);
    saturationSmoothed.setTargetValue(saturationVal);
    filterCutoffSmoothed.setTargetValue(filterCutoffVal);
    mixSmoothed.setTargetValue(mixVal);

    // Visualizer data (pre-processing)
    float inputRms = 0.0f;
    float peak = 0.0f;
    for (int ch = 0; ch < numChannels; ++ch)
    {
        inputRms += buffer.getRMSLevel(ch, 0, numSamples);
        peak = std::max(peak, buffer.getMagnitude(ch, 0, numSamples));
    }
    inputRms /= static_cast<float>(numChannels);
    currentRMS.store(inputRms);
    currentPeak.store(peak);

    if (bypassVal) return;

    // Get mode characteristics
    const auto& mc = modeChars[modeVal];

    // Age affects all degradation
    const float ageMult = 1.0f + (ageVal / 100.0f) * 2.0f;

    // Store overall degradation for visualizer
    degradationAmount.store((bitcrushVal + downsampleVal + noiseVal + wobbleVal + saturationVal) / 500.0f * ageMult);

    // =========================================================================
    // STORE DRY SIGNAL
    // =========================================================================
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    // =========================================================================
    // PROCESSING
    // =========================================================================

    // LFO increments for wow/flutter
    const float wowInc = (0.5f * mc.wobbleRate) / sampleRate;      // ~0.5 Hz wow
    const float flutterInc = (8.0f * mc.wobbleRate) / sampleRate;  // ~8 Hz flutter
    const float driftInc = 0.05f / sampleRate;                      // Very slow drift

    for (int ch = 0; ch < std::min(numChannels, 2); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            float sample = data[i];

            // Get smoothed values
            const float bcAmount = bitcrushSmoothed.getNextValue() / 100.0f * ageMult;
            const float dsAmount = downsampleSmoothed.getNextValue() / 100.0f * ageMult;
            const float noiseAmount = noiseSmoothed.getNextValue() / 100.0f * ageMult * mc.hissAmount;
            const float wobAmount = wobbleSmoothed.getNextValue() / 100.0f * mc.wobbleDepth * ageMult;
            const float satAmount = saturationSmoothed.getNextValue() / 100.0f * ageMult;

            // =====================================================
            // STAGE 1: BITCRUSHING
            // =====================================================
            if (bcAmount > 0.01f)
            {
                // Map 0-1 to 24-bit down to 2-bit
                const float bits = 24.0f - bcAmount * 22.0f;
                const float levels = std::pow(2.0f, bits);
                sample = std::round(sample * levels) / levels;

                // Add slight noise at low bit depths
                if (bits < 8.0f)
                {
                    sample += noiseDist(noiseGen) * 0.002f * (8.0f - bits);
                }
            }

            // =====================================================
            // STAGE 2: SAMPLE RATE REDUCTION
            // =====================================================
            if (dsAmount > 0.01f)
            {
                // Map 0-1 to factor 1-64
                const float factor = 1.0f + dsAmount * 63.0f;
                sampleHoldCounter[ch] += 1.0f;

                if (sampleHoldCounter[ch] >= factor)
                {
                    lastSample[ch] = sample;
                    sampleHoldCounter[ch] -= factor;
                }
                sample = lastSample[ch];
            }

            // =====================================================
            // STAGE 3: WOW & FLUTTER (Pitch Modulation)
            // =====================================================
            if (wobAmount > 0.01f)
            {
                // Wow: slow sine
                const float wow = std::sin(wowPhase * juce::MathConstants<float>::twoPi) * wobAmount * 15.0f;
                // Flutter: faster, irregular
                const float flutter = std::sin(flutterPhase * juce::MathConstants<float>::twoPi * 3.0f) * wobAmount * 5.0f;
                // Drift: very slow random-ish
                const float drift = std::sin(driftPhase * juce::MathConstants<float>::twoPi * 0.1f) * wobAmount * 3.0f;

                const float totalMod = wow + flutter + drift;

                // Write to delay line
                delayLine[ch][delayWritePos] = sample;

                // Calculate modulated read position
                const float baseDelay = 512.0f;
                const float modDelay = baseDelay + totalMod;
                modulatedDelay[ch] = modDelay;

                // Linear interpolation read
                const float readPosFloat = static_cast<float>(delayWritePos) - modDelay;
                int readPos = static_cast<int>(std::floor(readPosFloat));
                const float frac = readPosFloat - static_cast<float>(readPos);

                // Wrap
                while (readPos < 0) readPos += kMaxDelaySize;
                readPos = readPos % kMaxDelaySize;
                const int readPosNext = (readPos + 1) % kMaxDelaySize;

                sample = delayLine[ch][readPos] * (1.0f - frac) + delayLine[ch][readPosNext] * frac;
            }
            else
            {
                // Still write to delay line for consistency
                delayLine[ch][delayWritePos] = sample;
            }

            // =====================================================
            // STAGE 4: SATURATION (Mode-dependent)
            // =====================================================
            if (satAmount > 0.01f)
            {
                const float drive = 1.0f + satAmount * 5.0f;
                float driven = sample * drive;

                // Mode-dependent saturation curve
                switch (modeVal)
                {
                    case 0: // Cassette: warm tape saturation
                    {
                        driven = std::tanh(driven * 1.5f);
                        // Even harmonics for warmth
                        driven += 0.1f * satAmount * driven * std::abs(driven);
                        break;
                    }
                    case 1: // Vinyl: gentle compression
                    {
                        driven = driven / (1.0f + std::abs(driven) * 0.5f);
                        break;
                    }
                    case 2: // VHS: harsh, gritty
                    {
                        // Asymmetric clipping
                        if (driven > 0.7f) driven = 0.7f + (driven - 0.7f) * 0.2f;
                        if (driven < -0.5f) driven = -0.5f + (driven + 0.5f) * 0.3f;
                        driven = std::tanh(driven * 2.0f);
                        break;
                    }
                    case 3: // Radio: hard limiting
                    {
                        driven = std::clamp(driven, -0.8f, 0.8f);
                        driven = std::tanh(driven * 2.5f);
                        break;
                    }
                }

                // Makeup gain
                sample = driven * (1.0f / drive);
            }

            // =====================================================
            // STAGE 5: DROPOUT (Tape/VHS)
            // =====================================================
            if (dropoutVal > 0.01f && (modeVal == 0 || modeVal == 2))
            {
                // Random dropout trigger
                if (!inDropout && crackleChance(noiseGen) < dropoutVal * 0.0001f * ageMult)
                {
                    inDropout = true;
                    dropoutTimer = 0.0f;
                }

                if (inDropout)
                {
                    dropoutTimer += 1.0f;
                    const float dropoutLength = 50.0f + crackleChance(noiseGen) * 500.0f;

                    if (dropoutTimer < dropoutLength)
                    {
                        // Quick fade out, slow fade in
                        if (dropoutTimer < 10.0f)
                            dropoutEnvelope = 1.0f - (dropoutTimer / 10.0f);
                        else if (dropoutTimer > dropoutLength - 50.0f)
                            dropoutEnvelope = (dropoutTimer - (dropoutLength - 50.0f)) / 50.0f;
                        else
                            dropoutEnvelope = 0.0f;
                    }
                    else
                    {
                        inDropout = false;
                        dropoutEnvelope = 1.0f;
                    }
                }

                sample *= dropoutEnvelope;
            }

            // =====================================================
            // STAGE 6: NOISE & HISS
            // =====================================================
            if (noiseAmount > 0.001f)
            {
                float noiseOut = 0.0f;

                // Generate colored noise based on mode
                if (mc.noiseColor < 0.5f)
                {
                    // Pink-ish noise (filtered white)
                    noiseOut = noiseDist(noiseGen);
                    // Simple 1-pole lowpass for pink approximation
                    static float pinkState[2] = { 0.0f, 0.0f };
                    pinkState[ch] = pinkState[ch] * 0.9f + noiseOut * 0.1f;
                    noiseOut = pinkState[ch] * 3.0f;
                }
                else
                {
                    // White noise
                    noiseOut = noiseDist(noiseGen);
                }

                sample += noiseOut * noiseAmount * 0.05f;
            }

            // =====================================================
            // STAGE 7: CRACKLE & POPS (Vinyl mode)
            // =====================================================
            if (crackleVal > 0.01f)
            {
                // Random crackle trigger
                if (crackleChance(noiseGen) < crackleVal * 0.002f * ageMult)
                {
                    // Generate a pop
                    crackleEnvelope[ch] = 0.3f + crackleChance(noiseGen) * 0.7f;
                    crackleEnvelope[ch] *= (crackleChance(noiseGen) > 0.5f ? 1.0f : -1.0f);
                }

                // Decay crackle
                crackleEnvelope[ch] *= 0.85f;

                sample += crackleEnvelope[ch] * crackleVal / 100.0f;
                crackleActivity.store(std::abs(crackleEnvelope[ch]));
            }

            // =====================================================
            // STAGE 8: DC BLOCKING
            // =====================================================
            const float dcInput = sample;
            sample = sample - dcState[ch];
            dcState[ch] = dcInput * (1.0f - dcCoeff);

            data[i] = sample;
        }
    }

    // Advance delay write position (once per block for both channels)
    delayWritePos = (delayWritePos + numSamples) % kMaxDelaySize;

    // Advance LFO phases
    wowPhase += wowInc * static_cast<float>(numSamples);
    if (wowPhase >= 1.0f) wowPhase -= 1.0f;
    flutterPhase += flutterInc * static_cast<float>(numSamples);
    if (flutterPhase >= 1.0f) flutterPhase -= 1.0f;
    driftPhase += driftInc * static_cast<float>(numSamples);
    if (driftPhase >= 1.0f) driftPhase -= 1.0f;

    wobblePhaseVis.store(wowPhase);

    // =========================================================================
    // STAGE 9: FILTERING
    // =========================================================================
    const float cutoffNorm = filterCutoffSmoothed.getNextValue() / 100.0f;
    const float resNorm = filterResVal / 100.0f;
    const float driveNorm = filterDriveVal / 100.0f;

    // Map cutoff 0-1 to 200Hz - 20kHz with mode influence
    float cutoffFreq = 200.0f * std::pow(100.0f, cutoffNorm);
    cutoffFreq = std::min(cutoffFreq, mc.lpFreq);

    lowpassFilter.setCutoffFrequency(cutoffFreq);
    lowpassFilter.setResonance(0.5f + resNorm * 9.5f);

    // Apply filter drive (pre-filter saturation)
    if (driveNorm > 0.01f)
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i)
            {
                const float drive = 1.0f + driveNorm * 10.0f;
                data[i] = std::tanh(data[i] * drive) / drive;
            }
        }
    }

    // Apply lowpass
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    lowpassFilter.process(context);

    // Mode-specific filtering
    if (modeVal == 3) // Radio mode: additional bandpass
    {
        highpassFilter.setCutoffFrequency(mc.hpFreq);
        highpassFilter.process(context);
    }
    else
    {
        // Light high-pass for other modes
        highpassFilter.setCutoffFrequency(mc.hpFreq);
        highpassFilter.process(context);
    }

    // =========================================================================
    // STAGE 10: DRY/WET MIX
    // =========================================================================
    const float mixNorm = mixSmoothed.getNextValue() / 100.0f;

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* wet = buffer.getWritePointer(ch);
        const auto* dry = dryBuffer.getReadPointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            wet[i] = wet[i] * mixNorm + dry[i] * (1.0f - mixNorm);
        }
    }

    // =========================================================================
    // STAGE 11: OUTPUT GAIN
    // =========================================================================
    const float outputGain = juce::Decibels::decibelsToGain(outputVal);
    buffer.applyGain(outputGain);
}

juce::AudioProcessorEditor* OxideAudioProcessor::createEditor()
{
    return new OxideAudioProcessorEditor(*this);
}

void OxideAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty("stateVersion", kStateVersion, nullptr);

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OxideAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
    {
        auto newState = juce::ValueTree::fromXml(*xml);

        int version = newState.getProperty("stateVersion", 0);
        if (version < kStateVersion)
        {
            DBG("Migrating state from version " + juce::String(version) + " to " + juce::String(kStateVersion));
        }

        apvts.replaceState(newState);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OxideAudioProcessor();
}
