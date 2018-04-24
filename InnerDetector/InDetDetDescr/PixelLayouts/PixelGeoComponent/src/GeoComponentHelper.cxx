/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelGeoComponent/GeoComponentHelper.h"
#include "PixelGeoComponent/GeoSensor.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoElement.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"

#include <sstream>
//#include <iostream>
#include <iomanip>


const std::string idSuffix=std::string("#");

InDet::GeoComponentHelper::GeoComponentHelper(const GeoComponent& comp ):
  geoComponent( comp) 
{ }

InDet::GeoComponentHelper::~GeoComponentHelper() 
{ }


// **************************************************************************************************
// **************************************************************************************************
//
//    Find a GeoComponent starting from current GeoComponent
//
// **************************************************************************************************
// **************************************************************************************************

// Browse GeoComponent child tree to look for branch made of GeoComponents with tagNames parentName & volumeName 
//     -> returns only the last child   (i.e. the one corresponding to volumeName)
InDet::GeoComponent* InDet::GeoComponentHelper::findGeoComponent(const std::string& volumeName) const
{
  std::string namePrefix=geoComponent.getNameTag().substr(0,volumeName.length());
  if(namePrefix.compare(volumeName)==0) return const_cast<GeoComponent*>(&geoComponent);
  
  std::vector<InDet::GeoComponent*> compBranch=getGeoComponentBranch(volumeName);
  return compBranch[compBranch.size()-1];

}

// **************************************************************************************************
// **************************************************************************************************
//
//    Find the GeoComponent list (branch) between two geocomponents
//
// **************************************************************************************************
// **************************************************************************************************

// Browse GeoComponent child tree to look for branch made of GeoComponents with successive tagNames set to volumeNameSeq
//     -> returns the list of GeoComponents 
std::vector<InDet::GeoComponent*> InDet::GeoComponentHelper::getGeoComponentBranch(const std::string& volumeName) const
{
  std::vector<InDet::GeoComponent*> compList;
  
  std::vector<InDet::GeoComponent*> tmpList;
  int iFirstCopy=0;
  tmpList=getGeoComponentBranch_Level(geoComponent,volumeName);
  compList.insert(compList.end(),tmpList.begin()+iFirstCopy,tmpList.end());


  return compList;
}


// Browse GeoComponent child tree to look for GeoComponent with tagName set to "volumeName"
std::vector<InDet::GeoComponent*> InDet::GeoComponentHelper::getGeoComponentBranch_Level(const GeoComponent& currentComponent, const std::string& volumeName) const
{
  // parcours en largeur
  // FIXME : ajouter le parcours en hauteur et donner la possibilite de choisir le sens de parcours

  // Create GeoComponent list, push current GeoComponent in the list
  std::vector<InDet::GeoComponent*> compList;
  compList.clear();
  compList.push_back(const_cast<GeoComponent*>(&currentComponent));

  // Get list of child GeoComponent
  const std::vector<InDet::GeoComponent*> vec=currentComponent.components();

  // If no child is defined:
  //    - return empty list if current GeoComponent is not the one we are looking for
  //    - return list otherwise
  if(vec.size()==0)
    {
      if(currentComponent.getNameTag().substr(0,volumeName.length()).compare(volumeName)==0) return compList;
      std::vector<InDet::GeoComponent*> compEmpty;
      compEmpty.clear();
      return compEmpty;
    }

  // Loop over first level components f
  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) 
    {
      if((*i)->getNameTag().substr(0,volumeName.length()).compare(volumeName)==0)
	{
	  compList.push_back((*i));
	  return compList;
	}
    }

  // If GeoComponent is not found, loop over child components
  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) 
    {
      std::vector<GeoComponent *> compNext;
      compNext=getGeoComponentBranch_Level(*(*i),volumeName);
      //      compNext=(*i)->getGeoComponentBranch_Level(volumeName);
      //      compNext=(*i)->getGeoComponentBranch_Level(volumeName);
      //      std::cout<<"   -> # element "<<compNext.size()<<std::endl;
      if(compNext.size()>0)
	{
	  compList.insert(compList.end(),compNext.begin(),compNext.end());
	  return compList;
	}
    }

  // Nothing found, return empty list
  std::vector<InDet::GeoComponent*> compEmpty;
  compEmpty.clear();
  return compEmpty;
}





// **************************************************************************************************
// **************************************************************************************************
//
//    Find all child GeoComponent with nameTag matching a given pattern
//
// **************************************************************************************************
// **************************************************************************************************

// Browse GeoComponent child tree to look for branch made of GeoComponents with successive tagNames set to volumeNameSeq
//     -> returns only the last child   (i.e. the one corresponding to volumeName)
std::vector<InDet::GeoComponent*> InDet::GeoComponentHelper::findGeoComponentList(const std::string& volumeName, const std::string& parentName) const
{

  std::cout<<"findGeoComponentList : "<<volumeName<<" / "<<parentName<<"   current "<<geoComponent.getNameTag()<<std::endl;

  std::vector<InDet::GeoComponent*> compList;

  // If parent is defined look for parent GeoComponent
  if(parentName!="None")
    {
      //      std::cout<<"**> Parent defined "<<std::endl;
      GeoComponent *compFirst=findGeoComponent(parentName);
      if(compFirst==0)return compList;
      return findGeoComponentList_rec((*compFirst),volumeName);
    }
  else
    {
      //      std::cout<<"**> Parent undefined "<<std::endl;
      return findGeoComponentList_rec(geoComponent,volumeName);
    }

  return compList;

}

