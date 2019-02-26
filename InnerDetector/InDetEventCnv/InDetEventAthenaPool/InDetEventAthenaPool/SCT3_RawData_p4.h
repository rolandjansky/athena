/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** S.Oda, Oct 2018
 * replace Identifier::value_type by Identifier32::value_type and
   unsigned int by unsigned short for
   for the premixing samples for Overlay MC. */

#ifndef SCT3_RAWDATA_P4_H
#define SCT3_RAWDATA_P4_H

#include "Identifier/Identifier32.h"

class SCT3_RawData_p4 {
 public:
  SCT3_RawData_p4() {};
// List of Cnv classes that convert this into Rdo objects
  friend class SCT3_RawDataCnv_p4;
  //private:
  unsigned short m_rowStrip; // row and strip numbers from Identifier. Bits 0-12 for strip, Bits 13-15 for row
  unsigned short m_word; // raw data word. Bits 0-10 for group size, Bits 11-13 for time bin

  void setRowStrip(const int row, const int strip) {
    m_rowStrip  =  (static_cast<unsigned int>(strip) & 0x1FFF);
    m_rowStrip |= ((static_cast<unsigned int>(row) & 0x7) << 13);
  }

  int getRow() const {
    return ((m_rowStrip >> 13) & 0x7);
  }

  int getStrip() const {
    return (m_rowStrip & 0x1FFF);
  }

};

#endif // SCT3_RAWDATA_P4_H
