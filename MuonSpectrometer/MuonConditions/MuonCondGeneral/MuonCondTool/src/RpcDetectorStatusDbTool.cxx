/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "MuonIdHelpers/RpcIdHelper.h"

#include "PathResolver/PathResolver.h"
#include <fstream>
#include <string>
#include <stdio.h>
#include <map>
#include "MuonCondInterface/IRpcDetectorStatusDbTool.h" 
#include "MuonCondTool/RpcDetectorStatusDbTool.h"


#include "MuonCondSvc/MdtStringUtils.h"


RpcDetectorStatusDbTool::RpcDetectorStatusDbTool (const std::string& type,
                             const std::string& name,
                             const IInterface* parent)
  : AthAlgTool(type, name, parent), 
    m_FirstCall(true),
    m_detStore(0),
    m_IOVSvc(0),
    m_log( msgSvc(), name ),
    m_debug(false),
    m_verbose(false)
 
{

  declareInterface< IRpcDetectorStatusDbTool >(this) ;

  
  m_rpcDqStatusDataLocation="RpcDqStatusKey";

  m_rpcIdHelper = 0;
  declareProperty("RpcDqFolder",m_FolderName="/RPC/DQMF/ELEMENT_STATUS");
  declareProperty("RPC_PanelEfficiency",m_RPC_PanelEfficiency=0.50);

}




//StatusCode RpcDetectorStatusDbTool::updateAddress(SG::TransientAddress* tad)
StatusCode RpcDetectorStatusDbTool::updateAddress(StoreID::type /*storeID*/,
                                                  SG::TransientAddress* tad,
                                                  const EventContext& /*ctx*/)
{
  //MsgStream log(msgSvc(), name());
    // CLID clid        = tad->clID();
    std::string key  = tad->name();
    return StatusCode::FAILURE;
}





StatusCode RpcDetectorStatusDbTool::initialize()
{ 
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;
  
  m_FirstCall = true ;
 
  if( m_debug )  m_log << MSG::INFO << "Initializing" << endmsg;
  
   
  StatusCode sc = serviceLocator()->service("DetectorStore", m_detStore);
  if ( sc.isSuccess() ) {
    if( m_debug )  m_log << MSG::DEBUG << "Retrieved DetectorStore" << endmsg;
  }else{
    m_log << MSG::ERROR << "Failed to retrieve DetectorStore" << endmsg;
    return sc;
  }

  // Get interface to IOVSvc
  m_IOVSvc = 0;
  bool CREATEIF(true);
  sc = service( "IOVSvc", m_IOVSvc, CREATEIF );
  if ( sc.isFailure() )
  {
       m_log << MSG::ERROR << "Unable to get the IOVSvc" << endmsg;
       return StatusCode::FAILURE;
  }

  if(sc.isFailure()) return StatusCode::FAILURE;

  sc = m_detStore->retrieve(m_rpcIdHelper, "RPCIDHELPER" );
  if (sc.isFailure())
    {
      m_log<< MSG::FATAL << " Cannot retrieve RpcIdHelper " << endmsg;
      return sc;
    }
  
                                                             
  
  return sc;
}

StatusCode RpcDetectorStatusDbTool::loadParameterStatus(IOVSVC_CALLBACK_ARGS_P(I,keys)){
  
  StatusCode sc=StatusCode::SUCCESS;
  
  std::list<std::string>::const_iterator itr;

  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    
    m_log << MSG::INFO << *itr << " I="<<I<<" ";
   
  
     if(*itr==m_FolderName) {
       loadRpcDqStatus(I,keys);
       
     }
    
  }
  
  return sc;

}

