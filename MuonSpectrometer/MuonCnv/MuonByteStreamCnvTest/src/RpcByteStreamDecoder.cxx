/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamCnvTest/RpcByteStreamDecoder.h"
#include "RPCcablingInterface/CablingRPCBase.h"

namespace {
  static constexpr unsigned int const& rpcRawHitWordLength = 7;
}

RpcByteStreamDecoder::RpcByteStreamDecoder(const RPCbytestream * p_bytestream, const RpcCablingCondData* readCdo, const RpcIdHelper* rpcId, MsgStream* log) :
  m_bytestream(p_bytestream),
  m_cabling(readCdo),
  m_rpcIdHelper(rpcId)
{ 
  m_rpcpads = new std::vector<RpcPad*>;
  m_log = log;
  if ( m_log ) {
    m_debug = m_log->level() <= MSG::DEBUG;
    m_verbose = m_log->level() <= MSG::VERBOSE;
  } else {
    m_debug   = false;
    m_verbose = false; 
  }
}

RpcByteStreamDecoder::~RpcByteStreamDecoder()
{
  if( m_rpcpads != NULL ) { delete m_rpcpads; m_rpcpads = NULL; }
}

StatusCode RpcByteStreamDecoder::decodeByteStream()
{
  if ( m_debug && m_log ) *m_log << MSG::DEBUG << "Start decoding" << endmsg;

  PAD_Readout padReadout = m_bytestream->pad_readout();

  // Iterate on the readout PADS and decode them
  for (PAD_Readout::iterator it = padReadout.begin() ; it != padReadout.end(); ++it)
    {
      PADreadout pad = (*it).second;
      RpcPad* newpad = decodePad(pad);

      // Push back the decoded pad in the vector
      m_rpcpads->push_back(newpad);
    }

  if ( m_debug && m_log ) *m_log << MSG::DEBUG << "Number of decoded Pads : " 
                                 << m_rpcpads->size() << endmsg;

  return StatusCode::SUCCESS;
}

// Decode a pad and return a pointer to a RpcPad RDO
RpcPad* RpcByteStreamDecoder::decodePad(PADreadout& pad)
{

  if ( m_debug && m_log ) *m_log << MSG::DEBUG << "Decoding a new RpcPad" << endmsg;

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
  
  if ( m_debug && m_log ) *m_log << MSG::DEBUG << "Pad: Side " << side 
                                 << " Sector logic" << logic_sector 
                                 << " Id " << pad_id << endmsg;
  
  // Retrieve the identifier elements from the map
  const CablingRPCBase::RDOmap& pad_map = m_cabling->give_RDOs();
  RDOindex index = (*pad_map.find(key)).second;

  index.offline_indexes(name, eta, phi, doublet_r, doublet_z, doublet_phi, gas_gap, 
		        measures_phi, strip);

  // Build the pad offline identifier
  bool check = true;
  bool valid = false;
  Identifier id = m_rpcIdHelper->padID(name, eta, phi, doublet_r, doublet_z, doublet_phi, check, &valid);

  if ( m_log && !valid ) {
    *m_log << MSG::ERROR << "Invalid pad offline indices " << endmsg;
    *m_log << MSG::ERROR << "Name : "    << name << endmsg;
    *m_log << MSG::ERROR << "Eta "       << eta       << "   Phi "   
	   << phi << endmsg;
    *m_log << MSG::ERROR << "Doublet r " << doublet_r << "  Doublet_z " 
	   << doublet_z << "  Doublet_phi " << doublet_phi << endmsg;
    *m_log << MSG::ERROR << "Gas gap "   << gas_gap   << "   Measures_phi " 
	   << measures_phi << "  Strip " << strip << endmsg;
  }

  // Retrieve Pad status and error code from Pad header and footer
  PadReadOutStructure pad_header = readout->getHeader();
  PadReadOutStructure pad_footer = readout->getFooter();
  // Check the data format
  assert(pad_header.isHeader());
  assert(pad_footer.isFooter());

  unsigned int hashId = index.hash();
  unsigned int onlineId  = pad_id;
  // unsigned int status    = pad_header.status();
  unsigned int status = 0;
  unsigned int errorCode = pad_footer.errorCode();

  // Construct the new Pad
  RpcPad * rpc_pad = new RpcPad(id, hashId, onlineId, status, errorCode, sector);
  
  // Iterate on the matrices and decode them
  for (int i=0 ; i < readout->numberOfCMROFragments() ; ++i)
    {
      MatrixReadOut* matrix  = pad.matrices_readout(i);
      RpcCoinMatrix* coinMatrix = decodeMatrix(matrix, id);
      // Add the matrix to the pad
      rpc_pad->push_back(coinMatrix);
    }
  
  if ( m_debug && m_log ) *m_log << MSG::DEBUG << "Number of matrices in Pad : " 
                                 << rpc_pad->size() << endmsg; 
  
  return rpc_pad;
}


