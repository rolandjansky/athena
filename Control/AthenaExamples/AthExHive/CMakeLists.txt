################################################################################
# Package: AthExHive
################################################################################

# Declare the package name:
atlas_subdir( AthExHive )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/SGTools
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/StoreGate
                          Event/EventInfo
                          GaudiKernel )

# Component(s) in the package:
atlas_add_component( AthExHive
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel AthenaBaseComps EventInfo )

# Install files from the package:
atlas_install_headers( AthExHive )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/condDb.txt )

