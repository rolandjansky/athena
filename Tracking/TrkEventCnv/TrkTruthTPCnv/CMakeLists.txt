################################################################################
# Package: TrkTruthTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrkTruthTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/DataModelAthenaPool
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Generators/GeneratorObjects
                          Generators/GeneratorObjectsTPCnv
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTruthData
                          PRIVATE
                          Control/AthenaKernel
                          Control/CxxUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrkTruthTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrkTruthTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel DataModelAthenaPoolLib SGTools StoreGateLib SGtests AthenaPoolCnvSvcLib GeneratorObjects GeneratorObjectsTPCnv InDetIdentifier TrkTrack TrkTruthData AthenaKernel CxxUtils )

atlas_add_dictionary( TrkTruthTPCnvDict
                      src/TrkTruthTPCnvDict.h
                      TrkTruthTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel DataModelAthenaPoolLib SGTools StoreGateLib SGtests AthenaPoolCnvSvcLib GeneratorObjects GeneratorObjectsTPCnv InDetIdentifier TrkTrack TrkTruthData AthenaKernel CxxUtils TrkTruthTPCnv )

