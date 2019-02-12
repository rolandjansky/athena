/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNTOOLS_MUONLAYERHOUGHTOOL_H
#define MUONHOUGHPATTERNTOOLS_MUONLAYERHOUGHTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecToolInterfaces/IMuonHoughPatternFinderTool.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonLayerHough/HitNtuple.h"

#include "MuonClusterization/TgcHitClustering.h"
#include "MuonClusterization/RpcHitClustering.h"
#include "MuonRecToolInterfaces/IMuonTruthSummaryTool.h"
#include "MuonLayerHough/MuonRegionHough.h"
#include "MuonLayerHough/MuonLayerHough.h"
#include "MuonLayerHough/MuonPhiLayerHough.h"
#include "MuonLayerHough/MuonLayerHoughSelector.h"

#include "TMath.h"
#include <set>
#include "GaudiKernel/IIncidentListener.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"

class IIncidentSvc;

namespace Trk{
  class PrepRawData;
}

class TFile;
class TTree;
class PRD_MultiTruthCollection;

namespace MuonGM {
  class MuonDetectorManager;
}
namespace MuonHough {
  class HitDebugInfo;
}



static const InterfaceID IID_MuonLayerHoughTool("Muon::MuonLayerHoughTool",1,0);

namespace Muon {

  class MuonIdHelperTool;


  class MuonLayerHoughTool:  virtual public IMuonHoughPatternFinderTool, virtual public IIncidentListener, public AthAlgTool  {
  public:
    
    typedef std::vector<IdentifierHash>   HashVec;
    typedef std::vector< HashVec >        RegionHashVec;
    typedef std::vector< RegionHashVec >  TechnologyRegionHashVec;
    typedef RegionHashVec::const_iterator RegionHashVecCit;

    struct CollectionsPerSector {
      int sector;
      TechnologyRegionHashVec technologyRegionHashVecs;
    };
    typedef std::vector<CollectionsPerSector>       CollectionsPerSectorVec;
    typedef CollectionsPerSectorVec::const_iterator CollectionsPerSectorCit;

    typedef std::vector<MuonHough::Hit*>    HitVec;
    typedef std::vector< HitVec >           RegionHitVec;
    typedef std::vector<MuonHough::PhiHit*> PhiHitVec;
    typedef std::vector< PhiHitVec >        RegionPhiHitVec;
    typedef std::vector<MuonHough::MuonLayerHough::Maximum*>    MaximumVec;
    typedef std::vector<MuonHough::MuonPhiLayerHough::Maximum*> PhiMaximumVec; 
    typedef std::map<MuonHough::MuonLayerHough::Maximum*, MaximumVec > MaximumAssociationMap;

    typedef std::vector< MaximumVec >       RegionMaximumVec;
    typedef std::vector< PhiMaximumVec >    RegionPhiMaximumVec;

    struct HoughDataPerSector {
      HoughDataPerSector() {
	sector = -1;
	hitVec.resize(MuonStationIndex::sectorLayerHashMax());
	maxVec.resize(MuonStationIndex::sectorLayerHashMax());
	phiHitVec.resize(MuonStationIndex::DetectorRegionIndexMax);
	phiMaxVec.resize(MuonStationIndex::DetectorRegionIndexMax);
	nlayersWithMaxima.resize(MuonStationIndex::DetectorRegionIndexMax);
	nphilayersWithMaxima.resize(MuonStationIndex::DetectorRegionIndexMax);
	nmaxHitsInRegion.resize(MuonStationIndex::DetectorRegionIndexMax);
	nphimaxHitsInRegion.resize(MuonStationIndex::DetectorRegionIndexMax);
      }
      void cleanUp();
      int                   sector;
      RegionHitVec          hitVec;
      RegionPhiHitVec       phiHitVec;
      RegionMaximumVec      maxVec;
      RegionPhiMaximumVec   phiMaxVec;
      std::vector<int>      nlayersWithMaxima;
      std::vector<int>      nphilayersWithMaxima;
      std::vector<int>      nmaxHitsInRegion;
      std::vector<int>      nphimaxHitsInRegion;
      MaximumAssociationMap maxAssociationMap; // stores association of a given maximium with other maxima in neighbouring sectors
      std::set<MuonHough::MuonLayerHough::Maximum*> associatedToOtherSector; // used to flagged maxima that were already associated to another sector
      // returns the number of phi and eta hits in the region with most eta hits
      // regions with phi hits are always prefered over regions without
      int maxEtaHits() const{
	return std::max( nmaxHitsInRegion[0], std::max( nmaxHitsInRegion[1], nmaxHitsInRegion[2] ) );
      }
    };

