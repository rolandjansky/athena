/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPCsteering/TrigT1RPC.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "GeneratorObjects/McEventCollection.h"  // include for retrieving the Montecarlo
#include "HepMC/GenEvent.h"                      // thru

#include "TrigT1RPClogic/ShowData.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"

#include "MuonIdHelpers/RpcIdHelper.h"


#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonDigitContainer/RpcDigitCollection.h"
#include "MuonDigitContainer/RpcDigit.h"

#include "MuonReadoutGeometry/RpcReadoutElement.h"

// next candidate for removal
//#include "TrigT1RPCmonitoring/DetailedTW.h" 

#include "TrigT1RPChardware/SectorLogic.h"
#include "TrigT1RPChardware/MatrixReadOut.h"

#include "TrigT1RPClogic/decodeSL.h"

#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"


#include <algorithm>
#include <cmath>


using namespace std;

//static double time_correction(double, double, double);

static int digit_num = 0;
static int digit_out = 0;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

TrigT1RPC::TrigT1RPC(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_cabling_getter("RPCcablingServerSvc/RPCcablingServerSvc","TrigT1RPC"),
  m_cabling(0)
{
  declareProperty ( "FastDebug", m_fast_debug=0 );
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

  declareProperty ( "firstBC_to_MUCTPI", m_firstBC_to_MUCTPI=-1 );
  declareProperty ( "lastBC_to_MUCTPI", m_lastBC_to_MUCTPI=1 );
  
  m_MuonMgr=0;
  m_rpcId=0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigT1RPC::initialize(){

    //  MsgStream mylog(msgSvc(), name());
    ATH_MSG_INFO("Initializing");
    
    CHECK (detStore()->retrieve( m_MuonMgr ));
    m_rpcId = m_MuonMgr->rpcIdHelper();
    

    

    // initialize RPC cabling service
    CHECK(m_cabling_getter.retrieve());
    CHECK(m_cabling_getter->giveCabling(m_cabling));
    
    
  return StatusCode::SUCCESS;
}
 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode TrigT1RPC::execute() {


    ATH_MSG_DEBUG ("in execute()");
    
    RPCsimuData data;         // instanciate the container for the RPC digits
    CHECK(fill_RPCdata(data));  // fill the data with RPC simulated digts
    
    ATH_MSG_DEBUG(
        "RPC data loaded from G3:" << endl
        << ShowData<RPCsimuData>(data,"",m_data_detail) << endl
        << "RPC digits into station 1 ==> " 
        << data.how_many(-1,-1,1,-1,-1,-1) << endl
        << "RPC digits into station 2 ==> " 
        << data.how_many(-1,-1,2,-1,-1,-1) << endl
        << "RPC digits into station 3 ==> " 
        << data.how_many(-1,-1,3,-1,-1,-1)
        );
    
    // exit(1);
    
    // ******************** Start of Level-1 simulation section *****************
    
    
   unsigned long int debug;
    
    ///// Creates the CMA patterns from RPC digits /////////////////////////
  debug = (m_hardware_emulation)? m_cma_debug : m_fast_debug;           //
    //m_cabling.operator->()                         //
  CMAdata patterns(&data,m_cabling,debug);                              //
                                                                        //
  ATH_MSG_DEBUG ( "CMApatterns created from RPC digits:" << endl //
                  << ShowData<CMAdata>(patterns,"",m_data_detail) );      //
  ////////////////////////////////////////////////////////////////////////


  ///// Creates the PAD patterns from the CMA patterns ///////////////////
  debug = (m_hardware_emulation)? m_pad_debug : m_fast_debug;           //
  PADdata pads(&patterns,m_cabling,debug);                                        //
                                                                        //
  ATH_MSG_DEBUG ( "PADs created from CMA patterns:" << endl      
                  << ShowData<PADdata>(pads,"",m_data_detail) );
  ////////////////////////////////////////////////////////////////////////


  ///// Creates the SL patterns from the PAD patterns ////////////////////
  debug = (m_hardware_emulation)? m_sl_debug : m_fast_debug;            //
  SLdata sectors(&pads,debug);                                          //
                                                                        //
  ATH_MSG_DEBUG("Sector Logics created from PAD patterns:"     //
                << endl                                                         //
                << ShowData<SLdata>(sectors,"",m_data_detail) );        //  
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
      int logic_sector  = sector%32;//

      for (int dbc=m_firstBC_to_MUCTPI; dbc<=m_lastBC_to_MUCTPI; dbc++){
          
          unsigned int data_word = logic->outputToMuCTPI(dbc);
          
          ATH_MSG_DEBUG(                                               //
              "Input to MuCTPI: side=" << subsystem                    //
              << ", SL= " << logic_sector                                 //
              << ", BC= " << dbc
              << ", RoI=" << TriggerRPC::ROI1(data_word)                  //
              << ", Th=" << TriggerRPC::PT1(data_word)                    //
              << ", data word " << MSG::hex << data_word                  //
              << MSG::dec );                                      //

          ctpiInRPC->setSectorLogicData(data_word,0,subsystem,logic_sector,dbc);
      }
                                                                        //
      ++SLit;                                                           //
  }                                                                     //
                                                                        //
  CHECK(evtStore()->record(ctpiInRPC,DEFAULT_L1MuctpiStoreLocationRPC));  //
  
  ATH_MSG_DEBUG ("put RPC Lvl1MuCTPIInput into SG" ); //
  
  ////////////////////////////////////////////////////////////////////////
  


  // ******************* Start of Level-1 computation section *****************

  
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
      
      ATH_MSG_DEBUG("Start retrieving data from the RPC bytestream class");
      
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
	  ATH_MSG_DEBUG ("Start dumping the PAD " << (*it).second.PAD()
                         << " bytestream structure" << endl 
                         << PADdata.str());
          
          //access to MatrixReadOut classes given in input to that PAD
          for (int i=0;i<8;++i)
	  {
	  
              ATH_MSG_DEBUG( "Start dumping the Matrix " << i 
                             << " into the PAD n. " << (*it).second.PAD());
              
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
		  
		  ATH_MSG_DEBUG (CMAdata.str());
	      }
	      else
	      {
	          ATH_MSG_DEBUG( "Matrix Read Out not loaded");
	      }
	  }

          ++it;
      }

  }

  ATH_MSG_DEBUG ( "TrigT1RPC terminated succesfully!" );

  return StatusCode::SUCCESS;
}

