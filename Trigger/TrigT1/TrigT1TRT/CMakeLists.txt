################################################################################
# Package: TrigT1TRT
################################################################################

# Declare the package name:
atlas_subdir( TrigT1TRT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetRawData
                          PRIVATE
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigT1/TrigT1Interfaces )

# Component(s) in the package:
atlas_add_library( TrigT1TRTLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1TRT
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetReadoutGeometry InDetRawData TRT_ConditionsServicesLib
                   PRIVATE_LINK_LIBRARIES TrigConfL1Data TrigT1Interfaces )

atlas_add_component( TrigT1TRT
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetReadoutGeometry InDetRawData TRT_ConditionsServicesLib TrigConfL1Data TrigT1Interfaces TrigT1TRTLib )

