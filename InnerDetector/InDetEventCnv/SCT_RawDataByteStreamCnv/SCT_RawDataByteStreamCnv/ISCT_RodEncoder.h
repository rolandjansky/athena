/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 *      @header file for SCT_RodEncoder Class
 *      This class provides conversion from SCT RDO to ROD format ByteStream.
 *      @date 21/06/07:  SCT_RodEncoder turned into An AlgTool 
 *      @author: Nick Barlow nbarlow@mail.cern.ch, Kondo.Gnanvo@cern.ch
 *  
 */
 
#ifndef SCT_RAWDATABYTESTREAMCNV_ISCT_RODENCODER_H
#define SCT_RAWDATABYTESTREAMCNV_ISCT_RODENCODER_H

///STL
#include <vector>
#include <cstdint>

#include "GaudiKernel/IAlgTool.h"

///InDet
/** needed for typedef, cannot fwd declare */
#include "InDetRawData/SCT_RDO_Container.h"

class SCT_RDORawData;

class ISCT_RodEncoder : virtual public IAlgTool {
 public:

  typedef SCT_RDORawData RDO;
  typedef std::vector<const RDO*> vRDOs;
  
  /** destructor */
  virtual ~ISCT_RodEncoder() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_RodEncoder, 1, 0);

  /** convert all collections of RDO's in the current  list to vector of 32bit words */
  virtual void fillROD(std::vector<uint32_t>& vec32Data, const uint32_t& robID, vRDOs& vecRDOs) const = 0;

}; 

#endif // SCT_RAWDATABYTESTREAMCNV_ISCT_RODENCODER_H
