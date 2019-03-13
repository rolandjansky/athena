/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1RPClogic/ShowData.h"

#include "StoreGate/StoreGateSvc.h"

#include "MuonIdHelpers/RpcIdHelper.h"

#include "MuonDigitContainer/RpcDigitCollection.h"
#include "MuonDigitContainer/RpcDigit.h"

#include "EventInfoMgt/ITagInfoMgr.h"

//#include "RPCcablingInterface/RpcPadIdHash.h"

#include "MuonReadoutGeometry/RpcReadoutElement.h"

#include "MuonByteStreamCnvTest/RpcDigitToRpcRDO.h"

#include <algorithm>
#include <cmath>

using namespace std;

static double time_correction(double, double, double);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

RpcDigitToRpcRDO::RpcDigitToRpcRDO(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), /*m_simu_code(0),*/
  m_MuonMgr(0), m_cabling(0),
  m_rpcHelper(0)
{
  declareProperty ( "FastDebug", m_fast_debug=0 );

  declareProperty ( "Monitoring", m_monitoring=0 );

  declareProperty ( "Geometric", m_geometric_algo=false );
  declareProperty ( "GeometricParameters",m_geometric_algo_param=false );
  declareProperty ( "Detailed", m_detailed_algo=false );
  declareProperty ( "DetailedParameters", m_detail_algo_param=false );
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
  
  declareProperty ( "PatchForRpcTime", m_patch_for_rpc_time=false);
  
  m_cablingType = "UNKNOWN";
}


RpcDigitToRpcRDO::~RpcDigitToRpcRDO()
{
  //delete m_log; m_log=0;
  //delete m_padContainer; m_padContainer=0;
} 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode RpcDigitToRpcRDO::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );

  ATH_CHECK( detStore()->retrieve(m_rpcHelper, "RPCIDHELPER") );
  ATH_CHECK( detStore()->retrieve(m_MuonMgr) );

  ServiceHandle<IRPCcablingServerSvc> RpcCabGet ("RPCcablingServerSvc", name());
  ATH_CHECK( RpcCabGet.retrieve() );
  ATH_CHECK( RpcCabGet->giveCabling(m_cabling) );
    
    //determine teh type of cabling type 
    if (m_cabling->rpcCabSvcType() == "simLike_MapsFromFiles" || m_cabling->rpcCabSvcType() == "dataLike") m_cablingType="MuonRPC_Cabling";
    else if (m_cabling->rpcCabSvcType() == "simulationLike") m_cablingType="RPCcablingSim";
    else if (m_cabling->rpcCabSvcType() == "simulationLikeInitialization" ) m_cablingType="RPCcabling";
    else ATH_MSG_WARNING( "Unknown cabling type: rpcCabSvcType()="<< m_cabling->rpcCabSvcType() );

  // Fill Tag Info  
  if (fillTagInfo() != StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Unable to fill Tag Info "  );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Tag info filled successfully"  );
  }


    // // initialize RPC geometry service
    // sc = serviceLocator()->service("RPCgeometrySvc", m_geometry);
    // if (sc != StatusCode::SUCCESS ) 
    // {
    //     *m_log << MSG::ERROR << " Cannot get RPC geometry Service " << endmsg;
    //     return sc ;
    // }

  ATH_CHECK( m_padContainerKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_padContainerKey );
  ATH_CHECK( m_digitContainerKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_digitContainerKey );

  return StatusCode::SUCCESS;
}
 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode RpcDigitToRpcRDO::execute() {

  ATH_MSG_DEBUG( "in execute()"  );

  // create an empty pad container and record it
  SG::WriteHandle<RpcPadContainer> padContainer (m_padContainerKey);
  ATH_CHECK(padContainer.record((std::make_unique<RpcPadContainer>(600))));
  ATH_MSG_DEBUG("Recorded RpcPadContainer called " << padContainer.name() << " in store " << padContainer.store());

  RPCsimuData data;         // instantiate the container for the RPC digits

  if (fill_RPCdata(data).isFailure()) // fill the data with RPC simulated digits
    ATH_MSG_ERROR( "Fail to produce RPC data for byte stream simulation "  );

  ATH_MSG_DEBUG( "RPC data loaded from G3:" << endl
                 << ShowData<RPCsimuData>(data,"",m_data_detail)  );


  // ******************** Start of Level-1 simulation section *****************

  // Setting the algorithm type
  //  AlgoType type;
  //  if(m_detailed_algo)       type = DetailedAlgo;
  //  else if(m_geometric_algo) type = GeometricAlgo;

  unsigned long int debug;

  ///// Creates the CMA patterns from RPC digits /////////////////////////
  debug = (m_detailed_algo)? m_cma_debug : m_fast_debug;                //
                                                                        //
  //  CMAdata patterns(&data,m_cabling,type,debug);                     //
  CMAdata patterns(&data,m_cabling,debug);                              //
                                                                        //
  ATH_MSG_DEBUG( "CMApatterns created from RPC digits:" << endl   //
                 << ShowData<CMAdata>(patterns,"",m_data_detail)  );
  ////////////////////////////////////////////////////////////////////////


  // ******************* Start Byte Stream production *************************


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

  // ********************** create the RPC RDO's  *****************************

  RpcByteStreamDecoder* padDecoder = new RpcByteStreamDecoder(&bytestream, 
							      m_cabling,
							      m_rpcHelper,
                                                              &msg());
  if (padDecoder->decodeByteStream().isFailure()) 
    ATH_MSG_ERROR( "Fail to decode RPC byte stream"  );
 
  std::vector<RpcPad*>* pads = padDecoder->getPads();

  ATH_MSG_DEBUG( "Total number of pads in this event is " << pads->size()  );

  //RpcPadIdHash hashF = *(m_cabling->padHashFunction());

  std::vector<RpcPad*>::const_iterator it     = pads->begin();
  std::vector<RpcPad*>::const_iterator it_end = pads->end();
  for (; it!=it_end; ++it) {
    const RpcPad* pad = (*it); 
    //Identifier padId = pad->identify();
    //int elementHash = hashF( padId );
    int elementHash1 = pad->identifyHash();
    // if (elementHash1!=elementHash) 
//     {
// 	ATH_MSG_ERROR(" HashId not known to the Pad "<<	elementHash1<<" should be "<<elementHash );
//     }
//     else ATH_MSG_DEBUG(" HashId IS known to the Pad "<<elementHash1<<" = to the computed hashId "<<elementHash );
    
    
    if (padContainer->addCollection(pad, elementHash1).isFailure())
      ATH_MSG_ERROR( "Unable to record RPC Pad in IDC"   );
  }

  delete padDecoder;

  return StatusCode::SUCCESS;
}

