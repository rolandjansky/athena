################################################################################
# Package: VP1Algs
################################################################################

# Declare the package name:
atlas_subdir( VP1Algs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Event/EventInfo
                          Tools/PathResolver
                          graphics/VP1/VP1Gui
                          graphics/VP1/VP1UtilsBase )

# Component(s) in the package:
atlas_add_component( VP1Algs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel EventInfo PathResolver VP1Gui VP1UtilsBase )

# Install files from the package:
atlas_install_headers( VP1Algs )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/vp1 )
atlas_install_scripts( share/clear-bash-hash-table-vp1 )


