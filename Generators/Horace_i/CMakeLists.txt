################################################################################
# Package: Horace_i
################################################################################

# Declare the package name:
atlas_subdir( Horace_i )

# this line failed automatic conversion in cmt2cmake :
# macro Horace_i_fflags ""  32                "-DFVOIDP=INTEGER*4 -fno-second-underscore" 64                "-DFVOIDP=INTEGER*8 -fno-second-underscore"

# Component(s) in the package:
atlas_add_library( Horace_i
                   src/*.F
                   PUBLIC_HEADERS Horace_i )

# Install files from the package:
atlas_install_joboptions( share/*.py )

