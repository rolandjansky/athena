################################################################################
# Package: HGTD_IdCnv
################################################################################

# Declare the package name:
atlas_subdir( HGTD_IdCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/StoreGate
                          DetectorDescription/DetDescrCnvSvc
                          DetectorDescription/IdDictDetDescr
                          GaudiKernel
                          HighGranularityTimingDetector/HGTD_DetDescr/HGTD_Identifier )

# Component(s) in the package:
atlas_add_component( HGTD_IdCnv
                     src/*.cxx
                     LINK_LIBRARIES StoreGateLib SGtests DetDescrCnvSvcLib IdDictDetDescr GaudiKernel HGTD_Identifier )

# Install files from the package:
atlas_install_joboptions( share/*.py )