std::vector<InDet::GeoComponent*> InDet::GeoComponentHelper::findGeoComponentList_rec(const GeoComponent& currentComponent, const std::string& volumeName) const
{

  //  std::cout<<"findGeoComponentList_rec : "<<volumeName<<"   current "<<currentComponent.getNameTag()<<std::endl;

  std::vector<InDet::GeoComponent*> compList;

  // Test current component
  
  // Loop over component to extract readout geometry

  std::string namePrefix=currentComponent.getNameTag().substr(0,volumeName.length());
  //  std::cout<<"Comparison : -"<<namePrefix<<"- // -"<<volumeName<<"-"<<std::endl;  
  const std::vector<InDet::GeoComponent*> vec=currentComponent.components();
  if(namePrefix.compare(volumeName)==0)
    {
      //      std::cout<<"*** MATCH "<<std::endl;
      compList.push_back(const_cast<GeoComponent*>(&currentComponent));
    }
  if(vec.size()==0) return compList;

  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) 
    {
      //      std::cout<<"-> analyse : "<<(*i)->getNameTag()<<std::endl;

      std::string namePrefix=(*i)->getNameTag().substr(0,volumeName.length());
      if(namePrefix.compare(volumeName)==0)
	{
	  compList.push_back((*i));
	  //	  std::cout<<"*** STORED *** "<<(*i)->getNameTag()<<" // "<<compList.size()<<std::endl;
	}
      else
	{
	  std::vector<GeoComponent*> newCompList;
	  newCompList.clear();
	  newCompList = findGeoComponentList_rec(*(*i),volumeName);
	  //	  newCompList = (*i)->findGeoComponentList(volumeName);
	  
	  if(newCompList.size()>0)
	    compList.insert(compList.end(),newCompList.begin(),newCompList.end());
	}
    }
  
  return compList;
}



// Compute transform3D between current GeoComponent and GeoComponent with tag set to volumeName
HepGeom::Transform3D* InDet::GeoComponentHelper::getGeoComponentTransform3D(const std::string& volumeName) const
{
  
  std::vector<GeoComponent*> compList=getGeoComponentBranch(volumeName);
  
  const HepGeom::Transform3D& trf = geoComponent.getTransform3D(); 
  HepGeom::Transform3D trfGbl = trf;

  for (std::vector<GeoComponent*>::const_iterator i=compList.begin(); i!=compList.end(); i++) 
    {
      const HepGeom::Transform3D& trfNext=(*i)->getTransform3D();
      trfGbl=trfGbl*(trfNext);
    }

  return new HepGeom::Transform3D(trfGbl);

}


double InDet::GeoComponentHelper::getShapeVolume() const
{

  if(geoComponent.isFullPhysVol())
    return (geoComponent.physVolume<GeoFullPhysVol>())->getLogVol()->getShape()->volume();
  else
    return (geoComponent.physVolume<GeoPhysVol>())->getLogVol()->getShape()->volume();

  return 0.0;  // FIXME : should not happen   

}

std::string InDet::GeoComponentHelper::getMaterialName() const
{

  if(geoComponent.isFullPhysVol())
    return (geoComponent.physVolume<GeoFullPhysVol>())->getLogVol()->getMaterial()->getName();
  else
    return (geoComponent.physVolume<GeoPhysVol>())->getLogVol()->getMaterial()->getName();

}

const GeoMaterial* InDet::GeoComponentHelper::getMaterial() const
{

  if(geoComponent.isFullPhysVol())
    return ((geoComponent.physVolume<GeoFullPhysVol>())->getLogVol()->getMaterial());
  else
    return ((geoComponent.physVolume<GeoPhysVol>())->getLogVol()->getMaterial());

}


double InDet::GeoComponentHelper::getHalfSizeAlongZAxis() const 
{
  const GeoLogVol *logVol;
  double defaultLength =0.0;

  if((geoComponent.physVolume<GeoPhysVol>()))
    logVol=(geoComponent.physVolume<GeoPhysVol>())->getLogVol();
  else
    logVol=(geoComponent.physVolume<GeoFullPhysVol>())->getLogVol();

  bool bVerbose=false;
  if(bVerbose)std::cout<<"GeoComponent::getSizeAlongZAxis() const "<<logVol->getShape()->type()<<std::endl;

  if(logVol->getShape()->type()=="Box")
    {
      if(bVerbose)std::cout<<"component is a box "<<std::endl;
      const GeoShape* bmShape = logVol->getShape();
      const GeoBox* bmBox = dynamic_cast< const GeoBox*>( bmShape);
      if(bVerbose)std::cout<<"Size : "<<bmBox->getXHalfLength()<<" "<<bmBox->getYHalfLength()<<" "<<bmBox->getZHalfLength()<<std::endl;
      return bmBox->getZHalfLength();
    }
  
  if(logVol->getShape()->type()=="SimplePolygonBrep")
    {
      if(bVerbose)std::cout<<"component is a SimplePolygonBrep "<<std::endl;
      const GeoShape* bmShape = logVol->getShape();
      const GeoSimplePolygonBrep* bmBox = dynamic_cast< const GeoSimplePolygonBrep*>( bmShape);
      if(bVerbose)std::cout<<"Size : "<<bmBox->getDZ()<<std::endl;
      return bmBox->getDZ();
    }

  if(logVol->getShape()->type()=="Tube")
    {
      if(bVerbose)std::cout<<"component is a Tube "<<std::endl;
      const GeoShape* bmShape = logVol->getShape();
      const GeoTube* bmBox = dynamic_cast< const GeoTube*>( bmShape);
      if(bVerbose)std::cout<<"Size : "<<bmBox->getZHalfLength()<<std::endl;
      return bmBox->getZHalfLength();
    }

  return defaultLength;

 }

