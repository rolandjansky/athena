################################################################################
# Package: ProtosLHEF_i
################################################################################

# Declare the package name:
atlas_subdir( ProtosLHEF_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PRIVATE
                          GaudiKernel
                          Generators/GeneratorFortranCommon )

# Component(s) in the package:
atlas_add_component( ProtosLHEF_i
                     src/*.cxx
                     src/*.F
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel GeneratorFortranCommonLib )

# Install files from the package:
atlas_install_headers( ProtosLHEF_i )
atlas_install_joboptions( share/common/*.py )
atlas_install_runtime( share/file/*.events share/file/*.dat )

