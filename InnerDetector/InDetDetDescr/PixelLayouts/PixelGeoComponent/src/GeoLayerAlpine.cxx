/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoComponent/GeoComponentHelper.h"
#include "PixelGeoComponent/GeoLayerAlpine.h"
#include "PixelGeoComponent/GeoStaveAlpine.h"
#include "PixelGeoComponent/GeoDetModule.h"

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"


InDet::GeoLayerAlpine::GeoLayerAlpine( GeoFullPhysVol* vol):GeoComponent(vol)
{ m_layerStaves.clear(); }
InDet::GeoLayerAlpine::GeoLayerAlpine( GeoPhysVol* vol):GeoComponent(vol)
{ m_layerStaves.clear(); }
InDet::GeoLayerAlpine::GeoLayerAlpine(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  GeoComponent(vol, seqNum, trf, name, bFlag)
{ m_layerStaves.clear(); }
InDet::GeoLayerAlpine::GeoLayerAlpine(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  GeoComponent(vol, seqNum, trf, name, bFlag)
{ m_layerStaves.clear(); }

const std::vector<InDet::GeoComponent*> InDet::GeoLayerAlpine::components() const
{
  std::vector<InDet::GeoComponent*> result; 
  result.reserve( m_layerStaves.size());
  for (std::vector<GeoStaveAlpine*>::const_iterator i=m_layerStaves.begin(); i!=m_layerStaves.end(); i++) {
    result.push_back(dynamic_cast<InDet::GeoComponent*>(*i)); // implicit cast to base class
  }
  return result;
}



InDet::GeoLayerAlpine*  InDet::GeoLayerAlpine::place( const HepGeom::Transform3D& transform, int number) const
{

  std::cout<<"GeoLayerAlpine : place"<<std::endl;

  GeoLayerAlpine* newComp = 0;
  if(isPhysVol())
    newComp = new InDet::GeoLayerAlpine(this->physVolume<GeoPhysVol>(),number,transform,this->getNameTag(),this->getG4InsertFlag());
  else
    newComp = new InDet::GeoLayerAlpine(this->physVolume<GeoFullPhysVol>(),number,transform,this->getNameTag(),this->getG4InsertFlag());

  std::cout<<"GeoLayerAlpine : copy done"<<std::endl;

  int iCmpt=0;
  for (std::vector<GeoStaveAlpine*>::const_iterator it=m_layerStaves.begin(); it!=m_layerStaves.end(); it++) {
    std::cout<<"GeoLayerAlpine : place stave "<<iCmpt<<std::endl;
    newComp->placeStave(*(*it),(*it)->getTransform(),iCmpt++);
  }

  return newComp;
}

void InDet::GeoLayerAlpine::placeStave( GeoStaveAlpine& stave, const HepGeom::Transform3D& transform, int number)
{

  //First resize vector if necessary
  if((int)m_layerStaves.size()<=number)
      m_layerStaves.resize(number+1,0);

  GeoStaveAlpine *compNew=stave.place(transform, number);
  m_layerStaves[number]=compNew;
  connectComponent(*compNew);

}



// Find radius correspondign to center of barrel modules
// FIXME : computed only for stave 0
double InDet::GeoLayerAlpine::radius(bool bBarrel, bool bEndCap) const
{
  if(m_layerStaves.size()==0) return 0;

  // Get stave HepTransform
  const HepGeom::Transform3D& trf=m_layerStaves[0]->getTransform3D();

  // Get all sensors from defined for barrel
  std::vector<GeoDetModule*> compVec;
  if(bBarrel)
    {
      const std::vector<GeoDetModule*> comp_brl=m_layerStaves[0]->barrelModules();
      compVec.insert( compVec.end(), comp_brl.begin(), comp_brl.end());
    }
  if(bEndCap)
    {
      std::vector<GeoDetModule*> compEC_neg=m_layerStaves[0]->endcapModules(-1);
      compVec.insert( compVec.end(), compEC_neg.begin(), compEC_neg.end());
      std::vector<GeoDetModule*> compEC_pos=m_layerStaves[0]->endcapModules(1);
      compVec.insert( compVec.end(), compEC_pos.begin(), compEC_pos.end());
    }

  std::cout<<"  # sensor found for "<<this->getNameTag()<<"  "<<compVec.size();
  // Compute radii
  double radius=-1;
  //bool bIdenticalRadii=true;
  for (std::vector<GeoDetModule*>::const_iterator j=compVec.begin(); j!=compVec.end(); j++) 
    {
      GeoDetModule* comp  = (*j);
      
      HepGeom::Point3D<double>  p(0.0,0.0,0.0);
      const HepGeom::Transform3D& trfComp=comp->getTransform3D();
      HepGeom::Point3D<double>  midPoint=trf*(trfComp)*p;
      double r=sqrt(midPoint.x()*midPoint.x()+midPoint.y()*midPoint.y());
      
      if(radius<0)
	radius=r;
      //else
      //if (fabs(r-radius)>0.01) bIdenticalRadii=false;

      std::cout<<"Point -"<<comp->getNameTag()<<" - : "<<midPoint.x()<<" "<<midPoint.y()<<" "<<midPoint.z();
      std::cout<<"  -> rad "<<r<<std::endl;
    }

  return radius;

}


// Find radius correspondign to center of barrel modules
// FIXME : computed only for stave 0
double InDet::GeoLayerAlpine::radiusMin(bool bBarrel, bool bEndCap) const
{
  if(m_layerStaves.size()==0) return 0;
  
  std::vector<GeoDetModule*> compVec;
  if(bBarrel)
    {
      const std::vector<GeoDetModule*> comp_brl=m_layerStaves[0]->barrelModules();
      compVec.insert( compVec.end(), comp_brl.begin(), comp_brl.end());
    }
  if(bEndCap)
    {
      std::vector<GeoDetModule*> compEC_neg=m_layerStaves[0]->endcapModules(-1);
      compVec.insert( compVec.end(), compEC_neg.begin(), compEC_neg.end());
      std::vector<GeoDetModule*> compEC_pos=m_layerStaves[0]->endcapModules(1);
      compVec.insert( compVec.end(), compEC_pos.begin(), compEC_pos.end());
    }

  // No endcap modules -> return barrel radius
  if(compVec.size()==0) return 0.0;
    
  // Get stave HepTransform
  const HepGeom::Transform3D& trf=m_layerStaves[0]->getTransform3D();

  // Compute radii
  double radius=9999999999999.99;
  for (std::vector<GeoDetModule*>::const_iterator j=compVec.begin(); j!=compVec.end(); j++) 
    {
      GeoDetModule* comp  = (*j);
      
      GeoComponentHelper compHelper(*comp);
      const double r=compHelper.getRmin(trf);
      //      const double r=comp->getRmin(trf);
      if(r<radius) radius=r;
    }

  return radius;

}

// Find radius correspondign to center of barrel modules
// FIXME : computed only for stave 0
double InDet::GeoLayerAlpine::radiusMax(bool bBarrel, bool bEndCap) const
{
  if(m_layerStaves.size()==0) return 0;
  
  std::vector<GeoDetModule*> compVec;
  if(bBarrel)
    {
      const std::vector<GeoDetModule*> comp_brl=m_layerStaves[0]->barrelModules();
      compVec.insert( compVec.end(), comp_brl.begin(), comp_brl.end());
    }
  if(bEndCap)
    {
      std::vector<GeoDetModule*> compEC_neg=m_layerStaves[0]->endcapModules(-1);
      compVec.insert( compVec.end(), compEC_neg.begin(), compEC_neg.end());
      std::vector<GeoDetModule*> compEC_pos=m_layerStaves[0]->endcapModules(1);
      compVec.insert( compVec.end(), compEC_pos.begin(), compEC_pos.end());
    }

  // No endcap modules -> return barrel radius
  if(compVec.size()==0) return 0.0;
    
  // Get stave HepTransform
  const HepGeom::Transform3D& trf=m_layerStaves[0]->getTransform3D();

  // Compute radii
  double radius=-1;
  for (std::vector<GeoDetModule*>::const_iterator j=compVec.begin(); j!=compVec.end(); j++) 
    {
      GeoDetModule* comp  = (*j);
      GeoComponentHelper compHelper(*comp);
      const double r=compHelper.getRmax(trf);
      //      const double r=comp->getRmax(trf);
      if(r>radius) radius=r;
    }

  return radius;

}

// Get Zminmax sizes of a module defined by stave and module number
InDet::MinMaxHelper InDet::GeoLayerAlpine::getBarrelModuleZMinMax(int iStave, int iModule) const
{
  MinMaxHelper boxModule=MinMaxHelper(0.0,0.0);

  if(m_layerStaves.size()==0||(int)m_layerStaves.size()<iStave) return boxModule;

  const std::vector<GeoDetModule*> comp_brl=m_layerStaves[iStave]->barrelModules();
  const HepGeom::Transform3D& trf1=m_layerStaves[iStave]->getTransform3D();
  const HepGeom::Transform3D& trf2=comp_brl[iModule]->getTransform3D();

  return comp_brl[iModule]->getSensorZminmax((trf1*trf2));

}

// Get Zminmax sizes of a module defined by stave and module number
InDet::MinMaxHelper InDet::GeoLayerAlpine::getEndcapModuleZMinMax(int iStave, int iModule, int iSide) const
{
  MinMaxHelper boxModule=MinMaxHelper(0.0,0.0);

  if(m_layerStaves.size()==0||(int)m_layerStaves.size()<iStave) return boxModule;

  const std::vector<GeoDetModule*> comp_brl=m_layerStaves[iStave]->endcapModules(iSide);

  const HepGeom::Transform3D& trf1=m_layerStaves[iStave]->getTransform3D();
  const HepGeom::Transform3D& trf2=comp_brl[iModule]->getTransform3D();

  return comp_brl[iModule]->getSensorZminmax((trf1*trf2));
}

const GeoFullPhysVol* InDet::GeoLayerAlpine::getGeoFullPhysVol(int iStave, int iModule) const
{

  if(m_layerStaves.size()==0||(int)m_layerStaves.size()<iStave) return 0;
  const std::vector<GeoDetModule*> comp_brl=m_layerStaves[iStave]->barrelModules();
  return (comp_brl[iModule])->physVolume<GeoFullPhysVol>();
}


double InDet::GeoLayerAlpine::getModuleHalfSizeInPhi(int iStave,int iModule, int iSide) const
{
  if(m_layerStaves.size()==0||(int)m_layerStaves.size()<iStave) return 0;
  
  if(iSide==0){
    const std::vector<GeoDetModule*> comp_brl=m_layerStaves[iStave]->barrelModules();
    GeoComponentHelper compHelper(*comp_brl[iModule]);
    return (compHelper.getHalfSizeAlongYAxis());
    //    return (comp_brl[iModule])->getHalfSizeAlongYAxis();
  }
  if(iSide==1){
    const std::vector<GeoDetModule*> comp_brl=m_layerStaves[iStave]->endcapModules(1);
    GeoComponentHelper compHelper(*comp_brl[iModule]);
    return (compHelper.getHalfSizeAlongYAxis());
	    //    return (comp_brl[iModule])->getHalfSizeAlongYAxis();
  }
  if(iSide==-1){
    const std::vector<GeoDetModule*> comp_brl=m_layerStaves[iStave]->endcapModules(-1);
    GeoComponentHelper compHelper(*comp_brl[iModule]);
    return (compHelper.getHalfSizeAlongYAxis());
	    //    return (comp_brl[iModule])->getHalfSizeAlongYAxis();
  }
  return 0.0;
}

double InDet::GeoLayerAlpine::getModuleHalfSizeInEta(int iStave,int iModule, int iSide) const
{
  if(m_layerStaves.size()==0||(int)m_layerStaves.size()<iStave) return 0;

  if(iSide==0){
    const std::vector<GeoDetModule*> comp_brl=m_layerStaves[iStave]->barrelModules();
    GeoComponentHelper compHelper(*comp_brl[iModule]);
    return (compHelper.getHalfSizeAlongZAxis());
	    //    return (comp_brl[iModule])->getHalfSizeAlongZAxis();
  }
  if(iSide==1){
    const std::vector<GeoDetModule*> comp_brl=m_layerStaves[iStave]->endcapModules(1);
    GeoComponentHelper compHelper(*comp_brl[iModule]);
    return (compHelper.getHalfSizeAlongZAxis());
	    //    return (comp_brl[iModule])->getHalfSizeAlongZAxis();
  }
  if(iSide==-1){
    const std::vector<GeoDetModule*> comp_brl=m_layerStaves[iStave]->endcapModules(-1);
    GeoComponentHelper compHelper(*comp_brl[iModule]);
    return (compHelper.getHalfSizeAlongZAxis());
	    //    return (comp_brl[iModule])->getHalfSizeAlongZAxis();
  }
  return 0.0;

}

const HepGeom::Transform3D& InDet::GeoLayerAlpine::getBarrelModuleTransform3D(int iStave,int iModule) const
{
  if(m_layerStaves.size()==0||(int)m_layerStaves.size()<iStave){
    const HepGeom::Transform3D& trfGbl=HepGeom::Transform3D();
    return trfGbl;
  }
  const std::vector<GeoDetModule*> comp_brl=m_layerStaves[iStave]->barrelModules();
  const HepGeom::Transform3D& trfStave=m_layerStaves[iStave]->getTransform3D();
  const HepGeom::Transform3D& trfModule=comp_brl[iModule]->getTransform3D();

  const HepGeom::Transform3D& trfGbl=HepGeom::Transform3D((trfStave)*(trfModule));
  return trfGbl;                                                                         ;
}

std::string InDet::GeoLayerAlpine::getBarrelModuleName(int iStave,int iModule) const
{
  if(m_layerStaves.size()==0||(int)m_layerStaves.size()<iStave) return "xxxx";
  const std::vector<GeoDetModule*> comp_brl=m_layerStaves[iStave]->barrelModules();
  return comp_brl[iModule]->name();
}

const HepGeom::Transform3D& InDet::GeoLayerAlpine::getEndcapModuleTransform3D(int iStave,int iModule, int iSide) const
{
  if(m_layerStaves.size()==0||(int)m_layerStaves.size()<iStave){
    const HepGeom::Transform3D& trfGbl=HepGeom::Transform3D();
    return trfGbl;
  }

  const std::vector<GeoDetModule*> comp_brl=m_layerStaves[iStave]->endcapModules(iSide);

  const HepGeom::Transform3D& trfStave=m_layerStaves[iStave]->getTransform3D();
  const HepGeom::Transform3D& trfModule=comp_brl[iModule]->getTransform3D();

  const HepGeom::Transform3D& trfGbl=HepGeom::Transform3D((trfStave)*(trfModule));
  return trfGbl;                                                                         ;
}

std::string InDet::GeoLayerAlpine::getEndcapModuleName(int iStave,int iModule, int iSide) const
{
  if(m_layerStaves.size()==0||(int)m_layerStaves.size()<iStave) return "xxxx";

  const std::vector<GeoDetModule*> comp_brl=m_layerStaves[iStave]->endcapModules(iSide);
  return comp_brl[iModule]->name();
}

/// half length of the active part of the layer
double InDet::GeoLayerAlpine::activeHalfLength(bool bBarrel, bool bEndCap, int iStave, int iSide) const
{

  MinMaxHelper boxGbl=activeLength_minmax(bBarrel,bEndCap,iStave,iSide);
  return (boxGbl.getMax()-boxGbl.getMin())*0.5;

}

InDet::MinMaxHelper InDet::GeoLayerAlpine::activeLength_minmax(bool bBarrel, bool bEndCap, int iStave, int iSide) const
{
  if(m_layerStaves.size()==0||iStave>=(int)m_layerStaves.size()) return MinMaxHelper(0.0,0.0);
  
  // Check if endcap modules are defined
  MinMaxHelper boxGbl=MinMaxHelper(9999999.0,-9999999.0);
  const HepGeom::Transform3D trf=m_layerStaves[0]->getTransform3D();

  std::vector<GeoDetModule*> compVec;
  if(bBarrel)
    {
      const std::vector<GeoDetModule*> comp_brl=m_layerStaves[0]->barrelModules();
      compVec.insert( compVec.end(), comp_brl.begin(), comp_brl.end());
    }
  if(bEndCap)
    {
      if(iSide<=0){
	std::vector<GeoDetModule*> compEC_neg=m_layerStaves[0]->endcapModules(-1);
	compVec.insert( compVec.end(), compEC_neg.begin(), compEC_neg.end());
      }
      if(iSide>0){
	std::vector<GeoDetModule*> compEC_pos=m_layerStaves[0]->endcapModules(1);
	compVec.insert( compVec.end(), compEC_pos.begin(), compEC_pos.end());
      }
    }

  // No endcap modules -> return barrel radius
  if(compVec.size()==0) return  MinMaxHelper(0.0,0.0);

  for (std::vector<GeoDetModule*>::const_iterator j=compVec.begin(); j!=compVec.end(); j++) 
    {
      GeoDetModule* comp  = (*j);
      GeoComponentHelper compHelper(*comp);
      MinMaxHelper box=compHelper.getZminmax(trf);
      //      MinMaxHelper box=comp->getZminmax(trf);
      boxGbl.update(box);
    }
  
  return boxGbl;
}

// Return barrel module length
double InDet::GeoLayerAlpine::barrelModuleLength(int iSector) const
{

  // Sector (or stave) not defined 
  if(iSector>=(int)m_layerStaves.size()-1)
    {
      std::cout<<"GeoBarrelLayer::barrelModuleLength "<<iSector<<std::endl;
      return 0.0;
    }

  // Get module length from stave instance
  return m_layerStaves[iSector]->barrelModuleLength();
}

int InDet::GeoLayerAlpine::getNbBarrelModules(int iStave) const
{ 
  if(m_layerStaves.size()==0||(int)m_layerStaves.size()<iStave) return 0;
  return m_layerStaves[iStave]->getNbBarrelModules();
}

int InDet::GeoLayerAlpine::getNbEndcapModules(int iSide, int iStave) const
{ 
  if(m_layerStaves.size()==0||(int)m_layerStaves.size()<iStave) return 0;
  return m_layerStaves[iStave]->getNbEndcapModules(iSide);
}

// Return stave length
double InDet::GeoLayerAlpine::staveLength(int iSector) const
{

  // Sector (or stave) not defined 
  if(iSector>=(int)m_layerStaves.size()-1)
    {
      std::cout<<"GeoBarrelLayer::staveLength "<<iSector<<std::endl;
      return 0.0;
    }

  // Get module length from stave instance
  GeoComponentHelper compHelper(*m_layerStaves[iSector]);
  return (2.0*compHelper.getHalfSizeAlongZAxis());
  //  return (2.0*m_layerStaves[iSector]->getHalfSizeAlongZAxis());
}



// Remove stave supports from stave, only pixel module stay in place
void InDet::GeoLayerAlpine::removeAllStaveSupports()
{
  for (int i=0; i<(int)m_layerStaves.size(); i++){
    m_layerStaves[i]->removeStaveSupports();
  }
}

// Remove foam modules from stave
void InDet::GeoLayerAlpine::removeAllFoamModules()
{
  for (int i=0; i<(int)m_layerStaves.size(); i++){
    m_layerStaves[i]->removeFoamModules();
  }
}


int InDet::GeoLayerAlpine::getModuleType(int iStave, int iSide, int iModule) const
{
  if(m_layerStaves.size()==0||iStave>=(int)m_layerStaves.size()) return -1;  
  //  std::cout<<"getModuleType Layer "<<iStave<<" "<<iSide<<" "<<iModule<<std::endl;
  return m_layerStaves[iStave]->getModuleType(iSide,iModule);
}

int InDet::GeoLayerAlpine::getModuleType(int iStave, int iModule) const
{
  if(m_layerStaves.size()==0||iStave>=(int)m_layerStaves.size()) return -1;  
  //  std::cout<<"getModuleType Layer "<<iStave<<" "<<iSide<<" "<<iModule<<std::endl;
  return m_layerStaves[iStave]->getModuleType(iModule);
}

const GeoFullPhysVol* InDet::GeoLayerAlpine::getSensorFullPhysVolume(int iStave, int iSide, int iModule, int iSensor) const
{
  if(m_layerStaves.size()==0||iStave>=(int)m_layerStaves.size()) return 0;  
  return m_layerStaves[iStave]->getSensorFullPhysVolume(iSide,iModule,iSensor);
}

const GeoFullPhysVol* InDet::GeoLayerAlpine::getSensorFullPhysVolume(int iStave, int iModule, int iSensor) const
{
  if(m_layerStaves.size()==0||iStave>=(int)m_layerStaves.size()) return 0;  
  return m_layerStaves[iStave]->getSensorFullPhysVolume(iModule,iSensor);
}

double InDet::GeoLayerAlpine::getBarrelStaveWidth(int iStave) const
{
  return m_layerStaves[iStave]->getBarrelStaveWidth();
}
double InDet::GeoLayerAlpine::getBarrelStaveLength(int iStave) const
{
  return m_layerStaves[iStave]->getBarrelStaveLength();
}
double InDet::GeoLayerAlpine::getEndcapStaveWidth(int iStave, int iSide) const
{
  return m_layerStaves[iStave]->getEndcapStaveWidth(iSide);
}
double InDet::GeoLayerAlpine::getEndcapStaveLength(int iStave, int iSide) const
{
  return m_layerStaves[iStave]->getEndcapStaveLength(iSide);
}
const HepGeom::Transform3D& InDet::GeoLayerAlpine::getBarrelStaveTransform3D(int iStave) const
{
  return m_layerStaves[iStave]->getBarrelStaveTransform3D();
}
const HepGeom::Transform3D& InDet::GeoLayerAlpine::getEndcapStaveTransform3D(int iStave, int iSide) const
{
  return m_layerStaves[iStave]->getEndcapStaveTransform3D(iSide);
}
