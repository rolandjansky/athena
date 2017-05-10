/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloMonitoring/HLTCaloTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigCaloRec/IAlgToolEFCalo.h"
#include "CaloEvent/CaloCellContainer.h"

#include "LArIdentifier/LArIdManager.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"
#include "CaloIdentifier/TileID.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"


#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include <string>

HLTCaloTool::HLTCaloTool(const std::string & type, const std::string & name, const IInterface* parent) : IHLTMonTool(type,name,parent),
 m_tcrAlgTools(this),
 m_onlineHelper(NULL),
 m_tileID(NULL)
{
	declareProperty ("TCRTools", m_tcrAlgTools);
	declareProperty ("DoNtuple", m_ntuple = false);
	// Nothing for the moment
	return;
}

HLTCaloTool::~HLTCaloTool() {
}

StatusCode HLTCaloTool::init() {
	(*m_log) << MSG::DEBUG << "Initializing" << endmsg;

	if ( m_tcrAlgTools.retrieve().isFailure() ) {
    		(*m_log) << MSG::ERROR << "Failed to retrieve helper tools: " << m_tcrAlgTools << endmsg;
		return StatusCode::FAILURE;
	} else {
		(*m_log) << MSG::DEBUG << "Retrieved " << m_tcrAlgTools << endmsg;
	}
	if ( m_cablingSvc.retrieve().isFailure() ) {
                 (*m_log) << MSG::ERROR << "Failed to retrieve helper tools: " << m_cablingSvc << endmsg;
                 return StatusCode::FAILURE;
        } else {
                 (*m_log) << MSG::INFO << "Retrieved " << m_cablingSvc << endmsg;
        }

        ServiceHandle<StoreGateSvc> detStore("DetectorStore",name());
        if ( detStore.retrieve().isFailure() ) {
               (*m_log) << MSG::ERROR << "Unable to retrieve DetectorStore" << endmsg;
                return StatusCode::FAILURE;
        }
        const LArIdManager* larMgr;
        if ( (detStore->retrieve(larMgr)).isFailure() ) {
                (*m_log) << MSG::ERROR << "Unable to retrieve LArIdManager from DetectorStore" << endmsg;
                return StatusCode::FAILURE;
        } else {
                (*m_log) << MSG::DEBUG << "Successfully retrieved LArIdManager from DetectorStore" << endmsg;
        }
        m_onlineHelper = larMgr->getOnlineID();
        if (!m_onlineHelper) {
                (*m_log) << MSG::ERROR << "Could not access LArOnlineID helper" << endmsg;
                return StatusCode::FAILURE;
        } else {
        (*m_log) << MSG::DEBUG << "Successfully accessed LArOnlineID helper" << endmsg;
        }
        if ( (detStore->retrieve(m_tileID)).isFailure() ) {
                (*m_log) << MSG::ERROR << "Could not access TileID" << endmsg;
                return StatusCode::FAILURE;
	}

	return StatusCode::SUCCESS;

}

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTCaloTool::book() {
#else
StatusCode HLTCaloTool::book(bool newEventsBlock, bool newLumiBlock, bool newRun) {
#endif

  addMonGroup( new MonGroup(this,"HLT/CaloMon",run) );

#ifdef ManagedMonitorToolBase_Uses_API_201401
  if ( newRunFlag() ) {
#else
  if ( newRun ) {
#endif

  addHistogram(new TH1F("NCellsLAr","Number of HLT LAr cells; Number of HLT Cells; Number of Events",100,1.0e5,1.85e5) );
  addHistogram(new TH1F("NBadCellsLAr","Number of Bad LAr Cells; Number of Bad Cells; Number of Events",50,0,10000.) );
  addHistogram(new TH1F("PercentDiffLAr","Percentual Difference in LAr; %; Number of Events",80,0,20.0) );
  addHistogram(new TH1F("NCellsTile","Number of HLT Tile cells; Number of HLT Cells; Number of Events",100,3e3,6e3) );
  addHistogram(new TH1F("NBadCellsTile","Number of Bad Tile Cells; Number of Bad Cells; Number of Events",50,0,2500.) );
  addHistogram(new TH1F("PercentDiffTile","Percentual Difference in Tile; %; Number of Events",100,0,100.0) );
  addHistogram(new TH1F("conversionErrors","Number of conversionErrors per Event; conversionErrors; Number of Events",20,0,20.0) );
  addHistogram(new TH2F("etaphiLAr","Position of LAr cells with some important difference; \\eta; \\phi",100,-5.,5.,64,-3.2,3.2) );
  addHistogram(new TH2F("etaphiTile","Position of Tile cells with some important difference; \\eta; \\phi",85,-1.7,1.7,64,-3.2,3.2) );
  addHistogram(new TH2F("EnergyAccetaphiLAr","Transverse Energy accumulation for LAr cells; \\eta; \\phi",100,-5.,5.,64,-3.2,3.2) );
  addHistogram(new TH2F("EnergyAccetaphiTile","Transverse Energy accumulation for Tile cells; \\eta; \\phi",85,-1.7,1.7,64,-3.2,3.2) );
  addHistogram(new TH2F("HitAccetaphiLAr","Hit accumulation for LAr cells; \\eta; \\phi",100,-5.,5.,64,-3.2,3.2) );
  addHistogram(new TH2F("HitAccetaphiTile","Hit accumulation for Tile cells; \\eta; \\phi",85,-1.7,1.7,64,-3.2,3.2) );
  // lartile=0?lar:tile
  if ( m_ntuple ) 
  addTree( new TNtuple("Details","Details","et:eta:phi:gain:tet:teta:tphi:tgain:lartile") );
  
#ifdef ManagedMonitorToolBase_Uses_API_201401
  }else if ( newEventsBlockFlag() || newLumiBlockFlag() ){
#else
  }else if ( newEventsBlock || newLumiBlock ){
#endif
    return StatusCode::SUCCESS;
  }

  *m_log << MSG::DEBUG << "End of book" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode HLTCaloTool::fill() {

	const DataHandle<CaloCellContainer> AllCalo;
	if ( m_storeGate->retrieve(AllCalo,"AllCalo").isFailure() ){
		(*m_log) << MSG::DEBUG << "No Calo Cell Container found"
                        << endmsg;
		return StatusCode::SUCCESS;
	}
	if ( (*m_log).level() <= MSG::DEBUG ) {
	  (*m_log) << MSG::DEBUG << "Got container "; 
	  (*m_log) << "Size : " << AllCalo->size() << endmsg;
	}

	CaloCellContainer* pCaloCellContainer
		= new CaloCellContainer(SG::VIEW_ELEMENTS);
	ToolHandleArray<IAlgToolEFCalo>::iterator itrtcr = m_tcrAlgTools.begin();
	ToolHandleArray<IAlgToolEFCalo>::iterator endtcr = m_tcrAlgTools.end();
	double etamin, etamax,phimin, phimax;
	etamin=-4.8;
	etamax=4.8;
        uint32_t conversionError=0; // counter
	uint32_t error=0x0;
	for (; itrtcr!=endtcr; ++itrtcr) {

	if ( (*m_log).level() <= MSG::DEBUG ) {
	   (*m_log) << MSG::DEBUG << "Tool name : " 
		<< (*itrtcr).name() << endmsg;
	}
	phimin=-M_PI+0.001;
	phimax=M_PI-0.001;
	// Fix for stupid RS problem
	if ( (*itrtcr).name().find("Tile")!=std::string::npos ){
		phimin=0+0.001;
		phimax=2*M_PI-0.001;
	}
	StatusCode sc;
	double eta0=(etamax+etamin)/2;
	double phi0=(phimax+phimin)/2;
	if((*itrtcr).name() == "FullCaloCellContMaker") {
		sc= (*itrtcr)->execute(*pCaloCellContainer);
	} else {
	        sc = (*itrtcr)->execute(*pCaloCellContainer, TrigRoiDescriptor( eta0, etamin, etamax,phi0, phimin, phimax ) );
	}
	if ( sc.isFailure() ) {
		(*m_log) << MSG::ERROR << "Problem with filling the cont"
			<< endmsg;
	} else {
	  // This method does not exist in this place now
          uint32_t in_error = (*itrtcr)->report_error();
          //uint32_t in_error = 0x0;
          if (0x0FFFFFFF & in_error) conversionError++;
          error|=in_error;
        }
	} // End of loop over tools
	hist("conversionErrors")->Fill(conversionError);
	if ( error ) {
		(*m_log) << MSG::DEBUG << "Problems in unpacking : "
		<< error << endmsg;
	}

	if ( (*m_log).level() <=MSG::DEBUG) {
	(*m_log) << MSG::DEBUG << "HLT Container size : "
		<< pCaloCellContainer->size() << endmsg;
	(*m_log) << MSG::DEBUG << "Differences in container sizes : "
		<< (pCaloCellContainer->size())-(AllCalo->size()) <<endmsg;
	}
	CaloCellContainer::const_iterator tbeg = pCaloCellContainer->begin();
	CaloCellContainer::const_iterator tend = pCaloCellContainer->end();
	int count_tcellsL=0;
	int count_tcellspL=0;
	int count_tcellsT=0;
	int count_tcellspT=0;
        TH2* hist_etaphiTile = hist2("etaphiTile");
        TH2* hist_etaphiLAr = hist2("etaphiLAr");
	TH1* hist_PercentDiffTile = hist("PercentDiffTile");
	TH1* hist_PercentDiffLAr = hist("PercentDiffLAr");
	TH2* hist_EnergyAccetaphiTile = hist2("EnergyAccetaphiTile");
	TH2* hist_EnergyAccetaphiLAr = hist2("EnergyAccetaphiLAr");
	TH2* hist_HitAccetaphiTile = hist2("HitAccetaphiTile");
	TH2* hist_HitAccetaphiLAr = hist2("HitAccetaphiLAr");
	for(; tbeg!=tend ; ++tbeg){
		CaloCell* tcell = (*tbeg);
		IdentifierHash tid = tcell->caloDDE()->calo_hash();
		float tenergy = tcell->energy();
		CaloGain::CaloGain tgain = tcell->gain();
		const CaloCell* cell = AllCalo->findCell(tid);

		if ( cell && cell->caloDDE() ) {
			
			float energy = cell->energy();
			CaloGain::CaloGain gain = cell->gain();
			float diff = fabsf(tenergy-energy);
			float diffp = 0;
			float diff_time = 0;
			if ( (cell->quality()!=0) && (tcell->quality()!=0) )
				diff_time = fabs(cell->time()-tcell->time());
			double lartile = 0;
                        if (  (cell->et()) > 100 ){
                           diffp = 100*diff/energy;
                        }
                        if ( cell->caloDDE()->is_tile() )hist_PercentDiffTile->Fill(diffp);
                        else hist_PercentDiffLAr->Fill(diffp);
			if ( ( ((diff>20) && (diffp>6)) || gain!= tgain || (diff_time>1 && !cell->caloDDE()->is_tile() ) ) ) {
			//if ( ( (diff_time>3.6 && diff_time<4.6) && !cell->caloDDE()->is_tile() )  ) {
				if ( (*m_log).level() <=MSG::VERBOSE) {
				(*m_log) << MSG::VERBOSE;
				(*m_log) << "Offline cell found";
				(*m_log) << " energy : " << cell->energy();
				(*m_log) << " tenergy : " << tcell->energy();
				(*m_log) << " eta : " << cell->eta();
				(*m_log) << " teta : " << tcell->eta();
				(*m_log) << " phi : " << cell->phi();
				(*m_log) << " tphi : " << tcell->phi();
				(*m_log) << " samp : " << cell->caloDDE()->getSampling();
				(*m_log) << " tsamp : " << tcell->caloDDE()->getSampling();
                                (*m_log) << " time : " << cell->time();
                                (*m_log) << " ttime : " << tcell->time();
				(*m_log) << " qua : " << cell->quality();
				(*m_log) << " tqua : " << tcell->quality();
				(*m_log) << " gain : " << cell->gain();
				(*m_log) << " tgain : " << tcell->gain();
				(*m_log) << " ID : " << cell->ID();
				}
				if ( cell->caloDDE()->is_tile() ){
				 if ( (*m_log).level() <=MSG::VERBOSE)
				 (*m_log) << " TileID : " << m_tileID->to_string(cell->ID(),-2);
				 hist_etaphiTile->Fill(cell->eta(),cell->phi());
				 //hist_PercentDiffTile->Fill(diffp);  //moved up
				 lartile=1.0;
				 count_tcellspT++;
				}else{
				 if ( (*m_log).level() <=MSG::VERBOSE){
				   Identifier idh = cell->ID();
                		   HWIdentifier channel_hw = m_cablingSvc->createSignalChannelID(idh);
                		   HWIdentifier feb_hw = m_onlineHelper->feb_Id(channel_hw);
				   (*m_log) << " febID : " << feb_hw;
				 }
				 hist_etaphiLAr->Fill(cell->eta(),cell->phi());
				 //hist_PercentDiffLAr->Fill(diffp);//moved up
				 count_tcellspL++;
				}
				if ( (*m_log).level() <=MSG::VERBOSE)
				  (*m_log) << endmsg;
				if ( m_ntuple ) 
				 ((TNtuple*)tree("Details"))->Fill(cell->et(),cell->eta(),cell->phi(),(double)cell->gain(),tcell->et(),tcell->eta(),tcell->phi(),(double)tcell->gain(),lartile ); 
			}
		} else { // Cells not found are also a problem
		  if ( tcell->caloDDE() && tcell->caloDDE()->is_tile() )
			count_tcellspT++;
		  else
			count_tcellspL++;
		}
		if ( tcell->caloDDE() && tcell->caloDDE()->is_tile() ){
			hist_EnergyAccetaphiTile->Fill(tcell->eta(), tcell->phi(), tcell->et() );
			hist_HitAccetaphiTile->Fill(tcell->eta(), tcell->phi() );
			count_tcellsT++;
		}else{
			hist_EnergyAccetaphiLAr->Fill(tcell->eta(), tcell->phi(), tcell->et() );
			hist_HitAccetaphiLAr->Fill(tcell->eta(), tcell->phi() );
			count_tcellsL++;
		}
	}
	hist("NBadCellsLAr")->Fill(count_tcellspL);
	hist("NCellsLAr")->Fill(count_tcellsL);
	hist("NBadCellsTile")->Fill(count_tcellspT);
	hist("NCellsTile")->Fill(count_tcellsT);
	if ( (*m_log).level() <= MSG::DEBUG) {
		(*m_log) << MSG::DEBUG << "Number of LAr cells found " << count_tcellsL << endmsg;
		(*m_log) << MSG::DEBUG << "Number of LAr cells w problems " << count_tcellspL << endmsg;
		(*m_log) << MSG::DEBUG << "Number of Tile cells found " << count_tcellsT << endmsg;
		(*m_log) << MSG::DEBUG << "Number of Tile cells w problems " << count_tcellspT << endmsg;
	}
	delete pCaloCellContainer;

	return StatusCode::SUCCESS;


}

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTCaloTool::proc(){
#else
StatusCode HLTCaloTool::proc(bool /*endOfEventsBlock*/, bool /*endOfLumiBlock*/, bool /*endOfRun*/){
#endif

   return StatusCode::SUCCESS;
}


