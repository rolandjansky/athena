
################################################################################
# Package: TRTMonitoringRun3
################################################################################

# Declare the package name:
atlas_subdir( TRTMonitoringRun3 )
                  
atlas_add_component( TRTMonitoringRun3
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES AthenaMonitoringLib TrkToolInterfaces CommissionEvent TRT_DriftFunctionToolLib TRT_ConditionsServicesLib MagFieldInterfaces InDetByteStreamErrors InDetRIO_OnTrack
                     )
                   
# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

