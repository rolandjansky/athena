# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeneratorFortranCommon )

# Component(s) in the package:
atlas_add_library( GeneratorFortranCommonLib
   src/inituser.F src/useuser.F src/getaddr.cxx
   PUBLIC_HEADERS GeneratorFortranCommon
   LINK_LIBRARIES AthenaKernel GaudiKernel )
