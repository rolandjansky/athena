################################################################################
# Package: TrigExPartialEB
################################################################################

# Declare the package name:
atlas_subdir( TrigExPartialEB )

# Component(s) in the package:
atlas_add_component( TrigExPartialEB
                     src/*.cxx
                     src/components/*.cxx
                     PRIVATE_LINK_LIBRARIES GaudiKernel AthenaBaseComps
                     AthenaKernel ByteStreamCnvSvcBaseLib DecisionHandlingLib TrigCompositeUtilsLib 
                     TrigPartialEventBuildingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
