/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloMonitoring/HLTCaloFEBTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigCaloRec/IAlgToolEFCalo.h"
#include "CaloEvent/CaloCellContainer.h"

#include "LArIdentifier/LArIdManager.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCabling/LArCablingLegacyService.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigT2CaloCommon/ITrigDataAccess.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "CaloGeoHelpers/CaloSampling.h"


#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include <string>

HLTCaloFEBTool::HLTCaloFEBTool(const std::string & type, const std::string & name, const IInterface* parent) : IHLTMonTool(type,name,parent),
 m_tcrAlgTools(this),
 m_onlineHelper(0),
 m_cablingSvc("LArCablingLegacyService"),
 m_data(NULL),
 m_noisetool("CaloNoiseTool/CaloNoiseToolDefault"),
 m_threshold(100.0),
 m_nsigma(5.0),
 m_useloadfullcoll(false),
 m_difflimit(0.){
	declareProperty ("TCRTools", m_tcrAlgTools);
	declareProperty ("DoNtuple", m_ntuple = false);
        declareProperty("NoiseTool",m_noisetool);
        declareProperty ("threshold", m_threshold);
        declareProperty ("sigma", m_nsigma);
        declareProperty ("useLoadFullCollection", m_useloadfullcoll);
        declareProperty ("DiffLimit", m_difflimit);
        declareProperty ("TCRTools", m_tcrAlgTools);
	// Nothing for the moment
	return;
}

HLTCaloFEBTool::~HLTCaloFEBTool() {
}

