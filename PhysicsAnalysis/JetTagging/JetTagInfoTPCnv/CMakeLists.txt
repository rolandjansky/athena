################################################################################
# Package: JetTagInfoTPCnv
################################################################################

# Declare the package name:
atlas_subdir( JetTagInfoTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventPrimitives
                          GaudiKernel
                          PhysicsAnalysis/JetTagging/JetTagEvent
                          PhysicsAnalysis/JetTagging/JetTagInfo
                          PhysicsAnalysis/MuonID/MuonIDEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/Jet/JetEventTPCnv
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkEventCnv/TrkEventTPCnv
                          PRIVATE
                          Control/CxxUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( JetTagInfoTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS JetTagInfoTPCnv
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities EventPrimitives GaudiKernel JetTagEvent JetTagInfo MuonIDEvent JetEvent JetEventTPCnv Particle egammaEvent VxVertex TrkEventTPCnv DataModelAthenaPoolLib AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES CxxUtils )

atlas_add_dictionary( JetTagInfoTPCnvDict
                      JetTagInfoTPCnv/JetTagInfoTPCnvDict.h
                      JetTagInfoTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventPrimitives GaudiKernel JetTagEvent JetTagInfo MuonIDEvent JetEvent JetEventTPCnv Particle egammaEvent VxVertex TrkEventTPCnv CxxUtils JetTagInfoTPCnv )

atlas_add_dictionary( ARA_JetTagInfoTPCnvDict
                      JetTagInfoTPCnv/JetTagInfoTPCnvDict.h
                      JetTagInfoTPCnv/ARA_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventPrimitives GaudiKernel JetTagEvent JetTagInfo MuonIDEvent JetEvent JetEventTPCnv Particle egammaEvent VxVertex TrkEventTPCnv CxxUtils JetTagInfoTPCnv )

