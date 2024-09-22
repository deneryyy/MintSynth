pipewire_dep = dependency('libpipewire-0.3', version : '>=1.0.7')
quill_dep = dependency('quill')
libavcodec_dep = dependency('libavcodec')
deps = [pipewire_dep, quill_dep, libavcodec_dep]

mint_module = shared_library('mint_module', 'mint_module.cpp', dependencies : deps)
mint_project = shared_library('mint_project', 'mint_project.cpp', dependencies : deps, link_with : [
  mint_module
])
mint_synth = shared_library('mint_synth', 'mint_synth.cpp', dependencies : deps, link_with : [
  mint_project,
  mint_module
])

executable('mint', 'main.cpp', dependencies : deps, 
  link_with : [
    mint_synth,
    mint_project
  ])
