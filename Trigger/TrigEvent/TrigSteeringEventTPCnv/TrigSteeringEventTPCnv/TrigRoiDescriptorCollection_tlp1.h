/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigRoiDescriptorCollection_tlp1
 *
 * @brief "top level" persistent partner for TrigRoiDescriptorCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Tomasz Bold <Tomasz.Bold@cern.ch>           - AGH-UST Cracow/ UC Irvine
 *
 * File and Version Information:
 * $Id: TrigRoiDescriptorCollection_tlp1.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TrigRoiDescriptorCollection_TLP1_H
#define TRIGSTEERINGEVENTTPCNV_TrigRoiDescriptorCollection_TLP1_H

#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p1.h"


class TrigRoiDescriptorCollection_tlp1 
{
   
 public:
   
  TrigRoiDescriptorCollection_tlp1(){}
  friend class TrigRoiDescriptorCollectionCnv_tlp1;

 private:

  std::vector<TrigRoiDescriptorCollection_p1>       m_TrigRoiDescriptorCollection ;
  std::vector<TrigRoiDescriptor_p1>                 m_TrigRoiDescriptor           ;


};//end of class definitions
 
#endif
