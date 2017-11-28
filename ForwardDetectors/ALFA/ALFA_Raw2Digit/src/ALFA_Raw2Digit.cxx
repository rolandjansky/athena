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


	All_Trigger_norm = NULL;
	All_Trigger = NULL;
	m_ODdigitCollection = NULL;
	m_ODdigitObject = NULL;
	m_digitCollection = NULL;
	m_digitObject = NULL;
//	m_histSvc = NULL;
	m_rootOutput = NULL;
	m_tree = NULL;

	m_nEvents = 0;
	mbID      = 0;
	pmfID     = 0;
	fibID     = 0;
	MAPMTChan = 0;
	MapChan   = 0;
	m_pot     = 0;
	m_side    = 0;
	m_plate   = 0;
	m_fiber   = 0;
	m_ODpot   = 0;
	m_ODside  = 0;
	m_ODplate = 0;
	m_ODfiber = 0;

	memset(&act_lay_h, 0, sizeof(act_lay_h));
	memset(&hit_lay_h, 0, sizeof(hit_lay_h));
	lumi_block     = 0;
	lumi_block_old = 0;
	event_no       = 0;
	WordId_count   = 0;
	chan_i         = 0;
	chan_j         = 0;
}

StatusCode ALFA_Raw2Digit::initialize()
{
	ATH_MSG_DEBUG ("ALFA_Raw2Digit:initialize()");

	if (m_strMeasuredDataType=="tunnel")
	{
		mb2det[0] = 1; mb2det[1] = 0; mb2det[2] = 7; mb2det[3] = 2;
		mb2det[4] = 4; mb2det[5] = 5; mb2det[6] = 6; mb2det[7] = 3;
	}
	else if (m_strMeasuredDataType=="testbeam")
	{
		mb2det[0] = 0; mb2det[1] = 1; mb2det[2] = 2; mb2det[3] = 3;
		mb2det[4] = 4; mb2det[5] = 5; mb2det[6] = 6; mb2det[7] = 7;
	}

	m_nEvents = 0;
	MAPMTChan = -1;

	for (int m=0;m<8;m++)
	{
		hit_counter[m] = 0;
		for (int k=0;k<23;k++)
		{
			for (int i=0;i<64;i++)
			{
				pmf_chan_hit_counter[m][k][i] = 0;
			}
		}
	}

	StatusCode sc = mapping();

	if (sc.isFailure()) { msg (MSG::FATAL) << " Could not open mapping " << endreq; return sc; }
	else                  msg (MSG::DEBUG) << " mapping is fine " << endreq;

//	sc = service("THistSvc", m_histSvc);

	// retrieve ThistSvc ...
//	if(sc.isFailure())
//	{
//		msg(MSG::FATAL) << "Unable to retrieve pointer to THistSvc" << endreq;
//		return sc;
//	}

/*
	m_rootOutputFileName = "ALFA_toy_tree_tom.root";
	m_rootOutput = new TFile(m_rootOutputFileName,"RECREATE");
	m_tree = new TTree("MyTree", "ALFA_toy_tree");

	TBranch *branch;
	branch = m_tree->Branch("event_no", &event_no,"event_no/I"); branch->SetAutoDelete(kFALSE);
	branch = m_tree->Branch("lumi_block", &lumi_block, "lumi_block/I"); branch->SetAutoDelete(kFALSE);
	branch = m_tree->Branch("pot_b", pot_b, "pot_b[8]/O"); branch->SetAutoDelete(kFALSE);
	branch = m_tree->Branch("layer_b", layer_b, "layer_b[8][20]/O"); branch->SetAutoDelete(kFALSE);
	branch = m_tree->Branch("fiber_b", fiber_b, "fiber_b[8][20][64]/O"); branch->SetAutoDelete(kFALSE);
	branch = m_tree->Branch("trigger_pattern_b", trigger_pattern_b, "trigger_pattern[8][16]/O"); branch->SetAutoDelete(kFALSE);
	branch = m_tree->Branch("charge_1", &charge_1, "charge_1[8]/s"); branch->SetAutoDelete(kFALSE);
	branch = m_tree->Branch("charge_2", &charge_2, "charge_2[8]/s"); branch->SetAutoDelete(kFALSE);
*/

//	for (unsigned int j=0; j<8; j++)
//	{
// 		h_name1 = "MAPMT_activity_p_";
// 		h_name2 = "PMFvsMAPMT_p_";
// 		h_name3 = "PMFvsMAROC_p_";

// 		h_loc1 = "/AANT/MAPMT_activity_p_";
// 		h_loc2 = "/AANT/PMFvsMAPMT_p_";
// 		h_loc3 = "/AANT/PMFvsMAROC_p_";

// 		s.str("");
// 		s << j+1;

// 		h_name1 += s.str();
// 		h_name2 += s.str();
// 		h_name3 += s.str();

// 		h_loc1 += s.str();
// 		h_loc2 += s.str();
// 		h_loc3 += s.str();
     
/*  
		m_rootOutputFileName = "ALFA_toy_tree_tom.root";
		m_rootOutput = new TFile(m_rootOutputFileName,"RECREATE");
		m_tree = new TTree("MyTree", "ALFA_toy_tree");

		MAPMT_activity[j] = new TGraph(1472);
//		MAPMT_activity[j]->SetTitle("Trigger efficiency for Upper pot;MAROC Channel; Trigger efficiency");
		MAPMT_activity[j]->SetTitle(h_name1.c_str());
		MAPMT_activity[j]->SetMarkerStyle(6);
		MAPMT_activity[j]->Draw("AP");
		MAPMT_activity[j]->SetName(h_name1.c_str());

//		sc = m_histSvc->regHist(h_loc1.c_str(),MAPMT_activity[j]);

		PMFvsMAPMT[j]     = new TH2I(h_name2.c_str(),h_name2.c_str(),64,-0.5,63.5,23,-0.5,22.5);
		sc = m_histSvc->regHist(h_loc2.c_str(),PMFvsMAPMT[j]);

		PMFvsMAROC[j]     = new TH2I(h_name3.c_str(),h_name3.c_str(),64,-0.5,63.5,23,-0.5,22.5);
		sc = m_histSvc->regHist(h_loc3.c_str(),PMFvsMAROC[j]);

		for (unsigned int i=0; i<23; i++)
		{
			MapmtHit[j][i] = new TH2I(h_name1.c_str(),h_name1.c_str(),8,0.5,8.5,8,0.5,8.5);
			MAPMT_activity_h[j][i] = new TH2D("","",8,0.5,8.5,8,0.5,8.5);
			sc = m_histSvc->regHist(h_loc1.c_str(),MAPMT_activity_h[j][i]);
		}
*/
// 	}

	return StatusCode::SUCCESS;
}

