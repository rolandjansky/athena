/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4H8CalibSD_H__
#define __LArG4H8CalibSD_H__

#include "LArG4SD/LArG4CalibSD.h"


class LArG4H8CalibSD : public LArG4CalibSD
{
public:
  LArG4H8CalibSD(G4String name);
  
private:
  virtual void initializeProcessing();

  static bool m_initTBProcessing;

};

#endif // __LArG4H8CalibSD_H__