double InDet::GeoComponentHelper::getHalfSizeAlongYAxis() const 
{
  const GeoLogVol *logVol;
  double defaultLength =0.0;

  if((geoComponent.physVolume<GeoPhysVol>()))
    logVol=(geoComponent.physVolume<GeoPhysVol>())->getLogVol();
  else
    logVol=(geoComponent.physVolume<GeoFullPhysVol>())->getLogVol();

  bool bVerbose=false;
  if(bVerbose)std::cout<<"GeoComponent::getSizeAlongZAxis() const "<<logVol->getShape()->type()<<std::endl;

  if(logVol->getShape()->type()=="Box")
    {
      if(bVerbose)std::cout<<"component is a box "<<std::endl;
      const GeoShape* bmShape = logVol->getShape();
      const GeoBox* bmBox = dynamic_cast< const GeoBox*>( bmShape);
      if(bVerbose)std::cout<<"Size : "<<bmBox->getXHalfLength()<<" "<<bmBox->getYHalfLength()<<" "<<bmBox->getZHalfLength()<<std::endl;
      return bmBox->getYHalfLength();
    }
  
  if(logVol->getShape()->type()=="SimplePolygonBrep")
    {
      if(bVerbose)std::cout<<"component is a SimplePolygonBrep "<<std::endl;
      const GeoShape* bmShape = logVol->getShape();
      const GeoSimplePolygonBrep* bmBox = dynamic_cast< const GeoSimplePolygonBrep*>( bmShape);

      const unsigned int nbVertices=bmBox->getNVertices();
      double yMin=0.0, yMax=0.0;
      for(int i=0; i<(int)nbVertices; i++)
	{
	  const double& y=bmBox->getYVertex(i);
	  if(i==0){ yMin=y; yMax=y; }
	  else { if(y>yMax)yMax=y; if(y<yMin)yMin=y; }
	}
      const double yDim=(yMax-yMin)*0.5;
      return yDim;
    }

  return defaultLength;

 }

double InDet::GeoComponentHelper::getHalfSizeAlongXAxis() const 
{
  const GeoLogVol *logVol;
  double defaultLength =0.0;

  if((geoComponent.physVolume<GeoPhysVol>()))
    logVol=(geoComponent.physVolume<GeoPhysVol>())->getLogVol();
  else
    logVol=(geoComponent.physVolume<GeoFullPhysVol>())->getLogVol();

  bool bVerbose=false;
  if(bVerbose)std::cout<<"GeoComponent::getSizeAlongZAxis() const "<<logVol->getShape()->type()<<std::endl;

  if(logVol->getShape()->type()=="Box")
    {
      if(bVerbose)std::cout<<"component is a box "<<std::endl;
      const GeoShape* bmShape = logVol->getShape();
      const GeoBox* bmBox = dynamic_cast< const GeoBox*>( bmShape);
      if(bVerbose)std::cout<<"Size : "<<bmBox->getXHalfLength()<<" "<<bmBox->getYHalfLength()<<" "<<bmBox->getZHalfLength()<<std::endl;
      return bmBox->getXHalfLength();
    }
  
  if(logVol->getShape()->type()=="SimplePolygonBrep")
    {
      if(bVerbose)std::cout<<"component is a SimplePolygonBrep "<<std::endl;
      const GeoShape* bmShape = logVol->getShape();
      const GeoSimplePolygonBrep* bmBox = dynamic_cast< const GeoSimplePolygonBrep*>( bmShape);

      const unsigned int nbVertices=bmBox->getNVertices();
      double xMin=0.0, xMax=0.0;
      for(int i=0; i<(int)nbVertices; i++)
	{
	  const double& x=bmBox->getXVertex(i);
	  if(i==0){ xMin=x; xMax=x; }
	  else { if(x>xMax)xMax=x; if(x<xMin)xMin=x; }
	}
      const double xDim=(xMax-xMin)*0.5;
      return xDim;
    }

  return defaultLength;

 }

double InDet::GeoComponentHelper::getMaxSizeYAxis() const 
{
  const GeoLogVol *logVol;
  double defaultLength =0.0;

  if((geoComponent.physVolume<GeoPhysVol>()))
    logVol=(geoComponent.physVolume<GeoPhysVol>())->getLogVol();
  else
    logVol=(geoComponent.physVolume<GeoFullPhysVol>())->getLogVol();

  bool bVerbose=false;
  if(bVerbose)std::cout<<"GeoComponent::getSizeAlongZAxis() const "<<logVol->getShape()->type()<<std::endl;

  if(logVol->getShape()->type()=="Box")
    {
      if(bVerbose)std::cout<<"component is a box "<<std::endl;
      const GeoShape* bmShape = logVol->getShape();
      const GeoBox* bmBox = dynamic_cast< const GeoBox*>( bmShape);
      if(bVerbose)std::cout<<"Size : "<<bmBox->getXHalfLength()<<" "<<bmBox->getYHalfLength()<<" "<<bmBox->getZHalfLength()<<std::endl;
      return bmBox->getYHalfLength()*0.5;
    }
  
  if(logVol->getShape()->type()=="SimplePolygonBrep")
    {
      if(bVerbose)std::cout<<"component is a SimplePolygonBrep "<<std::endl;
      const GeoShape* bmShape = logVol->getShape();
      const GeoSimplePolygonBrep* bmBox = dynamic_cast< const GeoSimplePolygonBrep*>( bmShape);

      const unsigned int nbVertices=bmBox->getNVertices();
      double yMin=0.0, yMax=0.0;
      for(int i=0; i<(int)nbVertices; i++)
	{
	  double y=bmBox->getYVertex(i);
	  if(i==0){ yMin=y; yMax=y; }
	  else {  if(y>yMax)yMax=y; if(y<yMin)yMin=y; }
	}
      return yMax;
    }

  return defaultLength;

 }

