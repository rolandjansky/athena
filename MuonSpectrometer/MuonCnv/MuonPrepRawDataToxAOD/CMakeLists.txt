################################################################################
# Package: MuonPrepRawDataToxAOD
################################################################################

# Declare the package name:
atlas_subdir( MuonPrepRawDataToxAOD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonSimData
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkTools/TrkToolInterfaces 
                          Generators/AtlasHepMC )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( MuonPrepRawDataToxAOD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AtlasHepMCLib GaudiKernel AthenaBaseComps GeoPrimitives Identifier EventPrimitives xAODTracking MuonIdHelpersLib MuonRDO MuonPrepRawData MuonRIO_OnTrack MuonRecToolInterfaces MuonSimData TrkEventPrimitives TrkToolInterfaces )

