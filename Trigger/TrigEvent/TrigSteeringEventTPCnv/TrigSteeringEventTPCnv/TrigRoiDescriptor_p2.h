/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigRoiDescriptor_p2
 *
 * @brief persistent partner for TrigRoiDescriptor
 *
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TRIGROIDESCRIPTOR_P2_H
#define TRIGSTEERINGEVENTTPCNV_TRIGROIDESCRIPTOR_P2_H


class TrigRoiDescriptor_p2
{
 public:
  
  TrigRoiDescriptor_p2() {}
  friend class TrigRoiDescriptorCnv_p2;
  
 private:
  float geom[8];

/*   float m_phi0;                  */
/*   float m_eta0;    */
/*   float m_zed0; */
/*   float m_phiHalfWidth;          */
/*   float m_etaHalfWidth;          */
/*   float m_zedHalfWidth; */
/*   float m_etaPlus; */
/*   float m_etaMinus; */

  unsigned int ids[2];

/*   unsigned int m_roiId;          */
/*   unsigned int m_roiWord;        */
};

#endif
