/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigRoiDescriptor_p1
 *
 * @brief persistent partner for TrigRoiDescriptor
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigRoiDescriptor_p1.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TRIGROIDESCRIPTOR_P1_H
#define TRIGSTEERINGEVENTTPCNV_TRIGROIDESCRIPTOR_P1_H


class TrigRoiDescriptor_p1
{
 public:
  
  TrigRoiDescriptor_p1() {}
  friend class TrigRoiDescriptorCnv_p1;
  
 private:
    
  float m_phi0 = 0;                 
  float m_eta0 = 0;   
  float m_zed0 = 0;
  float m_phiHalfWidth = 0;         
  float m_etaHalfWidth = 0;         
  float m_zedHalfWidth = 0;
  float m_etaPlus = 0;
  float m_etaMinus = 0;
         
  unsigned int m_l1Id = 0;          
  unsigned int m_roiId = 0;         
  unsigned int m_roiWord = 0;       
  std::vector<uint32_t> m_serialized;


};

#endif
