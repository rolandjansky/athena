################################################################################
# Package: MuonIdCnv
################################################################################

# Declare the package name:
atlas_subdir( MuonIdCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/StoreGate
                          DetectorDescription/DetDescrCnvSvc
                          DetectorDescription/IdDictDetDescr
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers )

# Component(s) in the package:
atlas_add_component( MuonIdCnv
                     src/*.cxx
                     LINK_LIBRARIES StoreGateLib SGtests DetDescrCnvSvcLib IdDictDetDescr GaudiKernel MuonIdHelpersLib )

