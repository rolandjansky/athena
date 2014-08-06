// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ============================================================================
// PATCoreEnums.h
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
// ============================================================================

#ifndef PATCORE_PATCOREENUMS_H
#define PATCORE_PATCOREENUMS_H 1


/** Information about type of data used to fill particle.
 NOTE! This must be consistant with ../python/ParticleDataType.py */
namespace PATCore 
{
  namespace ParticleDataType
  {
    enum DataType {Data = 0, Full = 1, FastShower = 2, Fast = 3, True = 4} ;
  }
}



/** Information about type of data used to fill particle.
 NOTE! This must be consistant with ../python/ParticleType.py */
namespace PATCore 
{
  namespace ParticleType
  {
    enum Type
      {
        Unknown           = 0,
        Photon            = 10,
        UnconvertedPhoton = 11,
        ConvertedPhoton   = 12,
        Electron          = 20,
        Muon              = 30,
        Tau               = 40,
        OneProngTau       = 41,
        ThreeProngTau     = 42,
        Jet               = 50,
        GluonJet          = 51,
        LightQuarkJet     = 52,
        CharmJet          = 53,
        BJet              = 54
      };
  }
}



#endif
