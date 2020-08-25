################################################################################
# Package: LArMonTools
################################################################################

# Declare the package name:
atlas_subdir( LArMonTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( LArMonTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} CaloDetDescrLib CaloGeoHelpers CaloIdentifier AthenaMonitoringLib StoreGateLib SGtests Identifier GaudiKernel LArIdentifier LArRawEvent LArRecConditions LArCablingLib LWHists TrigDecisionToolLib CaloConditions AthenaKernel AthenaPoolUtilities xAODEventInfo LArCOOLConditions LArRawConditions LArRecEvent CaloInterfaceLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.txt share/*.py )

