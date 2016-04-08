################################################################################
# Package: DetDescrCnvSvc
################################################################################

# Declare the package name:
atlas_subdir( DetDescrCnvSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate )

# Component(s) in the package:
atlas_add_library( DetDescrCnvSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS DetDescrCnvSvc
                   LINK_LIBRARIES GaudiKernel StoreGateLib SGtests )

atlas_add_component( DetDescrCnvSvc
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel StoreGateLib SGtests DetDescrCnvSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

