################################################################################
# Package: AnalysisTriggerEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( AnalysisTriggerEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          PRIVATE
                          Control/AthenaKernel
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( AnalysisTriggerEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS AnalysisTriggerEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel AnalysisTriggerEvent AthenaKernel TrigT1Interfaces )

atlas_add_dictionary( AnalysisTriggerEventTPCnvDict
                      AnalysisTriggerEventTPCnv/AnalysisTriggerEventTPCnvDict.h
                      AnalysisTriggerEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel AnalysisTriggerEvent AthenaKernel TrigT1Interfaces AnalysisTriggerEventTPCnv )