void InDet::GeoComponentHelper::getMaxSizeXAxis(double& vMin, double &vMax) const 
{
  const GeoLogVol *logVol;
  vMin = vMax =0.0;

  if((geoComponent.physVolume<GeoPhysVol>()))
    logVol=(geoComponent.physVolume<GeoPhysVol>())->getLogVol();
  else
    logVol=(geoComponent.physVolume<GeoFullPhysVol>())->getLogVol();

  bool bVerbose=false;
  if(bVerbose)std::cout<<"GeoComponent::getSizeAlongZAxis() const "<<logVol->getShape()->type()<<std::endl;

  if(logVol->getShape()->type()=="Box")
    {
      if(bVerbose)std::cout<<"component is a box "<<std::endl;
      const GeoShape* bmShape = logVol->getShape();
      const GeoBox* bmBox = dynamic_cast< const GeoBox*>( bmShape);
      if(bVerbose)std::cout<<"Size : "<<bmBox->getXHalfLength()<<" "<<bmBox->getYHalfLength()<<" "<<bmBox->getZHalfLength()<<std::endl;
      vMin=-bmBox->getXHalfLength()*0.5;
      vMax=bmBox->getXHalfLength()*0.5;
      return;
    }
  
  if(logVol->getShape()->type()=="SimplePolygonBrep")
    {
      if(bVerbose)std::cout<<"component is a SimplePolygonBrep "<<std::endl;
      const GeoShape* bmShape = logVol->getShape();
      const GeoSimplePolygonBrep* bmBox = dynamic_cast< const GeoSimplePolygonBrep*>( bmShape);

      const unsigned int nbVertices=bmBox->getNVertices();
      double xMin=0.0, xMax=0.0;
      for(int i=0; i<(int)nbVertices; i++)
	{
	  const double& x=bmBox->getXVertex(i);
	  if(i==0){ xMin=x; xMax=x; }
	  else {
	    if(x>xMax)xMax=x; if(x<xMin)xMin=x;
	  }
	}
      vMin = xMin; vMax = xMax;
      return;
    }

 }

double InDet::GeoComponentHelper::getRmax(const HepGeom::Transform3D& trf) const
{
  double rMax=0.0;
  std::vector<HepGeom::Point3D<double> > outline=getVertexShape();
  for (std::vector<HepGeom::Point3D<double> >::const_iterator iPt=outline.begin(); iPt!=outline.end(); iPt++)
    {
      const HepGeom::Transform3D& trfComp=geoComponent.getTransform3D();
      HepGeom::Point3D<double> pTrf=(trf)*((trfComp)*(*iPt));
      double radius=sqrt(pTrf.x()*pTrf.x()+pTrf.y()*pTrf.y());
      if(radius>rMax)rMax=radius;
    }  

  return rMax;
}

double InDet::GeoComponentHelper::getRmin(const HepGeom::Transform3D& trf) const
{
  double rMin=9999999999.0;
  std::vector<HepGeom::Point3D<double> > outline=getVertexShape();
  for (std::vector<HepGeom::Point3D<double> >::const_iterator iPt=outline.begin(); iPt!=outline.end(); iPt++)
    {
      const HepGeom::Transform3D& trfComp=geoComponent.getTransform3D();
      HepGeom::Point3D<double> pTrf=(trf)*((trfComp)*(*iPt));
      double radius=sqrt(pTrf.x()*pTrf.x()+pTrf.y()*pTrf.y());
      if(radius<rMin)rMin=radius;
    }  
  return rMin;
}

InDet::MinMaxHelper InDet::GeoComponentHelper::getRminmax(const HepGeom::Transform3D& trf) const
{
  double rMin=0.0,rMax=0.0;
  std::vector<HepGeom::Point3D<double> > outline=getVertexShape();
  for (std::vector<HepGeom::Point3D<double> >::const_iterator iPt=outline.begin(); iPt!=outline.end(); iPt++)
    {
      const HepGeom::Transform3D& trfComp=geoComponent.getTransform3D();
      HepGeom::Point3D<double> pTrf=(trf)*((trfComp)*(*iPt));
      double radius=sqrt(pTrf.x()*pTrf.x()+pTrf.y()*pTrf.y());
      if(radius<rMin)rMin=radius;
      if(radius>rMax)rMax=radius;
    }  
  return MinMaxHelper(rMin,rMax);
}

InDet::MinMaxHelper InDet::GeoComponentHelper::getZminmax(const HepGeom::Transform3D& trf) const
{
  double zMax=-99999999.99,zMin=9999999.99;
  std::vector<HepGeom::Point3D<double> > outline=getVertexShape();
  for (std::vector<HepGeom::Point3D<double> >::const_iterator iPt=outline.begin(); iPt!=outline.end(); iPt++)
    {
      const HepGeom::Transform3D& trfComp=geoComponent.getTransform3D();
      HepGeom::Point3D<double> pTrf=(trf)*((trfComp)*(*iPt));
      if(pTrf.z()>zMax)zMax=pTrf.z();
      if(pTrf.z()<zMin)zMin=pTrf.z();
    }  

  MinMaxHelper box=MinMaxHelper(zMin,zMax);

  return box;
}

InDet::MinMax3DHelper InDet::GeoComponentHelper::get3Dminmax(const HepGeom::Transform3D& trf) const
{
  double xMax=-99999999.99,xMin=9999999.99;
  double yMax=-99999999.99,yMin=9999999.99;
  double zMax=-99999999.99,zMin=9999999.99;

  std::vector<HepGeom::Point3D<double> > outline=getVertexShape();
  for (std::vector<HepGeom::Point3D<double> >::const_iterator iPt=outline.begin(); iPt!=outline.end(); iPt++)
    {
      const HepGeom::Transform3D& trfComp=geoComponent.getTransform3D();
      HepGeom::Point3D<double> pTrf=(trf)*((trfComp)*(*iPt));
      if(pTrf.x()>xMax)xMax=pTrf.x();
      if(pTrf.x()<xMin)xMin=pTrf.x();
      if(pTrf.y()>yMax)yMax=pTrf.y();
      if(pTrf.y()<yMin)yMin=pTrf.y();
      if(pTrf.z()>zMax)zMax=pTrf.z();
      if(pTrf.z()<zMin)zMin=pTrf.z();
    }  

  MinMax3DHelper box=MinMax3DHelper(xMin,xMax,yMin,yMax,zMin,zMax);

  return box;
}


