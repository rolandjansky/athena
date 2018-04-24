/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoComponent/GeoComponentCnv.h"
#include "PixelGeoComponent/GeoComponent.h"
#include "PixelGeoComponent/GeoSimpleObject.h"

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"


InDet::GeoComponentCnv::GeoComponentCnv()
{}

InDet::GeoComponentCnv::~GeoComponentCnv()
{}

InDet::GeoSimpleObject* InDet::GeoComponentCnv::convertIntoGeoComponent(const GeoVPhysVol *vol) const
{

//   std::cout<<" --------------------------------------------------------------------------------------------------- "<<std::endl;
//   std::cout<<"InDet::GeoSimpleObject* InDet::GeoComponentCnv::convertIntoGeoComponent(const GeoVPhysVol *vol) const"<<std::endl;

  GeoComponent* newComp = browsePhysVolTree(vol);


  //  std::cout<<"--------------- final object -------------"<<std::endl;
  //  newComp->dump();

  GeoSimpleObject* newObj = new GeoSimpleObject(newComp);

  //  std::cout<<"--------------- final object -------------"<<std::endl;
  //   newObj->dump();

  return newObj;

}


InDet::GeoSimpleObject* InDet::GeoComponentCnv::convertIntoGeoComponent(const GeoVPhysVol *vol,  const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag)  const
{

//   std::cout<<" --------------------------------------------------------------------------------------------------- "<<std::endl;
//   std::cout<<"InDet::GeoSimpleObject* InDet::GeoComponentCnv::convertIntoGeoComponent(const GeoVPhysVol *vol) const"<<std::endl;

  GeoComponent* newComp = browsePhysVolTree(vol);


//   std::cout<<"--------------- final object -------------"<<std::endl;
//   newComp->dump();

  GeoSimpleObject* newObj = new GeoSimpleObject(newComp,trf,name,bFlag);
  //  newComp->UnRef();
  //  delete newComp;
//   std::cout<<"--------------- final object -------------"<<std::endl;
//   newObj->dump();

  return newObj;

}


InDet::GeoComponent* InDet::GeoComponentCnv::browsePhysVolTree(const GeoVPhysVol* vol, int iLevel) const
{
 
  int nbChildren=vol->getNChildVols();
  //  std::cout<<"BrowsePhysVolTree "<<vol.getLogVol()->getName()<<" "<<nbChildren<<"    level "<<iLevel<<std::endl;
  
  //   for(int iChild=0; iChild<nbChildren && iChild<8; iChild++)
  //     {
  //       const GeoVPhysVol* vol2 = &(*(vol.getChildVol(iChild)));
  //       std::cout<<"    - child "<<iChild<<" "<<vol2->getNChildVols()<<std::endl;
  //     }
  
  // for(int i=0; i<iLevel; i++)std::cout<<" "; std::cout<<"parent "<<vol.getLogVol()->getName()<<" "<<nbChildren<<std::endl;
  
  
  if(nbChildren==0)
    {
      GeoPhysVol* tmp = dynamic_cast<GeoPhysVol*>(const_cast<GeoVPhysVol*>(vol));
      if(tmp){
	GeoComponent* comp = new GeoComponent(tmp);
	return comp;
      }
      
      GeoFullPhysVol* tmp2 = dynamic_cast<GeoFullPhysVol*>(const_cast<GeoVPhysVol*>(vol));
      GeoComponent* comp = new GeoComponent(tmp2);
      return comp;
      
    }
  
  // GeoPhysVol *newVol = new GeoPhysVol(vol->getLogVol());
  
  GeoComponent* compLocal=0;
  GeoPhysVol* tmp = dynamic_cast<GeoPhysVol*>(const_cast<GeoVPhysVol*>(vol));
  if( tmp){
    GeoPhysVol *newVol = new GeoPhysVol(vol->getLogVol());
    compLocal = new GeoComponent(newVol);
  }
  else{
    GeoFullPhysVol *newVol = new GeoFullPhysVol(vol->getLogVol());
    compLocal = new GeoComponent(newVol);
  }
  
  std::string materialName = vol->getLogVol()->getMaterial()->getName();
  std::transform(materialName.begin(), materialName.end(), materialName.begin(), ::tolower);
  bool bNeutralMaterial=(materialName=="special::ether"||materialName=="std::air"||
			 materialName=="ether"||materialName=="air");
  std::cout<<"GeoComponentCnv : "<<vol->getLogVol()->getName()<<" "<<materialName<<" "<<bNeutralMaterial<<std::endl;
  
  for(int iChild=0; iChild<nbChildren; iChild++)
    {
      const GeoVPhysVol* vol2 = &(*(vol->getChildVol(iChild)));
      GeoComponent* compChild = browsePhysVolTree(vol2, iLevel+1);
      
      std::string name = vol->getLogVol()->getName();
      HepGeom::Transform3D trf = vol->getXToChildVol(iChild);
      
      //      for(int i=0; i<2*iLevel; i++)std::cout<<" "; std::cout<<"-> browse "<<name<<std::endl;

 
      if(bNeutralMaterial)
	compLocal->placeAsChild(*compChild, trf, 0);
      else {
	std::cout<<"    -> GeoComponentCnv : place bG4InsertFlag "<<vol2->getLogVol()->getName()<<std::endl;
	compLocal->placeAsChild_insert(*compChild, trf, 0);
      }

      //      compChild->UnRef();
      delete compChild;
    }
  return compLocal;
}  




