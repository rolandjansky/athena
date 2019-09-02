################################################################################
# Package: McEventSelector
################################################################################

# Declare the package name:
atlas_subdir( McEventSelector )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          PRIVATE
                          Event/EventInfo )

# Component(s) in the package:
atlas_add_component( McEventSelector
                     src/McAddress.cxx
                     src/McEventSelector.cxx
                     src/McEventCnv.cxx
                     src/McCnvSvc.cxx
                     src/McEventSelector_entries.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel EventInfo )

# Install files from the package:
atlas_install_headers( McEventSelector )
atlas_install_python_modules( python/*.py )