StatusCode RpcDigitToRpcRDO::fill_RPCdata(RPCsimuData& data)  {

    std::string space = "                          ";

    ATH_MSG_DEBUG( "in execute(): fill RPC data"  );

    IdContext rpcContext = m_rpcHelper->module_context();

    typedef RpcDigitContainer::const_iterator collection_iterator;
    typedef RpcDigitCollection::const_iterator digit_iterator;

  SG::ReadHandle<RpcDigitContainer> container (m_digitContainerKey);
  if (!container.isValid()) {
    ATH_MSG_ERROR("Could not find RpcDigitContainer called " << container.name() << " in store " << container.store());
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Found RpcDigitContainer called " << container.name() << " in store " << container.store());

    collection_iterator it1_coll= container->begin(); 
    collection_iterator it2_coll= container->end(); 

    for (  ; it1_coll!=it2_coll; ++it1_coll) 
    {
        const  RpcDigitCollection* rpcCollection = *it1_coll; 

        ATH_MSG_DEBUG( "RPC Digit -> Pad loop :: digitCollection at " << rpcCollection  );

        IdentifierHash moduleHash = rpcCollection->identifierHash();
        Identifier moduleId;
        
        //if (m_digit_position->initialize(moduleId))
	if (!m_rpcHelper->get_id(moduleHash, moduleId, &rpcContext))
        {
            digit_iterator it1_digit = rpcCollection->begin();
            digit_iterator it2_digit = rpcCollection->end();
            for ( ; it1_digit!=it2_digit; ++it1_digit) 
            {
                const RpcDigit* rpcDigit = *it1_digit;
                if (rpcDigit->is_valid(m_rpcHelper)) 
                {
                    Identifier channelId = rpcDigit->identify();
		    int stationType         = m_rpcHelper->stationName(channelId);
		    std::string StationName = m_rpcHelper->stationNameString(stationType);
		    int StationEta          = m_rpcHelper->stationEta(channelId);
		    int StationPhi          = m_rpcHelper->stationPhi(channelId);
                    int DoubletR            = m_rpcHelper->doubletR(channelId);
                    int DoubletZ            = m_rpcHelper->doubletZ(channelId);
                    int DoubletP            = m_rpcHelper->doubletPhi(channelId);
                    int GasGap              = m_rpcHelper->gasGap(channelId);
                    int MeasuresPhi         = m_rpcHelper->measuresPhi(channelId);
                    int Strip               = m_rpcHelper->strip(channelId);

                    ATH_MSG_DEBUG( "RPC Digit Type, Eta, Phi, dbR, dbZ, dbP, gg, mPhi, Strip "<<stationType<<" "<<StationEta<<" "<<StationPhi<<" "<<DoubletR<<" "<<DoubletZ<<" "<<DoubletP<<" "<<GasGap<<" "<<MeasuresPhi<<" "<<Strip );
		    const MuonGM::RpcReadoutElement* descriptor =
                                    m_MuonMgr->getRpcReadoutElement(channelId);
		    

		    //Get the global position of RPC strip from MuonDetDesc
		    Amg::Vector3D pos = descriptor->stripPos(channelId);
		      
		    // get now strip_code from cablingSvc 
		    unsigned long int strip_code_cab = m_cabling->strip_code_fromOffId (StationName, StationEta, StationPhi, 
											DoubletR, DoubletZ, DoubletP, 
											GasGap, MeasuresPhi, Strip);

                     ATH_MSG_DEBUG( "From RPC Cabling Layout, strip_code = "<<strip_code_cab );

		    
		    if (strip_code_cab) {
		      // Fill data for the Level-1 RPC digit
		      float xyz[4];
		      
		      double tp = time_correction(pos.x(),pos.y(),pos.z());
		                                  //time of flight
		      xyz[0] = (m_patch_for_rpc_time)? rpcDigit->time() - tp:
			                               rpcDigit->time();
		      xyz[1] = pos.x()/10.;//coo[0];            //RPC strip x coordinate
		      xyz[2] = pos.y()/10.;//coo[1];            //RPC strip y coordinate
		      xyz[3] = pos.z()/10.;//coo[2];            //RPC strip z coordinate

		      int param[3] = {0,0,0};

		      //std::cout<<" digit with strip_code (new) = "<<strip_code_new<<" is filling TrigT1SimuData"<<std::endl;
		      ATH_MSG_DEBUG( "Digit with strip_code = "<<strip_code_cab<<" passed to RDO/LVL1 Simulation (RPCsimuDigit)" );
		      RPCsimuDigit digit(0,strip_code_cab,param,xyz);
		      data << digit;

		    }
		}
	    }
	    //	    string id = moduleId;
	}
    }

    return StatusCode::SUCCESS;
}

double time_correction(double x, double y, double z)
{
    double speed_of_light = 299.792458;     // mm/ns
    return sqrt(x*x+y*y+z*z)/speed_of_light;
}


StatusCode RpcDigitToRpcRDO::fillTagInfo() const
{
  ServiceHandle<ITagInfoMgr> tagInfoMgr ("TagInfoMgr", name());
  if (tagInfoMgr.retrieve().isFailure())
    return StatusCode::FAILURE;
  
  StatusCode sc = tagInfoMgr->addTag("RPC_CablingType",m_cablingType);  

  if(sc.isFailure()) {
    ATH_MSG_WARNING( "RPC_CablingType " << m_cablingType
                     << " not added to TagInfo "  );
    return sc;
  } else {
    ATH_MSG_DEBUG( "RPC_CablingType " << m_cablingType 
                   << " is Added TagInfo "  );
  }
  
  return StatusCode::SUCCESS;
}
