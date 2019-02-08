/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArTCMPhysWavePredictor.h"
#include "LArCalibUtils/LArTCMFitterTool.h"

#include "GaudiKernel/ToolHandle.h"

#include "LArRawConditions/LArCaliWaveContainer.h"
#include "LArRawConditions/LArPhysWaveContainer.h"

#include "CaloIdentifier/CaloIdManager.h"

#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TTree.h"

typedef LArPhysWaveContainer::ConstConditionsMapIterator PhysWaveIt;

LArTCMPhysWavePredictor::LArTCMPhysWavePredictor (const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator)
{
  declareProperty("TestMode",m_testmode=false);

  declareProperty("DataFromFile",m_datafromfile=false);
  declareProperty("MinuitOutputLevel",m_minuitoutputlevel=1); // -1: no output, 1: std output
  declareProperty("RootRawDump",m_rootrawdump=false); // to dump caliwave,physwave and fit result to root file.
  declareProperty("RootOutputFile",m_rootoutputfile=std::string("dumpTCM.root")) ;
  
  //Cell filters
  declareProperty("FilterCells",m_filter_cells=false);
  
  declareProperty("FilterCells_Layer",m_filter_layer=-1);
  
  declareProperty("FilterCells_Phi",m_filter_phi=false);
  declareProperty("FilterCells_Phi_Min",m_filter_phi_min=-1);
  declareProperty("FilterCells_Phi_Max",m_filter_phi_max=3000); //fancy values...
  
  declareProperty("FilterCells_Eta",m_filter_eta=false);
  declareProperty("FilterCells_Eta_Min",m_filter_eta_min=-3000); //fancy values...
  declareProperty("FilterCells_Eta_Max",m_filter_eta_max=3000);
  
  // list of gains to be processed
  m_keylist.clear() ;
  m_keylist.push_back("HIGH");
  m_keylist.push_back("MEDIUM");
  m_keylist.push_back("LOW");
  declareProperty("KeyList",m_keylist);

  declareProperty("GroupingType",      m_groupingType);  

}

///////////////////////////////////////////////////
LArTCMPhysWavePredictor::~LArTCMPhysWavePredictor() 
{}

////////////////////////////////////////////////
StatusCode LArTCMPhysWavePredictor::initialize() 
{
  ATH_MSG_INFO ( " Initialize...");
  return StatusCode::SUCCESS ;
}

