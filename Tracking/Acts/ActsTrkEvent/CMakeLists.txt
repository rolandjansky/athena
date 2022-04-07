# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ActsTrkEvent )

# External dependencies:
find_package( Acts COMPONENTS Core )
find_package( Boost )
find_package( Eigen )

atlas_add_library( ActsTrkEvent
                   Root/*.cxx
                   PUBLIC_HEADERS ActsTrkEvent
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} 
		   AthenaBaseComps GaudiKernel AtlasDetDescr CxxUtils 
		   xAODCore ActsCore ActsFatras ActsGeometryLib BeamSpotConditionsData
		   MagFieldConditions MagFieldElements SiSPSeededTrackFinderData InDetRawData GeoPrimitives InDetPrepRawData
		   )

atlas_add_dictionary( ActsTrkEventDict
		      ActsTrkEvent/ActsTrkEventDict.h
		      ActsTrkEvent/selection.xml
		      LINK_LIBRARIES xAODCore ActsTrkEvent
		      DATA_LINKS 
		      ActsTrk::MeasurementContainer
		      ActsTrk::SpacePointData 
		      ActsTrk::SpacePointContainer 
		      ActsTrk::SeedContainer 
		      ActsTrk::BoundTrackParametersContainer
		      )
