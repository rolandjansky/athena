################################################################################
# Package: MuonCombinedEventStoreAlgs
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedEventStoreAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloUtils
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Reconstruction/MuonIdentification/muonEvent
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( MuonCombinedEventStoreAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloEvent CaloUtilsLib AthenaBaseComps AthenaKernel xAODCaloEvent GaudiKernel MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces muonEvent TrkGeometry TrkParameters TrkTrack TrkExInterfaces TrkToolInterfaces )

