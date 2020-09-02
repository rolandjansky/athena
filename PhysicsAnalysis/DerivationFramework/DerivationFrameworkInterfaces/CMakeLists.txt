# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkInterfaces )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkInterfaces
   DerivationFrameworkInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS DerivationFrameworkInterfaces
   LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( DerivationFrameworkInterfacesDict
   DerivationFrameworkInterfaces/DerivationFrameworkInterfacesDict.h
   DerivationFrameworkInterfaces/selection.xml
   LINK_LIBRARIES DerivationFrameworkInterfaces )

