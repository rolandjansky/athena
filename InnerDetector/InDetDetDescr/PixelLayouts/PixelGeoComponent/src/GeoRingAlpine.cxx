/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoComponent/GeoComponentHelper.h"
#include "PixelGeoComponent/GeoRingAlpine.h"
#include "PixelGeoComponent/GeoDetModule.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"

InDet::GeoRingAlpine::GeoRingAlpine( GeoFullPhysVol* vol):GeoComponent(vol)
{ m_ringModules.clear(); m_ringSupport=0; }
InDet::GeoRingAlpine::GeoRingAlpine( GeoPhysVol* vol):GeoComponent(vol)
{ m_ringModules.clear(); m_ringSupport=0; }
InDet::GeoRingAlpine::GeoRingAlpine(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  GeoComponent(vol, seqNum, trf, name, bFlag)
{ m_ringModules.clear(); m_ringSupport=0; }

const std::vector<InDet::GeoComponent*> InDet::GeoRingAlpine::components() const
{
  std::vector<InDet::GeoComponent*> result; 
  int iSize=m_ringModules.size();
  if(m_ringSupport)iSize++;

  result.reserve( iSize);

  for (std::vector<GeoDetModule*>::const_iterator i=m_ringModules.begin(); i!=m_ringModules.end(); i++) {
    result.push_back((*i)); // implicit cast to base class
  }

  //  if(m_ringSupport)result.push_back(m_barrelStave);
  return result;
}

void InDet::GeoRingAlpine::placeRingModule( GeoDetModule& module, const HepGeom::Transform3D& transform, int number, int index, int eta)
{
  //First resize vector if necessary
  if((int)m_ringModules.size()<=index)
    {
      m_ringModules.resize(index+1,0);
    }

  GeoDetModule *compNew=module.place(transform, number,eta);
  m_ringModules[index]=compNew;
  connectComponent(*compNew);
}

void InDet::GeoRingAlpine::placeRingSupport( GeoComponent& support, const HepGeom::Transform3D& transform)
{
  // store ring support
  GeoComponent* compNew=support.place(transform, 0);
  m_ringSupport=compNew;
  connectComponent(*compNew);
}

InDet::GeoRingAlpine*  InDet::GeoRingAlpine::place( const HepGeom::Transform3D& transform, int number) const
{

  GeoRingAlpine* newComp = new InDet::GeoRingAlpine(this->physVolume<GeoPhysVol>(),number,transform,this->getNameTag(),this->getG4InsertFlag());

  // Clone private datas
  int iCmpt=0;
  for (std::vector<GeoDetModule*>::const_iterator it=m_ringModules.begin(); it!=m_ringModules.end(); it++) 
    newComp->placeRingModule(*(*it),(*it)->getTransform(),(*it)->getSeqNumber(),iCmpt++,(*it)->getEtaModuleId());

  if(m_ringSupport)
    newComp->placeRingSupport(*m_ringSupport,m_ringSupport->getTransform3D());
     
  std::map<std::string, std::string>::const_iterator itrN;  
  for(itrN = m_compositeMatNames.begin(); itrN != m_compositeMatNames.end(); ++itrN)
    newComp->addCompositeMatName( std::pair<std::string,std::string>((*itrN).first,(*itrN).second));

  std::map<std::string, const GeoMaterial*>::const_iterator itr;  
  for(itr = m_compositeMat.begin(); itr != m_compositeMat.end(); ++itr)
    newComp->addCompositeMat( std::pair<std::string,const GeoMaterial *>((*itr).first,(*itr).second));

  return newComp;

}


void InDet::GeoRingAlpine::computeRingEquivalentCompositeMaterials(InDetMaterialManager* matMgr, GeoComponent *comp, std::string matName)
{
  std::vector<std::string> ignoreVolume;
  ignoreVolume.push_back("Endblock");       // not taken into account
  ignoreVolume.push_back("RingSupport");   // because made of Ether

  std::cout<<"Compute equivalent composite material : "<<matName<<std::endl;

  bool bMaterialFound=false;
  try{
    const GeoMaterial *matComposite=matMgr->getMaterial(matName);
    if(matComposite) bMaterialFound=true;
  }
  catch(...){}
  
  if(!bMaterialFound)
    {
      std::cout<<"Compute equivalent material : "<<matName<<std::endl;
      GeoComponentHelper helper(*comp);
      matMgr->addMaterial( helper.computeEquivalentMaterial(matMgr,matName,ignoreVolume));
      //      matMgr->addMaterial( comp->computeEquivalentMaterial(matMgr,matName,ignoreVolume));
      std::cout<<"Compute equivalent material : "<<matName<<"  -------------- stored"<<std::endl;
    }
}

void InDet::GeoRingAlpine::computeStaveEquivalentCompositeMaterials_combination(InDetMaterialManager* matMgr, GeoComponent *comp, std::string matName,
										std::string name1, std::string name2, double prop1, double prop2)
{
  std::vector<std::string> ignoreVolume;
  ignoreVolume.push_back("Endblock");       // not taken into account
  ignoreVolume.push_back("StaveSupport");   // because made of Ether

  std::cout<<"Compute equivalent composite material - combination : "<<matName<<std::endl;

  bool bMaterialFound=false;
  try{
    const GeoMaterial *matComposite=matMgr->getMaterial(matName);
    if(matComposite) bMaterialFound=true;
  }
  catch(...){}
  
  if(!bMaterialFound)
    {
      std::cout<<"Compute equivalent material : "<<matName<<std::endl;
      const GeoMaterial* mat1=matMgr->getMaterial(name1);
      const GeoMaterial* mat2=matMgr->getMaterial(name2);
      GeoComponentHelper helper(*comp);
      matMgr->addMaterial( helper.combineCompositeMaterial(matMgr,matName,
							  mat1, mat2, prop1, prop2));
      //      matMgr->addMaterial( comp->combineCompositeMaterial(matMgr,matName,
      //							  mat1, mat2, prop1, prop2));
      std::cout<<"Compute equivalent material : "<<matName<<"  -------------- stored"<<std::endl;
    }
}


void InDet::GeoRingAlpine::computeRingEquivalentCompositeMaterials_all(InDetMaterialManager* matMgr, std::string suffix)
{

  // For each material : check if it s already defined in manager, if not build composite materia

  m_compositeMatNames.insert( std::pair<std::string,const std::string>("ringModuleComposite","ringModuleComposite"+suffix));
  m_compositeMatNames.insert( std::pair<std::string,const std::string>("ringModuleAndRingComposite","ringModuleAndRingComposite"+suffix));
  m_compositeMatNames.insert( std::pair<std::string,const std::string>("ringSupportComposite","ringSupportComposite"+suffix));

  // Module material
  std::string matName=m_compositeMatNames["ringModuleComposite"];   //ringModuleMaterial_name;
  computeRingEquivalentCompositeMaterials(matMgr,m_ringModules[0],matName);
  const GeoMaterial *newMat1 = matMgr->getMaterial(matName);
  m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat1));

  // Ring support material
  matName=m_compositeMatNames["ringSupportComposite"];    //ringSupportMaterial_name;
  computeRingEquivalentCompositeMaterials(matMgr,m_ringSupport,matName);
  const GeoMaterial *newMat2 = matMgr->getMaterial(matName);
  m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat2));

  // Ring support surface
  const GeoLogVol *logVol=m_ringSupport->physVolume<GeoPhysVol>()->getLogVol();
  const GeoShape* bmShape = logVol->getShape();
  const GeoTube* bmTube = dynamic_cast< const GeoTube*>( bmShape);
  double outerRadius = bmTube->getRMax();
  double innerRadius = bmTube->getRMin();
  double ringThick = 2.*bmTube->getZHalfLength();
  double ringVolume=ringThick*3.141592*(outerRadius*outerRadius-innerRadius*innerRadius);

  // Module surface
  GeoComponentHelper compHelper(*(m_ringModules[0]));
  double width=2.*compHelper.getHalfSizeAlongYAxis();
  double length=2.*compHelper.getHalfSizeAlongZAxis();
  double thick=2.*compHelper.getHalfSizeAlongYAxis();
  double modVolume=width*length*thick;
  double modRmax=compHelper.getRmax(HepGeom::Transform3D());
  double modRmin=compHelper.getRmin(HepGeom::Transform3D());

