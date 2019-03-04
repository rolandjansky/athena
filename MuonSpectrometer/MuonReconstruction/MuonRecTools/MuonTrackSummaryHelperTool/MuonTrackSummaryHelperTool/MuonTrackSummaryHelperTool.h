/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKSUMMARYHELPERTOOL_H
#define MUONTRACKSUMMARYHELPERTOOL_H

#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"
//
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "StoreGate/ReadHandleKey.h"

#include <vector>
#include <bitset>

class RpcIdHelper;
class TgcIdHelper;
class CscIdHelper;
class MdtIdHelper;
// New Small Wheel
class MmIdHelper;
class sTgcIdHelper;

class Identifier;

namespace Trk {
    class RIO_OnTrack;
    class TrackStateOnSurface;
    class ITrackHoleSearchTool;
    class CompetingRIOsOnTrack;
    class IExtrapolator;
}

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {

  class MuonIdHelperTool;
  class MuonEDMHelperTool;
  
  class MuonTrackSummaryHelperTool :  virtual public Trk::ITrackSummaryHelperTool, public AthAlgTool   {
  public:
        MuonTrackSummaryHelperTool(const std::string&,const std::string&,const IInterface*);
        
        virtual ~MuonTrackSummaryHelperTool ();
        
        virtual StatusCode initialize();
        
        virtual void analyse(
                            const Trk::Track& trk, 
                            const Trk::RIO_OnTrack* rot, 
                            const Trk::TrackStateOnSurface* tsos,
                            std::vector<int>& information, 
                            std::bitset<Trk::numberOfDetectorTypes>& hitPattern  ) const ;
                            
        
        virtual void analyse( 
                            const Trk::Track& trk,
                            const Trk::CompetingRIOsOnTrack* crot,
                            const Trk::TrackStateOnSurface* tsos,
                            std::vector<int>& information, 
                            std::bitset<Trk::numberOfDetectorTypes>& hitPattern ) const;

            
        /** */
        void searchForHoles(
                  const Trk::Track& track,
                  std::vector<int>& information, Trk::ParticleHypothesis hyp) const;

	void addDetailedTrackSummary( const Trk::Track& track, Trk::TrackSummary& summary ) const;

private:

	const MdtPrepDataCollection* findMdtPrdCollection( const Identifier& chId ) const;
	void calculateRoadHits(Trk::MuonTrackSummary::ChamberHitSummary& chamberHitSummary, const Trk::TrackParameters& pars) const;
	bool isFirstProjection( const Identifier& id ) const;
	void updateHoleContent( Trk::MuonTrackSummary::ChamberHitSummary& chamberHitSummary ) const;

        // muon IdHelpers
        const RpcIdHelper* m_rpcId;
        const TgcIdHelper* m_tgcId;
        const CscIdHelper* m_cscId;
        const MdtIdHelper* m_mdtId;
        //New Small Wheel
        const sTgcIdHelper* m_stgcId;
        const MmIdHelper* m_mmId;
        
        /** tool used to do hole search */
        ToolHandle<Trk::ITrackHoleSearchTool> m_muonTgTool;
        
        /* used to work out layer ids etc*/
        ToolHandle<MuonIdHelperTool> m_idHelperTool;
        
        /* used to work out if track has momentum */
        ToolHandle<MuonEDMHelperTool> m_edmHelperTool;
        
        /* used to do hits-in-road search */
        ToolHandle<Trk::IExtrapolator> m_extrapolator;
        
        /* used to do hits-in-road search for straight tracks */
        ToolHandle<Trk::IExtrapolator> m_slExtrapolator;
        
        /**increment the 'type'*/
        void increment(int& type) const;
        
        /**Allows us to block the hole search whilst the Muon tracking geometry etc is being debugged*/
        bool m_doHoles;
        
	/** allow us to block the calculation of close hits */
	bool m_calculateCloseHits;

	/** width road use to associate close hits  */
	double m_roadWidth;
	
	/** storegate key of MdtPrepDataContainer */
	SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_mdtKey{this,"MdtPrepDataContainer","MDT_DriftCircles","MDT PRDs"};

        //std::string m_holeOnTrackToolName;
        mutable const Trk::TrackingGeometry* m_trackingGeometry;
        std::string  m_trackingGeometryName;
	const MuonGM::MuonDetectorManager*  m_detMgr;

    };
    
}
#endif 

