################################################################################
# Package: TrigInDetAccelerationTool
################################################################################

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetAccelerationTool )

# Component(s) in the package:
atlas_add_component( TrigInDetAccelerationTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetIdentifier TrigInDetEvent TrigSteeringEvent TrigInDetPattRecoTools AthenaBaseComps InDetIdentifier TrigInDetPattRecoEvent TrigAccelEvent TrigInDetAccelerationServiceLib)

# Install files from the package:
atlas_install_headers( TrigInDetAccelerationTool )

