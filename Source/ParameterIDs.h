#pragma once

namespace ParameterIDs
{
    // === DEGRADATION SECTION ===
    inline constexpr const char* bitcrush     = "bitcrush";     // Bit depth reduction (0-100%)
    inline constexpr const char* downsample   = "downsample";   // Sample rate reduction (0-100%)
    inline constexpr const char* noise        = "noise";        // Noise/hiss amount (0-100%)
    inline constexpr const char* crackle      = "crackle";      // Vinyl crackle amount (0-100%)

    // === TAPE/VINYL SECTION ===
    inline constexpr const char* wobble       = "wobble";       // Wow & flutter amount (0-100%)
    inline constexpr const char* dropout      = "dropout";      // Tape dropout frequency (0-100%)
    inline constexpr const char* saturation   = "saturation";   // Tape/vinyl saturation (0-100%)
    inline constexpr const char* age          = "age";          // Overall degradation intensity (0-100%)

    // === FILTER SECTION ===
    inline constexpr const char* filterCutoff = "filterCutoff"; // Low-pass filter (0-100%, maps to freq)
    inline constexpr const char* filterRes    = "filterRes";    // Filter resonance (0-100%)
    inline constexpr const char* filterDrive  = "filterDrive";  // Pre-filter drive (0-100%)

    // === CHARACTER MODES ===
    inline constexpr const char* mode         = "mode";         // 0=Cassette, 1=Vinyl, 2=VHS, 3=Radio

    // === OUTPUT ===
    inline constexpr const char* mix          = "mix";          // Dry/wet mix (0-100%)
    inline constexpr const char* output       = "output";       // Output gain (-24 to +12 dB)
    inline constexpr const char* bypass       = "bypass";       // Master bypass

    // Parameter ranges
    namespace Ranges
    {
        // Bitcrush: 0-100% (maps to 24-bit down to 2-bit)
        inline constexpr float bitcrushMin = 0.0f;
        inline constexpr float bitcrushMax = 100.0f;
        inline constexpr float bitcrushDefault = 0.0f;

        // Downsample: 0-100% (maps to full rate down to 1/64)
        inline constexpr float downsampleMin = 0.0f;
        inline constexpr float downsampleMax = 100.0f;
        inline constexpr float downsampleDefault = 0.0f;

        // Noise: 0-100%
        inline constexpr float noiseMin = 0.0f;
        inline constexpr float noiseMax = 100.0f;
        inline constexpr float noiseDefault = 15.0f;

        // Crackle: 0-100%
        inline constexpr float crackleMin = 0.0f;
        inline constexpr float crackleMax = 100.0f;
        inline constexpr float crackleDefault = 0.0f;

        // Wobble: 0-100%
        inline constexpr float wobbleMin = 0.0f;
        inline constexpr float wobbleMax = 100.0f;
        inline constexpr float wobbleDefault = 20.0f;

        // Dropout: 0-100%
        inline constexpr float dropoutMin = 0.0f;
        inline constexpr float dropoutMax = 100.0f;
        inline constexpr float dropoutDefault = 0.0f;

        // Saturation: 0-100%
        inline constexpr float saturationMin = 0.0f;
        inline constexpr float saturationMax = 100.0f;
        inline constexpr float saturationDefault = 30.0f;

        // Age: 0-100% (master degradation)
        inline constexpr float ageMin = 0.0f;
        inline constexpr float ageMax = 100.0f;
        inline constexpr float ageDefault = 25.0f;

        // Filter cutoff: 0-100% (maps to 200Hz - 20kHz)
        inline constexpr float filterCutoffMin = 0.0f;
        inline constexpr float filterCutoffMax = 100.0f;
        inline constexpr float filterCutoffDefault = 80.0f;

        // Filter resonance: 0-100%
        inline constexpr float filterResMin = 0.0f;
        inline constexpr float filterResMax = 100.0f;
        inline constexpr float filterResDefault = 0.0f;

        // Filter drive: 0-100%
        inline constexpr float filterDriveMin = 0.0f;
        inline constexpr float filterDriveMax = 100.0f;
        inline constexpr float filterDriveDefault = 0.0f;

        // Mode: 0=Cassette, 1=Vinyl, 2=VHS, 3=Radio
        inline constexpr int modeDefault = 0;

        // Mix: 0-100%
        inline constexpr float mixMin = 0.0f;
        inline constexpr float mixMax = 100.0f;
        inline constexpr float mixDefault = 100.0f;

        // Output: -24dB to +12dB
        inline constexpr float outputMin = -24.0f;
        inline constexpr float outputMax = 12.0f;
        inline constexpr float outputDefault = 0.0f;
    }
}
