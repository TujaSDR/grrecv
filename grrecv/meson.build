project('recv', 'cpp',
    default_options : ['c_std=c11',
    'cpp_std=c++11',
    'warning_level=3',
    'buildtype=debugoptimized',],
    version : '0.0.1',
    license : 'GPL')

cpp = meson.get_compiler('cpp')
#cc = meson.get_compiler('c')

# TODO: not sure why this is still needed
boost_dep = dependency('boost', modules : ['system'])

gr_tujasdr_dep = cpp.find_library('gnuradio-tujasdr')
gr_runtime_dep = dependency('gnuradio-runtime')
gr_blocks_dep = dependency('gnuradio-blocks')
gr_analog_dep = dependency('gnuradio-analog')
gr_filter_dep = dependency('gnuradio-filter')

gnuradio_dep = [gr_runtime_dep, gr_tujasdr_dep, gr_blocks_dep,
    gr_analog_dep, gr_filter_dep, boost_dep]

grrecv_sources = ['main.cpp',]

executable('grrecv',
    grrecv_sources,
    dependencies: [gnuradio_dep,])
