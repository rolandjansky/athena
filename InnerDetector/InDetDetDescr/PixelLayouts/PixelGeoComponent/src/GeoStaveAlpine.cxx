/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelGeoComponent/GeoComponentHelper.h"
#include "PixelGeoComponent/GeoStaveAlpine.h"
#include "PixelGeoComponent/GeoDetModule.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"


InDet::GeoStaveAlpine::GeoStaveAlpine( GeoFullPhysVol* vol):GeoComponent(vol)
      { m_negEcStave=0; m_posEcStave=0; m_barrelStave=0; 
	m_barrelModules.clear(); m_negEcModules.clear(); m_posEcModules.clear();
	m_negFoamModule.clear(); m_posFoamModule.clear();
      }

InDet::GeoStaveAlpine::GeoStaveAlpine( GeoPhysVol* vol):GeoComponent(vol)
      { m_negEcStave=0; m_posEcStave=0; m_barrelStave=0; 
	m_barrelModules.clear(); m_negEcModules.clear(); m_posEcModules.clear();
	m_negFoamModule.clear(); m_posFoamModule.clear();
      }

InDet::GeoStaveAlpine::GeoStaveAlpine(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  GeoComponent(vol, seqNum, trf, name, bFlag)
      { m_negEcStave=0; m_posEcStave=0; m_barrelStave=0; 
	m_barrelModules.clear(); m_negEcModules.clear(); m_posEcModules.clear();
	m_negFoamModule.clear(); m_posFoamModule.clear();
      }

InDet::GeoStaveAlpine::GeoStaveAlpine(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  GeoComponent(vol, seqNum, trf, name, bFlag)
      { m_negEcStave=0; m_posEcStave=0; m_barrelStave=0; 
	m_barrelModules.clear(); m_negEcModules.clear(); m_posEcModules.clear();
	m_negFoamModule.clear(); m_posFoamModule.clear();
      }


const std::vector<InDet::GeoComponent*> InDet::GeoStaveAlpine::components() const
{
  std::vector<InDet::GeoComponent*> result; 
  int iSize=m_barrelModules.size()+m_negEcModules.size()+m_posEcModules.size();

  if(m_barrelStave)iSize++;
  if(m_negEcStave)iSize++;
  if(m_posEcStave)iSize++;
  iSize+=m_negFoamModule.size()+m_posFoamModule.size();
  result.reserve( iSize);

  for (std::vector<GeoDetModule*>::const_iterator i=m_barrelModules.begin(); i!=m_barrelModules.end(); i++) {
    result.push_back((*i)); // implicit cast to base class
  }
  for (std::vector<GeoDetModule*>::const_iterator i=m_negEcModules.begin(); i!=m_negEcModules.end(); i++) {
    result.push_back((*i)); // implicit cast to base class
  }
  for (std::vector<GeoDetModule*>::const_iterator i=m_posEcModules.begin(); i!=m_posEcModules.end(); i++) {
    result.push_back((*i)); // implicit cast to base class
  }
  for (std::vector<GeoComponent*>::const_iterator i=m_negFoamModule.begin(); i!=m_negFoamModule.end(); i++) {
    result.push_back((*i)); // implicit cast to base class
  }
  for (std::vector<GeoComponent*>::const_iterator i=m_posFoamModule.begin(); i!=m_posFoamModule.end(); i++) {
    result.push_back((*i)); // implicit cast to base class
  }
  if(m_barrelStave)result.push_back(m_barrelStave);
  if(m_negEcStave)result.push_back(m_negEcStave);
  if(m_posEcStave)result.push_back(m_posEcStave);

  return result;
}

void InDet::GeoStaveAlpine::placeBarrelModule( GeoDetModule& module, const HepGeom::Transform3D& transform, int number, int index, int eta)
{
  //First resize vector if necessary
  if((int)m_barrelModules.size()<=index)
    {
      m_barrelModules.resize(index+1,0);
    }


  GeoDetModule *compNew=module.place(transform, number, eta);
  m_barrelModules[index]=compNew;
  connectComponent(*compNew);

}

void InDet::GeoStaveAlpine::placeEndcapModule( GeoDetModule& module, const HepGeom::Transform3D& transform, int number, int side, int index, int eta)
{
  //Negative endcap side
  if(side<0)
    {
      //First resize vector if necessary
      if((int)m_negEcModules.size()<=index) m_negEcModules.resize(index+1,0);
      
      GeoDetModule *compNew=module.place(transform, number, eta);
      m_negEcModules[index]=compNew;
      connectComponent(*compNew);

      return;
    }

  //First resize vector if necessary
  if((int)m_posEcModules.size()<=index) m_posEcModules.resize(index+1,0);

  GeoDetModule *compNew=module.place(transform, number,eta);
  m_posEcModules[index]=compNew;
  connectComponent(*compNew);

}

void InDet::GeoStaveAlpine::placeFoamModule( GeoComponent& foam, const HepGeom::Transform3D& transform, int number, int side, int index)
{
  //Negative endcap side
  if(side<0)
    {
      //First resize vector if necessary
      if((int)m_negFoamModule.size()<=index) m_negFoamModule.resize(index+1,0);
      
      //Insert new foam 
      GeoComponent *foamNew=foam.place(transform, number);
      m_negFoamModule[index]=foamNew;
      connectComponent(*foamNew);

      return;
    }

  //First resize vector if necessary
  if((int)m_posFoamModule.size()<=index) m_posFoamModule.resize(index+1,0);
  
  //Insert new foam 
  GeoComponent *foamNew=foam.place(transform,number);
  m_posFoamModule[index]=foamNew;
  connectComponent(*foamNew);

}


void InDet::GeoStaveAlpine::placeBarrelStave( GeoComponent& comp, const HepGeom::Transform3D& transform, int number)
{
  GeoComponent *compNew=comp.place(transform, number);
  m_barrelStave=compNew;
  connectComponent(*compNew);

}


void InDet::GeoStaveAlpine::placeEndcapStave( GeoComponent& comp, const HepGeom::Transform3D& transform, int number, int side)
{
  //Negative endcap side
  if(side<0)
    {
      GeoComponent *compNew=comp.place(transform, number);
      m_negEcStave=compNew;
      connectComponent(*compNew);
      return;
    }

  //Positive endcap side
  GeoComponent *compNew=comp.place(transform, number);
  m_posEcStave=compNew;
  connectComponent(*compNew);
}

InDet::GeoStaveAlpine*  InDet::GeoStaveAlpine::place( const HepGeom::Transform3D& transform, int number) const
{

  GeoStaveAlpine* newComp;
  if(isPhysVol())
    newComp = new InDet::GeoStaveAlpine(this->physVolume<GeoPhysVol>(),number,transform,this->getNameTag(),this->getG4InsertFlag());
  else
    newComp = new InDet::GeoStaveAlpine(this->physVolume<GeoFullPhysVol>(),number,transform,this->getNameTag(),this->getG4InsertFlag());

  // Clone private datas
  int iCmpt=0;
  for (std::vector<GeoDetModule*>::const_iterator it=m_barrelModules.begin(); it!=m_barrelModules.end(); it++) 
    newComp->placeBarrelModule(*(*it),(*it)->getTransform(),(*it)->getSeqNumber(),iCmpt++,(*it)->getEtaModuleId());
  iCmpt=0;
  for (std::vector<GeoDetModule*>::const_iterator it=m_negEcModules.begin(); it!=m_negEcModules.end(); it++) 
    newComp->placeEndcapModule(*(*it),(*it)->getTransform(),(*it)->getSeqNumber(),-1,iCmpt++,(*it)->getEtaModuleId());
  iCmpt=0;
  for (std::vector<GeoDetModule*>::const_iterator it=m_posEcModules.begin(); it!=m_posEcModules.end(); it++) 
    newComp->placeEndcapModule(*(*it),(*it)->getTransform(),(*it)->getSeqNumber(),1,iCmpt++,(*it)->getEtaModuleId());
  iCmpt=0;
  for (std::vector<GeoComponent*>::const_iterator it=m_negFoamModule.begin(); it!=m_negFoamModule.end(); it++) 
    newComp->placeFoamModule(*(*it),(*it)->getTransform(),(*it)->getSeqNumber(),-1,iCmpt++);
  iCmpt=0;
  for (std::vector<GeoComponent*>::const_iterator it=m_posFoamModule.begin(); it!=m_posFoamModule.end(); it++) 
    newComp->placeFoamModule(*(*it),(*it)->getTransform(),(*it)->getSeqNumber(),1,iCmpt++);

  if(m_barrelStave)
    newComp->placeBarrelStave(*m_barrelStave,m_barrelStave->getTransform3D(),m_barrelStave->getSeqNumber());
  if(m_negEcStave)
    newComp->placeEndcapStave(*m_negEcStave,m_negEcStave->getTransform3D(),m_negEcStave->getSeqNumber(),-1);
  if(m_posEcStave)
    newComp->placeEndcapStave(*m_posEcStave,m_posEcStave->getTransform3D(),m_posEcStave->getSeqNumber(),+1);

  std::map<std::string, std::string>::const_iterator itrN;  
  for(itrN = m_compositeMatNames.begin(); itrN != m_compositeMatNames.end(); ++itrN)
    newComp->addCompositeMatName( std::pair<std::string,std::string>((*itrN).first,(*itrN).second));

  std::map<std::string, const GeoMaterial*>::const_iterator itr;  
  for(itr = m_compositeMat.begin(); itr != m_compositeMat.end(); ++itr)
    newComp->addCompositeMat( std::pair<std::string,const GeoMaterial *>((*itr).first,(*itr).second));

  return newComp;

}

/// Return barrel module length 
double InDet::GeoStaveAlpine::barrelModuleLength() const
{

  // In case no barrel module is defined
  if(m_barrelModules.size()==0)
    {
      std::cout<<"GeoStaveAlpine::barrelModuleLength   no barrel module defined"<<std::endl;
      return 0.0;
    }

  GeoComponentHelper compHelper(*(m_barrelModules[0]));
  return (2.0*compHelper.getHalfSizeAlongZAxis());
  //  return (2.0*m_barrelModules[0]->getHalfSizeAlongZAxis());

}

// Set stave support sequential number
void InDet::GeoStaveAlpine::setStaveSupportSequentialNumber(int /*number*/)
{

//   if(m_barrelStave) m_barrelStave->setSequentialNumber(number);     // FIXME :  check if placeBarrelStave did the job
//   if(m_negEcStave) m_negEcStave->setSequentialNumber(number);
//   if(m_posEcStave) m_posEcStave->setSequentialNumber(number);

}

void InDet::GeoStaveAlpine::removeStaveSupports()
{
  if(m_barrelStave) m_barrelStave->deleteAllChildComponents();
  if(m_negEcStave) m_negEcStave->deleteAllChildComponents();
  if(m_posEcStave) m_posEcStave->deleteAllChildComponents();

  m_barrelStave=0;
  m_negEcStave=0;
  m_posEcStave=0;

}

void InDet::GeoStaveAlpine::removeFoamModules()
{
  for (std::vector<GeoComponent*>::const_iterator i=m_posFoamModule.begin(); i!=m_posFoamModule.end(); i++)(*i)->deleteAllChildComponents();
  m_posFoamModule.erase(m_posFoamModule.begin(),m_posFoamModule.end());

  for (std::vector<GeoComponent*>::const_iterator i=m_negFoamModule.begin(); i!=m_negFoamModule.end(); i++)(*i)->deleteAllChildComponents();
  m_negFoamModule.erase(m_negFoamModule.begin(),m_negFoamModule.end());
}


void InDet::GeoStaveAlpine::computeStaveEquivalentCompositeMaterials(InDetMaterialManager* matMgr, GeoComponent *comp, std::string matName)
{
  std::vector<std::string> ignoreVolume;
  ignoreVolume.push_back("Endblock");       // not taken into account
  ignoreVolume.push_back("StaveSupport");   // because made of Ether

  std::cout<<"STAVE : Compute equivalent composite material : "<<matName<<" for "<<comp->getNameTag()<<std::endl;

  comp->dumpAddresses(0);

  bool bMaterialFound=false;
  try{
    const GeoMaterial *matComposite=matMgr->getMaterial(matName);
    if(matComposite) bMaterialFound=true;
  }
  catch(...){}
  
  if(!bMaterialFound)
    {
      std::cout<<"STAVE : Compute equivalent material : "<<matName<<std::endl;
      GeoComponentHelper helper(*comp);
      matMgr->addMaterial( helper.computeEquivalentMaterial(matMgr,matName,ignoreVolume));
      //      matMgr->addMaterial( comp->computeEquivalentMaterial(matMgr,matName,ignoreVolume));
      std::cout<<"Compute equivalent material : "<<matName<<"  -------------- stored"<<std::endl;
    }
}


void InDet::GeoStaveAlpine::computeStaveEquivalentCompositeMaterials_combination(InDetMaterialManager* matMgr, GeoComponent *comp, std::string matName,
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


void InDet::GeoStaveAlpine::computeStaveEquivalentCompositeMaterials_all(InDetMaterialManager* matMgr, std::string suffix)
{

  // For each material : check if it s already defined in manager, if not build composite materia

  m_compositeMatNames.insert( std::pair<std::string,const std::string>("staveBrlComposite","staveBrlComposite"+suffix));
  m_compositeMatNames.insert( std::pair<std::string,const std::string>("stavePosEcComposite","stavePosEcComposite"+suffix));
  m_compositeMatNames.insert( std::pair<std::string,const std::string>("staveNegEcComposite","staveNegEcComposite"+suffix));

  m_compositeMatNames.insert( std::pair<std::string,const std::string>("foamTrModuleComposite","foamTrModuleComposite"+suffix));
  m_compositeMatNames.insert( std::pair<std::string,const std::string>("foamModuleComposite","foamModuleComposite"+suffix));

  m_compositeMatNames.insert( std::pair<std::string,const std::string>("barrelTrModuleComposite","barrelTrModuleComposite"+suffix));
  m_compositeMatNames.insert( std::pair<std::string,const std::string>("barrelModuleComposite","barrelModuleComposite"+suffix));
  m_compositeMatNames.insert( std::pair<std::string,const std::string>("ecTrModuleComposite","ecTrModuleComposite"+suffix));
  m_compositeMatNames.insert( std::pair<std::string,const std::string>("ecModuleComposite","ecModuleComposite"+suffix));

  m_compositeMatNames.insert( std::pair<std::string,const std::string>("barrelModuleAndStaveComposite","barrelModuleAndStaveComposite"+suffix));
  m_compositeMatNames.insert( std::pair<std::string,const std::string>("barrelTransModuleAndStaveComposite","barrelTransModuleAndStaveComposite"+suffix));
  m_compositeMatNames.insert( std::pair<std::string,const std::string>("ecModuleAndFoamComposite","ecModuleAndFoamComposite"+suffix));
  m_compositeMatNames.insert( std::pair<std::string,const std::string>("ecTransModuleAndFoamComposite","ecTransModuleAndFoamComposite"+suffix));
  
  if(m_barrelStave)
    { std::string matName=m_compositeMatNames.find("staveBrlComposite")->second; //  barrelCompositeMaterial_name;
      computeStaveEquivalentCompositeMaterials(matMgr,m_barrelStave,matName);
      std::cout<<"STORE MAT INTO map "<<std::endl;
      const GeoMaterial *newMat = matMgr->getMaterial(matName);
      std::cout<<"STORE MAT INTO map ---- to map"<<std::endl;
      m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat));
    }

  if(m_posEcStave)
    { std::string matName=m_compositeMatNames.find("stavePosEcComposite")->second; //posEcCompositeMaterial_name;
      computeStaveEquivalentCompositeMaterials(matMgr,m_posEcStave,matName);
      std::cout<<"STORE MAT INTO map "<<std::endl;
      const GeoMaterial *newMat = matMgr->getMaterial(matName);
      std::cout<<"STORE MAT INTO map ---- to map"<<std::endl;
      m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat));
    }

  if(m_negEcStave)
    { std::string matName=m_compositeMatNames.find("staveNegEcComposite")->second; //negEcCompositeMaterial_name;
      computeStaveEquivalentCompositeMaterials(matMgr,m_negEcStave,matName);
      std::cout<<"STORE MAT INTO map "<<std::endl;
      const GeoMaterial *newMat = matMgr->getMaterial(matName);
      std::cout<<"STORE MAT INTO map ---- to map"<<std::endl;
      m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat));
    }
  
  if(m_posFoamModule.size()>0){
    std::string matName=m_compositeMatNames.find("foamTrModuleComposite")->second; //foamTransModuleMaterial_name;
    computeStaveEquivalentCompositeMaterials(matMgr,m_posFoamModule[0],matName);
    std::cout<<"STORE MAT INTO map "<<std::endl;
    const GeoMaterial *newMat = matMgr->getMaterial(matName);
    std::cout<<"STORE MAT INTO map ---- to map"<<std::endl;
    m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat));

    matName=m_compositeMatNames.find("foamModuleComposite")->second; //oamModuleMaterial_name;
    computeStaveEquivalentCompositeMaterials(matMgr,m_posFoamModule[1],matName);
    std::cout<<"STORE MAT INTO map "<<std::endl;
    const GeoMaterial *newMat2 = matMgr->getMaterial(matName);
    std::cout<<"STORE MAT INTO map ---- to map"<<std::endl;
    m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat2));
  }

  if(m_barrelModules.size()>0){
    std::string matName=m_compositeMatNames.find("barrelTrModuleComposite")->second; //barrelTransModuleMaterial_name;
    computeStaveEquivalentCompositeMaterials(matMgr,m_barrelModules[0],matName);
    std::cout<<"STORE MAT INTO map "<<std::endl;
    const GeoMaterial *newMat = matMgr->getMaterial(matName);
    std::cout<<"STORE MAT INTO map ---- to map"<<std::endl;
    m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat));

    matName=m_compositeMatNames.find("barrelModuleComposite")->second; //barrelModuleMaterial_name;
    computeStaveEquivalentCompositeMaterials(matMgr,m_barrelModules[1],matName);
    std::cout<<"STORE MAT INTO map "<<std::endl;
    const GeoMaterial *newMat2 = matMgr->getMaterial(matName);
    std::cout<<"STORE MAT INTO map ---- to map"<<std::endl;
    m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat2));
  }

  if(m_negEcModules.size()>0){
    std::string matName=m_compositeMatNames.find("ecTrModuleComposite")->second; //ecTransModuleMaterial_name;
    computeStaveEquivalentCompositeMaterials(matMgr,m_negEcModules[0],matName);
    std::cout<<"STORE MAT INTO map "<<std::endl;
    const GeoMaterial *newMat = matMgr->getMaterial(matName);
    std::cout<<"STORE MAT INTO map ---- to map"<<std::endl;
    m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat));

    matName=m_compositeMatNames.find("ecModuleComposite")->second; //ecModuleMaterial_name;
    computeStaveEquivalentCompositeMaterials(matMgr,m_negEcModules[1],matName);
    std::cout<<"STORE MAT INTO map "<<std::endl;
    const GeoMaterial *newMat2 = matMgr->getMaterial(matName);
    std::cout<<"STORE MAT INTO map ---- to map"<<std::endl;
    m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat2));
  }

   // Combine barrel module and stave
  std::vector<std::string> ignoreVolume;
  ignoreVolume.push_back("Endblock");       // not taken into account
  ignoreVolume.push_back("StaveSupport");   // because made of Ether

  std::string matName=m_compositeMatNames.find("barrelModuleAndStaveComposite")->second; //barrelModuleAndStaveMaterial_name;
  GeoComponentHelper helperStave(*m_barrelStave);
  double barrelStaveVolume=helperStave.computeComponentVolume(ignoreVolume);
  GeoComponentHelper helperMod1(*m_barrelModules[1]);
  double barrelModuleVolume=helperMod1.computeComponentVolume();
  //  double barrelStaveVolume=m_barrelStave->computeComponentVolume(ignoreVolume);
  //  double barrelModuleVolume=m_barrelModules[1]->computeComponentVolume();

  double staveLength=2.0*helperStave.getHalfSizeAlongZAxis();
  //  double staveLength=2.0*m_barrelStave->getHalfSizeAlongZAxis();

  double moduleLength=2.0*helperMod1.getHalfSizeAlongZAxis();
  //  double moduleLength=2.0*m_barrelModules[1]->getHalfSizeAlongZAxis();
  computeStaveEquivalentCompositeMaterials_combination(matMgr, m_barrelStave, matName,
						       m_compositeMatNames.find("staveBrlComposite")->second,
						       m_compositeMatNames.find("barrelModuleComposite")->second,
						       // barrelCompositeMaterial_name, barrelModuleMaterial_name, 
						       barrelStaveVolume*moduleLength/staveLength, barrelModuleVolume);
  const GeoMaterial *newMat1 = matMgr->getMaterial(matName);
  m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat1));
    
  matName=m_compositeMatNames.find("barrelTransModuleAndStaveComposite")->second; //barrelTransModuleAndStaveMaterial_name;
  GeoComponentHelper helperMod0(*(m_barrelModules[0]));
  double barrelTransModuleVolume=helperMod0.computeComponentVolume();
  moduleLength=2.0*helperMod0.getHalfSizeAlongZAxis();
  //  double barrelTransModuleVolume=m_barrelModules[0]->computeComponentVolume();
  //  moduleLength=2.0*m_barrelModules[0]->getHalfSizeAlongZAxis();

  computeStaveEquivalentCompositeMaterials_combination(matMgr, m_barrelStave, matName,
						       m_compositeMatNames.find("staveBrlComposite")->second,
						       m_compositeMatNames.find("barrelTrModuleComposite")->second,
						       // barrelCompositeMaterial_name, barrelTransModuleMaterial_name, 
						       barrelStaveVolume*moduleLength/staveLength*0.5, barrelTransModuleVolume);
  const GeoMaterial *newMat2 = matMgr->getMaterial(matName);
  m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat2));

  if(m_posEcModules.size()>0)
    {
      // Combine ec module and foam module
      std::string matName=m_compositeMatNames.find("ecModuleAndFoamComposite")->second; //ecModuleAndFoamModuleMaterial_name;
      GeoComponentHelper helperFoamMod1(*(m_posFoamModule[1]));
      GeoComponentHelper helperEcMod1(*(m_posEcModules[1]));
      double foamModuleVolume=helperFoamMod1.computeComponentVolume();
      double ecModuleVolume=helperEcMod1.computeComponentVolume();
      //      double foamModuleVolume=m_posFoamModule[1]->computeComponentVolume();
      //      double ecModuleVolume=m_posEcModules[1]->computeComponentVolume();
      computeStaveEquivalentCompositeMaterials_combination(matMgr, m_barrelStave, matName,
							   m_compositeMatNames.find("foamModuleComposite")->second,
							   m_compositeMatNames.find("ecModuleComposite")->second,
							   // foamModuleMaterial_name, ecModuleMaterial_name,
							   foamModuleVolume,ecModuleVolume);
      const GeoMaterial *newMat1 = matMgr->getMaterial(matName);
      m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat1));

      // Combine ec module and foam module
      matName=m_compositeMatNames.find("ecTransModuleAndFoamComposite")->second; //ecTransModuleAndFoamModuleMaterial_name;
      GeoComponentHelper helperFoamMod0(*(m_posFoamModule[0]));
      GeoComponentHelper helperEcMod0(*(m_posEcModules[0]));
      double foamTransModuleVolume=helperFoamMod0.computeComponentVolume();
      double ecTransModuleVolume=helperEcMod0.computeComponentVolume();
      //      double foamTransModuleVolume=m_posFoamModule[0]->computeComponentVolume();
      //      double ecTransModuleVolume=m_posEcModules[0]->computeComponentVolume();
      computeStaveEquivalentCompositeMaterials_combination(matMgr, m_barrelStave, matName,
							   m_compositeMatNames.find("foamTrModuleComposite")->second,
							   m_compositeMatNames.find("ecTrModuleComposite")->second,
							   //foamTransModuleMaterial_name, ecTransModuleMaterial_name,
							   foamTransModuleVolume,ecTransModuleVolume);
      const GeoMaterial *newMat2 = matMgr->getMaterial(matName);
      m_compositeMat.insert( std::pair<std::string,const GeoMaterial *>(matName,newMat2));
    }

}


