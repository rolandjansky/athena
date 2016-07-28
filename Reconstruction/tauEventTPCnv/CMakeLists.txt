################################################################################
# Package: tauEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( tauEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/RootConversions
                          Event/EventCommonTPCnv
                          PhysicsAnalysis/AnalysisCommon/ParticleEventTPCnv
                          Reconstruction/tauEvent
                          Tracking/TrkEventCnv/TrkEventTPCnv
                          PRIVATE
                          Control/AthenaKernel )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( tauEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS tauEventTPCnv
                         INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions EventCommonTPCnv ParticleEventTPCnv tauEvent TrkEventTPCnv AthenaKernel )

atlas_add_dictionary( tauEventTPCnvDict
                      tauEventTPCnv/tauEventTPCnvDict.h
                      tauEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions EventCommonTPCnv ParticleEventTPCnv tauEvent TrkEventTPCnv AthenaKernel tauEventTPCnv )

