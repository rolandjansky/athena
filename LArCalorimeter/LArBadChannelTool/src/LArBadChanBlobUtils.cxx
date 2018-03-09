/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChanBlobUtils.h"

namespace LArBadChanBlobUtils {

  int machineEndianness()
  {
    int i = 1;
    char *p = (char *) &i;
    if (p[0] == 1) return LittleEndian; // Lowest address contains the least significant byte
    else return BigEndian;
  }

  void fillFebBlobV1( const std::vector<HWIdentifier>& vec, coral::Blob& blob) 
  {
    std::size_t nbytes = vec.size() * sizeof( Channel);
    blob.resize(nbytes);
    void* adr = blob.startingAddress();
    for ( std::vector<HWIdentifier>::const_iterator i=vec.begin();
	  i != vec.end(); i++) {
      Channel* ch = static_cast<Channel*>( adr);
      *ch = i->get_identifier32().get_compact(); // assign channel
      adr = static_cast<void*>(ch+1); //  points just after channel
    }
  }

  bool checkBlob( const coral::Blob* blobp,
		  std::size_t chanSize,
		  std::size_t stateSizeInBlob,
		  int endian,
		  int /*version*/,
		  std::size_t step,
		  std::size_t currentStateSize,
		  MsgStream& log) 
  {
    if ( endian != machineEndianness()) {
      log << MSG::ERROR << "BLOB endian-ness and computer endian-ness differ."
	  << " This version cannot decode such BLOBs" << endmsg;
      return false;
    }
    if ( chanSize != sizeof( Channel)) {
      log << MSG::ERROR << "The word size for HWidentifier in the BLOB (" 
	  << chanSize << ") diifers from the HWIdentifier::value_type size "
	  << sizeof( Channel) << endmsg;
      log << MSG::ERROR << "This version cannot decode such BLOBs" << endmsg;
      return false;
    }
    if ( stateSizeInBlob != 0 && stateSizeInBlob != currentStateSize) {
      log << MSG::ERROR << "The word size for LArBadChannel or LArBadFeb in the BLOB (" 
	  << stateSizeInBlob << ") diifers from the runtime BitWord size "
	  << currentStateSize << endmsg;
      log << MSG::ERROR << "This version cannot decode such BLOBs" << endmsg;
      return false;
    }
 
    if (blobp->size() % step != 0) {
      log << MSG::ERROR << "BLOB size (" << blobp->size() 
	  << ") is not an exact multiple of content size " 
	  << step << endmsg;
      log << MSG::ERROR << "The BLOB is probably corrupted." << endmsg;
      return false;
    }

    // FIXME: add version check

   return true;
  }

  std::vector<HWIdentifier> decodeFebBlobV1( const coral::Blob* blobp,
					   std::size_t chanSize,
					   int endian,
					   int version,
					   MsgStream& log) 
  {
    std::vector<HWIdentifier> result;
    std::size_t step; 
    if (version == 1) step = sizeof( Channel) ;
    else              step = sizeof( Channel) + sizeof( LArBadFeb::BitWord);

    if (!checkBlob( blobp, chanSize, 0, endian, version, step, 0, log)) {
      return result;
    }
    std::size_t vecSize = blobp->size() / step;
    result.reserve( vecSize);
    const void* adr = blobp->startingAddress();
    for (std::size_t i=0; i< vecSize; i++) {
      const Channel* ch = static_cast<const Channel*>( adr);
      adr = static_cast<const void*>(ch+1);
      result.push_back( HWIdentifier(Identifier32(*ch)));
    }
    return result;
  }

}
