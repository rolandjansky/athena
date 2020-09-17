/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNTOOLS_MUONLAYERHOUGHTOOL_H
#define MUONHOUGHPATTERNTOOLS_MUONLAYERHOUGHTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecToolInterfaces/IMuonHoughPatternFinderTool.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonLayerHough/HitNtuple.h"

#include "MuonClusterization/TgcHitClustering.h"
#include "MuonClusterization/RpcHitClustering.h"
#include "MuonRecToolInterfaces/IMuonTruthSummaryTool.h"
#include "MuonLayerHough/MuonRegionHough.h"
#include "MuonLayerHough/MuonLayerHough.h"
#include "MuonLayerHough/MuonPhiLayerHough.h"
#include "MuonLayerHough/MuonLayerHoughSelector.h"
#include "MuonHoughPatternTools/HoughDataPerSec.h"

#include <set>
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

namespace Trk{
  class PrepRawData;
}

class TFile;
class TTree;

namespace MuonGM {
  class MuonDetectorManager;
}
namespace MuonHough {
  class HitDebugInfo;
}

static const InterfaceID IID_MuonLayerHoughTool("Muon::MuonLayerHoughTool",1,0);

namespace Muon {

  class MuonLayerHoughTool:  virtual public IMuonHoughPatternFinderTool, public AthAlgTool  {
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

    typedef HoughDataPerSec::HitVec HitVec;
    typedef HoughDataPerSec::RegionHitVec RegionHitVec;
    typedef HoughDataPerSec::PhiHitVec PhiHitVec;
    typedef HoughDataPerSec::RegionPhiHitVec RegionPhiHitVec;
    typedef HoughDataPerSec::MaximumVec MaximumVec;
    typedef HoughDataPerSec::PhiMaximumVec PhiMaximumVec; 
    typedef HoughDataPerSec::MaximumAssociationMap MaximumAssociationMap;
    typedef HoughDataPerSec::RegionMaximumVec RegionMaximumVec;
    typedef HoughDataPerSec::RegionPhiMaximumVec RegionPhiMaximumVec;

    typedef HoughDataPerSec                       HoughDataPerSector;

    typedef Muon::HoughDataPerSectorVec HoughDataPerSectorVec;
    
    
    
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
    virtual ~MuonLayerHoughTool() = default;
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonLayerHoughTool; }

    virtual StatusCode initialize() override;

    virtual StatusCode finalize() override;
    
    std::pair<std::unique_ptr<MuonPatternCombinationCollection>, std::unique_ptr<HoughDataPerSectorVec>> analyse(
                 const MdtPrepDataContainer*  mdtCont,  
					       const CscPrepDataContainer*  cscCols,  
					       const TgcPrepDataContainer*  tgcCont,  
					       const RpcPrepDataContainer*  rpcCont,
					       const sTgcPrepDataContainer* stgcCont,  
					       const MMPrepDataContainer*  mmCont ) const;

    /** find patterns for a give set of MuonPrepData collections + optionally CSC segment combinations */
    virtual std::pair<std::unique_ptr<MuonPatternCombinationCollection>, std::unique_ptr<HoughDataPerSectorVec>>
    find( const std::vector<const MdtPrepDataCollection*>& mdtCols,  
          const std::vector<const CscPrepDataCollection*>& cscCols,  
          const std::vector<const TgcPrepDataCollection*>& tgcCols,  
          const std::vector<const RpcPrepDataCollection*>& rpcCols,  
          const MuonSegmentCombinationCollection* ) const override;

    void reset() const;

  private:

    void getSectors( const Amg::Vector3D& pos, std::vector<int>& sectors ) const;
    void getSectors( const TgcClusterObj3D& tgc, std::vector<int>& sectors ) const;

    double rCor( const Amg::Vector3D& pos, const Identifier& id ) const; 
    double rCor( const MuonCluster& rpc ) const; 
    double rCor( const MdtPrepData& mdt ) const;
    double rCor( const TgcClusterObj3D& tgc, int val, int sector ) const;

    int sublay( const Identifier& id, float z = 0 ) const; // the z value is only used for the tgcs

    struct State {
      MaximumVec seedMaxima; // Does not own the contained objects, they're just references to objects stored in houghDataPerSectorVec.
      std::unique_ptr<HoughDataPerSectorVec> houghDataPerSectorVec { std::make_unique<HoughDataPerSectorVec>() };
      std::set<Identifier> truthHits;
      std::set<Identifier> foundTruthHits;
      std::set<Identifier> outputTruthHits;
    };
  
