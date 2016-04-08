################################################################################
# Package: InDetVertexSplitter
################################################################################

# Declare the package name:
atlas_subdir( InDetVertexSplitter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          Generators/GeneratorObjects
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( InDetVertexSplitter
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODEventInfo GaudiKernel GeneratorObjects Particle TrkParticleBase TrkTrack VxVertex )

# Install files from the package:
atlas_install_joboptions( share/*.py )

