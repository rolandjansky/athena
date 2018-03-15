/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoComponent/GeoComponent.h"
#include "PixelGeoComponent/GeoSensor.h"
#include "PixelGeoComponent/GeoDetModule.h"
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

InDet::GeoComponent::GeoComponent(GeoFullPhysVol* vol) :  
  m_fullPhysVol( vol),
  m_physVol(0)
{ 
  m_number=-1;
  //	m_transform3D=new HepGeom::Transform3D();
  m_transform3D = HepGeom::Transform3D();
  //	m_nameTag=new GeoNameTag(vol->getLogVol()->getName());
  m_nameTag=std::string(vol->getLogVol()->getName());
  m_bG4InsertFlag=false;
  m_bLock=false;
  m_bMatMapped=false;
  m_bOriginalObject=true;
  m_components.clear();
}

InDet::GeoComponent::GeoComponent(GeoPhysVol* vol) :  
  m_physVol(vol)
{ 
  
  m_fullPhysVol=0;
  //      	m_transform3D=new HepGeom::Transform3D();
  m_transform3D = HepGeom::Transform3D();
  //	m_nameTag=new GeoNameTag(vol->getLogVol()->getName());
  m_nameTag=vol->getLogVol()->getName();
  //	m_idTag=0;
  m_bG4InsertFlag=false;
  m_components.clear();
  m_bLock=false;
  m_bMatMapped=false;
  m_number=-1;
  m_bOriginalObject=true;
  
}

