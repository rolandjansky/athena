/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MOOSEGMENTFINDERS_NSWSECTORROADFINDERALG_H
#define MOOSEGMENTFINDERS_NSWSECTORROADFINDERALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataCollection.h"
#include "MuonPrepRawData/sTgcPrepDataCollection.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonPattern/MuonPatternChamberIntersect.h"


class NSWSectorRoadFinderAlg : public AthAlgorithm
{
 public:
  NSWSectorRoadFinderAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~NSWSectorRoadFinderAlg();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();


 private:

  /** templated method since used for all technologies */
  template < class PrdT > void addContainerToMuonPatternCombination(const Muon::MuonPrepDataContainer< Muon::MuonPrepDataCollection< PrdT > >& cont, 
								    std::vector< Muon::MuonPatternChamberIntersect >& chamberData, int sector ) {
    // find the collections in the right sector
    typename Muon::MuonPrepDataContainer< Muon::MuonPrepDataCollection< PrdT > >::const_iterator it = cont.begin();
    typename Muon::MuonPrepDataContainer< Muon::MuonPrepDataCollection< PrdT > >::const_iterator it_end = cont.end();
    for( ;it!=it_end; ++it ){
      
      const Muon::MuonPrepDataCollection< PrdT >& col = **it;
      if( m_idHelper->sector(col.identify()) != sector ) continue;
      if( col.empty() ) continue;
      if( ! col.front()->detectorElement() ) {
	ATH_MSG_WARNING("PrepData with out detector element pointer, skipping collection ");
	continue;
      }
      ATH_MSG_VERBOSE(" chamber  " << m_idHelper->toStringChamber(col.identify()) << "  with hits " << col.size() );
      std::vector< const Trk::PrepRawData* > rios;
      rios.insert(rios.end(), col.begin(), col.end());
      chamberData.push_back( Muon::MuonPatternChamberIntersect( col.front()->detectorElement()->center(),col.front()->detectorElement()->normal(),rios ) );
    }
  }
  
  ToolHandle<Muon::MuonIdHelperTool> m_idHelper;

  /** storegate location of the MuonPrepDataContainer for all four technologies */
  std::string         m_keysTgc;
  std::string         m_keyMM;
  std::string         m_patternLocation;

};


#endif