const GeoMaterial* InDet::GeoStaveAlpine::findCompositeMaterial(std::string matName) const
{
  
//   std::cout<<"GeoStaveAlpine : findCompositeMaterial --"<<matName<<"--"<<std::endl;
//   std::map<std::string, std::string>::const_iterator itr0;
//   for(itr0 = m_compositeMatNames.begin(); itr0 != m_compositeMatNames.end(); ++itr0)
//     std::cout<<" -> "<<(*itr0).first<<"  "<<(*itr0).second<<std::endl;

  std::string localMatName=m_compositeMatNames.find(matName)->second;
  std::map<std::string, const GeoMaterial*>::const_iterator itr;
  for(itr = m_compositeMat.begin(); itr != m_compositeMat.end(); ++itr)
    {
      //      std::cout<<" -> "<<(*itr).first<<std::endl;
      if((*itr).first==localMatName) return (*itr).second;
    }
  return 0;
}

const GeoMaterial* InDet::GeoStaveAlpine::getBarrelModuleMaterial() const 
{ return findCompositeMaterial("barrelModuleComposite"); }
const GeoMaterial* InDet::GeoStaveAlpine::getBarrelTransModuleMaterial() const 
{ return findCompositeMaterial("barrelTrModuleComposite"); }
const GeoMaterial* InDet::GeoStaveAlpine::getEndcapModuleMaterial() const 
{ return findCompositeMaterial("ecModuleComposite"); }
const GeoMaterial* InDet::GeoStaveAlpine::getEndcapTransModuleMaterial() const 
{  return findCompositeMaterial("ecTrModuleComposite"); }

