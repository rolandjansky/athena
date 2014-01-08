/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyCocyli_H
#define MuonboyCocyli_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class MuonboyCocyli

   this class gives access to reconstruction results

  @author samusog@cern.ch
  
  */

class MuonboyCocyli{
public:
    MuonboyCocyli();
   ~MuonboyCocyli();

public:
///////////////////////////////////

   void   Print(std::ostream* out);

   int    NberOfCylinders();
   double RCYLIN(int CylinderNber);
   double ZCYLIN(int CylinderNber);
   double RCYLI2(int CylinderNber);

   void SetRCYLIN(int CylinderNber,double Value_i);
   void SetZCYLIN(int CylinderNber,double Value_i);
   
private:
///////////////////////////////////

   static const int NotDef = -1;

};

#endif
