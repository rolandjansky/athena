/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArWheelSolid_type
// Author: D. A. Maximov
// typedefs for LArWheelSolid

#ifndef __LArWheelSolid_type_HH__
#define __LArWheelSolid_type_HH__

typedef enum {
  InnerAbsorberWheel,
  OuterAbsorberWheel,
  InnerElectrodWheel,
  OuterElectrodWheel,
  InnerAbsorberModule,
  OuterAbsorberModule,
  InnerElectrodModule,
  OuterElectrodModule,
  InnerGlueWheel,
  OuterGlueWheel,
  InnerLeadWheel,
  OuterLeadWheel
} LArWheelSolid_t;


#endif // __LArWheelSolid_type_HH__