InDet::GeoComponent::GeoComponent(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  m_number(seqNum), m_bG4InsertFlag(bFlag)
{ 
  m_fullPhysVol= new GeoFullPhysVol(vol->getLogVol());
  m_physVol=0; 
  //	if(&trf==0)
  // 	  m_transform3D=new HepGeom::Transform3D();
  // 	else
  // 	  m_transform3D=new HepGeom::Transform3D(trf);
  m_transform3D=HepGeom::Transform3D(trf);
  //	m_idTag=0;
  //	m_nameTag=new GeoNameTag(name);
  m_nameTag=name;
  m_components.clear();
  m_bLock=false;
  m_bMatMapped=false;
  m_bOriginalObject=false;
}

InDet::GeoComponent::GeoComponent(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  m_number(seqNum), m_bG4InsertFlag(bFlag)
{ 
  m_physVol= new GeoPhysVol(vol->getLogVol());
  m_fullPhysVol=0; 
  // 	if(&trf==0)
  // 	  m_transform3D=new HepGeom::Transform3D();
  // 	else
  // 	  m_transform3D = new HepGeom::Transform3D(trf);
  m_transform3D=HepGeom::Transform3D(trf);
  //	m_idTag=0;
  //m_nameTag=new GeoNameTag(name);
  m_nameTag=name;
  m_components.clear();
  m_bLock=false;
  m_bMatMapped=false;
  m_bOriginalObject=false;
}

InDet::GeoComponent::~GeoComponent() 
{
   const std::vector<InDet::GeoComponent*> vec=components();
//   std::cout<<"~GeoComponent :  # objects : "<<vec.size()<<std::endl;

//   for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) if(*i) delete (*i);

  
//  std::cout<<"~GeoComponent :  delete  "<<name()<<"  "<<vec.size()<<std::endl;
//  if(m_bOriginalObject && m_transform3D) delete m_transform3D;

  //    delete m_fullPhysVol;    // FIXME : all destructors are private
  //  if(m_physVol) delete m_physVol;
  //    delete m_transform;
  //    delete m_idTag;
  //    delete m_nameTag;

}


void InDet::GeoComponent::UnRef() 
{
//   std::cout<<"~GeoComponent :  # objects : "<<vec.size()<<std::endl;
  
//  if(m_bOriginalObject && m_transform3D) delete m_transform3D;

//   std::cout<<"~GeoComponent :  unref  "<<name()<<"  "<<vec.size()<<std::endl;
//   for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) 
//     if(*i){
//       std::cout<<"     -- unref child of "<<name()<<" :  "<<(*i)->name()<<"  "<<std::endl;
//       (*i)->UnRef();
//      }
  
// // FIXME SES : need to declare GeoComponent as friend class in GeoModelKernel classes
// //   // Delete the complete physvol/fullPhysVol tree
// //   std::cout<<"------------ ~GeoComponent :  unref  object "<<name()<<std::endl;

// //   if(m_physVol) { /*std::cout<<"delete m_physVol"<<std::endl;*/ delete m_physVol; }
// //   if(m_fullPhysVol) { /*std::cout<<"delete m_fullPhysVol"<<std::endl;*/ delete m_fullPhysVol; }

  m_physVol=0;
  m_fullPhysVol=0;

//   // Delete the pointers
//   const std::vector<InDet::GeoComponent*> vec=components();
//   for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) if(*i) delete (*i);

//  std::cout<<"------------ ~GeoComponent :  unref  object "<<name()<<" -------------- DONE --------"<<std::endl;

}

void InDet::GeoComponent::setTransform3D(const HepGeom::Transform3D &trf)
{
   m_transform3D=HepGeom::Transform3D(trf);
}

// cree l'objet selon trf et num
InDet::GeoComponent* InDet::GeoComponent::place(const HepGeom::Transform3D& transform, int number, bool bG4InsertFlag) const
{
  GeoComponent* newComp=0;
  if(isPhysVol())
    newComp = new InDet::GeoComponent(this->physVolume<GeoPhysVol>(),number,transform,this->getNameTag(),bG4InsertFlag);
  else 
    if(isFullPhysVol())
      newComp = new InDet::GeoComponent(this->physVolume<GeoFullPhysVol>(),number,transform,this->getNameTag(),bG4InsertFlag);
    else{
      std::ostringstream msg;
      msg<<"InDet::GeoComponent* InDet::GeoComponent::clone : undefined object";
      std::cout<<msg.str()<<std::endl;
      throw msg.str();
    }

  const std::vector<InDet::GeoComponent*> vec=components();
  if(vec.size()==0) return newComp;
  
  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) {
    newComp->placeAsChild(*(*i),(*i)->getTransform3D(),(*i)->getSeqNumber());
  }

  return newComp;
}

// cree l'objet selon trf et num
InDet::GeoComponent* InDet::GeoComponent::placeAsChild(const GeoComponent& child, const HepGeom::Transform3D& transform, int number)
{
  
  GeoComponent* newComp = child.place(transform,number,child.getG4InsertFlag());  
  connectComponent(*newComp);
  return newComp;
}

InDet::GeoComponent* InDet::GeoComponent::placeAsChild_insert(const GeoComponent& child, const HepGeom::Transform3D& transform, int number)
{
  GeoComponent* newComp = child.place(transform,number,true);
  connectComponent(*newComp);
  return newComp;
}

InDet::GeoComponent* InDet::GeoComponent::clone() const
{
  GeoComponent* newComp=0;
  if(isPhysVol())
    newComp = new InDet::GeoComponent(this->physVolume<GeoPhysVol>(),
				      this->getSeqNumber(),(this->getTransform3D()),this->getNameTag(),this->getG4InsertFlag());
  else
    if(isFullPhysVol())
      newComp = new InDet::GeoComponent(this->physVolume<GeoFullPhysVol>(),
					this->getSeqNumber(),(this->getTransform3D()),this->getNameTag(),this->getG4InsertFlag());
    else{
      std::ostringstream msg;
      msg<<"InDet::GeoComponent* InDet::GeoComponent::clone : undefined object";
      std::cout<<msg.str()<<std::endl;
      throw msg.str();
    }


  const std::vector<InDet::GeoComponent*> vec=components();
  if(vec.size()==0) return newComp;
  
  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) {
    newComp->placeAsChild(*(*i),(*i)->getTransform3D(),(*i)->getSeqNumber());
  }

  return newComp;
}

// place l' objet dans la hierachie des (full )physvol et le sauve dans m_components
void InDet::GeoComponent::connectComponent(GeoComponent& child)
{

  if(isLocked()){
    std::ostringstream msg;
    msg<<"Geometry ERROR : could not add "<<child.getNameTag()<<" to "<<getNameTag()<<" / object already locked";
    std::cout<<msg.str()<<std::endl;
    throw msg.str();
  }

  child.lock();
  connectChildComponent(child);
  m_components.push_back(&child);

}

void InDet::GeoComponent::connectComponent(GeoComponent& child, const HepGeom::Transform3D &trf)
{

  if(isLocked()){
    std::ostringstream msg;
    msg<<"Geometry ERROR : could not add "<<child.getNameTag()<<" to "<<getNameTag()<<" / object already locked";
    std::cout<<msg.str()<<std::endl;
    throw msg.str();
  }

  child.setTransform3D(trf);
  child.lock();
  connectChildComponent(child);
  m_components.push_back(&child);

}

