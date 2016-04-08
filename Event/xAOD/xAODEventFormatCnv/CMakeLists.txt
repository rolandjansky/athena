################################################################################
# Package: xAODEventFormatCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODEventFormatCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODEventFormat
                          GaudiKernel
                          PRIVATE
                          Control/AthContainers
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolKernel )

# Component(s) in the package:
atlas_add_component( xAODEventFormatCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODEventFormat GaudiKernel AthContainers AthenaBaseComps AthenaKernel StoreGateLib SGtests )

# Install files from the package:
atlas_install_headers( xAODEventFormatCnv )

