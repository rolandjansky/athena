/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RawDataByteStreamCnv/ISCT_RodEncoder.h
 * @author Nick Barlow (nbarlow@mail.cern.ch, Kondo.Gnanvo@cern.ch)
 * @date 21 June 2007 (SCT_RodEncoder turned into An AlgTool)
 * 
 * This class provides conversion from SCT RDO to ROD format ByteStream.
 */
 
#ifndef SCT_RAWDATABYTESTREAMCNV_ISCT_RODENCODER_H
#define SCT_RAWDATABYTESTREAMCNV_ISCT_RODENCODER_H

#include "GaudiKernel/IAlgTool.h"

#include <vector>
#include <cstdint>

class SCT_RDORawData;

/**
 * @class ISCT_RodEncoder
 *
 * @brief Interface for Athena Algorithm Tool to convert from SCT RDO to ROD format BysteStream.
 * 
 * Convert collections of RDO's to a vector of 32 bit word.
 *
 * This class inherits from IAlgTool.
 */
class ISCT_RodEncoder : virtual public IAlgTool 
{
 public:

  /** Creates the InterfaceID and interfaceID() method */
  DeclareInterfaceID(ISCT_RodEncoder, 1, 0);

  /** Destructor */
  virtual ~ISCT_RodEncoder() = default;

  /** Convert method */
  virtual void fillROD(std::vector<uint32_t>& vec32Data, const uint32_t& robID, 
                       const std::vector<const SCT_RDORawData*>& vecRDOs) const = 0;
}; 

#endif // SCT_RAWDATABYTESTREAMCNV_ISCT_RODENCODER_H
