/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	SimpleLArDigitsChecks.cxx
 * PACKAGE : 	Trigger/L1CaloUpgrade/SimpleLArDigitsChecks
 *
 * AUTHOR : 	Denis Oliveira Damazio
 *
 *
 * **/

#include "SimpleLArDigitsChecks.h"
#include "CaloEvent/CaloCellContainer.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "xAODTracking/VertexContainer.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH1I.h"
#include <math.h>

SimpleLArDigitsChecks::SimpleLArDigitsChecks( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm (name, pSvcLocator)  {
}

SimpleLArDigitsChecks::~SimpleLArDigitsChecks(){}

StatusCode SimpleLArDigitsChecks::initialize(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "initializing SimpleLArDigitsChecks" << endmsg;
	std::string filename=name();
	filename+=".BasicCheck.root";
        m_counter=0;
	m_file = new TFile (filename.c_str(),"RECREATE");
	m_file->mkdir("EMB");
	m_file->cd("EMB");

	m_nSCells_perLayer.push_back( new TH1I( "nSCells_PreSamplerB","nSCells PreSamplerB",50,1900,1950) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_EMB1","nSCells EMB1",50,7525,7575) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_EMB2","nSCells EMB2",50,7280,7330) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_EMB3","nSCells EMB3",50,1760,1810) );

        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_PreSamplerB", "EtCells PreSamplerB",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_EMB1", "EtCells EMB1",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_EMB2", "EtCells EMB2",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_EMB3", "EtCells EMB3",60,0,10e3) );

        m_SSamples_perLayer.push_back( new TH2F( "SSamples_PreSamplerB", "SSamples PreSamplerB",8,0,8,1024,0,4096) );
        m_SSamples_perLayer.push_back( new TH2F( "SSamples_EMB1", "SSamples EMB1",8,0,8,1024,0,4096) );
        m_SSamples_perLayer.push_back( new TH2F( "SSamples_EMB2", "SSamples EMB2",8,0,8,1024,0,4096) );
        m_SSamples_perLayer.push_back( new TH2F( "SSamples_EMB3", "SSamples EMB3",8,0,8,1024,0,4096) );

        m_NorSSamples_perLayer.push_back( new TH2F( "NorSSamples_PreSamplerB", "NorSSamples PreSamplerB",8,0,8,1024,-0.2,1.2) );
        m_NorSSamples_perLayer.push_back( new TH2F( "NorSSamples_EMB1", "NorSSamples EMB1",8,0,8,1024,-0.2,1.2) );
        m_NorSSamples_perLayer.push_back( new TH2F( "NorSSamples_EMB2", "NorSSamples EMB2",8,0,8,1024,-0.2,1.2) );
        m_NorSSamples_perLayer.push_back( new TH2F( "NorSSamples_EMB3", "NorSSamples EMB3",8,0,8,1024,-0.2,1.2) );


	m_file->cd("/");
	m_file->mkdir("EMEC");
	m_file->cd("EMEC");
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_PreSamplerE","nSCells PreSamplerE",50,350,400) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_EME1","nSCells EME1",50,5350,5400) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_EME2","nSCells EME2",50,5850,5900) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_EME3","nSCells EME3",50,1500,1550) );

        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_PreSamplerE", "EtCells PreSamplerE",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_EME1", "EtCells EME1",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_EME2", "EtCells EME2",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_EME3", "EtCells EME3",60,0,10e3) );

        m_SSamples_perLayer.push_back( new TH2F( "SSamples_PreSamplerE", "SSamples PreSamplerE",8,0,8,1024,0,4096) );
        m_SSamples_perLayer.push_back( new TH2F( "SSamples_EME1", "SSamples EME1",8,0,8,1024,0,4096) );
        m_SSamples_perLayer.push_back( new TH2F( "SSamples_EME2", "SSamples EME2",8,0,8,1024,0,4096) );
        m_SSamples_perLayer.push_back( new TH2F( "SSamples_EME3", "SSamples EME3",8,0,8,1024,0,4096) );

        m_NorSSamples_perLayer.push_back( new TH2F( "NorSSamples_PreSamplerE", "NorSSamples PreSamplerE",8,0,8,1024,-0.2,1.2) );
        m_NorSSamples_perLayer.push_back( new TH2F( "NorSSamples_EME1", "NorSSamples EME1",8,0,8,1024,-0.2,1.2) );
        m_NorSSamples_perLayer.push_back( new TH2F( "NorSSamples_EME2", "NorSSamples EME2",8,0,8,1024,-0.2,1.2) );
        m_NorSSamples_perLayer.push_back( new TH2F( "NorSSamples_EME3", "NorSSamples EME3",8,0,8,1024,-0.2,1.2) );

	m_file->cd("/");
	m_file->mkdir("HEC");
	m_file->cd("HEC");

	m_nSCells_perLayer.push_back( new TH1I( "nSCells_HEC","nSCells HEC",50,0,2000) );

        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_HEC", "EtCells HEC",60,0,10e3) );
        m_SSamples_perLayer.push_back( new TH2F( "SSamples_HEC", "SSamples HEC",8,0,8,1024,0,4096) );
        m_NorSSamples_perLayer.push_back( new TH2F( "NorSSamples_HEC", "NorSSamples HEC",8,0,8,1024,-0.2,1.2) );

	m_file->cd("/");
	m_file->mkdir("FCAL");
	m_file->cd("FCAL");
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_FCAL0","nSCells FCAL0",50,350,400) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_FCAL1","nSCells FCAL1",50,230,280) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_FCAL2","nSCells FCAL2",50,100,150) );

        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_FCAL0", "EtCells FCAL0",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_FCAL1", "EtCells FCAL1",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_FCAL2", "EtCells FCAL2",60,0,10e3) );

        m_SSamples_perLayer.push_back( new TH2F( "SSamples_FCAL0", "SSamples FCAL0",8,0,8,1024,0,4096) );
        m_SSamples_perLayer.push_back( new TH2F( "SSamples_FCAL1", "SSamples FCAL1",8,0,8,1024,0,4096) );
        m_SSamples_perLayer.push_back( new TH2F( "SSamples_FCAL2", "SSamples FCAL2",8,0,8,1024,0,4096) );

        m_NorSSamples_perLayer.push_back( new TH2F( "NorSSamples_FCAL0", "NorSSamples FCAL0",8,0,8,1024,-0.2,1.2) );
        m_NorSSamples_perLayer.push_back( new TH2F( "NorSSamples_FCAL1", "NorSSamples FCAL1",8,0,8,1024,-0.2,1.2) );
        m_NorSSamples_perLayer.push_back( new TH2F( "NorSSamples_FCAL2", "NorSSamples FCAL2",8,0,8,1024,-0.2,1.2) );

	m_file->cd("/");
	m_file->mkdir("EMEC2");
	m_file->cd("EMEC2");

	m_file->cd("/");

        CHECK(m_cablingSCKey.initialize());

	return StatusCode::SUCCESS;
}

