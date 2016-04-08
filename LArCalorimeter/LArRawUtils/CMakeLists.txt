################################################################################
# Package: LArRawUtils
################################################################################

# Declare the package name:
atlas_subdir( LArRawUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloTTDetDescr
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawEvent
                          PRIVATE
                          Calorimeter/CaloTriggerTool
                          DetectorDescription/AtlasDetDescr
                          LArCalorimeter/LArTools )

# Component(s) in the package:
atlas_add_library( LArRawUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS LArRawUtils
                   LINK_LIBRARIES CaloIdentifier CaloTTDetDescr AthenaBaseComps AthenaKernel Identifier GaudiKernel LArIdentifier LArRawEvent StoreGateLib SGtests CaloTriggerToolLib LArToolsLib
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr )

atlas_add_component( LArRawUtils
                     src/components/*.cxx
                     LINK_LIBRARIES CaloIdentifier CaloTTDetDescr AthenaBaseComps AthenaKernel StoreGateLib SGtests Identifier GaudiKernel LArIdentifier LArRawEvent CaloTriggerToolLib AtlasDetDescr LArToolsLib LArRawUtilsLib )

