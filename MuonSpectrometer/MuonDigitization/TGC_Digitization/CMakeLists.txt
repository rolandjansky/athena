################################################################################
# Package: TGC_Digitization
################################################################################

# Declare the package name:
atlas_subdir( TGC_Digitization )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( TGC_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib GeoPrimitives Identifier GaudiKernel MuonSimEvent HitManagement xAODEventInfo GeneratorObjects MuonCondData MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonSimData PathResolver RDBAccessSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.dat )

