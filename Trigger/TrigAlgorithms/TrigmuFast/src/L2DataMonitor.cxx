/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/L2DataMonitor.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigmuFast/MuData.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/MdtCsmContainer.h"


#include "TrigT1RPClogic/RPCrawData.h"
#include "TrigT1RPClogic/CMAdata.h"

#include "TrigT1RPClogic/ShowData.h"

#include "PathResolver/PathResolver.h"

#include "TrigT1Result/RoIBResult.h"

#include "RPCcablingInterface/RpcPadIdHash.h" 

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "TrigMuonEvent/MuonFeature.h"

#include "MuonContainerManager/MuonRdoContainerAccess.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

//using namespace HLT; 

L2DataMonitor::L2DataMonitor(const std::string& name, ISvcLocator* pSvcLocator):
    HLT::FexAlgo(name, pSvcLocator),
    m_rpc_converter("RPC_RawDataProviderTool"),
    m_mdt_converter("MDT_RawDataProviderTool"),
    m_tgc_converter("TGC_RawDataProviderTool"),
    m_csc_converter("CSC_RawDataProviderTool"),
    m_rpc_geometry("RPCgeometrySvc",""),
    m_tgc_geometry("TGCgeometrySvc",""),
    m_csc_geometry("CSCgeometrySvc",""),
    m_mdt_cabling ("MDTcablingSvc","")
{    
    declareProperty("TestbeamRun",m_TestBeamRun=false);
    
    declareProperty("ROBmapperFile",m_ROBmapper_filename="RoiROBmap.data");
    
    declareProperty("RobId", m_RobIds);
    declareProperty("MdtId", m_MdtIds);
    declareProperty("SubsystemId",m_subsysID=1); // corresponding to |eta| > 0
    declareProperty("RoiNumber",m_RoINumber=10); // corresponding to PAD = 2
    declareProperty("SectorId",m_sectorID=31);   // cabled sector at H8
    declareProperty("histoPathBase",m_histo_path_base="/stat/Monitoring");

    declareProperty("UseCalibration",m_use_calib=false);

    declareProperty("EvtToShow",m_evtToShow=100);
            
    declareProperty("MUlvl1INFO",m_MUlvl1_info=false);
    declareProperty("MUtrackINFO",m_MUtrack_info=false);
    declareProperty("MUroadsINFO",m_MUroads_info=false);
    declareProperty("MUdecoINFO",m_MUdeco_info=false);
    declareProperty("MUcontINFO",m_MUcont_info=false);
    declareProperty("MUfitINFO",m_MUfit_info=false);
    declareProperty("MUsagINFO",m_MUsag_info=false);
    declareProperty("MUptINFO",m_MUpt_info=false);
    declareProperty("TestString",m_test_string=""); 
    
    declareProperty("RPC_RawDataProvider", m_rpc_converter);
    declareProperty("MDT_RawDataProvider", m_mdt_converter);
    declareProperty("TGC_RawDataProvider", m_tgc_converter);
    declareProperty("CSC_RawDataProvider", m_csc_converter);

    declareProperty( "RPC_Geometry", m_rpc_geometry);
    declareProperty( "TGC_Geometry", m_tgc_geometry);
    declareProperty( "CSC_Geometry", m_csc_geometry);
    declareProperty( "MDT_Cabling" , m_mdt_cabling);

    // CID 23232: UNINIT_CTOR
    m_eventsProcessed = 0;
    for(int i=0; i<12; i++) m_residuals[i] = 0;
    for(int i=0; i<3; i++)  m_mdt_beam_profile[i] = 0;
    for(int i=0; i<3; i++)  m_rpc_beam_profile[i] = 0;
}