void InDet::GeoComponent::connectChildComponent( GeoComponent& child)
{

  if(m_physVol)
    {
      m_physVol->add( new GeoNameTag(child.m_nameTag) );
      m_physVol->add( new GeoTransform(child.m_transform3D) );
      if(child.isFullPhysVol())
	m_physVol->add( child.physVolume<GeoFullPhysVol>());
      else
	m_physVol->add( child.physVolume<GeoPhysVol>());
    }
  else if(m_fullPhysVol)
    {
      m_fullPhysVol->add( new GeoNameTag(child.m_nameTag) );
      m_fullPhysVol->add( new GeoTransform(child.m_transform3D) );
      if(child.isFullPhysVol())
	m_fullPhysVol->add( child.physVolume<GeoFullPhysVol>());
      else
	m_fullPhysVol->add( child.physVolume<GeoPhysVol>());
    }
  else {
    std::ostringstream msg;
    msg<<"InDet::GeoComponent* InDet::GeoComponent::connectChildComponent : undefined object";
    std::cout<<msg.str()<<std::endl;
    throw msg.str();
  }

}



void InDet::GeoComponent::deleteAllChildComponents()
{
//   // Loop over components of current GeoComponent
//   const std::vector<InDet::GeoComponent*> vec=components();
//   if(vec.size()==0) return;

//   for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) 
//     {
//       const std::vector<InDet::GeoComponent*> vecTmp=(*i)->components();
//       if(vecTmp.size()>0)
// 	{
// 	  (*i)->deleteAllChildComponents();
// 	}
//     }

}


// void InDet::GeoComponent::addAlignableComponent( GeoComponent& child)
// {
//   // FIXME
//   // maybe check order and duplicate numbers?
//   connectChild( child);
//   m_components.push_back( &child);
// }


void InDet::GeoComponent::dump(int iLevel) const
{
  
  // FIXME
  iLevel++;

  // Dump components
  std::cout<<"GeoComponent::Dump  : "<<this<<" "<<components().size()<<" components // name : "<<name()<<" // seqNum "<<getSeqNumber()<<" // G4 "<<getG4InsertFlag()<<std::endl;
  showTransform();
  dumpNames();
  const std::vector<InDet::GeoComponent*> vec=components();
  if(vec.size()==0) return;
  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) {
     (*i)->dump();
   }
}


void InDet::GeoComponent::dumpNames(int iLevel) const
{
  // Dump fierarchy of components names
  const std::vector<InDet::GeoComponent*> vec=components();

  const GeoComponent& component  = (*this);

  std::ostringstream os;
  os<<" type : ";
  //  try{
  //    const GeoEnvelope& tmp=dynamic_cast<const GeoEnvelope&>(component); 
  //    os <<"GeoEnvelope "<<tmp.getNameTag();
//   }
//   catch(...){}
 //  try{
//     const GeoBarrelAlpine& tmp=dynamic_cast<const GeoBarrelAlpine&>(component); 
//     os <<"GeoBarrelAlpine "<<tmp.getNameTag();
//   }
//   catch(...){}
//   try{
//     const GeoLayerAlpine& tmp=dynamic_cast<const GeoLayerAlpine&>(component); 
//     os <<"GeoLayerAlpine "<<tmp.getNameTag();
//   }
//   catch(...){}
//   try{
//     const GeoStaveAlpine& tmp=dynamic_cast<const GeoStaveAlpine&>(component); 
//     os <<"GeoStaveAlpine "<<tmp.getNameTag();
//   }
//   catch(...){}
  try{
    const GeoDetModule& tmp=dynamic_cast<const GeoDetModule&>(component); 
    os <<"GeoDetModule "<<tmp.getNameTag();
  }
  catch(...){}
  
  if(iLevel==0) std::cout<<"Dump "<<vec.size()<<" component names for "<<getNameTag()<<" / "<<os.str()<<std::endl;
  if(vec.size()==0) return;
  if(vec.size()>0)for(int j=0; j<iLevel;j++) std::cout<<"  ";

  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) {
    std::cout<<(*i)->getNameTag()<<"x"<<(*i)->getSeqNumber()<<" / ";
  }
  if(vec.size()>0)std::cout<<std::endl;

  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++)  {
      (*i)->dumpNames(iLevel+1);
    }

}

