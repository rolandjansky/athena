################################################################################
# Package: TrigInDetAccelerationService
################################################################################

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:

atlas_subdir( TrigInDetAccelerationService )

# External dependencies:

find_package( TBB )

# Component(s) in the package:
atlas_add_library( TrigInDetAccelerationServiceLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigInDetAccelerationService
		   INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel GaudiKernel AthenaBaseComps StoreGateLib InDetIdentifier PixelReadoutGeometry SCT_ReadoutGeometry TrigAccelEvent ${TBB_LIBRARIES} rt dl)

atlas_add_component( TrigInDetAccelerationService
                     src/components/*.cxx
                     LINK_LIBRARIES TrigInDetAccelerationServiceLib AthenaKernel GaudiKernel AthenaBaseComps StoreGateLib TrigAccelEvent ${TBB_LIBRARIES} rt dl)

atlas_install_python_modules( python/*.py )
