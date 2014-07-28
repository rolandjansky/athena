/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/DataMonitor.h"

#include "TrigmuFast/MuData.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/MdtCsmContainer.h"

#include "TrigT1RPClogic/RPCrawData.h"
#include "TrigT1RPClogic/CMAdata.h"


#include "TrigT1Result/RoIBResult.h"

#include "TrigT1RPClogic/ShowData.h"

#include "TrigmuFast/LVL1emu.h"
#include "TrigmuFast/RpcPatRec.h"
#include "TrigmuFast/MdtPatRec.h"
#include "TrigmuFast/MuonFex.h"

#include "RPCcablingInterface/CMAparameters.h"
#include "RPCcablingInterface/RpcPadIdHash.h" 

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "GaudiKernel/ITHistSvc.h"
#include "MuonContainerManager/MuonRdoContainerAccess.h"

using namespace std;

DataMonitor::DataMonitor(const std::string& name, ISvcLocator* pSvcLocator):
Algorithm(name, pSvcLocator),
    m_rpc_converter("RPC_RawDataProviderTool"),
    m_mdt_converter("MDT_RawDataProviderTool"),
    m_tgc_converter("TGC_RawDataProviderTool"),
    m_csc_converter("CSC_RawDataProviderTool"),
    m_rpc_geometry("RPCgeometrySvc",""),
    m_tgc_geometry("TGCgeometrySvc",""),
    m_csc_geometry("CSCgeometrySvc",""),
    m_mdt_cabling ("MDTcablingSvc","")
{    
    declareProperty("RobId", m_RobIds);
    declareProperty("MdtId", m_MdtIds);
    declareProperty("SubsystemId",m_subsysID=1); // corresponding to |eta| > 0
    declareProperty("RoiNumber",m_RoINumber=10); // corresponding to PAD = 2
    declareProperty("SectorId",m_sectorID=31);   // cabled sector at H8
    declareProperty("histoPathBase",m_histo_path_base="/stat/Monitoring");
    
    declareProperty("UseCalibration",m_use_calib=false);
        
    declareProperty("MUlvl1INFO",m_MUlvl1_info=false);
    declareProperty("MUtrackINFO",m_MUtrack_info=false);
    declareProperty("MUroadsINFO",m_MUroads_info=false);
    declareProperty("MUdecoINFO",m_MUdeco_info=false);
    declareProperty("MUcontINFO",m_MUcont_info=false);
    declareProperty("MUfitINFO",m_MUfit_info=false);
    declareProperty("MUsagINFO",m_MUsag_info=false);
    declareProperty("MUptINFO",m_MUpt_info=false);
    declareProperty("TestString",m_test_string=""); 
    
    declareProperty( "RPC_RawDataProvider", m_rpc_converter);
    declareProperty( "MDT_RawDataProvider", m_mdt_converter);
    declareProperty( "TGC_RawDataProvider", m_tgc_converter);
    declareProperty( "CSC_RawDataProvider", m_csc_converter);
    
    declareProperty( "RPC_Geometry", m_rpc_geometry);
    declareProperty( "TGC_Geometry", m_tgc_geometry);
    declareProperty( "CSC_Geometry", m_csc_geometry);
    declareProperty( "MDT_Cabling",  m_mdt_cabling);

    // CID 23231: UNINIT_CTOR
    for(int i=0; i<12; i++) m_residuals[i] = 0;
    for(int i=0; i<3; i++)  m_mdt_beam_profile[i] = 0;
    for(int i=0; i<3; i++)  m_rpc_beam_profile[i] = 0;
}


