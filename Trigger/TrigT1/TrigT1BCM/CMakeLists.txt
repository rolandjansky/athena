################################################################################
# Package: TrigT1BCM
################################################################################

# Declare the package name:
atlas_subdir( TrigT1BCM )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetBCM_RawData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          PRIVATE
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigT1/TrigT1Interfaces )

# Component(s) in the package:
atlas_add_library( TrigT1BCMLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1BCM
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetBCM_RawData
                   PRIVATE_LINK_LIBRARIES TrigConfL1Data TrigT1Interfaces )

atlas_add_component( TrigT1BCM
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetBCM_RawData TrigConfL1Data TrigT1Interfaces TrigT1BCMLib )