StatusCode ALFA_Raw2Digit::execute()
{
	ATH_MSG_DEBUG ("ALFA_Raw2Digit: BEGINING of execute");

	m_nEvents++;
	msg(MSG::DEBUG) << "event # "<< m_nEvents << endreq;
//	m_toFile << "Event " << m_nEvents << std::endl;

	// retrieve the RawData RDO container from the TDS
 	const ALFA_RawDataContainer* container = 0;
	StatusCode sc_read = evtStore()->retrieve(container, m_ALFA_RawDataCollectionKey);

	if ( sc_read != StatusCode::SUCCESS ) {
		msg(MSG::ERROR) << "ALFA_Raw2Digit: Could not find container" << endreq;
		return StatusCode::SUCCESS;
	}

	else if ( sc_read == StatusCode::SUCCESS ) {
		msg(MSG::DEBUG)  << "ALFA_Raw2Digit: Raw data container retrieved" << endreq;
	}

	msg(MSG::DEBUG) << "MD record_collection()" << endreq;

	StatusCode sc = recordCollection();
	if  (sc.isFailure()) {
		msg(MSG::WARNING) << "ALFA_Raw2Digit: recordCollection failed" << endreq;
		return StatusCode::SUCCESS;
	}

	msg(MSG::DEBUG) << "OD record_collection()" << endreq;

	sc = recordODCollection();
	if  (sc.isFailure()) {
		msg(MSG::WARNING) << "ALFA_Raw2Digit, recordODCollection failed" << endreq;
		return StatusCode::SUCCESS;
	}


	for (unsigned int i=0;i<8;i++) {
	 pot_b[i] = false;
	 for (unsigned int j=0;j<20;j++) {
		 layer_b[i][j] = false;
		 for (unsigned int k=0;k<64;k++) {
			 fiber_b[i][j][k] = false;
		 }
	 }
	 for (unsigned int k=0;k<16;k++) {
		 trigger_pattern_b[i][k] = false;
	 }
	}


	event_no = m_nEvents;


	lumi_block = container->GetLumiBlock();
//	std::cout << "lumi_block = " << lumi_block << std::endl;

	// loop over container start
	ALFA_RawDataContainer::const_iterator RawData_Collection_Beg = container->begin();
	ALFA_RawDataContainer::const_iterator RawData_Collection_End = container->end();

	//loop over collection (container) with hits (i.e. over 1 event)
	for(;RawData_Collection_Beg!=RawData_Collection_End; ++RawData_Collection_Beg)
	{
		mbID = ((*RawData_Collection_Beg)->GetMBId_POT());
		msg(MSG::DEBUG) << "ALFA_Raw2Digit: MBId POT= " << mbID <<  endreq;

//		std::cout << "*******    Time stamp = " << (*RawData_Collection_Beg)->GetTimeStamp_POT() << std::endl;
//		time_t rawtime = (*RawData_Collection_Beg)->GetTimeStamp_POT();
//		struct tm * timeinfo;
////		time ( &rawtime );
////		timeinfo = localtime ( &rawtime );
//		timeinfo = gmtime ( &rawtime );
//		printf ( "The date/time is: %s", asctime (timeinfo) );

		pot_b[mbID-1] = true;
  //      event_no = ((*RawData_Collection_Beg)->GetEventCount_POT());

		charge_1[mbID-1] = ((*RawData_Collection_Beg)->Get_ADC1_POT());
		charge_2[mbID-1] = ((*RawData_Collection_Beg)->Get_ADC2_POT());

		const std::vector<bool>& patternPot = (*RawData_Collection_Beg)->Get_pattern_POT();
		for (unsigned int i=0; i<patternPot.size(); i++) {
		  trigger_pattern_b[mbID-1][i] = patternPot[i];
//	  msg(MSG::ERROR) << " pattern: bit " << i << ", value = "<< trigger_pattern_b[mbID-1][i] << " in MB = " << mbID-1 << endreq;
		}

// loop over collection start
		ALFA_RawDataCollection::const_iterator p_RawData_Beg=(*RawData_Collection_Beg)->begin();//pointer to RawData data vector
		ALFA_RawDataCollection::const_iterator p_RawData_End=(*RawData_Collection_Beg)->end();

		for(;p_RawData_Beg!=p_RawData_End; ++p_RawData_Beg)
		{
//			msg(MSG::DEBUG) << " all, pmfId = " << (*p_RawData_Beg)->GetPMFId_PMF() <<  ", MBId = " << (*p_RawData_Beg)->GetMBId_PMF() << endreq;

			if ((*p_RawData_Beg)->GetPMFId_PMF()>0 && ((*p_RawData_Beg)->GetWordId_PMF())<24)
			{
				pmfID= (*p_RawData_Beg)->GetPMFId_PMF();

				if (((*p_RawData_Beg)->GetWordId_PMF())==0) WordId_count=1;
				else  WordId_count=0;

				if (WordId_count==1){
					msg(MSG::DEBUG)  << " pmfId = " << (*p_RawData_Beg)->GetPMFId_PMF() <<  ", MBId = " << (*p_RawData_Beg)->GetMBId_PMF() <<endreq;
				}

//				msg(MSG::DEBUG) << " WordId_count " << WordId_count << endreq;

				for (unsigned int i=0;i<16;i++)
				{
					if ((*p_RawData_Beg)->Get_Chan(i)!=100)
					{
						msg(MSG::DEBUG)  <<"ALFA_Raw2Digit, maroc chanel # = " << (*p_RawData_Beg)->Get_Chan(i)<< endreq;

						fibID = (*p_RawData_Beg)->Get_Chan(i);

						// in Sara Diglio Byte Stream Converter PMF are counted from 1, then OD detectors have values of PMF in the range 2-4; fibers start at 0
						if ((pmfID == 1) || (pmfID > 4))
						{
							m_pot = mb2det[mbID-1];
//							m_pot = mbID-1;
							m_plate = pmf2layer[m_pot][pmfID-1];
							// changed by Petr - 19.12.2012 -----------------------------
//							m_fiber = 63-maroc2fiber[m_pot][fibID];
							m_fiber = maroc2fiber[m_pot][m_plate][fibID];

							layer_b[mbID-1][m_plate] = true;
							fiber_b[mbID-1][m_plate][m_fiber] = true;
//							msg(MSG::ERROR) << " bim ho, lumi = " << lumi_block << ", pot = "<< mbID-1 << ", plate " << m_plate << endreq;
							n_hits_lay[mbID-1][m_plate] = n_hits_lay[mbID-1][m_plate] + 1;
//							msg(MSG::ERROR) << " number of hits " << n_hits_lay[mbID-1][m_plate] << endreq;

//							m_digitCollection->push_back(new ALFA_Digit(m_pot,m_plate,m_fiber));
							m_digitCollection->push_back(new ALFA_Digit(mbID-1,m_plate,m_fiber));

							// changed by Petr - 19.12.2012 -----------------------------
//							MapChan = maroc2mapmt[m_pot][fibID];
							MapChan = maroc2mapmt[m_pot][m_plate][fibID];
							msg(MSG::DEBUG)  << "pot = " << m_pot << ", pmf = " << pmfID-1 << ", maroc = " << fibID << ", mapmt = " << MapChan <<  endreq;

//							PMFvsMAPMT[m_pot]->Fill(MapChan,pmfID-1);
//							PMFvsMAROC[m_pot]->Fill(fibID,pmfID-1);
							// changed by Petr - 19.12.2012 -----------------------------
//							ChanPlot(m_pot,fibID,chan_i,chan_j);
//							ChanPlot(m_pot,m_plate,fibID,chan_i,chan_j);
//							MapmtHit[m_pot][pmfID-1]->Fill(chan_i,chan_j);
							msg(MSG::DEBUG)  << "fibID = " << fibID << ", chan_i " << chan_i << ", chan_j = " << chan_j << endreq;

							pmf_chan_hit_counter[m_pot][pmfID-1][fibID]++;
							hit_counter[m_pot]++;

							msg(MSG::DEBUG)  << "ALFA_Raw2Digit: new digit (softsim numbering-starting from 0) in the collection, pot = " << m_pot << ", plate = " << m_plate << ", fiber = " << m_fiber << endreq;
						}
						else
						{
							m_ODpot = mb2det[mbID-1];
//							m_ODpot = mbID-1;
							m_ODside  = OD_pmf_maroc2side[m_ODpot][pmfID-1][fibID];
							m_ODplate = OD_pmf2layer[m_ODpot][pmfID-1];
							m_ODfiber = OD_pmf_maroc2fiber[m_ODpot][pmfID-1][fibID];

							// changed by Petr - 19.12.2012 ----------------------------- commented, is it needed by ODs? TODO: to correct MapChan for OD case
//							MapChan = maroc2mapmt[m_ODpot][fibID];
//							msg(MSG::DEBUG)  << " pot = " << m_ODpot << ", pmf = " << pmfID-1 << ", maroc = " << fibID << ", mapmt = " << MapChan << endreq;

							if (m_ODfiber!=98)
							{
//								m_ODfiber = (m_ODside==0)? m_ODfiber : 29-m_ODfiber;

//								m_ODdigitCollection->push_back(new ALFA_ODDigit(m_ODpot,m_ODside,m_ODplate,m_ODfiber));
								m_ODdigitCollection->push_back(new ALFA_ODDigit(mbID-1,m_ODside,m_ODplate,m_ODfiber));

//								msg(MSG::DEBUG) << "pot, side, plate, fiber = " << m_ODpot << ", " << m_ODside << ", " << m_ODplate << ", " << m_ODfiber << endreq;

							}


//							PMFvsMAPMT[m_ODpot]->Fill(MapChan,pmfID-1);
//							PMFvsMAROC[m_ODpot]->Fill(fibID,pmfID-1);

							// changed by Petr - 19.12.2012 ----------------------------- commented, is it needed by ODs? TODO: to correct ChanPlot for OD case
//							ChanPlot(m_ODpot,fibID,chan_i,chan_j);
//							MapmtHit[m_ODpot][pmfID-1]->Fill(chan_i,chan_j);
							msg(MSG::DEBUG)  << "fibID = " << fibID << ", chan_i " << chan_i << ", chan_j = " << chan_j << endreq;

							pmf_chan_hit_counter[m_ODpot][pmfID-1][fibID]++;
							hit_counter[m_ODpot]++;

							msg(MSG::DEBUG)  << "ALFA_Raw2Digit:  new OD digit (softsim numbering-starting from 0) in the collection,  pot = " << m_ODpot << ", side = " << m_ODside << ", plate = " << m_ODplate << ", fiber = " << m_ODfiber << endreq;
						}
					}
						// */
				}
			} //if (PFM!=0)
			else
			{
				msg(MSG::DEBUG) << "ALFA_Raw2Digit: More than n Raw Data " <<endreq;
				break;
			}
		} // loop raw data
	
		msg(MSG::DEBUG)  << "ALFA_Raw2Digit: finish loop over raw data " <<   endreq;
	} // loop collection

	msg(MSG::DEBUG)  << "ALFA_Raw2Digit: finish loop over collection " <<   endreq;

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
					MAPMT_activity_h[j][k]->SetBinContent(m,n,double(MapmtHit[j][k]->GetBinContent(m,n))/double(hit_counter[j]));
				}
			}
			for (int i=0;i<64;i++){
				MAPMT_activity[j]->SetPoint(k*64+i,k*64+i,double(pmf_chan_hit_counter[j][k][i])/double(hit_counter[j]));
			}
		}

		s.str("");
		s << j+1;

		h_name1 = "MAPMT_activity_p_";
		h_name1 += s.str();

		canvas->Clear();
		canvas->cd();

		MAPMT_activity[j]->SetTitle(h_name1.c_str());
		MAPMT_activity[j]->SetMarkerStyle(6);
		MAPMT_activity[j]->Draw("AP");
		MAPMT_activity[j]->SetName(h_name1.c_str());

		canvas->cd()->Update();

		h_name1 += ".eps";
		canvas->Print(h_name1.c_str());
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

		s.str("");
		s << j+1;
		CanName = "h_MAPMT_activity_p_";
		CanName += s.str();
		CanName+=".eps";

		POT_View->Print(CanName.c_str());
		POT_View->Close();
	}
  