//   double width=2.*m_ringModules[0]->getHalfSizeAlongYAxis();
//   double length=2.*m_ringModules[0]->getHalfSizeAlongZAxis();
//   double thick=2.*m_ringModules[0]->getHalfSizeAlongYAxis();
//   double modVolume=width*length*thick;
//   double modRmax=m_ringModules[0]->getRmax(HepGeom::Transform3D());
//   double modRmin=m_ringModules[0]->getRmin(HepGeom::Transform3D());

  double rMax=modRmax;
  double rMin=(modRmin>innerRadius)?modRmin:innerRadius;
  double ringVolumeBehindModules = ringThick*3.13159*(rMax*rMax-rMin*rMin);
  double ringVolumePerModule = ringVolumeBehindModules/(double)(m_ringModules.size());

  std::cout<<"Endcap ring composite material : "<<ringVolume<<" "<<ringVolumePerModule<<std::endl;

  // combine module and ring material for tracking geometry
  matName=m_compositeMatNames["ringModuleAndRingComposite"];     //ringModuleAndSupportMaterial_name;
  computeRingEquivalentCompositeMaterials(matMgr,m_ringModules[0],matName);
  computeStaveEquivalentCompositeMaterials_combination(matMgr, m_ringSupport, matName,
						       m_compositeMatNames["ringModuleComposite"],
						       m_compositeMatNames["ringSupportComposite"],
						       // ringModuleMaterial_name, ringSupportMaterial_name,
						       modVolume,ringVolumePerModule);
  const GeoMaterial *newMat3 = matMgr->getMaterial(matName);
  m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat3));
}

const GeoMaterial* InDet::GeoRingAlpine::findCompositeMaterial(std::string matName) const
{
  std::map<std::string, const GeoMaterial*>::const_iterator itr;
 
  std::string localMatName=m_compositeMatNames.find(matName)->second;
  for(itr = m_compositeMat.begin(); itr != m_compositeMat.end(); ++itr)
    {
       if((*itr).first==localMatName) return (*itr).second;
    }
  return 0;
}

const GeoMaterial* InDet::GeoRingAlpine::getRingModuleAndSupportMaterial() const 
{ return findCompositeMaterial("ringModuleAndRingComposite"); }

const GeoMaterial* InDet::GeoRingAlpine::getRingSupportMaterial() const 
{ return findCompositeMaterial("ringSupportComposite"); }


const HepGeom::Transform3D& InDet::GeoRingAlpine::getRingModuleTransform3D(int iModule) const
{
  return m_ringModules[iModule]->getTransform3D();
}

int InDet::GeoRingAlpine::getModuleType( int iModule) const
{
  return m_ringModules[iModule]->getModuleType();
}

const GeoFullPhysVol* InDet::GeoRingAlpine::getSensorFullPhysVolume( int iModule, int iSensor) const
{
  return m_ringModules[iModule]->getSensorFullPhysVolume(iSensor,0);
}

