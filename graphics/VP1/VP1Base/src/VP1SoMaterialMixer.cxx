/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////
//  Class: VP1SoMaterialMixer          //
//  First version: November 2007       //
//  Author: Thomas.Kittelmann@cern.ch  //
/////////////////////////////////////////

#include "VP1Base/VP1SoMaterialMixer.h"
#include "VP1Base/VP1Msg.h"
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/sensors/SoNodeSensor.h>

//____________________________________________________________________
class VP1SoMaterialMixer::Imp {
public:
  VP1SoMaterialMixer * theclass;
  void setMaterialFieldsAsAverageOfMatList(SoMaterial*mat, std::set<SoMaterial*> matlist );
  void setMaterialFieldsAsAverageOfMatList(SoMaterial*mat, std::map<SoMaterial*,double> matlist );
  static float norm(const float& x) { return (x<0.0f?0.0f:(x>1.0f?1.0f:x));}

  //Callback used to monitor changes in materials:
  static void materialChangedCB(void *data, SoSensor *);
  static std::map<SoSensor*,VP1SoMaterialMixer::Imp*> sensor2matmixerimp;
  void monitorMaterial(SoMaterial*);
  void materialChanged(SoMaterial*);

  //For dealing with invalid input materials:
  static bool inputMaterialValid(SoMaterial*);
  SoMaterial *defmat;
  SoMaterial * defaultMaterial()
  {
    if (!defmat) {
      defmat = new SoMaterial;
      defmat->ref();
    }
    return defmat;
  }

  //Data members:
  std::map<std::set<SoMaterial*>,SoMaterial*> matlists2mixedmats;
  std::map<std::map<SoMaterial*,double>,SoMaterial*> matlists2mixedmats_weighted;
  std::map<SoMaterial*,SoNodeSensor*> mat2sensors;

  //We could make it a bit more optimal by having different mat2sensor
  //map + callback methods for weighted mixing. But since a given
  //instance of the material mixer class is likely used mainly for
  //either weighted or unweighted mixing, this is questionable how
  //often this would actually help.
};

std::map<SoSensor*,VP1SoMaterialMixer::Imp*> VP1SoMaterialMixer::Imp::sensor2matmixerimp;

//____________________________________________________________________
VP1SoMaterialMixer::VP1SoMaterialMixer(IVP1System * sys)
  : VP1HelperClassBase(sys,"VP1SoMaterialMixer"), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->defmat = 0;
}

//____________________________________________________________________
VP1SoMaterialMixer::~VP1SoMaterialMixer()
{
  {
    std::map<std::set<SoMaterial*>,SoMaterial*>::const_iterator it, itE = m_d->matlists2mixedmats.end();
    std::set<SoMaterial*>::iterator it2, it2E;
    for (it = m_d->matlists2mixedmats.begin();it!=itE;++it) {
      for (it2=it->first.begin(), it2E=it->first.end();it2!=it2E;++it2)
	(*it2)->unref();
      it->second->unref();
    }
  }
  {
    std::map<std::map<SoMaterial*,double>,SoMaterial*>::iterator it, itE = m_d->matlists2mixedmats_weighted.end();
    std::map<SoMaterial*,double>::const_iterator it2,it2E;
    for (it = m_d->matlists2mixedmats_weighted.begin();it!=itE;++it) {
      for (it2=it->first.begin(),it2E=it->first.end();it2!=it2E;++it2)
 	it2->first->unref();
      it->second->unref();
    }
  }

  {
    std::map<SoMaterial*,SoNodeSensor*>::iterator it(m_d->mat2sensors.begin()), itE(m_d->mat2sensors.end());
    for (;it!=itE;++it) {
      m_d->sensor2matmixerimp.erase(m_d->sensor2matmixerimp.find(it->second));
      delete it->second;
      it->first->unref();
    }
  }
  if (m_d->defmat)
    m_d->defmat->unref();

  //Print out warning if too many different mixed materials, to give
  //the developers a chance to notice if they are doing something not
  //optimal.

  if (m_d->matlists2mixedmats.size()+m_d->matlists2mixedmats_weighted.size()>100)
    messageDebug("WARNING: Watched more than 100 ("
		 +str(m_d->matlists2mixedmats.size()+m_d->matlists2mixedmats_weighted.size())
		 + ") different material combinations. Try to use fewer combinations for better performance!");
  if (m_d->mat2sensors.size()>1000)
    messageDebug("WARNING: Monitored more than 1000 (" +str(m_d->mat2sensors.size())+
		 ") different materials. Try to lower this number for better performance!");

  delete m_d;
}

