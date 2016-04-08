################################################################################
# Package: AthExFortranAlgorithm
################################################################################

# Declare the package name:
atlas_subdir( AthExFortranAlgorithm )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PRIVATE
                          GaudiKernel )

# Component(s) in the package:
atlas_add_component( AthExFortranAlgorithm
                     src/*.cxx
                     src/*.F
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel )

# Install files from the package:
atlas_install_headers( AthExFortranAlgorithm )
atlas_install_joboptions( share/FortranAlgorithmOptions.txt share/FortranAlgorithmOptions.py )
atlas_install_runtime( share/FortranAlgorithmInput.data )
atlas_install_scripts( share/preFortAlgEx.sh )

