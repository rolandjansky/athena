/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////
//  Class: VP1SoMaterialMixer          //
//  First version: November 2007       //
//  Author: Thomas.Kittelmann@cern.ch  //
/////////////////////////////////////////

#ifndef VP1SOMATERIALMIXER_H
#define VP1SOMATERIALMIXER_H

#include "VP1Base/VP1HelperClassBase.h"
#include <set>
#include <map>
class SoMaterial;

class VP1SoMaterialMixer : public VP1HelperClassBase {
public:

  VP1SoMaterialMixer(IVP1System * sys = 0);//sys!=0 for messages in gui
  virtual ~VP1SoMaterialMixer();

  //First version - mixes materials with equal weight:
  SoMaterial * getMixedMaterial(const std::set<SoMaterial*>&);

  //The second version is similar, but each material has a weight (must be > 0)
  SoMaterial * getMixedMaterial(const std::map<SoMaterial*,double>&);

  //Convenience methods for when you are combining 2 materials (dispenses with need to put in a set):
  SoMaterial * getMixedMaterial( SoMaterial* mat1,SoMaterial* mat2 );
  SoMaterial * getMixedMaterial( SoMaterial* mat1, const double& weight1,
				 SoMaterial* mat2, const double& weight2 );

  //NB1: Never modify the returned material directly!!
  //     It is ok to modify the fields of the passed SoMaterials later
  //     - the inventor notification mechanism is used to monitor this
  //     and automatically update the fields of the mixed material node
  //     when that happens.
  //
  //NB2: All materials in input list must have exactly 1 value in each
  //     field!  (as is usually the case). Methods return a default
  //     material in case of any problems in the input list.

private:

  class Imp;
  Imp * m_d;

};

#endif
