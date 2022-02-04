# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ActsEDM )

# External dependencies:
find_package( Acts COMPONENTS Core )
find_package( Boost )
find_package( Eigen )

atlas_add_library( ActsEDM
                   Root/*.cxx
                   PUBLIC_HEADERS ActsEDM
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} 
		   AthenaBaseComps GaudiKernel AtlasDetDescr CxxUtils 
		   xAODCore ActsCore ActsFatras ActsGeometryLib BeamSpotConditionsData
		   MagFieldConditions MagFieldElements SiSPSeededTrackFinderData InDetRawData GeoPrimitives InDetPrepRawData
		   )

atlas_add_dictionary( ActsEDMDict
		      ActsEDM/ActsEDMDict.h
		      ActsEDM/selection.xml
		      LINK_LIBRARIES xAODCore ActsEDM
		      DATA_LINKS 
		      ActsTrk::MeasurementContainer
		      ActsTrk::SpacePointData 
		      ActsTrk::SpacePointContainer 
		      ActsTrk::SeedContainer 
		      )
