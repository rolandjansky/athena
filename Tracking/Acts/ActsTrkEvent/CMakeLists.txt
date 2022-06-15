# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ActsTrkEvent )

# External dependencies:
find_package( Acts COMPONENTS Core )
find_package( Boost )
find_package( Eigen )

atlas_add_library( ActsTrkEvent
                   Root/*.cxx
                   PUBLIC_HEADERS ActsTrkEvent
                   INCLUDE_DIRS ${Acts_INCLUDE_DIR}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} 
		   AthenaBaseComps GaudiKernel AtlasDetDescr CxxUtils 
		   xAODCore ActsCore ActsFatras ActsGeometryLib BeamSpotConditionsData
		   MagFieldConditions MagFieldElements GeoPrimitives
		   )

atlas_add_dictionary( ActsTrkEventDict
		      ActsTrkEvent/ActsTrkEventDict.h
		      ActsTrkEvent/selection.xml
		      LINK_LIBRARIES xAODCore ActsTrkEvent
		      DATA_LINKS 
		      ActsTrk::MeasurementContainer
		      ActsTrk::SpacePointData
		      ActsTrk::SpacePointMeasurementDetails
		      ActsTrk::SpacePointContainer 
		      ActsTrk::SeedContainer 
		      ActsTrk::BoundTrackParametersContainer
		      )
