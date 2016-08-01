################################################################################
# Package: ReweightUtils
################################################################################

# Declare the package name:
atlas_subdir( ReweightUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthAnalysisBaseComps
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODCutFlow
                          Event/xAOD/xAODEventInfo
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          PRIVATE
                          Event/xAOD/xAODParticleEvent
                          Event/xAOD/xAODTruth
                          GaudiKernel )

# External dependencies:
find_package( Lhapdf )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ReweightUtilsLib
                   src/*.cxx
                   src/components/*.cxx
                   PUBLIC_HEADERS ReweightUtils
                   INCLUDE_DIRS ${LHAPDF_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${LHAPDF_LIBRARIES} AthAnalysisBaseCompsLib AsgTools xAODCutFlow xAODEventInfo PATInterfaces
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} xAODParticleEvent xAODTruth GaudiKernel )

atlas_add_component( ReweightUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${LHAPDF_LIBRARIES} AthAnalysisBaseCompsLib AsgTools xAODCutFlow xAODEventInfo PATInterfaces xAODParticleEvent xAODTruth GaudiKernel ReweightUtilsLib )

atlas_add_dictionary( ReweightUtilsDict
                      ReweightUtils/ReweightUtilsDict.h
                      ReweightUtils/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${LHAPDF_LIBRARIES} AthAnalysisBaseCompsLib AsgTools xAODCutFlow xAODEventInfo PATInterfaces xAODParticleEvent xAODTruth GaudiKernel ReweightUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

