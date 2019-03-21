/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

StatusCode Muon::RpcROD_Decoder::fillCollection_v302new(BS data, const uint32_t data_size, RpcPad& v,
							const uint32_t& sourceId, RpcSectorLogicContainer* sectorLogicContainer ) const

{
  ATH_MSG_VERBOSE("in fillCollection_v302new");
  //std::cout<<" in fillCollection_v302"<<std::endl;
  std::map<Identifier,RpcPad*> vmap;
  vmap[v.identify()]=&v;
  StatusCode cnvsc= fillCollectionsFromRob_v302(data,data_size,vmap,sourceId, sectorLogicContainer);
  if (cnvsc!=StatusCode::SUCCESS) 
    {
      ATH_MSG_DEBUG("Some decoding problem observed");
    }
  return cnvsc;
}


StatusCode Muon::RpcROD_Decoder::fillCollectionsFromRob_v302(BS data, const uint32_t data_size,	std::map<Identifier,RpcPad*>& vmap, const uint32_t& sourceId,
    RpcSectorLogicContainer* sectorLogicContainer ) const 
{

  bool skipSectorLogicDecoding = (sectorLogicContainer == nullptr);
  if(skipSectorLogicDecoding)
    ATH_MSG_DEBUG("Skip SectorLogic decoding, so SLROC.decodeFragment is not being processed");

  /* for (unsigned int i = 0; i<1000; ++i) { */
  /*   //std::cout<<" aaa "<<std::endl; */
  /*   msg(MSG::VERBOSE) << "try to increase cpu time "<<log(pow(((double)i+1.)/999.,3))<<endmsg; */
  /* } */

  // m_bench.point(1);
  // unpack the 32 bits words into 16 bits 
  // no ROD header and footer 
  ATH_MSG_VERBOSE("in fillCollectionFromRob_v302");
  std::vector<uint16_t> p = get16bits_v301(data,data_size,0,0);
    
  const int size = p.size();

  ATH_MSG_DEBUG("In fillCollectionsFromRob_v302:");
  ATH_MSG_VERBOSE("**Start of v302********Decoder v302 dumping the words******** ");
  ////  notice that instructions in between   #ifndef NVERBOSE are executed in normal jobs with standard (INFO) output level !!!!	      
  /* #ifndef NVERBOSE */
  /*     std::cout<<" printing something for testing"<<std::endl; */
  /* #endif */

  //if (size > 0  && msgLvl(MSG::INFO) ) {
  if (size > 0  && msgLvl(MSG::VERBOSE) ) {
    msg(MSG::VERBOSE) << "The size of this ROD-read is " << size << endmsg;
    int  decoded             ;
    int iSLh = -1;
    char decoded_char[1000]  ;
    for (int i=0; i < size; i++){     
        
      decoded = (p[i] & 0xf000)>> 12 ;
      if(decoded<0x4 )sprintf(decoded_char,"Hit data"       );
      if(decoded==0x4)sprintf(decoded_char,"CM Footer"      );
      if(decoded==0x5)sprintf(decoded_char,"PAD Header"     );
      if(decoded==0x6)sprintf(decoded_char,"PAD/SL Subheader"  );
      if(decoded==0x7)sprintf(decoded_char,"PAD Footer"     );
      if(decoded==0x8)sprintf(decoded_char,"CM Subheader"   );
      if(decoded==0x9)sprintf(decoded_char,"RX Header"      );
      if(decoded==0xa)sprintf(decoded_char,"PAD Prefooter"  );
      if(decoded==0xb)sprintf(decoded_char,"RX Footer"      );
      if(decoded==0xc)sprintf(decoded_char,"CM Header"      );
      if(decoded==0xd){
	sprintf(decoded_char,"SL Header"      );
	iSLh = i;
      }
      if(decoded==0xe)sprintf(decoded_char,"RX Subheader"   );
      if(decoded==0xf)sprintf(decoded_char,"SL Footer"      );
      if (i>1 && i==iSLh+2 && (decoded!=0xf)) 
	{
	  sprintf(decoded_char,"SL hit ???"      );
	}
        
      //      msg(MSG::INFO) << "word " << i <<" = "<< MSG::hex << p[i] << MSG::dec 
      //			<< " " << MSG::hex << decoded << MSG::dec << " " << decoded_char << endmsg;
      msg(MSG::VERBOSE) << "word " << i <<" = "<< MSG::hex << p[i] << MSG::dec 
      		<< " " << MSG::hex << decoded << MSG::dec << " " << decoded_char << endmsg;
    }
  }
  int nwInCM = 0;

  
    
  if (size == 0) {
    ATH_MSG_VERBOSE(" Buffer size 0 ! ");
    return StatusCode::FAILURE;
  }
    
    
    
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "The source ID is: " << MSG::hex << sourceId << endmsg;
    
    
  uint16_t sectorForCabling = 0;
  uint16_t sector = 0;
    
  // counter of the numerb of sector logic fragments found
  uint16_t SLindex = 0;
    
  // the identifier of this collection 
  Identifier thisPadOfflineId;
  IdentifierHash thisPadHashOfflineId;
    
  // Identifier of the collection in data
  Identifier padOfflineId;
  bool foundPad=false;
  int nDecodedPads = 0;
  int nDecodedSL   = 0;
  int nDecodedRX   = 0;
    
  if (msgLvl(MSG::VERBOSE ))
    {
      for (std::map<Identifier,RpcPad*>::const_iterator it=vmap.begin(); it!=vmap.end(); ++it)
	{
	  Identifier myPadOfflineId = (*it).second->identify();
	  IdentifierHash myPadHashOfflineId =  (*it).second->identifyHash();	    
	  msg(MSG::VERBOSE) << "The offline ID requested for conversion are "
			    << m_pRpcIdHelper->show_to_string(myPadOfflineId)<<" hash version "<< (int)myPadHashOfflineId << endmsg;
	}
    }  
  else if (msgLvl(MSG::DEBUG ))
    {
      msg(MSG::DEBUG)<<"N. of pads requested for conversion = "<<vmap.size()<<endmsg;
    }
  bool isSLHeader    =false;
  bool isSLSubHeader =false;
  bool isSLFooter    =false;
  bool isSLFragment  =false;
  bool isRXHeader    =false;
  bool isRXFooter    =false;
  bool isPADFragment =false;
  bool isPadHeader   =false;
  bool isPadSubHeader=false;
  bool isPadPreFooter=false;
  bool isPadFooter   =false;
    
  //return;
    
  RPCRXRODDecode myRPC;
  RXReadOutStructure  RXROS;
  PadReadOutStructure PDROS;
  SectorLogicRXReadOutStructure SLROS;
  MatrixReadOutStructure matrixROS;
  RPCRODStructure rodReadout;
    
  char recField;
  char recFieldROS;
  char recFieldPAD;
  char recFieldSL;
  unsigned short int PadID=99;
  uint16_t slfel1id;
  uint16_t slid;
  uint16_t slbcid;
  uint16_t slstatus;
  uint16_t slcrc;
  unsigned int SLBodyWords = 0;
  constexpr unsigned int SL_data_sise = 500; //same value used for the size of SLBuff
  unsigned short int SLBuff[500];
    
  // decode the source Id and set side and sector number
  rodReadout.decodeSourceID(sourceId);
  uint16_t subDetectorID = (sourceId & 0xff0000)>>16;
    
  uint16_t side  = (subDetectorID == eformat::MUON_RPC_BARREL_A_SIDE) ? 1:0; 
  uint16_t rodId =  rodReadout.getSourceIDRODID();
  if (msgLvl(MSG::DEBUG) )
    {
      
      msg(MSG::DEBUG) << "subDetectorID = 0x" 
		      << MSG::hex << subDetectorID << MSG::dec 
		      << endmsg;
      
      msg(MSG::DEBUG) << "rodID = 0x" << MSG::hex
		      << rodId << MSG::dec << endmsg;
      msg(MSG::DEBUG) << "The side is " << side 
		      << endmsg;
    }
    
  //RpcSectorLogic* sl;
  RpcPad* thisv = NULL;

  // ---- boolean variables to keep trak of where we stand and what we expect -----
  //
  // bool bsErrCheck_InRX = false;
  // bool bsErrCheck_InSL = false;
  // bool bsErrCheck_InPD = false;
  // bool bsErrCheck_InCM = false;
  //
  //bool bsErrCheck_FromSLFooter  = false;
  // bool bsErrCheck_FromPDFooter  = false;
  // bool bsErrCheck_FromPDHeader  = false;
  //
  // bool bsErrCheck_errorInRXHeader    = false;
  //bool bsErrCheck_errorInRXSubHeader = false;
  // bool bsErrCheck_errorInRXFooter    = false;
  //bool bsErrCheck_errorInSLHeader    = false;
  //bool bsErrCheck_errorInSLSubHeader = false;
  //bool bsErrCheck_errorInSLFooter    = false;
  // bool bsErrCheck_errorInPDHeader    = false;
  // bool bsErrCheck_errorInPDSubHeader = false;
  //bool bsErrCheck_errorInPDFooter    = false;
  //bool bsErrCheck_errorInPDPreFooter = false;
  //bool bsErrCheck_errorInCMHeader    = false;
  //bool bsErrCheck_errorInCMSubHeader = false;
  //bool bsErrCheck_errorInCMPreFooter = false;
  //bool bsErrCheck_errorInCMFooter    = false;
  //
  // ------------------------------------------------------------------------------


  for (uint16_t i = 0; i < size; ++i) 
    {
      
      //std::cout << "REGISTER: " << i << std::endl;
      
      isRXHeader    =false;
      isRXFooter    =false;
      isPadHeader   =false;
      isPadSubHeader=false;
      isPadPreFooter=false;
      isPadFooter   =false;
      isSLHeader    =false;
      isSLSubHeader =false;
      isSLFooter    =false;
      uint32_t currentWord = p[i];
      
      
      RXROS.decodeFragment(currentWord,recFieldROS);
      PDROS.decodeFragment(currentWord,recFieldPAD);
      if(!skipSectorLogicDecoding){
        SLROS.decodeFragment(currentWord,recFieldSL);
      }
      
      if (RXROS.isHeader()) {
        isRXHeader=true;
        isSLFragment=false;
        isPADFragment=false;
      } else if (PDROS.isSubHeader() && !isSLFragment){
        isPadSubHeader=true;        
      } else if (RXROS.isFooter())   {
        isRXFooter=true;
      } else if (PDROS.isHeader())   {
        isPadHeader=true;
        isSLFragment=false;
      } else if (PDROS.isPreFooter()){
        isPadPreFooter=true;
      } else if (PDROS.isFooter() )  {
        isPadFooter=true;
      } else if (SLROS.isHeader())   {
        isSLHeader=true;
        isSLFragment=true;
        isPADFragment=false;
      } else if (SLROS.isFooter())   {
        isSLFooter=true;
        isSLFragment=false;
      } else if (SLROS.isSubHeader() && isSLFragment) {
        isSLSubHeader=true;
      }

      // The SLROS functions still return values (based on default values)
      if(skipSectorLogicDecoding){
        isSLHeader    = false;
        isSLSubHeader = false;
        isSLFragment  = false;
        isSLFooter    = false;
      }
      
      ATH_MSG_VERBOSE("\n isRXHeader " << isRXHeader <<
                      "\n isRXFooter " << isRXFooter <<
                      "\n isPadHeader " << isPadHeader <<
                      "\n isPadSubHeader " << isPadSubHeader <<
                      "\n isPADFragment " << isPADFragment <<
                      "\n isPadPreFooter " << isPadPreFooter <<
                      "\n isPadFooter " << isPadFooter <<
                      "\n isSLHeader " << isSLHeader <<
                      "\n isSLSubHeader " << isSLSubHeader <<
                      "\n isSLFragment " << isSLFragment <<
                      "\n isSLFooter " << isSLFooter );
      

      
      if (msgLvl(MSG::VERBOSE) ) {
        char decoded_char[256] ;
        if (isRXHeader) {
          sprintf(decoded_char," RX Header" );    
        }else if (isRXFooter) {
          sprintf(decoded_char," RX Footer" );
        }else if (isSLHeader) {
          sprintf(decoded_char," SL Header" );
        }else if (isSLSubHeader){
          sprintf(decoded_char," SL SubHeader" );
        }else if (isSLFooter){
          sprintf(decoded_char," SL Footer" );
        }else if (isPadHeader) {
          sprintf(decoded_char," Pad Header" );
        }else if (isPadSubHeader) {
          sprintf(decoded_char," Pad SubHeader" );
        }else if (isPadPreFooter) {
          sprintf(decoded_char," Pad PreFooter" );
        }else if (isPadFooter) {
          sprintf(decoded_char," Pad Footer" );    
        }else if (isSLFragment)  {
          sprintf(decoded_char," SL Fragment" );
        }else if (isPADFragment) {
          sprintf(decoded_char," Pad Fragment" );    
        }else{
          sprintf(decoded_char," Undecoded" );    
        }
        
        
        msg(MSG::VERBOSE)  << i << " -->current data word is " 
			   << MSG::hex << currentWord << MSG::dec
			   << decoded_char << " RecFieldROS,PAD,SL  "<<recFieldROS<<" "<<recFieldPAD<<" "<<recFieldSL<<endmsg;
        
      }
      
      //we must start with an RX Header:  
      if (i==0 && (!isRXHeader)) 
      {
        ATH_MSG_WARNING("Bad data from RPC ROD 0x" << MSG::hex << rodId << MSG::dec
        << ". RX header not found at ROB fragment begin. Skipping this ROB fragment.");
        return StatusCode::SUCCESS; //continue decoding data from other ROBs
	      //bsErrCheck_errorInRXHeader = true;
      }


      if(isRXHeader) { 
	      //bsErrCheck_errorInRXHeader = false;
	      //bsErrCheck_InRX = true;
	      // bsErrCheck_InSL = false;
	      // bsErrCheck_InPD = false;
	      // bsErrCheck_InCM = false;

 
        if (msgLvl(MSG::VERBOSE) ){
	        msg(MSG::VERBOSE)  << " this is a RX Header " << endmsg;
	        msg(MSG::VERBOSE)  << " Sector ID="    <<RXROS.RXid()<<endmsg;
	      }
        
        // get the sector id according to the new format
        // not yet implemented in the readout classes
        
        //uint16_t rxid = (currentWord & 0x800) >> 11;
        
        uint16_t rxid = RXROS.RXid();      // range (presumably) 0-1
        sectorForCabling = 2*rodId+rxid;   // range 0-31
        sector = side*32+sectorForCabling; // range 0-63    //side = 1 for side A or 0 for side C // side 1 corresponds to subDetID=65 (side 0 for subDetID=66)
	      if (msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE)  << "RXid, sectorForCabling, sector = "<<rxid<<" "<<sectorForCabling<<" "<<sector<<std::endl;
        
        //fix for M3 
        if ((rodId==3 || rodId==1) && (m_specialROBNumber > 0) ) {
          sector=39;
          sectorForCabling=7;
          //fix for M3 with runnumber up to 11533 (SFI)
          if(m_specialROBNumber == 0x650001){
            sector=40;
            sectorForCabling=8;
          }
	  if (msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE)  << "Here a change !!! RXid, sectorForCabling, sector = "<<rxid<<" "<<sectorForCabling<<" "<<sector<<endmsg;
        }
        else if ( (rodId==4 || rodId==2) && (m_specialROBNumber > 0)) {
          sector=40;
          sectorForCabling=8;
	  if (msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE)  << "Here a change !!! RXid, sectorForCabling, sector = "<<rxid<<" "<<sectorForCabling<<" "<<sector<<endmsg;
        }     
        

      } else if(isRXFooter) {
	// bsErrCheck_errorInRXHeader = false; 
	// bsErrCheck_errorInRXFooter = false; 
        ATH_MSG_VERBOSE(" this is a RX Footer ");
	++nDecodedRX;
	//if (nDecodedRX==2) return StatusCode::SUCCESS;
      } else if(isSLHeader || isSLFragment || isSLSubHeader || isSLFooter)  {
        
        // push only the lowest 16 bits
        int foundSL = myRPC.pushWord(currentWord,0);
        
        if(isSLHeader) { 
          SLBodyWords=0;
          slfel1id = SLROS.fel1id();
          slid = SLROS.slid();
          
          ATH_MSG_VERBOSE(" SL Header: slfel1id " << slfel1id 
			  << " slid: " << slid);
        }
        else if (isSLSubHeader) {
          slbcid = SLROS.slbcid();
          
          ATH_MSG_VERBOSE("SL subheader: slbcid: "
			  << slbcid);
          
        }
        // Decode the sector logic footer
        else if(isSLFooter ) {
          
          ++nDecodedSL;
          if (SLindex>1) {
            ATH_MSG_VERBOSE("More than 2 SL fragments in sector " << sector);
          }
          
          
          if (msgLvl(MSG::VERBOSE) )
	    {
            
	      msg(MSG::VERBOSE) 
		<< " Number of data words in SectorLogicReadOut= "
		<< SLBodyWords << endmsg;  
	      msg(MSG::VERBOSE) 
		<< " TEST SL: "<< foundSL << endmsg;
            
	      // Print out a raw dump of the SL fragment 
	      for(unsigned short j=0; j<SLBodyWords; j++) {
		msg(MSG::VERBOSE) << " SL data word " << j << " : " << MSG::hex
				  << SLBuff[j] << MSG::dec << endmsg;
	      }
	    }
          
          // Found the sector logic footer, the sector logic fragment 
          // can be added to the sector logic container
          
          SectorLogicRXReadOut* sectorLogic = myRPC.SLFragment();
          
          if (sectorLogicContainer && 
              !sectorLogicContainer->findSector(sector,side) ) {
            
            slstatus = SLROS.status();
            slcrc = SLROS.crc();
            // fill the hit content for the first sector
            RpcSectorLogic* sl =
	      new RpcSectorLogic(sector,slfel1id,slbcid,slstatus,slcrc);
            bool inputHeaderFound = false;
            bool outputHeaderFound = false;
            ATH_MSG_VERBOSE("New RpcSectorLogic: sector=" << sector
			    << " fel1id=" << slfel1id
			    << " BCID=" <<slbcid);
            
            uint16_t rowinBcid = 999;
            uint16_t slPadId   = 999;
            
            for (int islwords = 0 ; islwords<sectorLogic->numberOfInputWords() ; ++islwords) {
              
              uint16_t ptid;
              uint16_t roi;
              uint16_t outerPlane; 
              uint16_t overlapPhi; 
              uint16_t overlapEta;
              
              uint16_t slWord = sectorLogic->readSLHitCurrent();
              
              ATH_MSG_VERBOSE("SLhit word: " << std::hex << slWord << std::dec);
              SLROS.decodeFragment(slWord,recField);
              
              uint16_t triggerBcid;
              
              if (SLROS.isInputHeader()) {
                rowinBcid = SLROS.inputRowinBcid();
                slPadId = SLROS.inputPadId();
                inputHeaderFound = true;
              }
              else if (SLROS.isInputBody()) {
                if (!inputHeaderFound) {
                  ATH_MSG_VERBOSE("ERROR: inputSLHeader missing !!");
                } else {
                  ptid = SLROS.inputThreshold();
                  roi  = SLROS.inputRoi();
                  outerPlane = SLROS.inputOuterPlane();
                  overlapPhi = SLROS.inputOverlapPhi();
                  overlapEta = SLROS.inputOverlapEta();
                  triggerBcid = SLROS.inputTriggerBcid();
                  
                  RpcSLTriggerHit* slHit =
		    new RpcSLTriggerHit(  rowinBcid,  slPadId,  ptid,  roi,
					  outerPlane,  overlapPhi,  overlapEta,
					  triggerBcid);
                  slHit->setIsInput(true);
                  sl->push_back(slHit);
                  ATH_MSG_VERBOSE("New input RpcSLTriggerHit: ptid, roi=  " << ptid << " " << roi);                  
                }
              }
              else if (SLROS.isOutputHeader()) {
                rowinBcid = SLROS.outputRowinBcid();
                outputHeaderFound=true;
              }
              else if (SLROS.isOutputBody()) {
                if (!outputHeaderFound) {
                  rowinBcid = 999;
                  if (msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) 
					       << "ERROR: outputSLHeader missing !!" << endmsg;
                }
                else {
                  if (SLROS.isOutputDecoded()) {
                    
                    outerPlane=0;
                    overlapPhi=0;
                    
                    for (int icand = 0 ; icand<SLROS.nTriggerCand() ; ++icand) {
                      
                      ptid = SLROS.outputThreshold(icand);
                      triggerBcid = SLROS.outputTriggerBcid(icand);
                      roi = SLROS.outputRoi(icand);
                      overlapEta=SLROS.outputOverlap(icand);
                      RpcSLTriggerHit* slHit =
			new RpcSLTriggerHit( rowinBcid,  0,  ptid,  roi,
					     outerPlane,  overlapPhi,  overlapEta,
					     triggerBcid);
                      slHit->setIsInput(false);
                      // use only one out od the two sectors for the moment (???)
                      sl->push_back(slHit);
                      ATH_MSG_VERBOSE("New output RpcSLTriggerHit: ptid, roi=  " << ptid << " " << roi);
                      
                    } 
                    
                    if (SLROS.hasMoreThan2TriggerCand()) {
                      sl->setHasMoreThan2TriggerCand(true);
                    }
                    
                  }
                }
              }
            }
            
            
            
            
            if(sectorLogicContainer) sectorLogicContainer->push_back(sl);
            
            if (sectorLogicContainer && 
                !sectorLogicContainer->setSector(sector,side) ) {
              ATH_MSG_VERBOSE("Sector " << sector 
			      << " decoded more than once in SL");
            }
            
          }
          
        } else {
          if ( SLBodyWords >= SL_data_sise ) {
            ATH_MSG_VERBOSE("Sector Logic payload corrupted");
            return StatusCode::FAILURE;
          }
          SLBuff[SLBodyWords]=currentWord;
          SLBodyWords++;
        } // isSLHeader
        
      } else if(isPadHeader || isPADFragment)  {

	// if (bsErrCheck_InRX || bsErrCheck_FromSLFooter || bsErrCheck_FromPDFooter) 
	//   {
	//     if (!isPadHeader) 
	//       {
	// 	bsErrCheck_errorInPDHeader = true;
	//       }
	//   }
	//bsErrCheck_InRX = false;
	// bsErrCheck_InSL = false;
	// bsErrCheck_InPD = true;
	// bsErrCheck_InCM = false;

        
        // Now decoding the header of the pad
        ATH_MSG_VERBOSE(" Pad Header or Pad Fragment " );
        
        PDROS.decodeFragment(currentWord,recField);
        
        if(recField == 'H') {
	  // Here's a PAD HEADER
	  nwInCM=0;
	  // Here reset the CM Fragment and the flag SL_Found to cope with corruption of earlier data fragments (missing sl footer)
	  (myRPC.CMFragment())->reset();
	  myRPC.setSLFragmentFound(false); 
	  
          //bsErrCheck_errorInPDHeader = false;
	  //bsErrCheck_FromPDHeader    = true;
	  //bsErrCheck_FromSLFooter    = false;
	  //bsErrCheck_FromPDFooter    = false;
           
          PadID=PDROS.padid();
          
          // special setup for Dec06 sector 13 data
          if (m_sector13Data) {
            if (PadID<3) {
              sector=56;
            }
            else {
              sector=55;
              // move the pad value to the 0-2 range;
              PadID -= 3;
            }
          }

          side = (sector<32) ? 0:1; // 0 = side C[z<0], 1 = side A[z>0]
          uint16_t sectorLogic = sector-side*32; // range is 0-31
	  //if (sector==22 || sector==23 || sector==26 || sector==27) msg(MSG::INFO) <<"Decoding Pad Header - side,sectorLogic= "<<side<<" "<<sectorLogic<<"  PadID/Sector "<<PadID<<"/"<<sector<<" need to get from cabling padOfflineId"<<endmsg;
	  //else 
	  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"Decoding Pad Header - side,sectorLogic= "<<side<<" "<<sectorLogic<<"  PadID/Sector "<<PadID<<"/"<<sector<<" need to get from cabling padOfflineId"<<endmsg;

          // get the offline ID of the pad
          if(!m_cabling->giveOffflineID(side,sectorLogic,PadID,padOfflineId))
	    {
	      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
					  << "Cannot retrieve the OfflineID for the PAD n. " 
					  << PadID << " at side " << side << " and  sector " 
					  << sectorLogic << endmsg;
	    } else {
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
					<< "PadOfflineId " << m_pRpcIdHelper->show_to_string(padOfflineId)
					<< " associated to PAD n. " << PadID << " at side " 
					<< side << " and  sectorLogic " << sectorLogic << endmsg; 
          }
          
          // check if it's the pad to convert
          //if (thisPadOfflineId == padOfflineId) {
	  std::map<Identifier,RpcPad*>::const_iterator myPadInList = vmap.find(padOfflineId);
          if (myPadInList!=vmap.end()){
            thisPadOfflineId = ((*myPadInList).second)->identify();
	    thisPadHashOfflineId = ((*myPadInList).second)->identifyHash();
	    thisv = (*myPadInList).second; // pointer to RpcPad collection to be filled (if in the list requested)
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
					<< " match found with ID " 
					<< m_pRpcIdHelper->show_to_string(thisPadOfflineId)
					<< " requested for the conversion; return this collection with hash " << thisPadHashOfflineId
					<< endmsg; 
            
            foundPad = true;
            
            thisv->setOnlineId(PadID);
            thisv->setSector(sector);
            
            // set the lvl1 id
            thisv->setLvl1Id(PDROS.l1id());
            
          }
          else
	    {
	      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
					  << " match NOT found with ID "  
					  << m_pRpcIdHelper->show_to_string(thisPadOfflineId)
					  << " requested for the conversion" << endmsg;  
	    }
          
        }
        


        // if it's a subheader, set the bcid
        if (recField == 'S') {
          //bsErrCheck_FromPDHeader = false;
          if (foundPad) {
            thisv->setBcId(PDROS.bcid());
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
					<< "Found the subheader, setting bcid to: " << PDROS.bcid() 
					<< endmsg;
          }
        }
	else 
	  // if (bsErrCheck_FromPDHeader)
	  //   {
	  //     bsErrCheck_errorInPDSubHeader = true;
	  //   }
        
        if (recField == 'P') {
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
				      << "Found the prefooter" << endmsg;
          if (foundPad) thisv->setStatus(PDROS.status());
          
          if ( currentWord&0x0fff ) {
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)
					<< "Pad Busy status not zero ! value: " << MSG::hex
					<< (currentWord & 0x0fff) << MSG::dec << endmsg;
          }
        }
        
        if(recField == 'F') {
	  if (msgLvl(MSG::VERBOSE)) {
	    msg(MSG::VERBOSE) << " Pad Footer " << endmsg;
	  }
	  if (PDROS.errorCode()!=0) 
	    {
	      ATH_MSG_VERBOSE("Pad Error flag not zero ! value: " << MSG::hex
			      << PDROS.errorCode() << MSG::dec);  
	    }	  
	  if (foundPad)
	    {
	      // found the pad, bail out
	      thisv->setErrorCode(PDROS.errorCode());
	      ++nDecodedPads;
	      foundPad=false;
	      if ((unsigned int)nDecodedPads==vmap.size())
		{
		  ATH_MSG_DEBUG("All pads requested are found and decoded"); 
		  if (!m_decodeSL)
		    {
		      ATH_MSG_DEBUG("... since no SL decoding is requested exit here");
		      return StatusCode::SUCCESS;
		    } 
		  else ATH_MSG_DEBUG("... keep going to allow full SL decoding");
		}
	      // M.C. : CONTINUE TO SCAN TILL THE END of the ROB TO FIND LAST SectorLogic:
	      /* if (!(PadID>3&&sector%2>0))
		 {
		 msg(MSG::VERBOSE) 
		 << "Found Last sector Data Fragment ????  PadID=("<<PadID<<")<=3 || sector="<<sector<<" is odd"<< endmsg;  
		 return StatusCode::SUCCESS;
                 }
	      */
	    }          
	}
        
	isPadFooter ? isPADFragment=false : isPADFragment=true; 
        
	if (msgLvl(MSG::VERBOSE))
	  {          
	    msg(MSG::VERBOSE) 
	      << " current word "<< MSG::hex << currentWord << MSG::dec << endmsg;
          
	    msg(MSG::VERBOSE) 
	      << " ==isPADFragment= " << isPADFragment << endmsg;
	    msg(MSG::VERBOSE) 
	      << " calling pushword: " << MSG::hex << currentWord << MSG::dec << endmsg;
	  }
        
	// here scrolling all PAD/CM fragments until a CM footer is found (foundCM>0 when a CM footer is found)
	//if (sector==22 || sector==23 || sector==26 || sector==27)  myRPC.enablePrintOut();
	int foundCM = 0;
	foundCM = myRPC.pushWord(currentWord,0);
	//std::cout<<" foundCM = "<<foundCM<<std::endl;
	//myRPC.disablePrintOut();
        
	if(foundCM==1) { // corresponds to CM footer found - all CM words stored in myRPC
          
	  //if (sector==22 || sector==23 || sector==26 || sector==27)  msg(MSG::INFO)<< " in PadID/Sector "<<PadID<<"/"<<sector<<" CM footer found: n. of words in CM including Footer "<<nwInCM<<myRPC.CMFragment() << endmsg;
	  //else 
	  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " in PadID/Sector "<<PadID<<"/"<<sector<< " CM footer found: n. of words in CM including Footer "<<nwInCM<<myRPC.CMFragment() << endmsg;
          
	  // If the pad is the good one, add the CMs to the container   
	  if (foundPad) {
	    MatrixReadOut* matrix = myRPC.CMFragment();
            
	    //std::cout << myRPC.CMFragment()<< std::endl;
            
	    matrixROS = matrix->getHeader();  
	    uint16_t cmaId  = matrixROS.cmid();
	    // special "feet" towers have "high-pt" cmaIds in hardware, transform to low-pt 
	    if ((sectorForCabling==21||sectorForCabling==22||sectorForCabling==25||sectorForCabling==26)&& 
		(PadID==2||PadID==4||PadID==5||PadID==7)&& 
		cmaId>=4)   cmaId=cmaId-4; 
	    //msg(MSG::DEBUG) << "sector, Pad, cmaid : "<< sectorForCabling << ", " << PadID << ", " <<  cmaId << endmsg; 	    
	    uint16_t fel1id = matrixROS.fel1id();
            
	    matrixROS = matrix->getSubHeader();
	    uint16_t febcid = matrixROS.febcid();

	    //if (sector==22 || sector==23 || sector==26 || sector==27)  msg(MSG::INFO) 
	    //<< "Creating a new CM, cmaId=" << cmaId << " fel1id=" << fel1id
	    //<< " febcid=" << febcid <<" in PadID/Sector "<<PadID<<"/"<<sector<<endmsg;
	    //else 
	    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
					<< "Creating a new CM, cmaId=" << cmaId << " fel1id=" << fel1id
					<< " febcid=" << febcid <<" in PadID/Sector "<<PadID<<"/"<<sector<<endmsg;
            
	    // Create the new cm
	    RpcCoinMatrix * coinMatrix = new RpcCoinMatrix (padOfflineId,cmaId,fel1id,febcid);
            
	    matrixROS = matrix->getFooter();
	    coinMatrix->setCRC(matrixROS.crc());
            
	    //std::cout << matrix->numberOfBodyWords() << std::endl;
            
	    // Loop on the hits and push them in the coin matrix
	    for (int i=0 ; i<matrix->numberOfBodyWords() ; ++i) {
              
	      matrixROS = matrix->getCMAHit(i);
              
	      uint16_t bcid = matrixROS.bcid();
	      uint16_t time = matrixROS.time();
	      uint16_t ijk  = matrixROS.ijk();
              
	      RpcFiredChannel* firedChan=0;
              
	      if (ijk < 7 ) {
		uint16_t channel = matrixROS.channel();
		firedChan = new RpcFiredChannel(bcid,time,ijk,channel);
                
		
		//if (sector==22 || sector==23 || sector==26 || sector==27)
		//ATH_MSG_INFO (
		//		 "Adding a fired channel, bcid=" << bcid << " time=" 
		//		 << " ijk=" << ijk << " channel=" << channel<< " in PadID/Sector "<<PadID<<"/"<<sector);
		//else
		ATH_MSG_VERBOSE (
				 "Adding a fired channel, bcid=" << bcid << " time=" 
				 << " ijk=" << ijk << " channel=" << channel<< " in PadID/Sector "<<PadID<<"/"<<sector);
                
		// add the fired channel to the matrix
		coinMatrix->push_back(firedChan); 
	      }
	      else if (ijk==7) {
		uint16_t overlap   = matrixROS.overlap();
		uint16_t threshold = matrixROS.threshold();
		firedChan = new RpcFiredChannel(bcid,time,ijk,threshold,overlap);
                
		ATH_MSG_VERBOSE (
				 "Adding a fired channel, bcid=" << bcid << " time=" 
				 << " ijk=" << ijk << " overlap=" << overlap 
				 << " threshold=" << threshold);
                
		// add the fired channel to the matrix
		coinMatrix->push_back(firedChan);
                
	      }
	    }
            
	    thisv->push_back(coinMatrix);
            
	  } // end of the matrix decoding
          
	  (myRPC.CMFragment())->reset(); // reset to start storing a new CM 
          nwInCM=0;
	} // end of the CM decoding inside the PAD data fragment
	else
	  {
	    ++nwInCM;
	    ATH_MSG_VERBOSE ("generic CM word - counting words in this cm " << nwInCM);
	  }

	/*
	// here check pad condition errors (Niko)
	int decoded = (p[i] & 0xf000)>> 12 ;
	msg(MSG::DEBUG) << "WHEN looking for errors in pad fragment ---- word " << i <<" = "<< MSG::hex << p[i] << MSG::dec 
			  << " " << MSG::hex << decoded << MSG::dec << endmsg;
	if (decoded<0x4) msg(MSG::INFO) << "Unexpected word WHEN looking for errors in pad fragment ---- word " << i <<" = "<< MSG::hex << p[i] << MSG::dec 
			  << " " << MSG::hex << decoded << MSG::dec << endmsg;

	*/

      } // end of if is pad fragment or pad header
        
    } // end of word scan 
  ATH_MSG_DEBUG ("End of FillCollectionsForRob_v302 | nPadsDecoded/SL/RX "<<nDecodedPads<<"/"<<nDecodedSL<<"/"<<nDecodedRX);
  return StatusCode::SUCCESS;
      
}

  
