################################################################################
# Package: MinimalRunTime
################################################################################

# Declare the package name:
atlas_subdir( MinimalRunTime )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaCommon
                          Control/AthenaKernel
                          Control/AthenaServices
                          Control/CLIDSvc
                          Control/DataModel
                          Control/IOVSvc
                          Control/SGTools
                          Control/StoreGate
                          Database/ConnectionManagement/AtlasAuthentication
                          DetectorDescription/DetDescrCnvSvc
                          DetectorDescription/IdDictDetDescrCnv
                          GaudiAlg
                          GaudiAud
                          GaudiKernel
                          GaudiPython
                          GaudiSvc
                          Generators/McEventSelector
                          RootHistCnv )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Oracle )
find_package( RELAX )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( dcache_client )
find_package( pytools )

