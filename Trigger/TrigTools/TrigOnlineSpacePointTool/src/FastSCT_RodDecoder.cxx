/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigOnlineSpacePointTool/FastSCT_Clusterization.h"
#include "TrigOnlineSpacePointTool/FastSCT_RodDecoder.h"
#include "GaudiKernel/ListItem.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "ByteStreamData/ROBData.h" 
#include <algorithm> 

static const InterfaceID IID_IFastSCT_RodDecoder
            ("FastSCT_RodDecoder", 1, 0);

//constructor
FastSCT_RodDecoder::FastSCT_RodDecoder
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent)
{
  declareInterface< FastSCT_RodDecoder  >( this );
}

//destructor  
FastSCT_RodDecoder::~FastSCT_RodDecoder() {

}

const InterfaceID& FastSCT_RodDecoder::interfaceID( )
{ return IID_IFastSCT_RodDecoder;  }

StatusCode FastSCT_RodDecoder::initialize() {

  ATH_MSG_INFO(" initialize "); 
  StatusCode sc = AthAlgTool::initialize(); 

  sc = detStore()->retrieve(m_indet_mgr,"SCT"); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Cannot retrieve SCT_DetectorManager!");
    return StatusCode::FAILURE;
  } 
  
  if (detStore()->retrieve(m_sct_id, "SCT_ID").isFailure()) {                       
     ATH_MSG_FATAL("Could not get SCT ID helper");
     return StatusCode::FAILURE;
  }  

  m_cntx_sct = m_sct_id->wafer_context();
  return StatusCode::SUCCESS;
}

StatusCode FastSCT_RodDecoder::finalize() {

  ATH_MSG_INFO("in FastSCT_RodDecoder::finalize");
  return StatusCode::SUCCESS;
}