    std::pair<std::unique_ptr<MuonPatternCombinationCollection>, std::unique_ptr<HoughDataPerSectorVec>> analyse(State& state) const;

    void fillHitsPerSector( std::set<Identifier>& truthHits, 
           std::vector<std::unique_ptr<TgcHitClusteringObj>>& tgcClusteringObjs,
           const CollectionsPerSector& hashes,
			     const MdtPrepDataContainer*  mdtCont,  
			     const CscPrepDataContainer*  /*cscCont*/,  
			     const TgcPrepDataContainer*  tgcCont,  
			     const RpcPrepDataContainer*  rpcCont,
			     const sTgcPrepDataContainer* stgcCont,  
			     const MMPrepDataContainer*   mmCont,
			     HoughDataPerSector& houghData ) const;

    void fill( std::set<Identifier>& truthHits, const MdtPrepDataCollection& mdts, HitVec& hits ) const;
    void fill( std::set<Identifier>& truthHits, std::vector<std::unique_ptr<TgcHitClusteringObj>>& tgcClusteringObjs,
      const TgcPrepDataCollection& tgcs, HitVec& hits, PhiHitVec& phiHits, int sector ) const;
    void fill( std::set<Identifier>& truthHits, const RpcPrepDataCollection& rpcs, HitVec& hits, PhiHitVec& phiHits ) const;
    void fill( std::set<Identifier>& truthHits, const MMPrepDataCollection& mdts, HitVec& hits ) const;
    void fill( std::set<Identifier>& truthHits, const sTgcPrepDataCollection& stgcs, HitVec& hits, PhiHitVec& phiHits, int sector ) const;

    bool findMaxima( std::set<Identifier>& truthHits,
         std::set<Identifier>& foundTruthHits,
         MaximumVec& seedMaxima,
         MuonHough::MuonLayerHough& hough,
		     HitVec& hits, 
		     MaximumVec& maxima ) const ;
    bool findMaxima( std::set<Identifier>& truthHits,
         std::set<Identifier>& foundTruthHits,
         MuonHough::MuonPhiLayerHough& hough,
		     PhiHitVec& hits, 
		     PhiMaximumVec& maxima,
		     int sector ) const;

    void associateMaximaToPhiMaxima( MuonStationIndex::DetectorRegionIndex region, HoughDataPerSector& houghData,
				     std::map< MuonHough::MuonPhiLayerHough::Maximum*, MaximumVec >& phiEtaAssociations,
				     std::vector< MaximumVec >& unassEtaMaxima
				     ) const;

    void associateMaximaInNeighbouringSectors( HoughDataPerSector& houghData, std::vector<HoughDataPerSector>& houghDataPerSectorVec ) const;

    void extendSeed(MuonHough::MuonDetectorHough& detectorHoughTransforms, std::set<Identifier>& truthHits, 
      std::set<Identifier>& foundTruthHits, Road& road, HoughDataPerSector& sectorData ) const; // const;
    void associatePhiMaxima( Road& road, PhiMaximumVec& phiMaxima ) const;

    double combinedPeakheight( double ph,  double ph1,  double ph2, double phn, double rot, int layer, int /*region*/ ) const;
    void updateHits( HitVec& hits, MuonHough::MuonLayerHough& hough ) const;
    void updateHits( PhiHitVec& hits, MuonHough::MuonPhiLayerHough& hough ) const;
    void createPatternCombinations( std::vector< MaximumVec >& maxima,
				    MuonPatternCombinationCollection& patternCombis ) const;

    void createPatternCombinations(std::set<Identifier>& truthHits, std::set<Identifier>& outputTruthHits, 
            std::map< MuonHough::MuonPhiLayerHough::Maximum*, MaximumVec >& phiEtaAssociations,
				    MuonPatternCombinationCollection& patternCombis ) const;

    void fillNtuple( HoughDataPerSectorVec& houghDataPerSectorVec ) const;

    void insertHash( const IdentifierHash& hash, const Identifier& id );
    void insertHash( int sector, const IdentifierHash& hash, const Identifier& id );

    void matchTruth( std::set<Identifier>& truthHits, const PRD_MultiTruthCollection& truthCol, const Identifier& id, MuonHough::HitDebugInfo& debug ) const;
    void initializeSectorMapping(const MuonGM::MuonDetectorManager* detMgr);
    void getTruth() const;
    void printTruthSummary( std::set<Identifier>& truth, std::set<Identifier>& found ) const;

