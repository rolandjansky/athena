/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_Raw2Digit/ALFA_Raw2Digit.h"

#include "GaudiKernel/MsgStream.h"
#include <boost/lexical_cast.hpp>
#include "PathResolver/PathResolver.h"

#include <stdexcept>

ALFA_Raw2Digit::ALFA_Raw2Digit(const std::string& name, ISvcLocator* pSvcLocator) 
: AthAlgorithm(name, pSvcLocator)
{

	m_key_DigitCollection  = "ALFA_DigitCollection";
	m_key_ODDigitCollection  = "ALFA_ODDigitCollection";

	declareProperty("ALFA_RawDataCollectionKey", m_ALFA_RawDataCollectionKey = "ALFA_RawData");

	//allowed values (m_strMeasuredDataType="testbeam", "tunnel"), default value is "tunnel"
	//this property is needed due to different mapping in the testbeam and the tunnel data
	declareProperty("MeasuredDataType", m_strMeasuredDataType = "tunnel");


	m_All_Trigger_norm = NULL;
	m_All_Trigger = NULL;
	m_ODdigitCollection = NULL;
	m_ODdigitObject = NULL;
	m_digitCollection = NULL;
	m_digitObject = NULL;
//	m_histSvc = NULL;
	m_rootOutput = NULL;
	m_tree = NULL;

	m_nEvents = 0;
	m_mbID      = 0;
	m_pmfID     = 0;
	m_fibID     = 0;
	m_MAPMTChan = 0;
	m_MapChan   = 0;
	m_pot     = 0;
	m_side    = 0;
	m_plate   = 0;
	m_fiber   = 0;
	m_ODpot   = 0;
	m_ODside  = 0;
	m_ODplate = 0;
	m_ODfiber = 0;

	memset(&m_act_lay_h, 0, sizeof(m_act_lay_h));
	memset(&m_hit_lay_h, 0, sizeof(m_hit_lay_h));
	m_lumi_block     = 0;
	m_lumi_block_old = 0;
	m_event_no       = 0;
	m_WordId_count   = 0;
	m_chan_i         = 0;
	m_chan_j         = 0;
}

