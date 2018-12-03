/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArPulseShape.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArCondUtils/LArFEBTempTool.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArCOOLConditions/LArDSPConfig.h"
#include "CaloIdentifier/CaloGain.h"
#include "xAODEventInfo/EventInfo.h"
#include "TFile.h"
#include "TKey.h"
#include "TTree.h"
#include "TSystem.h"
#include "TSystem.h"

LArPulseShape::LArPulseShape(const std::string& name, ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator), m_initialized(false),  m_calo_id(0),  
  m_lumiTool("LuminosityTool"), m_nt(NULL) 
{
  declareProperty("NtupleTitle",m_ntTitle="Pulse shape");
  m_ntpath="/NTUPLES/FILE1/"+m_ntname; 
}

LArPulseShape::~LArPulseShape() {
}

StatusCode LArPulseShape::initialize() {
  m_detStore= &(*detStore()); //for backward compatiblity 
  ATH_CHECK( detStore()->retrieve( m_caloIdMgr ) );
  m_calo_id      = m_caloIdMgr->getCaloCell_ID();

  ATH_CHECK( m_lumiTool.retrieve() );
  
  ATH_CHECK( m_larCablingSvc.retrieve() ); 

  ATH_CHECK( m_trigDec.retrieve() );

  ATH_MSG_DEBUG("Initializing LArPulseShape base class - finished");     
  if (m_ntpath.size()==0 || m_ntTitle.size()==0) {
    ATH_MSG_ERROR( "Need to set variable 'm_ntpath' and 'm_ntTitle' in constructor of deriving class!" );
    return StatusCode::FAILURE;
  }

  size_t i=m_ntpath.rfind('/');
  if (i==std::string::npos) {
    ATH_MSG_ERROR( "Expected at least on '/' in path " << m_ntpath );
    return StatusCode::FAILURE;
  }
  std::string basepath(m_ntpath.begin(),m_ntpath.begin()+i);
  std::cout << "Basepath" << basepath << std::endl;

  NTupleFilePtr file1(ntupleSvc(),basepath);
  if (!file1){
    ATH_MSG_ERROR( "Could not get NTupleFilePtr with path " << basepath << " failed" );
    return StatusCode::FAILURE;
  }
  NTuplePtr nt(ntupleSvc(),m_ntpath);
  if (!nt) {
    nt=ntupleSvc()->book(m_ntpath,CLID_ColumnWiseTuple,m_ntTitle);
  }
  if (!nt){
    ATH_MSG_ERROR( "Booking of NTuple at "<< m_ntpath << " and name " << m_ntTitle << " failed" );
    return StatusCode::FAILURE; 
  }
  
  m_nt=nt;

  int ncell=m_calo_id->calo_cell_hash_max();
 
  for (int i=0;i<ncell;i++) { // loop over cells to book profiles
  
    IdentifierHash idHash=i;
    Identifier id=m_calo_id->cell_id(idHash);

    const int sampl = m_calo_id->sampling( id );
    m_OffId = (int)(id.get_identifier32().get_compact());
    std::string idoff_str = std::to_string(m_OffId);
    const char * id_char = (idoff_str).c_str();   
   
    if (m_calo_id->is_em(id)) {    // EM calo
      if (m_calo_id->is_em_barrel(id)) { //EMB
        if (m_calo_id->pos_neg(id) > 0 ) { //A-side
	
	  SmartDataPtr<NTuple::Directory> dir_EMBA(ntupleSvc(),"/NTUPLES/FILE1/EMBA");
   	  if ( !dir_EMBA ) dir_EMBA = ntupleSvc()->createDirectory(file1,"EMBA");
          if ( !dir_EMBA ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
                 
          if (sampl==0){ // if it's EMBA Presampler
            SmartDataPtr<NTuple::Directory> dir_EMBA_0(ntupleSvc(),"/NTUPLES/FILE1/EMBA/Presampler");
  	    if ( !dir_EMBA_0 ) dir_EMBA_0 = ntupleSvc()->createDirectory(file1,"EMBA/Presampler");
            if ( !dir_EMBA_0 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
	    m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));
  	  }
	  
	  if (sampl==1){ // if it's EMBA Sampling1
  	    SmartDataPtr<NTuple::Directory> dir_EMBA_1(ntupleSvc(),"/NTUPLES/FILE1/EMBA/Sampling1");
  	    if ( !dir_EMBA_1 ) dir_EMBA_1 = ntupleSvc()->createDirectory(file1,"EMBA/Sampling1");
  	    if ( !dir_EMBA_1 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
  	    m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));			    
  	  }	     
	  
	  if (sampl==2){ // if it's EMBA Sampling2
  	    SmartDataPtr<NTuple::Directory> dir_EMBA_2(ntupleSvc(),"/NTUPLES/FILE1/EMBA/Sampling2");
  	    if ( !dir_EMBA_2 ) dir_EMBA_2 = ntupleSvc()->createDirectory(file1,"EMBA/Sampling2");
  	    if ( !dir_EMBA_2 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
 	    m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));			    
  	  }
	  
	  if (sampl==3){ // if it's EMBA Sampling3
  	    SmartDataPtr<NTuple::Directory> dir_EMBA_3(ntupleSvc(),"/NTUPLES/FILE1/EMBA/Sampling3");
  	    if ( !dir_EMBA_3 ) dir_EMBA_3 = ntupleSvc()->createDirectory(file1,"EMBA/Sampling3");
  	    if ( !dir_EMBA_3 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
	    m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));			    
  	  }
	  		  
        } else {  // C-side
	
	  SmartDataPtr<NTuple::Directory> dir_EMBC(ntupleSvc(),"/NTUPLES/FILE1/EMBC");
   	  if ( !dir_EMBC ) dir_EMBC = ntupleSvc()->createDirectory(file1,"EMBC");
   	  if ( !dir_EMBC ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          if (sampl==0){ // if it's EMBC Presampler
            SmartDataPtr<NTuple::Directory> dir_EMBC_0(ntupleSvc(),"/NTUPLES/FILE1/EMBC/Presampler");
  	    if ( !dir_EMBC_0 ) dir_EMBC_0 = ntupleSvc()->createDirectory(file1,"EMBC/Presampler");
            if ( !dir_EMBC_0 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
            m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));
  	  }
	  
	  if (sampl==1){ // if it's EMBC Sampling1
  	    SmartDataPtr<NTuple::Directory> dir_EMBC_1(ntupleSvc(),"/NTUPLES/FILE1/EMBC/Sampling1");
  	    if ( !dir_EMBC_1 ) dir_EMBC_1 = ntupleSvc()->createDirectory(file1,"EMBC/Sampling1");
  	    if ( !dir_EMBC_1 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
  	    m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));			    
  	  }	     
	  
	  if (sampl==2){ // if it's EMBC Sampling2
  	    SmartDataPtr<NTuple::Directory> dir_EMBC_2(ntupleSvc(),"/NTUPLES/FILE1/EMBC/Sampling2");
  	    if ( !dir_EMBC_2 ) dir_EMBC_2 = ntupleSvc()->createDirectory(file1,"EMBC/Sampling2");
  	    if ( !dir_EMBC_2 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
 	    m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));			    
  	  }
	  
	  if (sampl==3){ // if it's EMBC Sampling3
  	    SmartDataPtr<NTuple::Directory> dir_EMBC_3(ntupleSvc(),"/NTUPLES/FILE1/EMBC/Sampling3");
  	    if ( !dir_EMBC_3 ) dir_EMBC_3 = ntupleSvc()->createDirectory(file1,"EMBC/Sampling3");
  	    if ( !dir_EMBC_3 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
	    m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));			    
  	  }
	  		     
	} 
	
      } // end EMB
      
      if( m_calo_id->is_em_endcap(id) ){ //EMEC
      
        if (m_calo_id->pos_neg(id) > 0 ) { //A-side
	
	  SmartDataPtr<NTuple::Directory> dir_EMECA(ntupleSvc(),"/NTUPLES/FILE1/EMECA");
   	  if ( !dir_EMECA ) dir_EMECA = ntupleSvc()->createDirectory(file1,"EMECA");
          if ( !dir_EMECA ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
                 
          if (sampl==0){ // if it's EMECA Presampler
            SmartDataPtr<NTuple::Directory> dir_EMECA_0(ntupleSvc(),"/NTUPLES/FILE1/EMECA/Presampler");
  	    if ( !dir_EMECA_0 ) dir_EMECA_0 = ntupleSvc()->createDirectory(file1,"EMECA/Presampler");
            if ( !dir_EMECA_0 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
   
            //stopper++;
	    //if (stopper<=10) 
	    m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));
  	  }
	  
	  if (sampl==1){ // if it's EMECA Sampling1
  	    SmartDataPtr<NTuple::Directory> dir_EMECA_1(ntupleSvc(),"/NTUPLES/FILE1/EMECA/Sampling1");
  	    if ( !dir_EMECA_1 ) dir_EMECA_1 = ntupleSvc()->createDirectory(file1,"EMECA/Sampling1");
  	    if ( !dir_EMECA_1 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
  	    m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));			    
  	  }	     
	  
	  if (sampl==2){ // if it's EMECA Sampling2
  	    SmartDataPtr<NTuple::Directory> dir_EMECA_2(ntupleSvc(),"/NTUPLES/FILE1/EMECA/Sampling2");
  	    if ( !dir_EMECA_2 ) dir_EMECA_2 = ntupleSvc()->createDirectory(file1,"EMECA/Sampling2");
  	    if ( !dir_EMECA_2 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
 	    m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));			    
  	  }
	  
	  if (sampl==3){ // if it's EMECA Sampling3
  	    SmartDataPtr<NTuple::Directory> dir_EMECA_3(ntupleSvc(),"/NTUPLES/FILE1/EMECA/Sampling3");
  	    if ( !dir_EMECA_3 ) dir_EMECA_3 = ntupleSvc()->createDirectory(file1,"EMECA/Sampling3");
  	    if ( !dir_EMECA_3 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
	    m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));			    
  	  }
	  		  
        } else {  // C-side
	
	  SmartDataPtr<NTuple::Directory> dir_EMECC(ntupleSvc(),"/NTUPLES/FILE1/EMECC");
   	  if ( !dir_EMECC ) dir_EMECC = ntupleSvc()->createDirectory(file1,"EMECC");
   	  if ( !dir_EMECC ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          if (sampl==0){ // if it's EMECC Presampler
            SmartDataPtr<NTuple::Directory> dir_EMECC_0(ntupleSvc(),"/NTUPLES/FILE1/EMECC/Presampler");
  	    if ( !dir_EMECC_0 ) dir_EMECC_0 = ntupleSvc()->createDirectory(file1,"EMECC/Presampler");
            if ( !dir_EMECC_0 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
            m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));
  	  }
	  
	  if (sampl==1){ // if it's EMECC Sampling1
  	    SmartDataPtr<NTuple::Directory> dir_EMECC_1(ntupleSvc(),"/NTUPLES/FILE1/EMECC/Sampling1");
  	    if ( !dir_EMECC_1 ) dir_EMECC_1 = ntupleSvc()->createDirectory(file1,"EMECC/Sampling1");
  	    if ( !dir_EMECC_1 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
  	    m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));			    
  	  }	     
	  
	  if (sampl==2){ // if it's EMECC Sampling2
  	    SmartDataPtr<NTuple::Directory> dir_EMECC_2(ntupleSvc(),"/NTUPLES/FILE1/EMECC/Sampling2");
  	    if ( !dir_EMECC_2 ) dir_EMECC_2 = ntupleSvc()->createDirectory(file1,"EMECC/Sampling2");
  	    if ( !dir_EMECC_2 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
 	    m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));			    
  	  }
	  
	  if (sampl==3){ // if it's EMECC Sampling3
  	    SmartDataPtr<NTuple::Directory> dir_EMECC_3(ntupleSvc(),"/NTUPLES/FILE1/EMECC/Sampling3");
  	    if ( !dir_EMECC_3 ) dir_EMECC_3 = ntupleSvc()->createDirectory(file1,"EMECC/Sampling3");
  	    if ( !dir_EMECC_3 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
	    m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));			    
  	  }
	  		     
	}       
      } // end EMEC
      
	  
	        
    } // end EM calo
    
    if( m_calo_id->is_hec(id) ){ //HEC
    
      if (m_calo_id->pos_neg(id) > 0 ) { //A-side

        SmartDataPtr<NTuple::Directory> dir_HECA(ntupleSvc(),"/NTUPLES/FILE1/HECA");
        if ( !dir_HECA ) dir_HECA = ntupleSvc()->createDirectory(file1,"HECA");
    	if ( !dir_HECA ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
    	       
    	if (sampl==0){
    	  SmartDataPtr<NTuple::Directory> dir_HECA_0(ntupleSvc(),"/NTUPLES/FILE1/HECA/Layer1");
          if ( !dir_HECA_0 ) dir_HECA_0 = ntupleSvc()->createDirectory(file1,"HECA/Layer1");
    	  if ( !dir_HECA_0 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));
        }
        
        if (sampl==1){
          SmartDataPtr<NTuple::Directory> dir_HECA_1(ntupleSvc(),"/NTUPLES/FILE1/HECA/Layer2");
          if ( !dir_HECA_1 ) dir_HECA_1 = ntupleSvc()->createDirectory(file1,"HECA/Layer2");
          if ( !dir_HECA_1 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" ))); 			  
        }	   
        
        if (sampl==2){
          SmartDataPtr<NTuple::Directory> dir_HECA_2(ntupleSvc(),"/NTUPLES/FILE1/HECA/Layer3");
          if ( !dir_HECA_2 ) dir_HECA_2 = ntupleSvc()->createDirectory(file1,"HECA/Layer3");
          if ( !dir_HECA_2 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" ))); 			  
        }
        
        if (sampl==3){
          SmartDataPtr<NTuple::Directory> dir_HECA_3(ntupleSvc(),"/NTUPLES/FILE1/HECA/Layer4");
          if ( !dir_HECA_3 ) dir_HECA_3 = ntupleSvc()->createDirectory(file1,"HECA/Layer4");
          if ( !dir_HECA_3 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" ))); 			  
        }
        		
      } else {  // C-side

        SmartDataPtr<NTuple::Directory> dir_HECC(ntupleSvc(),"/NTUPLES/FILE1/HECC");
        if ( !dir_HECC ) dir_HECC = ntupleSvc()->createDirectory(file1,"HECC");
        if ( !dir_HECC ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
    	if (sampl==0){ 
    	  SmartDataPtr<NTuple::Directory> dir_HECC_0(ntupleSvc(),"/NTUPLES/FILE1/HECC/Layer1");
          if ( !dir_HECC_0 ) dir_HECC_0 = ntupleSvc()->createDirectory(file1,"HECC/Layer1");
    	  if ( !dir_HECC_0 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
    	  m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" )));
        }
        
        if (sampl==1){
          SmartDataPtr<NTuple::Directory> dir_HECC_1(ntupleSvc(),"/NTUPLES/FILE1/HECC/Layer2");
          if ( !dir_HECC_1 ) dir_HECC_1 = ntupleSvc()->createDirectory(file1,"HECC/Layer2");
          if ( !dir_HECC_1 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" ))); 			  
        }	   
        
        if (sampl==2){ 
          SmartDataPtr<NTuple::Directory> dir_HECC_2(ntupleSvc(),"/NTUPLES/FILE1/HECC/Layer3");
          if ( !dir_HECC_2 ) dir_HECC_2 = ntupleSvc()->createDirectory(file1,"HECC/Layer3");
          if ( !dir_HECC_2 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" ))); 			  
        }
        
        if (sampl==3){ 
          SmartDataPtr<NTuple::Directory> dir_HECC_3(ntupleSvc(),"/NTUPLES/FILE1/HECC/Layer4");
          if ( !dir_HECC_3 ) dir_HECC_3 = ntupleSvc()->createDirectory(file1,"HECC/Layer4");
          if ( !dir_HECC_3 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" ))); 			  
        }
        		   
      }       
    } // end HEC      
    if( m_calo_id->is_fcal(id) ){ //FCAL
    
      if (m_calo_id->pos_neg(id) > 0 ) { //A-side

        SmartDataPtr<NTuple::Directory> dir_FCALA(ntupleSvc(),"/NTUPLES/FILE1/FCALA");
        if ( !dir_FCALA ) dir_FCALA = ntupleSvc()->createDirectory(file1,"FCALA");
    	if ( !dir_FCALA ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
        
        if (sampl==1){ // if it's FCALA Sampling1
          SmartDataPtr<NTuple::Directory> dir_FCALA_1(ntupleSvc(),"/NTUPLES/FILE1/FCALA/Sampling1");
          if ( !dir_FCALA_1 ) dir_FCALA_1 = ntupleSvc()->createDirectory(file1,"FCALA/Sampling1");
          if ( !dir_FCALA_1 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" ))); 			  
        }	   
        
        if (sampl==2){ // if it's FCALA Sampling2
          SmartDataPtr<NTuple::Directory> dir_FCALA_2(ntupleSvc(),"/NTUPLES/FILE1/FCALA/Sampling2");
          if ( !dir_FCALA_2 ) dir_FCALA_2 = ntupleSvc()->createDirectory(file1,"FCALA/Sampling2");
          if ( !dir_FCALA_2 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" ))); 			  
        }
        
        if (sampl==3){ // if it's FCALA Sampling3
          SmartDataPtr<NTuple::Directory> dir_FCALA_3(ntupleSvc(),"/NTUPLES/FILE1/FCALA/Sampling3");
          if ( !dir_FCALA_3 ) dir_FCALA_3 = ntupleSvc()->createDirectory(file1,"FCALA/Sampling3");
          if ( !dir_FCALA_3 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" ))); 			  
        }
        	
      } else {  // C-side

        SmartDataPtr<NTuple::Directory> dir_FCALC(ntupleSvc(),"/NTUPLES/FILE1/FCALC");
        if ( !dir_FCALC ) dir_FCALC = ntupleSvc()->createDirectory(file1,"FCALC");
        if ( !dir_FCALC ) ATH_MSG_ERROR ( " failed to get ntuple directory" );

        if (sampl==1){ // if it's FCALC Sampling1
          SmartDataPtr<NTuple::Directory> dir_FCALC_1(ntupleSvc(),"/NTUPLES/FILE1/FCALC/Sampling1");
          if ( !dir_FCALC_1 ) dir_FCALC_1 = ntupleSvc()->createDirectory(file1,"FCALC/Sampling1");
          if ( !dir_FCALC_1 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" ))); 			  
        }	   
        
        if (sampl==2){ // if it's FCALC Sampling2
          SmartDataPtr<NTuple::Directory> dir_FCALC_2(ntupleSvc(),"/NTUPLES/FILE1/FCALC/Sampling2");
          if ( !dir_FCALC_2 ) dir_FCALC_2 = ntupleSvc()->createDirectory(file1,"FCALC/Sampling2");
          if ( !dir_FCALC_2 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" ))); 			  
        }
        
        if (sampl==3){ // if it's FCALC Sampling3
          SmartDataPtr<NTuple::Directory> dir_FCALC_3(ntupleSvc(),"/NTUPLES/FILE1/FCALC/Sampling3");
          if ( !dir_FCALC_3 ) dir_FCALC_3 = ntupleSvc()->createDirectory(file1,"FCALC/Sampling3");
          if ( !dir_FCALC_3 ) ATH_MSG_ERROR ( " failed to get ntuple directory" );
          m_cellHistos.insert(std::make_pair(m_OffId, new TProfile(id_char, id_char, 40, -4 , 36, "s" ))); 			  
        }
      }     
    } // end FCAL     
    
  } // loop over cells

  SmartDataPtr<NTuple::Directory> dir_FCALC_3(ntupleSvc(),"/NTUPLES/FILE1/Check");
  if ( !dir_FCALC_3 ) dir_FCALC_3 = ntupleSvc()->createDirectory(file1,"Check");   
   
  TProfpulse_diff = new TProfile("test_diff", "test_diff",500, 0, 500, "s");  
   
  h_BCID = new TH1D("BCID", "BCID",3564, 0, 3564);    
  h_diffBCID = new TH1D("diffBCID", "BCID",3564, 0, 3564);     
  h_actualInt = new TH1D("ActualInt", "ActInt", 60, 0, 20); 
  
  addHistogram(TProfpulse_diff);
  
  addHistogram(h_BCID);
  addHistogram(h_diffBCID);  
  addHistogram(h_actualInt);   
  m_initialized=true;
  return StatusCode::SUCCESS;
}
/////////////////////////////////////////////////////////////////////
StatusCode LArPulseShape::execute() {


  short int bunchStr[8] = {1, 101, 201, 301, 1786, 1886, 1986, 2086}; //move to JO
  
  const xAOD::EventInfo* eventInfo = 0;
  ATH_CHECK( evtStore()->retrieve(eventInfo) );
  
  std::vector<std::string> chains = m_trigDec->getListOfTriggers();
  std::vector<std::string> myChains;
  
  if (m_trigDec->isPassed("L1_RD1_BGRP10")) {   

    int bunchId   = eventInfo->bcid();
    
    m_mindist = 3564;
    m_closestBC = 0;
    for (i = 0; i < 8; i++) {
      if ( (fabs(bunchId - bunchStr[i]) < m_mindist) || (fabs(bunchId - (bunchStr[i]+3564)) < m_mindist)) {
   	m_mindist = std::min(fabs(bunchId - bunchStr[i]),(fabs(bunchId - (bunchStr[i]+3564))));
   	m_closestBC = bunchStr[i];
      }
    }
    if (fabs(bunchId - m_closestBC)  < (fabs(bunchId - (m_closestBC+3564)))) {
      m_mindist = bunchId - m_closestBC; 
    } else {
      m_mindist = bunchId - (m_closestBC+3564);
    }  

    //std::cout << "bunchID = " << bunchId<< ", m_mindist = " << m_mindist << ", m_closestBC = " << m_closestBC << std::endl;
    h_BCID->Fill(eventInfo->bcid());
    h_diffBCID->Fill(m_mindist);  

    h_actualInt->Fill(m_lumiTool->lbLuminosityPerBCID(m_closestBC));
    
    const LArDigitContainer* larDigitContainer;
    ATH_CHECK( evtStore()->retrieve(larDigitContainer, "FREE") );
    ATH_CHECK( detStore()->retrieve(m_larPedestal) );
    if (larDigitContainer->size() == 0) {
      ATH_MSG_WARNING ( "LArDigitContainer with key= is empty!" );

      return StatusCode::SUCCESS;
    }

    for (LArDigitContainer::const_iterator digit = larDigitContainer->begin(); digit != larDigitContainer->end(); ++digit) {
    
      HWIdentifier channelID = (*digit)->hardwareID();  
      
      if (m_larCablingSvc->isOnlineConnected(channelID)) {
     
   	CaloGain::CaloGain gain=(*digit)->gain();

   	float pedestal = m_larPedestal->pedestal(channelID, gain); 

   	m_OffId_conv = (int)(m_larCablingSvc->cnvToIdentifier(channelID).get_identifier32().get_compact());
      
   	const std::vector<short>& samples = (*digit)->samples();
   	
   	isample=0;
   	for (std::vector<short>::const_iterator sample = samples.begin(); sample != samples.end(); sample++) {
	
          isample++; 
   	  if (m_lumiTool->lbLuminosityPerBCID(m_closestBC)!=0){
   	    TProfpulse_diff->Fill(m_mindist+isample, (*sample-pedestal)/m_lumiTool->lbLuminosityPerBCID(m_closestBC));//+0,1,2,3 
   	    m_cellHistos[m_OffId_conv]->Fill(m_mindist+isample, (*sample-pedestal)/m_lumiTool->lbLuminosityPerBCID(m_closestBC));    
          }
   	} //loop over samples
      }
    }  
  
  }//trigger
  return StatusCode::SUCCESS;  
}

/////////////////////////////////////////////////////////////////////
StatusCode LArPulseShape::stop() {

  ATH_MSG_INFO( "LArPulseShape has finished." );
  return StatusCode::SUCCESS;
   
}// end finalize-method.
   
