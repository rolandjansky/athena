# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeoExporter )

# External dependencies:
find_package( Qt5 COMPONENTS Sql Gui PrintSupport )
find_package( Boost )
find_package( GeoModelIO REQUIRED )

# Component(s) in the package:
atlas_add_library( GeoExporter
                   src/*.cxx
                   GeoExporter/*.h
                   PUBLIC_HEADERS GeoExporter
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES Qt5::Sql ${GEOMODELIO_LIBRARIES} GeoModelUtilities VP1Base VP1Utils
                  )
                  #NEW SYNTAX TO BE USED SOON: GeoModelIO::GeoModelDBManager GeoModelIO::GeoModelWrite

