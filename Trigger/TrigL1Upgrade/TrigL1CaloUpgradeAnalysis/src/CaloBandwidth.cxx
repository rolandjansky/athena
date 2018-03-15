/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	CaloBandwidth.cxx
 * PACKAGE : 	Trigger/L1Upgrade/L1CaloUpgradeAnalysis/CaloBandwidth
 *
 * AUTHOR : 	Denis Oliveira Damazio
 *
 *
 * **/

#include "CaloBandwidth.h"
#include "CaloEvent/CaloCellContainer.h"
#include "LArCabling/LArSuperCellCablingTool.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "TFile.h"
#include "TH1F.h"

CaloBandwidth::CaloBandwidth( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm (name, pSvcLocator), m_cabling("LArSuperCellCablingTool")  {
       declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise tool");
       declareProperty("EtInSigma", m_etInSigma = -1.0, "sigma cut");
       declareProperty("Et", m_et = 100.0, "et cut");
}

CaloBandwidth::~CaloBandwidth(){}

StatusCode CaloBandwidth::initialize(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "initializing CaloBandwidth" << endreq;
        if ( (m_etInSigma > 0 ) && ( m_et > 0 ) )
        msg << MSG::WARNING << "Configuration issue" << endreq;
        if ( m_etInSigma > 0 ) m_noiseTool.retrieve().ignore();
        m_counter = 0;
	std::string filename=name();
	filename+=".CaloBandwidth.root";
	m_file = new TFile (filename.c_str(),"RECREATE");
	m_file->mkdir("CaloBandwidth");
	m_file->cd("CaloBandwidth");

	// for cell <-> SCell comparison
	if ( m_cabling.retrieve().isFailure() ){
		msg << MSG::ERROR << "cannot perform comparisons between SuperCells and digits" << endreq;
	}

	for(float lower=-2.4;lower<2.4;lower+=0.1) {
		float upper = lower+0.1;
		std::pair<float,float> p(lower,upper);
		char s[20];
		sprintf(s,"delta_%2.1f_%2.1f",lower,upper);
		std::cout << p.first << " " << p.second << " " << s << std::endl;
		m_limits.push_back(p);
		TString titleallLa1("allLa1_"); titleallLa1+=s;
		m_allLa1.push_back( new TH1F(titleallLa1,titleallLa1,70,0,70) );
		TString titleallLa1AboveThr("allLa1AboveThr_"); titleallLa1AboveThr+=s;
		m_allLa1AboveThr.push_back( new TH1F(titleallLa1AboveThr,titleallLa1AboveThr,70,0,70) );
		TString titleallLa1Above1Sig("allLa1Above1Sig_"); titleallLa1Above1Sig+=s;
		m_allLa1Above1Sig.push_back( new TH1F(titleallLa1Above1Sig,titleallLa1Above1Sig,70,0,70) );
		TString titleallLa1Above2Sig("allLa1Above2Sig_"); titleallLa1Above2Sig+=s;
		m_allLa1Above2Sig.push_back( new TH1F(titleallLa1Above2Sig,titleallLa1Above2Sig,70,0,70) );
		TString titleallLa1Above3Sig("allLa1Above3Sig_"); titleallLa1Above3Sig+=s;
		m_allLa1Above3Sig.push_back( new TH1F(titleallLa1Above3Sig,titleallLa1Above3Sig,70,0,70) );
	}

	return StatusCode::SUCCESS;
}

StatusCode CaloBandwidth::finalize(){
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "finalizing SimpleLArDigitsChecks" << endreq;
	m_file->Write();
	m_file->Close();
	return StatusCode::SUCCESS;
}

StatusCode CaloBandwidth::execute(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "execute CaloBandwidth" << endreq;

	const CaloCellContainer* allcalo;
        if ( evtStore()->retrieve(allcalo,"AllCalo").isFailure() ) {
                msg << MSG::WARNING << "did not find allcalo container" << endreq;
		return StatusCode::SUCCESS;
        }

	// monitoring and identifying seeds
	//std::vector< int > seeds;
	//seeds.reserve(1000);
	int ii=-1;
	std::map< std::pair<float,float> , int > mallLa1;
	std::map< std::pair<float,float> , int > mallLa1AboveThr;
	std::map< std::pair<float,float> , int > mallLa1Above1Sig;
	std::map< std::pair<float,float> , int > mallLa1Above2Sig;
	std::map< std::pair<float,float> , int > mallLa1Above3Sig;
        for(auto cl : *allcalo) {
		int samp = cl->caloDDE()->getSampling();
		ii++;
		if ( !( (samp==1) || (samp==5) ) ) continue;
		if ( ! ( (cl->phi() >=0 ) && ( cl->phi() <0.2 ) ) ) continue;
		int idx=-1;
		for(unsigned int i=0;i<m_limits.size(); ++i ) {
			std::pair<float,float> p = m_limits[i];
			float eta = cl->eta();
			if ( (eta>=p.first) && (eta<p.second) ) { idx = i; break; }
		}
		std::pair<float,float> p = m_limits[idx];
		if ( mallLa1.find(p) == mallLa1.end() ) mallLa1[p] = 1;
		else mallLa1[p]++;
                float energy = cl->energy();
                float sigma = 0.0;
                sigma = m_noiseTool->getNoise(&(*cl),ICalorimeterNoiseTool::TOTALNOISE);
		if ( energy > 0.1 ) {
		if ( mallLa1AboveThr.find(p) == mallLa1AboveThr.end() ) mallLa1AboveThr[p] = 1;
		else mallLa1AboveThr[p]++;
		}
		if ( energy > sigma  ) {
		if ( mallLa1Above1Sig.find(p) == mallLa1Above1Sig.end() ) mallLa1Above1Sig[p] = 1;
		else mallLa1Above1Sig[p]++;
		}
		if ( energy > 2*sigma  ) {
		if ( mallLa1Above2Sig.find(p) == mallLa1Above2Sig.end() ) mallLa1Above2Sig[p] = 1;
		else mallLa1Above2Sig[p]++;
		}
		if ( energy > 3*sigma  ) {
		if ( mallLa1Above3Sig.find(p) == mallLa1Above3Sig.end() ) mallLa1Above3Sig[p] = 1;
		else mallLa1Above3Sig[p]++;
		}
	}
	for (auto i : mallLa1 ){
		//std::cout << i.first.first << " " << i.first.second << " " << i.second << std::endl;
		int idx=-1;
		for(unsigned int ii=0;ii<m_limits.size(); ++ii ) {
			std::pair<float,float> p = m_limits[ii];
			float eta = i.first.first;
			if ( TMath::Abs(eta-p.first)<0.002  ) { idx = ii; break; }
		}
		if ( idx >=0 ) m_allLa1[idx]->Fill( i.second );
		
	}
	for (auto i : mallLa1AboveThr ){
		//std::cout << i.first.first << " " << i.first.seecond << " " << i.second << std::endl;
                int idx=-1;
		for(unsigned int ii=0;ii<m_limits.size(); ++ii ) {
			std::pair<float,float> p = m_limits[ii];
			float eta = i.first.first;
			if ( TMath::Abs(eta-p.first)<0.002  ) { idx = ii; break; }
                }
                if ( idx >=0 ) m_allLa1AboveThr[idx]->Fill( i.second );
	}
	for (auto i : mallLa1Above1Sig ){
		//std::cout << i.first.first << " " << i.first.second << " " << i.second << std::endl;
                int idx=-1;
		for(unsigned int ii=0;ii<m_limits.size(); ++ii ) {
			std::pair<float,float> p = m_limits[ii];
			float eta = i.first.first;
			if ( TMath::Abs(eta-p.first)<0.002  ) { idx = ii; break; }
                }
                if ( idx >=0 ) m_allLa1Above1Sig[idx]->Fill( i.second );
	}
	for (auto i : mallLa1Above2Sig ){
		//std::cout << i.first.first << " " << i.first.second << " " << i.second << std::endl;
                int idx=-1;
		for(unsigned int ii=0;ii<m_limits.size(); ++ii ) {
			std::pair<float,float> p = m_limits[ii];
			float eta = i.first.first;
			if ( TMath::Abs(eta-p.first)<0.002  ) { idx = ii; break; }
                }
                if ( idx >=0 ) m_allLa1Above2Sig[idx]->Fill( i.second );
	}
	for (auto i : mallLa1Above3Sig ){
		//std::cout << i.first.first << " " << i.first.second << " " << i.second << std::endl;
                int idx=-1;
		for(unsigned int ii=0;ii<m_limits.size(); ++ii ) {
			std::pair<float,float> p = m_limits[ii];
			float eta = i.first.first;
			if ( TMath::Abs(eta-p.first)<0.002  ) { idx = ii; break; }
                }
                if ( idx >=0 ) m_allLa1Above3Sig[idx]->Fill( i.second );
	}
#ifdef DONTDO
		float et = cl->et();
		float energy = cl->energy();
		float etGeV = et*1e-3;
		float noiseToCompare = m_et;
		float sigma = 0.0;
                if ( m_etInSigma >= 0.0 ){
                    sigma = m_noiseTool->getNoise(&(*cl),ICalorimeterNoiseTool::TOTALNOISE);
                    noiseToCompare = m_etInSigma*sigma;
		}
		m_allLa1->Fill(etGeV);
		if ( energy > 0.1 ) m_allLa1AboveThr->Fill(etGeV); 
		if ( energy > sigma   ) m_allLa1Above1Sig->Fill(etGeV); 
		if ( energy > 2*sigma ) m_allLa1Above2Sig->Fill(etGeV); 
		if ( energy > 3*sigma ) m_allLa1Above3Sig->Fill(etGeV); 
		if ( energy > noiseToCompare ) seeds.push_back( ii );
        // Prepare output containers (with all xAOD annoying features)
	std::vector<float> etasG;
	etasG.reserve(200);
	for(unsigned int i=0; i < seeds.size() ; i++ ) {
		const CaloCell* seed = allcalo->at( seeds.at(i) );
		xAOD::TrigEMCluster* saveL1 = new xAOD::TrigEMCluster();
		clusters->push_back(saveL1);
		etasG.clear();
		float etaSeed = seed->eta();
		float phiSeed = seed->phi();
		saveL1->setEta(etaSeed);
		saveL1->setPhi(phiSeed);
        	for(auto cl : *allcalo) {
		   float etaCell = cl->eta();
		   if ( fabsf( etaCell - etaSeed ) > 0.12 ) continue;
		   float phiCell = cl->phi();
		   float dphi = fabsf( phiCell - phiSeed ) ;
		   dphi = fabsf( M_PI - dphi );
		   dphi = fabsf( M_PI - dphi );
		   if ( dphi > 0.11 ) continue;
		   int samp = cl->caloDDE()->getSampling();
		   if ( !( (samp==1) || (samp==5) ) ) continue;
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
                   for(auto cl : *allcalo) {
                      float etaCell = cl->eta();
                      if ( fabsf( etaCell - etaG ) > 0.0025 ) continue;
                      float phiCell = cl->phi();
                      float dphi = fabsf( phiCell - phiSeed ) ;
                      dphi = fabsf( M_PI - dphi );
                      dphi = fabsf( M_PI - dphi );
                      if ( dphi > 0.11 ) continue;
                      int samp = cl->caloDDE()->getSampling();
                      if ( !( (samp==1) || (samp==5) ) ) continue;
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
		if ( (max+max2) > 0.1 ) m_Eratio->Fill( (max-max2) / (max+max2) );
		saveL1->setEmaxs1( max );
		saveL1->setE2tsts1( max2 );
		if ( (max_index > 3) && (max_index<etasG.size()-3) ) {
			float strip7 = 0.0;
			for (int j=-3;j<=3;j++) strip7+=en1st[max_index-j];
			float strip3 = 0.0;
			for (int j=-1;j<=1;j++) strip3+=en1st[max_index-j];
			if ( strip3 > 1e-3 ) {
			   saveL1->setFracs1( (strip7-strip3) / strip3 );
			   m_frac73->Fill( (strip7-strip3) / strip3 );
			} else {
			    saveL1->setFracs1( 999.0 );
			    m_frac73->Fill( 999.0 );
			}
		}  else {
		   saveL1->setFracs1( 999.0 );
		   m_frac73->Fill( 999.0 );
		}
		float wtot = 0.0;
		float etot = 0.0;
		for (int j=-3;j<=3;j++) {
		    etot+=en1st[max_index-j];
		    wtot+=en1st[max_index-j]*(max_index-j)*(max_index-j);
		}
		if ( etot > 1e-3 ) {
		   saveL1->setWstot( sqrt( wtot/etot ) );
		   m_wStot->Fill ( sqrt( wtot/etot ) );
		}
		else {
		   saveL1->setWstot( 999.0 );
		   m_wStot->Fill ( 999.0 );
		}
	}

	m_counter++;
#endif
	
	return StatusCode::SUCCESS;
}

