/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

// local includes
#include "PadTriggerLogicOfflineTool.h"
#include "TrigT1NSWSimTools/PadData.h"
#include "TrigT1NSWSimTools/PadTrigger.h"
#include "PadWithHits.h"
#include "SectorTriggerCandidate.h"
#include "L1TdrStgcTriggerLogic.h"
#include "vector_utils.h"

//Event info includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// Muon software includes
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "MuonAGDDDescription/sTGCDetectorHelper.h"
// trk
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/TrapezoidBounds.h"

// random numbers
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
// local includes
#include "PadUtil.h"
#include "vector_utils.h"
// root
#include "TTree.h"
#include "TVector3.h"
// std
#include <functional>
#include <algorithm>
#include <map>
#include <utility> // make_pair

using namespace std;
using nsw::PadWithHits;

namespace NSWL1 {

//------------------------------------------------------------------------------
PadTriggerLogicOfflineTool::PadTriggerLogicOfflineTool( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent) :
    AthAlgTool(type,name,parent),
    m_incidentSvc("IncidentSvc",name),
    m_rndmSvc("AtRndmGenSvc",name),
    m_rndmEngine(0),
    m_detManager(0),
    // m_sTgcIdHelper(0),
    m_pad_cache_runNumber(-1),
    m_pad_cache_eventNumber(-1),
    // m_pad_cache_status(CLEARED),
    m_rndmEngineName(""),
    m_sTgcDigitContainer(""),
    m_sTgcSdoContainer(""),
    m_PadEfficiency(0.0),
    m_useSimple4of4(false),
    m_doNtuple(false),
    m_missingDetectorManagerErrorCounter(0),
    m_missingReadoutElementErrorCounter(0)
{
    declareInterface<NSWL1::IPadTriggerLogicTool>(this);
    declareProperty("RndmEngineName", m_rndmEngineName = "PadTriggerLogicOfflineTool", "the name of the random engine");
    declareProperty("sTGC_DigitContainerName", m_sTgcDigitContainer = "sTGC_DIGITS", "the name of the sTGC digit container");
    declareProperty("sTGC_SdoContainerName", m_sTgcSdoContainer = "sTGC_SDO", "the name of the sTGC SDO container");
    declareProperty("TimeJitter", m_PadEfficiency = 1.0, "pad trigger efficiency (tmp placeholder)");
    declareProperty("UseSimple4of4", m_useSimple4of4 = false, "use simplified logic requiring 4 hits on 4 gas gaps");
    declareProperty("DoNtuple", m_doNtuple = false, "save the trigger outputs in an analysis ntuple");

    // DG-todo // reserve enough slots for the trigger sectors and fills empty vectors
    // DG-todo m_pad_cache.reserve(32);
    // DG-todo std::vector< std::vector<PadData*> >::iterator it = m_pad_cache.begin();
    // DG-todo m_pad_cache.insert(it,32,std::vector<PadData*>());
}
//------------------------------------------------------------------------------
PadTriggerLogicOfflineTool::~PadTriggerLogicOfflineTool() {
    // clear the internal cache
    // this->clear_cache();
}
//------------------------------------------------------------------------------
// void PadTriggerLogicOfflineTool::clear_cache() {
//     for (std::vector<PadData*> &sector_pads : m_pad_cache) {
//         for (unsigned int p=0; p< sector_pads.size(); p++) {
//             delete sector_pads.at(p);
//         }
//         sector_pads.clear();
//     }
// }
//------------------------------------------------------------------------------
StatusCode PadTriggerLogicOfflineTool::initialize() {
    ATH_MSG_INFO( "initializing " << name() );
    ATH_MSG_INFO( name() << " configuration:");
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left)<< m_rndmEngineName.name() << m_rndmEngineName.value());
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left)<< m_sTgcDigitContainer.name() << m_sTgcDigitContainer.value());
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left)<< m_sTgcSdoContainer.name() << m_sTgcSdoContainer.value());
    // DG-todo print out other parameters

    const IInterface* parent = this->parent();
    const INamedInterface* pnamed = dynamic_cast<const INamedInterface*>(parent);
    std::string algo_name = pnamed->name();

    if ( m_doNtuple && algo_name=="NSWL1Simulation" ) {
        if(TTree *tree = get_tree_from_histsvc()){
            m_validation_tree.init_tree(tree);
        } else {
            ATH_MSG_FATAL("Cannot book requested output tree");
            return StatusCode::FAILURE;
        }
    }
    // retrieve the Incident Service
    if( m_incidentSvc.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve the Incident Service");
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("Incident Service successfully rertieved");
    }
    m_incidentSvc->addListener(this,IncidentType::BeginEvent);

    // retrieve the Random Service
    if( m_rndmSvc.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve the Random Number Service");
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("Random Number Service successfully retrieved");
    }

    // retrieve the random engine
    m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
    if (m_rndmEngine==0) {
        ATH_MSG_FATAL("Could not retrieve the random engine " << m_rndmEngineName);
        return StatusCode::FAILURE;
    }

    //  retrieve the MuonDetectormanager
    if( detStore()->retrieve( m_detManager ).isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve the MuonDetectorManager");
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("MuonDetectorManager successfully retrieved");
    }