std::vector<HepGeom::Point3D<double>> InDet::GeoComponentHelper::getVertexShape() const 
{
  const GeoLogVol *logVol;
  std::vector<HepGeom::Point3D<double> > vVertices;

  if((geoComponent.physVolume<GeoPhysVol>()))
    logVol=(geoComponent.physVolume<GeoPhysVol>())->getLogVol();
  else
    logVol=(geoComponent.physVolume<GeoFullPhysVol>())->getLogVol();

  bool bVerbose=false;
  if(bVerbose)std::cout<<"GeoComponent::getSizeAlongZAxis() const "<<logVol->getShape()->type()<<std::endl;

  if(logVol->getShape()->type()=="Box")
    {
      if(bVerbose)std::cout<<"component is a box "<<std::endl;
      const GeoShape* bmShape = logVol->getShape();
      const GeoBox* bmBox = dynamic_cast< const GeoBox*>( bmShape);
      double xHalfSize=bmBox->getXHalfLength();
      double yHalfSize=bmBox->getYHalfLength();
      double zHalfSize=bmBox->getZHalfLength();
      vVertices.push_back(HepGeom::Point3D<double>(xHalfSize,yHalfSize,-zHalfSize));
      vVertices.push_back(HepGeom::Point3D<double>(-xHalfSize,yHalfSize,-zHalfSize));
      vVertices.push_back(HepGeom::Point3D<double>(-xHalfSize,-yHalfSize,-zHalfSize));
      vVertices.push_back(HepGeom::Point3D<double>(xHalfSize,-yHalfSize,-zHalfSize));
      vVertices.push_back(HepGeom::Point3D<double>(xHalfSize,yHalfSize,zHalfSize));
      vVertices.push_back(HepGeom::Point3D<double>(-xHalfSize,yHalfSize,zHalfSize));
      vVertices.push_back(HepGeom::Point3D<double>(-xHalfSize,-yHalfSize,zHalfSize));
      vVertices.push_back(HepGeom::Point3D<double>(xHalfSize,-yHalfSize,zHalfSize));
      return vVertices;

    }
  else if(logVol->getShape()->type()=="SimplePolygonBrep")
    {
      if(bVerbose)std::cout<<"component is a SimplePolygonBrep "<<std::endl;
      const GeoShape* bmShape = logVol->getShape();
      const GeoSimplePolygonBrep* bmBox = dynamic_cast< const GeoSimplePolygonBrep*>( bmShape);
      double zHalfSize=bmBox->getDZ();

      unsigned int nVertices=bmBox->getNVertices();
      for(unsigned int i=0; i<nVertices; i++)
	{
	  HepGeom::Point3D<double> p=HepGeom::Point3D<double>(bmBox->getXVertex(i),bmBox->getYVertex(i),zHalfSize);
	  vVertices.push_back(p);
	}
      for(unsigned int i=0; i<nVertices; i++)
	{
	  HepGeom::Point3D<double> p=HepGeom::Point3D<double>(bmBox->getXVertex(i),bmBox->getYVertex(i),-zHalfSize);
	  vVertices.push_back(p);
	}
      return vVertices;
    }

  return vVertices;

 }







// **************************************************************************************************
// **************************************************************************************************
//
//    Gather module types indices 
//
// **************************************************************************************************
// **************************************************************************************************

void InDet::GeoComponentHelper::FindGeoComponentsMatchingName(const GeoComponent& currentComponent, const std::string& name, std::vector<GeoComponent*> &list) const
{

  //  std::cout<<"findGeoComponentList_rec : "<<name<<"   current "<<currentComponent.getNameTag()<<std::endl;

  // Loop over component to extract readout geometry
  std::string namePrefix=currentComponent.getNameTag().substr(0,name.length());
  //  std::cout<<"Comparison : -"<<namePrefix<<"- // -"<<name<<"-"<<std::endl;  
  const std::vector<InDet::GeoComponent*> vec=currentComponent.components();
  if(namePrefix.compare(name)==0)
    {
      //      std::cout<<"*** MATCH "<<std::endl;
      list.push_back(const_cast<GeoComponent*>(&currentComponent));
    }
  if(vec.size()==0) return;

  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) 
    {
      //      std::cout<<"-> analyse : "<<(*i)->getNameTag()<<std::endl;

      std::string namePrefix=(*i)->getNameTag().substr(0,name.length());
      if(namePrefix.compare(name)==0)
	{
	  list.push_back((*i));
	  //	  std::cout<<"*** STORED *** "<<(*i)->getNameTag()<<" // "<<list.size()<<std::endl;
	}
      else
	{
	  std::vector<GeoComponent*> newCompList;
	  newCompList.clear();
	  FindGeoComponentsMatchingName(*(*i),name,newCompList);
	  //newCompList = findGeoComponentList_rec(*(*i),name);
	  //	  newCompList = (*i)->findGeoComponentList(name);
	  
	  if(newCompList.size()>0)
	    list.insert(list.end(),newCompList.begin(),newCompList.end());
	}
    }
}