//____________________________________________________________________
bool VP1SoMaterialMixer::Imp::inputMaterialValid(SoMaterial*m)
{
  return m
    && m->ambientColor.getNum() == 1
    && m->diffuseColor.getNum() == 1
    && m->specularColor.getNum() == 1
    && m->emissiveColor.getNum() == 1
    && m->transparency.getNum() == 1
    && m->shininess.getNum() == 1;

}

//____________________________________________________________________
void VP1SoMaterialMixer::Imp::materialChangedCB(void *data, SoSensor * sensor)
{
  SoMaterial *material = (SoMaterial *)data;
  if (!inputMaterialValid(material)) {
    VP1Msg::message("ERROR: Monitored material changed to invalid state"
		    " (multiple values in a field). Ignore changes.");
    return;
  }

  std::map<SoSensor*,VP1SoMaterialMixer::Imp*>::iterator it = sensor2matmixerimp.find(sensor);
  if (it!=sensor2matmixerimp.end())
    it->second->materialChanged(material);
}

//____________________________________________________________________
void VP1SoMaterialMixer::Imp::monitorMaterial( SoMaterial* mat )
{
  if (mat2sensors.find(mat)!=mat2sensors.end())
    return;//We already monitor this material.

  SoNodeSensor * sensor = new SoNodeSensor(Imp::materialChangedCB, mat);
  sensor->attach(mat);
  sensor2matmixerimp[sensor] = this;
  mat2sensors[mat]=sensor;
  mat->ref();

}

//____________________________________________________________________
void VP1SoMaterialMixer::Imp::materialChanged( SoMaterial * mat )
{
  //First look in lists for un-weighted mixtures:
  std::map<std::set<SoMaterial*>,SoMaterial*>::const_iterator it, itE = matlists2mixedmats.end();
  for (it = matlists2mixedmats.begin();it!=itE;++it) {
    std::set<SoMaterial*>::iterator it2(it->first.begin()),it2E(it->first.end());
    for (;it2!=it2E;++it2) {
      if (mat==*it2) {
	setMaterialFieldsAsAverageOfMatList(it->second,it->first);
	break;
      }
    }
  }
  //Then look in lists for weighted mixtures:
  std::map<std::map<SoMaterial*,double>,SoMaterial*>::const_iterator it3, it3E = matlists2mixedmats_weighted.end();
  for (it3 = matlists2mixedmats_weighted.begin();it3!=it3E;++it3) {
    std::map<SoMaterial*,double>::const_iterator it4(it3->first.begin()),it4E(it3->first.end());
    for (;it4!=it4E;++it4) {
      if (mat==it4->first) {
	setMaterialFieldsAsAverageOfMatList(it3->second,it3->first);
	break;
      }
    }
  }

}

