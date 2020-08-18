################################################################################
# Package: TrigInDetAccelerationTool
################################################################################

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetAccelerationTool )

# Component(s) in the package:

atlas_add_library( TrigInDetAccelerationToolLib
		     src/*.cxx
 		     PUBLIC_HEADERS TrigInDetAccelerationTool
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetIdentifier TrigInDetEvent TrigSteeringEvent TrigInDetPattRecoTools AthenaBaseComps InDetIdentifier TrigInDetPattRecoEvent TrigAccelEvent TrigInDetAccelerationServiceLib)

atlas_add_component( TrigInDetAccelerationTool
                     src/components/*.cxx
                     LINK_LIBRARIES TrigInDetAccelerationToolLib)




