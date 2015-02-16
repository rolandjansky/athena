/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChanBlobUtils_H
#define LArBadChanBlobUtils_H

#include "CoolKernel/types.h"

#include "Identifier/HWIdentifier.h"
#include "LArRecConditions/LArBadChannel.h"
#include "LArRecConditions/LArBadFeb.h"

#include "GaudiKernel/MsgStream.h"

#include <vector>

namespace LArBadChanBlobUtils {

  typedef std::pair<HWIdentifier, LArBadChannel>                BadChanEntry;
  typedef std::pair<HWIdentifier, LArBadFeb>                    BadFebEntry;
  typedef std::vector<BadChanEntry>                             BadChanVec;
  typedef std::vector<BadFebEntry>                              BadFebVec;
  typedef Identifier32::value_type                              Channel;

  enum Endianness { LittleEndian, BigEndian};

  int machineEndianness();

  /// Creates a BLOB of the right size and copies the content of the 
  /// vector of pair<HWIdentifier, LArBadChannel> into the blob.
  //  but fill the blob with vector of pair<HWIdentifier32, LArBadChannel>
  /// Returns a pointer to the new blob, which has to be managed 
  /// (and eventually released) by the caller
  template <class T>
  void fillBlob( const std::vector< std::pair<HWIdentifier,T> >& vec, coral::Blob& blob);

  void fillFebBlobV1( const std::vector<HWIdentifier>& vec, coral::Blob& blob);

  /// Checks BLOB for differences in endian-ness and size of data types
  /// with respect to the architecture on which the code is executed
  /// and if the checks paass decodes the BLOB data into a vector of
  /// pair<HWIdentifier, LArBadChannel>. If the checks fail returns an 
  /// empty vector, and writes diagnostics on the MsgStream passed as an argument.
  /// The arguments chanSize, stateSize and endian must come from the 
  /// database that provides the BLOB, and must be filled by the 
  /// machine that created the BLOB.
  template <class T>
  std::vector<std::pair<HWIdentifier,T> >  decodeBlob( const coral::Blob* blobp,
						       std::size_t chanSize,
						       std::size_t stateSize,
						       int endian,
						       int version,
						       MsgStream& log);

  std::vector<HWIdentifier> decodeFebBlobV1( const coral::Blob* blobp,
					     std::size_t chanSize,
					     int endian,
					     int version,
					     MsgStream& log);

  bool checkBlob( const coral::Blob* blobp,
		  std::size_t chanSize,
		  std::size_t stateSizeInBlob,
		  int endian,
		  int /*version*/,
		  std::size_t step,
		  std::size_t currentStateSize,
		  MsgStream& log);
}

#include "LArBadChannelTool/LArBadChanBlobUtils.icc"

#endif