//____________________________________________________________________
void VP1SoMaterialMixer::Imp::setMaterialFieldsAsAverageOfMatList(SoMaterial*mat, std::set<SoMaterial*> matlist )
{
  float ambient_r(0.0f), ambient_g(0.0f), ambient_b(0.0f);
  float diffuse_r(0.0f), diffuse_g(0.0f), diffuse_b(0.0f);
  float specular_r(0.0f), specular_g(0.0f), specular_b(0.0f);
  float emissive_r(0.0f), emissive_g(0.0f), emissive_b(0.0f);
  float shininess(0.0f), transparency(0.0f);
  float r,g,b;
  std::set<SoMaterial*>::const_iterator it(matlist.begin()), itE(matlist.end());
  for (;it!=itE;++it) {
    (*it)->ambientColor[0].getValue(r,g,b); ambient_r += r; ambient_g += g; ambient_b += b;
    (*it)->diffuseColor[0].getValue(r,g,b); diffuse_r += r; diffuse_g += g; diffuse_b += b;
    (*it)->specularColor[0].getValue(r,g,b); specular_r += r; specular_g += g; specular_b += b;
    (*it)->emissiveColor[0].getValue(r,g,b); emissive_r += r; emissive_g += g; emissive_b += b;
    shininess += (*it)->shininess[0];
    transparency += (*it)->transparency[0];
  }

  float n = 1.0f/matlist.size();
  bool save = mat->enableNotify(false);
  mat->ambientColor.setValue(norm(n*ambient_r),norm(n*ambient_g),norm(n*ambient_b));
  mat->diffuseColor.setValue(norm(n*diffuse_r),norm(n*diffuse_g),norm(n*diffuse_b));
  mat->specularColor.setValue(norm(n*specular_r),norm(n*specular_g),norm(n*specular_b));
  mat->emissiveColor.setValue(norm(n*emissive_r),norm(n*emissive_g),norm(n*emissive_b));
  mat->shininess.setValue(norm(n*shininess));
  mat->transparency.setValue(norm(n*transparency));
  if (save) {
    mat->enableNotify(true);
    mat->touch();
    if (theclass->verbose())
      theclass->messageVerbose("Material ("+str(mat)+") updated and touched");
  } else {
    if (theclass->verbose())
      theclass->messageVerbose("Material ("+str(mat)+") updated but notifications were off");
  }
}

//____________________________________________________________________
void VP1SoMaterialMixer::Imp::setMaterialFieldsAsAverageOfMatList(SoMaterial*mat, std::map<SoMaterial*,double> matlist )
{
  float w, totweight(0.0f);
  float ambient_r(0.0f), ambient_g(0.0f), ambient_b(0.0f);
  float diffuse_r(0.0f), diffuse_g(0.0f), diffuse_b(0.0f);
  float specular_r(0.0f), specular_g(0.0f), specular_b(0.0f);
  float emissive_r(0.0f), emissive_g(0.0f), emissive_b(0.0f);
  float shininess(0.0f), transparency(0.0f);
  float r,g,b;
  std::map<SoMaterial*,double>::const_iterator it(matlist.begin()), itE(matlist.end());
  SoMaterial * m;
  for (;it!=itE;++it) {
    m = it->first;
    w = it->second;
    totweight += w;
    m->ambientColor[0].getValue(r,g,b); ambient_r += w*r; ambient_g += w*g; ambient_b += w*b;
    m->diffuseColor[0].getValue(r,g,b); diffuse_r += w*r; diffuse_g += w*g; diffuse_b += w*b;
    m->specularColor[0].getValue(r,g,b); specular_r += w*r; specular_g += w*g; specular_b += w*b;
    m->emissiveColor[0].getValue(r,g,b); emissive_r += w*r; emissive_g += w*g; emissive_b += w*b;
    shininess += m->shininess[0]*w;
    transparency += m->transparency[0]*w;
  }

  float n = 1.0f/totweight;
  bool save = mat->enableNotify(false);
  mat->ambientColor.setValue(norm(n*ambient_r),norm(n*ambient_g),norm(n*ambient_b));
  mat->diffuseColor.setValue(norm(n*diffuse_r),norm(n*diffuse_g),norm(n*diffuse_b));
  mat->specularColor.setValue(norm(n*specular_r),norm(n*specular_g),norm(n*specular_b));
  mat->emissiveColor.setValue(norm(n*emissive_r),norm(n*emissive_g),norm(n*emissive_b));
  mat->shininess.setValue(norm(n*shininess));
  mat->transparency.setValue(norm(n*transparency));
  if (save) {
    mat->enableNotify(true);
    mat->touch();
    if (theclass->verbose())
      theclass->messageVerbose("Material ("+str(mat)+") updated and touched");
  } else {
    if (theclass->verbose())
      theclass->messageVerbose("Material ("+str(mat)+") updated but notifications were off");
  }
}