const GeoMaterial* InDet::GeoStaveAlpine::getBarrelStaveMaterial() const 
{ return findCompositeMaterial("staveBrlComposite"); }
const GeoMaterial* InDet::GeoStaveAlpine::getEndcapStaveMaterial(int iSide) const 
{ return (iSide<0)?findCompositeMaterial("staveNegEcComposite"):findCompositeMaterial("stavePosEcComposite"); }
const GeoMaterial* InDet::GeoStaveAlpine::getFoamModuleMaterial() const 
{ return findCompositeMaterial("foamModuleComposite"); }
const GeoMaterial* InDet::GeoStaveAlpine::getTransFoamModuleMaterial() const 
{ return findCompositeMaterial("foamTrModuleComposite"); }

const GeoMaterial* InDet::GeoStaveAlpine::getBarrelModuleAndStaveMaterial() const 
{ return findCompositeMaterial("barrelModuleAndStaveComposite"); }
const GeoMaterial* InDet::GeoStaveAlpine::getBarrelTransModuleAndStaveMaterial() const 
{  return findCompositeMaterial("barrelTransModuleAndStaveComposite"); 
}

const GeoMaterial* InDet::GeoStaveAlpine::getEcModuleAndFoamModuleMaterial() const 
{ return findCompositeMaterial("ecModuleAndFoamComposite"); }
const GeoMaterial* InDet::GeoStaveAlpine::getEcTransModuleAndFoamModuleMaterial() const 
{ return findCompositeMaterial("ecTransModuleAndFoamComposite"); }


