################################################################################
# Package: MuonPrepRawDataProviderTools
################################################################################

# Declare the package name:
atlas_subdir( MuonPrepRawDataProviderTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonLayerEvent
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          PRIVATE
                          Event/EventPrimitives
                          MuonSpectrometer/MuonDetDescr/MuonDetDescrUtils
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces )

# Component(s) in the package:
atlas_add_library( MuonPrepRawDataProviderToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonPrepRawDataProviderTools
                   LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel MuonLayerEvent MuonPrepRawData MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES EventPrimitives MuonDetDescrUtils MuonReadoutGeometry MuonRIO_OnTrack MuonRecToolInterfaces )

atlas_add_component( MuonPrepRawDataProviderTools
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel MuonIdHelpersLib MuonLayerEvent MuonPrepRawData EventPrimitives MuonDetDescrUtils MuonReadoutGeometry MuonRIO_OnTrack MuonRecToolInterfaces MuonPrepRawDataProviderToolsLib )

