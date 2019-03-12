/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "IdDictParser/IdDictParser.h"  
#include "Identifier/IdentifierHash.h" 
#include "LArIdentifier/LArOnlineID.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream> 
#include <fstream> 
 
static void check_lar_online_id_decoding(IdDictMgr& idd)
/*=====================================================*/
{    
  std::cout << "====> Check_LarOnline_id_decoding" << std::endl;

  LArOnlineID laronline_id;
  // = idd.find_dictionary ("LArCalorimeter"); 

  const IdDictDictionary* dictionary = idd.find_dictionary ("LArCalorimeter"); 
  if(!dictionary) {
    std::cout << "[TEST_LARONLINE] [0] Cannot access LArCalorimeter dictionary "
	      << std::endl;
    //    return (1);
  }
  else{
    std::cout << "[TEST_LARONLINE] [0] Got dictionary" << std::endl;
  }
  
  if(laronline_id.initialize_from_dictionary (idd)) {
    std::cout << "[TEST_LARONLINE] [0] Check initialize_from_dictionary " << std::endl;
  }
  else{
    std::cout << "[TEST_LARONLINE] [0] Got dictionary from dictionary" << std::endl;
  }

  const LArOnlineID* p_laronline_id = &laronline_id;
  bool verif = laronline_id.do_checks();
  std::cout << "[TEST_LARONLINE] [1] Do_check verif0 = " << verif << std::endl;
  laronline_id.set_do_checks(true);
  verif = laronline_id.do_checks();
  std::cout << "[TEST_LARONLINE] [1] Do_Check verif1 = " << verif << std::endl;
  std::string l_version = laronline_id.dictionaryVersion();
  l_version = "fullAtlas";

  std::cout << "[TEST_LARONLINE] --------------------" << std::endl;
  std::cout << "[TEST_LARONLINE]| Version: " << l_version << " | " << std::endl;
  std::cout << "[TEST_LARONLINE] --------------------" << std::endl;

  //===================================================
  //===================================================
  /* Test OF FEEDTHROUGH HWIdentifiers */
  //===================================================
  //===================================================

  std::vector<HWIdentifier>::const_iterator itFT = p_laronline_id->feedthrough_begin();
  std::vector<HWIdentifier>::const_iterator itFTEnd = p_laronline_id->feedthrough_end();    
  std::cout << "[TEST_LARONLINE] [2] FeedThrough Loop : " << itFTEnd-itFT << " ()" << std::endl;

  int   Nembarrel    = 0;
  int   Nembarrel_old= 0;
  int   Nendcap      = 0;
  int   Nendcap_old  = 0;
  int   Nendcap_in   = 0;
  int   Nembps       = 0;
  int   Nemecps      = 0;
  int   Nfcal        = 0;
  int   Nhec         = 0;
  int   Nhec_old     = 0;

  bool  isEMB        = false;
  bool  isEMBold     = false;
  bool  isEMEC       = false;
  bool  isEMECin     = false;
  bool  isEMECold    = false;
  bool  isFCAL       = false;
  bool  isHEC        = false;
  bool  isHECold     = false;
  bool  isEMBPS      = false;
  bool  isEMECPS     = false;

 //bool  isPS         = false;
  //bool  error        = false;

  int   n_feedthroughId=0;

  for(; itFT!=itFTEnd;++itFT)
    {
      HWIdentifier l_feedthroughId = *itFT;
      isFCAL     = p_laronline_id->isFCALchannel   ( l_feedthroughId );
      isHEC      = p_laronline_id->isHECchannel    ( l_feedthroughId );
      isHECold   = p_laronline_id->isHecOnline     ( l_feedthroughId );
      isEMB      = p_laronline_id->isEMBchannel    ( l_feedthroughId );
      isEMBold   = p_laronline_id->isEmBarrelOnline( l_feedthroughId );
      isEMEC     = p_laronline_id->isEMECchannel   ( l_feedthroughId );
      isEMECin   = p_laronline_id->isEMECIW        ( l_feedthroughId );
      isEMECold  = p_laronline_id->isEmEndcapOnline( l_feedthroughId );
      isEMBPS    = p_laronline_id->isEMBPS         ( l_feedthroughId );
      isEMECPS   = p_laronline_id->isEMECPS        ( l_feedthroughId );
      std::string ftname = p_laronline_id->feedthrough_name( l_feedthroughId );
      if( isFCAL     == true ){ 
	//std::cout << "FCAL FeedthroughName= " << ftname << std::endl;
	Nfcal++;
      }
      if( isHEC      == true ){ Nhec++;}
      if( isHECold   == true ){ Nhec_old++;}
      if( isEMB      == true ){ Nembarrel++;}
      if( isEMBold   == true ){ Nembarrel_old++;}
      if( isEMEC     == true ){ Nendcap++;}
      if( isEMECin   == true ){ 
	std::cout << "EMEC inner wheel FeedthroughName = " << ftname << " (FT=9,10,16,22) " << std::endl;
	Nendcap_in++;
      }
      if( isEMECold  == true ){ Nendcap_old++;}
      if( isEMBPS    == true ){ Nembps++;}
      if( isEMECPS   == true ){ Nemecps++;}

      n_feedthroughId++;
      IdentifierHash hashId = p_laronline_id->feedthrough_Hash( l_feedthroughId );
      HWIdentifier l_feedthroughId2 = p_laronline_id->feedthrough_Id( hashId );
      if ( l_feedthroughId2 != l_feedthroughId ) {
	std::cout
	  << " FeedthroughId2 incorrect: " <<  p_laronline_id->show_to_string(l_feedthroughId2) 
	  << "                should be: " <<  p_laronline_id->show_to_string(l_feedthroughId) 
	  << "                  hashId = " << hashId
	  << std::endl;
	//error = true;
      }
      int i_barrel_ec   = p_laronline_id->barrel_ec(l_feedthroughId);
      int i_side        = p_laronline_id->pos_neg(l_feedthroughId);
      int i_feedthrough = p_laronline_id->feedthrough(l_feedthroughId);
      HWIdentifier l_feedthroughId3 = p_laronline_id->feedthrough_Id (i_barrel_ec, 
								      i_side, 
								      i_feedthrough );
	if ( l_feedthroughId3 != l_feedthroughId ) 
	  {
	    std::cout
	      << " feedthroughId3 incorrect: " <<  p_laronline_id->show_to_string(l_feedthroughId3) 
	      << "                should be: " <<  p_laronline_id->show_to_string(l_feedthroughId) 
	      << std::endl;
	    //error = true;
	  }	
      }
    if( l_version == "fullAtlas")
      {
	std::cout << "====================================================================== " << std::endl;
	std::cout << "  total number of feedthrough= " << n_feedthroughId << ", feedthrough_hash_max= " 
		  << p_laronline_id->feedthroughHashMax() << std::endl;
	std::cout << "  ---> # FCAL     = " << Nfcal << std::endl;
	std::cout << "  ---> # HEC      = " << Nhec  << " #(old) = " << Nhec_old  << std::endl;
	std::cout << "  ---> # EMB      = " << Nembarrel << " #(old) = " << Nembarrel_old << std::endl;	
	std::cout << "  ---> # EMEC     = " << Nendcap << " #(old) = " << Nendcap_old  
		  << " #(in HEC crate)= " << Nendcap_in << std::endl;
	std::cout << "  ---> # EMBPS    = " << Nembps << std::endl;
	std::cout << "  ---> # EMECPS   = " << Nemecps << std::endl;
      }

    //===================================================
    //===================================================
    /* Test of FEB HWIdentifier */
    //===================================================
    //===================================================


    Nfcal         = 0;
    Nhec          = 0;
    Nhec_old      = 0;
    Nembarrel     = 0;
    Nembarrel_old = 0;
    Nendcap       = 0;    
    Nendcap_in    = 0;
    Nendcap_old   = 0;
    Nembps        = 0;
    Nemecps       = 0;

    isFCAL   = false;
    isHEC    = false;
    isHECold = false;
    isEMB    = false;
    isEMBold = false;
    isEMEC   = false;
    isEMECin = false;
    isEMECold= false;
 
    std::vector<HWIdentifier>::const_iterator itFEB = p_laronline_id->feb_begin();
    std::vector<HWIdentifier>::const_iterator itFEBEnd = p_laronline_id->feb_end();    
    int n_febId=0;
    //bool error_feb1 = false;
    //bool error_feb2 = false;
    //bool error_feb3 = false;
    //bool error_feb4 = false;
    for(; itFEB!=itFEBEnd;++itFEB)
      {
	HWIdentifier l_febId = *itFEB;
	n_febId++;

	isFCAL     = p_laronline_id->isFCALchannel   ( l_febId );
	isHEC      = p_laronline_id->isHECchannel    ( l_febId );
	isHECold   = p_laronline_id->isHecOnlineFebId( l_febId );
	isEMB      = p_laronline_id->isEMBchannel    ( l_febId );
	isEMBold   = p_laronline_id->isEmBarrelOnline( l_febId );
	isEMEC     = p_laronline_id->isEMECchannel   ( l_febId );
	isEMECin   = p_laronline_id->isEMECIW        ( l_febId );
	isEMECold  = p_laronline_id->isEmEndcapOnline( l_febId );
	isEMBPS    = p_laronline_id->isEMBPS         ( l_febId );
	isEMECPS   = p_laronline_id->isEMECPS        ( l_febId );

	if( isFCAL     == true ){ Nfcal++;}
	if( isHEC      == true ){ Nhec++;}
	if( isHECold   == true ){ Nhec_old++;}
	if( isEMB      == true ){ Nembarrel++;}
	if( isEMBold   == true ){ Nembarrel_old++;}
	if( isEMEC     == true ){ Nendcap++;}
	if( isEMECin   == true ){ Nendcap_in++;}
	if( isEMECold  == true ){ Nendcap_old++;}
	if( isEMBPS    == true ){ Nembps++;}
	if( isEMECPS   == true ){ Nemecps++;}

       
	/* Test of channel from febHash Id */
	IdentifierHash hashId = p_laronline_id->feb_Hash( l_febId );
	HWIdentifier l_febId1 = p_laronline_id->feb_Id( hashId );
	if ( l_febId1 != l_febId ) {
	  std::cout
	    << " FebId1 incorrect: " <<  p_laronline_id->show_to_string(l_febId1) 
	    << "        should be: " <<  p_laronline_id->show_to_string(l_febId) 
	    << "          hashId : " << hashId
	    << std::endl;
	  //error_feb1 = true;
	}

	/* Test of channel from feb_Id(barrel,side,feedthr,slot) */
	int i_barrel_ec   = p_laronline_id->barrel_ec(l_febId);
	int i_side        = p_laronline_id->pos_neg(l_febId);
	int i_feedthrough = p_laronline_id->feedthrough(l_febId);
	int i_slot        = p_laronline_id->slot(l_febId);
	int i_channel     = p_laronline_id->channel(l_febId);
	HWIdentifier l_febId2 = p_laronline_id->feb_Id (i_barrel_ec, 
							i_side,
							i_feedthrough,
							i_slot );
	if ( l_febId2 != l_febId ) 
	  {
	    std::cout
	      << " febId2 incorrect: " <<  p_laronline_id->show_to_string(l_febId2) 
	      << "        should be: " <<  p_laronline_id->show_to_string(l_febId) 
	      << std::endl;
	    //error_feb2 = true;
	  }	

	/* Test feb from feb_Id(channelId) */
	HWIdentifier l_channelId_feb = p_laronline_id->channel_Id( l_febId, i_channel );
	HWIdentifier l_febId3 = p_laronline_id->feb_Id( l_channelId_feb );
	if ( l_febId3 != l_febId ) 
	  {
	    std::cout
	      << " febId3 incorrect: " <<  p_laronline_id->show_to_string(l_febId3) 
	      << "        should be: " <<  p_laronline_id->show_to_string(l_febId) 
	      << std::endl;
	    //error_feb3 = true;
	  }	

	/* Test feb from feb_Id(feedthroughId,slot) */
	HWIdentifier l_feedthroughId_feb = p_laronline_id->feedthrough_Id( l_febId );
	HWIdentifier l_febId4 = p_laronline_id->feb_Id( l_feedthroughId_feb, i_slot);
	if ( l_febId4 != l_febId ) 
	  {
	    std::cout
	      << " febId4 incorrect: " <<  p_laronline_id->show_to_string(l_febId4) 
	      << "        should be: " <<  p_laronline_id->show_to_string(l_febId) 
	      << std::endl;
	    //error_feb4 = true;
	  }	
      }
    if( l_version == "fullAtlas" )
      {
	std::cout << "  total number of feb= " << n_febId << ", feb_hash_max= " << p_laronline_id->febHashMax() << std::endl;
	std::cout << "  ---> # FCAL     = " << Nfcal << std::endl;
	std::cout << "  ---> # HEC      = " << Nhec  << " #(old) = " << Nhec_old  << std::endl;
	std::cout << "  ---> # EMB      = " << Nembarrel << " #(old) = " << Nembarrel_old << std::endl;	
	std::cout << "  ---> # EMEC     = " << Nendcap << " #(old) = " << Nendcap_old  
		  << " #(in HEC crate)= " << Nendcap_in << std::endl;
	std::cout << "  ---> # EMBPS    = " << Nembps << std::endl;
	std::cout << "  ---> # EMECPS   = " << Nemecps << std::endl;
      }



    //==========================================================
    //==========================================================
    /* Test of CHANNEL HWIdentifier */
    //==========================================================
    //==========================================================

    Nfcal         = 0;
    Nhec          = 0;
    Nhec_old      = 0;
    Nembarrel     = 0;
    Nembarrel_old = 0;
    Nendcap       = 0;    
    Nendcap_in    = 0;
    Nendcap_old   = 0;
    Nembps        = 0;
    Nemecps       = 0;

    isFCAL   = false;
    isHEC    = false;
    isHECold = false;
    isEMB    = false;
    isEMBold = false;
    isEMEC   = false;
    isEMECin = false;
    isEMECold= false;

    bool isH8=false;
    bool isH8crack=false;
    bool isH8low=false;
    bool isH8high=false;
    bool isH6=false;
    //bool isH8online=false;
    int  NH6=0;
    int  NH6fcal=0;
    int  NH6emec=0;
    int  NH6hec=0; 
    int  NH8=0;
    int  NH8crack=0;
    int  NH8low=0;
    int  NH8high=0;
    std::vector<HWIdentifier>::const_iterator itCH = p_laronline_id->channel_begin();
    std::vector<HWIdentifier>::const_iterator itCHEnd = p_laronline_id->channel_end();    
    int n_channelId=0;
    int NH8onlineOnly = 0;
    //bool error_ch1 = false;
    //bool error_ch2 = false;    
    //bool error_ch3 = false;
    //bool error_ch4 = false;

    // AL-> fstream open
    std::string myStat;
    std::fstream fout;
    std::fstream fileDict;
    fout.open("TEST_ONLINE.txt",std::ios::out);
    bool isDict=true;
    if(isDict){
      fileDict.open("DICTIONARY_DUMP.txt",std::ios::out);
      fileDict << " OnlineID for ATLAS Dictionary " << std::endl;
    }
    for(; itCH!=itCHEnd;++itCH)
      {
	HWIdentifier l_channelId = *itCH;
	n_channelId++;
	isFCAL     = p_laronline_id->isFCALchannel   ( l_channelId );
	isHEC      = p_laronline_id->isHECchannel    ( l_channelId );
	isHECold   = p_laronline_id->isHecOnline     ( l_channelId );
	isEMB      = p_laronline_id->isEMBchannel    ( l_channelId );
	isEMBold   = p_laronline_id->isEmBarrelOnline( l_channelId );
	isEMEC     = p_laronline_id->isEMECchannel   ( l_channelId );
	isEMECin   = p_laronline_id->isEMECIW        ( l_channelId );
	isEMECold  = p_laronline_id->isEmEndcapOnline( l_channelId );
	isEMBPS    = p_laronline_id->isEMBPS         ( l_channelId );
	isEMECPS   = p_laronline_id->isEMECPS        ( l_channelId );
	std::string ftname = p_laronline_id->channel_name( l_channelId );
	std::string TestChName;
	TestChName = p_laronline_id->channel_name(l_channelId);

	if( isFCAL     == true ){ Nfcal++;
	  std::cout << TestChName << std::endl;	 
	}
	if( isHEC      == true ){ Nhec++;
	  std::cout << TestChName << std::endl;	 
	}
	if( isHECold   == true ){ Nhec_old++;}
	if( isEMB      == true ){ Nembarrel++;}
	if( isEMBold   == true ){ Nembarrel_old++;}
	if( isEMEC     == true ){ Nendcap++;}

	if( isEMECin   == true ){ 
	  std::cout << TestChName << "EMEC inner wheel Channel = " << ftname << std::endl;	 
	  Nendcap_in++;
	}
	if( isEMECold  == true ){ Nendcap_old++;}
	if( isEMBPS    == true ){ Nembps++;}
	if( isEMECPS   == true ){ Nemecps++;}

	/*
	  isFCAL     = p_laronline_id->isFcalOnline( l_channelId );
	  isHEC      = p_laronline_id->isHecOnline( l_channelId );
	  isEMB      = p_laronline_id->isEmBarrelOnline( l_channelId );
	  isEMEC     = p_laronline_id->isEmEndcapOnline( l_channelId );
	  isH8online = p_laronline_id->is_H8onlineOnly( l_channelId );
	  isEMBPS    = p_laronline_id->isEMBPS( l_channelId );
	  isEMECPS   = p_laronline_id->isEMECPS( l_channelId );
	  isPS       = p_laronline_id->isPS( l_channelId );
	
	  std::string chanName;
	  chanName = p_laronline_id->channel_name( l_channelId ); 
	  if( isEMBPS == true ){ 
	  Nembps++;
	  }
	  if( isEMECPS == true ){ Nemecps++;}
	  if( isPS == true ){ 
	  Nps++;
	  
	  }
	  if( isFCAL == true )
	  { 
	  myStat="(FCAL)";
	  Nfcal++;
	  }
	  if( isHEC == true )
	  { 
	  Nhec++;
	  myStat="(HEC)";
	  }
	  if( isEMB == true )
	  {
	  myStat="(EM BARREL)";
	  Nembarrel++;
	  }
	  if( isEMEC == true )
	  {
	  myStat="(EMEC)";
	  Nendcap++;
	  }
	  if( isDict )
	  {
	  //	    fileDict << chanName << " OnlineID= "
	  //     << p_laronline_id->show_to_string(l_channelId) 
	  //     << " " << myStat 
	  //     << std::endl;
	  }
	*/
	std::string chanName;
	chanName = p_laronline_id->channel_name( l_channelId ); 
	if( isDict )
	  {
	    if( isH6 )
	      {
		NH8onlineOnly++;
		fileDict << chanName << " OnlineID= "
			 << p_laronline_id->show_to_string(l_channelId) 
			 << " " << myStat 
			 << std::endl;
	    }
	}
	// is_H8
	isH8= p_laronline_id->is_H8(l_channelId);
	isH8crack= p_laronline_id->is_H8crack(l_channelId);
	isH8low= p_laronline_id->is_H8low(l_channelId);
	isH8high= p_laronline_id->is_H8high(l_channelId);
	if( isH8 ){NH8++;}
	if( isH8crack ){NH8crack++;}
	if( isH8low   ){NH8low++;}
	if( isH8high  ){NH8high++;}

	// is_H6
	isH6=p_laronline_id->is_H6( l_channelId ); 
	if( isH6)
	  {
	    NH6++;
	    fout << "[" << n_channelId << "] OnlineID= " 
		 << p_laronline_id->show_to_string(l_channelId)
		 << std::endl;
	    if( p_laronline_id->is_H6emec(l_channelId) ){NH6emec++;};
	    if( p_laronline_id->is_H6fcal(l_channelId) ){NH6fcal++;};
	    if( p_laronline_id->is_H6hec(l_channelId) ){NH6hec++;};
	  }

	/* Test of channel from channelHash Id */
	IdentifierHash hashId = p_laronline_id->channel_Hash( l_channelId );
	HWIdentifier l_channelId1 = p_laronline_id->channel_Id( hashId );
	if ( l_channelId1 != l_channelId ) {
	  std::cout
	    << " ChannelId1 incorrect: " <<  p_laronline_id->show_to_string(l_channelId1) 
	    << "            should be: " <<  p_laronline_id->show_to_string(l_channelId) 
	    << "              hashId : " << hashId
	    << std::endl;
	  //error_ch1 = true;
	}

	/* Test of channel from channel_Id(barrel,side,feedthr,slot,channel) */
	int i_barrel_ec   = p_laronline_id->barrel_ec(l_channelId);
	int i_side        = p_laronline_id->pos_neg(l_channelId);
	int i_feedthrough = p_laronline_id->feedthrough(l_channelId);
	int i_slot        = p_laronline_id->slot(l_channelId);
	int i_channel     = p_laronline_id->channel(l_channelId);
	HWIdentifier l_channelId2 = p_laronline_id->channel_Id (i_barrel_ec, 
								i_side,
								i_feedthrough,
								i_slot,
								i_channel );
	if ( l_channelId2 != l_channelId ) 
	  {
	    std::cout
	      << " channelId3 incorrect: " <<  p_laronline_id->show_to_string(l_channelId2) 
	      << "                should be: " <<  p_laronline_id->show_to_string(l_channelId) 
	      << std::endl;
	    //error_ch2 = true;
	  }	

	/* Test of channel from channel_Id(feedthrough,slot,channel) */
	HWIdentifier l_feedthroughId_ch = p_laronline_id->feedthrough_Id( l_channelId );
	HWIdentifier l_channelId3 = p_laronline_id->channel_Id( l_feedthroughId_ch, i_slot, i_channel );
	if ( l_channelId3 != l_channelId ) 
	  {
	    std::cout
	      << " channelId3 incorrect: " <<  p_laronline_id->show_to_string(l_channelId3) 
	      << "            should be: " <<  p_laronline_id->show_to_string(l_channelId) 
	      << std::endl;
	    //error_ch3 = true;
	  }	

	/* Test of channel from channel_Id(feb , channel) */
	HWIdentifier l_febId = p_laronline_id->feb_Id( l_channelId );
	HWIdentifier l_channelId4 = p_laronline_id->channel_Id( l_febId, i_channel );
	if ( l_channelId4 != l_channelId ) 
	  {
	    std::cout
	      << " channelId4 incorrect: " <<  p_laronline_id->show_to_string(l_channelId4) 
	      << "            should be: " <<  p_laronline_id->show_to_string(l_channelId) 
	      << std::endl;
	    //error_ch4 = true;
	  }	


      }
    if( isDict )
      {
	fileDict.close();
      }

    fout.close();

    if( l_version == "fullAtlas" )
      {
	std::cout << "===>   total number of channel= " << n_channelId << ", channel_hash_max= " << p_laronline_id->channelHashMax() << std::endl;
	
	std::cout << "  ---> # FCAL     = " << Nfcal << std::endl;
	std::cout << "  ---> # HEC      = " << Nhec  << " #(old) = " << Nhec_old  << std::endl;
	std::cout << "  ---> # EMB      = " << Nembarrel << " #(old) = " << Nembarrel_old << std::endl;	
	std::cout << "  ---> # EMEC     = " << Nendcap << " #(old) = " << Nendcap_old  
		  << " #(in HEC crate)= " << Nendcap_in << std::endl;
	std::cout << "  ---> # EMBPS    = " << Nembps << std::endl;
	std::cout << "  ---> # EMECPS   = " << Nemecps << std::endl;
      }



    if( l_version == "H6TestBeam" )
      {
	std::cout << "============================================================================ " << std::endl;
	std::cout << "---> # H6 channel = " << NH6 << std::endl;
	std::cout << "           # FCAL = " << NH6fcal << std::endl;
	std::cout << "           # HEC  = " << NH6hec << std::endl; 
	std::cout << "           #EMEC  = " << NH6emec << std::endl;
	std::cout << "============================================================================ " << std::endl;
      }
    if( l_version == "H8TestBeam" )
      {
	std::cout << "============================================================================ " << std::endl;
	std::cout << "---> # H8 channel = " << NH8 << std::endl;  	     
	std::cout << "---> # H8 Online Only channels :" << NH8onlineOnly << std::endl;
	std::cout << "---> # H8 crack channel = " << NH8crack << std::endl; 
	std::cout << "---> # H8 low eta channel = " << NH8low << std::endl; 
	std::cout << "---> # H8 high eta channel = " << NH8high << std::endl; 
	std::cout << "============================================================================ " << std::endl;
      }


    /* Test of calib CHANNEL HWIdentifier */
    int Ncal     = 0;
    int NcalHec  = 0;
    int NcalFcal = 0;
    int NcalEmbarrel = 0;
    int NcalEndcap = 0;
    int Ncalibrated=0;
    std::vector<HWIdentifier>::const_iterator itCAL = p_laronline_id->calib_channel_begin();
    std::vector<HWIdentifier>::const_iterator itCALEND = p_laronline_id->calib_channel_end();    
    for(; itCAL!=itCALEND;++itCAL)
      {
	HWIdentifier l_calibId = *itCAL;
	Ncal++;
	/* Test of isFcalOnline() */
	isFCAL = p_laronline_id->isFcalOnline( l_calibId );
	isHEC = p_laronline_id->isHecOnline( l_calibId );
	isEMB = p_laronline_id->isEmBarrelOnline( l_calibId );
	isEMEC = p_laronline_id->isEmEndcapOnline( l_calibId );
	bool isCalib=p_laronline_id->isCalibration( l_calibId);
	if( isFCAL == true )
	  { 
	    NcalFcal++;
	  }
	if( isHEC == true )
	  { 
	    NcalHec++;
	  }
	if( isEMB == true )
	  {
	    NcalEmbarrel++;
	  }
	if( isEMEC == true )
	  {
	    NcalEndcap++;
	  }
	if( isCalib )
	  {
	    Ncalibrated++;
	  }
      }
    std::cout << "---> Total Nb of Calibrated Channels : " << Ncal << std::endl;
    std::cout << "--->        # Em Barrel cal Channels : " << NcalEmbarrel << std::endl;
    std::cout << "--->        # Em Endcap cal Channels : " << NcalEndcap << std::endl;
    std::cout << "--->        # Em FCAL   cal Channels : " << NcalFcal << std::endl;
    std::cout << "--->        # Em HEC    cal Channels : " << NcalHec << std::endl;
    std::cout << "--->        # calibrated    Channels : " << Ncalibrated << std::endl;
    std::cout << "============================================================================ " << std::endl;



   //   std::cout << "finally check the exception throwing... " << std::endl;
   /*
   try {
     HWIdentifier l_wrong_channelId = p_laronline_id->channel_Id (0,-1,0,0,0); 
   }
   catch(LArOnlID_Exception & except){
     std::cout << (std::string) except << std::endl ;
   }
   try {
     HWIdentifier l_good_febId = p_laronline_id->feb_Id (0,0,0,0); 
     HWIdentifier l_wrong_channelId = p_laronline_id->channel_Id (l_good_febId,-99); 
   }
   catch(LArOnlID_Exception & except){
     std::cout << (std::string) except << std::endl ;
   }
   try {
     HWIdentifier l_good_ftId = p_laronline_id->feedthrough_Id (0,0,0); 
     HWIdentifier l_wrong_channelId = p_laronline_id->channel_Id (l_good_ftId,0,-99); 
   }
   catch(LArOnlID_Exception & except){
     std::cout << (std::string) except << std::endl ;
   }
   try {
     HWIdentifier l_wrong_febId = p_laronline_id->feb_Id (0,-1,0,0); 
   }
   catch(LArOnlID_Exception & except){
     std::cout << (std::string) except << std::endl ;
   }
   try {
     HWIdentifier l_wrong_ftId = p_laronline_id->feedthrough_Id (0,-1,0); 
     }
     catch(LArOnlID_Exception & except){
     std::cout << (std::string) except << std::endl ;
     }
   */

}



int main (int argc, char* argv[])  
/*============================= */
{  
    if (argc < 2) return (1);  
    IdDictParser parser;  
    ///std::string lArIDFileName = "IdDictLArCalorimeter.xml";
    std::string lArIDFileName = "IdDictLArCalorimeter_DC3-05.xml";
    //std::string lArIDFileName = "IdDictLArCalorimeter_H6_2004.xml";
    //std::string lArIDFileName = "IdDictLArCalorimeter_H8_2004.xml";
    //std::string lArIDFileName = "IdDictLArCalorimeter_H8largeEta_2004.xml";
    //std::string lArIDFileName = "IdDictLArCalorimeter_H8crack_2004.xml";
    //std::string lArIDFileName = "IdDictLArCalorimeter_H8lowEta_2004.xml";
    parser.register_external_entity("LArCalorimeter",lArIDFileName);
    IdDictMgr& idd = parser.parse (argv[1]);  
    std::cout << "[MAIN] --> got dict mgr idd" << lArIDFileName << std::endl;
    check_lar_online_id_decoding(idd);     
    return 0;  
}  
  
 
 
 
 
 