StatusCode ALFA_Raw2Digit::initialize()
{
	ATH_MSG_DEBUG ("ALFA_Raw2Digit:initialize()");

	if (m_strMeasuredDataType=="tunnel")
	{
		m_mb2det[0] = 1; m_mb2det[1] = 0; m_mb2det[2] = 7; m_mb2det[3] = 2;
		m_mb2det[4] = 4; m_mb2det[5] = 5; m_mb2det[6] = 6; m_mb2det[7] = 3;
	}
	else if (m_strMeasuredDataType=="testbeam")
	{
		m_mb2det[0] = 0; m_mb2det[1] = 1; m_mb2det[2] = 2; m_mb2det[3] = 3;
		m_mb2det[4] = 4; m_mb2det[5] = 5; m_mb2det[6] = 6; m_mb2det[7] = 7;
	}

	m_nEvents = 0;
	m_MAPMTChan = -1;

	for (int m=0;m<8;m++)
	{
		m_hit_counter[m] = 0;
		for (int k=0;k<23;k++)
		{
			for (int i=0;i<64;i++)
			{
				m_pmf_chan_hit_counter[m][k][i] = 0;
			}
		}
	}

	StatusCode sc = mapping();

	if (sc.isFailure()) { msg (MSG::FATAL) << " Could not open mapping " << endmsg; return sc; }
	else                  msg (MSG::DEBUG) << " mapping is fine " << endmsg;

//	sc = service("THistSvc", m_histSvc);

	// retrieve ThistSvc ...
//	if(sc.isFailure())
//	{
//		msg(MSG::FATAL) << "Unable to retrieve pointer to THistSvc" << endmsg;
//		return sc;
//	}

/*
	m_rootOutputFileName = "ALFA_toy_tree_tom.root";
	m_rootOutput = new TFile(m_rootOutputFileName,"RECREATE");
	m_tree = new TTree("MyTree", "ALFA_toy_tree");

	TBranch *branch;
	branch = m_tree->Branch("m_event_no", &m_event_no,"m_event_no/I"); branch->SetAutoDelete(kFALSE);
	branch = m_tree->Branch("m_lumi_block", &m_lumi_block, "m_lumi_block/I"); branch->SetAutoDelete(kFALSE);
	branch = m_tree->Branch("pot_b", m_pot_b, "pot_b[8]/O"); branch->SetAutoDelete(kFALSE);
	branch = m_tree->Branch("layer_b", m_layer_b, "layer_b[8][20]/O"); branch->SetAutoDelete(kFALSE);
	branch = m_tree->Branch("fiber_b", m_fiber_b, "fiber_b[8][20][64]/O"); branch->SetAutoDelete(kFALSE);
	branch = m_tree->Branch("trigger_pattern_b", m_trigger_pattern_b, "trigger_pattern[8][16]/O"); branch->SetAutoDelete(kFALSE);
	branch = m_tree->Branch("charge_1", &m_charge_1, "charge_1[8]/s"); branch->SetAutoDelete(kFALSE);
	branch = m_tree->Branch("charge_2", &m_charge_2, "charge_2[8]/s"); branch->SetAutoDelete(kFALSE);
*/

//	for (unsigned int j=0; j<8; j++)
//	{
// 		m_h_name1 = "MAPMT_activity_p_";
// 		m_h_name2 = "PMFvsMAPMT_p_";
// 		m_h_name3 = "PMFvsMAROC_p_";

// 		m_h_loc1 = "/AANT/MAPMT_activity_p_";
// 		m_h_loc2 = "/AANT/PMFvsMAPMT_p_";
// 		m_h_loc3 = "/AANT/PMFvsMAROC_p_";

// 		m_s.str("");
// 		m_s << j+1;

// 		m_h_name1 += m_s.str();
// 		m_h_name2 += m_s.str();
// 		m_h_name3 += m_s.str();

// 		m_h_loc1 += m_s.str();
// 		m_h_loc2 += m_s.str();
// 		m_h_loc3 += m_s.str();
     
/*  
		m_rootOutputFileName = "ALFA_toy_tree_tom.root";
		m_rootOutput = new TFile(m_rootOutputFileName,"RECREATE");
		m_tree = new TTree("MyTree", "ALFA_toy_tree");

		MAPMT_activity[j] = new TGraph(1472);
//		MAPMT_activity[j]->SetTitle("Trigger efficiency for Upper pot;MAROC Channel; Trigger efficiency");
		MAPMT_activity[j]->SetTitle(m_h_name1.c_str());
		MAPMT_activity[j]->SetMarkerStyle(6);
		MAPMT_activity[j]->Draw("AP");
		MAPMT_activity[j]->SetName(m_h_name1.c_str());

//		sc = m_histSvc->regHist(m_h_loc1.c_str(),MAPMT_activity[j]);

		PMFvsMAPMT[j]     = new TH2I(m_h_name2.c_str(),m_h_name2.c_str(),64,-0.5,63.5,23,-0.5,22.5);
		sc = m_histSvc->regHist(m_h_loc2.c_str(),PMFvsMAPMT[j]);

		PMFvsMAROC[j]     = new TH2I(m_h_name3.c_str(),m_h_name3.c_str(),64,-0.5,63.5,23,-0.5,22.5);
		sc = m_histSvc->regHist(m_h_loc3.c_str(),PMFvsMAROC[j]);

		for (unsigned int i=0; i<23; i++)
		{
			MapmtHit[j][i] = new TH2I(m_h_name1.c_str(),m_h_name1.c_str(),8,0.5,8.5,8,0.5,8.5);
			MAPMT_activity_h[j][i] = new TH2D("","",8,0.5,8.5,8,0.5,8.5);
			sc = m_histSvc->regHist(m_h_loc1.c_str(),MAPMT_activity_h[j][i]);
		}
*/
// 	}

	return StatusCode::SUCCESS;
}