StatusCode SimpleLArDigitsChecks::finalize(){
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "finalizing SimpleLArDigitsChecks" << endmsg;
	m_file->Write();
	m_file->Close();
	return StatusCode::SUCCESS;
}

StatusCode SimpleLArDigitsChecks::execute(){
	
        MsgStream msg(msgSvc(), name());
	ATH_MSG_DEBUG( "execute SimpleLArDigitsChecks" );
        const CaloCellContainer* scells;
        const LArDigitContainer* allcalo(NULL);
	if ( evtStore()->retrieve(scells,"SCell").isFailure() ){
		ATH_MSG_WARNING( "did not find cell container" );
		return StatusCode::SUCCESS;
	}
	if ( evtStore()->retrieve(allcalo,"LArDigitSCL1").isFailure() ){
		ATH_MSG_WARNING( "did not find lardigit container for regular cells" );
		return StatusCode::SUCCESS;
	}
	const xAOD::VertexContainer* nvtx(NULL);
	if ( evtStore()->retrieve(nvtx,"PrimaryVertices").isFailure() ) {
		ATH_MSG_WARNING( "did not find Vectices container" );
		return StatusCode::SUCCESS;
	}
        SG::ReadCondHandle<LArOnOffIdMapping> scHdl{m_cablingSCKey};
        const LArOnOffIdMapping* scCont{*scHdl};
        if(!scCont) {
           ATH_MSG_WARNING("Do not have mapping object " << m_cablingSCKey.key());
           return StatusCode::SUCCESS;
        }
	unsigned int count_sCells=0;
	unsigned int count_sCells_Layer0=0;
	unsigned int count_sCells_Layer1=0;
	unsigned int count_sCells_Layer2=0;
	unsigned int count_sCells_Layer3=0;
	unsigned int count_sCells_Layer4=0;
	unsigned int count_sCells_Layer5=0;
	unsigned int count_sCells_Layer6=0;
	unsigned int count_sCells_Layer7=0;
	unsigned int count_sCells_Layer8=0;
	unsigned int count_sCells_Layer9=0;
	unsigned int count_sCells_Layer10=0;
	unsigned int count_sCells_Layer11=0;
        for(auto scell : *scells) {
		count_sCells++;
		int index=-1;
		float thre=500.0;
		if ( scell->caloDDE()->getSampling() == CaloSampling::PreSamplerB ){
			count_sCells_Layer0++;
			index=0;
			thre=2e3;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EMB1 ){
			count_sCells_Layer1++;
			index=1;
			thre=2e3;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EMB2 ){
			count_sCells_Layer2++;
			index=2;
			thre=3e3;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EMB3 ){
			count_sCells_Layer3++;
			index=3;
			thre=8e2;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::PreSamplerE ){
			count_sCells_Layer4++;
			index=4;
			thre=1e3;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EME1 ){
			count_sCells_Layer5++;
			index=5;
			thre=2e3;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EME2 ){
			count_sCells_Layer6++;
			index=6;
			thre=3e3;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EME3 ){
			count_sCells_Layer7++;
			index=7;
			thre=8e2;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::HEC0 ){
			count_sCells_Layer8++;
			index=8;
			thre=1.5e3;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::FCAL0 ){
			count_sCells_Layer9++;
			index=9;
			thre=3e3;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::FCAL1 ){
			count_sCells_Layer10++;
			index=10;
			thre=1e3;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::FCAL2 ){
			count_sCells_Layer11++;
			index=11;
			thre=5e2;
		}
		if ( index > -1 ) {
		     m_EtSCells_perLayer [index]->Fill( scell->et() );
		     if ( scell->et() > thre ) {
			HWIdentifier hwid = scCont->createSignalChannelIDFromHash( scell->caloDDE()->calo_hash() );
        		for(auto digit : *allcalo) {
			     if ( digit->hardwareID() != hwid ) continue;
			     int max=0;
			     std::vector<short> s = digit->samples();
			     for(unsigned int ii=0;ii<s.size(); ++ii){
			     m_SSamples_perLayer[index]->Fill( ii, s[ii] );
			     if ( s[ii] > max ) max=s[ii];
			     }
			     max-=1000;
			     if ( max > 0 )
			     for(unsigned int ii=0;ii<s.size(); ++ii){
			     m_NorSSamples_perLayer[index]->Fill( ii, (s[ii]-1000)/(float)max );
			     }
			}
		     }
		}
		if ( scell->et() < 3e3 ) continue;
	}
	m_nSCells_perLayer[0]->Fill ( count_sCells_Layer0 );
	m_nSCells_perLayer[1]->Fill ( count_sCells_Layer1 );
	m_nSCells_perLayer[2]->Fill ( count_sCells_Layer2 );
	m_nSCells_perLayer[3]->Fill ( count_sCells_Layer3 );
	m_nSCells_perLayer[4]->Fill ( count_sCells_Layer4 );
	m_nSCells_perLayer[5]->Fill ( count_sCells_Layer5 );
	m_nSCells_perLayer[6]->Fill ( count_sCells_Layer6 );
	m_nSCells_perLayer[7]->Fill ( count_sCells_Layer7 );
	m_nSCells_perLayer[8]->Fill ( count_sCells_Layer8 );
	m_nSCells_perLayer[9]->Fill ( count_sCells_Layer9 );
	m_nSCells_perLayer[10]->Fill ( count_sCells_Layer10 );
	m_nSCells_perLayer[11]->Fill ( count_sCells_Layer11 );
	
	m_counter++;
	return StatusCode::SUCCESS;
}

