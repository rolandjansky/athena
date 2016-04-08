################################################################################
# Package: InDetTruthAlgs
################################################################################

# Declare the package name:
atlas_subdir( InDetTruthAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetTruth/InDetTruthInterfaces
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Control/DataModel
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Reconstruction/Particle
                          Reconstruction/ParticleTruth
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( InDetTruthAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkTruthData TrkToolInterfaces DataModel InDetSimData InDetPrepRawData Particle ParticleTruth TrkTrack )

# Install files from the package:
atlas_install_headers( InDetTruthAlgs )

