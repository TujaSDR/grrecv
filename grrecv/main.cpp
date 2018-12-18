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
#include <tujasdr/opus_encoder_f.h>
#include <tujasdr/fast_sine_source_c.h>
#include <tujasdr/unix_dgram_sink_b.h>
#include <tujasdr/tx_processor_cc.h>

#include <gnuradio/realtime.h>
#include <gnuradio/top_block.h>
#include <gnuradio/filter/pfb_arb_resampler_fff.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/blocks/complex_to_float.h>
#include <gnuradio/blocks/add_cc.h>
#include <gnuradio/blocks/multiply_const_cc.h>
#include <gnuradio/blocks/multiply_cc.h>

// #include <gnuradio/blocks/null_sink.h>

int main(int argc, const char * argv[]) {
    // insert code here...

    // Temporary fs
    double fs = 89286;

    int rt = gr::enable_realtime_scheduling();

    printf("rt = %d\n", rt);
    
    gr::top_block_sptr rx_block = gr::make_top_block("rx");

    // Tuja source/sink
    auto tuja_source = gr::tujasdr::alsa_source::make(fs, "hw:CARD=tujasdr,DEV=0");
    auto tuja_sink = gr::tujasdr::alsa_sink::make(fs, "hw:CARD=tujasdr,DEV=0");

    auto tx_processor_cc = gr::tujasdr::tx_processor_cc::make(fs);
    
    tuja_source->set_thread_priority(90);
    tuja_sink->set_thread_priority(90);

    int prio0 = tuja_source->thread_priority();
    int prio1 = tuja_sink->thread_priority();

    printf("Prio: %d\n", prio0);
    printf("Prio: %d\n", prio1);
    
    rx_block->connect(tuja_source, 0, tx_processor_cc, 0);
    rx_block->connect(tx_processor_cc, 0, tuja_sink, 0);
    
    //rx_block->connect(sine_source, 0, mix, 1);
    //rx_block->connect(mix, 0, tuja_sink, 0);
    
    // ssb
    /*auto ssb_rx = gr::tujasdr::ssb_rx::make(fs);
    
    auto add_cc = gr::blocks::add_cc::make();
    auto sine_source = gr::tujasdr::fast_sine_source_c::make(fs, 1000., 0.1);
    
    auto mc = gr::blocks::multiply_const_cc::make(1.0);
    
    rx_block->connect(tuja_source, 0, ssb_rx, 0);
    rx_block->connect(sine_source, 0, add_cc, 0);
    rx_block->connect(ssb_rx, 0, mc, 0);
    rx_block->connect(mc, 0, add_cc, 1);
    rx_block->connect(add_cc, 0, tuja_sink, 0);
    
    // wsjt-x specific
    auto complex_to_float = gr::blocks::complex_to_float::make();
    std::vector<float> qr = gr::filter::firdes::low_pass_2(1.0, fs, 6000.0, 100, 60);
    auto pfb_resampler = gr::filter::pfb_arb_resampler_fff::make(48000.0/fs, qr);
    auto mono_sink = gr::tujasdr::mono_sink::make(48000, "hw:CARD=Loopback,DEV=0");
    rx_block->connect(ssb_rx, 0, complex_to_float, 0);
    rx_block->connect(complex_to_float, 0, pfb_resampler, 0);
    rx_block->connect(pfb_resampler, 0, mono_sink, 0);

    // lets try opus
    auto opus_encoder  = gr::tujasdr::opus_encoder_f::make(48000);

    auto dgram_sink = gr::tujasdr::unix_dgram_sink_b::make("/tmp/tujaopusrx.sock");
    
    rx_block->connect(pfb_resampler, 0, opus_encoder, 0);
    rx_block->connect(opus_encoder, 0, dgram_sink, 0);
    */
    // max_nooutput_items

    rx_block->start();
    rx_block->wait();
    
    return 0;
}
