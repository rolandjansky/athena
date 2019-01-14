/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
#include "TrigT1NSWSimTools/PadTrigger.h"
#include "TrigT1NSWSimTools/PadWithHits.h"
#include "TrigT1NSWSimTools/SectorTriggerCandidate.h"
#include "TrigT1NSWSimTools/SingleWedgePadTrigger.h"

#include "TrigT1NSWSimTools/vector_utils.h"

//Event info includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// Muon software includes
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "MuonAGDDDescription/sTGCDetectorHelper.h"
// trk
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/TrapezoidBounds.h"

// local includes
#include "TrigT1NSWSimTools/PadUtil.h"
#include "TrigT1NSWSimTools/vector_utils.h"

// root
#include "TTree.h"
#include "TVector3.h"
// std
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
    m_PadEfficiency(0.0),
    m_useSimple4of4(false),
    m_doNtuple(false),
    m_missingDetectorManagerErrorCounter(0),
    m_missingReadoutElementErrorCounter(0),
    m_tdrLogic()
{
    declareInterface<NSWL1::IPadTriggerLogicTool>(this);
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
}

StatusCode PadTriggerLogicOfflineTool::initialize() {
    ATH_MSG_INFO( "initializing " << name() );
    ATH_MSG_INFO( name() << " configuration:");
    // DG-todo print out other parameters

    const IInterface* parent = this->parent();
    const INamedInterface* pnamed = dynamic_cast<const INamedInterface*>(parent);
    std::string algo_name = pnamed->name();

    if ( m_doNtuple && algo_name=="NSWL1Simulation" ) {
        if(TTree *tree = get_tree_from_histsvc()){
            m_validation_tree.init_tree(tree);
        } else {
            ATH_MSG_ERROR("cannot book requested output tree");
            return StatusCode::FAILURE;;
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
        m_validation_tree.reset_ntuple_variables();
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
    // dummy function: just count four pads on 4 different layers with ieta, iphi == i or i+1
    std::vector<std::shared_ptr<PadData>> padsLayer0 = filterByGasGap(pads, 1);
    std::vector<std::shared_ptr<PadData>> padsLayer1 = filterByGasGap(pads, 2);
    std::vector<std::shared_ptr<PadData>> padsLayer2 = filterByGasGap(pads, 3);
    std::vector<std::shared_ptr<PadData>> padsLayer3 = filterByGasGap(pads, 4);
    std::vector<std::unique_ptr<PadTrigger>> triggers;
    for(auto p0 : padsLayer0){
        int p0ieta = p0->padEtaId();
        int p0iphi = p0->padPhiId();
        for(auto p1 : padsLayer1){
            int p1ieta = p1->padEtaId();
            int p1iphi = p1->padPhiId();
            bool p0_p1_match = ((p1ieta == p0ieta || p1ieta == p0ieta+1 ) &&
                                (p1iphi == p0iphi || p1iphi == p0iphi+1 ) );
            if(not p0_p1_match) continue;
            for(auto p2 : padsLayer2){
                int p2ieta = p2->padEtaId();
                int p2iphi = p2->padPhiId();
                bool p1_p2_match = ((p2ieta == p1ieta || p2ieta == p1ieta+1 ) &&
                                    (p2iphi == p1iphi || p2iphi == p1iphi+1 ) );
                if(not p1_p2_match) continue;
                for(auto p3 : padsLayer3){
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
    const std::vector<size_t> sides = {0, 1}; // see PadTdsOfflineTool::sideLabel()
    const std::vector<size_t> sectors = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    for(const size_t &side : sides){
        for(const size_t &sector : sectors){
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
                  std::vector<PadWithHits> pwhs;
                  for(const auto& p : sector_pads){
                     PadWithHits pwh(p);
                        fillGeometricInformation(*p,pwh);
                        pwhs.push_back(pwh);
                  }
                     m_tdrLogic.buildSectorTriggers(pwhs);
                     for( const SectorTriggerCandidate &st : m_tdrLogic.candidates()){
                        //S.I
                        auto p=std::make_unique<PadTrigger>(convert(st));
                        triggers.push_back(std::move(p));
                        //S.I
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
            //From MuonPadDesign... read pad local corners
            rdoEl->padCorners(Id,local_pad_corners);
            Amg::Vector3D pad_corner_global;
            float corner_positions[4][3] = {{0.}};
            for(unsigned int i=0; i<4; i++) {
              surface.localToGlobal(local_pad_corners.at(i), pad_corner_global, pad_corner_global);
              corner_positions[i][0] = pad_corner_global.x(); 
              corner_positions[i][1] = pad_corner_global.y(); 
              corner_positions[i][2] = pad_corner_global.z(); 
            }
            pwh.fillCornerCoords(corner_positions);
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
int PadTriggerLogicOfflineTool::Pad2BandId(const PadWithHits  &p, const float Yfrac){
	const static int offset[2][2][3][4] ={1,1,0,0, 35,35,36,36, 67,67,66,66, 0,0,-1,-1, 34,34,33,33, 64,64,63,63  ,  0,0,1,1, 33,33,33,33, 62,62,63,62, 0,0,-1,-1, 33,33,32,32, 61,61,60,60 }; //type,mult,module,layer
	int bandid = (Yfrac < 0.5) ? p.ieta*2-1 : p.ieta*2 ;
	bandid+=offset[p.sectortype][p.multiplet-1][p.module-1][p.layer-1];
	return bandid;
}


//------------------------------------------------------------------------------
NSWL1::PadTrigger PadTriggerLogicOfflineTool::convert(const SectorTriggerCandidate &t){
    //S.I 15.08.18 : This method probably needs to be refactored
    PadTrigger pt;
    
    const Polygon roi=t.triggerRegion3();    
    Vertex trgCntr=centroid(roi);
    SingleWedgePadTrigger swtrg = t.wedgeTrigs()[0];
    
    //*********** assign central etaphi cordinates of the triggering region *****************
    const float xcntr=coordinate<0>(trgCntr);
    const float ycntr=coordinate<1>(trgCntr);
    const float zcntr=swtrg.pads()[0].m_cornerXyz[1][2];
    TVector3 trigVector(xcntr,ycntr,zcntr);
    const float etaTrig=trigVector.Eta();
    const float phiTrig=trigVector.Phi();
    pt.m_eta    = etaTrig;
    pt.m_phi    = phiTrig;
    //**************************************************************************************
    
    //S.I 13.08.18 : Still BandID issues ?? Are we still to use halfPadCoordinates ???
    pt.m_bandid = swtrg.halfPadCoordinates().ieta; 
    pt.m_eta_id = swtrg.halfPadCoordinates().ieta;
    pt.m_phi_id = swtrg.halfPadCoordinates().iphi;
    pt.m_isSmall= int(swtrg.isSmallSector());
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
    //****************************************************************************************

     //S.I value of Z where trigger region is calculated.
     //from Z0 --> <Z of a pad> --->local coordinate
    for(const SingleWedgePadTrigger& swt : t.wedgeTrigs()){
    	for(const PadWithHits &p : swt.pads()){
    		if(p.m_padData) {//this is either overkill or ...
                //S.I 17-07-18
                const float padZ=p.m_cornerXyz[0][2];
    			const Polygon pol=Project(roi,zcntr,padZ);
    			Identifier Id( p.m_padData->id());
    			const Trk::PlaneSurface &padsurface = m_detManager->getsTgcReadoutElement(Id)->surface(Id);
    			std::vector<Amg::Vector2D> local_trgcorners;// corners of the triggering region as projected on the detector layer
                std::vector<Amg::Vector2D> local_padcorners;//pad's corners written in local coordinate system
                
                for(unsigned int i=0;i<4;i++){
                    const Amg::Vector3D global_padcorner(p.m_cornerXyz[i][0],p.m_cornerXyz[i][1],padZ);
                    Amg::Vector2D local_padcorner;
                    padsurface.globalToLocal(global_padcorner,Amg::Vector3D(),local_padcorner);
                    local_padcorners.push_back(local_padcorner);
                }

    			for(auto v : boost::geometry::exterior_ring(pol)){
    				float x=coordinate<0>(v);
    				float y=coordinate<1>(v);
    				const Amg::Vector3D globalcorner(x,y,padZ);
    				Amg::Vector2D localcorner;
    				padsurface.globalToLocal(globalcorner,globalcorner,localcorner);
    				local_trgcorners.push_back(localcorner);
    			}
                auto compare_y=[](const Amg::Vector2D &lhs, const Amg::Vector2D& rhs){return lhs.y()<rhs.y();};
    			auto local_padminmaxy=std::minmax_element(local_padcorners.begin(),local_padcorners.end(),compare_y);
    			float local_padminy=local_padminmaxy.first->y();
    			float local_padmaxy=local_padminmaxy.second->y();
                
                const auto local_trigminmaxy=std::minmax_element(local_trgcorners.begin(),local_trgcorners.end(),compare_y);
                float local_trigminy=local_trigminmaxy.first->y();
                float local_trigmaxy=local_trigminmaxy.second->y();
                pt.m_trglocalminY.push_back(local_trigminy);
    			pt.m_trglocalmaxY.push_back(local_trigmaxy);
    			pt.m_pads.push_back(p.m_padData);
                
                //***** Apply a small correction otherwise youll get incorrect results when making coordinate comparisons
                //In some cases trigger region falls slightly outside of the pad's boundaries
                //the difference is not more than a few microns and most of the time it is even less than a micron
                //But if the both edges of the triggering region are beyond the pad boundaries something must be seriously wrong
                //it is probably due to precision loss somewhere in the calculations
                float tol=0.005;//in milimeters
                float dymin=fabs(local_padminy-local_trigminy);
                float dymax=fabs(local_trigmaxy-local_padmaxy);
                
                if(local_trigminy<local_padminy && local_trigmaxy>local_padmaxy ){
                   ATH_MSG_FATAL("TRIGGER REGION FALLS OUTSIDE THE PAD!. SOMETHING IS WRONG.");
                }     
                
                if(dymin>0 && dymin<tol && local_trigminy<local_padminy){
                    ATH_MSG_DEBUG("Shifting trigger region minY[\u03BCmx10]: "<<int(local_trigminy*10000)<<"==>"<<int(local_padminy*10000));
                    local_trigminy=local_padminy;
                }
                if(dymax>0 && dymax<tol && local_trigmaxy>local_padmaxy){
                    ATH_MSG_DEBUG("Shifting trigger region maxY[\u03BCmx10]: "<<int(local_trigmaxy*10000)<<"==>"<<int(local_padmaxy*10000));
                    local_trigmaxy=local_padmaxy;
                }
                //******************************************************************************************************
                
                //#@!
                // Hack for pad<=>strip until we fix
    			//S.I Fix strip_pad_info hack 19.06.18       
                std::vector<float> pad_info;
    			pad_info.push_back(p.m_padData->multipletId());
    			pad_info.push_back(p.m_padData->gasGapId());
    			//S.I fix the hack (replace the below two lines that are commented out)
    			pad_info.push_back(local_trigminy);
    			pad_info.push_back(local_trigmaxy);
                //S.I before that was it below
    			//pad_info.push_back(p.m_loLocalY);
    			//pad_info.push_back(p.m_hiLocalY);
    			pt.m_pad_strip_info.push_back(pad_info);
                //S.I 17-07-18 
    		}
    		else
    			ATH_MSG_FATAL("PadTriggerLogicOfflineTool::convert: do not push back null pointers");
    		// so how do you handle it rather than printing something out ?
    	} // for(p)
    } // for (st)
    pt.m_multiplet_id = pt.m_pads.at(0)->multipletId();//S.I inner outer wedge or module 1 2 3 ??
    pt.m_gasGap_id    = pt.m_pads.at(0)->gasGapId();//S.I does assigning a gasgap id to a pad trigger ever make sense ?
    // Offset to BandId - get from the first pad in the trigger
    sTGCDetectorHelper sTGC_helper;
    sTGCDetectorDescription *sTGC=nullptr;
    char side      = pt.m_pads.at(0)->sideId()     == 0 ? 'C' : 'A';
    char type      = pt.m_pads.at(0)->sectorType() == 0 ? 'S' : 'L';
    int stationEta = pt.m_pads.at(0)->moduleId(); //if ( side == 'C' ) stationEta *= -1;
    int stationPhi = pt.m_pads.at(0)->sectorId();
    int layer      = pt.m_pads.at(0)->multipletId();

    int Offset[3] = {0,33,63};
    //S.I it seems we dont use the helper below. Let it stay there for now, we'll need it later
    sTGC = sTGC_helper.Get_sTGCDetector(type,stationEta,stationPhi,layer,side);
    if(sTGC == nullptr) {
        std::cerr<<"PadTriggerLogicOfflineTool::convert :: Could not find detector, no trigger bandid offset being applied"<<std::endl; 
    }
    else {
        sTGCReadoutParameters  roParams=sTGC->GetReadoutParameters();

        pt.m_bandid +=Offset[pt.m_pads.at(0)->moduleId()-1];
        
        //eof S.I
    }
    return pt;
}


//------------------------------------------------------------------------------


} // NSWL1