// DG-todo    //  retrieve the sTGC offline Id helper
// DG-todo    if( detStore()->retrieve( m_sTgcIdHelper ).isFailure() ){
// DG-todo        //--ATH_MSG_FATAL("Failed to retrieve sTgcIdHelper");
// DG-todo        return StatusCode::FAILURE;
// DG-todo    } else {
// DG-todo        //--ATH_MSG_INFO("sTgcIdHelper successfully retrieved");
// DG-todo    }
// DG-todo

    return StatusCode::SUCCESS;
}
//------------------------------------------------------------------------------
void PadTriggerLogicOfflineTool::handle(const Incident& inc) {
    if( inc.type()==IncidentType::BeginEvent ) {
        // this->clear_cache();
        m_validation_tree.reset_ntuple_variables();
        // m_pad_cache_status = CLEARED;
    }
}
//------------------------------------------------------------------------------

// //! filter a vector (remove elements for which Predicate is true)
// template < template <typename, typename> class Container,
//            typename Predicate,
//            typename Allocator,
//            typename A
//            >
// Container<A, Allocator> filter(Container<A, Allocator> const & container, Predicate const & pred) {
//     Container<A, Allocator> filtered(container);
//     filtered.erase(remove_if(filtered.begin(), filtered.end(), pred), filtered.end());
//     return filtered;
// }

///! helper function: copy pads with a given multiplet
std::vector<PadData*> filterByMultiplet(const std::vector<PadData*> &pads_in, const int &multiplet) {
    std::vector<PadData*> pads_out;
    pads_out.reserve(0.5*pads_in.size()); // educated guess (half inner multiplet, half outer multiplet)
    for(PadData* p : pads_in)
        if(p->multipletId()==multiplet)
            pads_out.push_back(p);
    return pads_out;
}
///! helper function: copy pads with a given gas gap
std::vector<PadData*> filterByGasGap(const std::vector<PadData*> &pads_in, const int &gasgap) {
    std::vector<PadData*> pads_out;
    pads_out.reserve(0.25*pads_in.size()); // educated guess (4 gas gaps)
    for(PadData* p : pads_in)
        if(p->gasGapId()==gasgap)
            pads_out.push_back(p);
    return pads_out;
}

