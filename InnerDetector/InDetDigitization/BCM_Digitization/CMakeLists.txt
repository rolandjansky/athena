################################################################################
# Package: BCM_Digitization
################################################################################

# Declare the package name:
atlas_subdir( BCM_Digitization )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( BCM_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} GaudiKernel AthenaBaseComps AthenaKernel PileUpToolsLib xAODEventInfo GeneratorObjects InDetBCM_RawData InDetSimData InDetSimEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py test/*.py )

