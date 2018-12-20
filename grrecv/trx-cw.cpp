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

// Gnuradio
#include <gnuradio/realtime.h>
#include <gnuradio/top_block.h>

int main(int argc, const char * argv[]) {
    // Our sample rate
    float sample_rate = 89285.71428;
    // float sample_rate = 20./32./7.;
    
    // Check that we can get real time scheduling
    int rt = gr::enable_realtime_scheduling();
    assert(rt == 0);

    // Top block
    gr::top_block_sptr tb = gr::make_top_block("rx");
    
    int block_size = 1024;
    
    // Tuja source/sink
    auto tuja_source = gr::tujasdr::alsa_source::make(ceilf(sample_rate),
                                                      "hw:CARD=tujasdr,DEV=0",
                                                      block_size);
    
    auto tuja_sink = gr::tujasdr::alsa_sink::make(ceilf(sample_rate),
                                                  "hw:CARD=tujasdr,DEV=0",
                                                  block_size);
    
    // Elevated priority for these IO threads
    tuja_source->set_thread_priority(50);
    tuja_sink->set_thread_priority(50);

    tuja_source->set_mode(gr::tujasdr::KEY_COMPLEX_SINE);
    
    //auto sig_source_c = gr::analog::sig_source_c::make(sample_rate,gr::analog::GR_SIN_WAVE , 800, 0.8);
    
    tb->connect(tuja_source, 0, tuja_sink, 0);
    
    tb->start();
    tb->wait();
    
    return 0;
}

