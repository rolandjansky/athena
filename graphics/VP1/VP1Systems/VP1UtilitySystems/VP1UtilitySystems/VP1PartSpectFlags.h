/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1PARTSPECTFLAGS_H
#define VP1PARTSPECTFLAGS_H
 
/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for the namespace VP1PartSpect                         //
//                                                                     //
//                                                                     //
//  Initial version: September 2009                                    //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

namespace VP1PartSpect
{
  enum ParticleType {
    Baryon = 0,
    E,
    Gamma,
    Lepton,
    Meson,
    Neutron,     
    Nucleus, 
    Pi, 
    Proton 
  };
}

#endif
