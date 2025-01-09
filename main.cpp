#include <iostream>
#include <vector>
#include <numbers>
#include <cmath>
#include <fstream>

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

    for ( float f : v )
    {
        ofs << f << std::endl;
    }
}

int main() {
    // サンプリングレートとカットオフ周波数を設定
    float sampleRate = 44100.0f;  // 44.1kHz
    float cutoffFrequency = 200.0f; // 200Hz

    HighPassFilter filter(sampleRate, cutoffFrequency);

    // テスト用のサンプルデータ
    std::vector<float> inputSignal = make_input( sampleRate );
    std::vector<float> outputSignal;

    // 信号をフィルタリング
    for (float sample : inputSignal) {
        float filteredSample = filter.processSample(sample);
        outputSignal.push_back(filteredSample);
    }

    HighPassFilter f2( sampleRate, 50.f );
    std::vector<float> outputSignal2;

    for (float sample : inputSignal) {
        float filteredSample = f2.processSample(sample);
        outputSignal2.push_back(filteredSample);
    }

    SecondOrderHighPassFilter f3( sampleRate, 200.f );
    std::vector<float> output3;

    for (float sample : inputSignal) {
        output3.push_back( f3.processSample( sample ) );
    }

    // 結果を出力
    /*
    std::cout << "Filtered Signal: ";
    for (float sample : outputSignal) {
        std::cout << sample << " ";
    }
    std::cout << std::endl;
    */

    save_csv( inputSignal, "input.csv" );
    save_csv( outputSignal, "output.csv" );
    save_csv( outputSignal2, "output2.csv" );
    save_csv( output3, "output3.csv" );

    return 0;
}