StatusCode ALFA_Raw2Digit::execute()
{
	ATH_MSG_DEBUG ("ALFA_Raw2Digit: BEGINING of execute");

	m_nEvents++;
	msg(MSG::DEBUG) << "event # "<< m_nEvents << endmsg;
//	m_toFile << "Event " << m_nEvents << std::endl;

	// retrieve the RawData RDO container from the TDS
 	const ALFA_RawDataContainer* container = 0;
	StatusCode sc_read = evtStore()->retrieve(container, m_ALFA_RawDataCollectionKey);

	if ( sc_read != StatusCode::SUCCESS ) {
		msg(MSG::ERROR) << "ALFA_Raw2Digit: Could not find container" << endmsg;
		return StatusCode::SUCCESS;
	}

	else if ( sc_read == StatusCode::SUCCESS ) {
		msg(MSG::DEBUG)  << "ALFA_Raw2Digit: Raw data container retrieved" << endmsg;
	}

	msg(MSG::DEBUG) << "MD record_collection()" << endmsg;

	StatusCode sc = recordCollection();
	if  (sc.isFailure()) {
		msg(MSG::WARNING) << "ALFA_Raw2Digit: recordCollection failed" << endmsg;
		return StatusCode::SUCCESS;
	}

	msg(MSG::DEBUG) << "OD record_collection()" << endmsg;

	sc = recordODCollection();
	if  (sc.isFailure()) {
		msg(MSG::WARNING) << "ALFA_Raw2Digit, recordODCollection failed" << endmsg;
		return StatusCode::SUCCESS;
	}


	for (unsigned int i=0;i<8;i++) {
	 m_pot_b[i] = false;
	 for (unsigned int j=0;j<20;j++) {
		 m_layer_b[i][j] = false;
		 for (unsigned int k=0;k<64;k++) {
			 m_fiber_b[i][j][k] = false;
		 }
	 }
	 for (unsigned int k=0;k<16;k++) {
		 m_trigger_pattern_b[i][k] = false;
	 }
	}


	m_event_no = m_nEvents;


	m_lumi_block = container->GetLumiBlock();
//	std::cout << "m_lumi_block = " << m_lumi_block << std::endl;

	// loop over container start
	ALFA_RawDataContainer::const_iterator RawData_Collection_Beg = container->begin();
	ALFA_RawDataContainer::const_iterator RawData_Collection_End = container->end();

	//loop over collection (container) with hits (i.e. over 1 event)
	for(;RawData_Collection_Beg!=RawData_Collection_End; ++RawData_Collection_Beg)
	{
		m_mbID = ((*RawData_Collection_Beg)->GetMBId_POT());
		msg(MSG::DEBUG) << "ALFA_Raw2Digit: MBId POT= " << m_mbID <<  endmsg;

//		std::cout << "*******    Time stamp = " << (*RawData_Collection_Beg)->GetTimeStamp_POT() << std::endl;
//		time_t rawtime = (*RawData_Collection_Beg)->GetTimeStamp_POT();
//		struct tm * timeinfo;
////		time ( &rawtime );
////		timeinfo = localtime ( &rawtime );
//		timeinfo = gmtime ( &rawtime );
//		printf ( "The date/time is: %m_s", asctime (timeinfo) );

		m_pot_b[m_mbID-1] = true;
  //      m_event_no = ((*RawData_Collection_Beg)->GetEventCount_POT());

		m_charge_1[m_mbID-1] = ((*RawData_Collection_Beg)->Get_ADC1_POT());
		m_charge_2[m_mbID-1] = ((*RawData_Collection_Beg)->Get_ADC2_POT());
		for (unsigned int i=0;i<16;i++) {
	  m_trigger_pattern_b[m_mbID-1][i] = ((*RawData_Collection_Beg)->Get_pattern_POT())[i];
//	  msg(MSG::ERROR) << " pattern: bit " << i << ", value = "<< m_trigger_pattern_b[m_mbID-1][i] << " in MB = " << m_mbID-1 << endmsg;
		}

// loop over collection start
		ALFA_RawDataCollection::const_iterator p_RawData_Beg=(*RawData_Collection_Beg)->begin();//pointer to RawData data vector
		ALFA_RawDataCollection::const_iterator p_RawData_End=(*RawData_Collection_Beg)->end();

		for(;p_RawData_Beg!=p_RawData_End; ++p_RawData_Beg)
		{
//			msg(MSG::DEBUG) << " all, pmfId = " << (*p_RawData_Beg)->GetPMFId_PMF() <<  ", MBId = " << (*p_RawData_Beg)->GetMBId_PMF() << endmsg;

			if ((*p_RawData_Beg)->GetPMFId_PMF()>0 && ((*p_RawData_Beg)->GetWordId_PMF())<24)
			{
				m_pmfID= (*p_RawData_Beg)->GetPMFId_PMF();

				if (((*p_RawData_Beg)->GetWordId_PMF())==0) m_WordId_count=1;
				else  m_WordId_count=0;

				if (m_WordId_count==1){
					msg(MSG::DEBUG)  << " pmfId = " << (*p_RawData_Beg)->GetPMFId_PMF() <<  ", MBId = " << (*p_RawData_Beg)->GetMBId_PMF() <<endmsg;
				}

//				msg(MSG::DEBUG) << " m_WordId_count " << m_WordId_count << endmsg;

				for (unsigned int i=0;i<16;i++)
				{
					if ((*p_RawData_Beg)->Get_Chan(i)!=100)
					{
						msg(MSG::DEBUG)  <<"ALFA_Raw2Digit, maroc chanel # = " << (*p_RawData_Beg)->Get_Chan(i)<< endmsg;

						m_fibID = (*p_RawData_Beg)->Get_Chan(i);

						// in Sara Diglio Byte Stream Converter PMF are counted from 1, then OD detectors have values of PMF in the range 2-4; fibers start at 0
						if ((m_pmfID == 1) || (m_pmfID > 4))
						{
							m_pot = m_mb2det[m_mbID-1];
//							m_pot = m_mbID-1;
							m_plate = m_pmf2layer[m_pot][m_pmfID-1];
							// changed by Petr - 19.12.2012 -----------------------------
//							m_fiber = 63-m_maroc2fiber[m_pot][m_fibID];
							m_fiber = m_maroc2fiber[m_pot][m_plate][m_fibID];

							m_layer_b[m_mbID-1][m_plate] = true;
							m_fiber_b[m_mbID-1][m_plate][m_fiber] = true;
//							msg(MSG::ERROR) << " bim ho, lumi = " << m_lumi_block << ", pot = "<< m_mbID-1 << ", plate " << m_plate << endmsg;
							m_n_hits_lay[m_mbID-1][m_plate] = m_n_hits_lay[m_mbID-1][m_plate] + 1;
//							msg(MSG::ERROR) << " number of hits " << m_n_hits_lay[m_mbID-1][m_plate] << endmsg;

//							m_digitCollection->push_back(new ALFA_Digit(m_pot,m_plate,m_fiber));
							m_digitCollection->push_back(new ALFA_Digit(m_mbID-1,m_plate,m_fiber));

							// changed by Petr - 19.12.2012 -----------------------------
//							m_MapChan = m_maroc2mapmt[m_pot][m_fibID];
							m_MapChan = m_maroc2mapmt[m_pot][m_plate][m_fibID];
							msg(MSG::DEBUG)  << "pot = " << m_pot << ", pmf = " << m_pmfID-1 << ", maroc = " << m_fibID << ", mapmt = " << m_MapChan <<  endmsg;

//							PMFvsMAPMT[m_pot]->Fill(m_MapChan,m_pmfID-1);
//							PMFvsMAROC[m_pot]->Fill(m_fibID,m_pmfID-1);
							// changed by Petr - 19.12.2012 -----------------------------
//							ChanPlot(m_pot,m_fibID,m_chan_i,m_chan_j);
//							ChanPlot(m_pot,m_plate,m_fibID,m_chan_i,m_chan_j);
//							MapmtHit[m_pot][m_pmfID-1]->Fill(m_chan_i,m_chan_j);
							msg(MSG::DEBUG)  << "fibID = " << m_fibID << ", chan_i " << m_chan_i << ", chan_j = " << m_chan_j << endmsg;

							m_pmf_chan_hit_counter[m_pot][m_pmfID-1][m_fibID]++;
							m_hit_counter[m_pot]++;

							msg(MSG::DEBUG)  << "ALFA_Raw2Digit: new digit (softsim numbering-starting from 0) in the collection, pot = " << m_pot << ", plate = " << m_plate << ", fiber = " << m_fiber << endmsg;
						}
						else
						{
							m_ODpot = m_mb2det[m_mbID-1];
//							m_ODpot = m_mbID-1;
							m_ODside  = m_OD_pmf_maroc2side[m_ODpot][m_pmfID-1][m_fibID];
							m_ODplate = m_OD_pmf2layer[m_ODpot][m_pmfID-1];
							m_ODfiber = m_OD_pmf_maroc2fiber[m_ODpot][m_pmfID-1][m_fibID];

							// changed by Petr - 19.12.2012 ----------------------------- commented, is it needed by ODs? TODO: to correct m_MapChan for OD case
//							m_MapChan = m_maroc2mapmt[m_ODpot][m_fibID];
//							msg(MSG::DEBUG)  << " pot = " << m_ODpot << ", pmf = " << m_pmfID-1 << ", maroc = " << m_fibID << ", mapmt = " << m_MapChan << endmsg;

							if (m_ODfiber!=98)
							{
//								m_ODfiber = (m_ODside==0)? m_ODfiber : 29-m_ODfiber;

//								m_ODdigitCollection->push_back(new ALFA_ODDigit(m_ODpot,m_ODside,m_ODplate,m_ODfiber));
								m_ODdigitCollection->push_back(new ALFA_ODDigit(m_mbID-1,m_ODside,m_ODplate,m_ODfiber));

//								msg(MSG::DEBUG) << "pot, side, plate, fiber = " << m_ODpot << ", " << m_ODside << ", " << m_ODplate << ", " << m_ODfiber << endmsg;

							}


//							PMFvsMAPMT[m_ODpot]->Fill(m_MapChan,m_pmfID-1);
//							PMFvsMAROC[m_ODpot]->Fill(m_fibID,m_pmfID-1);

							// changed by Petr - 19.12.2012 ----------------------------- commented, is it needed by ODs? TODO: to correct ChanPlot for OD case
//							ChanPlot(m_ODpot,m_fibID,m_chan_i,m_chan_j);
//							MapmtHit[m_ODpot][m_pmfID-1]->Fill(m_chan_i,m_chan_j);
							msg(MSG::DEBUG)  << "fibID = " << m_fibID << ", chan_i " << m_chan_i << ", chan_j = " << m_chan_j << endmsg;

							m_pmf_chan_hit_counter[m_ODpot][m_pmfID-1][m_fibID]++;
							m_hit_counter[m_ODpot]++;

							msg(MSG::DEBUG)  << "ALFA_Raw2Digit:  new OD digit (softsim numbering-starting from 0) in the collection,  pot = " << m_ODpot << ", side = " << m_ODside << ", plate = " << m_ODplate << ", fiber = " << m_ODfiber << endmsg;
						}
					}
						// */
				}
			} //if (PFM!=0)
			else
			{
				msg(MSG::DEBUG) << "ALFA_Raw2Digit: More than n Raw Data " <<endmsg;
				break;
			}
		} // loop raw data
	
		msg(MSG::DEBUG)  << "ALFA_Raw2Digit: finish loop over raw data " <<   endmsg;
	} // loop collection

	msg(MSG::DEBUG)  << "ALFA_Raw2Digit: finish loop over collection " <<   endmsg;

//	m_tree->Fill();

	return StatusCode::SUCCESS;
}