    void buildRoads(MaximumVec& seedMaxima, MuonHough::MuonDetectorHough& detectorHoughTransforms, 
      std::set<Identifier>& truthHits, std::set<Identifier>& foundTruthHits,
      std::unique_ptr<HoughDataPerSectorVec>& houghDataPerSectorVec, std::vector<Road>& roads ) const;
    void mergePhiMaxima( Road& road ) const;

    Gaudi::Property<bool> m_useSeeds{this,"UseSeeds",true};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ToolHandle<MuonEDMPrinterTool> m_printer{this, "printerTool", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
    ToolHandle<Muon::IMuonTruthSummaryTool> m_truthSummaryTool{this, "MuonTruthSummaryTool", "Muon::MuonTruthSummaryTool/MuonTruthSummaryTool"};

    std::vector<MuonHough::MuonLayerHoughSelector> m_selectors;
    std::vector<MuonHough::MuonLayerHoughSelector> m_selectorsLoose;
    Gaudi::Property<bool>       m_doNtuple{this,"DoNtuple",false};
    TFile*     m_file;
    TTree*     m_tree;
    mutable MuonHough::HitNtuple* m_ntuple ATLAS_THREAD_SAFE; // Marked as thread-safe because it's disabled when running multi-threaded

    SG::ReadHandleKeyArray< PRD_MultiTruthCollection >       m_truthNames{this, "TruthNames", {}}; 
    SG::ReadHandleKey<xAOD::TruthParticleContainer>       m_MuonTruthParticlesKey{this,"MuonTruthParticlesKey","MuonTruthParticles"};
    SG::ReadHandleKey<xAOD::MuonSegmentContainer>       m_MuonTruthSegmentsKey{this,"MuonTruthSegmentsKey","MuonTruthSegments"};
    
    Gaudi::Property<bool> m_useRpcTimeVeto{this,"RpcTimeVeto",false};
    Gaudi::Property<bool> m_requireTriggerConfirmationNSW{this,"TriggerConfirmationNSW",false};
    Gaudi::Property<bool> m_onlyUseCurrentBunch{this,"OnlyUseCurrentBunch",false};
    Gaudi::Property<bool> m_doTruth{this,"DoTruth",false};
    Gaudi::Property<bool> m_debugHough{this,"DebugHough",false};
    Gaudi::Property<bool> m_doParabolicExtrapolation{this,"DoParabolicExtrapolation",true}; // if true, do parabolic; if false, do linear extrapolation
    Gaudi::Property<float> m_extrapolationDistance{this,"ExtrapolationDistance",1500.}; // default value is 1500
    Gaudi::Property<bool> m_addSectors{this,"AddSectors",false}; // default false

    unsigned int m_ntechnologies;
    std::map<unsigned int,unsigned int> m_techToTruthNameIdx; // mapping the muon technology to the index of the m_truthNames vector
    CollectionsPerSectorVec m_collectionsPerSector;

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
    return m_sectorMapping.transformRToSector(pos.perp(),pos.phi(),m_idHelperSvc->sector(id));
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
    if( m_idHelperSvc->isMdt(id) ) {
      sublayer = m_idHelperSvc->mdtIdHelper().tubeLayer(id)-1;
      if( m_idHelperSvc->mdtIdHelper().multilayer(id) == 2 ) sublayer += 4;
    }else if( m_idHelperSvc->isMM(id) ) {
      sublayer = m_idHelperSvc->mmIdHelper().gasGap(id)-1;
      if( m_idHelperSvc->mmIdHelper().multilayer(id) == 2 ) sublayer += 4;
      sublayer += 600; // type info
    }else if( m_idHelperSvc->issTgc(id) ) {
      sublayer = m_idHelperSvc->stgcIdHelper().gasGap(id)-1;
      if( m_idHelperSvc->stgcIdHelper().multilayer(id) == 2 ) sublayer += 4;
      sublayer += 500; // type info
    }else if( m_idHelperSvc->isRpc(id) ){
      sublayer = m_idHelperSvc->rpcIdHelper().gasGap(id)-1;
      if( m_idHelperSvc->rpcIdHelper().doubletR(id) == 2 ) sublayer += 2;
      sublayer += 100; // type info
    }else if( m_idHelperSvc->isTgc(id) ){
      sublayer = m_idHelperSvc->tgcIdHelper().gasGap(id)-1;
      Muon::MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(id);
      if( stIndex == Muon::MuonStationIndex::EM ) {
       //T1 gets +3; T2 gets +3+3; T3 gets +3+6; T4 gets0 (because it is also EI)
	Muon::MuonStationIndex::PhiIndex phiIndex = m_idHelperSvc->phiIndex(id);
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