*/

//	m_rootOutput->Write();
//	m_rootOutput->Close();

	return StatusCode::SUCCESS;
}


StatusCode ALFA_Raw2Digit::recordCollection()
{
	msg(MSG::DEBUG) << " ALFA_Digitization::recordCollection " << endreq;

	m_digitCollection = new ALFA_DigitCollection();

	StatusCode sc = evtStore()->record(m_digitCollection, m_key_DigitCollection);
	if (sc.isFailure()) { msg (MSG::FATAL) << " MD - Could not record the empty digit collection in StoreGate " << endreq; return sc; }
	else                  msg (MSG::DEBUG) << " MD - Digit collection is recorded in StoreGate " << endreq;

	return sc;
}

StatusCode ALFA_Raw2Digit::recordODCollection()
{
	msg(MSG::DEBUG) << " ALFA_Digitization::recordODCollection " << endreq;

	m_ODdigitCollection = new ALFA_ODDigitCollection();

	StatusCode sc = evtStore()->record(m_ODdigitCollection, m_key_ODDigitCollection);
	if (sc.isFailure()) { msg (MSG::FATAL) << " OD - Could not record the empty digit collection in StoreGate " << endreq; return sc; }
	else                  msg (MSG::DEBUG) << " OD - Digit collection is recorded in StoreGate " << endreq;

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


//	mapname = "./mapping/motheboard2detector.dat";
//	inDet.open(mapname.c_str());
//	msg(MSG::DEBUG) << "file name " << mapname.c_str() << endreq;
//	for (unsigned int i=0;i<64;i++) {
//		inDet >> MBnum >> DETnum;
//		mb2det[MBnum] = DETnum;
//	}

	std::fill_n(&maroc2fiber[0][0][0], sizeof(maroc2fiber)/sizeof(int), -9999.0);
	std::fill_n(&maroc2mapmt[0][0][0], sizeof(maroc2mapmt)/sizeof(int), -9999.0);
	std::fill_n(&mapmt2maroc[0][0][0], sizeof(mapmt2maroc)/sizeof(int), -9999.0);


	for (unsigned int j=0; j<8; j++)
	{
		s.str("");

//		mapname = "mapping/MAROC_MAPMT_FIBER_MD";

		// changed by Petr - 19.12.2012 -------------------------------------------------
//		mapname = "MAROC_MAPMT_FIBER_MD";
		mapname = "MAROC_MAPMT_FIBER_NEW_MD_";
		s << j+1;
		mapname += s.str();
		mapname += ".dat";
		
		msg(MSG::DEBUG) << "file name " << mapname.c_str() << endreq;
		
		// ************
		std::string filePath = PathResolver::find_file(mapname.c_str(),"DATAPATH", PathResolver::RecursiveSearch);
		if(filePath.length() == 0)
		{
			msg(MSG::FATAL) << " the mapping file MD maroc-mapmt \"" <<  mapname.c_str() << "\" not found in Datapath." << endreq;
			throw std::runtime_error("FATAL: mapping MD maroc-mapmt not found in Datapath.");
		}
		else
		{
			msg(MSG::DEBUG) << "DEBUG: the mapping file MD maroc-mapmt \"" <<  mapname.c_str() << "\" found in Datapath." << endreq;	
			msg(MSG::DEBUG) << "DEBUG: filePath =  " << filePath.c_str() << endreq;
		}
                // **************	
		
//		inChan.open(mapname.c_str());
		inChan.open(filePath.c_str());		
		if (inChan.is_open())
		{
			// changed by Petr - 19.12.2012 ---------------------------------------------
//			for (unsigned int i=0;i<64;i++){
			for (unsigned int i=0;i<1280;i++){
//				inChan >> MarChan >> MapChan >> FibChan;
				inChan >> iLayer >> MarChan >> MapChan >> FibChan;
//				msg(MSG::DEBUG) << i << " maroc_mapmt_fiber, maroc = " << MarChan << ", map = "<< MapChan << ", fib = " << FibChan << endreq;
				msg(MSG::DEBUG) << i << " maroc_mapmt_fiber, layer = " << iLayer << ", maroc = " << MarChan << ", map = "<< MapChan << ", fib = " << FibChan << endreq;
	
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

//				maroc2fiber[j][MarChan] = FibChan;
//				maroc2mapmt[j][MarChan] = MapChan;
//				mapmt2maroc[j][MapChan] = MarChan;
				maroc2fiber[j][iLayer][MarChan] = FibChan;
				maroc2mapmt[j][iLayer][MarChan] = MapChan;
				mapmt2maroc[j][iLayer][MapChan] = MarChan;
			}
			inChan.close();
		}
		else
		{
			msg(MSG::ERROR) << "the file " << mapname.c_str() << " was not open" << endreq;
			return StatusCode::FAILURE;
		}
		
//		mapname = "mapping/PMF_LAYER_MD";
		mapname = "PMF_LAYER_MD";		
		mapname += s.str();
		mapname += ".dat";
		msg(MSG::DEBUG) << "file name " << mapname.c_str() << endreq;
		
		// ************
		filePath = PathResolver::find_file(mapname.c_str(),"DATAPATH", PathResolver::RecursiveSearch);
		if(filePath.length() == 0)
		{
			msg(MSG::FATAL) << " the mapping file PMF_LAYER_MD \"" <<  mapname.c_str() << "\" not found in Datapath." << endreq;
			throw std::runtime_error("FATAL: mapping PMF_LAYER_MD not found in Datapath.");
		}
		else
		{
			msg(MSG::DEBUG) << "DEBUG: the mapping file PMF_LAYER_MD \"" <<  mapname.c_str() << "\" found in Datapath." << endreq;	
			msg(MSG::DEBUG) << "DEBUG: filePath =  " << filePath.c_str() << endreq;
		}
                // **************

		inPMF.open(filePath.c_str());
		if (inPMF.is_open())
		{
			for (unsigned int i=0;i<20;i++)
			{
				inPMF >> PMFNum >> LAYNum;
				msg(MSG::DEBUG) << i << " pmf2layer, pmf = " << PMFNum  << ", layer = "<< LAYNum << endreq;
				
				if ((PMFNum < 0) || ( PMFNum > 22))
				{
					std::string s3("PMFNum out of range; file corrupted");
					throw std::runtime_error(s3);	
				}
				
				
				pmf2layer[j][PMFNum] = LAYNum;
	//			//	 layer2pmf[j][LAYNum] = PMFNum;
			}
			inPMF.close();
		}
		else
		{
			msg(MSG::WARNING) << "the file " << mapname.c_str() << " was not open" << endreq;
			return StatusCode::FAILURE;	
		}

//		mapname="mapping/OD_MAP";
		mapname="OD_MAP";		
		mapname += s.str();
		mapname+=".dat";
		msg(MSG::DEBUG) << "file name " << mapname.c_str() << endreq;

		// ************
		filePath = PathResolver::find_file(mapname.c_str(),"DATAPATH", PathResolver::RecursiveSearch);
		if(filePath.length() == 0)
		{
			msg(MSG::FATAL) << " the mapping file OD_MAP \"" <<  mapname.c_str() << "\" not found in Datapath." << endreq;
			throw std::runtime_error("FATAL: mapping OD_MAP not found in Datapath.");
		}
		else
		{
			msg(MSG::DEBUG) << "DEBUG: the mapping file OD_MAP \"" <<  mapname.c_str() << "\" found in Datapath." << endreq;	
			msg(MSG::DEBUG) << "DEBUG: filePath =  " << filePath.c_str() << endreq;
		}
                // **************
		
		inOD.open(filePath.c_str());
		if (inOD.is_open())
		{
			for(unsigned int jlay=0; jlay<3; jlay++)
			{
				for(unsigned int jside=0; jside<2; jside++)
				{
					for(unsigned int jch=0; jch<32; jch++)
					{
						inOD >> OD_PMFNum >> OD_Dieter >> OD_LAYNum >> OD_Side >> OD_MaPmtChan >> OD_FibChan; // warning!!! MaPmt, not Maroc!
						msg(MSG::DEBUG) << " pmf2layer, OD_PMFNum = " << OD_PMFNum  << ", OD_Dieter = "<< OD_Dieter << ", OD_LAYNum = " << OD_LAYNum << ", OD_Side = " << OD_Side << ", OD_MaPmtChan = " << OD_MaPmtChan << ", OD_FibChan = " << OD_FibChan << endreq;
	
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
						
						
// 						OD_pmf2layer[j][OD_PMFNum-1] = OD_LAYNum-1;
	//					OD_layer2pmf[j][OD_LAYNum-1] = OD_PMFNum-1;
						// changed by Petr - 19.12.2012 ---------------------------------
//						OD_MarChan = mapmt2maroc[j][OD_MaPmtChan-1];
// 						OD_MarChan = mapmt2maroc[j][OD_LAYNum-1][OD_MaPmtChan-1];
						//added to solve ATLAS coverity 13339 (only three OD layers are allowed)
						if ((OD_LAYNum > 0) && ( OD_LAYNum < 4))
						{
							OD_pmf2layer[j][OD_PMFNum-1] = OD_LAYNum-1;
							OD_MarChan = mapmt2maroc[j][OD_LAYNum-1][OD_MaPmtChan-1];
						}
						else
						{
							msg(MSG::DEBUG) << "OD_LAYNum out of bounds" << endreq;
						}

						OD_pmf_maroc2fiber[j][OD_PMFNum-1][OD_MarChan] = OD_FibChan-1;
						OD_pmf_maroc2side[j][OD_PMFNum-1][OD_MarChan] = OD_Side;
					}
				}
			}
	
			inOD.close();
		}
		else
		{
			msg(MSG::WARNING) << "the file " << mapname.c_str() << " was not open" << endreq;
			return StatusCode::FAILURE;
		}
	}

	return StatusCode::SUCCESS;
}