    typedef std::vector<HoughDataPerSector>       HoughDataPerSectorVec;
    typedef HoughDataPerSectorVec::const_iterator HoughDataPerSectorCit;
    
    
    class Road {
    public:
      Road(MuonHough::MuonLayerHough::Maximum& seed_) : neighbouringRegion(MuonStationIndex::DetectorRegionUnknown), neighbouringSector(-1), seed(&seed_) { add(seed); }
      Road() : neighbouringRegion(MuonStationIndex::DetectorRegionUnknown), neighbouringSector(-1), seed(0) {} 
      MuonStationIndex::DetectorRegionIndex neighbouringRegion;
      int  neighbouringSector;
      MuonHough::MuonLayerHough::Maximum* seed;
      void add( MuonHough::MuonLayerHough::Maximum* max ){
        maxima.push_back(max);
        maximumSet.insert(max);
      }
      void add( MuonHough::MuonPhiLayerHough::Maximum* max ){
        phiMaxima.push_back(max);
      }
      MaximumVec    maxima;
      PhiMaximumVec phiMaxima;
      std::set<MuonHough::MuonLayerHough::Maximum*>    maximumSet;

      std::vector<MuonHough::MuonPhiLayerHough::Maximum> mergedPhiMaxima;
    };
    

    /** Default constructor */
    MuonLayerHoughTool(const std::string& type, const std::string& name, const IInterface* parent);

    /** Destructor */
    virtual ~MuonLayerHoughTool();
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonLayerHoughTool; }

    StatusCode initialize();

    StatusCode finalize();
    
    MuonPatternCombinationCollection* analyse( const MdtPrepDataContainer*  mdtCont,  
					       const CscPrepDataContainer*  cscCols,  
					       const TgcPrepDataContainer*  tgcCont,  
					       const RpcPrepDataContainer*  rpcCont,
					       const sTgcPrepDataContainer* stgcCont,  
					       const MMPrepDataContainer*  mmCont ) const;

    /** find patterns for a give set of MuonPrepData collections + optionally CSC segment combinations */
    MuonPatternCombinationCollection* find( const std::vector<const MdtPrepDataCollection*>& mdtCols,  
					    const std::vector<const CscPrepDataCollection*>& cscCols,  
					    const std::vector<const TgcPrepDataCollection*>& tgcCols,  
					    const std::vector<const RpcPrepDataCollection*>& rpcCols,  
					    const MuonSegmentCombinationCollection* ) const;
    void reset() const;

    void getSectors( const Amg::Vector3D& pos, std::vector<int>& sectors ) const;
    void getSectors( const TgcClusterObj3D& tgc, std::vector<int>& sectors ) const;

    double rCor( const Amg::Vector3D& pos, const Identifier& id ) const; 
    double rCor( const MuonCluster& rpc ) const; 
    double rCor( const MdtPrepData& mdt ) const;
    double rCor( const TgcClusterObj3D& tgc, int val, int sector ) const;

    int sublay( const Identifier& id, float z = 0 ) const; // the z value is only used for the tgcs

    const MuonHough::MuonDetectorHough& houghTransforms() const { return m_detectorHoughTransforms; }
    const HoughDataPerSectorVec&        houghData() const { return m_houghDataPerSectorVec; }

    /**  incident service handle for EndEvent */
    void handle(const Incident& inc);// maybe in the future clear per event

  private:

    MuonPatternCombinationCollection* analyse() const;

    void fillHitsPerSector(  const CollectionsPerSector& hashes,
			     const MdtPrepDataContainer*  mdtCont,  
			     const CscPrepDataContainer*  /*cscCont*/,  
			     const TgcPrepDataContainer*  tgcCont,  
			     const RpcPrepDataContainer*  rpcCont,
			     const sTgcPrepDataContainer* stgcCont,  
			     const MMPrepDataContainer*   mmCont,
			     HoughDataPerSector& houghData ) const;

    void fill( const MdtPrepDataCollection& mdts, HitVec& hits ) const;
    void fill( const TgcPrepDataCollection& tgcs, HitVec& hits, PhiHitVec& phiHits, int sector ) const;
    void fill( const RpcPrepDataCollection& rpcs, HitVec& hits, PhiHitVec& phiHits ) const;
    void fill( const MMPrepDataCollection& mdts, HitVec& hits ) const;
    void fill( const sTgcPrepDataCollection& stgcs, HitVec& hits, PhiHitVec& phiHits, int sector ) const;

    bool findMaxima( MuonHough::MuonLayerHough& hough,
		     HitVec& hits, 
		     MaximumVec& maxima ) const ;
    bool findMaxima( MuonHough::MuonPhiLayerHough& hough,
		     PhiHitVec& hits, 
		     PhiMaximumVec& maxima,
		     int sector ) const;