StatusCode LArTCMPhysWavePredictor::stop()
{
  ATH_MSG_INFO ( "From stop...");
  
  ToolHandle<LArWFParamTool> larWFParamTool("LArWFParamTool");
  ATH_CHECK( larWFParamTool.retrieve() );

  ToolHandle<LArTCMFitterTool> larTCMFitterTool("LArTCMFitterTool");
  ATH_CHECK( larTCMFitterTool.retrieve() );
  
  larTCMFitterTool->setminuitoutputlevel(m_minuitoutputlevel);
  
  const LArEM_ID* emId;
   
  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  emId = caloIdMgr->getEM_ID();
  if (!emId) {
    ATH_MSG_ERROR ( "Could not access lar EM ID helper" );
    return StatusCode::FAILURE;
  }   
  
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key());
     return StatusCode::FAILURE;
  }

  //open ouput file for dumping waves
  TFile f(m_rootoutputfile.c_str(),"recreate");	//need to implement: if (m_rootrawdump)
  TTree* tree = 0;
  
  // TTree structure
  struct tree_struct {
      Int_t iGain;
      Int_t iLayer;
      Int_t iPhi;
      Int_t iEta;
      Int_t ipdg;
    
      Double_t taud;
      Double_t tauexp;
      Double_t f;
      Double_t w0;
      Double_t taur;
      Double_t SplineShift;
      Double_t top;
      Double_t Emean;
      Double_t MphyMcal;
      Double_t chi2;
    
      Int_t DeadCell;
      Int_t fit_result;

      Double_t peak_residus;

      Int_t CAL_nb;
      Int_t CAL_chan;
  };
  tree_struct ts1;

  const Int_t nlayer = 4;
  const Int_t ngain = 3;
  const Int_t nsampling = 4;
  const Int_t iphilimit[2][nsampling] = {{0,0,0,0},{4,4,16,16}};  //verifier layer 0 !!!
  const Int_t ietalimit[2][nsampling] = {{0,0,0,0},{57,451,57,27}}; //verifier layer 0 !!!

  Char_t temp[100];
  TH2F* CellMap[nlayer][ngain];
  //TH2F* NentriesMap[nlayer][ngain];

  if ( m_rootrawdump ) {

  //create histogram
      for(Int_t layer=0;layer<nlayer;layer++){
	  for(Int_t gain=0;gain<ngain;gain++) {
		
	      //fill CellMap
	      memset(temp,0,sizeof(temp));
	      sprintf(temp,"CellMaplayer%.1d_gain%.1d",layer,gain);
	      CellMap[layer][gain] = new TH2F(temp,temp,ietalimit[1][layer],ietalimit[0][layer],ietalimit[1][layer],iphilimit[1][layer],iphilimit[0][layer],iphilimit[1][layer]);
		
	      //fill NentriesMap
	      //memset(temp,0,sizeof(temp));
	      //sprintf(temp,"NentriesMaplayer%.1d_gain%.1d",layer,gain);
	      //NentriesMap[layer][gain] = new TH2F(temp,temp,ietalimit[1][layer],ietalimit[0][layer],ietalimit[1][layer],iphilimit[1][layer],iphilimit[0][layer],iphilimit[1][layer]);
	  }
      }
	
	
      tree = new TTree("tree","tree essai");
      tree->Branch("iGain",&ts1.iGain,"iGain/I");
      tree->Branch("iLayer",&ts1.iLayer,"iLayer/I");
      tree->Branch("iPhi",&ts1.iPhi,"iPhi/I");
      tree->Branch("iEta",&ts1.iEta,"iEta/I");
      tree->Branch("ipdg",&ts1.ipdg,"ipdg/I");
		
      tree->Branch("taud",&ts1.taud,"taud/D");
      tree->Branch("tauexp",&ts1.tauexp,"tauexp/D");
      tree->Branch("f",&ts1.f,"f/D");
      tree->Branch("w0",&ts1.w0,"w0/D");
      tree->Branch("taur",&ts1.taur,"taur/D");
      tree->Branch("SplineShift",&ts1.SplineShift,"SplineShift/D");
      tree->Branch("top",&ts1.top,"top/D");
      tree->Branch("Emean",&ts1.Emean,"Emean/D");
      tree->Branch("MphyMcal",&ts1.MphyMcal,"MphyMcal/D");
      tree->Branch("chi2",&ts1.chi2,"chi2/D");
		
      tree->Branch("DeadCell",&ts1.DeadCell,"DeadCell/I");
      tree->Branch("fit_result",&ts1.fit_result,"fit_result/I");

      tree->Branch("peak_residus",&ts1.peak_residus,"peak_residus/D");
      tree->Branch("CAL_nb",&ts1.CAL_nb,"CAL_nb/I");
      tree->Branch("CAL_chan",&ts1.CAL_chan,"CAL_chan/I");
  }

  //retrieve Physics pulses either from database or from file
  // Files are self root files produced by emtb for debug purpose
  LArPhysWaveContainer* physWaveContainer;
  if (!m_datafromfile) {
      // retrieve Physics pulses from database   
      const LArPhysWaveContainer* constphysWaveContainer;
      ATH_CHECK( detStore()->retrieve(constphysWaveContainer) );
      physWaveContainer = (LArPhysWaveContainer*) constphysWaveContainer;
      ATH_MSG_INFO ( "LArPhysWaveContainer found in StoreGate." );
  } else {
      //from self external file; to debug and compare with emtb
      // for the moment just test P13 phi=9 layer 2
      physWaveContainer = new LArPhysWaveContainer();
      ATH_CHECK( physWaveContainer->setGroupingType(m_groupingType,msg()) );

      ATH_CHECK( physWaveContainer->initialize() );
    
      TFile physicsinput("/afs/cern.ch/user/p/prieur/scratch0/OF-P13_21-PhyPredDataFile.root");
      physicsinput.cd("PhysicsPulses");
	
      int ilayer=2;
      int iphi = 9;
      int ieta_start = 25;
      int ieta_end = 35;
      int igain;
      if ((ilayer==1)||(ilayer==2)) igain = CaloGain::LARMEDIUMGAIN;
      else igain = CaloGain::LARHIGHGAIN;
	
      for(int ieta = ieta_start;ieta<ieta_end;ieta++) {
	  char name[80];
	  memset(name,0,sizeof(name));
	  sprintf(name,"hphys%.1d%.1d%.2d%.3d",igain,ilayer,iphi,ieta);
	  TProfile* physicspulse=(TProfile*)gDirectory->Get(name);
	  if(physicspulse==NULL) {
	      std::cout<<"Could not get physics pulse "<<name<<" from file."<<std::endl;
	      continue;
	  }
		
	  Identifier id=emId->channel_id(+1,ilayer,+0,ieta,iphi);		
	  HWIdentifier chID = cabling->createSignalChannelID(id);
	  //if ((ilayer==1)||(ilayer==2)) larPhysWave = new LArPhysWave(175,1,1); //gain 1 for debug	
	  //else larPhysWave = new LArPhysWave(175,1,1);
	  LArPhysWave larPhysWave (175, 1, 1);
	  ATH_MSG_DEBUG("name: "<<name<<" ilayer: "<<ilayer<<" ieta: "<<ieta<<" iphi: "<<iphi<<" id "<< chID.get_compact());
		
	  // decode id to check if correct
	  //HWIdentifier testchID = larPhysWave->channelID();
	  //Identifier testid=cabling->cnvToIdentifier(testchID);
	  //std::cout <<"physwave "<<" eta "<<emId->eta(testid)<<" phi "<< emId->phi(testid)<<" layer "<<emId->sampling(testid)<<" gain "<<larPhysWave->getGain()<<std::endl;
		
	  //Fill LArPhysWave
	  for(int i=0;i<physicspulse->GetNbinsX();i++) {
	      larPhysWave.setSample(i,physicspulse->GetBinContent(i+1));
	      //std::cout<<physicspulse->GetBinContent(i+1)<<" "<<larPhysWave->getSample(i)<<std::endl;
	  }

	  //physWaveContainer->push_back(larPhysWave); // old container
          physWaveContainer->setPdata(chID, larPhysWave, igain); // new container

      }

      //log<<MSG::INFO<<"LArPhysWaveContainer size: "<<physWaveContainer->size()<<endmsg;

  } // end else m_datafromfile

  unsigned nchannel = 0 ;

  //Create LArPhysWaveContainer for Predicted Pulses
  LArPhysWaveContainer*  larpredPhysWaveContainer = new LArPhysWaveContainer();
  ATH_CHECK( larpredPhysWaveContainer->setGroupingType(m_groupingType,msg()) );

  ATH_CHECK( larpredPhysWaveContainer->initialize() );
  
  //Create container for Params
