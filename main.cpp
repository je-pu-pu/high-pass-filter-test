#include <iostream>
#include <vector>
#include <numbers>
#include <cmath>
#include <fstream>

#include <ranges>

#include <AL/alut.h>

#include <sndfile.h>

// ハイパスフィルタのクラス定義
class HighPassFilter {
public:
    HighPassFilter(float sampleRate, float cutoffFrequency) {
        setParameters(sampleRate, cutoffFrequency);
        prevInput = 0.0f;
        prevOutput = 0.0f;
    }

    // サンプリングレートとカットオフ周波数を設定
    void setParameters(float sampleRate, float cutoffFrequency) {
        float RC = 1.0f / (2.0f * 3.14159f * cutoffFrequency);
        alpha = RC / (RC + (1.0f / sampleRate));
    }

    // サンプルをフィルタリング
    float processSample(float inputSample) {
        float output = alpha * (prevOutput + inputSample - prevInput);
        prevInput = inputSample;
        prevOutput = output;
        return output;
    }

private:
    float alpha;       // フィルタ係数
    float prevInput;   // 直前の入力値
    float prevOutput;  // 直前の出力値
};

// 2次ハイパスフィルタのクラス定義
class SecondOrderHighPassFilter {
public:
    SecondOrderHighPassFilter(float sampleRate, float cutoffFrequency) {
        setParameters(sampleRate, cutoffFrequency);
        prevInput1 = prevInput2 = 0.0f;
        prevOutput1 = prevOutput2 = 0.0f;
    }

    // サンプリングレートとカットオフ周波数を設定
    void setParameters(float sampleRate, float cutoffFrequency) {
        float omega = 2.0f * std::numbers::pi * cutoffFrequency / sampleRate;
        float sin_omega = std::sin(omega);
        float cos_omega = std::cos(omega);
        float alpha = sin_omega / (2.0f * std::sqrt(2.0f)); // Q = sqrt(2)/2 for Butterworth filter

        b0 = (1.0f + cos_omega) / 2.0f;
        b1 = -(1.0f + cos_omega);
        b2 = (1.0f + cos_omega) / 2.0f;
        a0 = 1.0f + alpha;
        a1 = -2.0f * cos_omega;
        a2 = 1.0f - alpha;

        // Normalize the coefficients
        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= a0;
        a2 /= a0;
    }

    // サンプルをフィルタリング
    float processSample(float inputSample) {
        float output = b0 * inputSample + b1 * prevInput1 + b2 * prevInput2
                       - a1 * prevOutput1 - a2 * prevOutput2;

        prevInput2 = prevInput1;
        prevInput1 = inputSample;
        prevOutput2 = prevOutput1;
        prevOutput1 = output;

        return output;
    }

private:
    float b0, b1, b2;  // フィルタ係数
    float a0, a1, a2;  // フィルタ係数
    float prevInput1, prevInput2;   // 直前の入力値
    float prevOutput1, prevOutput2; // 直前の出力値
};

std::vector< float > make_input( float sampleRate )
{
    std::vector< float > v;

    for ( int n = 0; n < sampleRate / 60 * 5; n++ )
    {
        float a = std::sin( ( n / sampleRate ) * ( std::numbers::pi * 2 ) * 60.f ) * 0.5f;
        float b = std::sin( ( n / sampleRate ) * ( std::numbers::pi * 2 ) * 440.f ) * 0.1f;

        v.push_back( a + b );
    }

    return v;
}

void save_csv( std::vector< float > v, std::string filename )
{
    std::ofstream ofs( filename );

    for ( float f : v | std::ranges::views::take( 3600 ) )
    {
        ofs << f << std::endl;
    }
}

void save_wav( const std::vector< float >& signal, const std::string& filename, int sampleRate )
{
    SF_INFO sfinfo;
    sfinfo.channels = 1;
    sfinfo.samplerate = sampleRate;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    SNDFILE* outfile = sf_open( filename.c_str(), SFM_WRITE, & sfinfo );
    if (!outfile) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    sf_write_float( outfile, signal.data(), signal.size() );
    sf_close( outfile );
}

int main( int argc, char** argv )
{
    SF_INFO sfinfo;
    SNDFILE* infile = sf_open( "sample.wav", SFM_READ, & sfinfo );

    if ( ! infile )
    {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    std::vector< float > input( sfinfo.frames );
    sf_read_float( infile, input.data(), sfinfo.frames );
    sf_close( infile );

    /*
    alutInit( & argc, argv );

    ALuint source;
    ALuint buffer = alutCreateBufferFromFile( "./sample.wav" );

    alGenSources( 1, & source );
    alSourcei( source, AL_BUFFER, buffer );
    // alSourcei( source, AL_LOOPING, AL_TRUE );
    alSourcePlay( source );
    // alutSleep( 5 );

    getchar();

    alutExit();

    return 0;
    */

    // サンプリングレートとカットオフ周波数を設定
    float sampleRate = 44100.0f;  // 44.1kHz
    float cutoffFrequency = 3800.0f; // 200Hz



    // テスト用のサンプルデータ
    // std::vector<float> input = make_input( sampleRate );

    HighPassFilter filter( sampleRate, cutoffFrequency );
    std::vector<float> output;

    // 信号をフィルタリング
    for (float sample : input) {
        output.push_back( filter.processSample( sample ) );
    }

    HighPassFilter f2( sampleRate, 50.f );
    std::vector<float> output2;

    for (float sample : input) {
        output2.push_back( f2.processSample( sample ) );
    }

    SecondOrderHighPassFilter f3( sampleRate, cutoffFrequency );
    std::vector<float> output3;

    for (float sample : input) {
        output3.push_back( f3.processSample( sample ) );
    }

    save_csv( input, "input.csv" );
    save_csv( output, "output.csv" );
    save_csv( output2, "output2.csv" );
    save_csv( output3, "output3.csv" );

    save_wav( output, "output.wav", sfinfo.samplerate );
    save_wav( output2, "output2.wav", sfinfo.samplerate );
    save_wav( output3, "output3.wav", sfinfo.samplerate );

    return 0;
}
