/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_STGCRAWDATA_H
#define MUONRDO_STGCRAWDATA_H

#include <iostream>
#include "Identifier/Identifier.h"

class MsgStream;

namespace Muon {
class STGC_RawData {
 friend class STGC_RawDataCnv_p1;
 friend class STGC_RawDataCnv_p2;
 
 private:

 // offline identifier
   const Identifier m_id;

   // readout data
   // keeping time in two variables: a float and an int. The float variable is kept for backward compatibility
   float m_time;
   unsigned int m_tdo; // time converted to TDC counts
   unsigned int m_charge;
   uint16_t m_bcTag;
   
   bool m_isDead;
   

 public:
   STGC_RawData (const Identifier id);
   STGC_RawData () {} //!< TODO remove this. Currently have problems with convertor if I do though.

   STGC_RawData (const Identifier id, const uint16_t bcTag, const float time, const unsigned int charge, 
		 const bool isDead );
 
   STGC_RawData (const Identifier id, const uint16_t bcTag, const float time, const unsigned int tdo, 
                 const unsigned int charge, const bool isDead );

   virtual ~STGC_RawData() {}

   const Identifier identify() const { return m_id; }

   // access functions
   float time() const {return m_time;}
   unsigned int tdo() const {return m_tdo;}
   unsigned int charge() const {return m_charge;}
   uint16_t bcTag() const {return m_bcTag;}

   bool isDead() const {return m_isDead;}

};
}

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const Muon::STGC_RawData& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const Muon::STGC_RawData& coll);

#endif

