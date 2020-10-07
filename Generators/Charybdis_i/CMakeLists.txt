# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( Charybdis_i )

# External dependencies:
find_package( Herwig )
find_package( Lhapdf )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Charybdis_i
   Charybdis_i/*.h Charybdis_i/*.inc src/*.cxx src/*.F
   PUBLIC_HEADERS Charybdis_i
   PRIVATE_INCLUDE_DIRS ${HERWIG_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS}
   LINK_LIBRARIES GeneratorFortranCommonLib
   PRIVATE_LINK_LIBRARIES ${HERWIG_LIBRARIES} ${LHAPDF_LIBRARIES} )

# Install files from the package:
atlas_install_joboptions( share/*.py )