    void associateMaximaToPhiMaxima( MuonStationIndex::DetectorRegionIndex region, HoughDataPerSector& houghData,
				     std::map< MuonHough::MuonPhiLayerHough::Maximum*, MaximumVec >& phiEtaAssociations,
				     std::vector< MaximumVec >& unassEtaMaxima
				     ) const;

    void associateMaximaInNeighbouringSectors( HoughDataPerSector& houghData, std::vector<HoughDataPerSector>& houghDataPerSectorVec ) const;

    void extendSeed( Road& road, HoughDataPerSector& sectorData ) const; // const;
    void associatePhiMaxima( Road& road, PhiMaximumVec& phiMaxima ) const;

    double combinedPeakheight( double ph,  double ph1,  double ph2, double phn, double rot, int layer, int /*region*/ ) const;
    void updateHits( HitVec& hits, MuonHough::MuonLayerHough& hough ) const;
    void updateHits( PhiHitVec& hits, MuonHough::MuonPhiLayerHough& hough ) const;
    void createPatternCombinations( std::vector< MaximumVec >& maxima,
				    MuonPatternCombinationCollection& patternCombis ) const;

    void createPatternCombinations( std::map< MuonHough::MuonPhiLayerHough::Maximum*, MaximumVec >& phiEtaAssociations,
				    MuonPatternCombinationCollection& patternCombis ) const;

    void fillNtuple( HoughDataPerSectorVec& houghDataPerSectorVec ) const;

    void insertHash( const IdentifierHash& hash, const Identifier& id );
    void insertHash( int sector, const IdentifierHash& hash, const Identifier& id );

    void matchTruth( const PRD_MultiTruthCollection& truthCol, const Identifier& id, MuonHough::HitDebugInfo& debug ) const;
    void initializeSectorMapping();
    void getTruth() const;
    void printTruthSummary( std::set<Identifier>& truth, std::set<Identifier>& found ) const;

    void buildRoads( std::vector<Road>& roads ) const;
    void mergePhiMaxima( Road& road ) const;

    mutable MaximumVec m_seedMaxima;
    bool m_useSeeds;

    mutable MuonHough::MuonDetectorHough m_detectorHoughTransforms;
    mutable HoughDataPerSectorVec        m_houghDataPerSectorVec;

    ToolHandle<MuonIdHelperTool> m_idHelper;
    ToolHandle<MuonEDMPrinterTool> m_printer;
    mutable ToolHandle<Muon::IMuonTruthSummaryTool>         m_truthSummaryTool;
    const MuonGM::MuonDetectorManager* m_detMgr;

    std::vector<MuonHough::MuonLayerHoughSelector> m_selectors;
    std::vector<MuonHough::MuonLayerHoughSelector> m_selectorsLoose;
    bool       m_doNtuple;
    TFile*     m_file;
    TTree*     m_tree;
    mutable MuonHough::HitNtuple* m_ntuple;

    SG::ReadHandleKeyArray< PRD_MultiTruthCollection >       m_truthNames; 
    SG::ReadHandleKey<xAOD::TruthParticleContainer>       m_MuonTruthParticlesKey;
    SG::ReadHandleKey<xAOD::MuonSegmentContainer>       m_MuonTruthSegmentsKey;

    mutable std::set<Identifier>            m_truthHits;
    mutable std::set<Identifier>            m_foundTruthHits;
    mutable std::set<Identifier>            m_outputTruthHits;
    
    bool m_useRpcTimeVeto;
    bool m_requireTriggerConfirmationNSW;
    bool m_onlyUseCurrentBunch;
    bool m_doTruth;
    bool m_debugHough;
    bool m_doParabolicExtrapolation; // if true, do parabolic; if false, do linear extrapolation
    float m_extrapolationDistance; // default value is 1500
    bool m_addSectors; // default true

    unsigned int m_ntechnologies;
    CollectionsPerSectorVec m_collectionsPerSector;
    mutable std::vector<TgcHitClusteringObj*> m_tgcClusteringObjs;