//  LArWFParamsContainer* larWFParamsContainer=new LArWFParamsContainer();

  // get the LArCaliWave from the detector store
  std::vector<std::string>::const_iterator key_it=m_keylist.begin();
  std::vector<std::string>::const_iterator key_it_e=m_keylist.end();

  for (;key_it!=key_it_e;++key_it) { // Loop over all LArCaliWave containers that are to be processed from m_keylist
  
      std::string keyCali = *key_it ;
      
      const LArCaliWaveContainer* caliWaveContainer = nullptr;
      ATH_CHECK( detStore()->retrieve(caliWaveContainer,keyCali) );
      
      ATH_MSG_INFO ( "Processing LArCaliWaveContainer from StoreGate, key = " << keyCali );
      
      for ( unsigned gain = CaloGain::LARHIGHGAIN ; gain < CaloGain::LARNGAIN ; ++gain ) {  // Loop over gains in current container
  
        ATH_MSG_VERBOSE ( "Now processing gain = " << gain << " in LArCaliWaveContainer with key = " << keyCali );
         
      // loop over cali wave container
      typedef LArCaliWaveContainer::ConstConditionsMapIterator const_iterator;
      const_iterator itVec   = caliWaveContainer->begin(gain);
      const_iterator itVec_e = caliWaveContainer->end(gain);
      
      if ( itVec == itVec_e ) {
        ATH_MSG_INFO ( "LArCaliWaveContainer (key=" << keyCali << ") has no wave with gain = " << gain );
         continue ;
      }

      for (; itVec != itVec_e; ++itVec) { // loop over cells for the current gain
	  	  	
	  LArCaliWaveContainer::LArCaliWaves::const_iterator cont_it   = (*itVec).begin();
          LArCaliWaveContainer::LArCaliWaves::const_iterator cont_it_e = (*itVec).end();
          	  
	  if ( cont_it == cont_it_e ) { 
            ATH_MSG_DEBUG (  "Empty channel found in LArCaliWave container: skipping..." );
	     continue ; 	  
	  } else {
            ATH_MSG_DEBUG ( (*itVec).size() << " LArCaliWaves found for channel 0x" << MSG::hex << itVec.channelId() << MSG::dec );
	  }
	  
	  const HWIdentifier chID = itVec.channelId();
	  Identifier id ;
	  
	  try {
	    id = cabling->cnvToIdentifier(chID);   
	  } catch ( const LArID_Exception& ) {
	    ATH_MSG_ERROR ( "Cabling exception caught for channel 0x" << MSG::hex << chID << MSG::dec );
	  }
	  int layer = emId->sampling(id);
          int eta   = emId->eta(id);
	  int phi   = emId->phi(id);
	  
	  if ( m_filter_cells ) {	//Filter Cells
	  
	      if ( m_filter_layer!=-1 && layer!=m_filter_layer ) {
	        ATH_MSG_INFO ( "Skipping channel 0x" << MSG::hex << chID << MSG::dec << " because of FilterCells_Layer cut.");
	        continue;
	      }	    	    
	      
	      if ( m_filter_eta )
	         if ( (eta<m_filter_eta_min) || (eta>m_filter_eta_max) ) {
	           ATH_MSG_INFO ( "Skipping channel 0x" << MSG::hex << chID << MSG::dec << " because of FilterCells_Eta cut.");
		   continue;		    	 
                 }
	  
              if ( m_filter_phi ) {
	         
		 if ( (layer==1 && ( phi<m_filter_phi_min/4 || phi>m_filter_phi_max/4) ) ||
		      (layer!=1 && ( phi<m_filter_phi_min   || phi>m_filter_phi_max  ) ) 
		    ) {
		   ATH_MSG_INFO ( "Skipping channel 0x" << MSG::hex << chID << MSG::dec << " because of FilterCells_Phi cut.");
		   continue;	    	 
		 } 

	      }
	      
	  } // end filter cells
	  
	  
	  ATH_MSG_VERBOSE( "Channel 0x" << MSG::hex << chID << MSG::dec 
                           << " (Layer = " << layer 
                           << " - Eta = " << eta
                           << " - Phi = " << phi
                           << " - Gain = " << gain
                           << ") passed filtering selection. Now processing...");
	  
	  // Always use the caliwave with higher dac value for each gain.
	  // Predicted waves are fitted to gain 0 physics waves for sampling 0 
	  // and to gain 1 physics waves for sampling 1,2,3
	  int idac=0, physgain=0;
	  if ((layer==0)&&(gain==0)) { idac=5000; physgain=0; } //use intermediate dac for avoiding saturation - was 10000 in emtb
	  if ((layer==1)&&(gain==0)) { idac=500;  physgain=1; }
	  if ((layer==1)&&(gain==1)) { idac=8000; physgain=1; }
	  if ((layer==2)&&(gain==0)) { idac=500;  physgain=1; } //fit seems netter with MG phys data
	  if ((layer==2)&&(gain==1)) { idac=5000; physgain=1; } 
	  if ((layer==3)&&(gain==0)) { idac=500;  physgain=0; }
	  if ((layer==3)&&(gain==1)) { idac=3000; physgain=0; } //no phys data for back in MG
	  
	  ATH_MSG_VERBOSE ( "... will use LArCaliWave with DAC = " << idac << " and LArPhysWave with Gain = " << physgain );
	  
	  unsigned ndac = 0 ;
	  
	  for (;cont_it!=cont_it_e;cont_it++) { // Loop over available DAC wavwsvalues for the given cell
	      
	      const LArCaliWave& larCaliWave = (*cont_it);
	      int dac = larCaliWave.getDAC() ;
	      
	      ATH_MSG_VERBOSE ( ++ndac << ". DAC = " << dac );

	      if ( dac!=idac ) continue; // skip unwanted DAC waves
		
	      ATH_MSG_INFO ( " Processing channel 0x" << MSG::hex << chID << MSG::dec 
                             << " -> Layer = " << layer
                             << " - Eta = " << eta
                             << " - Phi = " << phi
                             << " - Gain = "  << gain
                             << " - LArCaliWave DAC = "   << dac );
	      
	      // get corresponding physcs wave
	      const LArPhysWave& larPhysWave = physWaveContainer->get(chID,physgain);
	      
	      if ( larPhysWave.isEmpty() ) {
	      
                ATH_MSG_ERROR ( "No LArPhysWave found for channel 0x" << MSG::hex << itVec.channelId() << MSG::dec );
	      
	      } else { // LArPhysWave found, perform TCM fit and phys wave prediction from fit params
	      	          
		  //log << MSG::INFO << "Now processing channel " << ++nchannel << " with TCM fit " << endmsg ;  
		  
                ATH_MSG_DEBUG ( "larPhysWave gain = " << gain );
                ATH_MSG_DEBUG ( "larPhysWave size = " << larPhysWave.getSize() );
                ATH_MSG_DEBUG ( "larPhysWave flag = " << larPhysWave.getFlag() );
				  
		  LArPhysWave predlarPhysWave = 
		    larTCMFitterTool->Fit(larCaliWave,larPhysWave); // <---
		  
		  predlarPhysWave.setFlag(LArWave::predFitPhys);
			
		  ATH_MSG_INFO ( " End of TCM fit" );
			
		  //output fit values
		  ATH_MSG_DEBUG("Fit values from larTCMFitterTool:");
		  ATH_MSG_DEBUG(larTCMFitterTool->getTaud()<<" "<<larTCMFitterTool->getTauexp()<<" "<<larTCMFitterTool->getTaur()
                                <<" "<<larTCMFitterTool->getf()<<" "<<larTCMFitterTool->getW0());
		  ATH_MSG_DEBUG(larTCMFitterTool->getcaliStart()<<" "<<larTCMFitterTool->getphysShift());
		  ATH_MSG_DEBUG(larTCMFitterTool->getEmean()<<" "<<larTCMFitterTool->getMphyMcal()<<" "<<larTCMFitterTool->getchi2());
		
		  //Store fit parameters in LArWFParams 
#if 0
		  LArWFParams* larwfparams = new LArWFParams((unsigned int)LArWave::predFitPhys,
							     larTCMFitterTool->getTauexp(),
                                                             larTCMFitterTool->getf(),
                                                             larTCMFitterTool->getTaud(),
							     larTCMFitterTool->getW0(),
                                                             larTCMFitterTool->getTaur(),
                                                             -1.00,
                                                             larTCMFitterTool->getMphyMcal());
#endif
			
		  //Put predicted LArPhysWave in LArPhysWaveContainer
                  larpredPhysWaveContainer->setPdata(itVec.channelId(), predlarPhysWave, gain); // new container

// 		  //Put waveform params in LArWFParamsContainer
// 		  larWFParamsContainer->push_back(larwfparams);	
	      
	          
		  //for debug - dump LArWave to root file	
	          if (m_rootrawdump) {

		  unsigned int N;
		  TH1D* hist;
		  char temp[80];
			
		  N = larPhysWave.getSize();
			
		  hist=new TH1D("hist","hist",N,0,N);
		  for(unsigned int i=0; i<N; ++i) {
		      hist->SetBinContent(i+1,larPhysWave.getSample(i));
		  }
		  memset(temp,0,sizeof(temp));
		  sprintf(temp,"phys%.1u%.1d%.2d%.3d", gain,emId->sampling(id),emId->phi(id),emId->eta(id));
		  hist->SetName(temp);
		  hist->SetTitle(temp);
		  f.cd();
		  hist->Write();
		  delete hist;
			
		  N = larCaliWave.getSize();
		  hist=new TH1D("hist","hist",N,0,N);
		  for(unsigned int i=0; i<N; ++i) {
		      hist->SetBinContent(i+1,larCaliWave.getSample(i));
		  }
		  int Mcal=static_cast<int> (hist->GetMaximum());
		  memset(temp,0,sizeof(temp));
		  sprintf(temp,"cali%.1u%.1d%.2d%.3d", gain,emId->sampling(id),emId->phi(id),emId->eta(id));
		  hist->SetName(temp);
		  hist->SetTitle(temp);
		  f.cd();
		  hist->Write();
		  delete hist;
			
		  N = predlarPhysWave.getSize();   
		  hist=new TH1D("hist","hist",N,0,N);
		  for(unsigned int i=0; i<N; ++i) {
		      hist->SetBinContent(i+1,predlarPhysWave.getSample(i)*Mcal*larTCMFitterTool->getEmean()); //change normalization to match with delaypulse
		      //hist->SetBinContent(i+1,predlarPhysWave.getSample(i)); //same normalisation to what is stored in the DB - the wave is already normalized to MphyMcal value - needed for OFC
		  }
		  memset(temp,0,sizeof(temp));
		  sprintf(temp,"pphys%.1u%.1d%.2d%.3d", gain,emId->sampling(id),emId->phi(id),emId->eta(id));
		  hist->SetName(temp);
		  hist->SetTitle(temp);
		  f.cd();
		  hist->Write();
		  delete hist;
			
		  CellMap[emId->sampling(id)][gain]->SetBinContent(emId->eta(id)+1,emId->phi(id)+1,1.);
			
		  //Fill TTree
		  ts1.iGain = gain;
		  ts1.iLayer = emId->sampling(id);
		  ts1.iPhi = emId->phi(id);
		  ts1.iEta = emId->eta(id);
		  ts1.ipdg = -1;

		  ts1.taud = larTCMFitterTool->getTaud();
		  ts1.tauexp = larTCMFitterTool->getTauexp();	
		  ts1.f = larTCMFitterTool->getf();	
		  ts1.w0 = larTCMFitterTool->getW0();
		  ts1.taur = larTCMFitterTool->getTaur();
		  ts1.SplineShift = larTCMFitterTool->getcaliStart();
		  ts1.top = larTCMFitterTool->getphysShift();
		  ts1.Emean = larTCMFitterTool->getEmean();
		  ts1.chi2 = larTCMFitterTool->getchi2();
		  ts1.MphyMcal = larTCMFitterTool->getMphyMcal();
	
		  ts1.fit_result = -1;

		  ts1.peak_residus = -1;
		  ts1.CAL_nb = -1;
		  ts1.CAL_chan = -1;

		  tree->Fill();
			
	          } //end of rootrawdump
	     
	     } // end if check if LArPhysWave->isEmpty() 
	     
	  } // end loop over DAC values
       
          if ( m_testmode && nchannel >= 1 ) {
            ATH_MSG_DEBUG ( "Test mode selected: only 1 channel processed." );
            break ;
	  }
	  
        } // end loop over cells
      
     } // end loop over gains      

  } // end loop over LArCaliWave containers (keys in m_keylist)
  
  std::string keyout ;
  
  // Record larpredPhysWaveContainer to detector store
  keyout = "LArPhysWavesTCM" ;
  ATH_CHECK( detStore()->record(larpredPhysWaveContainer,keyout) );
  ATH_MSG_INFO ( "LArPhysWaveContainer has been recorded to StoreGate with key="<<keyout);

  // Record larWFParamsContainer to DetectorStore
//   keyout = "LArWFParamsTCM" ;
//   sc=detStore->record(larWFParamsContainer,keyout );
//   if (sc.isFailure()) {
//       log << MSG::FATAL << "Cannot record LArWFParamsContainer to StoreGate! key=" << keyout << endmsg;
//       return StatusCode::FAILURE;
//   } else log << MSG::INFO << "LArWFParamsContainer has been recorded to StoreGate with key="<<keyout<<endmsg;

  if (m_rootrawdump) {
      for(Int_t layer=0;layer<nlayer;layer++){
	  for(Int_t gain=0;gain<ngain;gain++) {
	      CellMap[layer][gain]->Write();
	      //NentriesMap[layer][gain]->Write();
	  }
      }
      tree->Write();	
  }
  
  return StatusCode::SUCCESS;
}
