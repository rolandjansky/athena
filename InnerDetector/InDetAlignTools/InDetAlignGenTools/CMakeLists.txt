# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetAlignGenTools )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( HepPDT )

atlas_add_library( InDetAlignGenToolsLib
                   InDetAlignGenTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS InDetAlignGenTools
                   LINK_LIBRARIES GaudiKernel CxxUtils Identifier AthenaKernel AthenaBaseComps InDetAlignTrkInfo EventPrimitives GeoPrimitives TrkTrack AthenaPoolUtilities InDetPrepRawData InDetReadoutGeometry StoreGateLib TrkToolInterfaces InDetAlignToolInterfaces TrkEventUtils TrkEventPrimitives )

# Component(s) in the package:
atlas_add_component( InDetAlignGenTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${CORAL_LIBRARIES} ${HEPPDT_LIBRARIES} AthContainers AthenaKernel AthenaPoolUtilities AtlasDetDescr AtlasHepMCLib DetDescrConditions GaudiKernel GeoPrimitives Identifier InDetAlignGenToolsLib InDetAlignTrkInfo InDetIdentifier InDetPrepRawData ReadoutGeometryBase InDetReadoutGeometry PixelReadoutGeometry RegistrationServicesLib SCT_ReadoutGeometry TrackRecordLib TrkEventPrimitives TrkExInterfaces TrkFitterInterfaces TrkMeasurementBase TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkToolInterfaces TrkTrack TrkTrackSummary TrkTruthData )