StatusCode ALFA_Raw2Digit::finalize()
{
	ATH_MSG_DEBUG ("ALFA_Raw2Digit:finalize()");

//	m_toFile.close();
//	m_tree->Write();

/*      
	TCanvas *canvas = new TCanvas("MAPMT_activity", "MAPMT_activity");
//	TCanvas *tcDistance = new TCanvas("tcDistance", "tcDistance");

	gStyle->SetPalette(1);

	for (int j=0;j<8;j++){
		for (int k=0;k<23;k++){
			for (int m=1;m<9;m++){
				for (int n=1;n<9;n++){
					MAPMT_activity_h[j][k]->SetBinContent(m,n,double(MapmtHit[j][k]->GetBinContent(m,n))/double(m_hit_counter[j]));
				}
			}
			for (int i=0;i<64;i++){
				MAPMT_activity[j]->SetPoint(k*64+i,k*64+i,double(m_pmf_chan_hit_counter[j][k][i])/double(m_hit_counter[j]));
			}
		}

		m_s.str("");
		m_s << j+1;

		m_h_name1 = "MAPMT_activity_p_";
		m_h_name1 += m_s.str();

		canvas->Clear();
		canvas->cd();

		MAPMT_activity[j]->SetTitle(m_h_name1.c_str());
		MAPMT_activity[j]->SetMarkerStyle(6);
		MAPMT_activity[j]->Draw("AP");
		MAPMT_activity[j]->SetName(m_h_name1.c_str());

		canvas->cd()->Update();

		m_h_name1 += ".eps";
		canvas->Print(m_h_name1.c_str());
	}

	delete canvas;

	for (int j=0;j<8;j++){
		TCanvas *POT_View = new TCanvas("POT_View","POT_View");
		POT_View->Divide(5,5);

		gStyle->SetPalette(1);

		for (unsigned int i=0;i<4;i++)
		{
			POT_View->cd(i+1);
			//	MapmtEff[i]->GetZaxis()->SetRangeUser(0,max_trig_eff_old[0]);
			MAPMT_activity_h[j][i]->Draw("COLZ,A");
			MAPMT_activity_h[j][i]->SetStats(0);
		}
	
		for (unsigned int i=4;i<11;i++)
		{
			POT_View->cd(i+2);
			//	MapmtEff[i]->GetZaxis()->SetRangeUser(0,max_trig_eff_old[0]);
			MAPMT_activity_h[j][i]->Draw("COLZ,A");
			MAPMT_activity_h[j][i]->SetStats(0);
		}
	
		for (unsigned int i=11;i<23;i++)
		{
			POT_View->cd(i+3);
			//	MapmtEff[i]->GetZaxis()->SetRangeUser(0,max_trig_eff_old[0]);
			MAPMT_activity_h[j][i]->Draw("COLZ,A");
			MAPMT_activity_h[j][i]->SetStats(0);
		}
      
		POT_View->cd();

		m_s.str("");
		m_s << j+1;
		m_CanName = "h_MAPMT_activity_p_";
		m_CanName += m_s.str();
		m_CanName+=".eps";

		POT_View->Print(m_CanName.c_str());
		POT_View->Close();
	}
  
*/

//	m_rootOutput->Write();
//	m_rootOutput->Close();

	return StatusCode::SUCCESS;
}