StatusCode DataMonitor::initialize()
{
  MsgStream log(messageService(), name());
  log << MSG::DEBUG
      << "Initialize DataMonitor"
      << endreq;
   
  // initialize StoreGate service
  StatusCode sc = service("StoreGateSvc", m_pStoreGate);
  if (sc.isFailure()) 
  {
      log << MSG::ERROR
          << "Unable to retrieve the StoreGate Service" << endreq;
      return sc;
  }
  
  sc = m_rpc_geometry.retrieve();
  if ( sc.isFailure() )
  {
    log << MSG::ERROR << "Could not retrieve " 
                        << m_rpc_geometry << endreq;
    return sc;
  } else
  {
    log << MSG::INFO << "Retrieved service " << m_rpc_geometry << endreq;
  }
  
  sc = m_tgc_geometry.retrieve();
  if ( sc.isFailure() )
  {
    log << MSG::ERROR << "Could not retrieve " 
                        << m_tgc_geometry << endreq;
    return sc;
  } else
  {
    log << MSG::INFO << "Retrieved service " << m_tgc_geometry << endreq;
  }
  
  sc = m_csc_geometry.retrieve();
  if ( sc.isFailure() )
  {
    log << MSG::ERROR << "Could not retrieve " 
                        << m_csc_geometry << endreq;
    return sc;
  } else
  {
    log << MSG::INFO << "Retrieved service " << m_csc_geometry << endreq;
  }
  
  sc = m_mdt_cabling.retrieve();
  if ( sc.isFailure() )
  {
    log << MSG::ERROR << "Could not retrieve " 
                        << m_mdt_cabling << endreq;
    return sc;
  } else
  {
    log << MSG::INFO << "Retrieved service " << m_mdt_cabling << endreq;
  }
  
  // ======================== GET ROBDATAPROVIDER ============================

  IService* svc ;
  if(StatusCode::SUCCESS != service("ROBDataProviderSvc",svc)){
    log << MSG::ERROR << " Can't get ROBDataProviderSvc " << endreq;
    return StatusCode::FAILURE;
  }

  m_pROBDataProvider = dynamic_cast<ROBDataProviderSvc*> (svc);
  if(m_pROBDataProvider == 0 ) {
    log<<MSG::ERROR<< " Cant cast to ROBDataProviderSvc " <<endreq;
    return StatusCode::FAILURE ;
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
			    
  m_msg = new MuMessageInterface(log,0,info,-1,m_test_string);
  m_svc = new MuServiceInterface(serviceLocator(),&m_rpc_geometry,&m_tgc_geometry,
				 &m_csc_geometry,&m_mdt_cabling,0,name(),m_use_calib,false,true,false,false,1.);
  if(!m_svc->isLoaded())
  {
      log << MSG::FATAL
	  << "unable to set the Muon Service Interface"
	  << endreq;
      return StatusCode::FAILURE;
  }


  
  // ************************************************************************
  // *********** SETTING UP ROBLIST AND MDT LIST FOR TESTBEAM DATA **********
  // ************************************************************************
  
  std::vector<int>::const_iterator it = m_RobIds.begin();
  while (it != m_RobIds.end())
  {
      m_robIds.push_back(*it);
      ++it;
  }
   
  // Collect the full ROB list //////////////////////////////////////////////
#ifndef NDEBUG                                                             //
  log << MSG::DEBUG                                                        //
       << " Number of ROBs to access = "                                   //
       << m_robIds.size() << endreq;                                       //
  log << MSG::DEBUG                                                        //
       << " List of ROBs: "  << endreq;                                    //
  for (std::vector<uint32_t>::const_iterator i=m_robIds.begin();           //
       i != m_robIds.end(); ++i)                                           //
  {                                                                        //
      log << MSG::DEBUG << *i << endreq;                                   //
  }                                                                        //
#endif                                                                     //
  ///////////////////////////////////////////////////////////////////////////
   
  
                                              
  std::vector<int>::const_iterator hs = m_MdtIds.begin();
  while (hs != m_MdtIds.end())
  {
      m_listOfMDTIds.push_back(*hs);
      ++hs;
  }
 
 
  
  // ************************************************************************
  // ******************* SETTING HISTOGRAMS FOR MONITORING ******************
  // ************************************************************************

  for (int l=0;l<6;++l)
  { 
      CSMid csmId = m_svc->mdtgeo()->getCsm(0x61,2,l);
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
      CSMid csmId = m_svc->mdtgeo()->getCsm(SubId,3,l);
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
  
  // ====================== CREATE THE ALGO MONITOR =========================
  
  ITHistSvc *rootHistSvc;
  sc = service("THistSvc", rootHistSvc, true);
  if ( sc.isFailure() ) {
      log << MSG::ERROR << "Unable to locate THistSvc" << endreq;
      return StatusCode::FAILURE;
  }
  
  
  m_monitor = new OnlineSurvey(rootHistSvc,m_histo_path_base);
 
  
  log << MSG::INFO 
      << "Initialization completed successfully" 
      << endreq;

  return StatusCode::SUCCESS;
}

StatusCode DataMonitor::finalize()
{
    return StatusCode::SUCCESS;
}

StatusCode DataMonitor::execute()
{ 
//#ifndef NDEBUG
  MsgStream log(messageService(), name());
//#endif

#ifndef NDEBUG
  log << MSG::DEBUG
      << "Executing DataMonitor"
      << endreq;
#endif

  StatusCode sc;

  service("StoreGateSvc", m_pStoreGate);
  
  // ************************************************************************
  // ******************** START ACCESSING TESTBEAM DATA *********************
  // ************************************************************************




  // Preload the data into the ROBdataProvider //////////////////////////////
  m_pROBDataProvider->addROBData(m_robIds);                                //
  ///////////////////////////////////////////////////////////////////////////
  
  
  // crate the data holder //////////////////////////////////////////////////
  MuData data;                                                             //
  ///////////////////////////////////////////////////////////////////////////


  // Retrieve the PAD identifiers using the cabling service /////////////////
  Identifier rpc_pad_id;                                                   //
  unsigned int logic_sector = 0;                                           //
  unsigned short int PADId  = 0;                                           //
                                                                           //
  if(!m_svc->rpccab()->give_PAD_address(m_subsysID,                        //
                                        m_sectorID,                        //
					m_RoINumber,                       //
					logic_sector,PADId,rpc_pad_id) )   //
  {                                                                        //
      log << MSG::ERROR                                                    //
        << "Cannot retrieve the PAD identifier from the Cabling Service"   //
        << endreq;                                                         //
      return false;                                                        //
  }                                                                        //
                                                                           //
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
     if ( log.level() <= MSG::DEBUG )                                      //
          log << MSG::DEBUG << "Rpc PAD container not registered by "      //
	                      << "MuonRdoContainerManager; retrieving it " //
			      << "from the store! " << endreq;             //
                                                                           //
      sc = m_pStoreGate->retrieve(PAD, "RPCPAD");                          //
      if (sc.isFailure()){                                                 //
          log << MSG::ERROR << "Retrieval of RpcPadContainer failed"       //
                              << endreq;                                   //
          return false;                                                    //
      }                                                                    // 
  }                                                                        //
                                                                           //
  m_pROBDataProvider->getROBData(m_robIds,vecRobs);                        // 
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
     if ( log.level() <= MSG::DEBUG )                                      //
          log << MSG::DEBUG << "MDT CSM container not registered by "      //
	                    << "MuonRdoContainerManager; retrieving it "   //
			    << "from the store! " << endreq;               //
                                                                           //
      sc = m_pStoreGate->retrieve(CSM, "MDTCSM");                          //
      if (sc.isFailure()){                                                 //
          log << MSG::ERROR << "Retrieval of MdtCsmContainer failed"       //
                              << endreq;                                   //
          return false;                                                    //
      }                                                                    // 
  }                                                                        //
                                                                           //
  m_mdt_converter->convert(vecRobs,m_listOfMDTIds);                        //
                                                                           //
  for(unsigned int i = 0; i < m_listOfMDTIds.size(); i++)                  //
  {                                                                        //
      MdtCsmContainer::const_iterator pCsmIt =                             //
      CSM->indexFind(m_listOfMDTIds[i]);                                   //
                                                                           //
      if(pCsmIt!=CSM->end())                                               //
      {                                                                    //
#ifndef NDEBUG                                                             //
          log << MSG::INFO                                                 //
	       << "Number of digit in Collection: "                        //
               << (*pCsmIt)->size()                                        //
	       << endreq;                                                  //
#endif                                                                     //
          data.get_csm(*pCsmIt);                                           //
      }                                                                    //
  }                                                                        //
  ///////////////////////////////////////////////////////////////////////////




  // ************************************************************************
  // *********************** RPC DATA MONITOR START *************************
  // ************************************************************************

  const RpcPad* trigger_pad = data.rpc_pad();
  
  log << MSG::INFO << "Start dumping the RPC data" << endl;
  
  RPCrawData rpc_digits;       // container for storing the RPC digits
  //CMAdata    cma_patterns;     // container for storing the CMA patterns
  // get the Sector Logic and the PAD Id
  unsigned long int sector = (long) trigger_pad->sector();
  //unsigned long int sector = 63;
  //cout << "sector=" << sector << endl;
  unsigned short int SubsystemId = (sector < 32)? 0 : 1;
  unsigned short int SectorId    = sector%32;
                     PADId       = trigger_pad->onlineId();
                     //PADId       = 2;
		     
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

	
//	std::string trig_type = (lh)? ", High Pt " : ", Low Pt ";
//#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
//        __osstream display;
//#else                                // put your gcc 2.95 specific code here
//        char buffer[80];
//        for (int i=0;i<80;++i) buffer[i] = '\0';
//        __osstream display(buffer,80);
//#endif
//        display << CMA->id()
//	        << trig_type+"contains " << cma->size() << " fired channels:";
	
//	log << MSG::INFO << "GET NEW RPC" << endl
//	     << display.str() << endreq;
	     
//	int ch = 0;
     
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
		
		//std::string side = (ijk<2)? "pivot" : "confirm";
		 
		//if(ijk < 2) ch = channel;
		//else        ch = channel+32*(ijk%2);

		//log << MSG::INFO
		//     << side << " channel n. " << ch << " decoded as" << endl
		//     << Digit << endreq;
		
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
		
		//std::string side = (ijk<2)? "pivot" : "confirm";
		 
		//if(ijk < 2) ch = channel;
		//else        ch = channel+32*(ijk%2);

		//log << MSG::INFO
		//     << side << " channel n. " << ch << " decoded as" << endl
		//     << Digit << endreq;
		
		//const RPCdigit* digit = rpc_digits << Digit;
		//cma_patterns.create_patterns(CMA,digit);
            }	    
        }
    }
    
    //display the data
    
    m_msg->lock();
    log << MSG::INFO
         << ShowData<RPCrawData>(rpc_digits,"",false) << endl
	 << endreq;
    m_msg->unlock();

        log << MSG::INFO
            << "Eta digits in Low Pt  station: [" 
	    << rpc_digits.how_many(2,-1,1,-1,4,-1) 
	    << "] [" << rpc_digits.how_many(2,-1,1,-1,3,-1) 
	    << "] [" << rpc_digits.how_many(2,-1,1,-1,2,-1) 
	    << "] [" << rpc_digits.how_many(2,-1,1,-1,1,-1) 
	    << "]" << endreq;
        log << MSG::INFO
            << "Eta digits in Pivot   station: [" 
	    << rpc_digits.how_many(2,-1,2,-1,4,-1) 
	    << "] [" << rpc_digits.how_many(2,-1,2,-1,3,-1) 
	    << "] [" << rpc_digits.how_many(2,-1,2,-1,2,-1) 
	    << "] [" << rpc_digits.how_many(2,-1,2,-1,1,-1) 
	    << "]" << endreq;
        log << MSG::INFO
            << "Eta digits in High Pt station: [" 
	    << rpc_digits.how_many(2,-1,3,-1,4,-1)
	    << "] [" << rpc_digits.how_many(2,-1,3,-1,3,-1) 
	    << "] [" << rpc_digits.how_many(2,-1,3,-1,2,-1)
	    << "] [" << rpc_digits.how_many(2,-1,3,-1,1,-1) 
	    << "]"  << endreq;

        log << MSG::INFO
            << "Phi digits in Low Pt  station: [" 
	    << rpc_digits.how_many(1,-1,1,-1,4,-1)
	    << "] [" << rpc_digits.how_many(1,-1,1,-1,3,-1) 
	    << "] [" << rpc_digits.how_many(1,-1,1,-1,2,-1)
	    << "] [" << rpc_digits.how_many(1,-1,1,-1,1,-1) 
	    << "]" << endreq;
        log << MSG::INFO
            << "Phi digits in Pivot   station: [" 
	    << rpc_digits.how_many(1,-1,2,-1,4,-1)
	    << "] [" << rpc_digits.how_many(1,-1,2,-1,3,-1) 
	    << "] [" << rpc_digits.how_many(1,-1,2,-1,2,-1)
	    << "] [" << rpc_digits.how_many(1,-1,2,-1,1,-1) 
	    << "]" << endreq;
        log << MSG::INFO
            << "Phi digits in High Pt station: [" 
	    << rpc_digits.how_many(1,-1,3,-1,4,-1)
	    << "] [" << rpc_digits.how_many(1,-1,3,-1,3,-1) 
	    << "] [" << rpc_digits.how_many(1,-1,3,-1,2,-1)
	    << "] [" << rpc_digits.how_many(1,-1,3,-1,1,-1) 
	    << "]" << endreq;


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

  log << MSG::INFO << "Start dumping the MDT data" << endl;
  
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
	      
	      log << MSG::INFO << " ============================== CSM "
	          << csmId.mnemonic()
		  << " ===============================" << endl;
              log << MSG::INFO << "CSM with Subsystem " << SubsystemId
		  << ", MrodId " << MrodId
		  << ", LinkId " << LinkId
		  << " : eta Readout Tower " << csmId.eta_tower() << endl;   
              
	      const Chamber* cha = csmId.get_chamber();


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

                      TDCgeo* tdc = cha->TDC[TdcId];
		      if(!tdc && ChannelId < 24)
                      {
                          log << MSG::INFO << "   TDC # " << TdcId
			       << " not found into the chamber structure!" 
			       << endreq;
			  //return StatusCode::FAILURE;                        
                      }

                      if( csmId.mnemonic() != "EOL3A01" || TdcId != 17 )
		      {
                          log << MSG::INFO << "   TDC # " << setw(2) << TdcId 
                              << "  TDC ch " << setw(2) << ChannelId 
                              << "  drift -> " << setw(4) << drift;

                          //int layer = tdc->Lay[ChannelId] - 1;
			  // CID 26428: FORWARD_NULL
                          // float Z = tdc->RZ[ChannelId][1];
                          // float R = tdc->RZ[ChannelId][0];
		          // int Tube       = tdc->Tub[ChannelId];
                          // int Layer      = tdc->Lay[ChannelId];
			  float Z = (tdc) ? tdc->RZ[ChannelId][1] : 0;
			  float R = (tdc) ? tdc->RZ[ChannelId][0] : 0;
			  int Tube       = (tdc) ? tdc->Tub[ChannelId] : 0;
			  int Layer      = (tdc) ? tdc->Lay[ChannelId] : 0;
		          int TubeLayers = cha->multi[0]->TubeLayers;
		          int MultiLayer = 1;
			  
                          if(TDCcounts<100) Zeta[TDCcounts++] = Z;
                          
			  if (Layer > TubeLayers)
                          { 
		              Layer -= TubeLayers;
			      MultiLayer = 2;
                          }
			  		          
			  log << MSG::INFO << ":  ML=" << setw(1) << MultiLayer
		              << ", LY="  << setw(1) << Layer
			      << ", TB="  << setw(2) << Tube 
			      << setiosflags(ios::fixed)
			      << ":  Z=" << setw(7) << setprecision(2) << Z 
			      << ", R="  << setw(7) << setprecision(2) << R 
			      << endl;

			
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
	      int histoNum = (MrodId == 2)? LinkId : LinkId + 6;
	      for(int i=0; i<TDCcounts;++i) 
	          (m_residuals[histoNum])->fill(Zeta[i]-mean,1.);

		  
              //fill the correlation plots between RPC and MDT
	      if(MrodId == 2 && LinkId == 3)
	      {
	          if(RpcResBML!=0.) m_BML_RPC_MDT->fill(RpcResBML-mean,1.);  
	      }
	      if(MrodId == 2 && LinkId == 5)
	      {
	          if(RpcResBOL!=0.) m_BOL_RPC_MDT->fill(RpcResBOL-mean,1.); 
	      }
	      
	      
            }
            else
            {

                log  << MSG::INFO << "CSM for Subsystem " << SubsystemId
	             << ", MrodId " << MrodId
	  	     << ", LinkId " << LinkId << " not found!" << endl;
            }

        }
	++csm;
    }
    
    log << MSG::INFO << endreq;
    
    
 
    
  // ************************************************************************
  // ********************** MUFAST ALGORITHM START **************************
  // ************************************************************************

     
  // Level-1 emulation //////////////////////////////////////////////////////
  LVL1emu lvl1_emu(*m_msg,*m_svc,false,false,false,0.,data);               //
  if(!lvl1_emu.start())                                                    //
  {                                                                        //
      return StatusCode::SUCCESS;                                          //
  }                                                                        //
                                                                           //
  const TriggerData& LVL1trig_data = lvl1_emu.trigger_data();              //
  ///////////////////////////////////////////////////////////////////////////

  // RPC Pattern Recognition ////////////////////////////////////////////////
  RpcPatRec RPCpatternRecognition(*m_msg,*m_svc,false,false,LVL1trig_data); //
  if(!RPCpatternRecognition.start())                                       //
  {                                                                        //
      //MuonFeature* muon_feature = new MuonFeature;                       //
      //return muFastSeed(outputTE,muon_feature);                          //
  }                                                                        //
                                                                           //
  const TrackFit rpc_track     = RPCpatternRecognition.trig_track();       //
  const RoadRegions muon_roads = RPCpatternRecognition.muon_roads();       //
  ///////////////////////////////////////////////////////////////////////////
  
  // MDT Pattern Recognition ////////////////////////////////////////////////
  MdtPatRec MDTpatternRecognition(*m_msg,*m_svc,rpc_track,muon_roads,data);//
  // CID 22894: ENUM_AS_BOOLEAN                                            //
  // if(!MDTpatternRecognition.start())                                    //
  MUFAST::ErrorCode code = MDTpatternRecognition.start();                  //
  if( code!=MUFAST::NO_ERROR )                                             //
  {                                                                        //
      //MuonFeature* muon_feature = new MuonFeature;                         //
      //return muFastSeed(outputTE,muon_feature);                            //
  }                                                                        //
                                                                           //
  const DigiPointer reco_digits = MDTpatternRecognition.digits();          //
  ///////////////////////////////////////////////////////////////////////////
  
  // Features extraction ////////////////////////////////////////////////////
  MuonFex fexExtraction(*m_msg,*m_svc,false,LVL1trig_data,rpc_track,       //
                        reco_digits.digi);                                 //
  if(!fexExtraction.start())                                               //
  {                                                                        //
      //MuonFeature* muon_feature = new MuonFeature;                         //
      //return muFastSeed(outputTE,muon_feature);                            //
  }                                                                        //
                                                                           //
  //MuonFeature* muon_feature = fexExtraction.muonFeatures();                //
  ///////////////////////////////////////////////////////////////////////////  

  
  // ************************************************************************
  // ********************* START MONITORING SECTION *************************
  // ************************************************************************
  
  //m_monitor->update(lvl1_emu,MDTpatternRecognition,fexExtraction);
  m_monitor->update(MDTpatternRecognition,fexExtraction);
 
  return StatusCode::SUCCESS;
}

