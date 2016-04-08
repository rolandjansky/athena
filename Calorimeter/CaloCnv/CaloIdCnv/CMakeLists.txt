################################################################################
# Package: CaloIdCnv
################################################################################

# Declare the package name:
atlas_subdir( CaloIdCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Control/SGTools
                          DetectorDescription/DetDescrCnvSvc
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/IdDictDetDescr
                          DetectorDescription/Identifier
                          GaudiKernel )

# Component(s) in the package:
atlas_add_component( CaloIdCnv
                     src/*.cxx
                     LINK_LIBRARIES CaloIdentifier AthenaKernel SGTools DetDescrCnvSvcLib StoreGateLib SGtests IdDictDetDescr Identifier GaudiKernel )

# Install files from the package:
atlas_install_headers( CaloIdCnv )
atlas_install_joboptions( share/*.py )

