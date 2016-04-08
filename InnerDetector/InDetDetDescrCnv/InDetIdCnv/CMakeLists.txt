################################################################################
# Package: InDetIdCnv
################################################################################

# Declare the package name:
atlas_subdir( InDetIdCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/StoreGate
                          DetectorDescription/DetDescrCnvSvc
                          DetectorDescription/IdDictDetDescr
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier )

# Component(s) in the package:
atlas_add_component( InDetIdCnv
                     src/*.cxx
                     LINK_LIBRARIES StoreGateLib SGtests DetDescrCnvSvcLib IdDictDetDescr GaudiKernel InDetIdentifier )

# Install files from the package:
atlas_install_joboptions( share/*.py )

