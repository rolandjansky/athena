################################################################################
# Package: ZdcRec
################################################################################
  
# Declare the package name:
atlas_subdir( ZdcRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Event/xAOD/xAODForward
                          Event/xAOD/xAODTrigL1Calo
                          ForwardDetectors/ZDC/ZdcEvent
			  ForwardDetectors/ZDC/ZdcAnalysis
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          ForwardDetectors/ZDC/ZdcCnv/ZdcByteStream
                          ForwardDetectors/ZDC/ZdcConditions
                          ForwardDetectors/ZDC/ZdcIdentifier) 


# External dependencies:
find_package( GSL )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( ZdcRecLib
                   src/*.cxx
                   PUBLIC_HEADERS ZdcRec
                   INCLUDE_DIRS ${GSL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GSL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AsgTools AthenaBaseComps xAODForward xAODTrigL1Calo ZdcEvent GaudiKernel StoreGateLib SGtests ZdcByteStreamLib ZdcAnalysisLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ZdcConditions ZdcIdentifier )

atlas_add_component( ZdcRec
                     src/components/*.cxx
                     INCLUDE_DIRS ${GSL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GSL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthenaBaseComps xAODForward xAODTrigL1Calo ZdcEvent GaudiKernel StoreGateLib SGtests ZdcByteStreamLib ZdcConditions ZdcIdentifier ZdcRecLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
