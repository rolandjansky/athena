/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNALGS_MUONHOUGHPATTERNFINDERTOOL_H
#define MUONHOUGHPATTERNALGS_MUONHOUGHPATTERNFINDERTOOL_H

#include <iostream>
#include <vector>

#include "MuonRecToolInterfaces/IMuonHoughPatternFinderTool.h"
#include "MuonRecToolInterfaces/IMuonCombinePatternTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPattern/MuonPatternCollection.h"
#include "TrkDriftCircleMath/DriftCircle.h"

class TH1F;
class TFile;

class MuonHoughHitContainer;
class StoreGateSvc;
class IMuonHoughPatternTool;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {
  class MuonIdHelperTool;
  class MuonEDMPrinterTool;
}

class RpcIdHelper;
class MdtIdHelper;
class CscIdHelper;
class TgcIdHelper;


namespace Muon {

  class MuonHoughPatternFinderTool  : virtual public IMuonHoughPatternFinderTool, public AthAlgTool
  {
  public:
    /** constructor */
    MuonHoughPatternFinderTool(const std::string&, const std::string&, const IInterface*);

    /** destructor */
    virtual ~MuonHoughPatternFinderTool();

    /** initialize */
    virtual StatusCode initialize();
    /** finalize */
    virtual StatusCode finalize();

    /** find patterns for a give set of MuonPrepData collections + optionally CSC segment combinations */
    MuonPatternCombinationCollection* find( const std::vector<const MdtPrepDataCollection*>& mdtCols,  
					    const std::vector<const CscPrepDataCollection*>& cscCols,  
					    const std::vector<const TgcPrepDataCollection*>& tgcCols,  
					    const std::vector<const RpcPrepDataCollection*>& rpcCols,  
					    const MuonSegmentCombinationCollection* cscSegmentCombis ) const;

  private:

    /** retrieves all hits and converts them into internal EDM */
    const MuonHoughHitContainer* getAllHits( const std::vector<const MdtPrepDataCollection*>& mdtCols,  
					     const std::vector<const CscPrepDataCollection*>& cscCols,  
					     const std::vector<const TgcPrepDataCollection*>& tgcCols,  
					     const std::vector<const RpcPrepDataCollection*>& rpcCols,  
					     const MuonSegmentCombinationCollection* cscSegmentCombis ) const ;

    /** possibility to skip events, nothing implemented */
    bool cut() const;

    /** clean up tool */
    void cleanUp() const;

    /** analyse hits */
    MuonPatternCombinationCollection* analyse( const MuonHoughHitContainer& hitcontainer ) const;


  private:

    /** record patterncollection to storegate or deletes collection when m_recordAllOutput is false */
    void record(const MuonPrdPatternCollection* patCol, const SG::WriteHandleKey<MuonPrdPatternCollection>& key ) const;


    /** convert and add rpc preprawdata collection (1 chamber) */
    void addRpcCollection(Muon::RpcPrepDataCollection::const_iterator cit_begin, Muon::RpcPrepDataCollection::const_iterator cit_end, MuonHoughHitContainer* hitcontainer) const;
    /** convert and add mdt preprawdata collection (1 chamber) */
    void addMdtCollection(Muon::MdtPrepDataCollection::const_iterator cit_begin, Muon::MdtPrepDataCollection::const_iterator cit_end, MuonHoughHitContainer* hitcontainer) const;
    /** convert and add csc preprawdata collection (1 chamber) */
    void addCscCollection(Muon::CscPrepDataCollection::const_iterator cit_begin, Muon::CscPrepDataCollection::const_iterator cit_end, MuonHoughHitContainer* hitcontainer) const;
    /** convert and add tgc preprawdata collection (1 chamber) */
    void addTgcCollection(Muon::TgcPrepDataCollection::const_iterator cit_begin, Muon::TgcPrepDataCollection::const_iterator cit_end, MuonHoughHitContainer* hitcontainer) const;

    /** finds best segment for given driftcircle vector (nl1/2 = number of dc's in ml 1 and 2, angledif is difference between angle of segment and chamberangle, sel is vector of selected hits (0 not selected, 1 selected) */
    void fastSegmentFinder(TrkDriftCircleMath::DCVec& dcs,int &nl1, int &nl2, double &angleDif, std::vector <int> &sel)const;

    /** calculateStationCode(const Identifier)*/
    int calculateStationCode(const Identifier)const;

    /** update station map for rpc chamber, with id of chamber, and size of hits in rpc chamber */
    void updateRpcMdtStationMap(const Identifier rpcid, int hit_begin, int hit_end) const;

    /** map between mdt chamber identifiers and corresponding rpc hits (hit_no_begin and hit_no_end)*/
    mutable std::map<int,std::vector<std::pair<int, int> > > m_rpcmdtstationmap;

