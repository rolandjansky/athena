/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LVL1_STANDALONE

#include "TrigT1RPCsteering/TrigT1RPC.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "GeneratorObjects/McEventCollection.h"  // include for retrieving the Montecarlo
#include "HepMC/GenEvent.h"                      // thru

#include "TrigT1RPClogic/ShowData.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"

#include "MuonIdHelpers/RpcIdHelper.h"


#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonDigitContainer/RpcDigitCollection.h"
#include "MuonDigitContainer/RpcDigit.h"

#include "MuonReadoutGeometry/RpcReadoutElement.h"

#include "TrigT1RPCmonitoring/DetailedTW.h"

#include "TrigT1RPClogic/decodeSL.h"

#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"


#include <algorithm>
#include <cmath>


using namespace std;

static double time_correction(double, double, double);

static int digit_num = 0;
static int digit_out = 0;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

TrigT1RPC::TrigT1RPC(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_EvtStore("StoreGateSvc",name),
  m_cabling_getter("RPCcablingServerSvc/RPCcablingServerSvc","TrigT1RPC"),
  m_cabling(0),
  m_geometry("RPCgeometrySvc/RPCgeometrySvc","TrigT1RPC")
{
  declareProperty ( "EventStore",m_EvtStore,"StoreGate Service");
  declareProperty ( "FastDebug", m_fast_debug=0 );

  declareProperty ( "Monitoring", m_monitoring=0 );
  //m_monitoring.setBounds( 0, 31 );

  declareProperty ( "Geometric", m_geometric_algo=false );
  declareProperty ( "Hardware", m_hardware_emulation=true );
  declareProperty ( "Logic", m_logic_emulation=false );
  declareProperty ( "RPCbytestream", m_bytestream_production=false );
  declareProperty ( "RPCbytestreamFile", m_bytestream_file="" );

  declareProperty ( "DataDetail", m_data_detail=false );

  declareProperty ( "CMAdebug", m_cma_debug=0 );
  declareProperty ( "PADdebug", m_pad_debug=0 );
  declareProperty ( "SLdebug", m_sl_debug=0 );

  declareProperty ( "CMArodebug", m_cma_ro_debug=0 );
  declareProperty ( "PADrodebug", m_pad_ro_debug=0 );
  declareProperty ( "RXrodebug", m_rx_ro_debug=0 );
  declareProperty ( "SLrodebug", m_sl_ro_debug=0 );

  declareProperty ( "CMArostructdebug", m_cma_rostruct_debug=0 );
  declareProperty ( "PADrostructdebug", m_pad_rostruct_debug=0 );
  declareProperty ( "RXrostructdebug", m_rx_rostruct_debug=0 );
  declareProperty ( "SLrostructdebug", m_sl_rostruct_debug=0 );

  declareProperty ( "DetailedTW" , m_detailedTW=false);
  declareProperty ( "EffMonitor" , m_EffMonitor=false);
  declareProperty ( "TimeGeoMon" , m_time_geo_monitor=false);

  declareProperty ( "PatchForRpcTime", m_patch_for_rpc_time=true);

  declareProperty ( "MaxMuon" , m_max_muon=10);
  declareProperty ( "MaxRoI"  , m_max_roi=10);
  
  declareProperty ( "KeyForTruth" , m_key_for_truth="TruthEvent");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigT1RPC::initialize(){

  MsgStream mylog(msgSvc(), name());
  mylog << MSG::DEBUG << " in initialize()";
  mylog << endreq;
        
  StatusCode sc;

    // initialize the Storegate service
    sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
    if (sc != StatusCode::SUCCESS ) {
      mylog << MSG::FATAL << " Cannot get ActiveStoreSvc " << endreq;
      return sc ;
    }

    //m_digit_position = 0;
    //const MuonDetDescrManager* muon_mgr;

    StoreGateSvc* detStore = 0;
    
    sc = serviceLocator()->service("DetectorStore", detStore);
    if ( sc.isSuccess() ) {
        sc = detStore->retrieve( m_MuonMgr );
        if ( sc.isFailure() ) {
            mylog << MSG::FATAL
	          << " Cannot retrieve MuonGeoModel " << endreq;
            return sc;
	} else {
            m_rpcId = m_MuonMgr->rpcIdHelper();
        }
    } else {
        mylog << MSG::ERROR 
	      << " MuonDetDescrMgr not found in DetectorStore " << endreq;
    }

/*    
    sc = serviceLocator()->service("DetectorStore", detStore);
    if (sc.isFailure()) {
        mylog << MSG::FATAL << "DetectorStore service not found !" << endreq;  
    } else {
        // Get the RPC id helper from the detector store
        const DataHandle<RpcIdHelper> rpcHelper;
        sc = detStore->retrieve(rpcHelper, "RPCIDHELPER");
        if (sc.isFailure()) {
            mylog << MSG::FATAL << "Could not get RpcIdHelper !" << endreq;
	    return StatusCode::FAILURE;
        } else {
            mylog << MSG::DEBUG << " Found the RpcIdHelper. " << endreq;
	    m_rpcId = rpcHelper;
        }
    }
*/    
/*    if (sc.isSuccess()) 
    {
        sc = detStore->retrieve( muon_mgr );
        if (sc.isFailure())
	{
            mylog << MSG::ERROR << " Cannot retrieve MuonDetDescrMgr " << endreq;
            return sc;
        }
        else
	{
             m_digit_position = new MuonDigitPosition (muon_mgr);
	     //m_muonId = muon_mgr->muonIdHelper();
	     m_rpcId  = muon_mgr->rpcIdHelper();
	}
    } else
    {
        mylog << MSG::ERROR 
              << " MuonDetDescrMgr not found in DetectorStore " << endreq;
	return sc;
    }
*/
    // initialize StoreGate service
    sc = m_EvtStore.retrieve();
    if (sc.isFailure()) 
    {
        mylog << MSG::FATAL
	    << "Unable to retrieve the StoreGate Service" << endreq;
        return sc;
    }

    // initialize RPC cabling service
    sc = m_cabling_getter.retrieve();
    if (sc != StatusCode::SUCCESS ) 
    {
        mylog << MSG::FATAL << " Cannot get RPCcablingServer Service " << endreq;
        return sc ;
    }
    
    sc = m_cabling_getter->giveCabling(m_cabling);
    if (sc != StatusCode::SUCCESS ) 
    {
        mylog << MSG::FATAL
	      << " Cannot get Rpc CablingGetter Service from the getter" 
	      << endreq;
        return sc ;
    } 
    
    // initialize RPC geometry service
    sc = m_geometry.retrieve();
    if (sc != StatusCode::SUCCESS ) 
    {
        mylog << MSG::FATAL << " Cannot get RPC geometry Service " << endreq;
        return sc ;
    }
    
    // book the ntuple and initialize the ntuple maker for trigger efficiency
    if(m_EffMonitor)
    {     
        // Book the ntuple
        SmartDataPtr<NTuple::Directory>
            ntdir(ntupleSvc(),"/NTUPLES/FILE1/RPCTRIG");
        if ( !ntdir )
        { 
            //    otherwise create the directory
            ntdir = ntupleSvc()->createDirectory("/NTUPLES/FILE1/RPCTRIG");
        }
  
        // Check if the directory is valid
        if ( ! ntdir )  
        {
            mylog << MSG::ERROR << " failed to get ntuple directory" << endreq;
            return StatusCode::FAILURE;
        }
	
	NTuple::Tuple* EffNtup = ntupleSvc()->book (ntdir.ptr(), 9, 
                                           CLID_ColumnWiseTuple,"RPCTRIG");

	m_TrigEfficiency = new TrigEfficiency (EffNtup,m_max_muon,m_max_roi);
	
    } else m_TrigEfficiency = 0;

    
    
    if(m_time_geo_monitor) {
            
      SmartDataPtr<NTuple::Directory>
          ntdir(ntupleSvc(),"/NTUPLES/FILE1/TRIGT1RPC");
      if ( !ntdir )
      { 
          //    otherwise create the directory
          ntdir = ntupleSvc()->createDirectory("/NTUPLES/FILE1/TRIGT1RPC");
      }
  
      // Check if the directory is valid
      if ( ! ntdir )  
      {
          mylog << MSG::ERROR << " failed to get ntuple directory" << endreq;
	  return StatusCode::FAILURE;
      }
    
      m_ntuple = ntupleSvc()->book (ntdir.ptr(), 40, CLID_ColumnWiseTuple,"TIMEGEO");
      
      m_ntuple->addItem("StationType" , m_rpc_stationType).ignore();
      m_ntuple->addItem("StationEta"  , m_rpc_stationEta).ignore();
      m_ntuple->addItem("StationPhi"  , m_rpc_stationPhi).ignore();
      m_ntuple->addItem("DoubletR"    , m_rpc_doubletR).ignore();
      m_ntuple->addItem("DoubletZ"    , m_rpc_doubletZ).ignore();
      m_ntuple->addItem("DoubletP"    , m_rpc_doubletP).ignore();
      m_ntuple->addItem("GasGap"      , m_rpc_gasGap).ignore();
      m_ntuple->addItem("MeasurePhi"  , m_rpc_measurePhi).ignore();
      m_ntuple->addItem("Strip"       , m_rpc_strip).ignore();

      m_ntuple->addItem("StripCode"    , m_rpc_code).ignore();    
      m_ntuple->addItem("Time"         , m_rpc_time).ignore();
      m_ntuple->addItem("Geomodel_X"   , m_rpc_geomodel_x).ignore();
      m_ntuple->addItem("Geomodel_Y"   , m_rpc_geomodel_y).ignore();
      m_ntuple->addItem("Geomodel_Z"   , m_rpc_geomodel_z).ignore();
      m_ntuple->addItem("Standalone_X" , m_rpc_standalone_x).ignore();
      m_ntuple->addItem("Standalone_Y" , m_rpc_standalone_y).ignore();
      m_ntuple->addItem("Standalone_Z" , m_rpc_standalone_z).ignore();
      
  } else m_ntuple= 0;


  return StatusCode::SUCCESS;
}
 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode TrigT1RPC::execute() {

  static  MsgStream mylog(msgSvc(), name());
  mylog << MSG::DEBUG << "in execute()" << endreq;

  StatusCode sc;

  RPCsimuData data;         // instanciate the container for the RPC digits
  sc = fill_RPCdata(data);  // fill the data with RPC simulated digts

  mylog << MSG::DEBUG << "RPC data loaded from G3:" << endl
      << ShowData<RPCsimuData>(data,"",m_data_detail) << endreq;


  mylog << MSG::DEBUG << "RPC digits into station 1 ==> " 
      << data.how_many(-1,-1,1,-1,-1,-1) << endreq;

  mylog << MSG::DEBUG << "RPC digits into station 2 ==> " 
      << data.how_many(-1,-1,2,-1,-1,-1) << endreq;

  mylog << MSG::DEBUG << "RPC digits into station 3 ==> " 
      << data.how_many(-1,-1,3,-1,-1,-1) << endreq;

  // exit(1);

  // ******************** Start of Level-1 simulation section *****************


  unsigned long int debug;

  ///// Creates the CMA patterns from RPC digits /////////////////////////
  debug = (m_hardware_emulation)? m_cma_debug : m_fast_debug;           //
                       //m_cabling.operator->()                         //
  CMAdata patterns(&data,m_cabling,debug);                              //
                                                                        //
  mylog << MSG::DEBUG << "CMApatterns created from RPC digits:" << endl //
        << ShowData<CMAdata>(patterns,"",m_data_detail) << endreq;      //
  ////////////////////////////////////////////////////////////////////////


  ///// Creates the PAD patterns from the CMA patterns ///////////////////
  debug = (m_hardware_emulation)? m_pad_debug : m_fast_debug;           //
  PADdata pads(&patterns,debug);                                        //
                                                                        //
  mylog << MSG::DEBUG << "PADs created from CMA patterns:" << endl      //
        << ShowData<PADdata>(pads,"",m_data_detail) << endreq;          //  
  ////////////////////////////////////////////////////////////////////////


  ///// Creates the SL patterns from the PAD patterns ////////////////////
  debug = (m_hardware_emulation)? m_sl_debug : m_fast_debug;            //
  SLdata sectors(&pads,debug);                                          //
                                                                        //
  mylog << MSG::DEBUG << "Sector Logics created from PAD patterns:"     //
        << endl                                                         //
        << ShowData<SLdata>(sectors,"",m_data_detail) << endreq;        //  
  ////////////////////////////////////////////////////////////////////////

  ///// Access the SL word and fill the MuCTPInterface ///////////////////
  LVL1MUONIF::Lvl1MuCTPIInput * ctpiInRPC =                             //
                                    new LVL1MUONIF::Lvl1MuCTPIInput;    //
                                                                        //
  SLdata::PatternsList sectors_patterns = sectors.give_patterns();      //
  SLdata::PatternsList::iterator SLit = sectors_patterns.begin();       //
                                                                        //
  while(SLit != sectors_patterns.end())                                 //
  {                                                                     //
      SectorLogic* logic = (*SLit)->give_SectorL();                     //
      int sector     = (*SLit)->sector();                               //
      int subsystem  = (sector > 31)? 1 : 0;                            //
      int logic_sector  = sector%32;                                    //
      unsigned int data_word = logic->outputToMuCTPI();                 //
                                                                        //
#if (__GNUC__) && (__GNUC__ > 2) // gcc 3.2 specific code here          //
      mylog << MSG::DEBUG                                               //
            << "Input to MuCTPI: side=" << subsystem                    //
	    << ", SL= " << logic_sector                                 //
	    << ", RoI=" << TriggerRPC::ROI1(data_word)                  //
	    << ", Th=" << TriggerRPC::PT1(data_word)                    //
	    << ", data word " << MSG::hex << data_word                  //
	    << MSG::dec << endreq;                                      //
#else                            // gcc 2.95 specific code here         //   
      mylog << MSG::DEBUG                                               //
            << "Input to MuCTPI: side=" << subsystem                    //
	    << ", SL= " << logic_sector                                 //
	    << ", RoI=" << TriggerRPC::ROI1(data_word)                  //
	    << ", Th=" << TriggerRPC::PT1(data_word)                    //
	    << ", data word " << hex << data_word                       //
	    << dec << endreq;                                           //
#endif		                                                        //
                                                                        //
                                                                        //
      ctpiInRPC->setSectorLogicData(data_word,0,subsystem,logic_sector);//
                                                                        //
      TrigEfficiency::TrigData trigdata(sector,data_word);              //
      if(m_TrigEfficiency) *m_TrigEfficiency << trigdata;               //
                                                                        //
      ++SLit;                                                           //
  }                                                                     //
                                                                        //
  sc = m_EvtStore->record(ctpiInRPC,DEFAULT_L1MuctpiStoreLocationRPC);  //
                                                                        //
  if (sc.isFailure()) {                                                 //
    mylog << MSG::ERROR                                                 //
  	  << "ERROR registering "                                       //
          << "RPC Lvl1MuCTPIInput object in StoreGate !"                //
  	  << endreq;                                                    //
    return sc;                                                          //
  } else {                                                              //
    mylog << MSG::DEBUG << "put RPC Lvl1MuCTPIInput into SG" << endreq; //
  }                                                                     //
  ////////////////////////////////////////////////////////////////////////
  


  // ******************* Start of Level-1 computation section *****************

  if(m_detailedTW)
  {                                  //m_cabling.operator->()
      DetailedTW trigger_windows(&data,m_cabling);
  }

  if(m_detailedTW || m_TrigEfficiency)
  {
      // retrieving the Montecarlo thru from the Storegate
      const DataHandle<McEventCollection> mcCollptr;
      //std::string	key = "GEN_EVENT";
      std::string	key = m_key_for_truth;

      if (m_EvtStore->contains<McEventCollection>(key) &&
          m_EvtStore->retrieve(mcCollptr,key).isSuccess() )
      {    
        // int n=0;
          McEventCollection::const_iterator itr;
          for(itr = mcCollptr->begin(); itr!=mcCollptr->end(); ++itr) 
          {  
              HepMC::GenEvent::particle_const_iterator Part;
	  
	      for (Part = (*itr)->particles_begin(); 
                   Part!=(*itr)->particles_end(); 
                   ++Part ) 
              {
                  // store MCthru only for muon particles
	          if ( ((*Part)->pdg_id() == -13 || (*Part)->pdg_id() == 13) ) 
                  {
	              MontecarloThru mcthru;
                      HepMC::GenVertex* Vert = (*Part)->production_vertex();

                      double            Tphi = (*Part)->momentum().phi();
                      if (Tphi < 0.)    Tphi =  Tphi + (2*M_PI);

                      if (Vert) 
                      {
                          mcthru.vertex_x = (float) Vert->position().x()/10.;
                          mcthru.vertex_y = (float) Vert->position().y()/10.;
                          mcthru.vertex_z = (float) Vert->position().z()/10.;
                      }

		      mcthru.phi  = Tphi;

		      float Pt = (*Part)->momentum().perp();
		      float theta = atan(Pt/fabs((*Part)->momentum().z())); 
		      float eta = -log (tan(theta/2.) );

                      //int charge = 1;
                      //if ((*Part)->pdg_id()>0) charge = -1;

                      mcthru.p_x = (float) (*Part)->momentum().x();
                      mcthru.p_y = (float) (*Part)->momentum().y();
                      mcthru.p_z = (float) (*Part)->momentum().z();
                      
                      mcthru.eta = (mcthru.p_z >= 0.)? eta : -eta;
		      		      
                      if(m_TrigEfficiency) *m_TrigEfficiency << mcthru;

                      //n++;
	          }
              }
          }
      }
      else {
          mylog << MSG::WARNING << "Could not retrieve McEventCollection" 
	        << endreq;
      }
  }
    

  if(m_TrigEfficiency) m_TrigEfficiency->write_ntp(ntupleSvc());

  // ******************* Start of Level-1 monitoring section ******************

  // ******************* Start Byte Stream production *************************

  if(m_bytestream_production)
  {
      RPCbytestream bytestream (patterns,
			       (std::string) m_bytestream_file,
                               (unsigned long int) m_cma_ro_debug,
                               (unsigned long int) m_pad_ro_debug,
                               (unsigned long int) m_rx_ro_debug,
                               (unsigned long int) m_sl_ro_debug,
                               (unsigned long int) m_cma_rostruct_debug,
                               (unsigned long int) m_pad_rostruct_debug,
                               (unsigned long int) m_rx_rostruct_debug,
                               (unsigned long int) m_sl_rostruct_debug);

      // Test of the decoder
      //RpcByteStreamDecoder* decoder = new RpcByteStreamDecoder(&bytestream, m_cabling);
      //StatusCode sc = decoder->decodeByteStream();

      // Example on how to access the byte stream data.
      // Only the bare structure is dump. To access a specific data member, use
      // the interface methods of MatrixReadOut and PadReadOut classes.
      
      mylog << MSG::DEBUG << "Start retrieving data from the RPC bytestream class"
          << endreq;
      
      RPCbytestream::PAD_Readout PADmap = bytestream.pad_readout();
      RPCbytestream::PAD_Readout::iterator it = PADmap.begin();
      while (it != PADmap.end())
      {
          // load the PAD data into a ostrstream object
#if (__GNUC__) && (__GNUC__ > 2) // put your gcc 3.2 specific code here
          __osstream PADdata;
#else                            // put your gcc 2.95 specific code here
          char buffer[10000];
          for (int i=0;i<10000;++i) buffer[i] = '\0';
          __osstream PADdata(buffer,10000);
#endif
	  
	  (*it).second.give_pad_readout()->bytestream(PADdata);
	  
	  //access to PadReadOut class and print the informations inside
	  mylog << MSG::DEBUG << "Start dumping the PAD " << (*it).second.PAD()
	      << " bytestream structure" << endl 
              << PADdata.str() << endreq;

          //access to MatrixReadOut classes given in input to that PAD
          for (int i=0;i<8;++i)
	  {
	  
              mylog << MSG::DEBUG << "Start dumping the Matrix " << i 
	          << " into the PAD n. " << (*it).second.PAD() << endreq;
		  
              MatrixReadOut* matrix_read_out=(*it).second.matrices_readout(i);
	  
	      if(matrix_read_out)
	      {   // load the Matrix data into a ostrstream object
#if (__GNUC__) && (__GNUC__ > 2) // put your gcc 3.2 specific code here
                  __osstream CMAdata;
#else                            // put your gcc 2.95 specific code here
                  char buffer[10000];
                  for (int i=0;i<10000;++i) buffer[i] = '\0';
                  __osstream CMAdata(buffer,10000);
#endif

	          CMAdata << *matrix_read_out;
		  
		  mylog << MSG::DEBUG << CMAdata.str() << endreq;
	      }
	      else
	      {
	          mylog << MSG::DEBUG << "Matrix Read Out not loaded" << endreq;
	      }
	  }

          ++it;
      }

  }

  mylog << MSG::DEBUG << "TrigT1RPC terminated succesfully!" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode TrigT1RPC::finalize() {
 
   MsgStream mylog(msgSvc(), name());
   mylog << MSG::DEBUG << "in finalize()" << endreq;
 
   mylog << MSG::DEBUG << "processed digits = " << digit_num << endreq;
   mylog << MSG::DEBUG << "digits out of the time window = " 
                       << digit_out << endreq;

   if(m_TrigEfficiency) delete m_TrigEfficiency;

   return StatusCode::SUCCESS;
}

StatusCode TrigT1RPC::fill_RPCdata(RPCsimuData& data)
{
    std::string space = "                          ";

    StatusCode sc;
    static  MsgStream mylog(msgSvc(), name());
    mylog << MSG::DEBUG << "in execute(): fill RPC data" << endreq;

    typedef RpcDigitContainer::const_iterator collection_iterator;
    typedef RpcDigitCollection::const_iterator digit_iterator;

    string key = "RPC_DIGITS";
    const DataHandle <RpcDigitContainer> container;
    sc = (*m_activeStore)->retrieve(container,key);

    if (sc.isFailure()) 
    {
        mylog << MSG::ERROR << " Cannot retrieve RPC Container " << endreq;
        return StatusCode::FAILURE;
    }

    collection_iterator it1_coll= container->begin(); 
    collection_iterator it2_coll= container->end(); 

    for (  ; it1_coll!=it2_coll; ++it1_coll) 
    {
        const  RpcDigitCollection* rpcCollection = *it1_coll; 
 
        Identifier moduleId = rpcCollection->identify();

//        if (m_rpcId->validElement(moduleId) && 
//	    m_digit_position->initialize(moduleId))

	if (m_rpcId->is_rpc(moduleId))
        {
            digit_iterator it1_digit = rpcCollection->begin();
            digit_iterator it2_digit = rpcCollection->end();

            //const int stationCode = m_rpcId->stationName(moduleId);
	    //std::string StationName = m_rpcId->stationNameString(stationCode);
            //int StationEta  = m_rpcId->stationEta(moduleId);
            //int StationPhi  = m_rpcId->stationPhi(moduleId);

            for ( ; it1_digit!=it2_digit; ++it1_digit) 
            {
                const RpcDigit* rpcDigit = *it1_digit;
		//Identifier channelId = rpcDigit->identify();
		
                if (rpcDigit->is_valid(m_rpcId)) 
                {
		    Identifier channelId = rpcDigit->identify();
		    const int stationType   = m_rpcId->stationName(channelId);
		    std::string StationName = m_rpcId->stationNameString(stationType);
		    int StationEta          = m_rpcId->stationEta(channelId);
		    int StationPhi          = m_rpcId->stationPhi(channelId); 
                    int DoubletR            = m_rpcId->doubletR(channelId);
                    int DoubletZ            = m_rpcId->doubletZ(channelId);
                    int DoubletP            = m_rpcId->doubletPhi(channelId);
                    int GasGap              = m_rpcId->gasGap(channelId);
                    int MeasuresPhi         = m_rpcId->measuresPhi(channelId);
                    int Strip               = m_rpcId->strip(channelId);
                    
		    if(m_ntuple) {
		        //fill ntuple variables
		        m_rpc_stationType = stationType;
                        m_rpc_stationEta  = StationEta;
                        m_rpc_stationPhi  = StationPhi;
                        m_rpc_doubletR    = DoubletR;
                        m_rpc_doubletZ    = DoubletZ;
                        m_rpc_doubletP    = DoubletP;
                        m_rpc_gasGap      = GasGap;
                        m_rpc_measurePhi  = MeasuresPhi;
                        m_rpc_strip       = Strip;
		    }
		    
                    // Get the Level-1 numbering schema for the RPC strips
                    unsigned long int strip_code =
		        m_geometry->give_strip_code(StationName,StationEta,
                                                    StationPhi,DoubletR,
                                                    DoubletZ,DoubletP,GasGap,
                                                    MeasuresPhi,Strip);

                    const MuonGM::RpcReadoutElement* descriptor =
                                    m_MuonMgr->getRpcReadoutElement(channelId);

                    if(strip_code)
		    {
                        //Get the global position of RPC strip from RPCGeometry
                        float coo[3] = {0.,0.,0.};
                        m_geometry->give_strip_coordinates(strip_code,coo);

                        //Get the global position of RPC strip from MuonDetDesc
                        Amg::Vector3D pos = descriptor->stripPos(channelId);

                        // Fill data for the Level-1 RPC digit
                        float xyz[4];
                        
			// xyz[0] = rpcDigit->time();  //time of flight
                        //xyz[0] = 0.;  // no time of flight for the time being
                        
			if(m_ntuple) {
                            //fill ntuple variables
			    m_rpc_code = strip_code;
			    
			    m_rpc_geomodel_x = pos.x();
                            m_rpc_geomodel_y = pos.y();
                            m_rpc_geomodel_z = pos.z();
  
			    m_rpc_standalone_x = coo[0];
		            m_rpc_standalone_y = coo[1];
			    m_rpc_standalone_z = coo[2];
			
			    m_rpc_time = rpcDigit->time();
			}
			
                        //patch for the cavern background
			double tp = time_correction(pos.x(),pos.y(),pos.z());
                        tp = 0.; // remove the subtraction of the TOF
			xyz[0] = (m_patch_for_rpc_time)? rpcDigit->time() - tp:
			                                 0.;
			
			++digit_num;
			if(xyz[0]<0. || xyz[0]>25.) ++digit_out;

/*			
			if( fabs(coo[0] - pos.x()) >0.011 ||
                           fabs(coo[1] - pos.y()) >0.011 ||
                           fabs(coo[2] - pos.z()) >0.011    )
		        {
			    mylog << MSG::WARNING << "Global Position from "
			        << "RPCGeometry doesn't match the one from GM!"
			        << endreq;
				
		            mylog << MSG::WARNING << "RPC Digit from GM:" << endl
                                << space << "GlobalPosition (mm) = " 
                                << setiosflags(ios::fixed) << setprecision(3)
                                << setw(11)<< pos.x() 
                                << setiosflags(ios::fixed) << setprecision(3)
                                << setw(11) << pos.y() 
                                << setiosflags(ios::fixed) << setprecision(3)
                                << setw(11) << pos.z() << endl 
				<< endreq;
			    
		            mylog << MSG::WARNING << "RPC Digit from RPCgeometry:"
                                << endl
                                << space << "GlobalPosition (cm) = " 
                                << setiosflags(ios::fixed) << setprecision(3)
                                << setw(11) << coo[0] 
                                << setiosflags(ios::fixed) << setprecision(3)
                                << setw(11) << coo[1] 
                                << setiosflags(ios::fixed) << setprecision(3)
                                << setw(11) << coo[2] << endl << endreq;
			    
			    mylog << MSG::WARNING << "Using the GM position!" 
                                << endreq;
				
                            xyz[1] = globalPosition.x();
                            xyz[2] = globalPosition.y();
                            xyz[3] = globalPosition.z();
		        } else {
                        */
			xyz[1] = coo[0];
                        xyz[2] = coo[1];
                        xyz[3] = coo[2];
			
			//}

                        int param[3] = {0,0,0};

                        RPCsimuDigit
			digit(0,strip_code,param,xyz,m_geometry->rpcGeometry());
			    
                        data << digit;


                        // Start of debugging printout


                        //char buffer[100];
                        //for(int i=0;i<100;++i) buffer[i] = '\0';
                        //ostrstream disp_digit (buffer,100);
                        //disp_digit << *rpcDigit;
						
                        mylog << MSG::DEBUG << "Muon Identifiers from GM:" <<endl
                            << space << "StationName = " << StationName << endl
			    << space << "StationEta  = " << StationEta << endl 
                            << space << "StationPhi  = " << StationPhi << endl
                            << space << "DoubletR    = " << DoubletR << endl
			    << space << "DoubletZ    = " << DoubletZ << endl
			    << space << "DoubletP    = " << DoubletP << endl
			    << space << "GasGap      = " << GasGap << endl
			    << space << "MeasuresPhi = " << MeasuresPhi << endl
			    << space << "Strip       = " << Strip << endreq;
		        mylog << MSG::DEBUG << "RPC Digit from GM:" << endl
			      << space << hex << channelId << dec << endl
                              //<< space << disp_digit.str() << endl
			      //<< space << *rpcDigit << endl
                              << space << "GlobalPosition (cm) = " 
                              << setiosflags(ios::fixed) << setprecision(3)
                              << setw(11)<< pos.x() 
                              << setiosflags(ios::fixed) << setprecision(3)
                              << setw(11) << pos.y() 
                              << setiosflags(ios::fixed) << setprecision(3)
                              << setw(11) << pos.z() << endl <<endreq;
		        mylog << MSG::DEBUG << "RPC Digit from RPCgeometry:" 
                            << endl
                            << space << "strip_code = " << strip_code << endl 
                            << space << "GlobalPosition (cm) = " 
                            << setiosflags(ios::fixed) << setprecision(3)
                            << setw(11) << coo[0] 
                            << setiosflags(ios::fixed) << setprecision(3)
                            << setw(11) << coo[1] 
                            << setiosflags(ios::fixed) << setprecision(3)
                            << setw(11) << coo[2] << endl << endreq;
			    
			    
                        if(m_ntuple) { 
                          StatusCode sc =
			  ntupleSvc()->writeRecord("/NTUPLES/FILE1/TRIGT1RPC/40");
                          if(sc.isFailure()) {
                            mylog << MSG::WARNING 
			          << "Could not write tree for TrigT1RPC ntuple" 
	                          << endreq;
                          }
			}
                    }
                }
            }
            string id = m_rpcId->show_to_string(moduleId);
        }
    }

    return StatusCode::SUCCESS;
}

double time_correction(double x, double y, double z)
{
    double speed_of_light = 299.792458;     // mm/ns
    
    /*
    // detector dimensions
    //double EcalMaxR = 2250.;
    //double EcalMaxZ = 3500.;
    double HcalMaxR = 4230.;
    double HcalMaxZ = 6650.;
    double IdMaxR   = 1150.;
    double IdMaxZ   = 3450.;

    // move to the r-z plane
    double rad = std::sqrt(x*x+y*y);

    if (z == 0.)
    {
        return (rad-(HcalMaxR-IdMaxR))/speed_of_light + 
	                1.66*(HcalMaxR-IdMaxR)/speed_of_light; 
    }
    
    double m = rad/fabsf(z);
    
    //calo entrance point
    double Zin = 0.;
    double Rin = 0.;
    
    Zin = IdMaxR / m;
    if(Zin > IdMaxZ) {
        Zin = IdMaxZ;
	Rin = IdMaxZ * m;
    } 
    else {
        Rin = IdMaxR;
    }
    
    //calo exit point
    double Zout = 0.;
    double Rout = 0.;
    
    Zout = HcalMaxR / m;
    if(Zout > HcalMaxZ) {
        Zout = HcalMaxZ;
	Rout = HcalMaxZ * m;
    } 
    else {
        Rout = HcalMaxR;
    }
    
    double d      = std::sqrt( x*x + y*y + z*z);
    double calo_d = std::sqrt( (Zout-Zin)*(Zout-Zin) + (Rout-Rin)*(Rout-Rin) );

    
    return (d-calo_d)/speed_of_light + 1.33*calo_d/speed_of_light;
    */
    return sqrt(x*x+y*y+z*z)/speed_of_light;
}

#endif
