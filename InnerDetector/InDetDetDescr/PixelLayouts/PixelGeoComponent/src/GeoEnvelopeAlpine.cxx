/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelGeoComponent/GeoEnvelopeAlpine.h"
// #include "PixelGeoComponent/GeoBarrel.h"
// #include "PixelGeoComponent/GeoEndcap.h"
#include "PixelGeoComponent/GeoBarrelAlpine.h"
#include "PixelGeoComponent/GeoEndcapAlpine.h"
#include "PixelGeoComponent/GeoLayerAlpine.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

InDet::GeoEnvelopeAlpine::GeoEnvelopeAlpine( GeoFullPhysVol* vol):GeoComponent(vol)
{ /*m_barrel=0; m_endcaps.clear();*/
  m_barrelAlpine=0; m_negEndcapAlpine=0; m_posEndcapAlpine=0;
}
InDet::GeoEnvelopeAlpine::GeoEnvelopeAlpine( GeoPhysVol* vol):GeoComponent(vol)
{ /*m_barrel=0; m_endcaps.clear();*/
  m_barrelAlpine=0; m_negEndcapAlpine=0; m_posEndcapAlpine=0;
}
InDet::GeoEnvelopeAlpine::GeoEnvelopeAlpine(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  GeoComponent(vol, seqNum, trf, name, bFlag)
{ /*m_barrel=0; m_endcaps.clear();*/
  m_barrelAlpine=0; m_negEndcapAlpine=0; m_posEndcapAlpine=0;
}
InDet::GeoEnvelopeAlpine::GeoEnvelopeAlpine(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  GeoComponent(vol, seqNum, trf, name, bFlag)
{ /*m_barrel=0; m_endcaps.clear();*/
  m_barrelAlpine=0; m_negEndcapAlpine=0; m_posEndcapAlpine=0;
}


const InDet::GeoComponent& InDet::GeoEnvelopeAlpine::activeComponent( int i) const 
{
  if (i == 0 && m_barrelAlpine != 0) return *m_barrelAlpine;
  else if(i==1)
    return *m_posEndcapAlpine;
  else
    return *m_negEndcapAlpine;
}

const std::vector<InDet::GeoComponent*> InDet::GeoEnvelopeAlpine::components() const
{
  std::vector<InDet::GeoComponent*> vecRes;

  if(m_barrelAlpine)
    vecRes.push_back(dynamic_cast<InDet::GeoComponent*>(m_barrelAlpine));
  if(m_negEndcapAlpine)
    vecRes.push_back(dynamic_cast<InDet::GeoComponent*>(m_negEndcapAlpine));
  if(m_posEndcapAlpine)
    vecRes.push_back(dynamic_cast<InDet::GeoComponent*>(m_posEndcapAlpine));
  
  return vecRes;
}


// void InDet::GeoEnvelopeAlpine::placeBarrel( GeoBarrel& /*barrel*/, const HepGeom::Transform3D& /*transform*/, int /*number*/)
// {

//   //FIXME : to be developed

// //   //Insert new module 
// //   GeoBarrel *moduleNew=barrel.copyandclone();
// //   moduleNew->setTransform(transform);
// //   m_barrel=moduleNew;

//   //FIXME
//   //  placeComponent(*moduleNew,transform,number);
  
// }

void InDet::GeoEnvelopeAlpine::placeBarrelAlpine( GeoBarrelAlpine& barrel, const HepGeom::Transform3D& transform, int /*number*/)
{

  //Insert new module 
  GeoBarrelAlpine *moduleNew=barrel.place(transform,0);
  m_barrelAlpine=moduleNew;
  connectComponent(*moduleNew);

  //FIXME
  //  placeComponent(*moduleNew,transform,number);
  
}

void InDet::GeoEnvelopeAlpine::placeEndcapAlpine( GeoEndcapAlpine& endcap, const HepGeom::Transform3D& transform, int /*number*/, int iSide)
{

  if(iSide<0)
    {
      //Insert new negative side endcap
      GeoEndcapAlpine *moduleNew=endcap.place(transform,iSide);
      m_negEndcapAlpine=moduleNew;
      connectComponent(*moduleNew);
      return;
    }

  //Insert new posative side endcap
  GeoEndcapAlpine *moduleNew=endcap.place(transform,iSide);
  m_posEndcapAlpine=moduleNew;
  connectComponent(*moduleNew);
  
}


std::vector<InDet::GeoEndcapAlpine*> InDet::GeoEnvelopeAlpine::endcapsAlpine() const
{

  std::vector<GeoEndcapAlpine*> vec;
  if(m_negEndcapAlpine)vec.push_back(m_negEndcapAlpine);
  if(m_posEndcapAlpine)vec.push_back(m_posEndcapAlpine);

  return vec;
}

// Get modules indices corresponding to sensors

// // Extract geometry made of sensors only
// InDet::GeoEnvelopeAlpine* InDet::GeoEnvelopeAlpine::extractSensorGeometry_Envelope() const
// {
  
//   //  InDet::GeoEnvelopeAlpine* newEnvelope = dynamic_cast<GeoEnvelopeAlpine*>(this->clone());

//   InDet::GeoEnvelopeAlpine* newEnvelope;

//   if(isFullPhysVol())
//     newEnvelope= new InDet::GeoEnvelopeAlpine( new GeoFullPhysVol(physVolume<GeoFullPhysVol>()->getLogVol()), m_number);
//   else
//     newEnvelope=new InDet::GeoEnvelopeAlpine( new GeoPhysVol(physVolume<GeoPhysVol>()->getLogVol()), m_number);


//   // Loop over component to extract readout geometry
//   const std::vector<InDet::GeoComponent*> vec=components();
//   if(vec.size()==0) return 0;

//   std::vector<GeoComponent*> compVec; compVec.clear();

//   for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) 
//     {
//       GeoComponent* component  = (*i);

//       const HepGeom::Transform3D trf=HepGeom::Transform3D();
//       compVec=component->extractSensorGeometry(trf,"");   //FIXME  : add component to global list

//       for (std::vector<GeoComponent*>::const_iterator j=compVec.begin(); j!=compVec.end(); j++) 
// 	{
// 	  GeoComponent* comp  = (*j);
// 	  newEnvelope->placeComponent(*comp,*comp->getTransform3D(),comp->sequentialNumber(),false);
// 	}
//     }

//   return newEnvelope;

// }

// // Simplify stave and endcap foam module geometry
// //     - stave are approximated by a G4tub object for each layer
// //     - each endcap foam module is approximated by a G4box object
// InDet::GeoEnvelopeAlpine* InDet::GeoEnvelopeAlpine::simplifyStaveGeometry(const std::string& /*compositeMatName*/,  InDetMaterialManager* /*matMgr*/ ) const
// {
  
//   //  InDet::GeoEnvelopeAlpine* newEnvelope = dynamic_cast<GeoEnvelopeAlpine*>(this->clone());

//   InDet::GeoEnvelopeAlpine* newEnvelope=this->copyandclone();

// //   // Loop over layers
// //   //    - create composite StaveCylinder replacing all staves
// //   //    - remove stavesupport GeoComponent
// //   //    - add StaveCylinder to layer
// //   for(int iLayer=0; iLayer<1; iLayer++)
// //     {
// //       std::cout<<"----------------------------------------------------------------------------------------"<<std::endl;
// //       std::cout<<"SIMPLIFY STAVE - layer "<<iLayer<<std::endl;
      
// //       std::ostringstream layerName, compositeMatName;
// //       layerName<<"Layer"<<iLayer;
// //       compositeMatName<<"StaveTube_L"<<iLayer;

// //       std::string staveSupportName="StaveSupport";

// //       // List materials
// //       std::cout << "---------------- Compute material volumes   NEW" << std::endl;
// //       std::map<std::string,double>materialMap =this->computeAllMaterialVolumes(staveSupportName,layerName.str());
// //       for (std::map<std::string,double>::iterator it=materialMap.begin() ; it != materialMap.end(); it++ )
// // 	std::cout<<(*it).first<<" "<<(*it).second<<"  /  ";
// //       std::cout<<std::endl;

// //       // Create composite material
// //       computeEquivalentMaterial(compositeMatName.str(),matMgr,materialMap);

// //       // Get tube dimensions
// //       double staveRmax=m_barrelAlpine->getModuleRmax("StaveSupport",iLayer);
// //       double moduleRmax=m_barrelAlpine->getModuleRmax("PixelModule",iLayer);

// //       std::cout<<"Tube min-max radii : "<<moduleRmax<<" "<<staveRmax<<std::endl;

// //       // Remove stave supports GeoComponents
// //       GeoLayerAlpine compLayer=m_barrelAlpine->activeComponent(iLayer);
// //       compLayer.removeAllStaves();
      
// //         // Buils layer sector component
// //   GeoPixelStaveAlpine sectorBuilder( &geoAccessor(), matMgr(), msgStream());
// //   InDet::GeoStaveAlpine *sector=sectorBuilder.build(ilayer);

// //     }

//   return newEnvelope;
// }


