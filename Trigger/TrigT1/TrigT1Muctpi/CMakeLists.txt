################################################################################
# Package: TrigT1Muctpi
################################################################################

# Declare the package name:
atlas_subdir( TrigT1Muctpi )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PRIVATE
                          Control/AthenaKernel
                          GaudiKernel
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Tools/PathResolver
                          Control/CxxUtils
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigConfiguration/TrigConfMuctpi
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( TrigT1Muctpi
                     src/Algorithms/*.cxx
                     src/Common/*.cxx
                     src/Logging/*.cxx
                     src/Mibak/*.cxx
                     src/Mictp/*.cxx
                     src/Mioct/*.cxx
                     src/Mirod/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${XERCESC_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel AnalysisTriggerEvent PathResolver TrigConfL1Data TrigConfMuctpi TrigT1Interfaces TrigT1Result )

# Install files from the package:
atlas_install_headers( TrigT1Muctpi )
atlas_install_python_modules( python/*.py )