StatusCode HLTCaloFEBTool::init() {
       ATH_MSG_DEBUG( "Initializing"  );

	if ( m_tcrAlgTools.retrieve().isFailure() ) {
                ATH_MSG_ERROR( "Failed to retrieve helper tools: " << m_tcrAlgTools  );
		return StatusCode::FAILURE;
	} else {
                ATH_MSG_DEBUG( "Retrieved " << m_tcrAlgTools  );
	}
	if ( m_cablingSvc.retrieve().isFailure() ) {
                 ATH_MSG_ERROR( "Failed to retrieve helper tools: " << m_cablingSvc  );
                 return StatusCode::FAILURE;
        } else {
                 ATH_MSG_INFO( "Retrieved " << m_cablingSvc  );
        }

        const LArIdManager* larMgr;
        if ( (detStore()->retrieve(larMgr)).isFailure() ) {
                ATH_MSG_ERROR( "Unable to retrieve LArIdManager from DetectorStore"  );
                return StatusCode::FAILURE;
        } else {
                ATH_MSG_DEBUG( "Successfully retrieved LArIdManager from DetectorStore"  );
        }
        m_onlineHelper = larMgr->getOnlineID();
        if (!m_onlineHelper) {
                ATH_MSG_ERROR( "Could not access LArOnlineID helper"  );
                return StatusCode::FAILURE;
        } else {
                ATH_MSG_DEBUG( "Successfully accessed LArOnlineID helper"  );
        }

        if ( toolSvc()->retrieveTool("TrigDataAccess",m_data).isFailure() ){
                ATH_MSG_ERROR( "Failed to retrieve helper tools: " << m_data  );
                return StatusCode::FAILURE;
        } else {
                ATH_MSG_INFO( "Retrieved " << m_data  );
        }

	ATH_CHECK(m_noisetool.retrieve());

	return StatusCode::SUCCESS;

}

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTCaloFEBTool::book() {
#else
StatusCode HLTCaloFEBTool::book(bool newEventsBlock, bool newLumiBlock, bool newRun) {
#endif

  addMonGroup( new MonGroup(this,"HLT/CaloFEBMon",run) );

  if ( newRunFlag() ) {

  addHistogram(new TH1I("NEMLArFEBs","Number of HLT EM LAr FEBs; Number of HLT FEBs; Number of Events",100,0,1500));
  addHistogram(new TH1I("NHECLArFEBs","Number of HLT HEC LAr FEBs; Number of HLT FEBs; Number of Events",100,0,500));
  addHistogram(new TH1I("NFCALLArFEBs","Number of HLT FCAL LAr FEBs; Number of HLT FEBs; Number of Events",100,0,1500));
  addHistogram(new TH1I("NEMBadLArFEBs","Number of Bad EM LAr FEBs; Number of Bad FEBs; Number of Events",150,0,150));
  addHistogram(new TH1I("NHECBadLArFEBs","Number of Bad HEC LAr FEBs; Number of Bad FEBs; Number of Events",150,0,150));
  addHistogram(new TH1I("NFCALBadLArFEBs","Number of Bad FCAL LAr FEBs; Number of Bad FEBs; Number of Events",150,0,150));

  addHistogram(new TH1F("PercentDiffLAr","Percentual Difference in LAr; %; Number of Events",80,0,10.0) );
  addHistogram(new TH1F("conversionErrors","Number of conversionErrors per Event; conversionErrors; Number of Events",20,0,20.0) );
  addHistogram(new TH2F("etaphiEMBAFEBLAr","Position of EMB A-side LAr FEBs with some important difference; \\eta; \\phi",100,-5.,5.,64,-3.2,3.2) );
  addHistogram(new TH2F("etaphiHECAFEBLAr","Position of HEC A-side LAr FEBs with some important difference; \\eta; \\phi",100,-5.,5.,64,-3.2,3.2) );
  addHistogram(new TH2F("etaphiFCALAFEBLAr","Position of FCAL A-side LAr FEBs with some important difference; \\eta; \\phi",100,-5.,5.,64,-3.2,3.2) );
  addHistogram(new TH2F("etaphiEMBCFEBLAr","Position of EMB C-side LAr FEBs with some important difference; \\eta; \\phi",100,-5.,5.,64,-3.2,3.2) );
  addHistogram(new TH2F("etaphiHECCFEBLAr","Position of HEC C-side LAr FEBs with some important difference; \\eta; \\phi",100,-5.,5.,64,-3.2,3.2) );
  addHistogram(new TH2F("etaphiFCALCFEBLAr","Position of FCAL C-side LAr FEBs with some important difference; \\eta; \\phi",100,-5.,5.,64,-3.2,3.2) );
  addHistogram(new TH2F("etaphiEMCAFEBLAr","Position of EMC A-side LAr FEBs with some important difference; \\eta; \\phi",100,-5.,5.,64,-3.2,3.2) );
  addHistogram(new TH2F("etaphiEMCCFEBLAr","Position of EMC C-side LAr FEBs with some important difference; \\eta; \\phi",100,-5.,5.,64,-3.2,3.2) );
// EMB A historgrams
  //addHistogram(new TH1F("deltaetaEMBAFEBLAr","EMB A-side #Delta#eta(online,offline); #Delta#eta;",64,-3.2,3.2) );
  //addHistogram(new TH1F("deltaphiEMBAFEBLAr","EMB A-side #Delta#phi(online,offline);  #Delta#phi",100,-5.,5.) );
  addHistogram(new TH1F("deltaEnexEMBAFEBLAr","EMB A-side #Delta Ex(online,offline); #Delta E_{x} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEneyEMBAFEBLAr","EMB A-side #Delta Ey(online,offline); #Delta E_{y} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEnezEMBAFEBLAr","EMB A-side #Delta Ez(online,offline); #Delta E_{z} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaTotEneEMBAFEBLAr","EMB A-side #Delta TotE(online,offline); #Delta E (MeV)",100,-2000.,2000.) );
// EMC A historgrams
  //addHistogram(new TH1F("deltaetaEMCAFEBLAr","EMC A-side #Delta#eta(online,offline); #Delta#eta;",64,-3.2,3.2) );
  //addHistogram(new TH1F("deltaphiEMCAFEBLAr","EMC A-side #Delta#phi(online,offline);  #Delta#phi",100,-5.,5.) );
  addHistogram(new TH1F("deltaEnexEMCAFEBLAr","EMC A-side #Delta Ex(online,offline); #Delta E_{x} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEneyEMCAFEBLAr","EMC A-side #Delta Ey(online,offline); #Delta E_{y} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEnezEMCAFEBLAr","EMC A-side #Delta Ez(online,offline); #Delta E_{z} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaTotEneEMCAFEBLAr","EMC A-side #Delta TotE(online,offline); #Delta E (MeV)",100,-2000.,2000.) );
// HEC A historgrams
  //addHistogram(new TH1F("deltaetaHECAFEBLAr","HEC A-side #Delta#eta(online,offline); #Delta#eta;",64,-3.2,3.2) );
  //addHistogram(new TH1F("deltaphiHECAFEBLAr","HEC A-side #Delta#phi(online,offline); #Delta#phi",100,-5.,5.) );
  addHistogram(new TH1F("deltaEnexHECAFEBLAr","HEC A-side #Delta Ex(online,offline); #Delta E_{x} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEneyHECAFEBLAr","HEC A-side #Delta Ey(online,offline); #Delta E_{y} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEnezHECAFEBLAr","HEC A-side #Delta Ez(online,offline); #Delta E_{z} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaTotEneHECAFEBLAr","HEC A-side #Delta TotE(online,offline); #Delta E (MeV)",100,-2000.,2000.) );
// FCAL A historgrams
  //addHistogram(new TH1F("deltaetaFCALAFEBLAr","FCAL A-side #Delta#eta(online,offline); #Delta #eta;",64,-3.2,3.2) );
  //addHistogram(new TH1F("deltaphiFCALAFEBLAr","FCAL A-side #Delta#phi(online,offline); #Delta#phi",100,-5.,5.) );
  addHistogram(new TH1F("deltaEnexFCALAFEBLAr","FCAL A-side #Delta Ex(online,offline); #Delta E_{x} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEneyFCALAFEBLAr","FCAL A-side #Delta Ey(online,offline); #Delta E_{y} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEnezFCALAFEBLAr","FCAL A-side #Delta Ez(online,offline); #Delta E_{z} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaTotEneFCALAFEBLAr","FCAL A-side #Delta TotE(online,offline); #Delta E (MeV)",100,-2000.,2000.) );
// EMB C historgrams
  //addHistogram(new TH1F("deltaetaEMBCFEBLAr","EMB C-side #Delta#eta(online,offline); #Delta#eta;",64,-3.2,3.2) );
  //addHistogram(new TH1F("deltaphiEMBCFEBLAr","EMB C-side #Delta#phi(online,offline);  #Delta#phi",100,-5.,5.) );
  addHistogram(new TH1F("deltaEnexEMBCFEBLAr","EMB C-side #Delta Ex(online,offline); #Delta E_{x} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEneyEMBCFEBLAr","EMB C-side #Delta Ey(online,offline); #Delta E_{y} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEnezEMBCFEBLAr","EMB C-side #Delta Ez(online,offline); #Delta E_{z} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaTotEneEMBCFEBLAr","EMB C-side #Delta TotE(online,offline); #Delta E (MeV)",100,-2000.,2000.) );
// EMC C historgrams
  //addHistogram(new TH1F("deltaetaEMCCFEBLAr","EMC C-side #Delta#eta(online,offline); #Delta#eta;",64,-3.2,3.2) );
  //addHistogram(new TH1F("deltaphiEMCCFEBLAr","EMC C-side #Delta#phi(online,offline);  #Delta#phi",100,-5.,5.) );
  addHistogram(new TH1F("deltaEnexEMCCFEBLAr","EMC C-side #Delta Ex(online,offline); #Delta E_{x} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEneyEMCCFEBLAr","EMC C-side #Delta Ey(online,offline); #Delta E_{y} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEnezEMCCFEBLAr","EMC C-side #Delta Ez(online,offline); #Delta E_{z} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaTotEneEMCCFEBLAr","EMC C-side #Delta TotE(online,offline); #Delta E (MeV)",100,-2000.,2000.) );
// HEC C historgrams
  //addHistogram(new TH1F("deltaetaHECCFEBLAr","HEC C-side #Delta#eta(online,offline); #Delta#eta;",64,-3.2,3.2) );
  //addHistogram(new TH1F("deltaphiHECCFEBLAr","HEC C-side #Delta#phi(online,offline); #Delta#phi",100,-5.,5.) );
  addHistogram(new TH1F("deltaEnexHECCFEBLAr","HEC C-side #Delta Ex(online,offline); #Delta E_{x} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEneyHECCFEBLAr","HEC C-side #Delta Ey(online,offline); #Delta E_{y} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEnezHECCFEBLAr","HEC C-side #Delta Ez(online,offline); #Delta E_{z} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaTotEneHECCFEBLAr","HEC C-side #Delta TotE(online,offline); #Delta E (MeV)",100,-2000.,2000.) );
// FCAL C historgrams
  //addHistogram(new TH1F("deltaetaFCALCFEBLAr","FCAL C-side #Delta#eta(online,offline); #Delta #eta;",64,-3.2,3.2) );
  //addHistogram(new TH1F("deltaphiFCALCFEBLAr","FCAL C-side #Delta#phi(online,offline); #Delta#phi",100,-5.,5.) );
  addHistogram(new TH1F("deltaEnexFCALCFEBLAr","FCAL C-side #Delta Ex(online,offline); #Delta E_{x} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEneyFCALCFEBLAr","FCAL C-side #Delta Ey(online,offline); #Delta E_{y} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaEnezFCALCFEBLAr","FCAL C-side #Delta Ez(online,offline); #Delta E_{z} (MeV)",100,-200.,200.) );
  addHistogram(new TH1F("deltaTotEneFCALCFEBLAr","FCAL C-side #Delta TotE(online,offline); #Delta E (MeV)",100,-2000.,2000.) );
  // lartile=0?lar:tile
  if ( m_ntuple ) 
    addTree( new TNtuple("Details","Details","et:eta:phi:gain:tet:teta:tphi:tgain:lartile") );
  
  }else if ( newEventsBlockFlag() || newLumiBlockFlag() ){
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG( "End of book"  );
  return StatusCode::SUCCESS;
}

StatusCode HLTCaloFEBTool::fill() {

	const DataHandle<CaloCellContainer> AllCalo;
	if ( evtStore()->retrieve(AllCalo,"AllCalo").isFailure() ){
             ATH_MSG_DEBUG( "No Calo Cell Container found" );
             return StatusCode::SUCCESS;
	}
#ifndef NDEBUG
        ATH_MSG_DEBUG( "Got container " << "Size : " << AllCalo->size()  );
#endif

	CaloCellContainer* pCaloCellContainer
                = new CaloCellContainer(SG::VIEW_ELEMENTS);
        ToolHandleArray<IAlgToolEFCalo>::iterator itrtcr = m_tcrAlgTools.begin();
        ToolHandleArray<IAlgToolEFCalo>::iterator endtcr = m_tcrAlgTools.end();
        double etamin, etamax,phimin, phimax;
        etamin=-4.8;
        etamax=4.8;
        for (; itrtcr!=endtcr; ++itrtcr) {

#ifndef NDEBUG
          ATH_MSG_VERBOSE( "Tool name : " << (*itrtcr).name()  );
#endif
        phimin=-M_PI;
        phimax=M_PI;
        // Fix for stupid RS problem
        if ( (*itrtcr).name().find("Tile")!=std::string::npos ){
                phimin=0;
                phimax=2*M_PI;
        }
        StatusCode sc;
	float eta0=(etamax+etamin)/2;
	float phi0=(phimax+phimin)/2;
        if((*itrtcr).name() == "FullCaloCellContMaker") {
                sc= (*itrtcr)->execute(*pCaloCellContainer);
        } else {
    	        sc = (*itrtcr)->execute(*pCaloCellContainer, TrigRoiDescriptor( eta0, etamin, etamax, phi0, phimin, phimax ) );
        }
        if ( sc.isFailure() ) {
          ATH_MSG_ERROR( "Problem with filling the cont" );
        }
        } // End of loop over tools

	CaloCellContainer::const_iterator beg = pCaloCellContainer->begin();
	CaloCellContainer::const_iterator end = pCaloCellContainer->end();

	std::map<HWIdentifier,LArFebEnergy> larfebmap;
	/*std::map<HWIdentifier,LArFebEnergy> larfebmap_fcal, larfebmap_fcalnoise;
	std::map<HWIdentifier,LArFebEnergy> larfebmap_em,larfebmap_emnoise;
	std::map<HWIdentifier,LArFebEnergy> larfebmap_hec, larfebmap_hecnoise, larfebmap_em,larfebmap_emnoise;*/
	std::map<HWIdentifier,std::pair<LArFebEnergy,LArFebEnergy> > larfebmap_fcal; 
	std::map<HWIdentifier,std::pair<LArFebEnergy,LArFebEnergy> > larfebmap_em;
	std::map<HWIdentifier,std::pair<LArFebEnergy,LArFebEnergy> > larfebmap_hec; 
	std::map<HWIdentifier,LArFebEnergy>::iterator it;

	for(; beg!=end ; ++beg){
		const CaloCell* cell = (*beg);
		if ( cell && cell->caloDDE()
			&& cell->caloDDE()->getSubCalo() < CaloCell_ID::TILE ){
			Identifier idh = cell->ID();
                        const CaloDetDescrElement *detdescr = cell->caloDDE();
			//IdentifierHash idh = cell->caloDDE()->calo_hash();
			HWIdentifier channel_hw = m_cablingSvc->createSignalChannelID(idh);
			HWIdentifier feb_hw = m_onlineHelper->feb_Id(channel_hw);
                        Identifier32::value_type feb_hw_n = feb_hw.get_identifier32().get_compact();
                        float cellnoise=0;
			if ( m_nsigma != 0 ){
			  cellnoise = m_noisetool->totalNoiseRMS(cell->caloDDE(),cell->gain());
			  m_threshold = m_nsigma * cellnoise;
			  //m_threshold = m_nsigma * m_noisetool->totalNoiseRMS(cell->caloDDE(),cell->gain());
                          //std::cout << "cell energy = " << cell->energy() << std::endl;
                        }
			if ( cell->energy() > m_threshold ) {
				int ienergy = (int) floorf(cell->energy()/64.0);
				float energy = (float)(64.0*ienergy);
				float et, ex, ey, ez;
				et = energy * cell->caloDDE()->sinTh();
				ex= et * cell->caloDDE()->cosPhi();
				ey= et * cell->caloDDE()->sinPhi();
				ez= et * cell->caloDDE()->cotTh();
                                double tmpx = cellnoise*cell->caloDDE()->sinTh()*cell->caloDDE()->cosPhi();
                                double tmpy = cellnoise*cell->caloDDE()->sinTh()*cell->caloDDE()->sinPhi();
                                double tmpz = cellnoise*cell->caloDDE()->sinTh()*cell->caloDDE()->cotTh();
                                double sigmax = (tmpx*tmpx);
                                double sigmay = (tmpy*tmpy);
                                double sigmaz = (tmpz*tmpz);
				//std::cout << "just test " << cell->caloDDE()->sinTh() << " " << 1./cosh ( cell->eta() ) << " " << cell->caloDDE()->cosPhi() << " " << cos ( cell->phi() ) << " " << cell->caloDDE()->sinTh()* cell->caloDDE()->cotTh() << " " << tanh(cell->eta() ) << " " << m_threshold << std::endl;
#ifndef NDEBUG
				if ( energy > 0 ) {
                                  ATH_MSG_VERBOSE( " All cells that compose feb : "
                                                   << std::hex
                                                   << feb_hw << " "
                                                   << idh << " "
                                                   << channel_hw << " "
                                                   << std::dec
                                                   << energy << " "
                                                   << cell->energy() << " "
                                                   << et << " "
                                                   << ex << " "
                                                   << ey << " "
                                                   << ez  );
				}
#endif
				if ( (it = larfebmap.find(feb_hw)) != larfebmap.end() ){
					// fill
#ifndef NDEBUG
                                        ATH_MSG_VERBOSE( "filling created FEB " 
                                                         << "with size = " << larfebmap.size()  );
#endif
					LArFebEnergy* larfebenergy =
						&((*it).second);
					double tmpex = larfebenergy->getFebEx();
					double tmpey = larfebenergy->getFebEy();
					double tmpez = larfebenergy->getFebEz();
#ifndef NDEBUG
                                        ATH_MSG_VERBOSE( "current ex = " << tmpex  );
                                        ATH_MSG_VERBOSE( "current ey = " << tmpey  );
                                        ATH_MSG_VERBOSE( "current ez = " << tmpez  );
#endif
					larfebenergy->setFebEx(
					  ex + tmpex);
					larfebenergy->setFebEy(
					  ey + tmpey);
					larfebenergy->setFebEz(
					  ez + tmpez);
				} else {
					LArFebEnergy larfebenergy =
					  LArFebEnergy(feb_hw_n,ex,ey,ez,0.);
					larfebmap.insert(std::pair<HWIdentifier,LArFebEnergy>(feb_hw,larfebenergy));
				}
                                CaloCell_ID::CaloSample s = detdescr->getSampling();
	                        //std::map<HWIdentifier,LArFebEnergy>::iterator myit;
	                        std::map<HWIdentifier,std::pair<LArFebEnergy,LArFebEnergy> >::iterator myit;
                                switch(s){
                                  case CaloCell_ID::PreSamplerB:
                                  case CaloCell_ID::PreSamplerE:
                                  case CaloCell_ID::EMB1:
                                  case CaloCell_ID::EMB2:
                                  case CaloCell_ID::EMB3:
                                  case CaloCell_ID::EME1:
                                  case CaloCell_ID::EME2:
                                  case CaloCell_ID::EME3:
	                            //std::map<HWIdentifier,LArFebEnergy>::iterator myit;
                                    if( (myit = larfebmap_em.find(feb_hw)) != larfebmap_em.end() ){
                                        LArFebEnergy* larfebenergy =
                                                &((*myit).second.first);
                                        double tmpex = larfebenergy->getFebEx();
                                        double tmpey = larfebenergy->getFebEy();
                                        double tmpez = larfebenergy->getFebEz();
                                        larfebenergy->setFebEx(
                                          ex + tmpex);
                                        larfebenergy->setFebEy(
                                          ey + tmpey);
                                        larfebenergy->setFebEz(
                                          ez + tmpez);
                                        LArFebEnergy* larfebnoise =
                                                &((*myit).second.second);
                                        double tmpsx = larfebnoise->getFebEx();
                                        double tmpsy = larfebnoise->getFebEy();
                                        double tmpsz = larfebnoise->getFebEz();
                                        larfebnoise->setFebEx(
                                          sigmax + tmpsx);
                                        larfebnoise->setFebEy(
                                          sigmay + tmpsy);
                                        larfebnoise->setFebEz(
                                          sigmaz + tmpsz);
                                    } else {
                                        LArFebEnergy larfebenergy =
                                          LArFebEnergy(feb_hw_n,ex,ey,ez,0.);
                                        LArFebEnergy larfebnoise =
                                          LArFebEnergy(feb_hw_n,sigmax,sigmay,sigmaz,0.);
                                        //larfebmap_em.insert(std::pair<HWIdentifier,LArFebEnergy>(feb_hw,larfebenergy));
                                        larfebmap_em.insert(std::pair<HWIdentifier,std::pair<LArFebEnergy,LArFebEnergy> >(feb_hw,std::make_pair(larfebenergy,larfebnoise)));
                                    }
                                    break;
                                  case CaloCell_ID::HEC0:
                                  case CaloCell_ID::HEC1:
                                  case CaloCell_ID::HEC2:
                                  case CaloCell_ID::HEC3:
	                            //std::map<HWIdentifier,LArFebEnergy>::iterator myit;
                                    if( (myit = larfebmap_hec.find(feb_hw)) != larfebmap_hec.end() ){
                                        LArFebEnergy* larfebenergy =
                                                &((*myit).second.first);
                                        double tmpex = larfebenergy->getFebEx();
                                        double tmpey = larfebenergy->getFebEy();
                                        double tmpez = larfebenergy->getFebEz();
                                        larfebenergy->setFebEx(
                                          ex + tmpex);
                                        larfebenergy->setFebEy(
                                          ey + tmpey);
                                        larfebenergy->setFebEz(
                                          ez + tmpez);
                                        LArFebEnergy* larfebnoise =
                                                &((*myit).second.second);
                                        double tmpsx = larfebnoise->getFebEx();
                                        double tmpsy = larfebnoise->getFebEy();
                                        double tmpsz = larfebnoise->getFebEz();
                                        larfebnoise->setFebEx(
                                          sigmax + tmpsx);
                                        larfebnoise->setFebEy(
                                          sigmay + tmpsy);
                                        larfebnoise->setFebEz(
                                          sigmaz + tmpsz);
                                    } else {
                                        LArFebEnergy larfebenergy =
                                          LArFebEnergy(feb_hw_n,ex,ey,ez,0.);
                                        LArFebEnergy larfebnoise =
                                          LArFebEnergy(feb_hw_n,sigmax,sigmay,sigmaz,0.);
                                        //larfebmap_hec.insert(std::pair<HWIdentifier,LArFebEnergy>(feb_hw,larfebenergy));
                                        larfebmap_hec.insert(std::pair<HWIdentifier,std::pair<LArFebEnergy,LArFebEnergy> >(feb_hw,std::make_pair(larfebenergy,larfebnoise)));
                                    }
                                    break;
                                  case CaloCell_ID::FCAL0:
                                  case CaloCell_ID::FCAL1:
                                  case CaloCell_ID::FCAL2:
	                            //std::map<HWIdentifier,LArFebEnergy>::iterator myit;
                                    if( (myit = larfebmap_fcal.find(feb_hw)) != larfebmap_fcal.end() ){
                                        LArFebEnergy* larfebenergy =
                                                &((*myit).second.first);
                                        double tmpex = larfebenergy->getFebEx();
                                        double tmpey = larfebenergy->getFebEy();
                                        double tmpez = larfebenergy->getFebEz();
                                        larfebenergy->setFebEx(
                                          ex + tmpex);
                                        larfebenergy->setFebEy(
                                          ey + tmpey);
                                        larfebenergy->setFebEz(
                                          ez + tmpez);
                                        LArFebEnergy* larfebnoise =
                                                &((*myit).second.second);
                                        double tmpsx = larfebnoise->getFebEx();
                                        double tmpsy = larfebnoise->getFebEy();
                                        double tmpsz = larfebnoise->getFebEz();
                                        larfebnoise->setFebEx(
                                          sigmax + tmpsx);
                                        larfebnoise->setFebEy(
                                          sigmay + tmpsy);
                                        larfebnoise->setFebEz(
                                          sigmaz + tmpsz);
                                    } else {
                                        LArFebEnergy larfebenergy =
                                          LArFebEnergy(feb_hw_n,ex,ey,ez,0.);
                                        LArFebEnergy larfebnoise =
                                          LArFebEnergy(feb_hw_n,sigmax,sigmay,sigmaz,0.);
                                        larfebmap_fcal.insert(std::pair<HWIdentifier,std::pair<LArFebEnergy,LArFebEnergy> >(feb_hw,std::make_pair(larfebenergy,larfebnoise)));
                                    }
                                    break;
                                  case CaloCell_ID::TileBar0:
                                  case CaloCell_ID::TileBar1:
                                  case CaloCell_ID::TileBar2:
                                  case CaloCell_ID::TileExt0:
                                  case CaloCell_ID::TileExt1:
                                  case CaloCell_ID::TileExt2:
                                  case CaloCell_ID::TileGap1:
                                  case CaloCell_ID::TileGap2:
                                  case CaloCell_ID::TileGap3:
                                  case CaloCell_ID::MINIFCAL0:
                                  case CaloCell_ID::MINIFCAL1:
                                  case CaloCell_ID::MINIFCAL2:
                                  case CaloCell_ID::MINIFCAL3:
                                  case CaloCell_ID::Unknown:
                                     break;
                                }
			}
		} // End of basic cell checks
		
	}
#ifndef NDEBUG
        ATH_MSG_DEBUG( " Number of Offline FEBs : "
                       << larfebmap.size()  );
        ATH_MSG_DEBUG( "COMPARISON "  );
#endif
	//LArFebEnergyCollection m_larfebcol;
        m_larfebcol.clear();
        m_larfebcol_hec.clear();
        m_larfebcol_em.clear();
        m_larfebcol_fcal.clear();
	//m_larfebcol.clear(SG::VIEW_ELEMENTS);
	//LArFebEnergyCollection::const_iterator tit,tbegin,tend;
	//LArFebEnergyCollection::const_iterator tit;
        bool prepare=true;
        if(m_useloadfullcoll){
          if(m_data->LoadFullCollections(m_tbegin,m_tend,TTEM,prepare).isFailure()){
                ATH_MSG_ERROR( "Problems reading LoadFullCollection" );
		return StatusCode::FAILURE;
          }
        } else {
        double etamin = -4.8;
        double etamax = 4.8;
        double phimin = -M_PI;
        double phimax = M_PI;
	//if ( m_data->LoadFullCollections(tbegin,tend,TTEM).isFailure() ) {
        m_data->RegionSelector(0,etamin,etamax,phimin,phimax, TTEM);
	//LArFebEnergyCollection::const_iterator titem0,tbeginem0,tendem0;
	if ( m_data->LoadCollections(m_tbegin,m_tend,0).isFailure() ) {
                ATH_MSG_ERROR( "Problems to read FEB info" );
		return StatusCode::FAILURE;
	}
        //LArFebEnergy *feb = new LArFebEnergy();
	for(LArFebEnergyCollection::const_iterator tit= m_tbegin; tit!=m_tend; ++tit) {
                LArFebEnergy feb = LArFebEnergy((*tit)->getFebId(),(*tit)->getFebEx(),(*tit)->getFebEy(),(*tit)->getFebEz(),0.);
                /*feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());*/
#ifndef NDEBUG
                ATH_MSG_VERBOSE( "TTEM 0 FEB ID = " << feb.getFebId()  );
                ATH_MSG_VERBOSE( "TTEM 0 FEB Ex = " << feb.getFebEx()  );
                ATH_MSG_VERBOSE( "TTEM 0 FEB Ey = " << feb.getFebEy()  );
                ATH_MSG_VERBOSE( "TTEM 0 FEB Ez = " << feb.getFebEz()  );
#endif
		m_larfebcol_em.push_back(feb);
	}
        m_data->RegionSelector(1,etamin,etamax,phimin,phimax,TTEM);
	//LArFebEnergyCollection::const_iterator tbeginem1;
	//LArFebEnergyCollection::const_iterator tendem1;
	if ( m_data->LoadCollections(m_tbegin,m_tend,1).isFailure() ) {
                ATH_MSG_ERROR( "Problems to read FEB info" );
		return StatusCode::FAILURE;
	}
	for(LArFebEnergyCollection::const_iterator tit = m_tbegin; tit!=m_tend; ++tit) {
                //LArFebEnergy *feb = new LArFebEnergy((*tit)->getFebId());
                LArFebEnergy feb = LArFebEnergy((*tit)->getFebId(),(*tit)->getFebEx(),(*tit)->getFebEy(),(*tit)->getFebEz(),0.);
                /*feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());*/
#ifndef NDEBUG
                ATH_MSG_VERBOSE( "TTEM 1 FEB ID = " << (*tit)->getFebId()  );
                ATH_MSG_VERBOSE( "TTEM 1 FEB Ex = " << (*tit)->getFebEx()  );
                ATH_MSG_VERBOSE( "TTEM 1 FEB Ey = " << (*tit)->getFebEy()  );
                ATH_MSG_VERBOSE( "TTEM 1 FEB Ez = " << (*tit)->getFebEz()  );
#endif
		m_larfebcol_em.push_back(feb);
	}
        m_data->RegionSelector(2,etamin,etamax,phimin,phimax, TTEM);
	//LArFebEnergyCollection::const_iterator titem2,tbeginem2,tendem2;
	if ( m_data->LoadCollections(m_tbegin,m_tend,2).isFailure() ) {
                ATH_MSG_ERROR( "Problems to read FEB info" );
		return StatusCode::FAILURE;
	}
	for( LArFebEnergyCollection::const_iterator tit= m_tbegin; tit!=m_tend; ++tit) {
                //LArFebEnergy *feb = new LArFebEnergy((*tit)->getFebId());
                LArFebEnergy feb = LArFebEnergy((*tit)->getFebId(),(*tit)->getFebEx(),(*tit)->getFebEy(),(*tit)->getFebEz(),0.);
                /*feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());*/
#ifndef NDEBUG
                ATH_MSG_VERBOSE( "TTEM 2 FEB ID = " << (*tit)->getFebId()  );
                ATH_MSG_VERBOSE( "TTEM 2 FEB Ex = " << (*tit)->getFebEx()  );
                ATH_MSG_VERBOSE( "TTEM 2 FEB Ey = " << (*tit)->getFebEy()  );
                ATH_MSG_VERBOSE( "TTEM 2 FEB Ez = " << (*tit)->getFebEz()  );
#endif
		m_larfebcol_em.push_back(feb);
	}
        m_data->RegionSelector(3,etamin,etamax,phimin,phimax, TTEM);
	//LArFebEnergyCollection::const_iterator titem3,tbeginem3,tendem3;
	if ( m_data->LoadCollections(m_tbegin,m_tend,3).isFailure() ) {
                ATH_MSG_ERROR( "Problems to read FEB info" );
		return StatusCode::FAILURE;
	}
	for(LArFebEnergyCollection::const_iterator  tit= m_tbegin; tit!=m_tend; ++tit) {
                LArFebEnergy feb = LArFebEnergy((*tit)->getFebId(),(*tit)->getFebEx(),(*tit)->getFebEy(),(*tit)->getFebEz(),0.);
                /*feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());*/
#ifndef NDEBUG
                ATH_MSG_VERBOSE( "TTEM 3 FEB ID = " << (*tit)->getFebId()  );
                ATH_MSG_VERBOSE( "TTEM 3 FEB Ex = " << (*tit)->getFebEx()  );
                ATH_MSG_VERBOSE( "TTEM 3 FEB Ey = " << (*tit)->getFebEy()  );
                ATH_MSG_VERBOSE( "TTEM 3 FEB Ez = " << (*tit)->getFebEz()  );
#endif
		m_larfebcol_em.push_back(feb);
	}
        m_data->RegionSelector(0,etamin,etamax,phimin,phimax, TTHEC);
	//LArFebEnergyCollection::const_iterator tithec0,tbeginhec0,tendhec0;
	if ( m_data->LoadCollections(m_tbegin,m_tend).isFailure() ) {
                ATH_MSG_ERROR( "Problems to read FEB info" );
		return StatusCode::FAILURE;
	}
	for(LArFebEnergyCollection::const_iterator  tit = m_tbegin; tit!=m_tend; ++tit) {
                LArFebEnergy feb = LArFebEnergy((*tit)->getFebId(),(*tit)->getFebEx(),(*tit)->getFebEy(),(*tit)->getFebEz(),0.);
                /*feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());*/
#ifndef NDEBUG
                ATH_MSG_VERBOSE( "TTHEC 0 FEB ID = " << (*tit)->getFebId()  );
                ATH_MSG_VERBOSE( "TTHEC 0 FEB Ex = " << (*tit)->getFebEx()  );
                ATH_MSG_VERBOSE( "TTHEC 0 FEB Ey = " << (*tit)->getFebEy()  );
                ATH_MSG_VERBOSE( "TTHEC 0 FEB Ez = " << (*tit)->getFebEz()  );
#endif
		//m_larfebcol.push_back(feb);
		m_larfebcol_hec.push_back(feb);
	}
        m_data->RegionSelector(0,etamin,etamax,phimin,phimax, FCALHAD);
	if ( m_data->LoadCollections(m_tbegin,m_tend).isFailure() ) {
                ATH_MSG_ERROR( "Problems to read FEB info" );
		return StatusCode::FAILURE;
	}
	for(LArFebEnergyCollection::const_iterator tit = m_tbegin; tit!=m_tend; ++tit) {
                LArFebEnergy feb = LArFebEnergy((*tit)->getFebId(),(*tit)->getFebEx(),(*tit)->getFebEy(),(*tit)->getFebEz(),0.);
                /*feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());*/
#ifndef NDEBUG
                ATH_MSG_VERBOSE( "FCALHAD 0 FEB ID = " << (*tit)->getFebId()  );
                ATH_MSG_VERBOSE( "FCALHAD 0 FEB Ex = " << (*tit)->getFebEx()  );
                ATH_MSG_VERBOSE( "FCALHAD 0 FEB Ey = " << (*tit)->getFebEy()  );
                ATH_MSG_VERBOSE( "FCALHAD 0 FEB Ez = " << (*tit)->getFebEz()  );
#endif
		m_larfebcol_fcal.push_back(feb);
	}
        m_data->RegionSelector(1,etamin,etamax,phimin,phimax, FCALHAD);
	if ( m_data->LoadCollections(m_tbegin,m_tend).isFailure() ) {
                ATH_MSG_ERROR( "Problems to read FEB info" );
		return StatusCode::FAILURE;
	}
	for(LArFebEnergyCollection::const_iterator tit = m_tbegin; tit!=m_tend; ++tit) {
                LArFebEnergy feb = LArFebEnergy((*tit)->getFebId(),(*tit)->getFebEx(),(*tit)->getFebEy(),(*tit)->getFebEz(),0.);
                /*feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());*/
#ifndef NDEBUG
                ATH_MSG_VERBOSE( "FCALHAD 1 FEB ID = " << (*tit)->getFebId()  );
                ATH_MSG_VERBOSE( "FCALHAD 1 FEB Ex = " << (*tit)->getFebEx()  );
                ATH_MSG_VERBOSE( "FCALHAD 1 FEB Ey = " << (*tit)->getFebEy()  );
                ATH_MSG_VERBOSE( "FCALHAD 1 FEB Ez = " << (*tit)->getFebEz()  );
#endif
		m_larfebcol_fcal.push_back(feb);
	}
        m_data->RegionSelector(0,etamin,etamax,phimin,phimax, FCALEM);
	if ( m_data->LoadCollections(m_tbegin,m_tend).isFailure() ) {
                ATH_MSG_ERROR( "Problems to read FEB info" );
		return StatusCode::FAILURE;
	}
	for(LArFebEnergyCollection::const_iterator tit = m_tbegin; tit!=m_tend; ++tit) {
                LArFebEnergy feb = LArFebEnergy((*tit)->getFebId(),(*tit)->getFebEx(),(*tit)->getFebEy(),(*tit)->getFebEz(),0.);
                /*feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());*/
#ifndef NDEBUG
                ATH_MSG_VERBOSE( "FCALEM 0 FEB ID = " << (*tit)->getFebId()  );
                ATH_MSG_VERBOSE( "FCALEM 0 FEB Ex = " << (*tit)->getFebEx()  );
                ATH_MSG_VERBOSE( "FCALEM 0 FEB Ey = " << (*tit)->getFebEy()  );
                ATH_MSG_VERBOSE( "FCALEM 0 FEB Ez = " << (*tit)->getFebEz()  );
#endif
		m_larfebcol_fcal.push_back(feb);
	}
	}
	int counthec = 0;
	int counthec_not_found = 0;
	int countphec = 0;
        double totalhec = 0;
        double totalhecx = 0;
        double totalhecy = 0;
        double totalhecz = 0;
        double offlinehecx = 0;
        double offlinehecy = 0;
        double offlinehecz = 0;
	ATH_MSG_DEBUG( "Compare FEBs in HEC"  );
        doComparison(m_larfebcol_hec,&larfebmap_hec,counthec,counthec_not_found,countphec,totalhec,totalhecx,totalhecy,totalhecz, offlinehecx,offlinehecy,offlinehecz);
        hist("NHECLArFEBs")->Fill(counthec);
        hist("NHECBadLArFEBs")->Fill(countphec);
#ifndef NDEBUG
       	if ( msgLvl(MSG::DEBUG) ){
            float tothecene = sqrt(pow(totalhecx,2) + pow(totalhecy,2) + pow(totalhecz,2));
	    msg() << MSG::DEBUG << "HLT HEC FEBs found : " << counthec << endmsg;
	    msg() << "HEC FEBs not found : " << counthec_not_found << endmsg;
	    msg() << "HEC FEBs with problems : " << countphec << endmsg;
	    msg() << "HEC FEBs using difflimit  : " << m_difflimit << endmsg;

	    msg() << MSG::DEBUG << "HEC HLT FEB total energy = " << tothecene;
	    msg() << MSG::DEBUG << "; HEC HLT FEB total energy x = " << totalhecx;
	    msg() << MSG::DEBUG << "; HEC HLT FEB total energy y = " << totalhecy;
	    msg() << MSG::DEBUG << "; HEC HLT FEB total energy z = " << totalhecz << endmsg;
            float offlinehecene = sqrt(offlinehecx*offlinehecx + offlinehecy*offlinehecy + offlinehecz*offlinehecz);
	    msg() << MSG::DEBUG << "HEC offline FEB total energy = " << offlinehecene;
	    msg() << MSG::DEBUG << "; HEC offline FEB total energy x = " << offlinehecx;
	    msg() << MSG::DEBUG << "; HEC offline FEB total energy y = " << offlinehecy;
	    msg() << MSG::DEBUG << "; HEC offline FEB total energy z = " << offlinehecz << endmsg;
	}
#endif
// now do EM 
	int countem = 0;
	int countem_not_found = 0;
	int countpem = 0;
        double totalem = 0;
        double totalemx = 0;
        double totalemy = 0;
        double totalemz = 0;
        double offlineemx = 0;
        double offlineemy = 0;
        double offlineemz = 0;
	ATH_MSG_DEBUG( "Compare FEBs in EM "  );
        doComparison(m_larfebcol_em,&larfebmap_em,countem,countem_not_found,countpem,totalem,totalemx,totalemy,totalemz,offlineemx,offlineemy,offlineemz);
        hist("NEMLArFEBs")->Fill(countem);
        hist("NEMBadLArFEBs")->Fill(countpem);
#ifndef NDEBUG
       	if ( msgLvl(MSG::DEBUG) ){
           float totemene = sqrt(pow(totalemx,2) + pow(totalemy,2) + pow(totalemz,2));
	   msg() << MSG::DEBUG << "HLT EM FEBs found : " << countem << endmsg;
	   msg() << "EM FEBs not found : " << countem_not_found << endmsg;
	   msg() << "EM FEBs with problems : " << countpem << endmsg;
	   msg() << "EM FEBs using difflimit  : " << m_difflimit << endmsg;
	   msg() << MSG::DEBUG << "EM HLT FEB total energy = " << totemene;
	   msg() << MSG::DEBUG << "; EM HLT FEB total energy x = " << totalemx;
	   msg() << MSG::DEBUG << "; EM HLT FEB total energy y = " << totalemy;
	   msg() << MSG::DEBUG << "; EM HLT FEB total energy z = " << totalemz << endmsg;
           float offlineemene = sqrt(offlineemx*offlineemx + offlineemy*offlineemy + offlineemz*offlineemz);
	   msg() << MSG::DEBUG << "EM offline FEB total energy = " << offlineemene;
	   msg() << MSG::DEBUG << "; EM offline FEB total energy x = " << offlineemx;
	   msg() << MSG::DEBUG << "; EM offline FEB total energy y = " << offlineemy;
	   msg() << MSG::DEBUG << "; EM offline FEB total energy z = " << offlineemz << endmsg;
	}
#endif
        
// now do FCAL
	int countfcal = 0;
	int countfcal_not_found = 0;
	int countpfcal = 0;
        double totalfcal = 0;
        double totalfcalx = 0;
        double totalfcaly = 0;
        double totalfcalz = 0;
        double offlinefcalx = 0;
        double offlinefcaly = 0;
        double offlinefcalz = 0;
        ATH_MSG_DEBUG( "Compare FEBs in FCAL "  );
        doComparison(m_larfebcol_fcal,&larfebmap_fcal,countfcal,countfcal_not_found,countpfcal,totalfcal,totalfcalx,totalfcaly,totalfcalz,offlinefcalx,offlinefcaly,offlinefcalz);
        hist("NFCALLArFEBs")->Fill(countfcal);
        hist("NFCALBadLArFEBs")->Fill(countpfcal);
#ifndef NDEBUG
       	if ( msgLvl(MSG::DEBUG) ){
           float totfcalene = sqrt(pow(totalfcalx,2) + pow(totalfcaly,2) + pow(totalfcalz,2));
	   msg() << MSG::DEBUG << "HLT FCAL FEBs found : " << countfcal << endmsg;
	   msg() << "FCAL FEBs not found : " << countfcal_not_found << endmsg;
	   msg() << "FCAL FEBs with problems : " << countpfcal << endmsg;
	   msg() << "FCAL FEBs using difflimit  : " << m_difflimit << endmsg;
	   msg() << MSG::DEBUG << "FCAL HLT FEB total energy = " << totfcalene;
	   msg() << MSG::DEBUG << "; FCAL HLT FEB total energy x = " << totalfcalx;
	   msg() << MSG::DEBUG << "; FCAL HLT FEB total energy y = " << totalfcaly;
	   msg() << MSG::DEBUG << "; FCAL HLT FEB total energy z = " << totalfcalz << endmsg;
           float offlinefcalene = sqrt(offlinefcalx*offlinefcalx + offlinefcaly*offlinefcaly + offlinefcalz*offlinefcalz);
	   msg() << MSG::DEBUG << "FCAL offline FEB total energy = " << offlinefcalene;
	   msg() << MSG::DEBUG << "; FCAL offline FEB total energy x = " << offlinefcalx;
	   msg() << MSG::DEBUG << "; FCAL offline FEB total energy y = " << offlinefcaly;
	   msg() << MSG::DEBUG << "; FCAL offline FEB total energy z = " << offlinefcalz << endmsg;
	}
#endif
        
#ifndef NDEBUG
        double febcellx = 0;
        double febcelly = 0;
        double febcellz = 0;
	for( it = larfebmap.begin(); it!=larfebmap.end();++it){
                LArFebEnergy larfebenergy = (*it).second;
       		if ( msgLvl(MSG::VERBOSE) ){
		   msg() << MSG::VERBOSE << "FEB from cell ID ";
		   msg() << MSG::VERBOSE << std::hex;
		   msg() << MSG::VERBOSE << larfebenergy.getFebId() << " ";
		   msg() << MSG::VERBOSE << std::dec;
		   msg() << MSG::VERBOSE << endmsg;
		   msg() << MSG::VERBOSE << "FEB from cell ex = " << larfebenergy.getFebEx();
		   msg() << MSG::VERBOSE << "FEB from cell ey = " << larfebenergy.getFebEy();
		   msg() << MSG::VERBOSE << "FEB from cell ez = " << larfebenergy.getFebEz() << endmsg;
		}
                febcellx += larfebenergy.getFebEx();
                febcelly += larfebenergy.getFebEy();
                febcellz += larfebenergy.getFebEz();
		// Delete the pointers now. Don't use after this point.
		//delete larfebenergy;
	}
        double febcelltot = sqrt(pow(febcellx,2) + pow(febcelly,2) + pow(febcellz,2));
        ATH_MSG_DEBUG( "FEB from cell total ene = " << febcelltot
                       << "; FEB from cell Ex = " << febcellx
                       << "; FEB from cell Ey = " << febcelly
                       << "; FEB from cell Ez = " << febcellz  );

#endif
	//larfebmap.clear();
        //m_larfebcol.clear();

	delete pCaloCellContainer;
	return StatusCode::SUCCESS;

}


void HLTCaloFEBTool::doComparison(std::vector<LArFebEnergy> &febcoll,std::map<HWIdentifier,std::pair<LArFebEnergy,LArFebEnergy> >* larfebmap,int &count,int &count_not_found,int &countp,double &total,double &totalx,double &totaly,double &totalz,double &offlinefebex, double &offlinefebey, double &offlinefebez){

    std::map<HWIdentifier,std::pair<LArFebEnergy,LArFebEnergy> >::iterator it;
    for(it = larfebmap->begin(); it!=larfebmap->end();++it){
         LArFebEnergy larfebenergy = (*it).second.first;
         offlinefebex += larfebenergy.getFebEx();
         offlinefebey += larfebenergy.getFebEy();
         offlinefebez += larfebenergy.getFebEz();
    }
//
//  outer loop on online FEBs:
//
    for( std::vector<LArFebEnergy>::const_iterator tit= febcoll.begin(); tit!=febcoll.end(); ++tit) {
                double ex = (*tit).getFebEx();
                double ey = (*tit).getFebEy();
                double ez = (*tit).getFebEz();
		total = ex*ex;
		total += ey*ey;
		total += ez*ez;
		total = sqrt (total);
                totalx += ex;
                totaly += ey;
                totalz += ez;
		//if ( total > 1e3 ) {
                if(!(ex==0&&ey==0&&ez==0)){
                   count++;
#ifdef DONTDO
                   ATH_MSG_VERBOSE( "validation FEB ID = " << std::hex << (*tit)->getFebId() << std::dec 
                                    << "FEB Ex = " << ex
                                    << "FEB Ey = " << ey
                                    << "FEB Ez = " << ez  );
#endif
                }
		bool found = false;
//
//  start inner loop on offline FEBs
//
                for(it = larfebmap->begin(); it!=larfebmap->end();++it){
                        LArFebEnergy larfebenergy = (*it).second.first;
                        unsigned int febid = (*tit).getFebId();
                        unsigned int febidshort = febid/0x1000000;
                        if ( febid == larfebenergy.getFebId() ){
                          float offFebEx = larfebenergy.getFebEx();
                          float offFebEy = larfebenergy.getFebEy();
                          float offFebEz = larfebenergy.getFebEz();
                          //float offFebE = sqrt(offFebEx*offFebEx + offFebEy*offFebEy
                                                 //+ offFebEz*offFebEz);
                          found = true;
                          float diffx = std::abs(offFebEx
                                - (*tit).getFebEx() );
                          //float diffxp = 0;
                          //if ( std::abs ((*tit).getFebEx()) > 0.2 )
                          //      diffxp = 100*diffx/(*tit).getFebEx();
                          float diffy = std::abs(offFebEy
                                - (*tit).getFebEy() );
                          //float diffyp = 0;
                          //if ( std::abs ((*tit).getFebEy()) > 0.2 )
                          //      diffyp = 100*diffy/(*tit).getFebEy();
                          float diffz = std::abs(offFebEz
                                - (*tit).getFebEz() );
                          //float diffzp = 0;
                          //float offFebPhi = atan2f(offFebEy,offFebEx);
                          //float offFebEta = atanh(offFebEz/offFebE);
                          float febene = sqrt( ((*tit).getFebEx())*((*tit).getFebEx()) +
                                ((*tit).getFebEy())*((*tit).getFebEy()) +
                                ((*tit).getFebEz())*((*tit).getFebEz()) );
                          float febex = (*tit).getFebEx();
                          float febey = (*tit).getFebEy();
                          float febez = (*tit).getFebEz();
                          //if ( std::abs ((*tit).getFebEz()) > 0.2 )
                          //      diffzp = 100*diffz/(*tit).getFebEz();
                          if ( ((diffx >= m_difflimit) ||
                               (diffy >= m_difflimit) ||
                               (diffz >= m_difflimit)) && febene!=0 ){
                                countp++; 
                                float febphi = 999999;
                                float febeta = 999999;
				LArFebEnergy larfebnoise = (*it).second.second;
                                if(febene!=0){
                                   febphi = atan2f((*tit).getFebEy(),(*tit).getFebEx());
                                   febeta = atanh((*tit).getFebEz()/febene);
                                }
//#ifndef NDEBUG
				if ( msgLvl(MSG::DEBUG) ) {
                                    float offFebSEx = larfebnoise.getFebEx();
                                    float offFebSEy = larfebnoise.getFebEy();
                                    float offFebSEz = larfebnoise.getFebEz();
                                    msg() << MSG::DEBUG;
                                    msg() << "Found Prob in : ";
                                    //msg() << std::hex;
                                    msg() << std::hex;
                                    msg() << "Offline ID " << larfebenergy.getFebId() << " ";
                                    msg() << "HLT ID " << (*tit).getFebId() << " ";
                                    msg() << std::dec;
                                    msg() << "Offline Ex " << larfebenergy.getFebEx() << " ";
                                    msg() << "Online Ex " << (*tit).getFebEx() << " ";
                                    msg() << "Offline Ey " << larfebenergy.getFebEy() << " ";
                                    msg() << "Online Ey " << (*tit).getFebEy() << " ";
                                    msg() << "Offline Ez " << larfebenergy.getFebEz() << " ";
                                    msg() << "Online Ez " << (*tit).getFebEz() << " ";
                                    msg() << "Offline sigma Ex " << offFebSEx << " ";
                                    msg() << "Offline sigma Ey " << offFebSEy << " ";
                                    msg() << "Offline sigma Ez " << offFebSEz << " ";
                                    msg() << endmsg;
				}
//#endif
                                if(febidshort == 0x38) {
#ifndef NDEBUG
                                    ATH_MSG_VERBOSE( "This is in the C-side EM barrel"  );
                                    ATH_MSG_VERBOSE( "febeta = " << febeta  );
                                    ATH_MSG_VERBOSE( "febphi = " << febphi  );
#endif
                                    hist2("etaphiEMBCFEBLAr")->Fill(febeta,febphi);
                                    //hist("deltaetaEMBCFEBLAr")->Fill(febeta-offFebEta);
                                    //hist("deltaphiEMBCFEBLAr")->Fill(febphi-offFebPhi);
                                    hist("deltaEnexEMBCFEBLAr")->Fill(febex-offFebEx);
                                    hist("deltaEneyEMBCFEBLAr")->Fill(febey-offFebEy);
                                    hist("deltaEnezEMBCFEBLAr")->Fill(febez-offFebEz);
                                    hist("deltaTotEneEMBCFEBLAr")->Fill( 
                                         sqrt(pow(febex,2)+pow(febey,2)+pow(febez,2)) - 
                                         sqrt(pow(offFebEx,2)+pow(offFebEy,2)+pow(offFebEz,2)) );
                                }
                                else if(febidshort == 0x39){
#ifndef NDEBUG
                                   ATH_MSG_VERBOSE( "This is in the A-side EM barrel"  );
                                   ATH_MSG_VERBOSE( "febeta = " << febeta  );
                                   ATH_MSG_VERBOSE( "febphi = " << febphi  );
#endif
                                   hist2("etaphiEMBAFEBLAr")->Fill(febeta,febphi);
                                   //hist("deltaetaEMBAFEBLAr")->Fill(febeta-offFebEta);
                                   //hist("deltaphiEMBAFEBLAr")->Fill(febphi-offFebPhi);
                                   hist("deltaEnexEMBAFEBLAr")->Fill(febex-offFebEx);
                                   hist("deltaEneyEMBAFEBLAr")->Fill(febey-offFebEy);
                                   hist("deltaEnezEMBAFEBLAr")->Fill(febez-offFebEz);
                                   hist("deltaTotEneEMBAFEBLAr")->Fill( 
                                         sqrt(pow(febex,2)+pow(febey,2)+pow(febez,2)) - 
                                         sqrt(pow(offFebEx,2)+pow(offFebEy,2)+pow(offFebEz,2)) );
                                }
                                else if(febidshort == 0x3a)
                                   switch(febid){
                                     case 0x3a300000:
                                     case 0x3a308000:
                                     case 0x3a310000:
                                     case 0x3a318000:
                                     case 0x3a320000:
                                     case 0x3a328000:
                                     case 0x3a330000:
                                     case 0x3a340000:
                                     case 0x3a348000:
                                     case 0x3a350000:
                                     case 0x3a358000:
                                     case 0x3a360000:
                                     case 0x3a368000:
                                     case 0x3a370000:
#ifndef NDEBUG
                                       ATH_MSG_VERBOSE( "This is in the C-side fcal"  );
#endif
                                        hist2("etaphiFCALCFEBLAr")->Fill(febeta,febphi);
                                        //hist("deltaetaFCALCFEBLAr")->Fill(febeta-offFebEta);
                                        //hist("deltaphiFCALCFEBLAr")->Fill(febphi-offFebPhi);
                                        hist("deltaEnexFCALCFEBLAr")->Fill(febex-offFebEx);
                                        hist("deltaEneyFCALCFEBLAr")->Fill(febey-offFebEy);
                                        hist("deltaEnezFCALCFEBLAr")->Fill(febez-offFebEz);
                                        hist("deltaTotEneFCALCFEBLAr")->Fill( 
                                            sqrt(pow(febex,2)+pow(febey,2)+pow(febez,2)) - 
                                            sqrt(pow(offFebEx,2)+pow(offFebEy,2)+pow(offFebEz,2)) );
                                        break;
                                     case 0x3a520000:
                                     case 0x3a528000:
                                     case 0x3a1a0000:
                                     case 0x3a1a8000:
                                     case 0x3ab20000:
                                     case 0x3ab28000:
                                     case 0x3a820000:
                                     case 0x3a828000:
                                     case 0x3a530000:
                                     case 0x3a538000:
                                     case 0x3a1b0000:
                                     case 0x3a1b8000:
                                     case 0x3ab30000:
                                     case 0x3ab38000:
                                     case 0x3a830000:
                                     case 0x3a838000:
                                     case 0x3a540000:
                                     case 0x3a548000:
                                     case 0x3a1c0000:
                                     case 0x3a1c8000:
                                     case 0x3ab40000:
                                     case 0x3ab48000:
                                     case 0x3a840000:
                                     case 0x3a848000:
#ifndef NDEBUG
                                        ATH_MSG_VERBOSE( "This is in the C-side HEC "  );
#endif
                                        hist2("etaphiHECCFEBLAr")->Fill(febeta,febphi);
                                        //hist("deltaetaHECCFEBLAr")->Fill(febeta-offFebEta);
                                        //hist("deltaphiHECCFEBLAr")->Fill(febphi-offFebPhi);
                                        hist("deltaEnexHECCFEBLAr")->Fill(febex-offFebEx);
                                        hist("deltaEneyHECCFEBLAr")->Fill(febey-offFebEy);
                                        hist("deltaEnezHECCFEBLAr")->Fill(febez-offFebEz);
                                        hist("deltaTotEneHECCFEBLAr")->Fill( 
                                            sqrt(pow(febex,2)+pow(febey,2)+pow(febez,2)) - 
                                            sqrt(pow(offFebEx,2)+pow(offFebEy,2)+pow(offFebEz,2)) );
                                        break;
                                     default: 
#ifndef NDEBUG
                                        ATH_MSG_VERBOSE( "This is in the C-side EMC"  );
#endif
                                        hist2("etaphiEMCCFEBLAr")->Fill(febeta,febphi);
                                        //hist("deltaetaEMCCFEBLAr")->Fill(febeta-offFebEta);
                                        //hist("deltaphiEMCCFEBLAr")->Fill(febphi-offFebPhi);
                                        hist("deltaEnexEMCCFEBLAr")->Fill(febex-offFebEx);
                                        hist("deltaEneyEMCCFEBLAr")->Fill(febey-offFebEy);
                                        hist("deltaEnezEMCCFEBLAr")->Fill(febez-offFebEz);
                                        hist("deltaTotEneEMCCFEBLAr")->Fill( 
                                            sqrt(pow(febex,2)+pow(febey,2)+pow(febez,2)) - 
                                            sqrt(pow(offFebEx,2)+pow(offFebEy,2)+pow(offFebEz,2)) );
                                        break;
                                   }
                                else if(febidshort == 0x3b)
                                   switch(febid){
                                     case 0x3b300000:
                                     case 0x3b308000:
                                     case 0x3b310000:
                                     case 0x3b318000:
                                     case 0x3b320000:
                                     case 0x3b328000:
                                     case 0x3b330000:
                                     case 0x3b340000:
                                     case 0x3b348000:
                                     case 0x3b350000:
                                     case 0x3b358000:
                                     case 0x3b360000:
                                     case 0x3b368000:
                                     case 0x3b370000:
#ifndef NDEBUG
                                        ATH_MSG_VERBOSE( "This is in the A-side fcal"  );
#endif
                                        hist2("etaphiFCALAFEBLAr")->Fill(febeta,febphi);
                                        //hist("deltaetaFCALAFEBLAr")->Fill(febeta-offFebEta);
                                        //hist("deltaphiFCALAFEBLAr")->Fill(febphi-offFebPhi);
                                        hist("deltaEnexFCALAFEBLAr")->Fill(febex-offFebEx);
                                        hist("deltaEneyFCALAFEBLAr")->Fill(febey-offFebEy);
                                        hist("deltaEnezFCALAFEBLAr")->Fill(febez-offFebEz);
                                        hist("deltaTotEneFCALAFEBLAr")->Fill( 
                                            sqrt(pow(febex,2)+pow(febey,2)+pow(febez,2)) - 
                                            sqrt(pow(offFebEx,2)+pow(offFebEy,2)+pow(offFebEz,2)) );
                                        break;
                                     case 0x3b520000:
                                     case 0x3b528000:
                                     case 0x3b1a0000:
                                     case 0x3b1a8000:
                                     case 0x3bb20000:
                                     case 0x3bb28000:
                                     case 0x3b820000:
                                     case 0x3b828000:
                                     case 0x3b530000:
                                     case 0x3b538000:
                                     case 0x3b1b0000:
                                     case 0x3b1b8000:
                                     case 0x3bb30000:
                                     case 0x3bb38000:
                                     case 0x3b830000:
                                     case 0x3b838000:
                                     case 0x3b540000:
                                     case 0x3b548000:
                                     case 0x3b1c0000:
                                     case 0x3b1c8000:
                                     case 0x3bb40000:
                                     case 0x3bb48000:
                                     case 0x3b840000:
                                     case 0x3b848000:
#ifndef NDEBUG
                                        ATH_MSG_VERBOSE( "This is in the A-side HEC "  );
#endif
                                        hist2("etaphiHECAFEBLAr")->Fill(febeta,febphi);
                                        //hist("deltaetaHECAFEBLAr")->Fill(febeta-offFebEta);
                                        //hist("deltaphiHECAFEBLAr")->Fill(febphi-offFebPhi);
                                        hist("deltaEnexHECAFEBLAr")->Fill(febex-offFebEx);
                                        hist("deltaEneyHECAFEBLAr")->Fill(febey-offFebEy);
                                        hist("deltaEnezHECAFEBLAr")->Fill(febez-offFebEz);
                                        hist("deltaTotEneHECAFEBLAr")->Fill( 
                                            sqrt(pow(febex,2)+pow(febey,2)+pow(febez,2)) - 
                                            sqrt(pow(offFebEx,2)+pow(offFebEy,2)+pow(offFebEz,2)) );
                                        break;
                                     default: 
#ifndef NDEBUG
                                        ATH_MSG_VERBOSE( "This is in the A-side EMC"  );
#endif
                                        hist2("etaphiEMCAFEBLAr")->Fill(febeta,febphi);
                                        //hist("deltaetaEMCAFEBLAr")->Fill(febeta-offFebEta);
                                        //hist("deltaphiEMCAFEBLAr")->Fill(febphi-offFebPhi);
                                        hist("deltaEnexEMCAFEBLAr")->Fill(febex-offFebEx);
                                        hist("deltaEneyEMCAFEBLAr")->Fill(febey-offFebEy);
                                        hist("deltaEnezEMCAFEBLAr")->Fill(febez-offFebEz);
                                        hist("deltaTotEneEMCAFEBLAr")->Fill( 
                                            sqrt(pow(febex,2)+pow(febey,2)+pow(febez,2)) - 
                                            sqrt(pow(offFebEx,2)+pow(offFebEy,2)+pow(offFebEz,2)) );
                                        break;
                                   }
                          } // end if matched FEB within limit
                          break;
                        } // enf if matching FEB Ids
                } // end for loop over FEBs from offline
           
		if ( (!found)&&(ex!=0||ey!=0||ez!=0) ) count_not_found++;
		//}
	}
}
