# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( AthExCUDA )

# Add a component library that has some CUDA code in it.
atlas_add_component( AthExCUDA
   src/*.h src/*.cxx src/*.cu src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel )

# Install extra files from the package.
atlas_install_joboptions( share/*.py )