StatusCode ALFA_Raw2Digit::recordCollection()
{
	msg(MSG::DEBUG) << " ALFA_Digitization::recordCollection " << endmsg;

	m_digitCollection = new ALFA_DigitCollection();

	StatusCode sc = evtStore()->record(m_digitCollection, m_key_DigitCollection);
	if (sc.isFailure()) { msg (MSG::FATAL) << " MD - Could not record the empty digit collection in StoreGate " << endmsg; return sc; }
	else                  msg (MSG::DEBUG) << " MD - Digit collection is recorded in StoreGate " << endmsg;

	return sc;
}

StatusCode ALFA_Raw2Digit::recordODCollection()
{
	msg(MSG::DEBUG) << " ALFA_Digitization::recordODCollection " << endmsg;

	m_ODdigitCollection = new ALFA_ODDigitCollection();

	StatusCode sc = evtStore()->record(m_ODdigitCollection, m_key_ODDigitCollection);
	if (sc.isFailure()) { msg (MSG::FATAL) << " OD - Could not record the empty digit collection in StoreGate " << endmsg; return sc; }
	else                  msg (MSG::DEBUG) << " OD - Digit collection is recorded in StoreGate " << endmsg;

	return sc;
}

StatusCode ALFA_Raw2Digit::mapping()
{
	int MarChan      = 0;
	int MapChan      = 0;
	int FibChan      = 0;
	int iLayer       = 0;
	int PMFNum       = 0;
	int LAYNum       = 0;
	int OD_PMFNum    = 0;
	int OD_Dieter    = 0;
	int OD_LAYNum    = 0;
	int OD_Side      = 0;
	int OD_MarChan   = 0;
	int OD_FibChan   = 0;
	int OD_MaPmtChan = 0;
   
//	int MBnum, DETnum;


//	m_mapname = "./mapping/motheboard2detector.dat";
//	m_inDet.open(m_mapname.c_str());
//	msg(MSG::DEBUG) << "file name " << m_mapname.c_str() << endmsg;
//	for (unsigned int i=0;i<64;i++) {
//		m_inDet >> MBnum >> DETnum;
//		m_mb2det[MBnum] = DETnum;
//	}

	std::fill_n(&m_maroc2fiber[0][0][0], sizeof(m_maroc2fiber)/sizeof(int), -9999.0);
	std::fill_n(&m_maroc2mapmt[0][0][0], sizeof(m_maroc2mapmt)/sizeof(int), -9999.0);
	std::fill_n(&m_mapmt2maroc[0][0][0], sizeof(m_mapmt2maroc)/sizeof(int), -9999.0);


	for (unsigned int j=0; j<8; j++)
	{
		m_s.str("");

//		m_mapname = "mapping/MAROC_MAPMT_FIBER_MD";

		// changed by Petr - 19.12.2012 -------------------------------------------------
//		m_mapname = "MAROC_MAPMT_FIBER_MD";
		m_mapname = "MAROC_MAPMT_FIBER_NEW_MD_";
		m_s << j+1;
		m_mapname += m_s.str();
		m_mapname += ".dat";
		
		msg(MSG::DEBUG) << "file name " << m_mapname.c_str() << endmsg;
		
		// ************
		std::string filePath = PathResolver::find_file(m_mapname.c_str(),"DATAPATH", PathResolver::RecursiveSearch);
		if(filePath.length() == 0)
		{
			msg(MSG::FATAL) << " the mapping file MD maroc-mapmt \"" <<  m_mapname.c_str() << "\" not found in Datapath." << endmsg;
			throw std::runtime_error("FATAL: mapping MD maroc-mapmt not found in Datapath.");
		}
		else
		{
			msg(MSG::DEBUG) << "DEBUG: the mapping file MD maroc-mapmt \"" <<  m_mapname.c_str() << "\" found in Datapath." << endmsg;	
			msg(MSG::DEBUG) << "DEBUG: filePath =  " << filePath.c_str() << endmsg;
		}
                // **************	
		
//		m_inChan.open(m_mapname.c_str());
		m_inChan.open(filePath.c_str());		
		if (m_inChan.is_open())
		{
			// changed by Petr - 19.12.2012 ---------------------------------------------
//			for (unsigned int i=0;i<64;i++){
			for (unsigned int i=0;i<1280;i++){
//				m_inChan >> MarChan >> MapChan >> FibChan;
				m_inChan >> iLayer >> MarChan >> MapChan >> FibChan;
//				msg(MSG::DEBUG) << i << " maroc_mapmt_fiber, maroc = " << MarChan << ", map = "<< MapChan << ", fib = " << FibChan << endmsg;
				msg(MSG::DEBUG) << i << " maroc_mapmt_fiber, layer = " << iLayer << ", maroc = " << MarChan << ", map = "<< MapChan << ", fib = " << FibChan << endmsg;
	
				if ((MarChan < 0) || (MarChan > 63))
				{
					std::string s1("MarChan out of range; file corrupted");
					throw std::runtime_error(s1);	
				}
				
				if ((MapChan < 0) || (MapChan > 63))
				{
					std::string s2("MapChan out of range; file corrupted");
					throw std::runtime_error(s2);	
				}			
				
				if ((iLayer < 0) || (iLayer > 19))
				{
					std::string s2("Layer out of range; file corrupted");
					throw std::runtime_error(s2);
				}

//				m_maroc2fiber[j][MarChan] = FibChan;
//				m_maroc2mapmt[j][MarChan] = MapChan;
//				m_mapmt2maroc[j][MapChan] = MarChan;
				m_maroc2fiber[j][iLayer][MarChan] = FibChan;
				m_maroc2mapmt[j][iLayer][MarChan] = MapChan;
				m_mapmt2maroc[j][iLayer][MapChan] = MarChan;
			}
			m_inChan.close();
		}
		else
		{
			msg(MSG::ERROR) << "the file " << m_mapname.c_str() << " was not open" << endmsg;
			return StatusCode::FAILURE;
		}
		
//		m_mapname = "mapping/PMF_LAYER_MD";
		m_mapname = "PMF_LAYER_MD";		
		m_mapname += m_s.str();
		m_mapname += ".dat";
		msg(MSG::DEBUG) << "file name " << m_mapname.c_str() << endmsg;
		
		// ************
		filePath = PathResolver::find_file(m_mapname.c_str(),"DATAPATH", PathResolver::RecursiveSearch);
		if(filePath.length() == 0)
		{
			msg(MSG::FATAL) << " the mapping file PMF_LAYER_MD \"" <<  m_mapname.c_str() << "\" not found in Datapath." << endmsg;
			throw std::runtime_error("FATAL: mapping PMF_LAYER_MD not found in Datapath.");
		}
		else
		{
			msg(MSG::DEBUG) << "DEBUG: the mapping file PMF_LAYER_MD \"" <<  m_mapname.c_str() << "\" found in Datapath." << endmsg;	
			msg(MSG::DEBUG) << "DEBUG: filePath =  " << filePath.c_str() << endmsg;
		}
                // **************

		m_inPMF.open(filePath.c_str());
		if (m_inPMF.is_open())
		{
			for (unsigned int i=0;i<20;i++)
			{
				m_inPMF >> PMFNum >> LAYNum;
				msg(MSG::DEBUG) << i << " pmf2layer, pmf = " << PMFNum  << ", layer = "<< LAYNum << endmsg;
				
				if ((PMFNum < 0) || ( PMFNum > 22))
				{
					std::string s3("PMFNum out of range; file corrupted");
					throw std::runtime_error(s3);	
				}
				
				
				m_pmf2layer[j][PMFNum] = LAYNum;
	//			//	 layer2pmf[j][LAYNum] = PMFNum;
			}
			m_inPMF.close();
		}
		else
		{
			msg(MSG::WARNING) << "the file " << m_mapname.c_str() << " was not open" << endmsg;
			return StatusCode::FAILURE;	
		}

//		m_mapname="mapping/OD_MAP";
		m_mapname="OD_MAP";		
		m_mapname += m_s.str();
		m_mapname+=".dat";
		msg(MSG::DEBUG) << "file name " << m_mapname.c_str() << endmsg;

		// ************
		filePath = PathResolver::find_file(m_mapname.c_str(),"DATAPATH", PathResolver::RecursiveSearch);
		if(filePath.length() == 0)
		{
			msg(MSG::FATAL) << " the mapping file OD_MAP \"" <<  m_mapname.c_str() << "\" not found in Datapath." << endmsg;
			throw std::runtime_error("FATAL: mapping OD_MAP not found in Datapath.");
		}
		else
		{
			msg(MSG::DEBUG) << "DEBUG: the mapping file OD_MAP \"" <<  m_mapname.c_str() << "\" found in Datapath." << endmsg;	
			msg(MSG::DEBUG) << "DEBUG: filePath =  " << filePath.c_str() << endmsg;
		}
                // **************
		
		m_inOD.open(filePath.c_str());
		if (m_inOD.is_open())
		{
			for(unsigned int jlay=0; jlay<3; jlay++)
			{
				for(unsigned int jside=0; jside<2; jside++)
				{
					for(unsigned int jch=0; jch<32; jch++)
					{
						m_inOD >> OD_PMFNum >> OD_Dieter >> OD_LAYNum >> OD_Side >> OD_MaPmtChan >> OD_FibChan; // warning!!! MaPmt, not Maroc!
						msg(MSG::DEBUG) << " pmf2layer, OD_PMFNum = " << OD_PMFNum  << ", OD_Dieter = "<< OD_Dieter << ", OD_LAYNum = " << OD_LAYNum << ", OD_Side = " << OD_Side << ", OD_MaPmtChan = " << OD_MaPmtChan << ", OD_FibChan = " << OD_FibChan << endmsg;
	
						if ((OD_PMFNum < 2) || ( OD_PMFNum > 4))
						{
							std::string s4("OD_PMFNum out of range; file corrupted");
							throw std::runtime_error(s4);	
						}
						
						if ((OD_MaPmtChan < 1) || ( OD_MaPmtChan > 64))
						{
							std::string s5("OD_MaPmtChan out of range; file corrupted");
							throw std::runtime_error(s5);	
						}
						
						
// 						m_OD_pmf2layer[j][OD_PMFNum-1] = OD_LAYNum-1;
	//					OD_layer2pmf[j][OD_LAYNum-1] = OD_PMFNum-1;
						// changed by Petr - 19.12.2012 ---------------------------------
//						OD_MarChan = m_mapmt2maroc[j][OD_MaPmtChan-1];
// 						OD_MarChan = m_mapmt2maroc[j][OD_LAYNum-1][OD_MaPmtChan-1];
						//added to solve ATLAS coverity 13339 (only three OD layers are allowed)
						if ((OD_LAYNum > 0) && ( OD_LAYNum < 4))
						{
							m_OD_pmf2layer[j][OD_PMFNum-1] = OD_LAYNum-1;
							OD_MarChan = m_mapmt2maroc[j][OD_LAYNum-1][OD_MaPmtChan-1];
						}
						else
						{
							msg(MSG::DEBUG) << "OD_LAYNum out of bounds" << endmsg;
						}

						m_OD_pmf_maroc2fiber[j][OD_PMFNum-1][OD_MarChan] = OD_FibChan-1;
						m_OD_pmf_maroc2side[j][OD_PMFNum-1][OD_MarChan] = OD_Side;
					}
				}
			}
	
			m_inOD.close();
		}
		else
		{
			msg(MSG::WARNING) << "the file " << m_mapname.c_str() << " was not open" << endmsg;
			return StatusCode::FAILURE;
		}
	}

	return StatusCode::SUCCESS;
}

