/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	LArFex.cxx
 * PACKAGE : 	Trigger/L1Upgrade/L1CaloUpgrade/LArFex
 *
 * AUTHOR : 	Denis Oliveira Damazio
 *
 *
 * **/

#include "LArFex.h"
#include "CaloEvent/CaloCellContainer.h"
#include "LArCabling/LArSuperCellCablingTool.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "TFile.h"
#include "TH1F.h"

LArFex::LArFex( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm (name, pSvcLocator), m_cabling("LArSuperCellCablingTool")  {
       declareProperty("InputClusterName", m_inputClusterName = "SCluster" );
       declareProperty("InputLvl1Name", m_inputLvl1Name = "LVL1EmTauRoIs" );
       declareProperty("OutputClusterName", m_outputClusterName = "LArLayer1Vars" );
       declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise tool");
       declareProperty("EtInSigma", m_etInSigma = -1.0, "sigma cut");
       declareProperty("Et", m_et = 100.0, "et cut");
       declareProperty("EtInSigmaSelect", m_etInSigmaS = -1.0, "sigma cut");
       declareProperty("EtSelect", m_etS = -100.0, "et cut");
       declareProperty("EnableMonitoring", m_enableMon=false);
}

LArFex::~LArFex(){}

StatusCode LArFex::initialize(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "initializing LArFex" << endreq;
        if ( (m_etInSigma > 0 ) && ( m_et > 0 ) )
        msg << MSG::WARNING << "Configuration issue" << endreq;
        if ( m_etInSigma > 0 ) m_noiseTool.retrieve().ignore();
        m_counter = 0;
	if ( m_enableMon ) {
	  std::string filename=name();
	  filename+=".LArFex.root";
	  m_file = new TFile (filename.c_str(),"RECREATE");
	  m_file->mkdir("LArFex");
	  m_file->cd("LArFex");
	}

	// for cell <-> SCell comparison
	if ( m_cabling.retrieve().isFailure() ){
		msg << MSG::ERROR << "cannot perform comparisons between SuperCells and digits" << endreq;
	}

	if ( m_enableMon ) {
	  m_allLa1 = new TH1F("allLa1","all Layer 1 et", 100,0,20);
	  m_allLa1AboveThr = new TH1F("allLa1AboveThr","all Layer 1 et above fixed Threshould", 100,0,20);
	  m_allLa1Above1Sig = new TH1F("allLa1Above1Sig","all Layer 1 et above 1 Sigma", 100,0,20);
	  m_allLa1Above2Sig = new TH1F("allLa1Above2Sig","all Layer 1 et above 2 Sigma", 100,0,200);
	  m_allLa1Above3Sig = new TH1F("allLa1Above3Sig","all Layer 1 et above 3 Sigma", 100,0,200);
	  m_Eratio = new TH1F("Eratio","Eratio",60,0.0,1.2);
	  m_frac73 = new TH1F("frac73","frac73",60,0.0,1.2);
	  m_wStot = new TH1F("wStot","wStot",60,0.0,3.0);
	}

	return StatusCode::SUCCESS;
}

StatusCode LArFex::finalize(){
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "finalizing SimpleLArDigitsChecks" << endreq;
	if ( m_enableMon ) {
	  m_file->Write();
	  m_file->Close();
	}
	return StatusCode::SUCCESS;
}

StatusCode LArFex::execute(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "execute LArFex" << endreq;

	const CaloCellContainer* allcalocells;
        if ( evtStore()->retrieve(allcalocells,"AllCalo").isFailure() ) {
                msg << MSG::WARNING << "did not find allcalo container" << endreq;
		return StatusCode::SUCCESS;
        }
	CaloCellContainer allcalo(SG::VIEW_ELEMENTS);
	if ( !( (m_etS>0) || ( m_etInSigmaS > 0 ) ) ) {
        for(auto cl : *allcalocells) {
                int samp = cl->caloDDE()->getSampling();
                if ( !( (samp==1) || (samp==5) ) ) continue;
		allcalo.push_back( cl );
	} 
	} else {
        for(auto cl : *allcalocells) {
                int samp = cl->caloDDE()->getSampling();
                if ( !( (samp==1) || (samp==5) ) ) continue;
                float et = cl->et();
                float noiseToCompare = m_etS;
                float sigma = 0.0;
                if ( m_etInSigmaS >= 0.0 ){
                    sigma = m_noiseTool->getNoise(&(*cl),ICalorimeterNoiseTool::TOTALNOISE);
                    noiseToCompare = m_etInSigmaS*sigma;
                }
		noiseToCompare*=cl->caloDDE()->sinTh();
                if ( et > noiseToCompare ) allcalo.push_back( cl );
        }
	}

	// monitoring and identifying seeds
	std::vector< int > seeds;
	seeds.reserve(1000);
	int ii=-1;
        for(auto cl : allcalo) {
		//int samp = cl->caloDDE()->getSampling();
		ii++;
		//if ( !( (samp==1) || (samp==5) ) ) continue;
		float et = cl->et();
		float etGeV = et*1e-3;
		float energy = cl->energy();
		float noiseToCompare = m_et;
		float sigma = 0.0;
                if ( m_etInSigma >= 0.0 ){
                    sigma = m_noiseTool->getNoise(&(*cl),ICalorimeterNoiseTool::TOTALNOISE);
                    noiseToCompare = m_etInSigma*sigma;
		}
		if ( m_enableMon ) {
		  m_allLa1->Fill(etGeV);
		  if ( energy > 0.1 ) m_allLa1AboveThr->Fill(etGeV); 
		  if ( energy > sigma   ) m_allLa1Above1Sig->Fill(etGeV); 
		  if ( energy > 2*sigma ) m_allLa1Above2Sig->Fill(etGeV); 
		  if ( energy > 3*sigma ) m_allLa1Above3Sig->Fill(etGeV); 
		}
		if ( energy > noiseToCompare ) seeds.push_back( ii );
	}
        // Prepare output containers (with all xAOD annoying features)
        xAOD::TrigEMClusterContainer* clusters = new xAOD::TrigEMClusterContainer();
        xAOD::TrigEMClusterAuxContainer* auxclusters = new xAOD::TrigEMClusterAuxContainer();
        clusters->setStore(auxclusters);
	clusters->reserve(1000);
        std::string clusterName(m_outputClusterName);
        if ( evtStore()->record(clusters,clusterName).isFailure() ){
                msg << MSG::ERROR  << "recording was not possible" << endreq;
                return StatusCode::FAILURE;
        }
        if ( evtStore()->record(auxclusters,clusterName+"Aux.").isFailure() ){
                msg << MSG::ERROR << "recording Aux was not possible" << endreq;
                return StatusCode::FAILURE;
        }
	std::vector<float> etasG;
	etasG.reserve(200);
	for(unsigned int i=0; i < seeds.size() ; i++ ) {
		const CaloCell* seed = allcalo.at( seeds.at(i) );
		xAOD::TrigEMCluster* saveL1 = new xAOD::TrigEMCluster();
		clusters->push_back(saveL1);
		etasG.clear();
		float etaSeed = seed->eta();
		float phiSeed = seed->phi();
		saveL1->setEta(etaSeed);
		saveL1->setPhi(phiSeed);
        	for(auto cl : allcalo) {
		   float etaCell = cl->eta();
		   if ( fabsf( etaCell - etaSeed ) > 0.12 ) continue;
		   float phiCell = cl->phi();
		   float dphi = fabsf( phiCell - phiSeed ) ;
		   dphi = fabsf( M_PI - dphi );
		   dphi = fabsf( M_PI - dphi );
		   if ( dphi > 0.11 ) continue;
		   //int samp = cl->caloDDE()->getSampling();
		   //if ( !( (samp==1) || (samp==5) ) ) continue;
		   float min_dist = 999;
		   for(auto etaG : etasG) {
			float dist = fabsf( etaG - etaCell );
			if ( dist < min_dist ) min_dist = dist;
		   }
		   if ( min_dist > 0.002 ) etasG.push_back(etaCell);
		}
		std::sort( etasG.begin(), etasG.end() );
		std::vector< float > en1st ( etasG.size(), 0.0 );
		int ii=-1;
		for(auto etaG : etasG) {
		   ii++;
                   for(auto cl : allcalo) {
                      float etaCell = cl->eta();
                      if ( fabsf( etaCell - etaG ) > 0.0025 ) continue;
                      float phiCell = cl->phi();
                      float dphi = fabsf( phiCell - phiSeed ) ;
                      dphi = fabsf( M_PI - dphi );
                      dphi = fabsf( M_PI - dphi );
                      if ( dphi > 0.11 ) continue;
                      //int samp = cl->caloDDE()->getSampling();
                      //if ( !( (samp==1) || (samp==5) ) ) continue;
		      en1st[ii]+=cl->et();
		   }
		}
		float max = -999;
		float max_index = -1;
		for( unsigned int ii=0; ii < en1st.size(); ii++ ){
		     if ( en1st[ii] > max ) { max = en1st[ii]; max_index=ii;}
		}
		float max2=-999.0;
		for( unsigned int ii=1; ii < en1st.size()-1; ii++ ){
		     if ( ii == max_index ) continue;
		     if (!((en1st[ii] > en1st[ii-1]) && (en1st[ii] >en1st[ii+1])))
			continue;
		     if ( en1st[ii] > max2 ) { max2 = en1st[ii]; }
		}
		if ( (max+max2) > 0.1 ) 
		   if ( m_enableMon ) m_Eratio->Fill( (max-max2) / (max+max2) );
		saveL1->setEmaxs1( max );
		saveL1->setE2tsts1( max2 );
		if ( (max_index > 3) && (max_index<etasG.size()-3) ) {
			float strip7 = 0.0;
			for (int j=-3;j<=3;j++) strip7+=en1st[max_index+j];
			float strip3 = 0.0;
			for (int j=-1;j<=1;j++) strip3+=en1st[max_index+j];
			if ( strip3 > 1e-3 ) {
			   saveL1->setFracs1( (strip7-strip3) / strip3 );
			   if ( m_enableMon ) 
			      m_frac73->Fill( (strip7-strip3) / strip3 );
			} else {
			    saveL1->setFracs1( 999.0 );
			    if ( m_enableMon ) 
			        m_frac73->Fill( 999.0 );
			}
		}  else {
		   saveL1->setFracs1( 999.0 );
		   if ( m_enableMon ) m_frac73->Fill( 999.0 );
		}
		float wtot = 0.0;
		float etot = 0.0;
		if ( (max_index > 3) && (max_index<etasG.size()-3) ) {
		for (int j=-3;j<=3;j++) {
		    etot+=en1st[max_index+j];
		    wtot+=en1st[max_index+j]*j*j;
		}
		}
		if ( (etot > 1e-3) && (wtot>1e-3) ) {
		   saveL1->setWstot( sqrt( wtot/etot ) );
		   if ( m_enableMon ) m_wStot->Fill ( sqrt( wtot/etot ) );
		}
		else {
		   saveL1->setWstot( 999.0 );
		   if ( m_enableMon ) m_wStot->Fill ( 999.0 );
		}
	}

	m_counter++;
	
	return StatusCode::SUCCESS;
}