    ServiceHandle< IIncidentSvc >  m_incidentSvc;
    MuonSectorMapping              m_sectorMapping;
  };

  struct SortHoughDataPerSector {
    bool operator()( const MuonLayerHoughTool::HoughDataPerSector* s1, const MuonLayerHoughTool::HoughDataPerSector* s2 ) const {
      return s2->maxEtaHits() < s1->maxEtaHits();
    }
  };

  inline void MuonLayerHoughTool::getSectors( const TgcClusterObj3D& tgc, std::vector<int>& sectors ) const {
    return getSectors(tgc.p11,sectors);
  }

  inline void MuonLayerHoughTool::getSectors( const Amg::Vector3D& pos, std::vector<int>& sectors ) const {
    return m_sectorMapping.getSectors(pos.phi(),sectors);
  }

  inline double MuonLayerHoughTool::rCor( const Amg::Vector3D& pos, const Identifier& id ) const {
    return m_sectorMapping.transformRToSector(pos.perp(),pos.phi(),m_idHelper->sector(id));
  }
  
  inline double MuonLayerHoughTool::rCor( const MuonCluster& mm ) const {
    return rCor(mm.globalPosition(),mm.identify());
  }

  inline double MuonLayerHoughTool::rCor( const MdtPrepData& mm ) const { 
    return rCor(mm.globalPosition(),mm.identify());
  } 
  
  inline double MuonLayerHoughTool::rCor( const TgcClusterObj3D& tgc, int val, int sector ) const {
    const Amg::Vector3D& pos = val == 1 ? tgc.p11 : (val == 2 ? tgc.p12 : (val == 3 ? tgc.p21 : tgc.p22 ) );
    return m_sectorMapping.transformRToSector(pos.perp(),pos.phi(),sector);
  }

  inline int MuonLayerHoughTool::sublay( const Identifier& id, float /*z*/ ) const {
    
    int sublayer = 0;
    if( m_idHelper->isMdt(id) ) {
      sublayer = m_idHelper->mdtIdHelper().tubeLayer(id)-1;
      if( m_idHelper->mdtIdHelper().multilayer(id) == 2 ) sublayer += 4;
    }else if( m_idHelper->isMM(id) ) {
      sublayer = m_idHelper->mmIdHelper().gasGap(id)-1;
      if( m_idHelper->mmIdHelper().multilayer(id) == 2 ) sublayer += 4;
      sublayer += 600; // type info
    }else if( m_idHelper->issTgc(id) ) {
      sublayer = m_idHelper->stgcIdHelper().gasGap(id)-1;
      if( m_idHelper->stgcIdHelper().multilayer(id) == 2 ) sublayer += 4;
      sublayer += 500; // type info
    }else if( m_idHelper->isRpc(id) ){
      sublayer = m_idHelper->rpcIdHelper().gasGap(id)-1;
      if( m_idHelper->rpcIdHelper().doubletR(id) == 2 ) sublayer += 2;
      sublayer += 100; // type info
    }else if( m_idHelper->isTgc(id) ){
      sublayer = m_idHelper->tgcIdHelper().gasGap(id)-1;
      Muon::MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(id);
      if( stIndex == Muon::MuonStationIndex::EM ) {
       //T1 gets +3; T2 gets +3+3; T3 gets +3+6; T4 gets0 (because it is also EI)
	Muon::MuonStationIndex::PhiIndex phiIndex = m_idHelper->phiIndex(id);
       sublayer += 3;
	if( phiIndex == Muon::MuonStationIndex::T2 )       sublayer += 3;
	else if( phiIndex == Muon::MuonStationIndex::T3 )  sublayer += 6;
	// float fz = fabs(z);
	// if( fz < 13350 )      sublayer += 0;
	// else if( fz < 13440 ) sublayer += 3;
	// else if( fz < 13520 ) sublayer += 6;
	// else if( fz < 13600 ) sublayer += 9;
	// else if( fz < 14650 ) sublayer += 12;
	// else if( fz < 14740 ) sublayer += 15;
	// else if( fz < 14800 ) sublayer += 18;
	// else if( fz < 14850 ) sublayer += 21;
	// else if( fz < 15070 ) sublayer += 24;
	// else if( fz < 15150 ) sublayer += 27;
	// else if( fz < 15220 ) sublayer += 30;
	// else if( fz < 15250 ) sublayer += 33;
	// else                  sublayer += 36;
      }
      sublayer += 300; // type info
    }
    return sublayer;
  }


  inline double MuonLayerHoughTool::combinedPeakheight( double ph,  double ph1,  double ph2, double phn, double /*rot*/, int layer, int /*region*/ ) const {
    if( layer == 0 && ph < 3. ) return ph;
    if( layer == 1 && ph < 4. ) return ph;
    if( layer == 2 && ph < 3. ) return ph;

    if( phn > 7 )     ph += phn;

    if( layer == 0 ){
      if( ph1 > 6.9 ) ph += 2;
      if( ph1 > 8.9 ) ph += 2;
      if( ph2 > 5.9 ) ph += 2;
      if( ph2 > 7.9 ) ph += 2;
    }
    if( layer == 1 ){
      if( ph1 > 6.9 )   ph += 2;
      if( ph2 > 5.9 )   ph += 2;
      if( ph2 > 7.9 )   ph += 2;
      if( ph2 > 11.9 )  ph += 2;
    }
    if( layer == 2 ){
      if( ph1 > 6.9 )   ph += 2;
      if( ph2 > 6.9 )   ph += 2;
      if( ph2 > 8.9 )   ph += 2;
    }
    return ph;
  }

}
#endif 
