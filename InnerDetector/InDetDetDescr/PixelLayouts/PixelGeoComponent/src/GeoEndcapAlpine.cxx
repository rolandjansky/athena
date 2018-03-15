/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoComponent/GeoComponentHelper.h"
#include "PixelGeoComponent/GeoEndcapAlpine.h"
#include "PixelGeoComponent/GeoRingAlpine.h"
/*#include "PixelGeoComponent/GeoRingSupportAlpine.h"*/

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include <sstream>

InDet::GeoEndcapAlpine::GeoEndcapAlpine( GeoFullPhysVol* vol):GeoComponent(vol)
{ m_rings.clear(); }
InDet::GeoEndcapAlpine::GeoEndcapAlpine( GeoPhysVol* vol):GeoComponent(vol)
{  m_rings.clear(); }
InDet::GeoEndcapAlpine::GeoEndcapAlpine(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  GeoComponent(vol, seqNum, trf, name, bFlag)
{  m_rings.clear(); }

const std::vector<InDet::GeoComponent*> InDet::GeoEndcapAlpine::components() const
{
  std::vector<InDet::GeoComponent*> result; 
  result.reserve( m_rings.size());
  for (std::vector<GeoRingAlpine*>::const_iterator i=m_rings.begin(); i!=m_rings.end(); i++) {
    result.push_back( dynamic_cast<InDet::GeoComponent*>(*i)); // implicit cast to base class
  }
//   for (std::vector<GeoRingSupportAlpine*>::const_iterator i=m_supports.begin(); i!=m_supports.end(); i++) {
//     result.push_back( dynamic_cast<InDet::GeoComponent*>(*i)); // implicit cast to base class
//   }
  return result;
}

void InDet::GeoEndcapAlpine::placeRing( GeoRingAlpine& ring, const HepGeom::Transform3D& transform, int number)
{
  //First resize vector if necessary
  if((int)m_rings.size()<=number)
      m_rings.resize(number+1,0);

  //Insert new module 
  GeoRingAlpine *ringNew=ring.place(transform,number);
  m_rings[number]=ringNew;
  connectComponent(*ringNew);
}

InDet::GeoEndcapAlpine*  InDet::GeoEndcapAlpine::place( const HepGeom::Transform3D& transform, int number) const
{

  GeoEndcapAlpine* newComp = new InDet::GeoEndcapAlpine(this->physVolume<GeoPhysVol>(),number,transform,this->getNameTag(),this->getG4InsertFlag());

  std::cout<<"place endcap alpine "<<std::endl;
  newComp->dump();

  int iCmpt=0;
  for (std::vector<GeoRingAlpine*>::const_iterator it=m_rings.begin(); it!=m_rings.end(); it++) 
    newComp->placeRing(*(*it),(*it)->getTransform(),iCmpt++);

//   for (int i=0; i<(int)m_supports.size(); i++)
//     newComp->placeSupport(*m_supports[i]->copyandclone(),*m_supports[i]->getTransform3D(),i);

  return newComp;

}

double InDet::GeoEndcapAlpine::radius() const
{

  if(numActiveComponents()==0) return 0;

  // Compute radii
  double radius=-1;
  bool bIdenticalRadii=true;

  // Get stave HepTransform
  const HepGeom::Transform3D& trf=getTransform3D();

  // Loop over rings 
  for(int iRing=0; iRing<numActiveComponents(); iRing++)
    {
      GeoRingAlpine* ring =m_rings[iRing];
      
      // Get all sensors from defined for barrel
      std::vector<GeoDetModule*> compVec=ring->ringModules();

      std::cout<<"  # sensor found for "<<this->getNameTag()<<"  "<<compVec.size()<<std::endl;
      for (std::vector<GeoDetModule*>::const_iterator j=compVec.begin(); j!=compVec.end(); j++) 
	{
	  GeoDetModule* comp  = (*j);
	  
	  HepGeom::Point3D<double>  p(0.0,0.0,0.0);
	  const HepGeom::Transform3D& trfComp=comp->getTransform3D();
	  HepGeom::Point3D<double>  midPoint=(trf)*(trfComp)*p;
	  double r=sqrt(midPoint.x()*midPoint.x()+midPoint.y()*midPoint.y());
	  
	  if(radius<0)
	    radius=r;
	  else
	    if (fabs(r-radius)>0.01) bIdenticalRadii=false;
	  
	  std::cout<<"Point -"<<comp->getNameTag()<<" - : "<<midPoint.x()<<" "<<midPoint.y()<<" "<<midPoint.z();
	  std::cout<<"  -> rad "<<r<<"  "<<bIdenticalRadii<<std::endl;
	}
    }

  return radius;

}


// Find radius correspondign to center of barrel modules
// FIXME : computed only for stave 0
double InDet::GeoEndcapAlpine::radiusMin() const
{
  if(numActiveComponents()==0) return 0;

  // Compute radii
  double radius=9999999999999.99;

  // Get stave HepTransform
  const HepGeom::Transform3D& trf=getTransform3D();

  // Loop over rings 
  for(int iRing=0; iRing<numActiveComponents(); iRing++)
    {
      GeoRingAlpine* ring =m_rings[iRing];
      
      // Get all sensors from defined for barrel
      std::vector<GeoDetModule*> compVec;
      const std::vector<GeoDetModule*> comp=ring->ringModules();

      std::cout<<"  # sensor found for "<<this->getNameTag()<<"  "<<compVec.size();
      for (std::vector<GeoDetModule*>::const_iterator j=compVec.begin(); j!=compVec.end(); j++) 
	{
	  GeoDetModule* comp  = (*j);
	  GeoComponentHelper compHelper(*comp);
	  double r=compHelper.getRmin(trf);
	  //	  double r=comp->getRmin(*trf);
	  if(r<radius) radius=r;
	}
    }
  return radius;
}

// Find radius correspondign to center of barrel modules
// FIXME : computed only for stave 0
double InDet::GeoEndcapAlpine::radiusMax() const
{
  if(numActiveComponents()==0) return 0;

  // Compute radii
  double radius=0.;

  // Get stave HepTransform
  const HepGeom::Transform3D& trf=getTransform3D();

  // Loop over rings 
  for(int iRing=0; iRing<numActiveComponents(); iRing++)
    {
      GeoRingAlpine* ring =m_rings[iRing];
      
      // Get all sensors from defined for barrel
      std::vector<GeoDetModule*> compVec;
      const std::vector<GeoDetModule*> comp=ring->ringModules();

      std::cout<<"  # sensor found for "<<this->getNameTag()<<"  "<<compVec.size();
      for (std::vector<GeoDetModule*>::const_iterator j=compVec.begin(); j!=compVec.end(); j++) 
	{
	  GeoDetModule* comp  = (*j);
	  GeoComponentHelper compHelper(*comp);
	  double r=compHelper.getRmax(trf);
	  //	  double r=comp->getRmax(*trf);
	  if(r>radius) radius=r;
	}
    }
  return radius;

}



// void InDet::GeoEndcapAlpine::placeSupport( GeoRingSupportAlpine& support, const HepGeom::Transform3D& transform, int number)
// {

//   //First resize vector if necessary
//   if((int)m_supports.size()<=number)
//     {
//       m_supports.resize(number+1,0);
//     }

//   //Insert new module 
//   GeoRingSupportAlpine *moduleNew=support.copyandclone();
//   moduleNew->setTransform(transform);
//   moduleNew->setSequentialNumber(number);
//   m_supports[number]=moduleNew;

//   //  placeComponent(layer,transform,number);
// }


int InDet::GeoEndcapAlpine::getNbRingModules(int iRing) const
{ 
  if(m_rings.size()==0||(int)m_rings.size()<iRing) return 0;
  return m_rings[iRing]->numActiveComponents();
}

int InDet::GeoEndcapAlpine::getNbRingModulesInPhi(int iRing) const
{ 
  if(m_rings.size()==0||(int)m_rings.size()<iRing) return 0;
  return m_rings[iRing]->numActiveComponents();
}

const HepGeom::Transform3D& InDet::GeoEndcapAlpine::getRingModuleTransform3D(int iRing,int iModule) const
{
  if(m_rings.size()==0||(int)m_rings.size()<iRing){
    const HepGeom::Transform3D& trfGbl=HepGeom::Transform3D();
    return trfGbl;
  }

  const std::vector<GeoDetModule*> comp_brl=m_rings[iRing]->ringModules();
  const HepGeom::Transform3D& trfStave=m_rings[iRing]->getTransform3D();
  const HepGeom::Transform3D& trfModule=comp_brl[iModule]->getTransform3D();

  const HepGeom::Transform3D& trfGbl=HepGeom::Transform3D((trfStave)*(trfModule));
  return trfGbl;
}

std::string InDet::GeoEndcapAlpine::getRingModuleName(int iRing,int iModule) const
{
  if(m_rings.size()==0||(int)m_rings.size()<iRing) return "xxxx";
  const std::vector<GeoDetModule*> comp_brl=m_rings[iRing]->ringModules();
  return comp_brl[iModule]->name();
}

int InDet::GeoEndcapAlpine::getModuleType(int iRing, int iModule) const
{
  if(m_rings.size()==0||iRing>=(int)m_rings.size()) return -1;  
  //  std::cout<<"getModuleType Layer "<<iRing<<" "<<iSide<<" "<<iModule<<std::endl;
  return m_rings[iRing]->getModuleType(iModule);
}

const GeoFullPhysVol* InDet::GeoEndcapAlpine::getSensorFullPhysVolume(int iRing, int iModule) const
{
  if(m_rings.size()==0||iRing>=(int)m_rings.size()) return 0;  
  return m_rings[iRing]->getSensorFullPhysVolume(iModule,0);
}

const GeoMaterial* InDet::GeoEndcapAlpine::getRingModuleAndSupportMaterial(int iRing) const
{
  return m_rings[iRing]->getRingModuleAndSupportMaterial();
}

const GeoMaterial* InDet::GeoEndcapAlpine::getRingSupportMaterial(int iRing) const
{
  return m_rings[iRing]->getRingSupportMaterial();
}

double InDet::GeoEndcapAlpine::getModuleHalfSizeInPhi(int iRing,int iModule) const
{
  if(m_rings.size()==0||(int)m_rings.size()<iRing) return 0;
  
  const std::vector<GeoDetModule*> comp_brl=m_rings[iRing]->ringModules();
  GeoComponentHelper compHelper(*comp_brl[iModule]);
  return (compHelper.getHalfSizeAlongYAxis());
  //  return (comp_brl[iModule])->getHalfSizeAlongYAxis();

}

double InDet::GeoEndcapAlpine::getModuleHalfSizeInEta(int iRing,int iModule) const
{
  if(m_rings.size()==0||(int)m_rings.size()<iRing) return 0;

  const std::vector<GeoDetModule*> comp_brl=m_rings[iRing]->ringModules();
  GeoComponentHelper compHelper(*comp_brl[iModule]);
  return (compHelper.getHalfSizeAlongZAxis());
  //  return (comp_brl[iModule])->getHalfSizeAlongZAxis();

}

std::vector<double> InDet::GeoEndcapAlpine::getZposFront() const
{
  std::vector<double> v;

  for(unsigned int iRing=0; iRing<m_rings.size(); iRing++)
    {
      const HepGeom::Transform3D& trfRing=m_rings[iRing]->getTransform3D();
      const std::vector<GeoDetModule*> compVec=m_rings[iRing]->ringModules();
      const HepGeom::Transform3D& trfComp=compVec[0]->getTransform3D();
      MinMaxHelper box=compVec[0]->getSensorZminmax((trfRing)*(trfComp));
      v.push_back(box.getMin());
    }
  
  return v;
}

std::vector<double> InDet::GeoEndcapAlpine::getZposBack() const
{
  std::vector<double> v;

  for(unsigned int iRing=0; iRing<m_rings.size(); iRing++)
    {
      const HepGeom::Transform3D& trfRing=m_rings[iRing]->getTransform3D();
      const std::vector<GeoDetModule*> compVec=m_rings[iRing]->ringModules();
      int iPos=(int)(compVec.size()-1);
      const HepGeom::Transform3D& trfComp=compVec[iPos]->getTransform3D();
      MinMaxHelper box=compVec[iPos]->getSensorZminmax((trfRing)*(trfComp));
      v.push_back(box.getMin());
    }
  return v;
}
