################################################################################
# Package: ForwardIdCnv
################################################################################

# Declare the package name:
atlas_subdir( ForwardIdCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/StoreGate
                          DetectorDescription/DetDescrCnvSvc
                          DetectorDescription/IdDictDetDescr
                          ForwardDetectors/ZDC/ZdcIdentifier
                          GaudiKernel )

# Component(s) in the package:
atlas_add_component( ForwardIdCnv
                     src/*.cxx
                     LINK_LIBRARIES StoreGateLib SGtests DetDescrCnvSvcLib IdDictDetDescr ZdcIdentifier GaudiKernel )