int InDet::GeoStaveAlpine::getModuleType(int iSide,int iModule) const
{
  if(iSide==0) return m_barrelModules[iModule]->getModuleType();
  if(iSide==1) return m_posEcModules[iModule]->getModuleType();
  if(iSide==-1) return m_negEcModules[iModule]->getModuleType();
  return -1;
}

int InDet::GeoStaveAlpine::getModuleType(int iModule) const
{
  if(iModule<(int)m_negEcModules.size())
    return m_negEcModules[iModule]->getModuleType();
  else if (iModule<(int)(m_negEcModules.size()+m_barrelModules.size())){
    int idModule=iModule-m_negEcModules.size();
    return m_barrelModules[idModule]->getModuleType();
  }

  int idModule=iModule-(m_negEcModules.size()+m_barrelModules.size());
  return m_posEcModules[idModule]->getModuleType();
}

const GeoFullPhysVol* InDet::GeoStaveAlpine::getSensorFullPhysVolume(int iSide, int iModule, int iSensor) const 
{
  if(iSide==0) return m_barrelModules[iModule]->getSensorFullPhysVolume(iSensor,0);
  if(iSide==1) return m_posEcModules[iModule]->getSensorFullPhysVolume(iSensor,0);
  if(iSide==-1) return m_negEcModules[iModule]->getSensorFullPhysVolume(iSensor,0);
  return 0;
}

