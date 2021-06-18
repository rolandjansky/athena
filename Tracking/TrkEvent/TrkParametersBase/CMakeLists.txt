# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkParametersBase )

# Declare an interface library for the headers of the package:
atlas_add_library( TrkParametersBase
   TrkParametersBase/*.h TrkParametersBase/*.icc
   INTERFACE
   PUBLIC_HEADERS TrkParametersBase
   LINK_LIBRARIES GeoPrimitives EventPrimitives TrkEventPrimitives GaudiKernel )
   
atlas_add_test( ParametersBase_test
   SOURCES test/ParametersBase_test.cxx TrkParametersBase/*.h TrkParametersBase/*.icc
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} 
   GaudiKernel  TrkParametersBase
   POST_EXEC_SCRIPT "nopost.sh" )

