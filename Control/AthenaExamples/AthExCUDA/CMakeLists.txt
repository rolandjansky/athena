# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( AthExCUDA )

# Add a component library that has some CUDA code in it.
atlas_add_component( AthExCUDA
   src/*.h src/*.cxx src/*.cu src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel AthCUDAInterfacesLib
   AthCUDAKernelLib )

# Install extra files from the package.
atlas_install_joboptions( share/*.py )

# Test(s) in the package.
atlas_add_test( LinearTransformTask
   SCRIPT athena.py AthExCUDA/LinearTransformTaskExample_jobOptions.py )
atlas_add_test( LinearTransformTaskMT
   SCRIPT athena.py --threads=4
          AthExCUDA/LinearTransformTaskExample_jobOptions.py )
