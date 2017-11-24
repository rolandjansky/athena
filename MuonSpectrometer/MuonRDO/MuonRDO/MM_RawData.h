/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MM_RAWDATA_H
#define MUONBYTESTREAM_MM_RAWDATA_H

#include <inttypes.h>
#include <vector>
#include <iostream>
#include "Identifier/Identifier.h"

class MsgStream;

/**
Temporary class to hold the MM RDO.
*/
class MM_RawData {

 private:
   
   const Identifier m_id;
   
 public:

  /** default constructor */
  MM_RawData ( const Identifier& id ) :  m_id(id) {}

  
  virtual ~MM_RawData() {}

  const Identifier& identify() const { return m_id; }

};

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const MM_RawData& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const MM_RawData& coll);


#endif