std::vector<int> InDet::GeoComponentHelper::getSensorModuleIndices() const
{
  std::vector<int> modIndices;

  std::vector<InDet::GeoComponent*> sensorList;
  FindGeoComponentsMatchingName(geoComponent,"siLog",sensorList);

//   std::cout<<"getSensorModuleIndices() : "<<sensorList.size()<<std::endl;
//   //  std::vector<InDet::GeoComponent*> sensorList= geoComponent.findGeoComponentList("siLog");
//   for(std::vector<InDet::GeoComponent*>::iterator it=sensorList.begin();  it!=sensorList.end(); it++)
//     {
//       const GeoComponent& component  = *((*it));
//       const GeoSensor& sensor=dynamic_cast<const GeoSensor&>(component); 

//       int moduleType=sensor.getModuleType();

//       bool bFound=false;
//       for (std::vector<int>::const_iterator k=modIndices.begin(); k!=modIndices.end(); k++) 
// 	{
// 	  if ((*k)==moduleType)bFound=true;
// 	}
//       if(!bFound)modIndices.push_back(moduleType);
//     }

//   std::sort(modIndices.begin(), modIndices.end());

  return modIndices;
}







// **************************************************************************************************
// **************************************************************************************************
//
//    Compute volumes omaterial equivalent to  all subtree components
//
// **************************************************************************************************
// **************************************************************************************************

// Compute all 3D material volumes of a GeoComponent given its name (or mask) and its branch: 
//    - if parentName="None", starting GeoComponent is current GeoComponent
//                   if not , starting poinr is current GeoComponent child with "parentName"  nameTag


std::map<std::string,double> InDet::GeoComponentHelper::computeSubTreeComponentsMaterialAndVolume(std::vector<std::string> ignoreVol) const
{ 

  std::map<std::string,double>materialMap;
  std::cout<<"ComputeAllMaterialVolume  : "<<geoComponent.getNameTag()<<std::endl;

  // First of all : compute volumes of GeoComponent tree
  bool bGroupVolume=true;
  std::map<std::string,double>volumeMap=computeSubTreeComponentsVolume(bGroupVolume);
  //    std::cout<<"**> back to computeSubTreeComponentsMaterialAndVolume"<<std::endl;
  
  // Loop over volume to get Material and complete map with key set to materialName
  for (std::map<std::string,double>::iterator it=volumeMap.begin() ; it != volumeMap.end(); it++ )
    {  
      std::string volNameTmp=(*it).first;	
      //	std::cout<<"    -> find : "<<volNameTmp<<"  / "<<getNameTag()<<std::endl;
      
      bool bIgnoreVolume=false;
      for (std::vector<std::string>::iterator it2=ignoreVol.begin() ; it2 != ignoreVol.end(); it2++ )
	{
	  std::string volNameIgn=(*it2);
	  if((volNameTmp.substr(0,volNameIgn.length())).compare(volNameIgn)==0)
	    {
	      bIgnoreVolume=true;
	      //		std::cout<<"    -> ignore : "<<volNameTmp<<"  / "<<volNameIgn<<std::endl;
	    }
	}
      
      if(!bIgnoreVolume)
	{
	  GeoComponent *comp=findGeoComponent(volNameTmp);
	  std::string matName=comp->getMaterialName();
	  //	    std::cout<<"    -> corresp. mat : "<<matName<<std::endl;
	  
	  // Lopp over material map
	  std::map<std::string,double>::iterator itGbl;
	  bool bMatAlreadyDefined=false;
	  for ( itGbl=materialMap.begin() ; itGbl != materialMap.end(); itGbl++ )
	    {
	      if((*itGbl).first.compare(matName)==0)
		{
		  bMatAlreadyDefined=true;
		  (*itGbl).second+=(*it).second;
		}
	    }
	  if(!bMatAlreadyDefined) materialMap.insert(std::pair<std::string,double>(matName,(*it).second));
	}
    }
  
  return materialMap;
}



// **************************************************************************************************
// **************************************************************************************************
//
//    Compute material equivalent to  all subtree components
//
// **************************************************************************************************
// **************************************************************************************************