StatusCode RpcDetectorStatusDbTool::loadRpcDqStatus(IOVSVC_CALLBACK_ARGS_P(I,keys)) 
{
  
  
  StatusCode sc=StatusCode::SUCCESS;
   
  m_log << MSG::INFO << "Load Rpc Status flags  from DB" << endmsg;
   
  
   
  // Print out callback information
  if( m_debug )  m_log << MSG::DEBUG << "Level " << I << " Keys: ";
   
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt) if( m_debug )  m_log << MSG::DEBUG << *keyIt << " ";
  if( m_debug )  m_log << MSG::DEBUG << endmsg;
   
     

  const CondAttrListCollection * atrc;
  sc=m_detStore->retrieve(atrc,m_FolderName);
  if(sc.isFailure())  {
    m_log << MSG::ERROR 
	  << "could not retreive the CondAttrListCollection from DB folder " 
	  << m_FolderName << endmsg;
    return sc;
  }
   
  CondAttrListCollection::const_iterator itr;
  CondAttrListCollection::ChanNum channum;
   

    
  int iEntry=0;
  if( m_debug )  m_log << MSG::DEBUG << "Folder Size " <<atrc->size() << endmsg;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
    const coral::AttributeList& atr=itr->second;
    channum=itr->first;
    std::string eff_panel, striplist, eff; 
     
    //Identifier chamberId = m_rpcIdHelper->elementID(Identifier(channum));
    Identifier  chamberId ;
    chamberId = channum;     
     
    ++iEntry;
     
    eff_panel=*(static_cast<const std::string*>((atr["PanelRes"]).addressOfData()));
    striplist=*(static_cast<const std::string*>((atr["StripStatus"]).addressOfData()));
     
    if( m_debug )  {
      m_log << MSG::DEBUG << "-----------------------------entry #"<<iEntry<<" total # of entries in the folder ="<<atrc->size()<<endmsg;
      m_log << MSG::DEBUG << "channel ID = Panel ID " <<  channum <<" as identif. = "<<m_rpcIdHelper->show_to_string(chamberId)<< endmsg; 
      m_log << MSG::DEBUG << "eff_panel load is " << eff_panel << endmsg;
      m_log << MSG::DEBUG << "striplist load is " << striplist << " "<< striplist.size() << endmsg;
    } 
    std::string delimiter = " ";
    std::vector<std::string> info_panel;
    std::vector<float> info_panel_test;
    MuonCalib::MdtStringUtils::tokenize(eff_panel,info_panel,delimiter);
              
    char * ch_tmp;
    char * ch_panel;
	     
    if (m_verbose)
      {
	for(unsigned int i=0; i<info_panel.size();i++){       
	  ch_tmp= const_cast<char*>(info_panel[i].c_str());       
	  m_log << MSG::VERBOSE << " info_panel " << ch_tmp << endmsg;
	  // version(3), # tracks, #strips, eff, errors, eff_gap, errors, res_cs1, errors, res_cs2, errors, res_csother, errors,
	  // noise, errors, noise_cor, errors, cluster_size, errors, frac_cs1, frac_cs2, frac_cs3_8, aver_cs3_8, frac_cs9_up*100, aver_cs9_up 
	}
      }

    char * SDBversion = const_cast<char*>(info_panel[0].c_str());    
    int DBversion = atoi( SDBversion ) ;
    if (m_FirstCall) m_log << MSG::INFO << " DB version  "   << info_panel[ 0] <<" Size of info_panel array = "<<info_panel.size()<<endmsg;
 
    if( m_FirstCall  && DBversion > 2) { 
      m_log << MSG::INFO << " DB version  "   << info_panel[ 0] ;
      m_log << MSG::INFO << " # Tracks    "   << info_panel[ 1] ;
      m_log << MSG::INFO << " # Strips    "   << info_panel[ 2] ; 
      m_log << MSG::INFO << " eff	   "   << info_panel[ 3] ;
      m_log << MSG::INFO << " errors	   "   << info_panel[ 4] ;
      m_log << MSG::INFO << " eff_gap     "   << info_panel[ 5] ;
      m_log << MSG::INFO << " errors	   "   << info_panel[ 6] ;
      m_log << MSG::INFO << " res_cs1     "   << info_panel[ 7] ;
      m_log << MSG::INFO << " errors	   "   << info_panel[ 8] ;
      m_log << MSG::INFO << " res_cs2     "   << info_panel[ 9] ;
      m_log << MSG::INFO << " errors	   "   << info_panel[10] ;
      m_log << MSG::INFO << " res_csother "   << info_panel[11] ;
      m_log << MSG::INFO << " errors	   "   << info_panel[12] << endmsg ;
      m_log << MSG::INFO << " noise	   "   << info_panel[13] ;
      m_log << MSG::INFO << " errors	   "   << info_panel[14] ;
      m_log << MSG::INFO << " noise_cor   "   << info_panel[15] ;
      m_log << MSG::INFO << " errors	   "   << info_panel[16] ;
      m_log << MSG::INFO << " clustersize "   << info_panel[17] ;
      m_log << MSG::INFO << " errors	   "   << info_panel[18] ;
      m_log << MSG::INFO << " frac_cs1    "   << info_panel[19] ;
      m_log << MSG::INFO << " frac_cs2    "   << info_panel[20] ;
      m_log << MSG::INFO << " frac_cs3_8  "   << info_panel[21] ;
      m_log << MSG::INFO << " aver_cs3_8  "   << info_panel[22] ;
      m_log << MSG::INFO << " frac_cs9_up%"   << info_panel[23] ;
      m_log << MSG::INFO << " aver_cs9_up "   << info_panel[24] << endmsg;
    }
     
     
    char * SNStrip = const_cast<char*>(info_panel[2].c_str());    
    int npanelstrip = atoi( SNStrip ) ;
   
    char * SProjectedTracks=const_cast<char*>(info_panel[1].c_str());
    double ProjectedTracks = atof(SProjectedTracks);
    //m_RPC_PanelProjectedTracksMap.insert(std::make_pair(chamberId,ProjectedTracks));
    m_RPC_PanelProjectedTracksMap[chamberId]=ProjectedTracks;

    if( m_debug )  m_log << MSG::DEBUG << "Size of  RPC_PanelProjectedTracksMap " << m_RPC_PanelProjectedTracksMap.size() << "; in panel " <<  channum << " SProjectedTracks " << SProjectedTracks << " ProjectedTracks " << ProjectedTracks << endmsg;     

    char * SEfficiency=const_cast<char*>(info_panel[3].c_str());
    double Efficiency = atof(SEfficiency);
    //m_RPC_PanelEfficiencyMap.insert(std::make_pair(chamberId,Efficiency));
    m_RPC_PanelEfficiencyMap[chamberId]=Efficiency;

    if( m_debug )  m_log << MSG::DEBUG << "Size of  RPC_PanelEfficiencyMap " << m_RPC_PanelEfficiencyMap.size() << "; in panel " <<  channum << " SEfficiency " << SEfficiency << " Efficiency " << Efficiency << endmsg;     

    char * SGapEfficiency=const_cast<char*>(info_panel[5].c_str());
    double GapEfficiency = atof(SGapEfficiency);
    //m_RPC_GapEfficiencyMap.insert(std::make_pair(chamberId,GapEfficiency));
    m_RPC_GapEfficiencyMap[chamberId]=GapEfficiency;

    if( m_debug )  m_log << MSG::DEBUG << "Size of  RPC_GapEfficiencyMap " << m_RPC_GapEfficiencyMap.size() << "; in panel " <<  channum << " SGapEfficiency " << SGapEfficiency << " GapEfficiency " << GapEfficiency << endmsg;

    char * SMeanClusterSizeMap=const_cast<char*>(info_panel[17].c_str());
    double MeanClusterSizeMap = atof(SMeanClusterSizeMap);
    //m_RPC_PanelMeanClusterSizeMap.insert(std::make_pair(chamberId,MeanClusterSizeMap));
    m_RPC_PanelMeanClusterSizeMap[chamberId]=MeanClusterSizeMap;

    if( m_debug )  m_log << MSG::DEBUG << "Size of  RPC_PanelMeanClusterSizeMap " << m_RPC_PanelMeanClusterSizeMap.size() << "; in panel " <<  channum << " SMeanClusterSizeMap " << SMeanClusterSizeMap << " MeanClusterSizeMap " << MeanClusterSizeMap << endmsg;     
    //std::cout<<"Here DBversion="<<DBversion<<std::endl;
    //std::cout<<"info_panel size  = "<<info_panel.size()<<std::endl;
    //for(unsigned int ii=0; ii<info_panel.size();ii++){       
    //  char* ch_tmp1= const_cast<char*>(info_panel[ii].c_str());       
    //  m_log << MSG::INFO << " info_panel " <<ii<<" "<< ch_tmp1 << endmsg;
    //}
    if( DBversion>2 ){
      char * SFracClusterSize1Map_a=const_cast<char*>(info_panel[19].c_str());
      char * SFracClusterSize1Map_b=const_cast<char*>(info_panel[20].c_str());
      double FracClusterSize1Map = atof(SFracClusterSize1Map_a)+atof(SFracClusterSize1Map_b)*10000;
      //m_RPC_PanelFracClusterSize1Map.insert(std::make_pair(chamberId,FracClusterSize1Map));
      m_RPC_PanelFracClusterSize1Map[chamberId]=FracClusterSize1Map;

      if( m_debug )  
	m_log << MSG::DEBUG << "Size of  RPC_PanelFracClusterSize1Map " << m_RPC_PanelFracClusterSize1Map.size() << "; in panel " <<  channum << " SFracClusterSize1Map_a " << SFracClusterSize1Map_a << " SFracClusterSize1Map_b " << SFracClusterSize1Map_b <<" FracClusterSize1Map " << FracClusterSize1Map << endmsg;     

      char * SFracClusterSize2Map_a=const_cast<char*>(info_panel[21].c_str());
      char * SFracClusterSize2Map_b=const_cast<char*>(info_panel[22].c_str());
      double FracClusterSize2Map = atof(SFracClusterSize2Map_a)+atof(SFracClusterSize2Map_b)*10000;
      //m_RPC_PanelFracClusterSize2Map.insert(std::make_pair(chamberId,FracClusterSize2Map));     
      m_RPC_PanelFracClusterSize2Map[chamberId]=FracClusterSize2Map;

      if( m_debug )  m_log << MSG::DEBUG << "Size of  RPC_PanelFracClusterSize2Map " << m_RPC_PanelFracClusterSize2Map.size() << "; in panel " <<  channum << " SFracClusterSize2Map_a " << SFracClusterSize2Map_a << " SFracClusterSize2Map_b " << SFracClusterSize2Map_b <<" FracClusterSize2Map " << FracClusterSize2Map << endmsg;     

      char * SFracClusterSize3Map_a=const_cast<char*>(info_panel[23].c_str());
      char * SFracClusterSize3Map_b=const_cast<char*>(info_panel[24].c_str());
      double FracClusterSize3Map = atof(SFracClusterSize3Map_a)+atof(SFracClusterSize3Map_b)*10000;
      //m_RPC_PanelFracClusterSize3Map.insert(std::make_pair(chamberId,FracClusterSize3Map));     
      m_RPC_PanelFracClusterSize3Map[chamberId]=FracClusterSize3Map;

      if( m_debug )  m_log << MSG::DEBUG << "Size of  RPC_PanelFracClusterSize3Map " << m_RPC_PanelFracClusterSize3Map.size() << "; in panel " <<  channum << " SFracClusterSize3Map_a " << SFracClusterSize3Map_a << " SFracClusterSize3Map_b " << SFracClusterSize3Map_b <<" FracClusterSize3Map " << FracClusterSize3Map << endmsg;     

    }    
    else{
      if (info_panel.size()>20){
	char * SFracClusterSize1Map=const_cast<char*>(info_panel[19].c_str());
	double FracClusterSize1Map = atof(SFracClusterSize1Map);
	//m_RPC_PanelFracClusterSize1Map.insert(std::make_pair(chamberId,FracClusterSize1Map));
	m_RPC_PanelFracClusterSize1Map[chamberId]=FracClusterSize1Map;

	if( m_debug )  m_log << MSG::DEBUG << "Size of  RPC_PanelFracClusterSize1Map " << m_RPC_PanelFracClusterSize1Map.size() << "; in panel " <<  channum << " SFracClusterSize1Map " << SFracClusterSize1Map << " FracClusterSize1Map " << FracClusterSize1Map << endmsg;     

	char * SFracClusterSize2Map=const_cast<char*>(info_panel[20].c_str());
	double FracClusterSize2Map = atof(SFracClusterSize2Map);
	//m_RPC_PanelFracClusterSize2Map.insert(std::make_pair(chamberId,FracClusterSize2Map));     
	m_RPC_PanelFracClusterSize2Map[chamberId]=FracClusterSize2Map;

	if( m_debug )  m_log << MSG::DEBUG << "Size of  RPC_PanelFracClusterSize2Map " << m_RPC_PanelFracClusterSize2Map.size() << "; in panel " << channum << " SFracClusterSize2Map " << SFracClusterSize2Map << " FracClusterSize2Map " << FracClusterSize2Map << endmsg;     
      }
      else
	{
	  //m_RPC_PanelFracClusterSize1Map.insert(std::make_pair(chamberId,0.6));     
	  //m_RPC_PanelFracClusterSize2Map.insert(std::make_pair(chamberId,0.2));     
	  m_RPC_PanelFracClusterSize1Map[chamberId]=0.6;
	  m_RPC_PanelFracClusterSize2Map[chamberId]=0.2;
	  m_log << MSG::DEBUG << "Here a panel with incomplete info in the DB, size = "<<info_panel.size()<<" instead of being>20 panel ID="<<channum<<" = "<<m_rpcIdHelper->show_to_string(chamberId)
		<<" cluster size 1 & 2 fraction set to .... 0.6 and 0.2"<<endmsg;
	}
    }
     
     
    // strip status 
    // n chars = #strips (status between 0--9)
     
    int countdeadstrip  = 0 ;     
    int countdeadstripinfidarea  = 0 ;     
    int countpanelstrip = 0 ;
     
    // update for the timing and error on timing 
    // new info strip |status time error_on_time|

    std::string delimiter_strip = "|";
    std::vector<std::string> info_strip;
    std::string strip_status_list="";
    std::vector<float> info_strip_test;

    MuonCalib::MdtStringUtils::tokenize(striplist,info_strip,delimiter_strip);

    
    char * ch_strip2;
    if( m_debug )  m_log << MSG::DEBUG << "strip list "  << striplist << " info list "<<info_strip [0]<< " "<<info_strip.size()   << " del "<< delimiter_strip<< endmsg; 
    
    if (info_strip.size()>1){
      if(m_FirstCall)  m_log << MSG::INFO << "TIMING INFO AVAILABLE" << endmsg;
     
           
      for(unsigned int i=0; i<info_strip.size();i++){

	ch_strip2= const_cast<char*>(info_strip[i].c_str());
	if( m_verbose )  m_log << MSG::VERBOSE << "strip # "<<i<<" Status+Timing INFO (time, error) " <<ch_strip2 << endmsg;
	 
	std::string delimiter_strip2 = "  ";
	std::vector<std::string> info_strip2;
	std::vector<float> info_strip_test2;
	char * ch_strip_new;
	// 	 //
	MuonCalib::MdtStringUtils::tokenize(ch_strip2,info_strip2,delimiter_strip2);
	 
	if( m_verbose ) {
	  for(unsigned int ii=0; ii<info_strip2.size();ii++)
	    {
	      ch_strip_new= const_cast<char*>(info_strip2[ii].c_str());
	      m_log << MSG::VERBOSE << " info_strip with time " <<  info_strip2[ii] << "  again  " <<ch_strip_new << endmsg;	
	    }
	}
	 
	char * STime=const_cast<char*>(info_strip2[1].c_str());
	double Time = atof(STime);
	char * SSigmaTime=const_cast<char*>(info_strip2[2].c_str());
	double SigmaTime = atof(SSigmaTime);
	char* strip_status= const_cast<char*>(info_strip2[0].c_str());

	if( m_verbose )  m_log << MSG::VERBOSE << "strip # "<<i<<" after dec: Status+Timing INFO (time, error) " << strip_status <<" "<<Time<<"+/-"<<SigmaTime<< endmsg; 
	strip_status_list = strip_status_list + strip_status ;
	
	std::vector<double> Time_vect;
	Time_vect.push_back(Time);
	Time_vect.push_back(SigmaTime);
	 
	Identifier strip_id ;
	CondAttrListCollection::ChanNum stripnum;
	stripnum = channum + i*4;
	strip_id = channum + i*4;
	 

	if( m_verbose )  m_log << MSG::VERBOSE << "strip #"<<i+1
			       << " strip_id " <<  stripnum <<" expanded "<<m_rpcIdHelper->show_to_string(strip_id)<< endmsg;

	//m_RPC_StripTimeMap.insert(std::make_pair(strip_id,Time_vect));   
	m_RPC_StripTimeMap[strip_id]=Time_vect;

	countpanelstrip++ ;
 
	if(strip_status[0]=='0'){
	  if (m_debug && (!m_verbose)) m_log << MSG::DEBUG << "strip #"<<i+1<<" info_strip " <<  strip_status[0]
					     << " strip_id " <<  stripnum <<" expanded "<<m_rpcIdHelper->show_to_string(strip_id)<< endmsg;
	  //m_RPC_PanelDeadStripList.insert(std::make_pair(strip_id,1));     
	  m_RPC_PanelDeadStripList[strip_id]=1;
	  countdeadstrip++;   
	  if( m_debug )  {
	    m_log << MSG::DEBUG << "Size of RPC_PanelDeadStripList (full det) " << m_RPC_PanelDeadStripList.size() << " in this panel "<<countdeadstrip
		  <<" for "<< m_rpcIdHelper->show_to_string(strip_id) << endmsg; 
	  }
	  if (i>1 && i<info_strip.size()-2)
	    {
	      // strip in the fiducial area for the efficiency measurement
	      countdeadstripinfidarea++;   
	      if( m_debug )  {
		m_log << MSG::DEBUG << "# of dead strips in fid. area for deadFrac measurement " <<countdeadstripinfidarea
		      <<" for "<<  stripnum << " " <<m_rpcIdHelper->show_to_string(strip_id) << endmsg; 
	      }
	    }
	  else 
	    {
	      if( m_debug )  {
		m_log << MSG::DEBUG << "dead strips n. "<<i+1<<" is at the border - not used for deadFrac of "<<  m_rpcIdHelper->show_to_string(strip_id) << endmsg; 
	      }
	    }
	}
	 
      }
       
    } else{
      if (m_FirstCall) m_log << MSG::INFO << "NO TIMING INFO" << endmsg;
      if( m_verbose )  {
	//m_log << MSG::VERBOSE << "NO TIMING INFO" << endmsg;
        m_log << MSG::VERBOSE << "Size of the strip list " << striplist.size() << endmsg;
      }
      for(unsigned int i=0; i<striplist.size();i++){
	 
	std::string part_strip = striplist.substr(i,1);
	 
	strip_status_list = strip_status_list + part_strip ;
	 
	ch_panel= const_cast<char*>(part_strip.c_str());
	 

	 
	Identifier strip_id ;
	CondAttrListCollection::ChanNum stripnum;
	stripnum = channum + i*4;
	strip_id = channum + i*4;
	 
	if( m_verbose )  m_log << MSG::VERBOSE << "strip #"<<i+1<<" info_strip " << part_strip 
			       << " strip_id " <<  stripnum <<" expanded "<<m_rpcIdHelper->show_to_string(strip_id)
			       << " panel = " << ch_panel<< endmsg;
	 
	countpanelstrip++ ;
 
	if(part_strip=="0"){
	  if (m_debug && (!m_verbose)) m_log << MSG::DEBUG << "strip #"<<i+1<<" info_strip " << part_strip 
					     << " strip_id " <<  stripnum <<" expanded "<<m_rpcIdHelper->show_to_string(strip_id)<< endmsg;
	  countdeadstrip++;   
	  //m_RPC_PanelDeadStripList.insert(std::make_pair(strip_id,1));     
	  m_RPC_PanelDeadStripList[strip_id]=1;
	  if( m_debug )  {
	    m_log << MSG::DEBUG << "Size of RPC_PanelDeadStripList (full det) " << m_RPC_PanelDeadStripList.size() << " in this panel "<<countdeadstrip
		  <<" for "<< m_rpcIdHelper->show_to_string(strip_id) << endmsg; 
	  }
	  if (i>1 && i<striplist.size()-2){
	    // strip in the fiducial area for the efficiency measurement
	    countdeadstripinfidarea++;   
	    if( m_debug )  {
	      m_log << MSG::DEBUG << "# of dead strips in fid. area for deadFrac measurement " <<countdeadstripinfidarea
		    <<" for "<<  stripnum << " " <<m_rpcIdHelper->show_to_string(strip_id) << endmsg; 
	    }
	  }
	  else 
	    {
	      if( m_debug )  {
		m_log << MSG::DEBUG << "dead strips n. "<<i+1<<" is at the border - not used for deadFrac of "<<  m_rpcIdHelper->show_to_string(strip_id) << endmsg; 
	      }
	    }
	}
	 
      }
    }
     
     
    if(countpanelstrip!= npanelstrip) m_log << MSG::WARNING << "WARNING (no side effects for this, just a reminder for a proper fix of the DB content) no matching strip number!!! " << countpanelstrip << " != " <<npanelstrip << " Identifier: " << channum << endmsg; 
     
    float FracDeadStripMap = 0 ;
     
    if(countpanelstrip-4>0) FracDeadStripMap = float(countdeadstripinfidarea)/float(countpanelstrip-4);

    // store in the suitabel maps 
    //m_RPC_PanelDeadStripListMap.insert(std::make_pair(chamberId,strip_status_list));               
    //m_RPC_PanelFracDeadStripMap.insert(std::make_pair(chamberId,FracDeadStripMap));     
    m_RPC_PanelDeadStripListMap[chamberId]=strip_status_list;
    m_RPC_PanelFracDeadStripMap[chamberId]=FracDeadStripMap;
     
    if( m_debug )  {
      m_log << MSG::DEBUG << "Size of RPC_PanelFracDeadStripMap " << m_RPC_PanelFracDeadStripMap.size() 
	    << "; in panel " <<  channum 
	    << " FracDeadStri(in fid.area) " << FracDeadStripMap ;
      if( countpanelstrip == 0 ) m_log << " (incl. borders) DIVISION BY ZERO IMPOSSIBLE";
      else m_log << " (incl. borders) "<<float(countdeadstrip)/float(countpanelstrip);
      m_log << " nDeadStrips,InFidArea/nStrips "<<countdeadstrip<<","<<countdeadstripinfidarea<<"/"<<countpanelstrip
	    <<endmsg;    
    }
    if (Efficiency-(0.99-FracDeadStripMap)>0.) 
      {
	if (Efficiency<0.995 || FracDeadStripMap>0.01 )
	  {
	    if (Efficiency>0.005 || FracDeadStripMap<0.99) 
	      {
		m_log << MSG::DEBUG << "WARNING: Inconsistent panel eff.="<<Efficiency
		      <<" and 0.99-dead_frac="<<0.99-FracDeadStripMap
		      <<" nDeadStrips,InFidArea/nStrips "<<countdeadstrip<<","
		      <<countdeadstripinfidarea<<"/"<<countpanelstrip
		      <<" for panelId="<<m_rpcIdHelper->show_to_string(chamberId);
		if      (Efficiency-(0.99-FracDeadStripMap)>0.2)	
		  m_log << " difference >0.2"<<endmsg;
		else if (Efficiency-(0.99-FracDeadStripMap)>0.1)	
		  m_log << " difference >0.1"<<endmsg;
		else if (Efficiency-(0.99-FracDeadStripMap)>0.05)	
		  m_log << " difference >0.05"<<endmsg;
		else if (Efficiency-(0.99-FracDeadStripMap)>0.025)	
		  m_log << " difference >0.025"<<endmsg;
		else if (Efficiency-(0.99-FracDeadStripMap)>0.01)	
		  m_log << " difference >0.01"<<endmsg;
		else 
		  m_log << " difference >0 but <=0.01"<<endmsg;
	      }
	    
	  }
	
      }
    
    if( Efficiency <= m_RPC_PanelEfficiency ){
       
      m_effPanelId.push_back(chamberId);
       
    }

    m_FirstCall = false ;

  }
  if( m_debug ) m_log << MSG::DEBUG <<"----------------------------- end of BLOB reached; reading and decoding DetStatus DB DONE"<<endmsg;

  return  sc; 
}



