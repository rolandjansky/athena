/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MOOSEGMENTFINDERS_NSWSECTORROADFINDERALG_H
#define MOOSEGMENTFINDERS_NSWSECTORROADFINDERALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataCollection.h"
#include "MuonPrepRawData/sTgcPrepDataCollection.h"
#include "MuonPattern/MuonPatternChamberIntersect.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class NSWSectorRoadFinderAlg : public AthAlgorithm
{
 public:
  NSWSectorRoadFinderAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~NSWSectorRoadFinderAlg()=default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;


 private:

  /** templated method since used for all technologies */
  template < class PrdT > void addContainerToMuonPatternCombination(const Muon::MuonPrepDataContainer< Muon::MuonPrepDataCollection< PrdT > >& cont, 
								    std::vector< Muon::MuonPatternChamberIntersect >& chamberData, int sector ) {
    // find the collections in the right sector
    typename Muon::MuonPrepDataContainer< Muon::MuonPrepDataCollection< PrdT > >::const_iterator it = cont.begin();
    typename Muon::MuonPrepDataContainer< Muon::MuonPrepDataCollection< PrdT > >::const_iterator it_end = cont.end();
    for( ;it!=it_end; ++it ){
      
      const Muon::MuonPrepDataCollection< PrdT >& col = **it;
      if( m_idHelperSvc->sector(col.identify()) != sector ) continue;
      if( col.empty() ) continue;
      if( ! col.front()->detectorElement() ) {
	ATH_MSG_WARNING("PrepData with out detector element pointer, skipping collection ");
	continue;
      }
      ATH_MSG_VERBOSE(" chamber  " << m_idHelperSvc->toStringChamber(col.identify()) << "  with hits " << col.size() );
      std::vector< const Trk::PrepRawData* > rios;
      rios.insert(rios.end(), col.begin(), col.end());
      chamberData.push_back( Muon::MuonPatternChamberIntersect( col.front()->detectorElement()->center(),col.front()->detectorElement()->normal(),rios ) );
    }
  }

  /** storegate location of the MuonPrepDataContainer for all four technologies */
  SG::ReadHandleKey<Muon::sTgcPrepDataContainer>         m_keysTgc;
  SG::ReadHandleKey<Muon::MMPrepDataContainer>           m_keyMM;
  SG::WriteHandleKey<MuonPatternCombinationCollection>   m_patternLocation;
  
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
};


#endif
