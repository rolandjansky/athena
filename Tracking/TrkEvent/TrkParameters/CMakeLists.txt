# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkParameters )

# Component(s) in the package:
atlas_add_library( TrkParameters
                   src/*.cxx
                   PUBLIC_HEADERS TrkParameters
                   LINK_LIBRARIES TrkSurfaces TrkParametersBase )

atlas_add_dictionary( TrkParametersDict
                      TrkParameters/TrkParametersDict.h
                      TrkParameters/selection.xml
                      LINK_LIBRARIES TrkParameters )

# Tests:
atlas_add_executable( TrkParameters_testConstExprMethods
	                  test/testConstExpr.cxx
		              LINK_LIBRARIES TrkParameters)

atlas_add_test( ut_TrkParameters_testConstExprMethods
                SCRIPT TrkParameters_testConstExprMethods)
