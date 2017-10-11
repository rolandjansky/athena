/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_sTGCRAWDATA_H
#define MUONRDO_sTGCRAWDATA_H

#include <iostream>
#include "Identifier/Identifier.h"

class MsgStream;

class sTGC_RawData {

 private:
   Identifier m_id;

 public:
   sTGC_RawData (const Identifier id);
   virtual ~sTGC_RawData() {}
   const Identifier& identify() const { return m_id; }
};

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const sTGC_RawData& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const sTGC_RawData& coll);

#endif

