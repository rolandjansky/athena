# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrackingGeometry )

# External dependencies:
find_package( Boost )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( InDetTrackingGeometry
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps CxxUtils GeoPrimitives GaudiKernel InDetIdentifier TrkDetDescrInterfaces TrkDetDescrUtils TrkGeometry BeamPipeGeoModelLib StoreGateLib Identifier InDetReadoutGeometry PixelReadoutGeometry SCT_ReadoutGeometry TRT_ReadoutGeometry TrkDetDescrGeoModelCnv TrkSurfaces TrkVolumes SubDetectorEnvelopesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
