/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigOperationalInfo_p1
 *
 * @brief persistent partner for TrigOperationalInfo
 *
 * @author Tomasz Bold (based on exampels by Andrew Hamilton and Francesca Bucci)
 *
 * File and Version Information:
 * $Id: TrigOperationalInfo_p1.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TrigOperationalInfo_P1_H
#define TRIGSTEERINGEVENTTPCNV_TrigOperationalInfo_P1_H


class TrigOperationalInfo_p1
{
 public:
  
  TrigOperationalInfo_p1() {}
  friend class TrigOperationalInfoCnv_p1;
  
 private:
  std::vector<std::string> m_keys;
  std::vector<float> m_values;

};

#endif
