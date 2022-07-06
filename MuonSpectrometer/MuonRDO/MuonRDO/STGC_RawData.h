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
 friend class STGC_RawDataCnv_p3;
 
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
   bool m_timeAndChargeInCounts;
   

 public:
   STGC_RawData (const Identifier id);
   STGC_RawData () {} //!< TODO remove this. Currently have problems with convertor if I do though.

   STGC_RawData (const Identifier id, const uint16_t bcTag, const float time, const unsigned int charge, 
		 const bool isDead, const bool timeAndChargeInCounts);
 
   STGC_RawData (const Identifier id, const uint16_t bcTag, const float time, const unsigned int tdo, 
                 const unsigned int charge, const bool isDead, const bool timeAndChargeInCounts );

   virtual ~STGC_RawData() {}

   const Identifier identify() const { return m_id; }

   // access functions
   float time() const {return m_time;}
   unsigned int tdo() const {return m_tdo;}
   unsigned int charge() const {return m_charge;}
   uint16_t bcTag() const {return m_bcTag;}

   bool isDead() const {return m_isDead;}
   bool timeAndChargeInCounts() const {return m_timeAndChargeInCounts;}

   // Lower time bound of digits selected at the end of digitization.
   // As of June 2022, the main event is assigned to relative_BCID=3, which is set to correspond
   // to the time interval [-12.5ns, +12.5ns]. The digits within the BC window [-3, +4] are kept. 
   // Therefore the lower time bound is -87.5 ns.
   static constexpr double s_lowerTimeBound{-87.5};

   // BC window
   static constexpr int s_BCWindow{8};

  // A Simple constant to convert time to TDC counts in the absence of time calibration.
  // This constant should be used only to preserve backward compatibility, since it doesn't have 
  // physical meaning and the value is arbitrary. The value is chosen to be greater than 
  // 25 ns plus the absolute value of the time corresponding to the first bunch crossing defined in
  // sTGC_Digitization/python/sTGC_DigitizationConfig.py, which is -100ns as of 2022-05-01.
  // Therefore, after time-tdo conversion, the resulting TDC count is positive.
  static constexpr int s_timeTdoShift{200};
};
}

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const Muon::STGC_RawData& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const Muon::STGC_RawData& coll);

#endif

