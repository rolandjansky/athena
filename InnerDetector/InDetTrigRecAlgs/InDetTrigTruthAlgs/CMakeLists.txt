################################################################################
# Package: InDetTrigTruthAlgs
################################################################################

# Declare the package name:
atlas_subdir( InDetTrigTruthAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetSimData
                          Tracking/TrkEvent/TrkTruthData
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/AthLinks
                          Control/StoreGate
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetTruth/InDetTruthInterfaces
                          Reconstruction/Particle
                          Reconstruction/ParticleTruth
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigEvent/TrigParticle )

# Component(s) in the package:
atlas_add_component( InDetTrigTruthAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel InDetSimData TrkTruthData TrigInterfacesLib AthLinks StoreGateLib SGtests InDetPrepRawData Particle ParticleTruth TrkTrack TrkToolInterfaces TrigParticle )

# Install files from the package:
atlas_install_headers( InDetTrigTruthAlgs )
atlas_install_python_modules( python/*.py )

