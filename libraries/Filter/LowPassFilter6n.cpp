#include "LowPassFilter6n.h"
//#include <GCS_MAVLink/GCS.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DigitalSixOrderFilter
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
DigitalSixOrderFilter<T>::DigitalSixOrderFilter() {
    for(int i = 0 ;i<FILTER_ORDER;i++){
        xBuf[i] = T();
        yBuf[i] = T();
    }
}

template <class T>
T DigitalSixOrderFilter<T>::apply(const T &sample, const struct sixorder_params &params) {
    
    for(int i = FILTER_ORDER - 1; i>0; i--)
    {
        yBuf[i] = yBuf[i-1];
        xBuf[i] = xBuf[i-1];
    }

    xBuf[0] = sample;
    yBuf[0] = T();

    for(int i=1;i<FILTER_ORDER;i++)
    {
        yBuf[0] = yBuf[0] + xBuf[i]*params.b[i];
        yBuf[0] = yBuf[0] - yBuf[i]*params.a[i];
    }
    
    yBuf[0] = yBuf[0] + xBuf[0]*params.b[0];

    //gcs().send_text(MAV_SEVERITY_CRITICAL, "filter! %.15lf ", (double)params.b[0]);

    return yBuf[0];
    
}

template <class T>
void DigitalSixOrderFilter<T>::reset() { 
    
    for(int i = 0 ;i<FILTER_ORDER;i++){
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
    
    double temp_a[FILTER_ORDER] = {1,-2.95936119553685,2.91954395361978,-0.96017453612764};
    double temp_b[FILTER_ORDER] = {0.00000102774441178284,0.00000308323323534852,0.00000308323323534852,0.00000102774441178284};
    
    for(int i = 0;i<FILTER_ORDER;i++){
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
    set_cutoff_frequency(20, 30);
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
template class LowPassFilter6n<Vector3d>;