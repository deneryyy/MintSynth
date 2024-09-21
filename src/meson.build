pipewire_dep = dependency('libpipewire-0.3', version : '>=1.0.7')
quill_dep = dependency('quill')
deps = [pipewire_dep, quill_dep]

mint_synth = shared_library('mint_synth', 'mint_synth.cpp', dependencies : deps)
mint_project = shared_library('mint_project', 'mint_project.cpp', dependencies : deps)

executable('mint', 'main.cpp', dependencies : deps, 
  link_with : [
    mint_synth
  ])
