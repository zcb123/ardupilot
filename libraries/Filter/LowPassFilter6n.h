/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <AP_Math/AP_Math.h>
#include <cmath>
#include <inttypes.h>

#define FILTER_ORDER 4
/// @file   LowPassFilter6n.h
/// @brief  A class to implement a sixth order low pass filter
/// @authors: zhangchenbin <zcb2048@163.com>, template implmentation: zhangchenbin <zcb2048@163.com>
template <class T>
class DigitalSixOrderFilter {
public:
    struct sixorder_params {
        float cutoff_freq;
        float sample_freq;
        double a[FILTER_ORDER];  
        double b[FILTER_ORDER];  
    };

    CLASS_NO_COPY(DigitalSixOrderFilter);

    DigitalSixOrderFilter();

    T apply(const T &sample, const struct sixorder_params &params);
    void reset();
    void reset(const T &value, const struct sixorder_params &params);
    static void compute_params(float sample_freq, float cutoff_freq, sixorder_params &ret);
    
private:
    T xBuf[FILTER_ORDER];
    T yBuf[FILTER_ORDER];
    bool initialised;
};

template <class T>
class LowPassFilter6n {
public:
    LowPassFilter6n();
    // constructor
    LowPassFilter6n(float sample_freq, float cutoff_freq);
    // change parameters
    void set_cutoff_frequency(float sample_freq, float cutoff_freq);
    // return the cutoff frequency
    float get_cutoff_freq(void) const;
    float get_sample_freq(void) const;
    T apply(const T &sample);
    void reset(void);
    void reset(const T &value);

    CLASS_NO_COPY(LowPassFilter6n);

protected:
    struct DigitalSixOrderFilter<T>::sixorder_params _params;
    
private:
    DigitalSixOrderFilter<T> _filter;
};

// Uncomment this, if you decide to remove the instantiations in the implementation file
/*
template <class T>
LowPassFilter6n<T>::LowPassFilter6n() { 
    memset(&_params, 0, sizeof(_params) ); 
}

// constructor
template <class T>
LowPassFilter6n<T>::LowPassFilter6n(float sample_freq, float cutoff_freq) {
    // set initial parameters
    set_cutoff_frequency(sample_freq, cutoff_freq);
}
*/

typedef LowPassFilter6n<int>      LowPassFilter6nInt;
typedef LowPassFilter6n<long>     LowPassFilter6nLong;
typedef LowPassFilter6n<float>    LowPassFilter6nFloat;
typedef LowPassFilter6n<double>    LowPassFilter6nDouble;
typedef LowPassFilter6n<Vector2f> LowPassFilter6nVector2f;
typedef LowPassFilter6n<Vector3f> LowPassFilter6nVector3f;
typedef LowPassFilter6n<Vector3d> LowPassFilter6nVector3d;
