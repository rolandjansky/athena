/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloMonitoring/HLTCaloToolL2.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloEvent/CaloCellContainer.h"

#include "LArIdentifier/LArIdManager.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCabling/LArCablingService.h"
#include "CaloIdentifier/TileID.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include <string>

HLTCaloToolL2::HLTCaloToolL2(const std::string & type, const std::string & name, const IInterface* parent) : IHLTMonTool(type,name,parent),
        m_onlineHelper(NULL),
	m_tileID(NULL)
{
	declareProperty ("DoNtuple", m_ntuple = false);
	declareProperty ("ListOfCellsKeys", m_listOfCellsKeys);
	// Nothing for the moment
        m_listOfCellsKeys.push_back("HLT_TrigT2CaloEgammaCells");
	return;
}

HLTCaloToolL2::~HLTCaloToolL2() {
}

StatusCode HLTCaloToolL2::init() {
        ATH_MSG_DEBUG( "Initializing"  );
        for(std::vector<std::string>::const_iterator i =
		m_listOfCellsKeys.begin(); i!= m_listOfCellsKeys.end(); ++i){
                ATH_MSG_DEBUG( "Will use this key : " << (*i)  );
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
        if ( (detStore()->retrieve(m_tileID)).isFailure() ) {
                ATH_MSG_ERROR( "Could not access TileID"  );
                return StatusCode::FAILURE;
        }


	return StatusCode::SUCCESS;

}

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTCaloToolL2::book() {
#else
StatusCode HLTCaloToolL2::book(bool newEventsBlock, bool newLumiBlock, bool newRun) {
#endif

  addMonGroup( new MonGroup(this,"HLT/CaloMonL2",run) );

  if ( newRunFlag() ) {

  addHistogram(new TH1F("NCellsLAr","Number of HLT LAr cells; Number of HLT Cells; Number of Events",100,1,1501) );
  addHistogram(new TH1F("NBadCellsLAr","Number of Bad LAr Cells; Number of Bad Cells; Number of Events",50,0,10000.) );
  addHistogram(new TH1F("PercentDiffLAr","Percentual Difference in LAr; %; Number of Events",80,0,10.0) );
  addHistogram(new TH1F("NCellsTile","Number of HLT Tile cells; Number of HLT Cells; Number of Events",100,1,301) );
  addHistogram(new TH1F("NBadCellsTile","Number of Bad Tile Cells; Number of Bad Cells; Number of Events",50,0,250.) );
  addHistogram(new TH1F("PercentDiffTile","Percentual Difference in Tile; %; Number of Events",80,0,10.0) );
  addHistogram(new TH2F("etaphiLAr","Position of LAr cells with some important difference; \\eta; \\phi",100,-5.,5.,64,-3.2,3.2) );
  addHistogram(new TH2F("etaphiTile","Position of Tile cells with some important difference; \\eta; \\phi",85,-1.7,1.7,64,-3.2,3.2) );
  addHistogram(new TH2F("EnergyAccetaphiLAr","Transverse Energy accumulation for LAr cells; \\eta; \\phi",100,-5.,5.,64,-3.2,3.2) );
  addHistogram(new TH2F("EnergyAccetaphiTile","Transverse Energy accumulation for Tile cells; \\eta; \\phi",85,-1.7,1.7,64,-3.2,3.2) );
  addHistogram(new TH2F("HitAccetaphiLAr","Hit accumulation for LAr cells; \\eta; \\phi",100,-5.,5.,64,-3.2,3.2) );
  addHistogram(new TH2F("HitAccetaphiTile","Hit accumulation for Tile cells; \\eta; \\phi",85,-1.7,1.7,64,-3.2,3.2) );

  // Cells profile histograms
  addHistogram(new TH1F("ECellsLRoI","Energy for HLT LAr Cells; E_{T} [GeV]; Number of cells",150,0,3e3) );
  addHistogram(new TH1F("ECellsTRoI","Energy for HLT Tile Cells; E_{T} [GeV]; Number of cells",150,0,3e3) );
  addHistogram(new TH1F("ECellsLRoI_QC65535","Energy for HLT LAr Cells with Quality<65536; E_{T} [GeV]; Number of cells",150,0,3e3) );
  addHistogram(new TH1F("ECellsTRoI_QC16","Energy for HLT Tile Cells with Quality<16; E_{T} [GeV]; Number of cells",150,0,3e3) );
  addHistogram(new TH1F("ECellsLRoI_QC4000","Energy for HLT LAr Cells with Quality<4000; E_{T} [GeV]; Number of cells",150,0,3e3) );
  addHistogram(new TH1F("ECellsTRoI_QC1","Energy for HLT Tile Cells with Quality<1; E_{T} [GeV]; Number of cells",150,0,3e3) );
  addHistogram(new TH1F("TimeCellsLRoI","Pulse Time for HLT LAr Cells; Time [ns]; Number of cells",200,-100,100) );
  addHistogram(new TH1F("TimeCellsTRoI","Pulse Time for HLT Tile Cells; Time [ns]; Number of cells",200,-100,100) );
  addHistogram(new TH1F("TimeCellsLRoI_EC1G","Pulse Time for HLT LAr Cells with E>1GeV; Time [ns]; Number of cells",200,-100,100) );
  addHistogram(new TH1F("TimeCellsTRoI_EC1G","Pulse Time for HLT Tile Cells with E>1GeV; Time [ns]; Number of cells",200,-100,100) );
  addHistogram(new TH1F("TimeCellsLRoI_EC10G","Pulse Time for HLT LAr Cells with E>10GeV; Time [ns]; Number of cells",200,-100,100) );
  addHistogram(new TH1F("TimeCellsTRoI_EC10G","Pulse Time for HLT Tile Cells with E>10GeV; Time [ns]; Number of cells",200,-100,100) );
  addHistogram(new TH1F("QCellsLRoI","Quality for HLT LAr Cells; Quality; Number of cells",200,0,65536) );
  addHistogram(new TH1F("QCellsTRoI","Quality for HLT Tile Cells; Quality; Number of cells",200,0,256) );

  // Cells energies for eta/phi
  addHistogram(new TH2F("LArCellsEta","LAr Cells E_{T} for #eta; #eta ; LArCells E_{T}",50,-2.5,2.5,50,0,50) );
  addHistogram(new TH2F("LArCellsPhi","LAr Cells E_{T} for #phi; #phi ; LArCells E_{T}",64,-3.2,3.2,50,0,50) );
  addHistogram(new TH2F("TileCellsEta","Tile Cells E_{T} for #eta; #eta ; TileCells E_{T}",50,-2.5,2.5,50,0,50) );
  addHistogram(new TH2F("TileCellsPhi","Tile Cells E_{T} for #phi; #phi ; TileCells E_{T}",64,-3.2,3.2,50,0,50) );

  // lartile=0?lar:tile
  if ( m_ntuple ) 
  addTree( new TNtuple("Details","Details","et:eta:phi:gain:tet:teta:tphi:tgain:lartile") );
  
  }else if ( newEventsBlockFlag() || newLumiBlockFlag() ){
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG( "End of book"  );
  return StatusCode::SUCCESS;
}

StatusCode HLTCaloToolL2::fill() {

	const DataHandle<CaloCellContainer> AllCalo;
	if ( evtStore()->retrieve(AllCalo,"AllCalo").isFailure() ){
                ATH_MSG_DEBUG( "No Calo Cell Container found" );
		return StatusCode::SUCCESS;
	}
        ATH_MSG_DEBUG( "Got container " << "Size : " << AllCalo->size()  );
	int count_tcellsL=0;
	int count_tcellspL=0;
	int count_tcellsT=0;
	int count_tcellspT=0;
	for(std::vector< std::string >::const_iterator key =
                m_listOfCellsKeys.begin(); key!= m_listOfCellsKeys.end();++key){
	const DataHandle<CaloCellContainer> pCaloCellContainer;
	if ( evtStore()->retrieve(pCaloCellContainer,*key).isFailure() ){
                ATH_MSG_DEBUG( "No HLT Calo Cell Container found" );
		return StatusCode::SUCCESS;
	}
        ATH_MSG_DEBUG( "HLT Container size : " << pCaloCellContainer->size()  );

	// Cache pointers to improve speed
        TH2* hist2_etaphiTile = hist2("etaphiTile");
        TH1* hist_PercentDiffTile = hist("PercentDiffTile");
        TH2* hist2_etaphiLAr   = hist2("etaphiLAr");
        TH1* hist_PercentDiffLAr = hist("PercentDiffLAr");
        TH2* hist2_EnergyAccetaphiTile = hist2("EnergyAccetaphiTile");
        TH2* hist2_HitAccetaphiTile = hist2("HitAccetaphiTile");
        TH2* hist2_EnergyAccetaphiLAr = hist2("EnergyAccetaphiLAr");
        TH2* hist2_HitAccetaphiLAr = hist2("HitAccetaphiLAr");
        // Cells profile histograms
        TH1* hist_ECellsLRoI = hist("ECellsLRoI");
        TH1* hist_ECellsTRoI = hist("ECellsTRoI");
        TH1* hist_ECellsLRoI_QC65535 = hist("ECellsLRoI_QC65535");
        TH1* hist_ECellsTRoI_QC16 = hist("ECellsTRoI_QC16");
        TH1* hist_ECellsLRoI_QC4000 = hist("ECellsLRoI_QC4000");
        TH1* hist_ECellsTRoI_QC1 = hist("ECellsTRoI_QC1");
        TH1* hist_TimeCellsLRoI = hist("TimeCellsLRoI");
        TH1* hist_TimeCellsTRoI = hist("TimeCellsTRoI");
        TH1* hist_TimeCellsLRoI_EC1G = hist("TimeCellsLRoI_EC1G");
        TH1* hist_TimeCellsLRoI_EC10G = hist("TimeCellsLRoI_EC10G");
        TH1* hist_TimeCellsTRoI_EC1G = hist("TimeCellsTRoI_EC1G");
        TH1* hist_TimeCellsTRoI_EC10G = hist("TimeCellsTRoI_EC10G");
        TH1* hist_QCellsLRoI = hist("QCellsLRoI");
        TH1* hist_QCellsTRoI = hist("QCellsTRoI");
       
        // Cells energies for eta/phi
        TH2* hist_LArCellsEta = hist2("LArCellsEta");
        TH2* hist_LArCellsPhi = hist2("LArCellsPhi");
        TH2* hist_TileCellsEta = hist2("TileCellsEta");
        TH2* hist_TileCellsPhi = hist2("TileCellsPhi");


	CaloCellContainer::const_iterator tbeg = pCaloCellContainer->begin();
	CaloCellContainer::const_iterator tend = pCaloCellContainer->end();
	for(; tbeg!=tend ; ++tbeg){
		const CaloCell* tcell = (*tbeg);
		IdentifierHash tid = tcell->caloDDE()->calo_hash();
		float tenergy = tcell->energy();
		float ttime = tcell->time();
                if ( !tcell->caloDDE()->is_tile() )ttime/=1e3;
		CaloGain::CaloGain tgain = tcell->gain();
		const CaloCell* cell = AllCalo->findCell(tid);
		if ( cell && cell->caloDDE() ) {
			float energy = cell->energy();
			float time = cell->time();
			CaloGain::CaloGain gain = cell->gain();
			float diff = fabsf(tenergy-energy);
			float tdiff = fabsf(ttime-time);
			float diffp = 0;
			//float tdiffp = 0;
			double lartile = 0;
			if ( fabsf (energy) > 0.2 )
				diffp = 100*diff/energy;
			//if ( fabsf (time) > 0.2 )
			//	tdiffp = 100*tdiff/time;
			// time disabled for the moment
                        //


                        if ( cell->caloDDE()->is_tile() )hist_PercentDiffTile->Fill(diffp);
                        else hist_PercentDiffLAr->Fill(diffp);

			if ( ( ((diff>10) && (diffp>6)) || gain!= tgain || (tdiff>0.1 && !cell->caloDDE()->is_tile() ) ) ) {
                            if ( msgLvl(MSG::VERBOSE)) {
				msg() << MSG::VERBOSE;
				msg() << "Offline cell found";
                                msg() << " energy : " << cell->energy();
                                msg() << " tenergy : " << tcell->energy();
                                msg() << " eta : " << cell->eta();
                                msg() << " teta : " << tcell->eta();
                                msg() << " phi : " << cell->phi();
                                msg() << " tphi : " << tcell->phi();
                                msg() << " samp : " << cell->caloDDE()->getSampling();
                                msg() << " tsamp : " << tcell->caloDDE()->getSampling();
                                msg() << " time : " << cell->time();
                                msg() << " ttime : " << tcell->time();
                                msg() << " gain : " << cell->gain();
                                msg() << " tgain : " << tcell->gain();
                                msg() << " qua : " << cell->quality();
                                msg() << " tqua : " << tcell->quality();
			    }
				if ( cell->caloDDE()->is_tile() ){
                                  msg() << " TileID : " << m_tileID->to_string(cell->ID(),-2) << endmsg;
				 hist2_etaphiTile->Fill(cell->eta(),cell->phi());
				 //hist_PercentDiffTile->Fill(diffp); //moved up
				 lartile=1.0;
				 count_tcellspT++;
				}else{
                                 Identifier idh = cell->ID();
                                 HWIdentifier channel_hw = m_cablingSvc->createSignalChannelID(idh);
                                 HWIdentifier feb_hw = m_onlineHelper->feb_Id(channel_hw);
                                 if ( msgLvl(MSG::VERBOSE) )
                                   msg() << " febID : " << feb_hw;
				 hist2_etaphiLAr->Fill(cell->eta(),cell->phi());
				 //hist_PercentDiffLAr->Fill(diffp); //moved up
				 count_tcellspL++;
				}
                                if ( msgLvl(MSG::VERBOSE) )
                                msg() << endmsg;
				if ( m_ntuple ) 
				 ((TNtuple*)tree("Details"))->Fill(cell->et(),cell->eta(),cell->phi(),(double)cell->gain(),tcell->et(),tcell->eta(),tcell->phi(),(double)tcell->gain(),lartile ); 
			}
		} else { // did not find the cell. This is also an error
		   if ( tcell->caloDDE() && tcell->caloDDE()->is_tile() )
                        count_tcellspT++;
                   else
                        count_tcellspL++;
		}
                if ( tcell->caloDDE() && tcell->caloDDE()->is_tile() ){
                        hist2_EnergyAccetaphiTile->Fill(tcell->eta(), tcell->phi(), tcell->et() );
                        hist2_HitAccetaphiTile->Fill(tcell->eta(), tcell->phi() );
			hist_ECellsTRoI->Fill ( tcell->e()/1e3 );
			hist_TimeCellsTRoI->Fill ( tcell->time() );
			int quality = ((tcell->quality())&0xff00)>>16;
			quality += ((tcell->quality())&0xff);
			hist_QCellsTRoI->Fill ( quality );
			if ( quality < 16 ){
			  hist_ECellsTRoI_QC16->Fill ( tcell->e()/1e3 );
			  if ( quality < 1 )
			    hist_ECellsTRoI_QC1->Fill ( tcell->e()/1e3 );
			}
			hist_TimeCellsTRoI->Fill( tcell->time() );
			if ( tcell->e() > 1e3 ){
				hist_TimeCellsTRoI_EC1G->Fill( tcell->time() );
				if ( tcell->e() > 1e4 )
					hist_TimeCellsTRoI_EC10G->Fill( tcell->time() );
			}
			hist_TileCellsEta->Fill(tcell->eta(), tcell->et()/1e3 );
			hist_TileCellsPhi->Fill(tcell->phi(), tcell->et()/1e3 );
                        count_tcellsT++;
                }else{
                        hist2_EnergyAccetaphiLAr->Fill(tcell->eta(), tcell->phi(), tcell->et() );
                        hist2_HitAccetaphiLAr->Fill(tcell->eta(), tcell->phi() );
			hist_ECellsLRoI->Fill ( tcell->e()/1e3 );
			hist_TimeCellsLRoI->Fill ( tcell->time() );
			hist_QCellsLRoI->Fill ( (tcell->quality())&0xffff );
			if ( ((tcell->quality())&0xffff) < 0xffff ){
			  hist_ECellsLRoI_QC65535->Fill ( tcell->e()/1e3 );
			  if  ( ((tcell->quality())&0xffff) < 4000 )
                            hist_ECellsLRoI_QC4000->Fill ( tcell->e()/1e3 );
			}
			hist_TimeCellsLRoI->Fill( tcell->time() );
			if ( tcell->e() > 1e3 ){
				hist_TimeCellsLRoI_EC1G->Fill( tcell->time() );
				if ( tcell->e() > 1e4 )
					hist_TimeCellsLRoI_EC10G->Fill( tcell->time() );
			}
			hist_LArCellsEta->Fill(tcell->eta(), tcell->et()/1e3 );
			hist_LArCellsPhi->Fill(tcell->phi(), tcell->et()/1e3 );
                        count_tcellsL++;
                }
	}
	} // End of for loop on container keys
	hist("NBadCellsLAr")->Fill(count_tcellspL);
	hist("NCellsLAr")->Fill(count_tcellsL);
	hist("NBadCellsTile")->Fill(count_tcellspT);
	hist("NCellsTile")->Fill(count_tcellsT);
        ATH_MSG_DEBUG( "Number of LAr cells found " << count_tcellsL  );
        ATH_MSG_DEBUG( "Number of LAr cells w problems " << count_tcellspL  );
        ATH_MSG_DEBUG( "Number of Tile cells found " << count_tcellsT  );
        ATH_MSG_DEBUG( "Number of Tile cells w problems " << count_tcellspT  );

	return StatusCode::SUCCESS;


}

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTCaloToolL2::proc(){
#else
StatusCode HLTCaloToolL2::proc(bool /*endOfEventsBlock*/, bool /*endOfLumiBlock*/, bool /*endOfRun*/){
#endif

   return StatusCode::SUCCESS;

}