void InDet::GeoComponent::dumpAddresses(int iLevel) const
{
  // Dump fierarchy of components names
  const std::vector<InDet::GeoComponent*> vec=components();

  if(iLevel==0){
    std::string lockStatus="LCK";
    if(!this->isLocked()) lockStatus="UNLCK";

    std::string vol="";
    if(this->isPhysVol()) vol+="PV";
    if(this->isFullPhysVol()) vol+="FPV";
    if(!this->isPhysVol()&&!this->isFullPhysVol()) vol+="NoVol";

    std::cout<<"Dump "<<vec.size()<<" component adresses for "<<getNameTag()<<" "<<lockStatus<<" "<<vol<<" / "<<this<<std::endl;
  }
  if(vec.size()==0) return;
  if(vec.size()>0)for(int j=0; j<iLevel;j++) std::cout<<"  ";

  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) {
    std::string lockStatus="LCK";
    if(!(*i)->isLocked()) lockStatus="UNLCK";

    std::string vol="";
    if((*i)->isPhysVol()) vol+="PV";
    if((*i)->isFullPhysVol()) vol+="FPV";
    if(!(*i)->isPhysVol()&&!(*i)->isFullPhysVol()) vol+="NoVol";

    std::cout<<(*i)->getNameTag()<<"x"<<(*i)->getSeqNumber()<<" "<<(*i)<<" "<<lockStatus<<" "<<vol<<" / ";
  }
  if(vec.size()>0)std::cout<<std::endl;

  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++)  {
      (*i)->dumpAddresses(iLevel+1);
    }

}

void InDet::GeoComponent::showTransform() const
{

  HepGeom::Point3D<double> p1=HepGeom::Point3D<double>(1.00,0.0,0.0);
  HepGeom::Point3D<double> p2=HepGeom::Point3D<double>(0.0,1.00,0.0);
  HepGeom::Point3D<double> p3=HepGeom::Point3D<double>(0.0,0.0,1.00);

  const HepGeom::Transform3D& trf = this->getTransform3D(); 
  HepGeom::Rotate3D trfRot = trf.getRotation();
  HepGeom::Vector3D<double> trfTrans = trf.getTranslation();

  HepGeom::Point3D<double> p1_trf=trfRot*p1;
  HepGeom::Point3D<double> p2_trf=trfRot*p2;
  HepGeom::Point3D<double> p3_trf=trfRot*p3;

  if(fabs(p1_trf.x())<0.001)p1_trf.setX(0.0); if(fabs(p1_trf.y())<0.001)p1_trf.setY(0.0); if(fabs(p1_trf.z())<0.001)p1_trf.setZ(0.0);
  if(fabs(p2_trf.x())<0.001)p2_trf.setX(0.0); if(fabs(p2_trf.y())<0.001)p2_trf.setY(0.0); if(fabs(p2_trf.z())<0.001)p2_trf.setZ(0.0);
  if(fabs(p3_trf.x())<0.001)p3_trf.setX(0.0); if(fabs(p3_trf.y())<0.001)p3_trf.setY(0.0); if(fabs(p3_trf.z())<0.001)p3_trf.setZ(0.0);

  std::cout<<std::setprecision(3)<<p1_trf.x()<<"  "<<p2_trf.x()<<"  "<<p3_trf.x()<<"  -  "<<trfTrans.x()<<std::endl;
  std::cout<<std::setprecision(3)<<p1_trf.y()<<"  "<<p2_trf.y()<<"  "<<p3_trf.y()<<"  -  "<<trfTrans.y()<<std::endl;
  std::cout<<std::setprecision(3)<<p1_trf.z()<<"  "<<p2_trf.z()<<"  "<<p3_trf.z()<<"  -  "<<trfTrans.z()<<std::endl;

}



// HepGeom::Transform3D* InDet::GeoComponent::getTransform3D() const
// {
//   return m_transform3D;
// }

const HepGeom::Transform3D& InDet::GeoComponent::getTransform3D() const
{
  return m_transform3D;
}

const HepGeom::Transform3D& InDet::GeoComponent::getTransform() const
{
  return m_transform3D;
}

const std::string& InDet::GeoComponent::name() const
{
  return getNameTag();
}


double InDet::GeoComponent::getShape() const
{

  if(isFullPhysVol())
    return m_fullPhysVol->getLogVol()->getShape()->volume();
  else
    return m_physVol->getLogVol()->getShape()->volume();

  return 0.0;  // FIXME : should not happen   

}

std::string InDet::GeoComponent::getMaterialName() const
{

  if(isFullPhysVol())
    return m_fullPhysVol->getLogVol()->getMaterial()->getName();
  else
    return m_physVol->getLogVol()->getMaterial()->getName();

}

const GeoMaterial* InDet::GeoComponent::getMaterial() const
{

  if(isFullPhysVol())
    return (m_fullPhysVol->getLogVol()->getMaterial());
  else
    return (m_physVol->getLogVol()->getMaterial());

}

const std::string& InDet::GeoComponent::getNameTag() const
{
  return m_nameTag;    //->getName();
}



