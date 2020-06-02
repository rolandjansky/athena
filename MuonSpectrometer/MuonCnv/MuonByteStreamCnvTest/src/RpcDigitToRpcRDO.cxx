/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPClogic/ShowData.h"

#include "StoreGate/StoreGateSvc.h"

#include "MuonDigitContainer/RpcDigitCollection.h"
#include "MuonDigitContainer/RpcDigit.h"

#include "EventInfoMgt/ITagInfoMgr.h"

#include "MuonReadoutGeometry/RpcReadoutElement.h"

#include "MuonByteStreamCnvTest/RpcDigitToRpcRDO.h"

#include <algorithm>
#include <cmath>
#include "GaudiKernel/PhysicalConstants.h"

static double time_correction(double, double, double);

namespace {
  static constexpr unsigned int const& rpcRawHitWordLength = 7;
  static constexpr double const& inverseSpeedOfLight = 1 / Gaudi::Units::c_light; // need 1/299.792458
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

RpcDigitToRpcRDO::RpcDigitToRpcRDO(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator),
  m_MuonMgr(nullptr)
{
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode RpcDigitToRpcRDO::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );

  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(detStore()->retrieve(m_MuonMgr));

  ATH_CHECK(m_cabling.retrieve()) ;

  ATH_CHECK(m_readKey.initialize());

