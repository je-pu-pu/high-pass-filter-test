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

    return 0;
}
