/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigOperationalInfoCollection_p1
 *
 * @brief persistent partner for TrigOperationalInfoCollection_p1
 *
 * @author Tomasz Bold (based on exampels by Andrew Hamilton and Francesca Bucci)
 *
 * File and Version Information:
 * $Id: TrigOperationalInfoCollection_p1.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TrigOperationalInfoCollection_p1
#define TRIGSTEERINGEVENTTPCNV_TrigOperationalInfoCollection_p1

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigOperationalInfoCollection_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigOperationalInfoCollection_p1(){}
     
};// end of class definitions
 
 
#endif
