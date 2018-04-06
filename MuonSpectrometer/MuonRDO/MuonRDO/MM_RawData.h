/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_MM_RAWDATA_H
#define MUONRDO_MM_RAWDATA_H

#include <inttypes.h>
#include <vector>
#include <iostream>
#include "Identifier/Identifier.h"

class MsgStream;

namespace Muon {

/**
Temporary class to hold the MM RDO.
*/
class MM_RawData {
  friend class MM_RawDataCnv_p1;
  

 private:
   
   const Identifier m_id;
   
 public:

  /** default constructor */
  MM_RawData ( const Identifier& id ) :  m_id(id) {}
  MM_RawData () {} ; //!< TODO remove this. Currently have problems with convertor if I do though.
  
  virtual ~MM_RawData() {}

  const Identifier& identify() const { return m_id; }

};
}

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const Muon::MM_RawData& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const Muon::MM_RawData& coll);


#endif