//____________________________________________________________________
SoMaterial * VP1SoMaterialMixer::getMixedMaterial(const std::set<SoMaterial*>& matlist) {
  //Check incoming materials!
  bool error(false);
  std::set<SoMaterial*>::const_iterator it(matlist.begin()), itE(matlist.end());
  for (;it!=itE;++it) {
    if (!m_d->inputMaterialValid(*it)) {
     error=true;
     break;
   }
  }
  if (matlist.empty()) {
    VP1Msg::message("ERROR: asked to handle empty material list.");
    return m_d->defaultMaterial();
  }
  if (error) {
    VP1Msg::message("ERROR: asked to handle invalid material list.");
    return m_d->defaultMaterial();
  }

  //Someone might be silly enough to call with just one material:
  if (matlist.size()==1)
    return *(matlist.begin());

  //Do we already have a mixed material for this list:
  std::map<std::set<SoMaterial*>,SoMaterial*>::const_iterator it2 = m_d->matlists2mixedmats.find(matlist);
  if (it2!=m_d->matlists2mixedmats.end())
    return it2->second;

  //Create new mixed material:
  SoMaterial * mixmat = new SoMaterial;
  m_d->setMaterialFieldsAsAverageOfMatList(mixmat,matlist);

  //Set up monitoring, add to maps and refcount materials:
  std::set<SoMaterial*>::const_iterator it3(matlist.begin()), it3E(matlist.end());
  for (;it3!=it3E;++it3) {
    m_d->monitorMaterial(*it3);
    (*it3)->ref();//since we keep the pointer.
  }
  mixmat->ref();
  m_d->matlists2mixedmats[matlist] = mixmat;

  return mixmat;
}

//____________________________________________________________________
SoMaterial * VP1SoMaterialMixer::getMixedMaterial(const std::map<SoMaterial*,double>& matlist)
{
  //Check incoming materials!
  bool error(false);
  std::map<SoMaterial*,double>::const_iterator it(matlist.begin()), itE(matlist.end());
  for (;it!=itE;++it) {
    if (!m_d->inputMaterialValid(it->first)||it->second<=0.0) {
     error=true;
     break;
   }
  }
  if (matlist.empty()) {
    VP1Msg::message("ERROR: asked to handle empty material list.");
    return m_d->defaultMaterial();
  }
  if (error) {
    VP1Msg::message("ERROR: asked to handle invalid material list.");
    return m_d->defaultMaterial();
  }

  //Someone might be silly enough to call with just one material:
  if (matlist.size()==1)
    return matlist.begin()->first;

  //Do we already have a mixed material for this list:
  std::map<std::map<SoMaterial*,double>,SoMaterial*>::const_iterator it2 = m_d->matlists2mixedmats_weighted.find(matlist);
  if (it2!=m_d->matlists2mixedmats_weighted.end())
    return it2->second;

  //Create new mixed material:
  SoMaterial * mixmat = new SoMaterial;
  m_d->setMaterialFieldsAsAverageOfMatList(mixmat,matlist);

  //Set up monitoring, add to maps and refcount materials:
  std::map<SoMaterial*,double>::const_iterator it3(matlist.begin()), it3E(matlist.end());
  for (;it3!=it3E;++it3) {
    m_d->monitorMaterial(it3->first);
    it3->first->ref();//since we keep the pointer.
  }
  mixmat->ref();
  m_d->matlists2mixedmats_weighted[matlist] = mixmat;

  return mixmat;
}

//____________________________________________________________________
SoMaterial * VP1SoMaterialMixer::getMixedMaterial(SoMaterial*m1,SoMaterial*m2)
{
  std::set<SoMaterial*> matlist;
  matlist.insert(m1);
  matlist.insert(m2);
  return getMixedMaterial(matlist);
}

//____________________________________________________________________
SoMaterial * VP1SoMaterialMixer::getMixedMaterial( SoMaterial* mat1, const double& weight1,
						   SoMaterial* mat2, const double& weight2 )
{
  std::map<SoMaterial*,double> matlist;
  matlist[mat1] = weight1;
  matlist[mat2] = weight2;
  return getMixedMaterial(matlist);
}