HLT::ErrorCode L2DataMonitor::hltInitialize()
{
  if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Initialize L2DataMonitor" << endreq;
  
  StatusCode sc;
  
  if(!m_TestBeamRun)
  {
  // ==================== GET THE REGION SELECTOR TOOL =======================
      //sc = toolSvc()->retrieveTool("RegionSelector", m_pRegionSelector,this);
      sc = service("RegSelSvc", m_pRegionSelector);
      if(sc.isFailure()){
        msg() << MSG::FATAL
              << "Unable to retrieve RegionSelector tool"
              << endreq;
        return HLT::BAD_JOB_SETUP;
  }
  }
  
  // ======================== GET ROBDATAPROVIDER ============================

  IService* svc ;
  if(StatusCode::SUCCESS != service("ROBDataProviderSvc",svc)){
    msg() << MSG::ERROR << " Can't get ROBDataProviderSvc " << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  m_pROBDataProvider = dynamic_cast<ROBDataProviderSvc*> (svc);
  if(m_pROBDataProvider == 0 ) {
    msg() << MSG::ERROR<< " Cant cast to TrigL2ROBDataProviderSvc " <<endreq;
    return HLT::BAD_JOB_SETUP;
  }

  
  if(!m_TestBeamRun)
  {
  // ====================== INITIALIZE THE ROBMAPPER =========================

  // implement the search of RODFile trought the pathresolver utility.
  std::string ROBmapper_filename;
  ROBmapper_filename = PathResolver::find_file (m_ROBmapper_filename, "DATAPATH");
    
  m_pROBmapper = new MUON::ROBmapper(ROBmapper_filename);  
  msg() << MSG::INFO << "ROB mapper loaded!" << endreq;

  if(msgLvl() <= MSG::DEBUG) {
      for(int i=0;i<2;++i)
          for(int j=0;j<32;++j)
              for(int k=1;k<28;++k)
	      {
	          std::vector<uint32_t> robs;
                  //robs.reserve(2);
	          m_pROBmapper->lookup(i,j,k,robs);
	          if(robs.size())
	          {
	              if( robs.size() == 2)
		      {
	                  msg() << MSG::DEBUG << "ROBMAPPER. Sub, Sec, RoI =" 
		                        << i << "," << j << "," << k 
				        << "|-> " << std::setiosflags(std::ios::showbase)
					<< MSG::hex << robs[0] << " " 
					<< robs[1] << MSG::dec << endreq;
                      } else
		      {
	                  msg() << MSG::DEBUG << "ROBMAPPER. Sub, Sec, RoI =" 
		                        << i << "," << j << "," << k 
				        << "|-> " << std::setiosflags(std::ios::showbase)
					<< MSG::hex << robs[0] << MSG::dec
					<< endreq;		  
		      }
	          }
	      }  
       }

   }
   else
   {  
   
      // **********************************************************************
      // ********** SETTING UP ROBLIST AND MDT LIST FOR TESTBEAM DATA *********
      // **********************************************************************

       std::vector<int>::const_iterator it = m_RobIds.begin();
       while (it != m_RobIds.end())
       {
           m_robIds.push_back(*it);
           ++it;
       }
   
  // Collect the full ROB list //////////////////////////////////////////////
  if(msgLvl() <= MSG::DEBUG) {                                             //
      msg() << MSG::DEBUG                                                  //
          << " Number of ROBs to access = "                                //
          << m_robIds.size() << endreq;                                    //
      msg() << MSG::DEBUG                                                  //
          << " List of ROBs: "  << endreq;                                 //
      for (std::vector<uint32_t>::const_iterator i=m_robIds.begin();       //
           i != m_robIds.end(); ++i)                                       //
      {                                                                    //
          msg() << MSG::DEBUG << *i << endreq;                             //
      }                                                                    //
  }                                                                        //
  ///////////////////////////////////////////////////////////////////////////
  
  
      std::vector<int>::const_iterator hs = m_MdtIds.begin();
      while (hs != m_MdtIds.end())
      {
          m_listOfMDTIds.push_back(*hs);
          ++hs;
      }

   }
   
  sc = m_tgc_geometry.retrieve();
  if ( sc.isFailure() )
  {
    msg() << MSG::ERROR << "Could not retrieve " 
                        << m_tgc_geometry << endreq;
    return HLT::BAD_JOB_SETUP;;
  } else
  {
    msg() << MSG::INFO << "Retrieved service " << m_tgc_geometry << endreq;
  }
  
  sc = m_mdt_cabling.retrieve();
  if ( sc.isFailure() )
  {
    msg() << MSG::ERROR << "Could not retrieve " 
                        << m_mdt_cabling << endreq;
    return HLT::BAD_JOB_SETUP;;
  } else
  {
    msg() << MSG::INFO << "Retrieved service " << m_mdt_cabling << endreq;
  }
  
  // ============= FILL THE INTERFACES TOWARDS THE MUON SERVICES =============
  unsigned short int info = ((1 & m_MUlvl1_info)  << 0) |
                            ((1 & m_MUtrack_info) << 1) |
			    ((1 & m_MUroads_info) << 2) |
			    ((1 & m_MUdeco_info)  << 3) |
			    ((1 & m_MUcont_info)  << 4) |
			    ((1 & m_MUfit_info)   << 5) |
			    ((1 & m_MUsag_info)   << 6) |
			    ((1 & m_MUpt_info)    << 7);
  
  m_msg = new MuMessageInterface(msg(),0,info,m_evtToShow,m_test_string);
  m_svc = new MuServiceInterface(serviceLocator(),&m_rpc_geometry,&m_tgc_geometry,
				 &m_csc_geometry,&m_mdt_cabling,0,name(),m_use_calib,false,true,false,false,1.);
  if(!m_svc->isLoaded())
  {
      msg() << MSG::FATAL
	    << "unable to set the Muon Service Interface"
	    << endreq;
      return HLT::BAD_JOB_SETUP;
  }
  
  
  // Here one is supposed to open the file if needed and do the setup...
  
  // ************************************************************************
  // ******************* SETTING HISTOGRAMS FOR MONITORING ******************
  // ************************************************************************


  for (int l=0;l<6;++l)
  { 
      CSMid csmId = m_svc->mdtgeo()->getCsm(0x61,2,l+1);
      if(csmId)
      {
          m_residuals[l] = histoSvc()->book(m_histo_path_base, l+1,
                           "Z tube residuals of "+csmId.mnemonic()+" (cm)",
			   100,-50.,50.);
      }
  }
  
  unsigned int SubId = (m_svc->mdtgeo()->getCsm(0x61,3,1))? 0x61 : 0x63;
  
  for (int l=0;l<6;++l)
  { 
      CSMid csmId = m_svc->mdtgeo()->getCsm(SubId,3,l+1);
      if(csmId)
      {
          m_residuals[l+6] = histoSvc()->book(m_histo_path_base, l+1+6,
                             "Z tube residuals of "+csmId.mnemonic()+" (cm)",
		  	     100,-50.,50.);
      }
  }
 
  m_mdt_beam_profile[0] = histoSvc()->book(m_histo_path_base, 31,
                                           "MDT BIL beam profile",100, -150.,150.);
  m_mdt_beam_profile[1] = histoSvc()->book(m_histo_path_base, 32,
                                           "MDT BML beam profile",100, -150.,150.);
  m_mdt_beam_profile[2] = histoSvc()->book(m_histo_path_base, 33,
                                           "MDT BOL beam profile",100, -150.,150.);					   
  
  m_rpc_beam_profile[0] = histoSvc()->book(m_histo_path_base, 34,
                                           "RPC Station1 beam profile",100, -150.,150);
  m_rpc_beam_profile[1] = histoSvc()->book(m_histo_path_base, 35,
                                           "RPC Station2 beam profile",100, -150.,150);
  m_rpc_beam_profile[2] = histoSvc()->book(m_histo_path_base, 36,
                                           "RPC Station3 beam profile",100, -150.,150);					   
  
  
  
  m_BML_RPC_MDT = histoSvc()->book(m_histo_path_base, 15,
                  "MDT-RPC residuals of BML (cm)", 100,-50.,50.);
  m_BOL_RPC_MDT = histoSvc()->book(m_histo_path_base, 16,
                  "MDT-RPC residuals of BOL (cm)", 100,-50.,50.);
  
  //m_display = histoSvc()->book(m_histo_path_base,20, "Online Event Display",
  //                             240, -220.,260.,280,3000.,5600.);
			       
  m_MdtDisp = histoSvc()->book(m_histo_path_base,21, 
                 "Online Barrel Event Display",240, -220.,260.,200,3360.,3980.);
  
  m_RpcDisp = histoSvc()->book(m_histo_path_base,22, 
                 "Online Barrel Event Display",240, -220.,260.,200,3360.,3980.);
  
  msg() << MSG::INFO 
        << "Initialization completed successfully" 
        << endreq;


  m_eventsProcessed = 0;

  return HLT::OK;
}

HLT::ErrorCode L2DataMonitor::hltFinalize()
{
    return HLT::OK;
}


////////////////////////////////////////////////////////////////////////////

/** @fn bool muFast::execHLTAlgorithm(TriggerElement* outputTE)
 * @brief Execute(TE) method.
 * @return bool
 * This is an example to illustrate several aspects of the HLTSSW framework:

 * - Seeding mechanism: how to navigate the following structure at input:
 *
 *         +-------------+   uses     +------------+
 *         |  InputTe    | ---------> |  EmTauRoI  |
 *         +-------------+            +------------+
 *               ^
 *               | seeded by
 *         +-------------+
 *         |  OutputTE   |
 *         +-------------+
 *
 * - Region lookup
 * - RawDataObject access (from BS)
 * - Linking a new object into the TEOutput for subsequent use
 * - OutputTE activation to signal a positive decision
 * All is done using the LAr as an example detector.
 */

HLT::ErrorCode L2DataMonitor::hltExecute(const HLT::TriggerElement* inputTE,
                                               HLT::TriggerElement* outputTE)
{

  m_eventsProcessed++;

  m_pStoreGate = store();                     // retrieve the store

  StatusCode sc = StatusCode::FAILURE;


  // Get the Lvl1 Id to intrumet the NetLoger calls /////////////////////////
  //int currentLvl1ID = 0;                                                   //
  //  const DataHandle<ROIB::RoIBResult> dobj;                                 //
  //  sc = m_pStoreGate->retrieve(dobj) ;                                      //
  /*
  if ( sc.isFailure() )                                                    //
  {                                                                        //
      Mlog << MSG::ERROR << "Could not find Lvl1Result" << endreq;         //
      return sc;                                                           //
  } else                                                                   //
  {                                                                        //
      currentLvl1ID = dobj->cTPResult().header().L1ID();                   //
#ifndef NDEBUG                                                             //
      Mlog << MSG::DEBUG << " ---> Lvl1Result value "                      //
           << currentLvl1ID << endreq;                                     //
#endif                                                                     //
  } 
  */                                                                       //
  ///////////////////////////////////////////////////////////////////////////


 if(msgLvl() <= MSG::DEBUG)
     msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId()
           << endreq;



  // ************************************************************************
  // ***************** START TRIGGER ELEMENT PROCESSING *********************
  // ************************************************************************

  
  // Get vector of of pointers to all RecMuonRoIs with label "uses" /////////
  std::vector<const LVL1::RecMuonRoI*> vectorOfRecMuonRoI;                 //
  HLT::ErrorCode status = getFeatures(inputTE,vectorOfRecMuonRoI,"");      //
  if(status!=HLT::OK) return HLT::NAV_ERROR;                               //
                                                                           //
  /** Check that there is only one RecMuonRoI.                             //
   *  We only expect ONE input RoI.                                        //
   */                                                                      //
  if (vectorOfRecMuonRoI.size() != 1){                                     //
    msg() << MSG::ERROR                                                    //
          << "Size of vector is " << vectorOfRecMuonRoI.size()             //
          << endreq;                                                       //
    return HLT::NAV_ERROR;                                                 //
  }                                                                        //
  ///////////////////////////////////////////////////////////////////////////

  // Get first (and only) RoI ///////////////////////////////////////////////
  const LVL1::RecMuonRoI* pMuonRoI = vectorOfRecMuonRoI.front();           //
  if(!pMuonRoI){                                                           //
    msg() << MSG::ERROR                                                    //
        << "Retrieval of RoI from vector failed"                           //
        << endreq;                                                         //
    return HLT::NAV_ERROR;                                                 //
  }                                                                        //
                                                                           //
 if(msgLvl() <= MSG::DEBUG) {                                              //
     msg() << MSG::DEBUG                                                   //
           << "The position of the RoI is phi = "                          //
           << pMuonRoI->phi()                                              //
           << " / eta = "                                                  //
           << pMuonRoI->eta()                                              //
           << endreq;                                                      //
     msg() << MSG::DEBUG                                                   //
           << "RoI code = " << pMuonRoI->getRoINumber()                    //
           << " ,  RoI Threshold = " << pMuonRoI->getThresholdNumber()     //
           << endreq;                                                      //
  }                                                                        //
  ///////////////////////////////////////////////////////////////////////////


  // Exit from processing if it is a TGC RoI ////////////////////////////////
  if (pMuonRoI->sysID())                                                   //
  {                                                                        //
      if(msgLvl() <= MSG::DEBUG) {                                         //
          msg() << MSG::DEBUG                                              //
                << "TGC RoI processing not yet working!" << endreq;        //
          msg() << MSG::DEBUG                                              //
                << "Creating a fake L2 MuonFeature." << endreq;            //
      }                                                                    //
                                                                           //
      return HLT::OK;                                                      //
  }                                                                        //
  ///////////////////////////////////////////////////////////////////////////  
  
  
  std::vector<IdentifierHash>* listOfMDTIds = new (std::vector<IdentifierHash>);

  unsigned int rpc_subsys_id = 0;  
  unsigned int rpc_rob_id = 0; 
  
  if(!m_TestBeamRun)   // i.e. atlas setup run
  {
  // ************************************************************************
  // ******************** START ROI DATA PROCESSING *************************
  // ************************************************************************

  // Get RoI actual dimension ///////////////////////////////////////////////
  float EtaMin = 0.;                                                       //
  float EtaMax = 0.;                                                       //
  float PhiMin = 0.;                                                       //
  float PhiMax = 0.;                                                       //
                                                                           //
  unsigned int EtaLowBorder = 0;                                           //
  unsigned int EtaHighBorder = 0;                                          //
  unsigned int PhiLowBorder = 0;                                           //
  unsigned int PhiHighBorder = 0;                                          //
                                                                           //
  bool GCs;                                                                //
  if(pMuonRoI->getThresholdNumber() > 3)                                   //
  {                                                                        //
      if(!(GCs = m_svc->rpccab()->give_HighPt_borders(pMuonRoI->subsysID(),//
                                             pMuonRoI->sectorID(),         //
                                             pMuonRoI->getRoINumber(),     //
					     EtaLowBorder,EtaHighBorder,   //
					     PhiLowBorder,PhiHighBorder)) )//
      {                                                                    //
          msg() << MSG::ERROR                                              //
          << "Cannot retrieve the RoI border code of the RPC strips "      //
          << endreq;                                                       //
      }                                                                    //
  }                                                                        //
  else                                                                     //
  {                                                                        //
       if(!(GCs = m_svc->rpccab()->give_LowPt_borders(pMuonRoI->subsysID(),//
                                             pMuonRoI->sectorID(),         //
                                             pMuonRoI->getRoINumber(),     //
					     EtaLowBorder,EtaHighBorder,   //
					     PhiLowBorder,PhiHighBorder)) )//
      {                                                                    //
          msg() << MSG::ERROR                                              //
          << "Cannot retrieve the RoI border code of the RPC strips "      //
          << endreq;                                                       //
      }                                                                    //
  }                                                                        // 
                                                                           //
  if(GCs)                                                                  //
  {                                                                        //
      if(msgLvl() <= MSG::DEBUG) {                                         //
        msg() << MSG::DEBUG                                                //
              << "RoI EtaLowCode,EtaHighCode = (" << EtaLowBorder          //
	      << "," << EtaHighBorder << ")  " << endreq;                  //
        msg() << "RoI PhiLowCode,PhiHighCode = (" << PhiLowBorder << ","   //
	      << PhiHighBorder << ")"                                      //
              << endreq;                                                   //
      }                                                                    //
      float firstEta[3] = {0.,0.,0.};                                      //
      float lastEta[3]  = {0.,0.,0.};                                      //
                                                                           //
      float RadMin = 0.;                                                   //
      float RadMax = 0.;                                                   //
                                                                           //
      bool GC = true;                                                      //
      if(!m_svc->rpcgeo()->give_strip_coordinates(EtaLowBorder,firstEta))  //
      {                                                                    //
          msg() << MSG::ERROR                                              //
              << "Cannot retrieve the coordinates "                        //
	      << "for EtaLowBorder=" << EtaLowBorder << endreq;            //
          msg() << MSG::ERROR << firstEta[0] << " " << firstEta[1]         //
	      << " " << firstEta[2] << endreq;                             //
	  GC = false;                                                      //
      }                                                                    //
                                                                           //
      if(!m_svc->rpcgeo()->give_strip_coordinates(EtaHighBorder,lastEta))  //
      {                                                                    //
          msg() << MSG::ERROR                                              //
                << "Cannot retrieve the coordinates "                      //
	        << "for EtaHighBorder=" << EtaHighBorder << endreq;        //
	  GC = false;                                                      //
      }                                                                    //
                                                                           //
                                                                           //
      if(!m_svc->rpcgeo()->give_strip_radius(EtaLowBorder,RadMin))         //
      {                                                                    //
	   msg() << MSG::ERROR                                             //
                 << "Cannot retrieve the gas Layer Radius "                //
	         << "for EtaLowBorder=" << EtaLowBorder << endreq;         //
	   GC = false;                                                     //
      }                                                                    //
                                                                           //
      if(!m_svc->rpcgeo()->give_strip_radius(EtaHighBorder,RadMax))        //
      {                                                                    //
	   msg() << MSG::ERROR                                             //
                 << "Cannot retrieve the gas Layer Radius "                //
	         << "for EtaHighBorder=" << EtaHighBorder << endreq;       //
	   GC = false;                                                     //
      }                                                                    //
                                                                           //
      if(GC)                                                               //
      {                                                                    //
          float Zmin = firstEta[2];                                        //
          float ThMin  = (Zmin)? atan(RadMin/fabsf(Zmin)): asin(1.);       //
          EtaMin = (Zmin>0.)?  -log(tan(ThMin/2.)) : log(tan(ThMin/2.));   //
	                                                                   //
          float Zmax = lastEta[2];                                         //
          float ThMax  = (Zmax)? atan(RadMax/fabsf(Zmax)): asin(1.);       //
          EtaMax = (Zmax>0.)?  -log(tan(ThMax/2.)) : log(tan(ThMax/2.));   //  
	                                                                   //
	  PhiMin = pMuonRoI->phi() - 0.02;                                 //
	  PhiMax = pMuonRoI->phi() + 0.02;                                 //	  
	  if(PhiMin < 0.) PhiMin += 2*3.141593;                            //
          if(PhiMax < 0.) PhiMax += 2*3.141593;                            //
      }                                                                    //
      else                                                                 //   
      {                                                                    //
          EtaMin = pMuonRoI->eta() - 0.1;                                  //
	  EtaMax = pMuonRoI->eta() + 0.1;                                  //
	  PhiMin = pMuonRoI->phi() - 0.02;                                 //
	  PhiMax = pMuonRoI->phi() + 0.02;                                 //
      }                                                                    //
  }                                                                        //
  else                                                                     //
  {                                                                        //
          EtaMin = pMuonRoI->eta() - 0.1;                                  //
	  EtaMax = pMuonRoI->eta() + 0.1;                                  //
	  PhiMin = pMuonRoI->phi() - 0.02;                                 //
	  PhiMax = pMuonRoI->phi() + 0.02;                                 //  
  }                                                                        //
                                                                           //
  if (EtaMin > EtaMax)                                                     //
  {                                                                        //
      float tmp = EtaMin;                                                  //
      EtaMin = EtaMax;                                                     //
      EtaMax = tmp;                                                        //
  }                                                                        //
                                                                           //
  if(msgLvl() <= MSG::DEBUG) {                                             //
       msg() << MSG::DEBUG                                                 //
          << "RoI EtaMin,EtaMax = (" << EtaMin << "," << EtaMax << ")  "   //
          << "RoI PhiMin,PhiMax = (" << PhiMin << "," << PhiMax << ")"     //
          << endreq;                                                       //
  }                                                                        //
  ///////////////////////////////////////////////////////////////////////////

  // create the roi descriptor for the Region Selector  
  TrigRoiDescriptor _roi( pMuonRoI->eta(), EtaMin, EtaMax, pMuonRoI->phi(), PhiMin, PhiMax );

  // Ask the RegionSelector for the Identifiers of the Collections //////////
  m_pRegionSelector->DetHashIDList(MDT, _roi, *listOfMDTIds);               //
                                                                           //
                                                                           //
  if(msgLvl() <= MSG::DEBUG) {                                             //
      for (unsigned int i=0;i<(*listOfMDTIds).size();i++)                  //
          msg() << MSG::DEBUG                                              //
                << "MDT chamber n.  " << i                                 //
                << ": hash id " << (*listOfMDTIds)[i]                      //
                << endreq;                                                 //
  }                                                                        //
  ///////////////////////////////////////////////////////////////////////////


  // compute the ROB ID for the RPC data and preload the ROBDataProvider ////
  rpc_subsys_id = (pMuonRoI->subsysID()==1)? 0x65 : 0x66;                  //  
  rpc_rob_id    = (        1         << 16) |                              //
                  ( rpc_subsys_id    << 8)  |                              //
                  (pMuonRoI->sectorID()/2) ;                               //
  ///////////////////////////////////////////////////////////////////////////


  } 
  else  // collect the MDT identifiers from the 
  {
      std::vector<int>::const_iterator hs = m_MdtIds.begin();
      while (hs != m_MdtIds.end())
      {
          (*listOfMDTIds).push_back(*hs);
          ++hs;
      }   
  }
  
  
  
  
  // Retrieve the PAD identifiers using the cabling service /////////////////
  Identifier rpc_pad_id;                                                   //
  unsigned int logic_sector = 0;                                           //
  unsigned short int PADId  = 0;                                           //
                                                                           //
  bool success = (!m_TestBeamRun)?                                         //
          m_svc->rpccab()->give_PAD_address(pMuonRoI->subsysID(),          //
                                            pMuonRoI->sectorID(),          //
		                            pMuonRoI->getRoINumber(),      //
					    logic_sector,PADId,rpc_pad_id)://
          m_svc->rpccab()->give_PAD_address(m_subsysID,                    //
                                            m_sectorID,                    //
		                            m_RoINumber,                   //
					    logic_sector,PADId,rpc_pad_id);//
                                                                           //
  if (!success)	                                                           //				    
  {                                                                        //
      msg() << MSG::ERROR                                                  //
        << "Cannot retrieve the PAD identifier from the Cabling Service"   //
        << endreq;                                                         //
      return HLT::TOOL_FAILURE;                                            //
  }                                                                        //
                                                                           //
  ///////////////////////////////////////////////////////////////////////////


  std::vector<uint32_t> mdt_rob_ids(m_robIds);

  if(!m_TestBeamRun)
  {
  // Preload the data into the ROBdataProvider //////////////////////////////
  mdt_rob_ids.clear();                                                     //
  m_pROBmapper->lookup(pMuonRoI->subsysID(),                               //
                       pMuonRoI->sectorID(),                               //
		       pMuonRoI->getRoINumber(),                           //
		       mdt_rob_ids);                                       //
  std::vector<uint32_t> robIds(mdt_rob_ids);                               //
  if(msgLvl() <= MSG::DEBUG) {                                             //
      msg() << MSG::DEBUG                                                  //
            << " Muon ROB Mapper: Number of ROBIds = "                     //
            << robIds.size() << endreq;                                    //
      msg() << MSG::DEBUG                                                  //
            << " Muon ROB Mapper: List of ROBIds   = "  << endreq;         //
  }                                                                        //
  for (std::vector<uint32_t>::const_iterator i=robIds.begin();             //
       i != robIds.end(); ++i)                                             //
  {                                                                        //
      msg() << MSG::DEBUG << *i << endreq;                                 //
  }                                                                        //
                                                                           //
  
  robIds.push_back(rpc_rob_id);                                            //
  m_pROBDataProvider->addROBData(robIds);                                  //
  ///////////////////////////////////////////////////////////////////////////
  }
  else
  { 
  // Preload the data into the ROBdataProvider //////////////////////////////
      m_pROBDataProvider->addROBData(m_robIds);                            //
  ///////////////////////////////////////////////////////////////////////////
  }
  
  
  // crate the data holder //////////////////////////////////////////////////
  MuData data;                                                             //
  ///////////////////////////////////////////////////////////////////////////


  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecRobs;

  // Get the Data for the RPC PADs //////////////////////////////////////////
  RpcPadContainer* PAD =                                                   //
           Muon::MuonRdoContainerAccess::retrieveRpcPad("RPCPAD");         //
	                                                                   //
  vecRobs.clear();                                                         //
                                                                           //
  if(PAD==0)                                                               //
  {                                                                        //
     if ( msgLvl() <= MSG::DEBUG )                                         //
          msg() << MSG::DEBUG << "Rpc PAD container not registered by "    //
	                      << "MuonRdoContainerManager; retrieving it " //
			      << "from the store! " << endreq;             //
                                                                           //
      sc = m_pStoreGate->retrieve(PAD, "RPCPAD");                          //
      if (sc.isFailure()){                                                 //
          msg() << MSG::ERROR << "Retrieval of RpcPadContainer failed"     //
                              << endreq;                                   //
          return HLT::SG_ERROR;                                            //
      }                                                                    // 
  }                                                                        //
                                                                           //
  m_pROBDataProvider->                                                     //
      getROBData(std::vector<uint32_t>(1,rpc_rob_id),vecRobs);             // 
	                                                                   //
  // IdentifierHash pad_id = PAD->idToHash(rpc_pad_id);                       //
  RpcPadIdHash padHashFunction = *(m_svc->iRpcCablingSvc()->padHashFunction());
  IdentifierHash pad_id = padHashFunction(rpc_pad_id);
                                                                           //
  m_rpc_converter->                                                        //
      convert(vecRobs,std::vector<IdentifierHash>(1,pad_id));              //
                                                                           //
  RpcPadContainer::const_iterator itPAD = PAD->indexFind(pad_id);          //
                                                                           //
  const RpcPad* rpc_pad = *itPAD;                                          //
  ///////////////////////////////////////////////////////////////////////////

  
  data.get_pad(rpc_pad);


  // Get the Container for the CSMs /////////////////////////////////////////
  MdtCsmContainer* CSM =                                                   //
           Muon::MuonRdoContainerAccess::retrieveMdtCsm("MDTCSM");         //
	                                                                   //
  vecRobs.clear();                                                         //
                                                                           //
  if(CSM==0)                                                               //
  {                                                                        //
     if ( msgLvl() <= MSG::DEBUG )                                         //
          msg() << MSG::DEBUG << "MDT CSM container not registered by "    //
	                      << "MuonRdoContainerManager; retrieving it " //
			      << "from the store! " << endreq;             //
                                                                           //
      sc = m_pStoreGate->retrieve(CSM, "MDTCSM");                          //
      if (sc.isFailure()){                                                 //
          msg() << MSG::ERROR << "Retrieval of MdtCsmContainer failed"     //
                              << endreq;                                   //
          return HLT::SG_ERROR;                                            //
      }                                                                    // 
  }                                                                        //
                                                                           //
  m_pROBDataProvider->getROBData(mdt_rob_ids,vecRobs);                     // 
	                                                                   //
  m_mdt_converter->convert(vecRobs,*listOfMDTIds);                         //
                                                                           //
  for(unsigned int i = 0; i < (*listOfMDTIds).size(); i++)                 //
  {                                                                        //
      MdtCsmContainer::const_iterator pCsmIt =                             //
          CSM->indexFind((*listOfMDTIds)[i]);                              //
                                                                           //
      if(pCsmIt!=CSM->end())                                               //
      {                                                                    //
      if(msgLvl() <= MSG::DEBUG)                                           //
          msg() << MSG::DEBUG                                              //
	       << "Number of digit in Collection: "                        //
               << (*pCsmIt)->size()                                        //
	       << endreq;                                                  //
                                                                           //
          data.get_csm(*pCsmIt);                                           //
      }                                                                    //
  }                                                                        //
  delete listOfMDTIds;                                                     //
  ///////////////////////////////////////////////////////////////////////////




  // ************************************************************************
  // *********************** RPC DATA MONITOR START *************************
  // ************************************************************************

  const RpcPad* trigger_pad = data.rpc_pad();
  
  msg() << MSG::INFO << "Start dumping the RPC data" << std::endl;
  
  RPCrawData rpc_digits;       // container for storing the RPC digits
  //CMAdata    cma_patterns;     // container for storing the CMA patterns
  
  // get the Sector Logic and the PAD Id
  unsigned long int sector = (long) trigger_pad->sector();
    
  unsigned short int SubsystemId = (sector < 32)? 0 : 1;
  unsigned short int SectorId    = sector%32;
                     PADId       = trigger_pad->onlineId();
    
  // loop over CMAs into the PAD to load data into Level-1 logic
  RpcPad::const_iterator it3 = trigger_pad->begin(); 
  RpcPad::const_iterator it4 = trigger_pad->end();
  for (; it3!=it4; ++it3) 
  {
      const RpcCoinMatrix * cma = (*it3);

      unsigned long int cmaId = (long) cma->onlineId();
       
      unsigned short int CMAId = cmaId & 3;
      unsigned short int lh    = (cmaId >> 2) & 1;
       
      const CMAparameters* CMA = m_svc->rpccab()->
                                    give_CMA(SubsystemId,SectorId,PADId,CMAId);
       
      
        std::list <unsigned int> CodeList;

       
        //for each cma for over fired channels
        RpcCoinMatrix::const_iterator it5 = (*it3)->begin(); 
        RpcCoinMatrix::const_iterator it6 = (*it3)->end();

/*	
	std::string trig_type = (lh)? ", High Pt " : ", Low Pt ";
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
        __osstream display;
#else                                // put your gcc 2.95 specific code here
        char buffer[80];
        for (int i=0;i<80;++i) buffer[i] = '\0';
        __osstream display(buffer,80);
#endif
        display << CMA->id()
	        << trig_type+"contains " << cma->size() << " fired channels:";
	
	Mlog << MSG::INFO << "GET NEW RPC" << endl
	     << display.str() << endreq;
	     
	int ch = 0;
*/     
        for (; it5!=it6; ++it5) 
        {
            CodeList.clear();
	    const RpcFiredChannel * fChannel = (*it5);

            unsigned short int ijk     = fChannel->ijk();
            unsigned short int channel = fChannel->channel();
	    float time                 = fChannel->time();
	    
	    //cout << "ijk = " << ijk
	    //     << ", channel = " << channel << endl;
	    
	    // load Low-pt data into Level-1 logic
	    if(lh == 0 && ijk != 6 && ijk != 7)
	    {
	        CMA->give_strip_code(sector,lh,ijk,channel,CodeList);
		//std::list<unsigned int>::const_iterator it = CodeList.begin();
		//while (it != CodeList.end())
		//{
		//    cout << "code = " << *it << endl; 
		//    ++it;
		//}
                RPCrawDigit Digit(0,CodeList,time,m_svc->rpcgeo());
		/*
		std::string side = (ijk<2)? "pivot" : "confirm";
		 
		if(ijk < 2) ch = channel;
		else        ch = channel+32*(ijk%2);

		Mlog << MSG::INFO
		     << side << " channel n. " << ch << " decoded as" << endl
		     << Digit << endreq;
		*/
		//const RPCdigit* digit = rpc_digits << Digit;
		//cma_patterns.create_patterns(CMA,digit);
            }
	    
	    // load High-pt data into Level-1 logic
	    if(lh == 1 && (ijk == 2 || ijk == 3 || ijk == 4 || ijk == 5) )
	    {
	        CMA->give_strip_code(sector,lh,ijk,channel,CodeList);
		//std::list<unsigned int>::const_iterator it = CodeList.begin();
		
		//while (it != CodeList.end())
		//{
		//    cout << "code = " << *it << endl; 
		//    ++it;
		//}
		
		RPCrawDigit Digit(0,CodeList,time,m_svc->rpcgeo());
		/*
		std::string side = (ijk<2)? "pivot" : "confirm";
		 
		if(ijk < 2) ch = channel;
		else        ch = channel+32*(ijk%2);

		Mlog << MSG::INFO
		     << side << " channel n. " << ch << " decoded as" << endl
		     << Digit << endreq;
		*/
		//const RPCdigit* digit = rpc_digits << Digit;
		//cma_patterns.create_patterns(CMA,digit);
            }	    
        }
    }
    
    //display the data
    
    if(m_eventsProcessed <= m_evtToShow)
    {
        m_msg->lock();
        msg() << MSG::INFO
              << ShowData<RPCrawData>(rpc_digits,"",false) << std::endl
	      << endreq;
        m_msg->unlock();

        msg() << MSG::INFO
              << "Eta digits in Low Pt  station: [" 
	      << rpc_digits.how_many(2,-1,1,-1,4,-1) 
	      << "] [" << rpc_digits.how_many(2,-1,1,-1,3,-1) 
	      << "]" << endreq;
        msg() << MSG::INFO
              << "Eta digits in Pivot   station: [" 
	      << rpc_digits.how_many(2,-1,2,-1,4,-1) 
	      << "] [" << rpc_digits.how_many(2,-1,2,-1,3,-1) 
	      << "]" << endreq;
        msg() << MSG::INFO
              << "Eta digits in High Pt station: [" 
	      << rpc_digits.how_many(2,-1,3,-1,4,-1)
	      << "] [" << rpc_digits.how_many(2,-1,3,-1,3,-1) 
	      << "]" << endreq;

        msg() << MSG::INFO
              << "Phi digits in Low Pt  station: [" 
	      << rpc_digits.how_many(1,-1,1,-1,4,-1)
	      << "] [" << rpc_digits.how_many(1,-1,1,-1,3,-1) 
	      << "]" << endreq;
        msg() << MSG::INFO
              << "Phi digits in Pivot   station: [" 
	      << rpc_digits.how_many(1,-1,2,-1,4,-1)
	      << "] [" << rpc_digits.how_many(1,-1,2,-1,3,-1) 
	      << "]" << endreq;
        msg() << MSG::INFO
              << "Phi digits in High Pt station: [" 
	      << rpc_digits.how_many(1,-1,3,-1,4,-1)
	      << "] [" << rpc_digits.how_many(1,-1,3,-1,3,-1) 
	      << "]" << endreq;
    }

    const RPCrawData::DigitList etaDigits = rpc_digits.eta_digits();
    const RPCrawData::DigitList phiDigits = rpc_digits.phi_digits();
    
    RPCrawData::DigitList::const_iterator eta = etaDigits.begin();
    
    float RpcZeta[3] = {0.,0.,0.};
    int RpcDigits[3] = {0,0,0};
    
    while(eta != etaDigits.end())
    {
        float v[3];
        (*eta).xyz(v);
	
	int station = (*eta).decoding().lvl1_station() - 1;
	RpcZeta[station] += v[2];
	(RpcDigits[station])++;
	
        ++eta;
	//float R = sqrt(v[0]*v[0] + v[1]*v[1] );
	
	//fill the beam profiles
	m_rpc_beam_profile[station]->fill(v[2]);
	//fill the display plot
	m_RpcDisp->fill(v[2],v[0],1.);
    }
  
    float RpcResMean1 = (RpcDigits[0])? RpcZeta[0]/RpcDigits[0] : 0.;
    float RpcResMean2 = (RpcDigits[1])? RpcZeta[1]/RpcDigits[1] : 0.;
    float RpcresMean3 = (RpcDigits[2])? RpcZeta[2]/RpcDigits[2] : 0.;
    
    float RpcResBML = 0.;
    float RpcResBOL = RpcresMean3;
    if(RpcDigits[0] && RpcDigits[1]) RpcResBML = (RpcResMean1+RpcResMean2)/2.;
    else
    {
        if (RpcDigits[0]) RpcResBML = RpcResMean1;
	else RpcResBML = RpcResMean2;
    }
  
  
  // ************************************************************************
  // *********************** MDT DATA MONITOR START *************************
  // ************************************************************************
  
  const MuData::CSMlist& csm_data = data.mdt_csm();
  
  MuData::CSMlist::const_iterator csm = csm_data.begin();

  msg() << MSG::INFO << "Start dumping the MDT data" << std::endl;
  
  while(csm != csm_data.end())
  {   
      if(!(*csm)->empty())
      {
	  unsigned short int SubsystemId = (*csm)->SubDetId();
	  unsigned short int MrodId = (*csm)->MrodId();
	  unsigned short int LinkId = (*csm)->CsmId();

	  CSMid csmId = m_svc->mdtgeo()->getCsm(SubsystemId,MrodId,LinkId);
	
	  if(csmId)
	  {
	      float Zeta[100];
              for (int i=0; i<100;++i) Zeta[i] = 0.;
      
              int TDCcounts = 0;
	      
	      if(m_eventsProcessed <= m_evtToShow)
              {
	          msg() << MSG::INFO << " ============================== CSM "
	                << csmId.mnemonic()
		        << " ===============================" << std::endl;
                  msg() << MSG::INFO << "CSM with Subsystem " << SubsystemId
		        << ", MrodId " << MrodId
		        << ", LinkId " << LinkId
		        << " : eta Readout Tower " << csmId.eta_tower() 
		        << std::endl;   
              }
	      
	      //const Chamber* cha = csmId.get_chamber();


	      //int StationEta = csmId.StationEta();
		    //std::string StationName = csmId.name_str();
	      //int StationName = csmId.name();

	      MdtCsm::const_iterator amt = (*csm)->begin();
	      while(amt != (*csm)->end())
	      {
	          if((*amt)->leading())
	          {
		      unsigned short int TdcId = (*amt)->tdcId();
		      unsigned short int ChannelId = (*amt)->channelId();
		      unsigned short int drift = (*amt)->fine() | 
                                                       ( (*amt)->coarse() << 5);

                      const TDCgeo* tdc = csmId.get_tdc(TdcId);
		      if(!tdc && ChannelId < 24)
                      {
                          msg() << "   TDC # " << TdcId
			        << " not found into the chamber structure!" 
			        << endreq;   
                      }

                      if( csmId.mnemonic() != "EOL3A01" || TdcId != 17 )
		      {
		          if(m_eventsProcessed <= m_evtToShow)
                          {
                              msg() << "   TDC # "  << std::setw(2) << TdcId 
                                    << "  TDC ch "  << std::setw(2) << ChannelId 
                                    << "  drift -> " << std::setw(4) << drift;
                          }
                          //int layer = tdc->Lay[ChannelId] - 1;
			  // CID 26429: FORWARD_NULL
                          // float Z = tdc->RZ[ChannelId][1];
                          // float R = tdc->RZ[ChannelId][0];
		          // int Tube       = tdc->Tub[ChannelId];
                          // int Layer      = tdc->Lay[ChannelId];
			  float Z = (tdc) ? tdc->RZ[ChannelId][1] : 0;
			  float R = (tdc) ? tdc->RZ[ChannelId][0] : 0;
			  int Tube       = (tdc) ? tdc->Tub[ChannelId] : 0;
			  int Layer      = (tdc) ? tdc->Lay[ChannelId] : 0;
		          int TubeLayers = (csmId.get_chamber(TdcId))->multi[0]->TubeLayers;
		          int MultiLayer = 1;
			  
                          if(TDCcounts<100) Zeta[TDCcounts++] = Z;
                          
			  if (Layer > TubeLayers)
                          { 
		              Layer -= TubeLayers;
			      MultiLayer = 2;
                          }
			  
			  if(m_eventsProcessed <= m_evtToShow)
                          {		          
			      msg() << MSG::INFO << ":  ML=" << std::setw(1) << MultiLayer
		              << ", LY="  << std::setw(1) << Layer
			      << ", TB="  << std::setw(2) << Tube 
			      << setiosflags(std::ios::fixed)
			      << ":  Z=" << std::setw(7) << std::setprecision(2) << Z 
			      << ", R="  << std::setw(7) << std::setprecision(2) << R 
			      << std::endl;
                          }
			
			  // fill the display histos and the mdt beam profiles
			  //m_display->fill(Z,R,1.);
			  
			  if(csmId.mnemonic()[0]=='B') 
			  {
			      m_MdtDisp->fill(Z,R,1.);
			      if(csmId.mnemonic()[1]=='I')
			                     m_mdt_beam_profile[0]->fill(Z,1.);
			      if(csmId.mnemonic()[1]=='M')
			                     m_mdt_beam_profile[1]->fill(Z,1.);
			      if(csmId.mnemonic()[1]=='O')
			                     m_mdt_beam_profile[2]->fill(Z,1.);
			  }
                      
		          
		          if(csmId.mnemonic()[0]=='B') m_MdtDisp->fill(Z,R,1.);
		      
		      }
			      
                  }
	     
		  ++amt;
	      }
	      
	      //compute the mean Z value
	      float sum  = 0.;
	      for(int i=0; i<TDCcounts;++i) sum += Zeta[i];
	      float mean = sum/TDCcounts;
	      //book chamber tube Z residual
	      int histoNum = (MrodId == 2)? LinkId-1 : LinkId + 5;
	      for(int i=0; i<TDCcounts;++i) 
	          (m_residuals[histoNum])->fill(Zeta[i]-mean,1.);

		  
              //fill the correlation plots between RPC and MDT
	      if(MrodId == 2 && LinkId == 4)
	      {
	          if(RpcResBML!=0.) m_BML_RPC_MDT->fill(RpcResBML-mean,1.);  
	      }
	      if(MrodId == 2 && LinkId == 6)
	      {
	          if(RpcResBOL!=0.) m_BOL_RPC_MDT->fill(RpcResBOL-mean,1.); 
	      }
	      
	      
            }
            else
            {

                msg() << "CSM for Subsystem " << SubsystemId
	              << ", MrodId " << MrodId
	  	      << ", LinkId " << LinkId << " not found!" << std::endl;
            }

        }
	++csm;
    }
    
    msg() << endreq;
    
    // build muon features
    MuonFeature* muon_feature = new MuonFeature(1,0,3250,-0.030,0.016,0,0,0,0);

    // seeding the Event Filter ///////////////////////////////////////////////
    std::string skey = "";                                                   //
    status = recordAndAttachFeature( outputTE, muon_feature, skey, "");      //
    if(status!=HLT::OK){                                                     //
        msg() << MSG::ERROR                                                  //
             << "Record of Muon Feature in TriggerElement failed" << endreq; //
        return status;                                                       //
                                                                             //
    } else {                                                                 //
        if(msgLvl() <= MSG::DEBUG)                                           //
            msg() << MSG::INFO                                               //
                  << "Muon Feature attached to the TriggerElement"           //
                  << endreq;                                                 //
	                                                                     //
    }                                                                        //
                                                                             //
                                                                             //
    outputTE->setActiveState(true);                                          //
    ///////////////////////////////////////////////////////////////////////////
  
    return HLT::OK;
}

