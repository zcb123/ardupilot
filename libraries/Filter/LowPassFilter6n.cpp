#include "LowPassFilter6n.h"


////////////////////////////////////////////////////////////////////////////////////////////
// DigitalSixOrderFilter
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
DigitalSixOrderFilter<T>::DigitalSixOrderFilter() {
    for(int i = 0 ;i<7;i++){
        xBuf[i] = T();
        yBuf[i] = T();
    }
}

template <class T>
T DigitalSixOrderFilter<T>::apply(const T &sample, const struct sixorder_params &params) {
    
    for(int i = 6; i>0; i--)
    {
        yBuf[i] = yBuf[i-1];
        xBuf[i] = xBuf[i-1];
    }

    xBuf[0] = sample;
    yBuf[0] = T();

    for(int i=1;i<7;i++)
    {
        yBuf[0] = yBuf[0] + xBuf[i]*params.b[i];
        yBuf[0] = yBuf[0] - yBuf[i]*params.a[i];
    }
    
    yBuf[0] = yBuf[0] + xBuf[0]*params.b[0];

    return yBuf[0];
    
}

template <class T>
void DigitalSixOrderFilter<T>::reset() { 
    
    for(int i = 0 ;i<7;i++){
        xBuf[i] = T();
        yBuf[i] = T();
    }
    initialised = false;
}

template <class T>
void DigitalSixOrderFilter<T>::reset(const T &value, const struct sixorder_params &params) {
    
    initialised = true;
}

template <class T>
void DigitalSixOrderFilter<T>::compute_params(float sample_freq, float cutoff_freq, sixorder_params &ret) {
    ret.cutoff_freq = cutoff_freq;              //暂时没用
    ret.sample_freq = sample_freq;              //暂时没用
    
    double temp_a[7] = {1,-5.81793057376491,14.1061535298373,-18.2443651768826,13.2755158076669,-5.1528964847001,0.833522907852272};
    double temp_b[7] = {0.000000000156389596618205,0.000000000938337579709231,0.00000000234584394927308,0.0000000031277919323641,0.00000000234584394927308,
        0.000000000938337579709231,0.000000000156389596618205};
    
    for(int i = 0;i<7;i++){
        ret.a[i] = temp_a[i];
        ret.b[i] = temp_b[i];
    }
}


////////////////////////////////////////////////////////////////////////////////////////////
// LowPassFilter6n
////////////////////////////////////////////////////////////////////////////////////////////

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

// change parameters
template <class T>
void LowPassFilter6n<T>::set_cutoff_frequency(float sample_freq, float cutoff_freq) {
    DigitalSixOrderFilter<T>::compute_params(sample_freq, cutoff_freq, _params);
}

// return the cutoff frequency
template <class T>
float LowPassFilter6n<T>::get_cutoff_freq(void) const {
    return _params.cutoff_freq;
}

template <class T>
float LowPassFilter6n<T>::get_sample_freq(void) const {
    return _params.sample_freq;
}

template <class T>
T LowPassFilter6n<T>::apply(const T &sample) {
    //暂时未用
    // if (!is_positive(_params.cutoff_freq)) {
    //     // zero cutoff means pass-thru
    //     return sample;
    // }
    return _filter.apply(sample, _params);
}

template <class T>
void LowPassFilter6n<T>::reset(void) {
    return _filter.reset();
}

template <class T>
void LowPassFilter6n<T>::reset(const T &value) {
    return _filter.reset(value, _params);
}

/* 
 * Make an instances
 * Otherwise we have to move the constructor implementations to the header file :P
 */
template class LowPassFilter6n<int>;
template class LowPassFilter6n<long>;
template class LowPassFilter6n<float>;
template class LowPassFilter6n<Vector2f>;
template class LowPassFilter6n<Vector3f>;
