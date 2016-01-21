/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigRoiDescriptor_p3
 *
 * @brief persistent partner for TrigRoiDescriptor
 *
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TRIGROIDESCRIPTOR_P3_H
#define TRIGSTEERINGEVENTTPCNV_TRIGROIDESCRIPTOR_P3_H


class TrigRoiDescriptor_p3
{
 public:
  
  TrigRoiDescriptor_p3() {}
  friend class TrigRoiDescriptorCnv_p3;
  
 private:
  float    geom[9];

  unsigned ids[2];

  bool     fullscan = false;

  std::vector< std::vector<float> > rois;

/*   unsigned int m_roiId;          */
/*   unsigned int m_roiWord;        */
};

#endif