    /** update station map for tgc chamber, with id of chamber, and size of hits in tgc chamber */
    void updateTgcMdtStationMap(const Identifier tgcid, int hit_begin, int hit_end) const;

    /** map between mdt chamber identifiers and corresponding tgc hits (hit_no_begin and hit_no_end)*/
    mutable std::map<int,std::vector<std::pair<int, int> > > m_tgcmdtstationmap;

    /** calculates an unique stationcode integer (own convention)*/
    int stationCode(Identifier id)const;

    /** calculates an unique stationcode integer (own convention)*/
    int stationCode(int stationname, int phi, int eta)const;

    void addToStationMap(std::map<int,std::vector<std::pair<int, int> > > &stationmap, std::map<int,std::vector<std::pair<int, int> > >::iterator &it, int &stationcode, const int &hit_begin, const int &hit_end) const;

    /** Pointer to Detectormanager */
    const MuonGM::MuonDetectorManager*  m_detMgr;

    /** Pointer to RpcIdHelper */
    const RpcIdHelper*                  m_rpcIdHelper;
    /** Pointer to TgcIdHelper */
    const TgcIdHelper*                  m_tgcIdHelper;
    /** Pointer to CscIdHelper */
    const CscIdHelper*                  m_cscIdHelper;
    /** Pointer to MdtIdHelper */
    const MdtIdHelper*                  m_mdtIdHelper;

    ToolHandle <IMuonHoughPatternTool>   m_muonHoughPatternTool;    //!< Pointer to concrete tool
    mutable ToolHandle <Muon::IMuonCombinePatternTool>  m_muonCombinePatternTool;   //!< Pointer to concrete tool  
    ToolHandle <Muon::MuonIdHelperTool>  m_idHelperTool;  //!< Pointer to concrete tool
    /** ToolHandle for EDM printing of segments */
    ToolHandle<Muon::MuonEDMPrinterTool> m_printer;
 
    /** reweight hits (true) */
    bool       m_hit_reweights;
    /** use adc cut (true) */
    bool       m_mdt_adc_cut;
    /** value of adc cut (50) */
    int        m_mdt_adc_min;
    /** use tdc cut (false) */
    bool       m_mdt_tdc_cut;

    /** number of events counter, not in use */
    mutable int        m_count;
    /** use rpc preprawdata (true) */
    bool       m_use_rpc;
    /** use tgc preprawdata (true) */
    bool       m_use_tgc;
    /** use csc preprawdata (true) */
    bool       m_use_csc;
    /** use mdt preprawdata (true) */
    bool       m_use_mdt;
    /** use weight for csc segments */
    double     m_weight_csc_on_segment;

    /** reduce cpu for showers (true) */
    bool m_showerskip;
    /** percentage of occupancy to skip MDT chamber (0.3) */
    double m_showerskipperc;

    /** flag to output a root file to study the weights of hits */
    bool m_use_histos;

    /** flag to print out a summary of what comes in and what comes out */
    bool m_summary; 

    /** flag to write out intermediate patterns */
    bool m_recordAllOutput;

    /** map for association between (csc) hits and their segments (only used when m_use_csc_segments is true), used for writing out a MuonSegPatAssMap */
    mutable std::map <const Trk::PrepRawData*, const Muon::MuonSegmentCombination*> m_cschitsegassociation;

    /** storegate location for csc association map */
    std::string m_cscAssoOutputLocation;

    /** pointer to the CSC segment combination collection */
    // const MuonSegmentCombinationCollection* m_csc_segments;

    /** map for association between trigger eta hits (first) and phi hits (second) within the same gasgap, used for combining patterns in MuonCombinePatternTool */
    mutable std::map <const Trk::PrepRawData*, std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> >* m_phietahitassociation;

    /** histogram file for studies on weighting (only in use, when m_use_histos is true) */
    TFile* m_file;
    /** all hits histograms for studies on weighting (only in use, when m_use_histos is true) */
    TH1F* m_weighthistogram;
    /** mdt histogram */
    TH1F* m_weighthistogrammdt;
    /** rpc histogram */
    TH1F* m_weighthistogramrpc;
    /** tgc histogram */
    TH1F* m_weighthistogramtgc;
    /** csc histogram */
    TH1F* m_weighthistogramcsc;

    SG::WriteHandleKey<MuonPrdPatternCollection> m_CosmicPhiPatternsKey;
    SG::WriteHandleKey<MuonPrdPatternCollection> m_CosmicEtaPatternsKey;
    SG::WriteHandleKey<MuonPrdPatternCollection> m_COMBINED_PATTERNSKey;
  };

}

#endif //MUONHOUGHPATTERNALGS_MUONHOUGHPATTERNALG_H