StatusCode TrigT1RPC::finalize() {
 
    
    ATH_MSG_DEBUG ( "in finalize()" << endreq
                    << "processed digits = " << digit_num << endreq
                    << "digits out of the time window = " 
                    << digit_out );
    
   return StatusCode::SUCCESS;
}

StatusCode TrigT1RPC::fill_RPCdata(RPCsimuData& data)
{
    std::string space = "                          ";

    ATH_MSG_DEBUG("in execute(): fill RPC data");

    typedef RpcDigitContainer::const_iterator collection_iterator;
    typedef RpcDigitCollection::const_iterator digit_iterator;

    string key = "RPC_DIGITS";
    const DataHandle <RpcDigitContainer> container;
    CHECK(evtStore()->retrieve(container,key));


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
                    
                    const MuonGM::RpcReadoutElement* descriptor =
                                    m_MuonMgr->getRpcReadoutElement(channelId);

		    //Get the global position of RPC strip from MuonDetDesc
		    Amg::Vector3D pos = descriptor->stripPos(channelId);		    

                    // Get the Level-1 numbering schema for the RPC strips
		    unsigned long int strip_code_cab = 
                        m_cabling->strip_code_fromOffId (StationName,StationEta,StationPhi,
						       DoubletR,DoubletZ,DoubletP,
						       GasGap,MeasuresPhi,Strip);
		    

                    if(strip_code_cab) {
		      // Fill data for the Level-1 RPC digit
                        float xyz[4];
			xyz[1] = pos.x()/10.;//coo[0];            //RPC strip x coordinate 
			xyz[2] = pos.y()/10.;//coo[1];            //RPC strip y coordinate 
			xyz[3] = pos.z()/10.;//coo[2];            //RPC strip z coordinate 
                        xyz[0] = rpcDigit->time();  //time of digits
                        
			
			++digit_num;
			if(xyz[0]<0. || xyz[0]>25.) ++digit_out;

                        int param[3] = {0,0,0};

                        RPCsimuDigit digit(0,strip_code_cab,param,xyz);
			    
                        data << digit;


                        // Start of debugging printout


                        //char buffer[100];
                        //for(int i=0;i<100;++i) buffer[i] = '\0';
                        //ostrstream disp_digit (buffer,100);
                        //disp_digit << *rpcDigit;
						
                        ATH_MSG_DEBUG ( "Muon Identifiers from GM:" <<endl
                                        << space << "StationName = " << StationName << endl
                                        << space << "StationEta  = " << StationEta << endl 
                                        << space << "StationPhi  = " << StationPhi << endl
                                        << space << "DoubletR    = " << DoubletR << endl
                                        << space << "DoubletZ    = " << DoubletZ << endl
                                        << space << "DoubletP    = " << DoubletP << endl
                                        << space << "GasGap      = " << GasGap << endl
                                        << space << "MeasuresPhi = " << MeasuresPhi << endl
                                        << space << "Strip       = " << Strip );
                        
		        ATH_MSG_DEBUG ("RPC Digit from GM:" << endl
                                       << space << hex << channelId << dec << endl
                                       << space << "GlobalPosition (cm) = " 
                                       << setiosflags(ios::fixed) << setprecision(3)
                                       << setw(11)<< pos.x() 
                                       << setiosflags(ios::fixed) << setprecision(3)
                                       << setw(11) << pos.y() 
                                       << setiosflags(ios::fixed) << setprecision(3)
                                       << setw(11) << pos.z() );
                        
                    }
                }
            }
            string id = m_rpcId->show_to_string(moduleId);
        }
    }

    return StatusCode::SUCCESS;
}