GeoMaterial* InDet::GeoComponentHelper::computeEquivalentMaterial(InDetMaterialManager* matMgr, 
								  std::string newMatName,  std::vector<std::string> ignoreVol) const
{ 
 
  std::cout<<"***> compute equivalent material : "<<geoComponent.getNameTag()<<std::endl;
 std::map<std::string,double> materialMap=computeSubTreeComponentsMaterialAndVolume(ignoreVol);

  bool bVerbose=true;
  if(bVerbose){
    std::cout<<"ComputeEquivalentMaterial : "<<std::endl;
    for (std::map<std::string,double>::iterator it=materialMap.begin() ; it != materialMap.end(); it++ ) std::cout<<(*it).first<<" "<<(*it).second<<" / ";
  }
  
  // Compute total weight
  double totalWeight=0;
  for (std::map<std::string,double>::iterator it=materialMap.begin() ; it != materialMap.end(); it++ ) 
    {
      std::cout<<(*it).first<<" "<<(*it).second<<" / ";
      if(((*it).first).compare("Ether")!=0)totalWeight+=(*it).second;
    }
  if(bVerbose)std::cout<<"Total weight : "<<totalWeight<<std::endl;
  double invTotalWeight=1.0/totalWeight;

  // Compute new density
  double compositeDensity=0;
  std::map<std::string,double> compositeElmt;
  std::vector<std::string>matPrefix;
  matPrefix.push_back("pix::"); matPrefix.push_back("std::"); matPrefix.push_back(""); matPrefix.push_back("special::");
  for (std::map<std::string,double>::iterator it=materialMap.begin() ; it != materialMap.end(); it++ ) 
    if(((*it).first).compare("Ether")!=0)
      {
	bool bMaterialDefined=false;
	for (std::vector<std::string>::iterator itPrf=matPrefix.begin() ; itPrf != matPrefix.end()&&!bMaterialDefined; itPrf++ ) 
	  {
	    const std::string matName=(*itPrf)+(*it).first;
	    const GeoMaterial *mat=matMgr->getMaterial(matName);
	    if(mat!=0)
	      {
		bMaterialDefined=true;
		double density=mat->getDensity();
		if(bVerbose)std::cout<<" -> "<<mat->getName()<<" "<<density<<std::endl;
		compositeDensity+=((*it).second*invTotalWeight)/density;
		compositeElmt.insert(std::pair<std::string,double>(matName,(*it).second*invTotalWeight));
	      }
	}
	if(!bMaterialDefined)std::cout<<"**** material not found "<<(*it).first<<std::endl;
      }

  compositeDensity=1.0/compositeDensity;
  if(bVerbose)std::cout<<"Composite density : "<<compositeDensity<<std::endl;
  
  GeoMaterial* compositeMat=new GeoMaterial(newMatName,compositeDensity);
  if(bVerbose)std::cout<<" -> new material "<<compositeMat->getName()<<" "<<compositeMat->getDensity()<<std::endl;
  for (std::map<std::string,double>::iterator it=compositeElmt.begin() ; it != compositeElmt.end(); it++ ) 
    {
      GeoMaterial *mat=const_cast<GeoMaterial*>(matMgr->getMaterial((*it).first));
      compositeMat->add(mat,(*it).second);
    }  
  compositeMat->lock();

  if(bVerbose){
    std::cout<<" -> new material elements "<<std::endl;
    unsigned int nbElements=compositeMat->getNumElements();
    for(unsigned int iEl=0; iEl<nbElements; iEl++)
      {
	const GeoElement *el=compositeMat->getElement(iEl);
	double wgFraction=compositeMat->getFraction(iEl);
	std::cout<<el->getName()<<" "<<wgFraction<<std::endl;
      }
  }

//   std::cout<<"**> and finally add new composite material to material manager"<<std::endl;
//   matMgr->addMaterial(compositeMat);

  if(bVerbose)std::cout<<"RETURN composite material"<<std::endl;

  return compositeMat;

}


GeoMaterial* InDet::GeoComponentHelper::combineCompositeMaterial(InDetMaterialManager* /*matMgr*/, std::string newMatName, 
								 const GeoMaterial* mat1_ct, const GeoMaterial* mat2_ct, const double ratio1, const double ratio2) const 

{ 
  GeoMaterial* compositeMat;

  std::cout<<"CombineCompositeMaterials : "<<mat1_ct->getName()<<" "<<mat2_ct->getName()<<"  "<<ratio1<<" "<<ratio2<<std::endl;

  GeoMaterial *mat1=const_cast<GeoMaterial*>(mat1_ct);
  GeoMaterial *mat2=const_cast<GeoMaterial*>(mat2_ct);

  double frac1=ratio1/(ratio1+ratio2);
  double frac2=ratio2/(ratio1+ratio2);

  // Compute new density
  double tmp=frac1/mat1->getDensity()+frac2/mat2->getDensity();
  double newDensity=1.0/tmp;

  // Material composition
  std::cout<<"Composite density : "<<newDensity<<std::endl;
  
  compositeMat=new GeoMaterial(newMatName,newDensity);
  std::cout<<" -> new material "<<compositeMat->getName()<<" "<<compositeMat->getDensity()<<std::endl;
  compositeMat->add(mat1,frac1);
  compositeMat->add(mat2,frac2);
  compositeMat->lock();

  std::cout<<"RETURN combined material"<<std::endl;

  return compositeMat;
}


// **************************************************************************************************
// **************************************************************************************************
//
//    Compute all subtree components volumes
//
// **************************************************************************************************
// **************************************************************************************************

std::map<std::string,double> InDet::GeoComponentHelper::computeSubTreeComponentsVolume(bool bGroupVolume) const
{ 
  std::map<std::string,double>volumeMap;
  volumeMap.clear();
  return computeSubTreeComponentsVolume_rec(geoComponent,bGroupVolume);  
}


std::map<std::string,double> InDet::GeoComponentHelper::computeSubTreeComponentsVolume_rec(const GeoComponent& currentComponent, bool bGroupVolume) const
{ 
  std::map<std::string,double>volumeMap;
  volumeMap.clear();

  // Compute volume of root GeoComponent and recursively all subtree components volumes
  std::map<std::string,double>volumeMap_Child=computeSubTreeComponentsVolume_level(currentComponent,0);

  bool bVerbose=false;
  if(bVerbose)std::cout<<"**> computeAllVolume done"<<std::endl;  

  // Loop over new computed volumes 
  std::map<std::string,double>::iterator it;
  for ( it=volumeMap_Child.begin() ; it != volumeMap_Child.end(); it++ )
    {
      bool bVolAlreadyDefined=false;
	      
      // Remove #number in case volumes are grouped
      std::string volName=(*it).first;
      if(bGroupVolume)
	{
	  size_t found=volName.rfind("-"+idSuffix);
	  if (found!=std::string::npos) volName=volName.substr(0,found);
	}		      
      if(bVerbose)std::cout<<"  -> volume : "<<volName<<std::endl;

      // Loop over new computed volumes 
      std::map<std::string,double>::iterator itGbl;
      for ( itGbl=volumeMap.begin() ; itGbl != volumeMap.end(); itGbl++ )
	{
	  if((*itGbl).first.compare(volName)==0)
	    {
	      bVolAlreadyDefined=true;
	      (*itGbl).second+=(*it).second;
	    }
	}
      if(bVerbose)std::cout<<"  -> already defined : "<<bVolAlreadyDefined<<std::endl;
      if(!bVolAlreadyDefined) volumeMap.insert(std::pair<std::string,double>(volName,(*it).second));
    }

  std::cout<<"***> return computeSubTreeComponentsVolume"<<std::endl;
  return volumeMap;
}


