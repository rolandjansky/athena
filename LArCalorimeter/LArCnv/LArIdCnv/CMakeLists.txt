################################################################################
# Package: LArIdCnv
################################################################################

# Declare the package name:
atlas_subdir( LArIdCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/DetDescrCnvSvc
                          PRIVATE
                          Calorimeter/CaloIdentifier
                          Control/StoreGate
                          DetectorDescription/IdDictDetDescr
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArIdentifier )

# Component(s) in the package:
atlas_add_component( LArIdCnv
                     src/*.cxx
                     LINK_LIBRARIES DetDescrCnvSvcLib CaloIdentifier StoreGateLib SGtests IdDictDetDescr Identifier GaudiKernel LArIdentifier )

# Install files from the package:
atlas_install_headers( LArIdCnv )
atlas_install_joboptions( share/*.py )