const GeoFullPhysVol* InDet::GeoStaveAlpine::getSensorFullPhysVolume(int iModule, int iSensor) const 
{
  if(iModule<(int)m_negEcModules.size())
    return m_negEcModules[iModule]->getSensorFullPhysVolume(iSensor,0);
  else if (iModule<(int)(m_negEcModules.size()+m_barrelModules.size())){
    int idModule=iModule-m_negEcModules.size();
    return m_barrelModules[idModule]->getSensorFullPhysVolume(iSensor,0);
  }

  int idModule=iModule-(m_negEcModules.size()+m_barrelModules.size());
  return m_posEcModules[idModule]->getSensorFullPhysVolume(iSensor,0);
}


double InDet::GeoStaveAlpine::getBarrelStaveWidth() const
{
  GeoComponentHelper compHelper(*(m_barrelStave));
  return (2.0*compHelper.getHalfSizeAlongXAxis());
  //  return (2.0*m_barrelStave->getHalfSizeAlongXAxis());
}

double InDet::GeoStaveAlpine::getBarrelStaveLength() const
{
  GeoComponentHelper compHelper(*(m_barrelStave));
  return (2.0*compHelper.getHalfSizeAlongZAxis());
  //  return (2.0*m_barrelStave->getHalfSizeAlongZAxis());
}

