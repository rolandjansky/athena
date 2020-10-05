# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1HEPVis )

# External dependencies:
find_package( Coin3D  )
#find_package( OpenGL ) # not needed for SLC6. But is it needed for Ubuntu?
if ( APPLE )
	find_package(OpenGL REQUIRED)
endif()

#message("VP1HEPVis: '${COIN3D_FOUND}' '${COIN3D_INCLUDE_DIRS}'
#'${COIN3D_LIBRARIES}' '${COIN3D_LIBRARY}' '${COIN3D_INCLUDE_DIR}' ")

# Component(s) in the package:
if ( APPLE )
	atlas_add_library( VP1HEPVis src/*.cxx
	   PUBLIC_HEADERS VP1HEPVis
	   INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
	   LINK_LIBRARIES ${COIN3D_LIBRARIES} ${OPENGL_LIBRARIES} ${OPENGL_gl_LIBRARY} )
endif()
if ( UNIX AND NOT APPLE)
	atlas_add_library( VP1HEPVis src/*.cxx
	   PUBLIC_HEADERS VP1HEPVis
	   INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
	   LINK_LIBRARIES ${COIN3D_LIBRARIES} ${OPENGL_LIBRARIES} GL )
endif()
