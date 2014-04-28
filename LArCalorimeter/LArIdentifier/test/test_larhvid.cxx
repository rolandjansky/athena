/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /build/atlas/cvs/atlas/offline/LArCalorimeter/LArIdentifier/test/test_larhvid.cxx,v 1.6 2008-05-30 14:46:45 lucotte Exp $ 
  
#include "IdDictParser/IdDictParser.h"  
#include "Identifier/IdentifierHash.h" 
#include "LArIdentifier/LArHVLineID.h"
#include "LArIdentifier/LArElectrodeID.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream> 
#include <fstream> 
 
static void check_lar_highvoltage_id_decoding(IdDictMgr& idd)
/*========================================================*/
{    
  std::cout << " Entering Check_LarHighVoltageID_decoding" << std::endl;
  LArHVLineID larhv_id;
  if( larhv_id.initialize_from_dictionary(idd)){
    std::cout << "check_lar_highvoltage_id_decoding - cannot init from dict " << std::endl;
  }

  const IdDictDictionary* dictionary = idd.find_dictionary ("LArHighVoltage"); 
  if(!dictionary) {
    std::cout << "[TEST_LARHVLINE] [0] Cannot access LArCalorimeter dictionary "
	      << std::endl;
  }
  else{
    std::cout << "[TEST_LARHVLINE] [0] Got LArHighVoltage dictionary" << std::endl;
  }
  const LArHVLineID* p_larhvId = &larhv_id;

  /* Test OF HVline HWIdentifiers */
  std::vector<HWIdentifier>::const_iterator itFT = p_larhvId->hvline_begin();
  std::vector<HWIdentifier>::const_iterator itFTEnd = p_larhvId->hvline_end();    
  std::cout << "[TEST_LARHVLINE] [1] HV Line Loop : " << itFTEnd-itFT << std::endl;
  int nEMB   = 0;
  int nEMBA   = 0;
  int nEMBC   = 0;
  int nEMBPS = 0;

  int nEMEC  = 0;
  int nEMECA = 0;
  int nEMECC = 0;

  int nEMECPS= 0;
  int nFCAL  = 0;
  int nFCALA  = 0;
  int nFCALC  = 0;

  //int nEMBPUR = 0;
  //int nECPUR  = 0;

  int nHEC   =0;
  int nHECA   =0;
  int nHECC   =0;

  int n_hvline=0;
  //bool error = false;

  std::fstream fileHEC;
  fileHEC.open("TEST_HV_HEC.txt",std::ios::out);
  std::fstream fileFCAL;
  fileFCAL.open("TEST_HV_FCAL.txt",std::ios::out);
  std::fstream fileEMB;
  fileEMB.open("TEST_HV_EMB.txt",std::ios::out);
  std::fstream fileEMEC;
  fileEMEC.open("TEST_HV_EMEC.txt",std::ios::out);
  std::fstream fileEMECPS;
  fileEMECPS.open("TEST_HV_EMECPS.txt",std::ios::out);
  std::fstream fileEMBPS;
  fileEMBPS.open("TEST_HV_EMBPS.txt",std::ios::out);

  for(; itFT!=itFTEnd;++itFT)
    {// Start loop over hvlines
      HWIdentifier hvId = *itFT;      
      n_hvline++;
      /*
	int PART= p_larhvId->partition( hvId );
	int CANL= p_larhvId->can_line( hvId );
	int CANN= p_larhvId->can_node( hvId );
	int LINE= p_larhvId->hv_line( hvId );
	if( p_larhvId->isHEC( hvId ) ){ 
	nHEC++;
	fileHEC << nHEC << " hvlineID=[" << PART << "," << CANL << "," << CANN << "," << LINE << "]" << std::endl;
	}
	if( p_larhvId->isHECA( hvId ) ){ 
	nHECA++;
	}
	if( p_larhvId->isHECC( hvId ) ){ 
	nHECC++;
	}
	if( p_larhvId->isFCAL( hvId ) ){ 
	nFCAL++;
	fileFCAL << nFCAL << " hvlineID=[" << PART << "," << CANL << "," << CANN << "," << LINE << "]" << std::endl;
	}
	if( p_larhvId->isFCALA( hvId ) ){ 
	nFCALA++;
	}
	if( p_larhvId->isFCALC( hvId ) ){ 
	nFCALC++;
	}      
	if( p_larhvId->isEMEC( hvId ) ){ 
	nEMEC++;
	fileEMEC << nEMEC << " hvlineID=[" << PART << "," << CANL << "," << CANN << "," << LINE << "]" << std::endl;
	}
	if( p_larhvId->isEMECA( hvId ) ){ 
	nEMECA++;
	}
	if( p_larhvId->isEMECC( hvId ) ){ 
	nEMECC++;
	}
	if( p_larhvId->isEMECPS( hvId ) ){ 
	nEMECPS++;
	fileEMECPS << nEMECPS << " hvlineID=[" << PART << "," << CANL << "," << CANN << "," << LINE << "]" << std::endl;
	}
      if( p_larhvId->isEMB( hvId ) ){ 
	nEMB++;
	fileEMB << nEMB << " hvlineID=[" << PART << "," << CANL << "," << CANN << "," << LINE << "]" << std::endl;
      }
      if( p_larhvId->isEMBA( hvId ) ){ 
	nEMBA++;
      }
      if( p_larhvId->isEMBC( hvId ) ){ 
	nEMBC++;
      }
      if( p_larhvId->isEMBPS( hvId ) ){ 
	nEMBPS++;
	fileEMBPS << nEMBPS << " hvlineID=[" << PART << "," << CANL << "," << CANN << "," << LINE << "]" << std::endl;
      }
      */
      IdentifierHash hashId = p_larhvId->hvlineHash( hvId );
      HWIdentifier   hvId2  = p_larhvId->HVLineId( hashId );
      if ( hvId2 != hvId ) {
	std::cout
	  << " HvId2 incorrect: " <<  p_larhvId->show_to_string(hvId2) 
	  << " should be: " <<  p_larhvId->show_to_string(hvId) 
	  << " hashId: " << hashId
	  << std::endl;
	//error = true;
      }
    }
  int nDET = nFCAL+nHEC+nEMB+nEMEC+nEMBPS+nEMECPS;

  fileHEC.close();
  fileFCAL.close();
  fileEMB.close();
  fileEMEC.close();
  fileEMECPS.close();
  fileEMBPS.close();




  std::cout << "============================================================================ " << std::endl;
  std::cout << "  total number HVline " << n_hvline << ", hvline_hash_max= " 
	    << p_larhvId->hvlineHashMax() << std::endl;
  std::cout << "  -- EMB      : " << nEMB  << " (ref: 1008)" << std::endl;
  std::cout << "    - A-side  :  " << nEMBA  << " (ref: 488)" << std::endl;
  std::cout << "    - C-side  :  " << nEMBC  << " (ref: 520)" << std::endl;
  std::cout << "  -- EMBPS    :  " << nEMBPS << " (ref:  512)" << std::endl;
  std::cout << "  -- EMECPS   : " << nEMECPS << " (ref:  128)" << std::endl;
  std::cout << "  -- EMEC     : " << nEMEC << " (ref: 1493)" << std::endl; 
  std::cout << "    - A-side  :  " << nEMECA << " (ref:  754)" << std::endl; 
  std::cout << "    - C-side  :  " << nEMECC << " (ref:  739)" << std::endl; 
  std::cout << "  -- FCAL     :  " << nFCAL << " (ref:  224)" << std::endl;
  std::cout << "    - A-side  :  " << nFCALA  << " (ref: xxx)" << std::endl;
  std::cout << "    - C-side  :  " << nFCALC  << " (ref: xxx)" << std::endl;
  std::cout << "  -- HEC      :  " << nHEC  << " (ref: 1024)" << std::endl;	    
  std::cout << "    - A-side  :  " << nHECA  << " (ref: 512)" << std::endl;	    
  std::cout << "    - C-side  :  " << nHECC  << " (ref: 512)" << std::endl;	    
  //std::cout << "  -- EMBPUR   :  " << nEMBPUR << " (ref: 20)" << std::endl;	    
  //std::cout << "  -- ECPUR    :  " << nECPUR << " (ref: 40)" << std::endl;	    
  std::cout << "============================================================================ " << std::endl;
  std::cout << "  -- TOTAL    : " << nDET << std::endl; 
  std::cout << "============================================================================ " << std::endl; 
}




