################################################################################
# Package: egammaEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( egammaEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/EventCommonTPCnv
                          PhysicsAnalysis/AnalysisCommon/ParticleEventTPCnv
                          Reconstruction/egamma/egammaEvent
                          PRIVATE
                          Control/AthenaKernel
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( egammaEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS egammaEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv ParticleEventTPCnv egammaEvent AthenaKernel GaudiKernel )

atlas_add_dictionary( egammaEventTPCnvDict
                      egammaEventTPCnv/egammaEventTPCnvDict.h
                      egammaEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv ParticleEventTPCnv egammaEvent AthenaKernel GaudiKernel egammaEventTPCnv )

atlas_add_dictionary( OLD_egammaEventTPCnvDict
                      egammaEventTPCnv/egammaEventTPCnvDict.h
                      egammaEventTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv ParticleEventTPCnv egammaEvent AthenaKernel GaudiKernel egammaEventTPCnv )

