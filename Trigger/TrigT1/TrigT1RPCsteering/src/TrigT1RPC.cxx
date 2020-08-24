/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPC.h"

#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenEvent.h"

#include "MuonReadoutGeometry/RpcReadoutElement.h"

#include "TrigT1RPChardware/SectorLogic.h"
#include "TrigT1RPChardware/MatrixReadOut.h"

#include "TrigT1RPClogic/decodeSL.h"
#include "TrigT1RPClogic/ShowData.h"

#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"
#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"

#include <algorithm>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

TrigT1RPC::TrigT1RPC(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_cabling_getter("RPCcablingServerSvc/RPCcablingServerSvc","TrigT1RPC"),
  m_cabling(nullptr) {
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigT1RPC::initialize(){
    ATH_MSG_INFO("Initializing");
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_cabling_getter.retrieve());
    ATH_CHECK(m_cabling_getter->giveCabling(m_cabling));
    ATH_CHECK(m_readKey.initialize());
    ATH_CHECK(m_rpcDigitKey.initialize());
    ATH_CHECK(m_muctpiPhase1Key.initialize(m_useRun3Config));
    ATH_CHECK(m_muctpiKey.initialize(!m_useRun3Config));
    ATH_CHECK(m_muDetMgrKey.initialize());
    return StatusCode::SUCCESS;
}
 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode TrigT1RPC::execute() {

    ATH_MSG_DEBUG ("in execute()");

    SG::ReadCondHandle<RpcCablingCondData> readHandle{m_readKey, Gaudi::Hive::currentContext()};
    const RpcCablingCondData* readCdo{*readHandle};
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
    const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();
    
    RPCsimuData data;         // instanciate the container for the RPC digits
    CHECK(fill_RPCdata(data, readCdo, muDetMgr));  // fill the data with RPC simulated digts
    
    ATH_MSG_DEBUG(
        "RPC data loaded from G3:" << std::endl
        << ShowData<RPCsimuData>(data,"",m_data_detail) << std::endl
        << "RPC digits into station 1 ==> " 
        << data.how_many(-1,-1,1,-1,-1,-1) << std::endl
        << "RPC digits into station 2 ==> " 
        << data.how_many(-1,-1,2,-1,-1,-1) << std::endl
        << "RPC digits into station 3 ==> " 
        << data.how_many(-1,-1,3,-1,-1,-1)
        );
    // ******************** Start of Level-1 simulation section *****************
  unsigned long int debug;
    
    ///// Creates the CMA patterns from RPC digits /////////////////////////
  debug = (m_hardware_emulation)? m_cma_debug : m_fast_debug;           //
  CMAdata patterns(&data, m_cabling, debug);                              //
                                                                        //
  ATH_MSG_DEBUG ( "CMApatterns created from RPC digits:" << std::endl //
                  << ShowData<CMAdata>(patterns,"",m_data_detail) );      //
  ////////////////////////////////////////////////////////////////////////


  ///// Creates the PAD patterns from the CMA patterns ///////////////////
  debug = (m_hardware_emulation)? m_pad_debug : m_fast_debug;           //
  PADdata pads(&patterns,debug);                                        //
                                                                        //
  ATH_MSG_DEBUG ( "PADs created from CMA patterns:" << std::endl      
                  << ShowData<PADdata>(pads,"",m_data_detail) );
  ////////////////////////////////////////////////////////////////////////


  ///// Creates the SL patterns from the PAD patterns ////////////////////
  debug = (m_hardware_emulation)? m_sl_debug : m_fast_debug;            //
  SLdata sectors(&pads,debug);                                          //
                                                                        //
  ATH_MSG_DEBUG("Sector Logics created from PAD patterns:"     //
                << std::endl                                                         //
                << ShowData<SLdata>(sectors,"",m_data_detail) );        //  
  ////////////////////////////////////////////////////////////////////////

  ///// Access the SL word and fill the MuCTPInterface ///////////////////
  LVL1MUONIF::Lvl1MuCTPIInput * ctpiInRPC = nullptr;
  LVL1MUONIF::Lvl1MuCTPIInputPhase1 * ctpiPhase1InRPC = nullptr;
  if(m_useRun3Config){
    SG::WriteHandle<LVL1MUONIF::Lvl1MuCTPIInputPhase1> wh_muctpiRpc(m_muctpiPhase1Key);
    ATH_CHECK(wh_muctpiRpc.record(std::make_unique<LVL1MUONIF::Lvl1MuCTPIInputPhase1>()));
    ctpiPhase1InRPC = wh_muctpiRpc.ptr();
  }else{
    SG::WriteHandle<LVL1MUONIF::Lvl1MuCTPIInput> wh_muctpiRpc(m_muctpiKey);
    ATH_CHECK(wh_muctpiRpc.record(std::make_unique<LVL1MUONIF::Lvl1MuCTPIInput>()));
    ctpiInRPC = wh_muctpiRpc.ptr();
  }

                                                                        //
  SLdata::PatternsList sectors_patterns = sectors.give_patterns();      //
  SLdata::PatternsList::iterator SLit = sectors_patterns.begin();       //
                                                                        //
  while(SLit != sectors_patterns.end())                                 //
  {                                                                     //
      SectorLogic* logic = (*SLit)->give_SectorL(readCdo);                     //
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
	  if(m_useRun3Config){
	    LVL1MUONIF::Lvl1MuBarrelSectorLogicData sldata;
	    sldata.convertFromWordFormat(data_word);
	    LVL1MUONIF::Lvl1MuBarrelSectorLogicDataPhase1 sldata2;
	    sldata2.bcid( sldata.bcid() );
	    if(sldata.is2candidatesInSector()) sldata2.set2candidatesInSector();
	    else sldata2.clear2candidatesInSector();
	    for(int icand = 0 ; icand < 2 ; icand++){ // up to 2 candidates
	      sldata2.pt(icand, sldata.pt(icand) );
	      sldata2.roi(icand, sldata.roi(icand) );
	      sldata2.ovl(icand, sldata.ovl(icand) );
	      sldata2.charge(icand, sldata.charge(icand) );
	      if(sldata.is2candidates(icand)) sldata2.set2candidates(icand);
	      else sldata2.clear2candidates(icand);
	    }
	    ctpiPhase1InRPC->setSectorLogicData(sldata2,0,subsystem,logic_sector,dbc);
	  }else{
	    ctpiInRPC->setSectorLogicData(data_word,0,subsystem,logic_sector,dbc);
	  }
      }
                                                                        //
      ++SLit;                                                           //
  }          

                                                           //
                                                                        //
  
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
                         << " bytestream structure" << std::endl 
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

StatusCode TrigT1RPC::fill_RPCdata(RPCsimuData& data, const RpcCablingCondData* readCdo, const MuonGM::MuonDetectorManager* muDetMgr)
{
    std::string space = "                          ";

    ATH_MSG_DEBUG("in execute(): fill RPC data");

    typedef RpcDigitContainer::const_iterator collection_iterator;
    typedef RpcDigitCollection::const_iterator digit_iterator;

    SG::ReadHandle<RpcDigitContainer> rh_rpcDigits(m_rpcDigitKey);
    if(!rh_rpcDigits.isValid()){
      ATH_MSG_WARNING("No RPC digits container found");
      return StatusCode::SUCCESS;
    }
    const RpcDigitContainer* container = rh_rpcDigits.cptr();


    collection_iterator it1_coll= container->begin(); 
    collection_iterator it2_coll= container->end(); 

    for (  ; it1_coll!=it2_coll; ++it1_coll) 
    {
        const  RpcDigitCollection* rpcCollection = *it1_coll; 
 
        Identifier moduleId = rpcCollection->identify();

	if (m_idHelperSvc->isRpc(moduleId))
        {
            digit_iterator it1_digit = rpcCollection->begin();
            digit_iterator it2_digit = rpcCollection->end();

            for ( ; it1_digit!=it2_digit; ++it1_digit) 
            {
                const RpcDigit* rpcDigit = *it1_digit;
		
                if (rpcDigit->is_valid(&m_idHelperSvc->rpcIdHelper())) 
                {
		    Identifier channelId = rpcDigit->identify();
		    const int stationType   = m_idHelperSvc->rpcIdHelper().stationName(channelId);
		    std::string StationName = m_idHelperSvc->rpcIdHelper().stationNameString(stationType);
		    int StationEta          = m_idHelperSvc->rpcIdHelper().stationEta(channelId);
		    int StationPhi          = m_idHelperSvc->rpcIdHelper().stationPhi(channelId); 
                    int DoubletR            = m_idHelperSvc->rpcIdHelper().doubletR(channelId);
                    int DoubletZ            = m_idHelperSvc->rpcIdHelper().doubletZ(channelId);
                    int DoubletP            = m_idHelperSvc->rpcIdHelper().doubletPhi(channelId);
                    int GasGap              = m_idHelperSvc->rpcIdHelper().gasGap(channelId);
                    int MeasuresPhi         = m_idHelperSvc->rpcIdHelper().measuresPhi(channelId);
                    int Strip               = m_idHelperSvc->rpcIdHelper().strip(channelId);
                    
                    const MuonGM::RpcReadoutElement* descriptor =
                                    muDetMgr->getRpcReadoutElement(channelId);

		    //Get the global position of RPC strip from MuonDetDesc
		    Amg::Vector3D pos = descriptor->stripPos(channelId);		    

                    // Get the Level-1 numbering schema for the RPC strips
		    unsigned long int strip_code_cab = readCdo->strip_code_fromOffId(StationName,StationEta,StationPhi,
						       DoubletR,DoubletZ,DoubletP,GasGap,MeasuresPhi,Strip);
		    

                    if(strip_code_cab) {
		      // Fill data for the Level-1 RPC digit
                        float xyz[4];
			xyz[1] = pos.x()/10.;//coo[0];            //RPC strip x coordinate 
			xyz[2] = pos.y()/10.;//coo[1];            //RPC strip y coordinate 
			xyz[3] = pos.z()/10.;//coo[2];            //RPC strip z coordinate 
                        xyz[0] = rpcDigit->time();  //time of digits
                        
			
                        int param[3] = {0,0,0};

                        RPCsimuDigit digit(0,strip_code_cab,param,xyz);
			    
                        data << digit;
						
                        ATH_MSG_DEBUG ( "Muon Identifiers from GM:" <<std::endl
                                        << space << "StationName = " << StationName << std::endl
                                        << space << "StationEta  = " << StationEta << std::endl 
                                        << space << "StationPhi  = " << StationPhi << std::endl
                                        << space << "DoubletR    = " << DoubletR << std::endl
                                        << space << "DoubletZ    = " << DoubletZ << std::endl
                                        << space << "DoubletP    = " << DoubletP << std::endl
                                        << space << "GasGap      = " << GasGap << std::endl
                                        << space << "MeasuresPhi = " << MeasuresPhi << std::endl
                                        << space << "Strip       = " << Strip );
                        
		        ATH_MSG_DEBUG ("RPC Digit from GM:" << std::endl
                                       << space << std::hex << channelId << std::dec << std::endl
                                       << space << "GlobalPosition (cm) = " 
                                       << setiosflags(std::ios::fixed) << std::setprecision(3)
                                       << std::setw(11)<< pos.x() 
                                       << setiosflags(std::ios::fixed) << std::setprecision(3)
                                       << std::setw(11) << pos.y() 
                                       << setiosflags(std::ios::fixed) << std::setprecision(3)
                                       << std::setw(11) << pos.z() );
                        
                    }
                }
            }
            std::string id = m_idHelperSvc->rpcIdHelper().show_to_string(moduleId);
        }
    }

    return StatusCode::SUCCESS;
}

