################################################################################
# Package: MuonIdCnv
################################################################################

# Declare the package name:
atlas_subdir( MuonIdCnv )

# Component(s) in the package:
atlas_add_component( MuonIdCnv
                     src/*.cxx
                     LINK_LIBRARIES StoreGateLib SGtests DetDescrCnvSvcLib IdDictDetDescr GaudiKernel MuonIdHelpersLib AthenaKernel )

