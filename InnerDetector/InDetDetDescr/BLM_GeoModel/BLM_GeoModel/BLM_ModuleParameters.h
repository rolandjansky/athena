/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BLM_ModuleParameters.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef BLMMODULEPARAMETERS_H
#define BLMMODULEPARAMETERS_H

#include <vector>

  /** @class BLM_ModuleParameters

  Beam Loss Monitor module parameters
      @author  Bostjan Macek <bostjan.macek@cern.ch>
  */  

class BLM_ModuleParameters
{
 public:
  void Set(int module_number, const std::vector<double>* module_property);

  int Number() const {return m_module_number;}
  double Z() const {return m_trans_X;}
  double R() const {return m_trans_Y;}
  double Phi() const {return m_trans_Z;}
  double Rotation_X() const {return m_rot_X;}
  double Rotation_Y() const {return m_rot_Y;}
  double Rotation_Z() const {return m_rot_Z;}
  double DiamondPosition_X() const {return m_diamond_trans_X;}
  double DiamondPosition_Y() const {return m_diamond_trans_Y;}
  double DiamondPosition_Z() const {return m_diamond_trans_Z;}

  
 private:
  int m_module_number;
  double m_trans_X;
  double m_trans_Y;
  double m_trans_Z;
  double m_rot_X;
  double m_rot_Y;
  double m_rot_Z;
  double m_diamond_trans_X;
  double m_diamond_trans_Y;
  double m_diamond_trans_Z;
};

#endif
