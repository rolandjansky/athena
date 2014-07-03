/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNTOOLS_MUOLAYERHOUGHTOOL_H
#define MUONHOUGHPATTERNTOOLS_MUOLAYERHOUGHTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
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
#include "TMath.h"
#include <set>
#include "GaudiKernel/IIncidentListener.h"
#include "GeoPrimitives/GeoPrimitives.h"

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


  class MuonLayerHoughTool: public AthAlgTool, virtual public IIncidentListener  {
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
        phiMaximumSet.insert(max);
      }
      MaximumVec    maxima;
      PhiMaximumVec phiMaxima;
      std::set<MuonHough::MuonLayerHough::Maximum*>    maximumSet;
      std::set<MuonHough::MuonPhiLayerHough::Maximum*> phiMaximumSet;

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
					       const MMPrepDataContainer*  mmCont );
    void reset();

    bool tgcInRange( int sector, double phi ) const;
    std::vector<int> getTgcSectors( const Amg::Vector3D& pos ) const;
    std::vector<int> getTgcSectors( const TgcClusterObj3D& tgc ) const;
    std::vector<int> getTgcSectors( double phi ) const;
    double phiRef( int sector ) const;
    double phiCor( double phi, int sector, bool toSector = true ) const;
    double phiOverlap( int sector1, int sector2 ) const;
    bool   closeToSectorBoundary( double phi ) const;
    double rCor( const RpcPrepData& rpc ) const;
    double rCor( const sTgcPrepData& rpc ) const;
    double rCor( const MMPrepData& rpc ) const;
    double rCor( const MdtPrepData& mdt, int sector ) const;
    double rCor( const MdtPrepData& mdt ) const;
    double rCor( const TgcClusterObj3D& tgc, int val, int sector ) const;
    double rCor( const Amg::Vector3D& pos, int sectorHit, int sector ) const;
    double rCor( double r, int sectorHit, int sector ) const;

    int sublay( const Identifier& id, float z = 0 ) const; // the z value is only used for the tgcs

    const MuonHough::MuonDetectorHough& houghTransforms() const { return m_detectorHoughTransforms; }
    const HoughDataPerSectorVec&        houghData() const { return m_houghDataPerSectorVec; }

    /**  incident service handle for EndEvent */
    void handle(const Incident& inc);// maybe in the future clear per event

  private:

    void fillHitsPerSector(  const CollectionsPerSector& hashes,
			     const MdtPrepDataContainer*  mdtCont,  
			     const CscPrepDataContainer*  /*cscCont*/,  
			     const TgcPrepDataContainer*  tgcCont,  
			     const RpcPrepDataContainer*  rpcCont,
			     const sTgcPrepDataContainer* stgcCont,  
			     const MMPrepDataContainer*   mmCont,
			     HoughDataPerSector& houghData );

    void fill( const MdtPrepDataCollection& mdts, HitVec& hits );
    void fill( const TgcPrepDataCollection& tgcs, HitVec& hits, PhiHitVec& phiHits, int sector );
    void fill( const RpcPrepDataCollection& rpcs, HitVec& hits, PhiHitVec& phiHits );
    void fill( const MMPrepDataCollection& mdts, HitVec& hits );
    void fill( const sTgcPrepDataCollection& stgcs, HitVec& hits, PhiHitVec& phiHits, int sector );

    bool findMaxima( MuonHough::MuonLayerHough& hough,
		     HitVec& hits, 
		     MaximumVec& maxima );
    bool findMaxima( MuonHough::MuonPhiLayerHough& hough,
		     PhiHitVec& hits, 
		     PhiMaximumVec& maxima,
		     int sector );

    void associateMaximaToPhiMaxima( MuonStationIndex::DetectorRegionIndex region, HoughDataPerSector& houghData,
				     std::map< MuonHough::MuonPhiLayerHough::Maximum*, MaximumVec >& phiEtaAssociations,
				     std::vector< MaximumVec >& unassEtaMaxima
				     );

    void associateMaximaInNeighbouringSectors( HoughDataPerSector& houghData, std::vector<HoughDataPerSector>& houghDataPerSectorVec ) const;

    void extendSeed( Road& road, HoughDataPerSector& sectorData ) const;
    void associatePhiMaxima( Road& road, PhiMaximumVec& phiMaxima ) const;

    double combinedPeakheight( double ph,  double ph1,  double ph2, double phn, double rot, int layer, int /*region*/ ) const;
    void updateHits( HitVec& hits, MuonHough::MuonLayerHough& hough );
    void updateHits( PhiHitVec& hits, MuonHough::MuonPhiLayerHough& hough );
    void createPatternCombinations( std::vector< MaximumVec >& maxima,
				    MuonPatternCombinationCollection& patternCombis );

    void createPatternCombinations( std::map< MuonHough::MuonPhiLayerHough::Maximum*, MaximumVec >& phiEtaAssociations,
				    MuonPatternCombinationCollection& patternCombis );

    void fillNtuple( HoughDataPerSectorVec& houghDataPerSectorVec );

    void insertHash( const IdentifierHash& hash, const Identifier& id );
    void insertHash( int sector, const IdentifierHash& hash, const Identifier& id );

    void matchTruth( const PRD_MultiTruthCollection& truthCol, const Identifier& id, MuonHough::HitDebugInfo& debug );
    void initializeSectorMapping();
    void getTruth();
    void printTruthSummary( std::set<Identifier>& truth, std::set<Identifier>& found ) const;

    void buildRoads( std::vector<Road>& roads );
    void mergePhiMaxima( Road& road ) const;

    MaximumVec m_seedMaxima;
    bool m_useSeeds;

    MuonHough::MuonDetectorHough m_detectorHoughTransforms;
    HoughDataPerSectorVec        m_houghDataPerSectorVec;

    ToolHandle<MuonIdHelperTool> m_idHelper;
    ToolHandle<MuonEDMPrinterTool> m_printer;
    ToolHandle<Muon::IMuonTruthSummaryTool>         m_truthSummaryTool;
    const MuonGM::MuonDetectorManager* m_detMgr;

    double pifactor; 
    double rangephi[2];
    std::vector<double> m_cutValues;
    std::vector<double> m_cutValuesLoose;

    bool m_doNtuple;
    TFile*     m_file;
    TTree*     m_tree;
    MuonHough::HitNtuple* m_ntuple;

    std::vector< std::string >                     m_truthNames; 
    std::vector< const PRD_MultiTruthCollection* > m_truthCollections;

    std::set<Identifier>            m_truthHits;
    std::set<Identifier>            m_foundTruthHits;
    std::set<Identifier>            m_outputTruthHits;
    

    double m_cutConfirm;
    bool m_useRpcTimeVeto;
    bool m_requireTriggerConfirmationNSW;
    bool m_onlyUseCurrentBunch;
    bool m_doTruth;
    bool m_debugHough;

    unsigned int m_ntechnologies;
    CollectionsPerSectorVec m_collectionsPerSector;
    std::vector<TgcHitClusteringObj*> m_tgcClusteringObjs;

    ServiceHandle< IIncidentSvc >                     m_incidentSvc;
  };

  struct SortHoughDataPerSector {
    bool operator()( const MuonLayerHoughTool::HoughDataPerSector* s1, const MuonLayerHoughTool::HoughDataPerSector* s2 ) const {
      return s2->maxEtaHits() < s1->maxEtaHits();
    }
  };




  inline bool MuonLayerHoughTool::tgcInRange( int sector, double phi ) const {
    int sectorCor = sector - 1;
    if( sector > 8 ) sectorCor -= 16;
    double sectorPhiMin = pifactor*(sectorCor-rangephi[sector%2]);
    double sectorPhiMax = pifactor*(sectorCor+rangephi[sector%2]);
    if( sectorPhiMin < -TMath::Pi() ){
      // swap min and max
      sectorPhiMin += 2*TMath::Pi();
      if( phi < sectorPhiMax || phi > sectorPhiMin ) return true;
      return false;
    }
    if( phi < sectorPhiMin || phi > sectorPhiMax ) return false;
    return true;
  }


  inline std::vector<int> MuonLayerHoughTool::getTgcSectors( const TgcClusterObj3D& tgc ) const {
    return getTgcSectors(tgc.p11);
  }

  inline std::vector<int> MuonLayerHoughTool::getTgcSectors( const Amg::Vector3D& pos ) const {
    return getTgcSectors(atan2(pos.y(),pos.x()));
  }

  inline std::vector<int> MuonLayerHoughTool::getTgcSectors( double phi ) const {
    std::vector<int> sectors;
    double val = phi/pifactor;
    if( val < -0.5 ) val+=0.5;
    else             val+=1.5;
    int sectorTgc = val;
    if( sectorTgc <= 0 ) sectorTgc += 16.;
    //     else                 sectorTgc += 1;
    //   std::cout << " phi " << phi << " sector " << sectorTgc << "   val " << phi/pifactor << " "  << phi/pifactor+0.5 << std::endl;
    
    int sectorTgcNext = sectorTgc != 16 ? sectorTgc + 1 : 1;
    int sectorTgcBefore = sectorTgc != 1 ? sectorTgc -1 : 16;
    if( tgcInRange(sectorTgcBefore,phi) ) {
      sectors.push_back(sectorTgcBefore);
      //     std::cout << " adding before";
    }
    if( tgcInRange(sectorTgc,phi) ){
      sectors.push_back(sectorTgc);
      //     std::cout << " adding current";
    }
    if( tgcInRange(sectorTgcNext,phi) ){
      sectors.push_back(sectorTgcNext);
      //     std::cout << " adding next";
    }
    //   std::cout << std::endl;
    return sectors;
  }
  
  inline bool MuonLayerHoughTool::closeToSectorBoundary( double phi ) const {
    return getTgcSectors(phi).size() > 1;
  }

  inline double MuonLayerHoughTool::phiCor( double phi, int sector, bool toSector ) const {
    double sign = toSector ? -1 : 1;
    double dphi = phi+sign*phiRef(sector);
    if( dphi > TMath::Pi() ) dphi -= 2*TMath::Pi();
    if( dphi < -TMath::Pi() ) dphi += 2*TMath::Pi();
    return dphi;
  }

  inline double MuonLayerHoughTool::phiRef( int sector ) const {
    if( sector < 10 ) return TMath::Pi()*(sector-1)/8.;
    return -TMath::Pi()*(2-(sector-1)/8.);
  }

  inline double MuonLayerHoughTool::phiOverlap( int sector1, int sector2 ) const {
    if( sector1 == sector2 ) return phiRef(sector1);

    int s1 = sector1 < sector2 ? sector1 : sector2;
    int s2 = sector1 > sector2 ? sector1 : sector2;
    if( s2 == 16 && s1 == 1 ){
      s1 = 16;
      s2 = 1;
    }else if( abs(s1-s2) > 1 ){
      std::cout << " bad sector combination: not neighbouring " << s1 << "   " << s2 << std::endl;
      return 0;
    }

    double phi1 = phiRef(s1);
    double sectorWidth1 = TMath::Pi()/8*(rangephi[s1%2]-0.1);
    double phio1 = phi1 + sectorWidth1;
    if( phio1 > TMath::Pi() ) phio1 -= 2*TMath::Pi();

/*     double phi2 = phiRef(s2); */
/*     double sectorWidth2 = TMath::Pi()/8*(rangephi[s2%2]-0.1); */
/*     double phio2 = phi2 - sectorWidth2; */
/*     if( phio1 < -TMath::Pi() ) phio1 += 2*TMath::Pi(); */

/*     std::cout << " phi1 " << phi1 << " phi2 " << phi2 << " phio1 " << phio1 << " phio2 " << phio2 << " diff " << phio1-phio2 << std::endl; */
    
    return phio1;
  }

  inline double MuonLayerHoughTool::rCor( const MMPrepData& mm ) const {
    double r    = mm.globalPosition().perp();
    double phi  = atan2(mm.globalPosition().y(),mm.globalPosition().x());
    int sector  = m_idHelper->sector(mm.identify());
    double dphi = phiCor(phi,sector);

     if( fabs(dphi) > 0.3 ){
      ATH_MSG_WARNING(" large dphi detected!!: sector " << sector << " phi "  << phiRef(sector)
                      << " mm " << phi << " dphi " << dphi);
    }
    return r*cos(dphi);
  }
  inline double MuonLayerHoughTool::rCor( const sTgcPrepData& stgc ) const {
    double r    = stgc.globalPosition().perp();
    double phi  = atan2(stgc.globalPosition().y(),stgc.globalPosition().x());
    int sector  = m_idHelper->sector(stgc.identify());
    double dphi = phiCor(phi,sector);

     if( fabs(dphi) > 0.3 ){
      ATH_MSG_WARNING(" large dphi detected!!: sector " << sector << " phi "  << phiRef(sector)
                      << " sTgc " << phi << " dphi " << dphi);
    }
    return r*cos(dphi);
  }
  inline double MuonLayerHoughTool::rCor( const RpcPrepData& rpc ) const {
    double r = rpc.globalPosition().perp();
    double phi = atan2(rpc.globalPosition().y(),rpc.globalPosition().x());
    int sector = m_idHelper->sector(rpc.identify());
    double dphi = phiCor(phi,sector);
    /*
    double phi_mdt = phiRef(sector);
    double dphi = phi-phi_mdt;
    if( dphi < -TMath::Pi() ) dphi += 2* TMath::Pi();
    */
    if( fabs(dphi) > 0.3 ){
      ATH_MSG_WARNING(" large dphi detected!!: sector " << sector << " phi "  << phiRef(sector)
		      << " rpc " << phi << " dphi " << dphi);
    }
    return r*cos(dphi);
  }

  inline double MuonLayerHoughTool::rCor( const MdtPrepData& mdt, int sector ) const {
    return rCor(mdt.globalPosition(),m_idHelper->sector(mdt.identify()),sector);
  }

  inline double MuonLayerHoughTool::rCor( const MdtPrepData& mdt ) const { 
    double r=mdt.globalPosition().perp(); 
    double phi=atan2(mdt.globalPosition().y(),mdt.globalPosition().x()); 
    int msector = m_idHelper->sector(mdt.identify()); 
    double dphi = phiCor(phi,msector); 
    if( fabs(dphi) > 0.3 ){ 
      ATH_MSG_WARNING(" large dphi detected!!: sector " << msector << " phi "  << phiRef(msector) 
		      << " mdt " << phi << " dphi " << dphi); 
    } 
    return r*cos(dphi); 
  } 
  
  inline double MuonLayerHoughTool::rCor( const Amg::Vector3D& pos, int sectorHit, int sector ) const {
    double r = pos.perp();
    double phi = atan2(pos.y(),pos.x());
    double phio = phiOverlap(sectorHit,sector);
    double dphio = phi-phio;
    if( dphio < -TMath::Pi() ) dphio += 2* TMath::Pi();
    double redge = r/cos(dphio);
    double phi_sec = phiRef(sector);
    double dphi = phio-phi_sec;
    if( dphi < -TMath::Pi() ) dphi += 2* TMath::Pi();
    if( fabs(dphi) > 0.3 ){
      ATH_MSG_WARNING(" large dphi detected!!: sector " << sector << " of hit " << sectorHit << " phi ref sector "  << phi_sec
		      << " hit " << phi << " dphi " << dphi);
    }
    return redge*cos(dphi);
  }

  inline double MuonLayerHoughTool::rCor( double r, int sectorHit, int sector ) const {
    double phi = phiRef(sectorHit);
    double phio = phiOverlap(sectorHit,sector);
    double dphio = phi-phio;
    if( dphio < -TMath::Pi() ) dphio += 2* TMath::Pi();
    double redge = r/cos(dphio);
    double phi_sec = phiRef(sector);
    double dphi = phio-phi_sec;
    if( dphi < -TMath::Pi() ) dphi += 2* TMath::Pi();
    if( fabs(dphi) > 0.3 ){
      ATH_MSG_WARNING(" large dphi detected!!: sector " << sector << " of hit " << sectorHit << " phi ref sector "  << phi_sec
		      << " hit " << phi << " dphi " << dphi);
    }
    return redge*cos(dphi);
  }

  inline double MuonLayerHoughTool::rCor( const TgcClusterObj3D& tgc, int val, int sector ) const {
    double x = val == 1 ? tgc.p11.x() : val == 2 ? tgc.p12.x() : val == 3 ? tgc.p21.x() : tgc.p22.x();
    double y = val == 1 ? tgc.p11.y() : val == 2 ? tgc.p12.y() : val == 3 ? tgc.p21.y() : tgc.p22.y();
    double r = sqrt(x*x+y*y);
    double phi = atan2(y,x);
    double phi_sec = phiRef(sector);
    double dphi = phi-phi_sec;
    if( dphi < -TMath::Pi() ) dphi += 2* TMath::Pi();
    // if( fabs(dphi) > 0.3 ){
    //   ATH_MSG_WARNING(" large dphi detected!!: sector " << sector << " phi "  << phi_sec
    //     	      << " tgc " << phi << " dphi " << dphi);
    // }
    return r*cos(dphi);
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

	Muon::MuonStationIndex::PhiIndex phiIndex = m_idHelper->phiIndex(id);
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
