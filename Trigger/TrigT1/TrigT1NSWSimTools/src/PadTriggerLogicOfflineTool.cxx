/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
// local includes
#include "TrigT1NSWSimTools/PadTriggerLogicOfflineTool.h"
#include "TrigT1NSWSimTools/PadData.h"
#include "TrigT1NSWSimTools/PadOfflineData.h"
#include "TrigT1NSWSimTools/PadTrigger.h"
#include "TrigT1NSWSimTools/SectorTriggerCandidate.h"
#include "TrigT1NSWSimTools/SingleWedgePadTrigger.h"
#include "TrigT1NSWSimTools/tdr_compat_enum.h"
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


// root
#include "TTree.h"
#include "TVector3.h"
// std
#include <functional>
#include <algorithm>
#include <map>
#include <utility> // make_pair


namespace NSWL1 {
//------------------------------------------------------------------------------
PadTriggerLogicOfflineTool::PadTriggerLogicOfflineTool( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent) :
    AthAlgTool(type,name,parent),
    m_incidentSvc("IncidentSvc",name),
    m_detManager(0),
    m_pad_cache_runNumber(-1),
    m_pad_cache_eventNumber(-1),
    m_rndmEngineName(""),
    m_sTgcDigitContainer(""),
    m_sTgcSdoContainer(""),
    m_PadEfficiency(0.0),
    m_useSimple4of4(false),
    m_doNtuple(false),
    m_tdrLogic()
{
    declareInterface<NSWL1::IPadTriggerLogicTool>(this);
    declareProperty("TimeJitter", m_PadEfficiency = 1.0, "pad trigger efficiency (tmp placeholder)");
    declareProperty("UseSimple4of4", m_useSimple4of4 = false, "use simplified logic requiring 4 hits on 4 gas gaps");
    declareProperty("DoNtuple", m_doNtuple = false, "save the trigger outputs in an analysis ntuple");
}
//------------------------------------------------------------------------------
PadTriggerLogicOfflineTool::~PadTriggerLogicOfflineTool() {

}

StatusCode PadTriggerLogicOfflineTool::initialize() {
    ATH_MSG_INFO( "initializing " << name() );
    ATH_MSG_INFO( name() << " configuration:");


    const IInterface* parent = this->parent();
    const INamedInterface* pnamed = dynamic_cast<const INamedInterface*>(parent);
    std::string algo_name = pnamed->name();

    if ( m_doNtuple && algo_name=="NSWL1Simulation" ) {
        if(TTree *tree = get_tree_from_histsvc()){
            m_validation_tree.init_tree(tree);
        } else {
            return STATUSCODE(StatusCode::FAILURE, IssueSeverity::FATAL, "cannot book requested output tree");
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



    //  retrieve the MuonDetectormanager
    if( detStore()->retrieve( m_detManager ).isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve the MuonDetectorManager");
        return StatusCode::FAILURE;
    } 
    else {
        ATH_MSG_INFO("MuonDetectorManager successfully retrieved");
    }
    
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


void PadTriggerLogicOfflineTool::fillGeometricInformation(const std::shared_ptr<PadOfflineData>& pod){
    
    const MuonGM::sTgcReadoutElement* rdoEl = m_detManager->getsTgcReadoutElement(pod->Id());
    const Trk::PlaneSurface &surface = rdoEl->surface(pod->Id());
    std::vector<Amg::Vector2D> local_pad_corners;
    //From MuonPadDesign... read pad local corners
    rdoEl->padCorners(pod->Id(),local_pad_corners);
    Amg::Vector3D pad_corner_global;
    for(unsigned int i=0; i<4; i++) {
        surface.localToGlobal(local_pad_corners.at(i), pad_corner_global, pad_corner_global);
        pod->m_cornerXyz[i][0] = pad_corner_global.x(); 
        pod->m_cornerXyz[i][1] = pad_corner_global.y(); 
        pod->m_cornerXyz[i][2] = pad_corner_global.z(); 
    }
} 




///! helper function: copy pads with a given multiplet
std::vector<std::shared_ptr<PadData>> filterByMultiplet(const std::vector<std::shared_ptr<PadData>> &pads_in, const int &multiplet) {
    std::vector<std::shared_ptr<PadData>> pads_out;
    pads_out.reserve(0.5*pads_in.size()); // educated guess (half inner multiplet, half outer multiplet)
    for(auto p : pads_in)
        if(p->multipletId()==multiplet)
            pads_out.push_back(p);
    return pads_out;
}
///! helper function: copy pads with a given gas gap
std::vector<std::shared_ptr<PadData>> filterByGasGap(const std::vector<std::shared_ptr<PadData>> &pads_in, const int &gasgap) {
    std::vector<std::shared_ptr<PadData>> pads_out;
    pads_out.reserve(0.25*pads_in.size()); // educated guess (4 gas gaps)
    for(auto p : pads_in)
        if(p->gasGapId()==gasgap)
            pads_out.push_back(p);
    return pads_out;
}

std::vector<std::unique_ptr<PadTrigger>> PadTriggerLogicOfflineTool::build4of4SingleWedgeTriggers(const std::vector<std::shared_ptr<PadData>> &pads) {

    std::vector<std::unique_ptr<PadTrigger>> triggers;
    for(auto p0 : filterByGasGap(pads, 1)){
        int p0ieta = p0->padEtaId();
        int p0iphi = p0->padPhiId();
        for(auto p1 : filterByGasGap(pads, 2)){
            int p1ieta = p1->padEtaId();
            int p1iphi = p1->padPhiId();
            bool p0_p1_match = ((p1ieta == p0ieta || p1ieta == p0ieta+1 ) &&
                                (p1iphi == p0iphi || p1iphi == p0iphi+1 ) );
            if(not p0_p1_match) continue;
            for(auto p2 :filterByGasGap(pads, 3) ){
                int p2ieta = p2->padEtaId();
                int p2iphi = p2->padPhiId();
                bool p1_p2_match = ((p2ieta == p1ieta || p2ieta == p1ieta+1 ) &&
                                    (p2iphi == p1iphi || p2iphi == p1iphi+1 ) );
                if(not p1_p2_match) continue;
                for(auto p3 : filterByGasGap(pads, 4)){
                    int p3ieta = p3->padEtaId();
                    int p3iphi = p3->padPhiId();
                    bool p2_p3_match = ((p3ieta == p2ieta || p3ieta == p2ieta+1 ) &&
                                        (p3iphi == p2iphi || p3iphi == p2iphi+1 ) );
                    if(p2_p3_match){
                        auto trg=std::make_unique<PadTrigger>();
                        trg->m_pads.push_back(p0);
                        trg->m_pads.push_back(p1);
                        trg->m_pads.push_back(p2);
                        trg->m_pads.push_back(p3);
                        triggers.push_back(std::move(trg));
                    }
                } // for(p3)
            } // for(p2)
        } // for(p1)
    } // for(p0)
    return triggers;
}


StatusCode PadTriggerLogicOfflineTool::compute_pad_triggers(const std::vector<std::shared_ptr<PadData>>& pads,
                                                            std::vector<std::unique_ptr<PadTrigger>> &triggers)
{
    ATH_MSG_DEBUG(" <N> receiving "<<pads.size()<<" pad data");
    ATH_MSG_DEBUG("calling compute_pad_triggers() (pads.size() "<<pads.size()<<")");
    for(const auto pad : pads){
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
    for(const size_t &side : SIDES){
        for(const size_t &sector : SECTORS){
            std::vector<std::shared_ptr<PadData>> sector_pads;
            copy_if(pads.begin(), pads.end(),
                    back_inserter(sector_pads),
                    [&](std::shared_ptr<PadData> p) { return (p->sideId()==static_cast<int>(side) &&
                                                    (2*p->sectorId()-1-p->sectorType())==static_cast<int>(sector));});

            if(sector_pads.size()){
                const std::shared_ptr<PadData> firstPad = sector_pads[0];
                ATH_MSG_DEBUG("<N> side "
                              <<(firstPad->sideId()==0?"A":"C")
                              <<" trigger sector "<< (2*firstPad->sectorId()-1-firstPad->sectorType())
                              <<" : "<<sector_pads.size()<<" pads");
                if(m_useSimple4of4) {
                    const int innerMultiplet(1), outerMultiplet(2); // DG-2015-10-07 move to enum?
                    std::vector<std::shared_ptr<PadData>> padsInner(filterByMultiplet(sector_pads, innerMultiplet));
                    std::vector<std::shared_ptr<PadData>> padsOuter(filterByMultiplet(sector_pads, outerMultiplet));
                    std::vector<std::unique_ptr<PadTrigger>> triggersInner = build4of4SingleWedgeTriggers(padsInner);
                    std::vector<std::unique_ptr<PadTrigger>> triggersOuter = build4of4SingleWedgeTriggers(padsOuter);
                    ATH_MSG_DEBUG("found "
                                  <<triggersInner.size()<<" inner triggers"
                                  <<" and "
                                  <<triggersOuter.size()<<" outer triggers");
                     triggers.reserve(triggers.size() + triggersInner.size()+triggersOuter.size());

                    triggers.insert(triggers.end(),std::make_move_iterator(triggersInner.begin()),std::make_move_iterator(triggersInner.end()));
                    triggers.insert(triggers.end(),std::make_move_iterator(triggersOuter.begin()),std::make_move_iterator(triggersOuter.end()));
                } 
                else {
                  std::vector<std::shared_ptr<PadOfflineData>> trgpads;
                  for(const auto& p : sector_pads){
                     auto pod=std::dynamic_pointer_cast<PadOfflineData> (p);
                       fillGeometricInformation(pod);
                       trgpads.push_back(pod);
                  }
                     m_tdrLogic.buildSectorTriggers(trgpads);
                     int index=0;
                     for( const auto& st : m_tdrLogic.candidates()){
                        auto p=std::make_unique<PadTrigger>(convert(st));
                        p->m_triggerindex=index;
                        if (p->m_pads.size()==0) continue;//dont record null triggers (rejected or empty)
                        triggers.push_back(std::move(p));
                        index++;
                     }
                     ATH_MSG_DEBUG("found "<<m_tdrLogic.candidates().size()<<" triggers using the tdr logic");
               }
            } // if(sector_pads)
        } // for(sector)
    } // for(side)
    // Fill Ntuple
    if(m_doNtuple) {
      m_validation_tree.fill_num_pad_triggers(triggers.size());
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
NSWL1::PadTrigger PadTriggerLogicOfflineTool::convert(const SectorTriggerCandidate &stc){
    //check if [b0,b1] is within [a0,a1]
    static auto within=[](const auto& a0,const auto&  a1,const auto&  b0,const auto&  b1,const auto& tol){
        if(a0<=b0 && a1>=b1) return true;
        
        if(b0<a0 && b1<a1){
            if(fabs(b0-a0)<tol){
                return true;
            }
        }
        if(b0<a0 && b1>a1){
            if(fabs(b0-a0)<tol && fabs(b1-a1)<tol){
                return true;
            }
        }
        
        if(b1>a1 && a0<b0){
            if(fabs(b1-a1)<tol) return true;
        }
        
        if(b1>a1 && b0>a0){
            if(fabs(b1-a1)<tol && fabs(b0-a0)<tol) return true;
        }
        return false;
    };
    
    
    PadTrigger pt;
    const Polygon roi=stc.triggerRegion3();    
    Vertex trgCntr=centroid(roi);
    SingleWedgePadTrigger innertrg = stc.wedgeTrigs().at(0);
    
    //*********** assign central etaphi cordinates of the triggering region *****************
    const float xcntr=coordinate<0>(trgCntr);
    const float ycntr=coordinate<1>(trgCntr);
    const float zcntr=innertrg.pads().at(0)->m_cornerXyz[1][2];
    TVector3 trigVector(xcntr,ycntr,zcntr);
    const float etaTrig=trigVector.Eta();
    const float phiTrig=trigVector.Phi();
    pt.m_eta    = etaTrig;
    pt.m_phi    = phiTrig;
    //**************************************************************************************
    auto pad0=innertrg.pads().at(0);
    
    pt.m_multiplet_id = pad0->multipletId();
    pt.m_eta_id = innertrg.halfPadCoordinates().ieta;//this is meaningless and shoiuld be removed
    pt.m_phi_id = innertrg.halfPadCoordinates().iphi;//This is the PHI-ID of the trigger 
    pt.m_isSmall= int(innertrg.isSmallSector());
    
    //************** assign extrema of the trigger region coordinates in eta-phi **************
    std::vector<std::pair<float,float>> trg_etaphis;
    for(auto v : boost::geometry::exterior_ring(roi)){
        const float xcurr=coordinate<0>(v);
        const float ycurr=coordinate<1>(v);
        const float zcurr=zcntr;
        const float etacurr=eta(xcurr,ycurr,zcurr);
        const float phicurr=phi(xcurr,ycurr,zcurr);
        trg_etaphis.emplace_back(etacurr,phicurr);
    }
    const auto trg_etaminmax=std::minmax_element(trg_etaphis.begin(),trg_etaphis.end(),[](const auto& l,const auto& r){return l.first<r.first;});
    const auto trg_phiminmax=std::minmax_element(trg_etaphis.begin(),trg_etaphis.end(),[](const auto& l,const auto& r){return l.second<r.second;});
    pt.m_etamin=(trg_etaminmax.first)->first;
    pt.m_etamax=(trg_etaminmax.second)->first;
    pt.m_phimin=(trg_phiminmax.first)->second;
    pt.m_phimax=(trg_phiminmax.second)->second;
    
    pt.m_moduleIdInner=-1;
    pt.m_moduleIdOuter=-1;
    
    if(stc.wedgeTrigs().size()==2){//double wedge trigger
        pt.m_moduleIdInner=stc.wedgeTrigs().at(0).pads().at(0)->moduleId();
        pt.m_moduleIdOuter=stc.wedgeTrigs().at(1).pads().at(1)->moduleId();
    }
    else{//single wedge trigger
        //if the first one is inner set m_moduleIdInner
        int multId0=stc.wedgeTrigs().at(0).pads().at(0)->multipletId();
        if( multId0==1){
            pt.m_moduleIdInner=stc.wedgeTrigs().at(0).pads().at(0)->moduleId();
        }
        //if the first one is outer set m_moduleIdOuter
        else{
            pt.m_moduleIdOuter=stc.wedgeTrigs().at(0).pads().at(0)->moduleId();
        }
        //one of the module Ids remain as -1;
    }

    //****************************************************************************************

     //S.I value of Z where trigger region is calculated.
     //from Z0 --> <Z of a pad> --->local coordinate

    for(const SingleWedgePadTrigger& swt : stc.wedgeTrigs()){
        int currwedge=swt.pads().at(0)->multipletId();
        std::vector<float> trglocalminY;
        std::vector<float> trglocalmaxY;
        std::vector<int> trgSelectedLayers;
        std::vector<int> trgSelectedBands;
        std::vector<int> trgPadPhiIndices;
        std::vector<int> trgPadEtaIndices;
        std::vector< std::shared_ptr<PadData>> trgPads;
    	for(const auto &p : swt.pads()){
                //S.I 17-07-18
                const float padZ=p->m_cornerXyz[0][2];
    			const Polygon pol=Project(roi,zcntr,padZ);
    			Identifier Id( p->id());
    			const Trk::PlaneSurface &padsurface = m_detManager->getsTgcReadoutElement(Id)->surface(Id);
    			std::vector<Amg::Vector2D> local_trgcorners;// corners of the triggering region as projected on the detector layer
                std::vector<Amg::Vector2D> local_padcorners;//pad's corners written in local coordinate system
                
                for(unsigned int i=0;i<4;i++){
                    const Amg::Vector3D global_padcorner(p->m_cornerXyz[i][0],p->m_cornerXyz[i][1],padZ);
                    Amg::Vector2D local_padcorner;
                    padsurface.globalToLocal(global_padcorner,Amg::Vector3D(),local_padcorner);
                    local_padcorners.push_back(local_padcorner);
                }

    			for(const auto& v : boost::geometry::exterior_ring(pol)){
    				float x=coordinate<0>(v);
    				float y=coordinate<1>(v);
    				const Amg::Vector3D globalcorner(x,y,padZ);
    				Amg::Vector2D localcorner;
    				padsurface.globalToLocal(globalcorner,globalcorner,localcorner);
    				local_trgcorners.push_back(localcorner);
    			}
                static auto compare_y=[](const Amg::Vector2D &lhs, const Amg::Vector2D& rhs){return lhs.y()<rhs.y();};
                auto local_padminmaxy=std::minmax_element(local_padcorners.begin(),local_padcorners.end(),compare_y);
    			float local_padminy=local_padminmaxy.first->y();
    			float local_padmaxy=local_padminmaxy.second->y();
                const auto local_trigminmaxy=std::minmax_element(local_trgcorners.begin(),local_trgcorners.end(),compare_y);
                float local_trigminy=local_trigminmaxy.first->y();
                float local_trigmaxy=local_trigminmaxy.second->y();
                float padMidY=0.5*(local_padminy+local_padmaxy);  
                //select the band that matches to the trigger region within given tolerance (now set to 1 strip pitch 3.2 mm)
                bool bandisUp=within(padMidY,local_padmaxy,local_trigminy,local_trigmaxy,3.2);
                bool bandisDown=within(local_padminy,padMidY, local_trigminy,local_trigmaxy,3.2);
                int selectedbandId=-1;
                int bandOffset=0;
                char side      = p->sideId()     == 0 ? 'C' : 'A';
                char type      = p->sectorType() == 0 ? 'S' : 'L';
                int stationEta = p->moduleId(); //this is also meaningless as we might have different inner-outer module IDs in transition
                int multiplet=p->multipletId();
                int layer=p->gasGapId();
                //if ( side == 'C' ) stationEta *= -1;
                int stationPhi = p->sectorId();
                static sTGCDetectorHelper sTGC_helper;
                static auto stationEtas={1,2,3};
                
                float bandLocalMinY=-1;
                float bandLocalMaxY=-1;
                
                for(const auto& ieta : stationEtas){
                    if(ieta==stationEta) break;
                    bandOffset+=sTGC_helper.Get_sTGCDetector(type,ieta,stationPhi,multiplet,side)->GetReadoutParameters().nTriggerBands.at(layer-1);
                    bandOffset+=0;//do not apply band Offsets as strip channels start from 1 on the next module;
                }
                if(bandisUp ){
                      selectedbandId=p->padEtaId()*2;
                      bandLocalMinY=padMidY;
                      bandLocalMaxY=local_padmaxy;
                }
                else if(bandisDown){
                      selectedbandId=p->padEtaId()*2-1;
                      bandLocalMinY=local_padminy;
                      bandLocalMaxY=padMidY;
                }
                else{

                     return PadTrigger();
                }
                
                selectedbandId+=bandOffset;
                trglocalminY.push_back(bandLocalMinY);
                trglocalmaxY.push_back(bandLocalMaxY);
                trgSelectedLayers.push_back(p->gasGapId());
                trgSelectedBands.push_back(selectedbandId);
                trgPadPhiIndices.push_back(p->padPhiId());
                trgPadEtaIndices.push_back(p->padEtaId());
                trgPads.push_back(p);
    			pt.m_pads.push_back(p);
                
                //it seems pad overlap is  precise enough around 5microns .
                if(!within(local_padminy,local_padmaxy,local_trigminy,local_trigmaxy,0.005)){
                    ATH_MSG_FATAL("TRIGGER REGION FALLS OUTSIDE THE PAD!. SOMETHING IS WRONG.");
                }
    	} // for(p) pads
        if(currwedge==1){
            pt.m_trglocalminYInner=trglocalminY;
            pt.m_trglocalmaxYInner=trglocalmaxY;
            pt.m_trgSelectedLayersInner=trgSelectedLayers;
            pt.m_trgSelectedBandsInner=trgSelectedBands;
            pt.m_trgPadPhiIndicesInner=trgPadPhiIndices;
            pt.m_trgPadEtaIndicesInner=trgPadEtaIndices;
            pt.m_padsInner=trgPads;
        }
        if(currwedge==2){
            pt.m_trglocalminYOuter=trglocalminY;
            pt.m_trglocalmaxYOuter=trglocalmaxY;                
            pt.m_trgSelectedLayersOuter=trgSelectedLayers;
            pt.m_trgSelectedBandsOuter=trgSelectedBands;
            pt.m_trgPadPhiIndicesOuter=trgPadPhiIndices;
            pt.m_trgPadEtaIndicesOuter=trgPadEtaIndices;
            pt.m_padsOuter=trgPads;
        }    	
	

        trglocalminY.clear();
        trglocalmaxY.clear();
        trgSelectedLayers.clear();
        trgSelectedBands.clear();
        trgPadPhiIndices.clear();
        trgPadEtaIndices.clear();
        trgPads.clear();
        
    } // for (swt) single wedge trigger

    pt.m_bandid=pt.m_trgSelectedBandsInner.size() >0? pt.m_trgSelectedBandsInner.at(0) : pt.m_trgSelectedBandsOuter.at(0);
    return pt;
}


//------------------------------------------------------------------------------


} // NSWL1