bool FastSCT_RodDecoder::fillCollections(const ROBFragment* rob, uint32_t robid, 
					ISCT_CablingSvc* cablingSvc,
					std::vector<bool>& listOfIds, FastSCT_Clusterization* pClusterization) 
{
  m_pClusterization=pClusterization;
  m_cablingSvc=cablingSvc;

  // get the ROD version. It could be used to decode the data in one way or another
  /*
  eformat::helper::Version rodVersion(rob->rod_version()); 
  //  const uint16_t rodMinorVersion = rodVersion.minor() ;
  eformat::helper::SourceIdentifier sid_rob =  rob->rod_source_id() ;
  uint32_t detid = sid_rob.subdetector_id();
  if(outputLevel <= MSG_DEBUG)
    ATH_MSG_DEBUG<<"The Det Id 0x"<<std::hex<<detid<<" robid 0x"<<robid<<" Rod version "<<rodMinorVersion
       <<std::dec<<endreq ;
  */
  int nNewStrips=0;

  int strip = 0;
  int oldstrip = -1;	
  int chip = 0;		
  int side = 0;
  int linkNb = 0 ;
  uint32_t onlineId = 0;

  //  int tbin = 0;
  int groupSize = 0;	
  bool condensed = true;	
  // bool ErrorHeader = false;
  // bool ErrorTrailer = false;
  int saved[768];
  float errorHit[20];
  int ABCerror = 0;
  int wordcount = -1;
  int n;	
  int ERRORS = 0; //encodes the errors on the header
                  // bit 0: Preamble ERROR
                  // bit 1: LVL1 ID ERROR
                  // bit 2: BCID ERROR
                  // bit 3: error in formatter
                  // bit 4: error in condensed mode 1rst hit
                  // bit 5: error in condensed mode 2nd hit

  memset(&saved[0],0,sizeof(saved));
  for(int i=0; i<20;  i++) errorHit[i] = 0;

  //For the BS Time Out Error
  std::vector<uint32_t>  TimeOutErrOnlineIds ;

 // loop over the ROB  iterator over 32 bits words

  ROBData::iterator it1 = ((ROBData) rob).begin() ;
  ROBData::iterator it2 = ((ROBData) rob).end() ;

  for(; it1!=it2; ++it1) 
    {
      wordcount++;
      /*
      if(outputLevel <= MSG_DEBUG)
	ATH_MSG_DEBUG<<"fillCollection() word no "<<std::dec<<wordcount<<", input rod : "<<std::hex<<*rob_it1
	   <<" :-> robid 0x"<<robid<<endreq ;
      */
      // the data is 16-bits wide packed to a 32-bit word (rob_it1). So we unpack it here.
      uint16_t d[2];
      d[1] = (*it1) & 0xFFFF; 
      d[0] = ((*it1)>>16) & 0xFFFF; 
      for( n=0;n<2;n++)
	{
	  uint16_t dataWord=d[n];

	  if (dataWord&0x8000) 
	    {
	      if (condensed)
		{
		  /** Condensed mode
		   *  chip info : 4 bits  dataWord>>11)0xF
		   *  chip number == (dataWord>>11)&0x7 and chip side == (dataWord>>14)&0x1
		   *  For example if dataWord>>11)0xF = 0101 => chip5 or chip5 on 
		   side0, dataWord>>11)0xF = 1101 => 
		   chip13 or chip5 on side1
		  */
		  chip  = ((dataWord>>11)&0x7) ; 
		  side = ((dataWord>>14)&0x1) ;
		  strip = chip*128 + ((dataWord>>4)&0x7F) ;
		  //tbin = 0x2 ; //assuming tbin is 010 in condensed mode
		  if(chip>5)
		    {
		      ATH_MSG_ERROR("    Hit condensed : xxx Chip number = "<<chip<<" > 5 "
			  <<" for hit "<<std::hex<<dataWord);
		      continue ;
		    }
		  //------------ Search for redundancy only for the master chip 
		  if ((side == 1) && ((linkNb%2)==0)) linkNb++ ;
		  if ((side == 0) && ((linkNb%2)!=0)) linkNb-- ;

		  //Kondo 14/02/07: New calculation of onlineId
		  //onlineId = (robid |(linkNb << 8));
		  onlineId = ((robid & 0xFFFFFF)|(linkNb << 24));
		  /*
		    if(outputLevel <= MSG_DEBUG)
		    ATH_MSG_DEBUG<<"    Hit condensed ->hit: "<<std::hex<<dataWord<<" link "<<
		    linkNb<<" robid "<<robid<<" onlineId "<<onlineId<<std::dec<<endreq ;
		  */
		  if (groupSize == 0) 
		    { //if it's the first condensed word
		      oldstrip = strip;
		    }
		  if (!(dataWord&0x1))
		    { // 1-hit
		      /*
		      if(outputLevel <= MSG_DEBUG)
			ATH_MSG_DEBUG<<"    Hit condensed -> 1-hit : "<<std::hex<<dataWord<<" side "<<side<<" link "<<
			  linkNb<<" chip "<<chip<<" strip "<<strip<<" groupSize "<<groupSize<<std::dec<<endreq ;
		      */
		      if (strip != oldstrip)
			{ //if it is a new cluster,  make RDO with the previous cluster
			  nNewStrips+=addNewStrip(oldstrip,groupSize,onlineId,ERRORS,errorHit, listOfIds);
			  saved[oldstrip]=1;
			  oldstrip = strip;
			  groupSize = 0;	
			  /*
			    if(outputLevel <= MSG_DEBUG)
			    ATH_MSG_DEBUG<< "    Hit condensed : New cluster, make RDO with the previous cluster" << 
			    endreq;
			  */
			}
		      if (dataWord&0x4)
			{ // Error in the hit
			  // ATH_MSG_ERROR << "    Hit condensed : xxx ERROR in 1-hit "<<std::hex<<dataWord<< endreq ;
			  errorHit[groupSize] = 1;
			  ERRORS = (ERRORS | 0x10);
			}
		      groupSize++ ;
		    }
		  else
		    { //2-hits
		      /*
			if(outputLevel <= MSG_DEBUG)
			ATH_MSG_DEBUG<<"    Hit condensed -> 2-hits : "<<std::hex<<dataWord<<" side "<<side<<" link "<<
			linkNb<<" chip "<<chip<<" strip "<<strip<<" groupSize "<<groupSize<<std::dec<<endreq ;
		      */
		      if (strip != oldstrip) 
			{ //if it is a new cluster, make RDO with the previous cluster
			  nNewStrips+=addNewStrip(oldstrip,groupSize,onlineId,ERRORS,errorHit,listOfIds);
			  saved[oldstrip]=1;	 
			  oldstrip = strip;
			  groupSize = 0;
			  /*
			    if(outputLevel <= MSG_DEBUG)		
			    ATH_MSG_DEBUG<<"Hit condensed : New cluster, make RDO with the previous cluster"
			    <<endreq;
			  */
			}	    
		      if (dataWord&0x4) 
			{ // Error in the first hit
			  // ATH_MSG_ERROR << "    Hit condensed : xxx ERROR in 1st hit"<<std::hex<<dataWord<< endreq ;
			  errorHit[groupSize] = 1 ;
			  ERRORS = (ERRORS | 0x10) ;
			}
		      groupSize++ ;	    
		      if (dataWord&0x8) 
			{ // Error in the second hit
			  // ATH_MSG_ERROR << "    Hit condensed : xxx ERROR in 2nd hit "<<std::hex<<dataWord<< endreq ;
			  errorHit[groupSize] = 1 ;
			  ERRORS = (ERRORS | 0x20) ;
			}
		      groupSize++ ;
		    }
		}
	      else 
		{ 
		  /** Expanded mode
		   * chip info from the first word of expanded cluster : 4 bits  dataWord>>11)0xF
		   * chip number == (dataWord>>11)&0x7 and chip side == (dataWord>>14)&0x1
		   * For example if dataWord>>11)0xF = 0101 => chip5 or chip5 on side0, dataWord>>11)0xF = 1101 => 
		                                                chip13 or chip5 on side1
		  */

		  if (!(dataWord&0x8)) 
		    {  // 1st hit cluster expanded
		      /*
			if(outputLevel <= MSG_DEBUG)
			ATH_MSG_DEBUG<<" Expanded hit : -> First hit "<<endreq ;
		      */
		      chip  = ((dataWord>>11)&0x7) ;  
		      side = ((dataWord>>14)&0x1) ;
		      strip = chip*128 + ((dataWord>>4)&0x7F) ;
		      //tbin = dataWord&0x7 ; 
		      // Real way for obtaining tbin info
		      //	    tbin = 0x2 ; 
		      //assuming tbin is 010 i expanded mode for debugging purposes
		      if(chip>5)
			{
			  ATH_MSG_ERROR("\tExpanded hit: First hit xxx ERROR chip Nb = " <<chip<< " > 5");
			  continue ;
			}
		      /*
		      if(outputLevel <= MSG_DEBUG)
			ATH_MSG_DEBUG<<"\tExpanded hit: -> * side "<<side<<", Link "<<linkNb<<", chip "<<chip
			   <<", strip "<<strip<<endreq ;
		      */
		      //-------------- Search for redundancy only for the master chip 
		      if ((side == 1) && ((linkNb%2)==0))  linkNb++ ;
		      if ((side == 0) && ((linkNb%2)!=0))  linkNb-- ;

		      //Kondo 14/02/07: New calculation of onlineId 
		      //onlineId = (robid |(linkNb << 8)) ;
		      onlineId =((robid & 0xFFFFFF)|(linkNb << 24)); 
		      /*
			if(outputLevel <= MSG_DEBUG)
			ATH_MSG_DEBUG<<"    Hit expanded ->hit: "<<std::hex<<dataWord<<" link "<<linkNb<<" robid "<<
			robid<<" online Id "<<onlineId<<std::dec<<endreq ;
		      */
		      groupSize =  1 ;
		      nNewStrips+=addNewStrip(strip,groupSize,onlineId,ERRORS,errorHit,listOfIds) ;
		      saved[strip] = 1;
		      groupSize = 0 ;
		    }
		  else 
		    {  // next hits cluster expanded
		      if (dataWord&0x80) 
			{ // paired hits 
			  // first hit from the pair
			  /*
			    if(outputLevel <= MSG_DEBUG)
			    ATH_MSG_DEBUG<<"\tExpanded hit: -> Paired hits "<<endreq ;	
			  */ 
			  if(chip>5)
			    {
			      ATH_MSG_ERROR("\tExpanded Hit: paired hits xxx ERROR chip Nb = "<<chip<<" > 5"); 
			      continue ;
			    }
			  strip++;
			  // tbin = dataWord&0x7;
			  groupSize = 1;
			  nNewStrips+=addNewStrip(strip,groupSize,onlineId,ERRORS,errorHit,listOfIds);
			  saved[strip] = 1;
			  // second hit from the pair
			  strip++;
			  // tbin = (dataWord >> 4) & 0x7 ;
			  nNewStrips+=addNewStrip(strip,groupSize,onlineId,ERRORS,errorHit,listOfIds);
			  saved[strip] = 1;
			  groupSize = 0;
			}
		      else 
			{  // Last hit of the cluster
			  /*
			  if(outputLevel <= MSG_DEBUG)
			    ATH_MSG_DEBUG<<" Expanded hit: -> Last hits "<<endreq ;	
			  */ 
			  if(chip>5)
			    {
			      ATH_MSG_ERROR("\tExpanded Hit: last hit xxx ERROR chip Nb = "<<chip<<" > 5");	  
			      continue ;
			    }
			  strip++;
			  // tbin = dataWord&0x7;
			  groupSize = 1;
			  nNewStrips+=addNewStrip(strip,groupSize,onlineId,ERRORS,errorHit,listOfIds);
			  saved[strip] = 1;  
			  groupSize = 0; 
			}
		    }
		} // end non-condensed mode
	    }// end of hit element
	  else if (((dataWord>>13)&0x7) == 0x1)
	    {
	      /*
	      if(outputLevel <= MSG_DEBUG)
		ATH_MSG_DEBUG<<"HEADER   "<<std::hex<<dataWord<<endreq ;
	      */
	      if (saved[strip]==0 && oldstrip>=0)
		{
		  /*
		  if(outputLevel <= MSG_DEBUG)
		    {
		      ATH_MSG_DEBUG<<"    Header -> saved["<<strip<< "] = "<<saved[strip]
			 <<" && oldstrip>=0 (hit) => Make RDO  "<<endreq ;
		      ATH_MSG_DEBUG<<"    Header -> 1-hit : "<<std::hex<<dataWord<<", GroupSize "<<
			groupSize<<", Link "<<linkNb<<", chip "<<chip<<std::dec<<" strip "<<strip<< endreq ;
		    }
		  */
		  nNewStrips+=addNewStrip(strip, groupSize++, onlineId, ERRORS, errorHit,listOfIds);	 
		  saved[strip] = 1;
		}
	      //Every thing is set to default for a new hunt of RDO
	      strip =0 ;
	      oldstrip = -1 ;
	      groupSize = 0 ;
	      // ErrorHeader = false ;
	      ERRORS = 0 ;

	      memset(&saved[0],0,sizeof(saved));
	      for(int i=0; i<20; i++)  errorHit[i] = 0 ;

	      // Link Number (or stream) in the ROD fragment
	      
	      int rodlinkNb = dataWord & 0x7F ;  
	      // This is the real calculation	for the offline  
	      linkNb  = ((( rodlinkNb >>4) & 0x7)*12+(rodlinkNb & 0xF));
	      /*
	      if(outputLevel <= MSG_DEBUG)
		ATH_MSG_DEBUG<<"Header: Link nb  "<<std::hex<<linkNb<<" ROD Link Number "<<rodlinkNb<<endreq ;
	      */
	      if (linkNb > 95) 
		{
		  ATH_MSG_ERROR("Header: xxx Link nb out of range (skipping following data)"<<std::dec<<linkNb);
		  break;
		}
	      if (dataWord&0x100)
		{
		  /*
		    if(outputLevel <= MSG_DEBUG)
		    ATH_MSG_DEBUG<< "    Header: -oo- Condensed Mode " << endreq;
		  */
		  condensed = true;
		} 
	      else 
		{
		  /*
		  if(outputLevel <= MSG_DEBUG)
		    ATH_MSG_DEBUG << "    Header: -oo- Expanded Mode " << endreq;
		  */	
		  condensed = false;
		}
	      // if (dataWord&0x800)
	      //	{
	      //	  ErrorHeader = true;	
	      //	}
	      if (dataWord&0x1000)
		{
		  //  ErrorHeader = true ;
		  ATH_MSG_ERROR("    Header: xxx Preamble Error");
		  ERRORS = (ERRORS | 0x1);
		}
	      if (dataWord&0x400)
		{ 
		  // ErrorHeader = true;
		  ATH_MSG_ERROR("    Header: xxx LVL1 ID Error ");
		  ERRORS = (ERRORS | 0x2);
		}
	      if (dataWord&0x200)
		{
		  // ErrorHeader = true;
		  ATH_MSG_ERROR("    Header: xxx BCID Error ");
		  ERRORS = (ERRORS | 0x4);
		}
	      if ((dataWord&0xF) > 11) 
		{
		  // ErrorHeader = true;
		  ATH_MSG_WARNING("    Header: xxx Error in formatter ");
		  ERRORS = (ERRORS | 0x8);
		}
	      continue;	
	    } // end header
	  //---------------------------------------------------------------------
	  // trailer
	  //---------------------------------------------------------------------
	  else if (((dataWord>>13)&0x7) == 0x2)
	    { 
	      //  ErrorTrailer = false;
	      if (dataWord&0x1000)
		{
		  //  ErrorTrailer = true;
		  ATH_MSG_ERROR("    Trailer: xxx Trailer ERROR " << std::hex << dataWord);
		}
	      if (dataWord&0x800)
		{
		  // ErrorTrailer = true;	// no data should appear between header and trailer
		  ATH_MSG_ERROR("    Trailer: xxx Header-Trailer limit ERROR " << std::hex << dataWord);
		}
	      if (dataWord&0x400)
		{
		  // ErrorTrailer = true; // not sure if there are hit elements before 
		  //(probably yes but in principle they are fine
		}
	      continue; 
	    }
	  //---------------------------------------------------------------------
	  // Flagged error
	  //---------------------------------------------------------------------
	  else if (((dataWord>>13)&0x7) == 0x0)
	    {
	      chip = ((dataWord>>3)&0x7) ;
	      ABCerror = dataWord&0x7; 
	      // no data should appear for that chip but how do we want to transmit this information ?
	      ATH_MSG_ERROR(" xxx Flagged ERROR in chip "<<chip<<" Error code ABCerror "<<
		ABCerror<<" Link Nb (or Stream) "<<linkNb);
	      continue;
	    }
      
	  //---------------------------------------------------------------------
	  // Raw Data
	  //---------------------------------------------------------------------
	  else if (((dataWord>>13)&0x7) == 0x3)
	    {
	      ATH_MSG_ERROR(" xxx Raw Data Mode "<<std::hex<<dataWord);
	      // too many errors in the BS for the ROD to decode the data
	      continue;
	    }
	  //---------------------------------------------------------------------
	  // hit element
	  //---------------------------------------------------------------------
	  else 
	    {
	      ATH_MSG_ERROR("Data word format unknown ");
	    }
	}
    } // end of 32-bit word loop
  
  // create RDO of the last link or stream of the event
  if (saved[strip]==0 && oldstrip>=0)
    {
      nNewStrips+=addNewStrip(strip, groupSize++, onlineId, ERRORS, errorHit,listOfIds);
      saved[strip] = 1;
    }
  /*
  if(outputLevel <= MSG_DEBUG)
    ATH_MSG_DEBUG << std::dec<<wordcount<<" datawords processed, total "<<nNewStrips
	<< " strips are submitted for clusterization "<< endreq;
  */
  return (nNewStrips!=0);
}

