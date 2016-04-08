################################################################################
# Package: xAODTrigBphysCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigBphysCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigBphys
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Trigger/TrigEvent/TrigParticle )

# Component(s) in the package:
atlas_add_component( xAODTrigBphysCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODTrigBphys GaudiKernel AthenaBaseComps AthenaKernel TrigParticle )

# Install files from the package:
atlas_install_headers( xAODTrigBphysCnv )
atlas_install_joboptions( share/*.py )

