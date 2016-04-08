################################################################################
# Package: JetEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( JetEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventCommonTPCnv
                          PhysicsAnalysis/AnalysisCommon/ParticleEventTPCnv
                          Reconstruction/Jet/JetEvent
                          PRIVATE
                          Control/AthenaKernel
                          Control/DataModel
                          PhysicsAnalysis/MuonID/MuonIDEvent
                          Reconstruction/egamma/egammaEvent
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( JetEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS JetEventTPCnv
                         INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv ParticleEventTPCnv JetEvent AthenaKernel DataModel MuonIDEvent egammaEvent VxVertex )

atlas_add_dictionary( JetEventTPCnvDict
                      JetEventTPCnv/JetEventTPCnvDict.h
                      JetEventTPCnv/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv ParticleEventTPCnv JetEvent AthenaKernel DataModel MuonIDEvent egammaEvent VxVertex JetEventTPCnv )

atlas_add_dictionary( OLD_JetEventTPCnvDict
                      JetEventTPCnv/JetEventTPCnvDict.h
                      JetEventTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv ParticleEventTPCnv JetEvent AthenaKernel DataModel MuonIDEvent egammaEvent VxVertex JetEventTPCnv )

