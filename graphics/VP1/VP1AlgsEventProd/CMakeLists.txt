# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1AlgsEventProd )

# Component(s) in the package:
atlas_add_component( VP1AlgsEventProd
   VP1AlgsEventProd/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel StorageSvc PathResolver
   VP1UtilsBase )