// Function decoding a coincidence matrix
RpcCoinMatrix* RpcByteStreamDecoder::decodeMatrix(MatrixReadOut* matrix, Identifier& id)
{

  if ( m_debug && m_log ) *m_log << MSG::DEBUG << "Decoding a new RpcCoinMatrix" << endmsg;

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

      if (cm_hit.ijk() < rpcRawHitWordLength )
	{
	  firedChannel = new RpcFiredChannel(cm_hit.bcid(),
					     cm_hit.time(),
					     cm_hit.ijk(),
					     cm_hit.channel());
	}
      else if (cm_hit.ijk() == rpcRawHitWordLength )
	{
	  firedChannel = new RpcFiredChannel(cm_hit.bcid(),
					     cm_hit.time(),
					     cm_hit.ijk(),
					     cm_hit.threshold(),
					     cm_hit.overlap());
	}
      
      coinMatrix->push_back(firedChannel);
    }

  if ( m_debug && m_log ) *m_log << MSG::DEBUG << "Number of Fired Channels in Matrix : " 
                                 << coinMatrix->size() << endmsg; 
  return coinMatrix;
}

// Printout the RDOs for testing
void RpcByteStreamDecoder::print()
{
  std::cout << "Printing out RPC RDO's : " << std::endl;
  std::cout << "Number of Pads in event : " << m_rpcpads->size() << std::endl;
  std::vector<RpcPad*>::const_iterator pad_it;
  int ipad=1;
  // Iterate on pads
  for (pad_it = m_rpcpads->begin() ; 
       pad_it != m_rpcpads->end() ; ++pad_it, ++ipad)
    {
      std::cout << "Pad number " << ipad << " Identifier : " << std::endl;
      (*pad_it)->identify().show();
      std::cout << std::endl;
      std::cout << "Number of Matrices in Pad : " << (*pad_it)->size() << std::endl; 
      // Iterate on the pad's coincidence matrices
      RpcPad::const_iterator mat_it;
      int imat=1;
      for (mat_it = (*pad_it)->begin() ; 
	   mat_it != (*pad_it)->end() ; ++mat_it, ++imat)
	{
	  std::cout << "Matrix number " << imat << std::endl;
	  std::cout << "Number of fired channels: " << (*mat_it)->size() << std::endl;
	  RpcCoinMatrix::const_iterator chan_it;
	  int ichan=1;
	  // Printout the fired channels
	  for (chan_it = (*mat_it)->begin() ; 
	       chan_it != (*mat_it)->end() ; ++chan_it, ++ichan)
	    {
	      std::cout << "Fired Channel " << ichan 
			<< " :: bcid "      << (*chan_it)->bcid() 
			<< "   ijk "        << (*chan_it)->ijk() 
			<< "   channel "    << (*chan_it)->channel() << std::endl;
	    }
	}
    }
}