std::map<std::string,double> InDet::GeoComponentHelper::computeSubTreeComponentsVolume_level(const GeoComponent& currentComponent, int iLevel) const
{
  std::map<std::string,double>compVolumeMap;
  compVolumeMap.clear();

  double compVolume=0.0;

  //  compVolume=this->getShapeVolume();

  //  for(int j=0; j<iLevel;j++) std::cout<<"  ";
  //  std::cout<<"Volume initial : "<<this->getNameTag()<<" : "<<compVolume<<std::endl;

  // Compute current component volume
  const std::vector<InDet::GeoComponent*> vec=currentComponent.components();
  GeoComponentHelper compHelper(currentComponent);
  compVolume=compHelper.computeComponentVolume_basic();
  
  // Add current component volume to map
  bool bVolAlreadyDefined=false;
  std::map<std::string,double>::iterator itGbl;
  for ( itGbl=compVolumeMap.begin() ; itGbl != compVolumeMap.end(); itGbl++ )   // FIXME : this test looks stupid ;-))
    {
      if((*itGbl).first.compare(currentComponent.getNameTag())==0)
	{
	  bVolAlreadyDefined=true;
	  (*itGbl).second+=compVolume;
	}
    }
  if(!bVolAlreadyDefined)
    compVolumeMap.insert(std::pair<std::string,double>(currentComponent.getNameTag(),compVolume));


  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) 
    {
      //	  for(int j=0; j<iLevel;j++) std::cout<<"  ";
      //	  std::cout<<"Volume ignored : "<<(*i)->getNameTag()<<std::endl;
      std::map<std::string,double>volumeMap_Child;
      volumeMap_Child=computeSubTreeComponentsVolume_level(*(*i),iLevel+1);
      //      volumeMap_Child=(*i)->computeSubTreeComponentsVolume(iLevel+1);
      
      std::map<std::string,double>::iterator it;
      for ( it=volumeMap_Child.begin() ; it != volumeMap_Child.end(); it++ )
	{
	  bool bVolAlreadyDefined=false;
	  std::map<std::string,double>::iterator itGbl;
	  for ( itGbl=compVolumeMap.begin() ; itGbl != compVolumeMap.end(); itGbl++ )
	    {
	      if((*itGbl).first.compare((*it).first)==0)
		{
		  bVolAlreadyDefined=true;
		  (*itGbl).second+=(*it).second;
		}
	    }
	  if(!bVolAlreadyDefined)
	    {
	      compVolumeMap.insert((*it));
	    }
	}
    }

  
   for(int j=0; j<iLevel;j++) std::cout<<"  ";
   std::cout<<"Volume final : "<<currentComponent.getNameTag()<<" : "<<compVolume<<std::endl;
  

//   for ( itGbl=compVolumeMap.begin() ; itGbl != compVolumeMap.end(); itGbl++ )
//     {
//       std::cout<<(*itGbl).first<<" "<<(*itGbl).second<<"  /  ";
//     }
//   std::cout<<std::endl;

  return compVolumeMap;
}

// Compute volume of a geocomponent object ( + some child tag names can be ignored (ex : StaveSupport,...) )
double InDet::GeoComponentHelper::computeComponentVolume(std::vector<std::string> ignoreVol) const
{ 

  // Compute all sub-volumes
  std::map<std::string,double> volumeMap=computeSubTreeComponentsVolume(true);

  // Loop over all GeoComponents with name corresponding to volName or volName mask
  bool bVerbose=false;
  double globalVolume=0;
  for (std::map<std::string,double>::iterator it=volumeMap.begin() ; it != volumeMap.end(); it++ )
    {  
      std::string volNameTmp=(*it).first;	
      bool bIgnoreVolume=false;
      for (std::vector<std::string>::iterator it2=ignoreVol.begin() ; it2 != ignoreVol.end(); it2++ )
	{
	  std::string volNameIgn=(*it2);
	  if((volNameTmp.substr(0,volNameIgn.length())).compare(volNameIgn)==0)
	    {
	      bIgnoreVolume=true;
	      if(bVerbose)std::cout<<"    -> ignore : "<<volNameTmp<<"  / "<<volNameIgn<<std::endl;
	    }
	}
      
      if(!bIgnoreVolume)
	{
	  if(bVerbose)std::cout<<"    -> add : "<<volNameTmp<<std::endl;
	  globalVolume+=(*it).second;
	}
    }  

  if(bVerbose)std::cout<<"computeComponentVolume : "<<geoComponent.getNameTag()<<" "<<globalVolume<<std::endl;

  return globalVolume;
}

// Compute volume of a simple geocomponent object (based on logvol->shape volumes)
double InDet::GeoComponentHelper::computeComponentVolume_basic(int /*iLevel*/) const
{

  double compVolume=0.0;
  compVolume=getShapeVolume();
  double shapeVolume = compVolume;

  // Get GeoComponents of current object
  const std::vector<InDet::GeoComponent*> vec=geoComponent.components();

  // Loop over childs : if child volume is set as " inserted", subtract child volume
  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) 
    {
      if((*i)->getG4InsertFlag()==true)
	{
	  GeoComponentHelper compHelper(*(*i));
	  double vol2=compHelper.getShapeVolume();
	  //	  double vol2=(*i)->getShapeVolume();
	  compVolume-=vol2;
	}
    }

  //  for(int j=0; j<iLevel;j++) std::cout<<"  ";
  std::cout<<"computeVolume : Volume final : "<<geoComponent.getNameTag()<<" shape/real : "<<shapeVolume<<" "<<compVolume<<" "<<geoComponent.getMaterialName()<<std::endl;
  return compVolume;
}