double InDet::GeoStaveAlpine::getEndcapStaveWidth(int iSide) const
{
  if(iSide<0) {
      GeoComponentHelper compHelper(*(m_negEcStave));
      return (2.0*compHelper.getHalfSizeAlongXAxis());
      //      return (2.0*m_negEcStave->getHalfSizeAlongXAxis());
    }
  GeoComponentHelper compHelper(*(m_posEcStave));
  return (2.0*compHelper.getHalfSizeAlongXAxis());
  //  return (2.0*m_posEcStave->getHalfSizeAlongXAxis());
}

double InDet::GeoStaveAlpine::getEndcapStaveLength(int iSide) const
{
  if(iSide<0) {
    GeoComponentHelper compHelper(*(m_negEcStave));
    return (2.0*compHelper.getHalfSizeAlongZAxis());
    //    return (2.0*m_negEcStave->getHalfSizeAlongZAxis());
  }
  GeoComponentHelper compHelper(*(m_posEcStave));
  return (2.0*compHelper.getHalfSizeAlongZAxis());
  //  return (2.0*m_posEcStave->getHalfSizeAlongZAxis());
}

const HepGeom::Transform3D& InDet::GeoStaveAlpine::getBarrelStaveTransform3D() const
//  /!\ this function returns the transform3D in the barrel module plane, not the real 3D one
{
  const HepGeom::Transform3D& trfStave = this->getTransform3D(); 
  const HepGeom::Transform3D& trfBarrelStave = HepGeom::Transform3D();   //m_barrelStave->getTransform3D(); 
  const HepGeom::Transform3D& trf=((trfStave)*(trfBarrelStave));
  return trf;

}

const HepGeom::Transform3D& InDet::GeoStaveAlpine::getEndcapStaveTransform3D(int iSide) const
//  /!\ this function returns the transform3D in the barrel module plane, not the real 3D one
{

  const HepGeom::Transform3D& trfStave = this->getTransform3D(); 
  HepGeom::Vector3D<double>  trans;
  if(iSide<0)
    trans=(m_negEcStave->getTransform3D()).getTranslation();
  else
    trans=(m_posEcStave->getTransform3D()).getTranslation();

  HepGeom::Rotate3D rotation;
  HepGeom::Transform3D trf= HepGeom::Transform3D(CLHEP::HepRotation(),HepGeom::Point3D<double> (trans.x(),0.,trans.z()));

  const HepGeom::Transform3D& trfGbl=(trfStave*trf);
  return trfGbl;

}

