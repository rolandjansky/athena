################################################################################
# Package: TrigExPartialEB
################################################################################

# Declare the package name:
atlas_subdir( TrigExPartialEB )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/ByteStreamCnvSvcBase
                          Trigger/TrigSteer/DecisionHandling
                          Trigger/TrigAlgorithms/TrigPartialEventBuilding )

# Component(s) in the package:
atlas_add_component( TrigExPartialEB
                     src/*.cxx
                     src/components/*.cxx
                     PRIVATE_LINK_LIBRARIES GaudiKernel AthenaBaseComps
                     AthenaKernel ByteStreamCnvSvcBaseLib DecisionHandlingLib
                     TrigPartialEventBuildingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
