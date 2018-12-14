//
//  main.cpp
//  grrecv
//
//  Created by Albin Stigö on 2018-12-05.
//  Copyright © 2018 Albin Stigo. All rights reserved.
//

#include <iostream>

#include <tujasdr/alsa_sink.h>
#include <tujasdr/alsa_source.h>
#include <tujasdr/mono_sink.h>
#include <tujasdr/ssb_rx.h>

#include <gnuradio/top_block.h>
#include <gnuradio/filter/pfb_arb_resampler_fff.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/blocks/complex_to_float.h>

int main(int argc, const char * argv[]) {
    // insert code here...

    // Temporary fs
    double fs = 89286;
    
    gr::top_block_sptr rx_block = gr::make_top_block("rx");
    
    // Tuja source/sink
    auto tuja_source = gr::tujasdr::alsa_source::make(fs, "hw:CARD=tujasdr,DEV=0");
    auto tuja_sink = gr::tujasdr::alsa_sink::make(fs, "hw:CARD=tujasdr,DEV=0");
    // ssb
    auto ssb_rx = gr::tujasdr::ssb_rx::make(fs);
    
    rx_block->connect(tuja_source, 0, ssb_rx, 0);
    rx_block->connect(ssb_rx, 0, tuja_sink, 0);
    
    // wsjt-x specific
    auto complex_to_float = gr::blocks::complex_to_float::make();
    std::vector<float> qr = gr::filter::firdes::low_pass_2(1.0, fs, 6000.0, 100, 60);
    auto pfb_resampler = gr::filter::pfb_arb_resampler_fff::make(48000.0/fs, qr);
    auto mono_sink = gr::tujasdr::mono_sink::make(48000, "hw:CARD=Loopback,DEV=0");
    rx_block->connect(ssb_rx, 0, complex_to_float, 0);
    rx_block->connect(complex_to_float, 0, pfb_resampler, 0);
    rx_block->connect(pfb_resampler, 0, mono_sink, 0);
    
    // max_nooutput_items
    rx_block->start();
    rx_block->wait();
    
    return 0;
}

/*
 std::vector<float> qr = gr::filter::firdes::low_pass_2(1.0, fs, 6000.0, 100, 60);
 auto resampler = gr::filter::pfb_arb_resampler_fff::make(48000.0/fs, qr);
 auto loop_sink = gr::tujasdr::mono_sink::make(48000, "hw:CARD=Loopback,DEV=0");
 */
