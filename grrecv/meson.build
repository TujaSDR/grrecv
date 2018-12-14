project('recv', 'cpp',
    default_options : [ 'warning_level=3',
    'buildtype=debugoptimized' ],
    version : '0.0.1',
    license : 'GPL')

cpp = meson.get_compiler('cpp')

m_dep = cpp.find_library('m')
boost_dep = dependency('boost', modules : ['system'])

gr_tujasdr_dep = cpp.find_library('gnuradio-tujasdr')
alsa_dep = dependency('alsa')

gr_runtime_dep = dependency('gnuradio-runtime')
# gr_tujasdr_dep = dependency('gnuradio-tujasdr')
gr_blocks_dep = dependency('gnuradio-blocks')
# gr_audio_dep = dependency('gnuradio-audio')
gr_analog_dep = dependency('gnuradio-analog')
gr_filter_dep = dependency('gnuradio-filter')

gnuradio_dep = [gr_runtime_dep, gr_tujasdr_dep, gr_blocks_dep,
    gr_analog_dep, gr_filter_dep]

# alsa_dep = dependency('alsa')
# liquid_dep = cc.find_library('liquid')
# dl_dep = cc.find_library('dl')
# threads_dep = dependency('threads')
# gpiod_dep = dependency('libgpiod')
# rt_dep = cc.find_library('rt')

recv_sources = ['main.cpp',]

executable('grrecv',
    recv_sources,
    dependencies: [boost_dep, alsa_dep, gnuradio_dep])