  // Fill Tag Info
  if (fillTagInfo() != StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Unable to fill Tag Info "  );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Tag info filled successfully"  );
  }

  ATH_CHECK( m_padContainerKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_padContainerKey );
  ATH_CHECK( m_digitContainerKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_digitContainerKey );

  return StatusCode::SUCCESS;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode RpcDigitToRpcRDO::execute(const EventContext& ctx) const {

  ATH_MSG_DEBUG( "in execute()"  );

  // create an empty pad container and record it
  SG::WriteHandle<RpcPadContainer> padContainer (m_padContainerKey, ctx);
  ATH_CHECK(padContainer.record((std::make_unique<RpcPadContainer>(600))));
  ATH_MSG_DEBUG("Recorded RpcPadContainer called " << padContainer.name() << " in store " << padContainer.store());

  RPCsimuData data;         // instantiate the container for the RPC digits

  SG::ReadCondHandle<RpcCablingCondData> readHandle{m_readKey, ctx};
  const RpcCablingCondData* readCdo{*readHandle};

  // fill the data with RPC simulated digits
  if (fill_RPCdata(data, ctx, readCdo).isFailure()) {
    ATH_MSG_ERROR( "Fail to produce RPC data for byte stream simulation "  );
  }
  ATH_MSG_DEBUG( "RPC data loaded from G3:" << std::endl
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
  CMAdata patterns(&data,&*m_cabling,debug);                                //
                                                                        //
  ATH_MSG_DEBUG( "CMApatterns created from RPC digits:" << std::endl    //
                 << ShowData<CMAdata>(patterns,"",m_data_detail)  );    //
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

  std::vector<RpcPad*> rpcpads;
  ATH_MSG_DEBUG( "Start decoding");
  PAD_Readout padReadout = bytestream.pad_readout();
  // Iterate on the readout PADS and decode them
  for (auto& padro : padReadout) {
    RpcPad* newpad = decodePad(padro.second, readCdo);
    // Push back the decoded pad in the vector
    rpcpads.push_back(newpad);
  }
  ATH_MSG_DEBUG("Total number of pads in this event is " << rpcpads.size());

  for (const auto& pad : rpcpads) {
    const int elementHash1 = pad->identifyHash();
    if (padContainer->addCollection(pad, elementHash1).isFailure()) {
      ATH_MSG_ERROR( "Unable to record RPC Pad in IDC"   );
    }
  }
  rpcpads.clear();
  return StatusCode::SUCCESS;
}

StatusCode RpcDigitToRpcRDO::fill_RPCdata(RPCsimuData& data, const EventContext& ctx, const RpcCablingCondData* readCdo) const {

  std::string space = "                          ";

  ATH_MSG_DEBUG( "in execute(): fill RPC data"  );

  IdContext rpcContext = m_idHelperSvc->rpcIdHelper().module_context();

  SG::ReadHandle<RpcDigitContainer> container (m_digitContainerKey, ctx);
  if (!container.isValid()) {
    ATH_MSG_ERROR("Could not find RpcDigitContainer called " << container.name() << " in store " << container.store());
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Found RpcDigitContainer called " << container.name() << " in store " << container.store());

  for (const RpcDigitCollection* rpcCollection : *container) {

    ATH_MSG_DEBUG( "RPC Digit -> Pad loop :: digitCollection at " << rpcCollection  );

    IdentifierHash moduleHash = rpcCollection->identifierHash();
    Identifier moduleId;
    if (!m_idHelperSvc->rpcIdHelper().get_id(moduleHash, moduleId, &rpcContext)) {
      for (const RpcDigit* rpcDigit : *rpcCollection) {
        if (rpcDigit->is_valid(m_idHelperSvc->rpcIdHelper())) {
          Identifier channelId = rpcDigit->identify();
          int stationType         = m_idHelperSvc->rpcIdHelper().stationName(channelId);
          std::string StationName = m_idHelperSvc->rpcIdHelper().stationNameString(stationType);
          int StationEta          = m_idHelperSvc->rpcIdHelper().stationEta(channelId);
          int StationPhi          = m_idHelperSvc->rpcIdHelper().stationPhi(channelId);
          int DoubletR            = m_idHelperSvc->rpcIdHelper().doubletR(channelId);
          int DoubletZ            = m_idHelperSvc->rpcIdHelper().doubletZ(channelId);
          int DoubletP            = m_idHelperSvc->rpcIdHelper().doubletPhi(channelId);
          int GasGap              = m_idHelperSvc->rpcIdHelper().gasGap(channelId);
          int MeasuresPhi         = m_idHelperSvc->rpcIdHelper().measuresPhi(channelId);
          int Strip               = m_idHelperSvc->rpcIdHelper().strip(channelId);

          ATH_MSG_DEBUG( "RPC Digit Type, Eta, Phi, dbR, dbZ, dbP, gg, mPhi, Strip "<<stationType<<" "<<StationEta<<" "<<StationPhi<<" "<<DoubletR<<" "<<DoubletZ<<" "<<DoubletP<<" "<<GasGap<<" "<<MeasuresPhi<<" "<<Strip );
          const MuonGM::RpcReadoutElement* descriptor =
            m_MuonMgr->getRpcReadoutElement(channelId);


          //Get the global position of RPC strip from MuonDetDesc
          Amg::Vector3D pos = descriptor->stripPos(channelId);

          // get now strip_code from cablingSvc
          unsigned long int strip_code_cab = readCdo->strip_code_fromOffId (StationName, StationEta, StationPhi,
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

            ATH_MSG_DEBUG( "Digit with strip_code = "<<strip_code_cab<<" passed to RDO/LVL1 Simulation (RPCsimuDigit)" );
            RPCsimuDigit digit(0,strip_code_cab,param,xyz);
            data << digit;

          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

double time_correction(double x, double y, double z)
{
  return std::sqrt(x*x+y*y+z*z)*inverseSpeedOfLight;
}

// NOTE: although this function has no clients in release 22, currently the Run2 trigger simulation is still run in
//       release 21 on RDOs produced in release 22. Since release 21 accesses the TagInfo, it needs to be written to the
//       RDOs produced in release 22. The fillTagInfo() function thus needs to stay in release 22 until the workflow changes
StatusCode RpcDigitToRpcRDO::fillTagInfo() const
{
  ServiceHandle<ITagInfoMgr> tagInfoMgr ("TagInfoMgr", name());
  ATH_CHECK(tagInfoMgr.retrieve());

  std::string cablingType="MuonRPC_Cabling";
  StatusCode sc = tagInfoMgr->addTag("RPC_CablingType",cablingType);
  if(sc.isFailure()) {
    ATH_MSG_WARNING( "RPC_CablingType " << cablingType << " not added to TagInfo "  );
    return sc;
  } else {
    ATH_MSG_DEBUG( "RPC_CablingType " << cablingType << " is Added TagInfo "  );
  }

  return StatusCode::SUCCESS;
}

// Decode a pad and return a pointer to a RpcPad RDO
RpcPad* RpcDigitToRpcRDO::decodePad(PADreadout& pad, const RpcCablingCondData* readCdo) const
{

  ATH_MSG_DEBUG( "Decoding a new RpcPad" );

  // Identifier elements
  int name = 0;
  int eta = 0;
  int phi = 0;
  int doublet_r = 0;
  int doublet_z = 0;
  int doublet_phi = 0;
  int gas_gap = 0;
  int measures_phi = 0;
  int strip = 0;

  PadReadOut* readout = pad.give_pad_readout();

  // Retrieve PAD sector and PAD ID
  int sector = pad.sector();
  int pad_id = pad.PAD();
  // Compute side and logic sector
  int side = (sector < 32) ? 0:1;
  int logic_sector = sector%32;
  // Compute the key to retrieve the offline id from the map (as from LVL1 sim.)
  int key = side*10000+logic_sector*100+pad_id;

  ATH_MSG_DEBUG( "Pad: Side " << side << " Sector logic" << logic_sector << " Id " << pad_id );

  // Retrieve the identifier elements from the map
  const RpcCablingCondData::RDOmap& pad_map = readCdo->give_RDOs();
  RDOindex index = (*pad_map.find(key)).second;

  index.offline_indexes(name, eta, phi, doublet_r, doublet_z, doublet_phi, gas_gap, measures_phi, strip);

  // Build the pad offline identifier
  bool check = true;
  bool valid = false;
  Identifier id = m_idHelperSvc->rpcIdHelper().padID(name, eta, phi, doublet_r, 
      doublet_z, doublet_phi, check, &valid);

  ATH_MSG_DEBUG( "Invalid pad offline indices " );
  ATH_MSG_DEBUG( "Name : "    << name  );
  ATH_MSG_DEBUG( "Eta "       << eta << "   Phi "   << phi );
  ATH_MSG_DEBUG( "Doublet r " << doublet_r << "  Doublet_z " << doublet_z << "  Doublet_phi " << doublet_phi );
  ATH_MSG_DEBUG( "Gas gap "   << gas_gap   << "   Measures_phi " << measures_phi << "  Strip " << strip );

  // Retrieve Pad status and error code from Pad header and footer
  PadReadOutStructure pad_header = readout->getHeader();
  PadReadOutStructure pad_footer = readout->getFooter();
  // Check the data format
  assert(pad_header.isHeader());
  assert(pad_footer.isFooter());

  unsigned int hashId = index.hash();
  unsigned int onlineId  = pad_id;
  unsigned int status = 0;
  unsigned int errorCode = pad_footer.errorCode();

  // Construct the new Pad
  RpcPad* rpc_pad = new RpcPad(id, hashId, onlineId, status, errorCode, sector);

  // Iterate on the matrices and decode them
  for (int i=0 ; i < readout->numberOfCMROFragments() ; ++i)
  {
    MatrixReadOut* matrix  = pad.matrices_readout(i);
    RpcCoinMatrix* coinMatrix = decodeMatrix(matrix, id);
    // Add the matrix to the pad
    rpc_pad->push_back(coinMatrix);
  }

  ATH_MSG_DEBUG( "Number of matrices in Pad : " << rpc_pad->size() );

  return rpc_pad;
}

// Function decoding a coincidence matrix
RpcCoinMatrix* RpcDigitToRpcRDO::decodeMatrix(MatrixReadOut* matrix, Identifier& id) const
{

  ATH_MSG_DEBUG( "Decoding a new RpcCoinMatrix" );

  // Matrix Header and SubHeader      
  MatrixReadOutStructure matrix_header    = matrix->getHeader();
  MatrixReadOutStructure matrix_subheader = matrix->getSubHeader();
  MatrixReadOutStructure matrix_footer    = matrix->getFooter();
  // Check the data structure
  assert(matrix_header.isHeader());
  assert(matrix_subheader.isSubHeader());
  assert(matrix_footer.isFooter());

  // Create the coincidence matrix 
  RpcCoinMatrix* coinMatrix = new RpcCoinMatrix(id, 
      matrix_header.cmid(), 
      matrix_footer.crc(), 
      matrix_header.fel1id(), 
      matrix_subheader.febcid());   

  // Iterate on fired channels and decode them
  MatrixReadOutStructure cm_hit;
  for (int j=0; j < matrix->numberOfBodyWords() ; ++j)
  {
    cm_hit = matrix->getCMAHit(j);
    assert(cm_hit.isBody());
    RpcFiredChannel* firedChannel=0;

    if (cm_hit.ijk() < rpcRawHitWordLength)
    {
      firedChannel = new RpcFiredChannel(cm_hit.bcid(),
          cm_hit.time(),
          cm_hit.ijk(),
          cm_hit.channel());
    }
    else if (cm_hit.ijk() == rpcRawHitWordLength)
    {
      firedChannel = new RpcFiredChannel(cm_hit.bcid(),
          cm_hit.time(),
          cm_hit.ijk(),
          cm_hit.threshold(),
          cm_hit.overlap());
    }

    coinMatrix->push_back(firedChannel);
  }

  ATH_MSG_DEBUG( "Number of Fired Channels in Matrix : " << coinMatrix->size());
  return coinMatrix;
}

