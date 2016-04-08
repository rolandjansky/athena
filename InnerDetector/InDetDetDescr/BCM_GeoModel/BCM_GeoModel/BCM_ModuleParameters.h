/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BCM_ModuleParameters.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef BCMMODULEPARAMETERS_H
#define BCMMODULEPARAMETERS_H

#include <vector>

  /** @class BCM_ModuleParameters

  Beam Condition Monitor module parameters
      @author  Bostjan Macek <bostjan.macek@cern.ch>
  */  

class BCM_ModuleParameters
{
 public:
  void Set(int module_number, const std::vector<double>* module_property);

  int Number() const {return m_module_number;}
  double Position_X() const {return m_trans_X;}
  double Position_Y() const {return m_trans_Y;}
  double Position_Z() const {return m_trans_Z;}
  double Rotation_X() const {return m_rot_X;}
  double Rotation_Y() const {return m_rot_Y;}
  double Rotation_Z() const {return m_rot_Z;}
  double BackWallThickness() const {return m_dimension_Z;}
  double DiamondPosition_Y() const {return m_diamond_trans_Y;}
  double DiamondPosition_Z() const {return m_diamond_trans_Z;}
  double DiamondDelta_X() const {return m_diamond_diamond_X;}
  double DiamondDelta_Y() const {return m_diamond_diamond_Y;}
  double DiamondDelta_Z() const {return m_diamond_diamond_Z;}
  double LamelOffset_H() const {return m_lamel_offset_H;}
  double LamelOffset_I() const {return m_lamel_offset_I;}
  double LamelOffset_J() const {return m_lamel_offset_J;}
  
 private:
  int m_module_number;
  double m_trans_X;
  double m_trans_Y;
  double m_trans_Z;
  double m_rot_X;
  double m_rot_Y;
  double m_rot_Z;
  double m_dimension_Z;
  double m_diamond_trans_Y;
  double m_diamond_trans_Z;
  double m_diamond_diamond_X;
  double m_diamond_diamond_Y;
  double m_diamond_diamond_Z;
  double m_lamel_offset_H;
  double m_lamel_offset_I;
  double m_lamel_offset_J;
};

#endif
