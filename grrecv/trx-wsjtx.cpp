//
//  main.cpp
//  grrecv
//
//  Created by Albin Stigö on 2018-12-05.
//  Copyright © 2018 Albin Stigo. All rights reserved.
//

//#include <iostream>

// Tuja specific
#include <tujasdr/alsa_source.h>
#include <tujasdr/alsa_sink.h>
#include <tujasdr/mono_sink.h>
#include <tujasdr/add_real_imag_cc.h>
#include <tujasdr/agc_cc.h>

// Gnuradio
#include <gnuradio/filter/fft_filter_ccc.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/analog/sig_source_f.h>
#include <gnuradio/realtime.h>
#include <gnuradio/top_block.h>
#include <gnuradio/filter/pfb_arb_resampler_fff.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/blocks/complex_to_float.h>
#include <gnuradio/blocks/multiply_const_cc.h>
#include <gnuradio/blocks/multiply_const_ff.h>

int main(int argc, const char * argv[]) {
    // Our sample rate
    float sample_rate = 89285.71428;
    // float sample_rate = 20./32./7.;
    
    // Check that we can get real time scheduling
    int rt = gr::enable_realtime_scheduling();
    assert(rt == 0);

    // Top block
    gr::top_block_sptr tb = gr::make_top_block("rx");
    
    // Set up complex filter taps
    std::vector<gr_complex> Q = gr::filter::firdes::complex_band_pass(1.0,
                                                                      sample_rate, 300.,
                                                                      3300.,
                                                                      250.0);
    // FFT convolution filter
    auto fft_filter = gr::filter::fft_filter_ccc::make(1, Q);
    
    // Block size of FFT filter. We adapt ALSA buffers after this.
    int block_size = fft_filter->output_multiple();
    // printf("block size = %d\n", block_size);
    
    // Tuja source/sink
    auto tuja_source = gr::tujasdr::alsa_source::make(ceilf(sample_rate),
                                                      "hw:CARD=tujasdr,DEV=0",
                                                      block_size);

    auto tuja_sink = gr::tujasdr::alsa_sink::make(ceilf(sample_rate),
                                                  "hw:CARD=tujasdr,DEV=0",
                                                  block_size);
    // Elevated priority for these IO threads
    tuja_source->set_thread_priority(90);
    tuja_sink->set_thread_priority(90);
    
    // Polyphase resampler block
    // We probably don't even need a filter since we are already
    // well bellow the nyquist rate.

    // It's faster to decimate by 2 and then resample than to resample at higher rate.
    // auto decimate_ff = gr::tujasdr::decimate_ff::make(decim);
    std::vector<float> q = gr::filter::firdes::low_pass_2(1.0, sample_rate, 3000.0, 250, 60);
    auto pfb_resampler = gr::filter::pfb_arb_resampler_fff::make(48000.0/sample_rate, q);
    
    // I = I + Q, Q = 0
    auto add_real_imag = gr::tujasdr::add_real_imag_cc::make();
    // Complex to float
    auto complex_to_float = gr::blocks::complex_to_float::make();
    // ALSA loopback sink
    auto mono_sink = gr::tujasdr::mono_sink::make(48000, "hw:CARD=Loopback,DEV=0");
    mono_sink->set_thread_priority(50);

    auto gain_cc = gr::blocks::multiply_const_cc::make(gr_complex(30, 0));
    auto gain_ff = gr::blocks::multiply_const_ff::make(1000);

    auto agc_cc = gr::tujasdr::agc_cc::make(0.5, 0.001, 0.3, 1.0, 65536.);
    
    tb->connect(tuja_source, 0, fft_filter, 0);
    tb->connect(fft_filter, 0, agc_cc, 0);
    tb->connect(agc_cc, 0, add_real_imag, 0);
    tb->connect(add_real_imag, 0, tuja_sink, 0);
    
    tb->connect(add_real_imag, 0, complex_to_float, 0);
    tb->connect(complex_to_float, 0, pfb_resampler, 0);
    tb->connect(pfb_resampler, 0, mono_sink, 0);

    tb->start();
    tb->wait();
    
    return 0;
}
