/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** N.Barlow, Apr 2008
 * replace float[20] with vector<int> to represent strips with errors.
 * in transient class.. in persistent version this vector is stored directly in 
 * the container, as ROOT is not good at dealing with vectors of vectors. */

#ifndef SCT3_RAWDATA_P2_H
#define SCT3_RAWDATA_P2_H

#include "Identifier/Identifier.h"

class SCT3_RawData_p2 {
 public:
  SCT3_RawData_p2(): m_rdoId{0}, m_word{0} {};
// List of Cnv classes that convert this into Rdo objects
  friend class SCT3_RawDataCnv_p2;
  // private:
  Identifier32::value_type m_rdoId; //Offline ID for readout channel
  unsigned int m_word; // raw data word 

};

#endif