std::vector<PadTrigger*> PadTriggerLogicOfflineTool::build4of4SingleWedgeTriggers(const std::vector<PadData*> &pads) {
    // dummy function: just count four pads on 4 different layers with ieta, iphi == i or i+1
    std::vector<PadData*> padsLayer0 = filterByGasGap(pads, 1);
    std::vector<PadData*> padsLayer1 = filterByGasGap(pads, 2);
    std::vector<PadData*> padsLayer2 = filterByGasGap(pads, 3);
    std::vector<PadData*> padsLayer3 = filterByGasGap(pads, 4);
    std::vector<PadTrigger*> triggers;
    for(const PadData* p0 : padsLayer0){
        int p0ieta = p0->padEtaId();
        int p0iphi = p0->padPhiId();
        for(const PadData* p1 : padsLayer1){
            int p1ieta = p1->padEtaId();
            int p1iphi = p1->padPhiId();
            bool p0_p1_match = ((p1ieta == p0ieta || p1ieta == p0ieta+1 ) &&
                                (p1iphi == p0iphi || p1iphi == p0iphi+1 ) );
            if(not p0_p1_match) continue;
            for(const PadData* p2 : padsLayer2){
                int p2ieta = p2->padEtaId();
                int p2iphi = p2->padPhiId();
                bool p1_p2_match = ((p2ieta == p1ieta || p2ieta == p1ieta+1 ) &&
                                    (p2iphi == p1iphi || p2iphi == p1iphi+1 ) );
                if(not p1_p2_match) continue;
                for(const PadData* p3 : padsLayer3){
                    int p3ieta = p3->padEtaId();
                    int p3iphi = p3->padPhiId();
                    bool p2_p3_match = ((p3ieta == p2ieta || p3ieta == p2ieta+1 ) &&
                                        (p3iphi == p2iphi || p3iphi == p2iphi+1 ) );
                    if(p2_p3_match){
                        PadTrigger* trigger = new PadTrigger();
                        trigger->m_pads.push_back(p0);
                        trigger->m_pads.push_back(p1);
                        trigger->m_pads.push_back(p2);
                        trigger->m_pads.push_back(p3);
                        triggers.push_back(trigger);
                    }
                } // for(p3)
            } // for(p2)
        } // for(p1)
    } // for(p0)
    return triggers;
}
////------------------------------------------------------------------------------
//nsw::PadWithHits PadTriggerLogicOfflineTool::convert(const PadData &pd)
//{
//    PadWithHits pwh(pd);
//    //PadWithHits pwh(pd.padEtaId(), pd.padPhiId(),
//    //                pd.multipletId(),
//    //                pd.gasGapId(), pd.sectorId(), pd.sideId());
//    return pwh;
//}
//------------------------------------------------------------------------------
StatusCode PadTriggerLogicOfflineTool::compute_pad_triggers(const std::vector<PadData*>& pads,
                                                            std::vector<PadTrigger*> &triggers)
{
    ATH_MSG_DEBUG(" <N> receiving "<<pads.size()<<" pad data");
    ATH_MSG_DEBUG("calling compute_pad_triggers() (pads.size() "<<pads.size()<<")");
    for(const PadData *pad : pads){
        ATH_MSG_DEBUG(" <N> building trig from pad "
                      <<" side "<<pad->sideId()<<""
                      <<", sector "<<pad->sectorId()
                      <<", sector type "<<pad->sectorType()
                      <<", module "<<pad->moduleId()
                      <<", multiplet "<<pad->multipletId()
                      <<", gas gap "<<pad->gasGapId()
                      <<", pad eta "<<pad->padEtaId()
                      <<", pad phi "<<pad->padPhiId());
    }
    const vector<size_t> sides = {0, 1}; // see PadTdsOfflineTool::sideLabel()
    const vector<size_t> sectors = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    for(const size_t &side : sides){
        for(const size_t &sector : sectors){
            vector<PadData*> sector_pads;
            copy_if(pads.begin(), pads.end(),
                    back_inserter(sector_pads),
                    [&](const PadData* p) { return (p->sideId()==static_cast<int>(side) &&
                                                    (2*p->sectorId()-1-p->sectorType())==static_cast<int>(sector));});

            if(sector_pads.size()){
                const PadData* firstPad = sector_pads[0];
                ATH_MSG_DEBUG("<N> side "
                              <<(firstPad->sideId()==0?"A":"C")
                              <<" trigger sector "<< (2*firstPad->sectorId()-1-firstPad->sectorType())
                              <<" : "<<sector_pads.size()<<" pads");
                if(m_useSimple4of4) {
                    const int innerMultiplet(1), outerMultiplet(2); // DG-2015-10-07 move to enum?
                    vector<PadData*> padsInner(filterByMultiplet(sector_pads, innerMultiplet));
                    vector<PadData*> padsOuter(filterByMultiplet(sector_pads, outerMultiplet));
                    vector<PadTrigger*> triggersInner = build4of4SingleWedgeTriggers(padsInner);
                    vector<PadTrigger*> triggersOuter = build4of4SingleWedgeTriggers(padsOuter);
                    ATH_MSG_DEBUG("found "
                                  <<triggersInner.size()<<" inner triggers"
                                  <<" and "
                                  <<triggersOuter.size()<<" outer triggers");
                    triggers.reserve(triggers.size() + triggersInner.size()+triggersOuter.size());
                    triggers.insert(triggers.end(), triggersInner.begin(), triggersInner.end());
                    triggers.insert(triggers.end(), triggersOuter.begin(), triggersOuter.end());
                } else {
                    vector<PadWithHits> pwhs;
                    for(const PadData *p : sector_pads){
                        PadWithHits pwh(*p);
                        fillGeometricInformation(*p,pwh);
                        pwhs.push_back(pwh);
                        //pwhs.push_back(convert(*p));
                    }
                    nsw::L1TdrStgcTriggerLogic tdrLogic;
                    tdrLogic.m_writePickle = false;
                    tdrLogic.m_verbose = true;
                    tdrLogic.buildSectorTriggers(pwhs, nsw::indices(pwhs));
                    for(const nsw::SectorTriggerCandidate &st : tdrLogic.candidates())
                        triggers.push_back(new PadTrigger(convert(st)));
                    ATH_MSG_DEBUG("found "<<tdrLogic.candidates().size()<<" triggers using the tdr logic");
                }
            } // if(sector_pads)
        } // for(sector)
    } // for(side)
    // Fill Ntuple
    if(m_doNtuple) {
      m_validation_tree.fill_num_pad_triggers(triggers.size());
      //if(triggers.size()>0)
      //  ATH_MSG_INFO("SERHAN :: " << triggers.at(0)->m_pads.size());
      m_validation_tree.fill_pad_trigger_basics(triggers);
    }
    return StatusCode::SUCCESS;
}
//------------------------------------------------------------------------------
TTree* PadTriggerLogicOfflineTool::get_tree_from_histsvc()
{
    TTree *tree = nullptr;
    ITHistSvc* tHistSvc=NULL;
    m_validation_tree.clear_ntuple_variables();
    if(service("THistSvc", tHistSvc).isFailure()) {
        //--ATH_MSG_FATAL("Unable to retrieve THistSvc");
    } else {
        std::string algoname = dynamic_cast<const INamedInterface*>(parent())->name();
        std::string treename = PadTriggerValidationTree::treename_from_algoname(algoname);
        if(tHistSvc->getTree(treename, tree).isFailure()) {
            ATH_MSG_FATAL(("Could not retrieve the analysis ntuple "+treename+" from the THistSvc").c_str());
        }
    }
    return tree;
}
//------------------------------------------------------------------------------
bool PadTriggerLogicOfflineTool::fillGeometricInformation(const PadData &pd, PadWithHits &pwh)
{
    bool success = false;
    const int maxNumberOfMessages = 100;
    if(m_detManager){
        Identifier Id( pd.id() );
        if(const MuonGM::sTgcReadoutElement* rdoEl = m_detManager->getsTgcReadoutElement(Id)){
            ///////// << == NEW  - ASM-2016-04-25
            const Trk::PlaneSurface &surface = rdoEl->surface(Id);
            std::vector<Amg::Vector2D> local_pad_corners;
            rdoEl->padCorners(Id,local_pad_corners);
            pwh.m_loLocalY = local_pad_corners[0][1];
            pwh.m_hiLocalY = local_pad_corners[2][1];
            Amg::Vector3D pad_corner_global;
            float corner_positions[4][3] = {{0.}};
            for(unsigned int i=0; i<4; i++) {
              //std::cout << "SERHAN :: Local Pad corners are x = " << local_pad_corners[i][0] 
              //                                         << " y = " << local_pad_corners[i][1]
              //                                         << std::endl;
              surface.localToGlobal(local_pad_corners.at(i), pad_corner_global, pad_corner_global);
              corner_positions[i][0] = pad_corner_global.x(); 
              corner_positions[i][1] = pad_corner_global.y(); 
              corner_positions[i][2] = pad_corner_global.z(); 
              //std::cout << "SERHAN :: Gloabal Pad corners are x = " << corner_positions[i][0] 
              //                                           << " y = " << corner_positions[i][1]
              //                                           << " z = " << corner_positions[i][2]   
              //                                           << std::endl;
            }
            pwh.fillCornerCoords(corner_positions);
            ///////// << == NEW  - ASM-2016-04-25
            ///////// << == OLD  - ASM-2016-04-25
            //const Trk::PlaneSurface &surface = rdoEl->surface(Id);
            //const Trk::TrapezoidBounds &bounds = static_cast<Trk::TrapezoidBounds const &>(rdoEl->bounds(Id));
            ///**
            //   @todo DG-2015-10-08 fix pad corners
            //   I am almost sure this is wrong...for now just putting a
            //   placeholder. We are not using the corners but we were
            //   in the TDR.
            //   See TrapezoidBounds docs at
            //   http://atlas-computing.web.cern.ch/atlas-computing/links/nightlyDocDirectory/TrkSurfaces/html/index.html
            //*/
            //Amg::Vector2D pad_lpos;
            //Amg::Vector3D pad_gpos;
            //rdoEl->stripPosition(Id,pad_lpos); // shouldn't this be padPosition? DG 2014-01-17
            //surface.localToGlobal(pad_lpos, pad_gpos, pad_gpos);
            //// ATH_MSG_DEBUG("pad geo: loc "<<pad_lpos<<" glob "<<pad_gpos<<" bounds "<<bounds);
            //float loHeight = pad_lpos.y() - bounds.halflengthY();
            //float hiHeight = pad_lpos.y() + bounds.halflengthY();
            //float loPhi = atan2(pad_lpos.y() - bounds.halflengthY(), pad_lpos.x() - bounds.minHalflengthX());
            //float hiPhi = atan2(pad_lpos.y() + bounds.halflengthY(), pad_lpos.x() + bounds.maxHalflengthX());
            //float zPos = pad_gpos.z();
            //float angleToSec5 = 0.0; // DG-2015-01-08 is this always true in local coords?
            //pwh.fillCornerCoords(loHeight, hiHeight, loPhi, hiPhi, zPos, angleToSec5);
            ///////// << == OLD  - ASM-2016-04-25
        } else {
            if(m_missingReadoutElementErrorCounter < maxNumberOfMessages)
                ATH_MSG_ERROR("fillGeometricInformation: missing detManager "
                              <<m_missingReadoutElementErrorCounter<<"/"<<maxNumberOfMessages);
            m_missingReadoutElementErrorCounter++;
        }
    } else {
        if(m_missingDetectorManagerErrorCounter < maxNumberOfMessages)
            ATH_MSG_ERROR("fillGeometricInformation: missing detManager "
                          <<m_missingDetectorManagerErrorCounter<<"/"<<maxNumberOfMessages);
        m_missingDetectorManagerErrorCounter++;
    }
    return success;
}
//------------------------------------------------------------------------------
NSWL1::PadTrigger PadTriggerLogicOfflineTool::convert(const nsw::SectorTriggerCandidate &t)
{
    PadTrigger pt;
    pt.m_eta    = t.triggerRegion().eta();
    pt.m_phi    = t.triggerRegion().phi();
    // ASM-2016-10-04 currently we compute one bandId for the inner-most single-wedge candidate
    // Shall we return a bandId per pad layer??
    pt.m_pad_strip_info.clear();
    std::vector<float> m_pad_info; 
    const nsw::SingleWedgePadTrigger& swt = t.wedgeTrigs()[0];
    pt.m_bandid = swt.halfPadCoordinates().ieta; 
    pt.m_eta_id = swt.halfPadCoordinates().ieta;
    pt.m_phi_id = swt.halfPadCoordinates().iphi;
    for(const nsw::SingleWedgePadTrigger& swt : t.wedgeTrigs()){
        for(const nsw::PadWithHits &p : swt.pads()){
            if(p.m_padData) {
                pt.m_pads.push_back(p.m_padData);
                // Hack for pad<=>strip until we fix 
                m_pad_info.clear();
                m_pad_info.push_back(p.m_padData->multipletId());
                m_pad_info.push_back(p.m_padData->gasGapId());
                m_pad_info.push_back(p.m_loLocalY);
                m_pad_info.push_back(p.m_hiLocalY);
                pt.m_pad_strip_info.push_back(m_pad_info);
                //cout << "PadStrip Info " << m_pad_info[0] << " " 
                //                         << m_pad_info[1] << " "
                //                         << m_pad_info[2] << " "
                //                         << m_pad_info[3] << endl;
            }
            else
                cerr<<"PadTriggerLogicOfflineTool::convert: do not push back null pointers"<<endl;
        } // for(p)
    } // for (st)
    //cout << "PadStrip :: " << pt.m_pad_strip_info.size() << endl;
    // Offset to BandId - get from the first pad in the trigger
    sTGCDetectorHelper sTGC_helper;
    sTGCDetectorDescription *sTGC=nullptr;
    char side      = pt.m_pads.at(0)->sideId()     == 0 ? 'C' : 'A';
    char type      = pt.m_pads.at(0)->sectorType() == 0 ? 'S' : 'L';
    int stationEta = pt.m_pads.at(0)->moduleId(); //if ( side == 'C' ) stationEta *= -1;
    int stationPhi = pt.m_pads.at(0)->sectorId();
    int layer      = pt.m_pads.at(0)->multipletId();
    sTGC = sTGC_helper.Get_sTGCDetector(type,stationEta,stationPhi,layer,side); 
    if(sTGC == nullptr) {
        cerr<<"PadTriggerLogicOfflineTool::convert :: Could not find detector, no trigger bandid offset being applied"<<endl;  
    }
    else {
        sTGCReadoutParameters  roParams=sTGC->GetReadoutParameters();
        pt.m_bandid += roParams.firstTriggerBand.at(pt.m_pads.at(0)->gasGapId()+1);
    }
    pt.m_multiplet_id = pt.m_pads.at(0)->multipletId(); 
    pt.m_gasGap_id    = pt.m_pads.at(0)->gasGapId();
// << ASM-2016-10-03 == Need to re-implement correctly!!!
// << ASM-2016-10-03 == Need to re-implement correctly!!!
// << ASM-2016-10-03 == Need to re-implement correctly!!!
//    if(t.wedgeTrigs().size()==1) {
//        /// DG-2015-12-10 for now only worry about this simple case; discuss with Jake & Serhan the other cases
//        const nsw::SingleWedgePadTrigger& swt = t.wedgeTrigs()[0];
//        pt.m_bandid = swt.halfPadCoordinates().ieta;
//        /// ASM-2016-01-28 add the offset using Jake's logic in StripTdsOfflineTool
//        cout<<"PadTriggerLogicOfflineTool::convert :: ========================================= " <<endl;
//        cout<<"PadTriggerLogicOfflineTool::convert :: Trigger bandid before offset " << pt.m_bandid<<endl;
//        //for(auto &paddata : pt.m_pads) {
//        //    cout<<"PadTriggerLogicOfflineTool::convert Pad side " << paddata->sideId()
//        //    << " module "    << paddata->moduleId()
//        //    << " sector "    << paddata->sectorId()
//        //    << " multiplet " << paddata->multipletId()
//        //    << " gasGapId "  << paddata->gasGapId()
//        //    << " channelId " << paddata->channelId()
//        //    << " padEtaId "  << paddata->padEtaId()
//        //    << " padPhiId "  << paddata->padPhiId() << endl;
//        //} 
//        char type = ((pt.m_pads.at(0)->sectorId() + 1) % 2)==1 ? 'S' : 'L'; // Even : S - Odd : L
//        int stationEta = pt.m_pads.at(0)->moduleId() + 1;
//        int stationPhi = type == 'S' ? (pt.m_pads.at(0)->sectorId() + 1)*0.5 : (pt.m_pads.at(0)->sectorId() + 2)*0.5;
//        int layer = pt.m_pads.at(0)->gasGapId();
//        //cout<<"PadTriggerLogicOfflineTool::convert :: Station Eta " << stationEta << " Station Phi " << stationPhi << " Layer " << layer <<endl;
//        sTGCDetectorHelper sTGC_helper;
//        sTGCDetectorDescription *sTGC=0;
//        sTGC = sTGC_helper.Get_sTGCDetector(type,stationEta,stationPhi); 
//        if(!sTGC) {
//            cerr<<"PadTriggerLogicOfflineTool::convert :: Could not find detector, no trigger bandid offset being applied"<<endl;  
//        }
//        else {
//            sTGCReadoutParameters  roParams=sTGC->GetReadoutParameters();
//            pt.m_bandid += roParams.firstTriggerBand.at(layer); // Layer
//        }
//        cout<<"PadTriggerLogicOfflineTool::convert :: Trigger bandid after offset " << pt.m_bandid<<endl;
//        cout<<"PadTriggerLogicOfflineTool::convert :: ========================================= " <<endl;
//        /// end of ASM-2016-01-28
//
//    } else if(t.wedgeTrigs().size()>1) {
//        cerr<<"bandId not yet implemented for triggers with inner and outer;"
//            <<" (got "<<t.wedgeTrigs().size()<<" SingleWedgePadTrigger)"<<endl;
//    } else {
//        cerr<<"got a trigger with "<<t.wedgeTrigs().size()<<" SingleWedgePadTrigger)...probably wrong"<<endl;
//    }
// << ASM-2016-10-03 == Need to re-implement correctly!!!
// << ASM-2016-10-03 == Need to re-implement correctly!!!
// << ASM-2016-10-03 == Need to re-implement correctly!!!
    return pt;
}
//------------------------------------------------------------------------------


} // NSWL1

