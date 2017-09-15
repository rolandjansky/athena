/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *      @header file for SCT_RodEncoder Class
 *      This class provides conversion from SCT RDO to ROD format ByteStream.
 *      @date 21/06/07:  SCT_RodEncoder turned into An AlgTool 
 *      @author: Nick Barlow nbarlow@mail.cern.ch, Kondo.Gnanvo@cern.ch
 *  
 */
 
#ifndef SCT_RAWDATABYTESTREAM_ISCT_RODENCODER_H
#define SCT_RAWDATABYTESTREAM_ISCT_RODENCODER_H

///STL
#include <vector>

#include "GaudiKernel/IAlgTool.h"

///InDet
/** needed for typedef, cannot fwd declare */
#include "InDetRawData/SCT_RDO_Container.h"

class InterfaceID;
class StatusCode;
class SCT_RDORawData;

class ISCT_RodEncoder : virtual public IAlgTool {
 public:

  typedef SCT_RDORawData RDO ;
  typedef std::vector<const RDO*> vRDOs ;
  
  /** destructor */
  virtual ~ISCT_RodEncoder() {}; 

  /** AlgTool InterfaceID */
  static const InterfaceID& interfaceID() ;

  /** convert all collections of RDO's in the current  list to vector of 32bit words */
  virtual void fillROD(std::vector<uint32_t>&, uint32_t, vRDOs&) = 0;

  /** on the different sides of modules, strip numbers go 0-767 or 767-0.  */
  virtual void addSwapModuleId (Identifier IdColl) = 0; 

}; 


inline const InterfaceID& ISCT_RodEncoder::interfaceID() {
  static const InterfaceID ISCT_RodEncoderIID("ISCT_RodEncoder",1,0);
  return ISCT_RodEncoderIID;
}


#endif
