/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  MdtOnTrackTool 
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface for MuonDriftCircleOnTrack production
// (for MDT technology)
///////////////////////////////////////////////////////////////////
// Version 1.0 18/07/2004 
///////////////////////////////////////////////////////////////////

#ifndef MUON_MUONCOMPETINGCLUSTERSONTRACKCREATOR_H
#define MUON_MUONCOMPETINGCLUSTERSONTRACKCREATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkEventPrimitives/LocalParameters.h"

class Identifier;
class MdtIdHelper;
class MdtCalibrationSvc;

namespace MuonGM {
  class MuonDetectorManager;
}




namespace Muon {

  class IMuonTofTool;
  class MdtPrepData;
  class MdtDriftCircleOnTrack;
  class IMuonClusterOnTrackCreator;

  /**
     @brief Tool to create MuonCompetingClustersOnTrack objects 
  */
  class MuonCompetingClustersOnTrackCreator : public AthAlgTool, virtual public IMuonCompetingClustersOnTrackCreator
  {
  public:

    MuonCompetingClustersOnTrackCreator(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonCompetingClustersOnTrackCreator ();
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();

    /** method for the creation of a single 
	Trk::CompetingRIOsOnTrack:
        It takes a list of RIOs (PrepRawData) and the given Track 
	Parameter, cutValue defines
        the cut used for calculating the initial assignment probabilities 
	(see Trk::IWeightCalculator). Not implemented for now!! 
    */
    const Trk::CompetingRIOsOnTrack* createCompetingROT(const std::list< const Trk::PrepRawData * > &,
							const Trk::TrackParameters&,
							const Trk::IWeightCalculator::AnnealingFactor ) const
      {
	return 0;
      }
    
    /** method for the update of the assignment 
	probabilities and effective measurements
        of an Muon::CompetingMuonClustersOnTrack using a new track prediction. Not implemented for now!! */
    void updateCompetingROT( Trk::CompetingRIOsOnTrack&,
			     const Trk::TrackParameters&,
			     const Trk::IWeightCalculator::AnnealingFactor ) const
    {
    }
    
    /** method to create a CompetingMuonClustersOnTrack using the PrepRawData hits and a scaled factor for the errors */
    const CompetingMuonClustersOnTrack* createBroadCluster(const std::list< const Trk::PrepRawData * > &,   const double errorScaleFactor ) const;


  private:
 
   ToolHandle<Muon::IMuonClusterOnTrackCreator>    m_clusterCreator;        //<! pointer to muon cluster rio ontrack creator

 
  };

}

#endif 