// changed by Petr - 19.12.2012 ---------------------------------------------------------
//StatusCode ALFA_Raw2Digit::ChanPlot(uint32_t pot_n, uint32_t MarChan, uint32_t &m_chan_i, uint32_t &m_chan_j)
StatusCode ALFA_Raw2Digit::ChanPlot(uint32_t pot_n, uint32_t iLayer, uint32_t MarChan, uint32_t &chan_i, uint32_t &chan_j)
{
	// returs values in range 1-8; input is 0-63!
//	if (int(fmod(double(m_maroc2mapmt[pot_n][MarChan]+1),double(8)))>0)
	if (int(fmod(double(m_maroc2mapmt[pot_n][iLayer][MarChan]+1),double(8)))>0)
	{
//		chan_i=int((m_maroc2mapmt[pot_n][MarChan]+1)/8)+1;
//		chan_j=9-int(fmod(double(m_maroc2mapmt[pot_n][MarChan]+1),double(8)));
		chan_i=int((m_maroc2mapmt[pot_n][iLayer][MarChan]+1)/8)+1;
		chan_j=9-int(fmod(double(m_maroc2mapmt[pot_n][iLayer][MarChan]+1),double(8)));
	}
	else
	{
//		chan_i=int((m_maroc2mapmt[pot_n][MarChan]+1)/8);
		chan_i=int((m_maroc2mapmt[pot_n][iLayer][MarChan]+1)/8);
		chan_j=1;
	}

//	msg(MSG::FATAL)  << "in funct: m_fibID = " << MarChan << ", chan_i " << chan_i << ", chan_j = " << chan_j <<  endmsg;
	return StatusCode::SUCCESS;
}