// changed by Petr - 19.12.2012 ---------------------------------------------------------
//StatusCode ALFA_Raw2Digit::ChanPlot(uint32_t pot_n, uint32_t MarChan, uint32_t &chan_i, uint32_t &chan_j)
StatusCode ALFA_Raw2Digit::ChanPlot(uint32_t pot_n, uint32_t iLayer, uint32_t MarChan, uint32_t &chan_i, uint32_t &chan_j)
{
	// returs values in range 1-8; input is 0-63!
//	if (int(fmod(double(maroc2mapmt[pot_n][MarChan]+1),double(8)))>0)
	if (int(fmod(double(maroc2mapmt[pot_n][iLayer][MarChan]+1),double(8)))>0)
	{
//		chan_i=int((maroc2mapmt[pot_n][MarChan]+1)/8)+1;
//		chan_j=9-int(fmod(double(maroc2mapmt[pot_n][MarChan]+1),double(8)));
		chan_i=int((maroc2mapmt[pot_n][iLayer][MarChan]+1)/8)+1;
		chan_j=9-int(fmod(double(maroc2mapmt[pot_n][iLayer][MarChan]+1),double(8)));
	}
	else
	{
//		chan_i=int((maroc2mapmt[pot_n][MarChan]+1)/8);
		chan_i=int((maroc2mapmt[pot_n][iLayer][MarChan]+1)/8);
		chan_j=1;
	}

//	msg(MSG::FATAL)  << "in funct: fibID = " << MarChan << ", chan_i " << chan_i << ", chan_j = " << chan_j <<  endreq;
	return StatusCode::SUCCESS;
}
