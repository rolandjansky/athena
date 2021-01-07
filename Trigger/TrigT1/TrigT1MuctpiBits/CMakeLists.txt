# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( TrigT1MuctpiBits )

# Component(s) in the package.
atlas_add_library( TrigT1MuctpiBits
                   TrigT1MuctpiBits/*.h
		   INTERFACE
		   PUBLIC_HEADERS TrigT1MuctpiBits )
