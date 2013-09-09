/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigOperationalInfoCollection_tlp1
 *
 * @brief "top level" persistent partner for TrigOperationalInfoCollection
 *
 * @author Tomasz Bold <Tomasz.Bold@cern.ch>           - AGH-UST Cracow/ UC Irvine
 *
 * File and Version Information:
 * $Id: TrigOperationalInfoCollection_tlp1.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TrigOperationalInfoCollection_TLP1_H
#define TRIGSTEERINGEVENTTPCNV_TrigOperationalInfoCollection_TLP1_H

#include "TrigSteeringEventTPCnv/TrigOperationalInfoCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfoCnv_p1.h"


class TrigOperationalInfoCollection_tlp1 
{
   
 public:
   
  TrigOperationalInfoCollection_tlp1(){}
  friend class TrigOperationalInfoCollectionCnv_tlp1;

 private:

  std::vector<TrigOperationalInfoCollection_p1>       m_TrigOperationalInfoCollection ;
  std::vector<TrigOperationalInfo_p1>                 m_TrigOperationalInfo           ;


};//end of class definitions
 
#endif
