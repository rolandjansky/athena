# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( CaloDetDescr )

# External dependencies.
find_package( Boost )

# Component(s) in the package.
atlas_add_library( CaloDetDescrLib
   CaloDetDescr/*.h src/*.h src/*.cxx
   PUBLIC_HEADERS CaloDetDescr
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel AthenaKernel AthContainers
   AthenaBaseComps CxxUtils GeoModelInterfaces Identifier CaloIdentifier
   CaloGeoHelpers GeoPrimitives LArReadoutGeometry
   PRIVATE_LINK_LIBRARIES CaloConditions StoreGateLib )

atlas_add_component( CaloDetDescr
   src/components/*.cxx
   LINK_LIBRARIES CaloDetDescrLib )

atlas_add_dictionary( CaloDetDescrDict
   CaloDetDescr/CaloDetDescrDict.h
   CaloDetDescr/selection.xml
   LINK_LIBRARIES CaloDetDescrLib )

# Install files from the package.
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py
   POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )

# Test(s) in the package.
atlas_add_test( CaloConstIteratorAdaptor_test
  SOURCES test/CaloConstIteratorAdaptor_test.cxx
  LINK_LIBRARIES CaloDetDescrLib )