//==================================================================
// check for LArElectrodeID 
//================================================================= 
//
// AL on AUG-26-2006
//
//==================================================================
static void check_larElectrode_decoding(IdDictMgr& idd)
//==================================================================
{    
  std::cout << " Entering Check_LarElectrodeID_decoding" << std::endl;
  LArElectrodeID larElectrode;
  if( larElectrode.initialize_from_dictionary(idd)){
    std::cout << "[TEST_LARELECTRODE] check_lar_electrode_id_decoding - cannot init from dict " << std::endl;
  }

  const IdDictDictionary* dictionary = idd.find_dictionary ("LArElectrode"); 
  if(!dictionary) {
    std::cout << "[TEST_LARELECTRODE] Cannot access LArCalorimeter dictionary "
	      << std::endl;
  }
  else{
    std::cout << "[TEST_LARELECTRODE] Got LArElectrode dictionary" << std::endl;
  }
  const LArElectrodeID* m_electrodeHelper = &larElectrode;
  
  // Test OF Electrode HWIdentifiers 
  std::vector<HWIdentifier>::const_iterator itFT = m_electrodeHelper->electrode_begin();
  std::vector<HWIdentifier>::const_iterator itFTEnd = m_electrodeHelper->electrode_end();    
  std::cout << "[TEST_LARELECTRODE] HV Line Loop : " << itFTEnd-itFT << std::endl;
  int nEMB   = 0;
  int nEMBPS = 0;

  int nEMEC   = 0;
  int nEMECin = 0;
  int nEMECout= 0;

  //int nEMBPUR = 0;
  //int nECPUR  = 0;

  int nEMECPS= 0;
  int nFCAL  = 0;
  int nFCAL1 = 0;
  int nFCAL2 = 0;
  int nFCAL3 = 0;

  int nHEC   =0;

  int n_electrode=0;
  //bool error = false;
  for(; itFT!=itFTEnd;++itFT)
    {// Start loop over electrodes
      HWIdentifier elecId = *itFT;      
      n_electrode++;
      if( m_electrodeHelper->isHEC( elecId ) ){ 
	nHEC++;
	//	std::cout << "HV line # " << nHEC << " =[" << cannode << "," << line << "]" << std::endl;
	//std::cout << "HV line " << m_electrodeHelper->show_to_string(elecId) << std::endl;
      }
      if( m_electrodeHelper->isFCAL( elecId ) ){ nFCAL++;}
      if( m_electrodeHelper->isFCAL1( elecId ) ){ nFCAL1++;}
      if( m_electrodeHelper->isFCAL2( elecId ) ){ nFCAL2++;}
      if( m_electrodeHelper->isFCAL3( elecId ) ){ nFCAL3++;}
      if( m_electrodeHelper->isEMEC( elecId ) ){ nEMEC++;}
      if( m_electrodeHelper->isEMECinWHEEL( elecId ) ){ nEMECin++;}
      if( m_electrodeHelper->isEMECoutWHEEL( elecId ) ){ nEMECout++;}
      if( m_electrodeHelper->isEMB( elecId ) ){ nEMB++;}
      if( m_electrodeHelper->isEMBPS( elecId ) ){ nEMBPS++;}
      if( m_electrodeHelper->isEMECPS( elecId ) ){ nEMECPS++;}
      //if( m_electrodeHelper->isEMBPUR( elecId ) ){ nEMBPUR++;}
      //if( m_electrodeHelper->isECPUR( elecId ) ){ nECPUR++;}
      IdentifierHash hashId = m_electrodeHelper->electrodeHash( elecId );
      HWIdentifier   elecId2  = m_electrodeHelper->ElectrodeId( hashId );
      if ( elecId2 != elecId ) {
	std::cout
	  << " HvId2 incorrect: " <<  m_electrodeHelper->show_to_string(elecId2) 
	  << " should be: " <<  m_electrodeHelper->show_to_string(elecId) 
	  << " hashId: " << hashId
	  << std::endl;
	//error = true;
      }
    }
  int nDET = nFCAL+nHEC+nEMB+nEMEC+nEMBPS+nEMECPS;
  std::cout << "============================================================================ " << std::endl;
  std::cout << "  total number Electrode " << n_electrode << ", electrode_hash_max= " 
	    << m_electrodeHelper->electrodeHashMax() << std::endl;
  std::cout << "  -- EMB      : " << nEMB  << " (ref: 28672 for 1008 HV lines)" << std::endl;
  std::cout << "  -- EMEC     : " << nEMEC << " (ref: 23552 for 1493 HV lines)" << std::endl; 
  std::cout << "    - inWheel :  " << nEMECin << " (ref:  2048)" << std::endl; 
  std::cout << "    - outWheel: " << nEMECout << " (ref: 21504)" << std::endl; 
  std::cout << "  -- HEC      : " << nHEC  << " (ref: 10240 for 1024 HV lines)" << std::endl;	     
  std::cout << "  -- FCAL     :   " << nFCAL << " (ref:  384 for 224 HV lines)" << std::endl;
  std::cout << "     FCAL1    :  " << nFCAL1<< " (ref:  384 for 224 HV lines)" << std::endl;
  std::cout << "     FCAL2    :  " << nFCAL2<< " (ref:  288 for 224 HV lines)" << std::endl;
  std::cout << "     FCAL3    :  " << nFCAL3<< " (ref:  384 for 224 HV lines)" << std::endl;
  std::cout << "  -- EMBPS    :  " << nEMBPS << " (ref: 1024 for 512 HV lines)" << std::endl;
  std::cout << "  -- EMECPS   :   " << nEMECPS << " (ref:  128 for 128 HV lines)" << std::endl;
  //std::cout << "  -- EMBPUR   :    " << nEMBPUR << " (ref: 20 for 20 HV lines)" << std::endl;
  //std::cout << "  -- ECPUR    :    " << nECPUR  << " (ref: 40 for 40 HV lines)" << std::endl;
  std::cout << "============================================================================ " << std::endl;
  std::cout << "  -- TOTAL    : " << nDET << std::endl; 
  std::cout << "============================================================================ " << std::endl; 
}


int main (int argc, char* argv[])  
/*============================= */
{// start of main 
    if (argc < 2) return (1);  
    IdDictParser parser;  

    // Test of LArElectrode
    std::string lArIDFileName = "IdDictLArElectrode.xml";
    parser.register_external_entity("LArElectrode",lArIDFileName);
    IdDictMgr& idd = parser.parse (argv[1]);  
    std::cout << "[MAIN] --> got dict mgr idd=" << lArIDFileName << std::endl;
    check_larElectrode_decoding(idd);

    // Test of LArHVLine
    lArIDFileName = "IdDictLArHighVoltage.xml";
    parser.register_external_entity("LArHighVoltage",lArIDFileName);
    idd = parser.parse (argv[1]);  
    check_lar_highvoltage_id_decoding(idd);     

    return 0;  
}  
  
 
 
 
 
 

