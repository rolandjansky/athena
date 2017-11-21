/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_STGCRAWDATA_H
#define MUONRDO_STGCRAWDATA_H

#include <iostream>
#include "Identifier/Identifier.h"

class MsgStream;

namespace Muon {
class STGC_RawData {
 friend class STGC_RawDataCnv_p1;
 
 private:
   Identifier m_id;

 public:
   STGC_RawData (const Identifier id);
   STGC_RawData (); //!< TODO remove this. Currently have problems with convertor if I do though.
   virtual ~STGC_RawData() {}
   const Identifier& identify() const { return m_id; }
};
}

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const Muon::STGC_RawData& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const Muon::STGC_RawData& coll);

#endif

