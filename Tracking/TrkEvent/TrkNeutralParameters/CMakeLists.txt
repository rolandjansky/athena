# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkNeutralParameters )

# installed library
atlas_add_library( TrkNeutralParameters
                   PUBLIC_HEADERS TrkNeutralParameters
                   LINK_LIBRARIES TrkSurfaces TrkParametersBase )

#Executables for tests
atlas_add_executable( TrkNeutralParameters_testConstExprMethods
                      test/testConstExpr.cxx 
                      LINK_LIBRARIES TrkNeutralParameters)
#Executables for tests
atlas_add_test( ut_TrkNeutralParameters_testConstExprMethods
	        SCRIPT TrkNeutralParameters_testConstExprMethods)