int FastSCT_RodDecoder::addNewStrip(int Strip0, int groupSize, uint32_t onlineId, 
                                    int /*ERRORS*/, float /*errorHit*/[20], std::vector<bool>& listOfIds)

{
  const IdentifierHash hashId = m_cablingSvc->getHashFromOnlineId(onlineId) ;

  if(!listOfIds[hashId]) 
    {
      //  if(outputLevel <= MSG_DEBUG)
      //	ATH_MSG_DEBUG<< "Hash "<<(int)hashId<<" is NOT requested "<<endreq ;
      return 0;
    }

  Identifier idColl;

  if ( m_sct_id->get_id(hashId,idColl,&m_cntx_sct) ) 
    {
      // ATH_MSG_ERROR<< "Unknown offlineId for OnlineId 0x" <<std::hex<<onlineId <<" -> cannot add strip"<<endreq ;
      return 0;
    }

  const InDetDD::SiDetectorElement * p_element = (m_indet_mgr->getDetectorElement(hashId));
  int iStrip,strip;
  if (p_element->swapPhiReadoutDirection()) 
    {
      for(iStrip=0;iStrip<groupSize;iStrip++)
	{
	  strip=Strip0;
	  strip = 767 - strip ;
	  strip = strip-(groupSize-1)+iStrip;
	  /*
	  if(outputLevel <= MSG_DEBUG)
	    ATH_MSG_DEBUG<< "add strip " <<strip<<" groupSize="<<groupSize<<" HashId "<<(int)hashId<<"  "<<
	      m_sct_id->print_to_string(idColl)<<endreq;
	  */
	  m_pClusterization->addHit(idColl, hashId,strip);
	}
    }
  else
    {
      // if(outputLevel <= MSG_DEBUG)
      //	ATH_MSG_DEBUG<< "nonswapped readout" <<endreq;
      for(iStrip=0;iStrip<groupSize;iStrip++)
	{
	  strip=Strip0+iStrip;
	  /*
	  if(outputLevel <= MSG_DEBUG)
	    ATH_MSG_DEBUG<< "add strip " <<strip<<" groupSize="<<groupSize<<" HashId "<<(int)hashId<<"  "<<
	      m_sct_id->print_to_string(idColl)<<endreq;
	  */
	  m_pClusterization->addHit(idColl, hashId,strip);
	}
    }
  return groupSize;
}
