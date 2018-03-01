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

 // offline identifier
   Identifier m_id;

   // readout data ( not sure whether time and charge should be float )
   float m_time;
   float m_charge;
   uint16_t m_bcTag;
   
   bool m_isDead;
   

 public:
   STGC_RawData (const Identifier id);
   STGC_RawData (); //!< TODO remove this. Currently have problems with convertor if I do though.

   STGC_RawData (const Identifier id, const uint16_t bcTag, const float time, const float charge, 
		 const bool isDead );
 
   virtual ~STGC_RawData() {}
   const Identifier& identify() const { return m_id; }

   // access functions
   float time() const {return m_time;}
   float charge() const {return m_charge;}
   uint16_t bcTag() const {return m_bcTag;}

   bool isDead() const {return m_isDead;}

};
}

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const Muon::STGC_RawData& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const Muon::STGC_RawData& coll);

#endif

