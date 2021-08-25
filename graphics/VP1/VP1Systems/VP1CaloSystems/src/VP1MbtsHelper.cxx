/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1CaloSystems/VP1MbtsHelper.h"
#include "VP1CaloSystems/VP1CaloCells.h"
#include "VP1CaloSystems/VP1CaloCellController.h"

#include "VP1HEPVis/nodes/SoGenericBox.h"

#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSeparator.h>

#include "StoreGate/StoreGateSvc.h"

#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPVConstLink.h"
#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoTrd.h"

#include "TileEvent/TileCellContainer.h"
#include "CaloIdentifier/TileTBID.h"

#include <stdexcept>

class VP1MbtsHelper::Clockwork
{
 public:
  // ID helper
  const TileTBID* tiletb_id = nullptr;

  // Event data
  const TileCellContainer* mbts_container = nullptr;

  // VP1Mbts objects of this event
  std::vector<VP1Mbts*> vp1_mbts;

  // Maps
  VP1CC_MbtsScinInfoMap  scinInfoMap;
  VP1CC_SoNode2MbtsMap   node2mbtsMap;

  // Separator helper
  SoSeparator* separator = nullptr;
  
	VP1CaloCellController* controller = nullptr;
};


VP1MbtsHelper::VP1MbtsHelper(bool outline):
  m_clockwork(new Clockwork()),
  m_outline(outline),
  m_clipRadius(10e8),
  m_run2Geo(false)
{
  m_clockwork->tiletb_id = 0;
  m_clockwork->mbts_container = 0;
  m_clockwork->separator = 0;
}


VP1MbtsHelper::~VP1MbtsHelper()
{
  delete m_clockwork;
  m_clockwork = 0;
}

void VP1MbtsHelper::systemcreate(StoreGateSvc* detstore)
{
  // Initialize identifier helper
  StatusCode status = detstore->retrieve(m_clockwork->tiletb_id);
  if(status.isFailure() || m_clockwork->tiletb_id==0)
    throw std::runtime_error("Unable to retrieve TileTBID");


  VP1CC_MbtsXfMap aTransforms1, aTransforms2, cTransforms1, cTransforms2;

  // Get Stored Full phys volumes for LAr end caps
  StoredPhysVol* storedLArECA = 0;
  StoredPhysVol* storedLArECC = 0;

  status = detstore->retrieve(storedLArECA,"LARCRYO_EC_POS");
  if(status.isFailure() || storedLArECA == 0)
    throw std::runtime_error("Failed to locate LAr Endcap A");

  status = detstore->retrieve(storedLArECC,"LARCRYO_EC_NEG");
  if(status.isFailure() || storedLArECC == 0)
    throw std::runtime_error("Failed to locate LAr Endcap C");


  // Get full physical volumes and their absolute transforms
  GeoFullPhysVol* pvLArECA = storedLArECA->getPhysVol();
  GeoFullPhysVol* pvLArECC = storedLArECC->getPhysVol();

  GeoTrf::Transform3D xfLArECA = pvLArECA->getAbsoluteTransform();
  GeoTrf::Transform3D xfLArECC = pvLArECC->getAbsoluteTransform();

  // Find a pointer to MBTS mother volume and its local transform
  GeoTrf::Transform3D xfMbtsMother;
  PVConstLink pvMbtsMother;
  bool mbtsExists = false;

  GeoVolumeCursor cursor1(pvLArECA);
  while(!cursor1.atEnd())
  {
    PVConstLink child = cursor1.getVolume();
    if(child->getLogVol()->getName() == "MBTS_mother")
    {
      pvMbtsMother = child;
      xfMbtsMother = cursor1.getTransform();
      mbtsExists = true;
      break;
    }
    cursor1.next();
  }

  if(!mbtsExists)
    throw std::runtime_error("Unable to find MBTS volume");

  // Find shapes of scintillators and their local transforms
  // Here we need to distinguish between two geometry description: "OLD" and "NEW"                                 
  //                                                                                                               
  // OLD description: scintillator volumes are placed directly into the mother.                                    
  //                  each scintillator colume has copy number assigned to it                                      
  //                                                                                                               
  // NEW description: here is the volume hierarchy tree leading from MBTS_mother to scintillators:                 
  //                                                                                                               
  //  MBTS_mother                                                                                                  
  //    MBTSAirEnv <--- Copy numbers                                                                               
  //      MBTSAluEnv                                                                                               
  //        MBTSAirInAlu                                                                                           
  //          MBTS1                                                                                                
  //          MBTS1                                                                                                
                                                                                                                   
  GeoVolumeCursor cursor2(pvMbtsMother);

  PVConstLink pvScin1;
  PVConstLink pvScin2;

  bool scin1Exists = false;
  bool scin2Exists = false;

  while(!cursor2.atEnd())
  {
    PVConstLink child = cursor2.getVolume();
    std::string childName = child->getLogVol()->getName();
    if(childName.find("MBTSAirEnv")!=std::string::npos) {                                                         
      m_run2Geo = true;                                                                                               
      break;                                                                                                       
    }                                                                                                              
    if(childName == "MBTS1")
    {
      scin1Exists = true;
      pvScin1 = child;
      int copyNo=  cursor2.getId();

      aTransforms1[copyNo] = xfLArECA * xfMbtsMother * cursor2.getTransform();
      cTransforms1[copyNo] = xfLArECC * xfMbtsMother * cursor2.getTransform();
    }
    else if(childName == "MBTS2")
    {
      scin2Exists = true;
      pvScin2 = child;
      int copyNo=  cursor2.getId();

      aTransforms2[copyNo] = xfLArECA * xfMbtsMother * cursor2.getTransform();
      cTransforms2[copyNo] = xfLArECC * xfMbtsMother * cursor2.getTransform();
    }
    cursor2.next();
  }

  if(m_run2Geo) {                                                                                                     
    GeoVolumeCursor cursor2a(pvMbtsMother);                                                                      
    while(!cursor2a.atEnd()) {                                                                                    
      PVConstLink pvAirEnv = cursor2a.getVolume();                                                               
      if(pvAirEnv->getLogVol()->getName().find("MBTSAirEnv")!=std::string::npos) {                                
       int copyNo =  cursor2a.getId();                                                                           
                                                                                                                   
       // **** Find Aluminun Envelope ****                                                                         
       GeoVolumeCursor cursor3(pvAirEnv);                                                                        
       bool aluEnvExists(false);                                                                                   
       while(!cursor3.atEnd()) {                                                                                  
         if(cursor3.getVolume()->getLogVol()->getName().find("MBTSAluEnv")!=std::string::npos) {                  
           aluEnvExists = true;                                                                                    
           break;                                                                                                  
         }                                                                                                         
         cursor3.next();                                                                                          
       }                                                                                                           
       if(!aluEnvExists)                                                                                           
         throw std::runtime_error("Problems with MBTS geometry: Cannot find MBTSAluEnv!");                         
                                                                                                                   
       PVConstLink pvAluEnv = cursor3.getVolume();
                                                                                                                   
       // **** Find "Air in Aluminum" ****                                                                         
       GeoVolumeCursor cursor4(pvAluEnv);                                                                        
       bool airInAluExists(false);                                                                                 
       while(!cursor4.atEnd()) {
         if(cursor4.getVolume()->getLogVol()->getName().find("MBTSAirInAlu")!=std::string::npos) {
           airInAluExists = true;
           break;
         }
         cursor4.next();
       }
       if(!airInAluExists)
         throw std::runtime_error("Problems with MBTS geometry: Cannot find MBTSAirInAlu!");

       PVConstLink pvAirInAluEnv = cursor4.getVolume();

       // **** Find scintillators ****
       GeoVolumeCursor cursor5(pvAirInAluEnv);
       while(!cursor5.atEnd()) {
         PVConstLink child = cursor5.getVolume();
         if(child->getLogVol()->getName()=="MBTS1") {
           scin1Exists = true;
           pvScin1 = child;
           aTransforms1[copyNo] = xfLArECA * xfMbtsMother * cursor2a.getTransform() * cursor3.getTransform() * cursor4.getTransform() * cursor5.getTransform();
           cTransforms1[copyNo] = xfLArECC * xfMbtsMother * cursor2a.getTransform() * cursor3.getTransform() * cursor4.getTransform() * cursor5.getTransform();
         }
         if(child->getLogVol()->getName()=="MBTS2") {
           scin2Exists = true;
           pvScin2 = child;
           aTransforms2[copyNo] = xfLArECA * xfMbtsMother * cursor2a.getTransform() * cursor3.getTransform() * cursor4.getTransform() * cursor5.getTransform();
           cTransforms2[copyNo] = xfLArECC * xfMbtsMother * cursor2a.getTransform() * cursor3.getTransform() * cursor4.getTransform() * cursor5.getTransform();
         }
         cursor5.next();
       }
      } // Dealing with MBTSAirEnv
      cursor2a.next();
    } // Looping over MBTS mother daughters
  } // In New Geometry

  if(!scin1Exists)
    throw std::runtime_error("No MBTS1 in the geometry");

  if(!scin2Exists)
    throw std::runtime_error("No MBTS2 in the geometry");

  const GeoShape* shape1 = pvScin1->getLogVol()->getShape();
  const GeoShape* shape2 = pvScin2->getLogVol()->getShape();

  const GeoTrd* trd1 = dynamic_cast<const GeoTrd*>(shape1);
  const GeoTrd* trd2 = dynamic_cast<const GeoTrd*>(shape2);

  if(trd1==0)
    throw std::runtime_error("The shape of MBTS1 is not TRD");

  if(trd2==0)
    throw std::runtime_error("The shape of MBTS2 is not TRD");

  // Create new Scin Info objects and store them into map
  VP1CC_MbtsScinInfo* scinInfo1 = new VP1CC_MbtsScinInfo();
  VP1CC_MbtsScinInfo* scinInfo2 = new VP1CC_MbtsScinInfo();

  scinInfo1->dx1 = trd1->getXHalfLength1();
  scinInfo1->dx2 = trd1->getXHalfLength2();
  scinInfo1->dy1 = trd1->getYHalfLength1();
  scinInfo1->dy2 = trd1->getYHalfLength2();
  scinInfo1->dz = trd1->getZHalfLength();
  scinInfo1->aTransforms = aTransforms1;
  scinInfo1->cTransforms = cTransforms1;

  scinInfo2->dx1 = trd2->getXHalfLength1();
  scinInfo2->dx2 = trd2->getXHalfLength2();
  scinInfo2->dy1 = trd2->getYHalfLength1();
  scinInfo2->dy2 = trd2->getYHalfLength2();
  scinInfo2->dz = trd2->getZHalfLength();
  scinInfo2->aTransforms = aTransforms2;
  scinInfo2->cTransforms = cTransforms2;


  m_clockwork->scinInfoMap[0] = scinInfo1;
  m_clockwork->scinInfoMap[1] = scinInfo2;
}

void VP1MbtsHelper::buildEventSceneGraph(StoreGateSvc* sg, SoSeparator* root)
{
  // This method is called only once per event
  // It does not actually build anything for 3D scene
  // The method retrieves MBTS Container and builds corresponding VP1Mbts objects
  m_clockwork->separator = root;

  StatusCode status = sg->retrieve(m_clockwork->mbts_container,"MBTSContainer");
  if(status.isFailure() || m_clockwork->mbts_container == 0)
  {
    m_clockwork->mbts_container = 0;
    throw std::runtime_error("Unable to retrieve MBTS Container");
  }

  TileCellContainer::const_iterator it = m_clockwork->mbts_container->begin();

  for(; it!=m_clockwork->mbts_container->end(); it++)
  {
    VP1Mbts* mbts = new VP1Mbts(*it,m_clockwork->tiletb_id,root,m_run2Geo);
    m_clockwork->vp1_mbts.push_back(mbts);
  }
}

void VP1MbtsHelper::systemerase()
{
 if(m_clockwork->separator)
    m_clockwork->separator->removeAllChildren();
  m_clockwork->node2mbtsMap.clear();

  // Clear VP1Mbts vector
  for(size_t ii=0; ii < m_clockwork->vp1_mbts.size(); ii++)
    delete m_clockwork->vp1_mbts[ii];
  m_clockwork->vp1_mbts.clear();

  m_clockwork->mbts_container = 0;
  m_clockwork->separator = 0;

}

void VP1MbtsHelper::refreshGraph(const VP1Interval& interval)
{
  // Remove all previously created objects
 if(m_clockwork->separator)
    m_clockwork->separator->removeAllChildren();
  m_clockwork->node2mbtsMap.clear();

  // Loop over all VP1Mbts elements
  if(!interval.isEmpty())
    for(size_t i=0; i<m_clockwork->vp1_mbts.size(); i++)
      m_clockwork->vp1_mbts[i]->UpdateScene(&m_clockwork->scinInfoMap, &m_clockwork->node2mbtsMap, interval.lower(),m_outline, m_clipRadius);
}

void VP1MbtsHelper::setController( VP1CaloCellController* controller ){
  m_clockwork->controller = controller;
}

void VP1MbtsHelper::clipVolumeRadiusChanged(double radius)
{
  std::cout<<" clipVolumeRadiusChanged to "<<radius<<std::endl;
  m_clipRadius=radius; // Adjust radius to match new value
  refreshGraph(m_clockwork->controller->selectionMbts());
}

std::vector<std::string> VP1MbtsHelper::userPickedNode(SoNode* pickedNode)
{
  // std::cout << "VP1MbtsHelper::userPickedNode()..." << std::endl;

  std::vector<std::string> result;
  VP1CC_SoNode2MbtsMap::iterator it = m_clockwork->node2mbtsMap.find(pickedNode);
  if(it!=m_clockwork->node2mbtsMap.end())
    result = it->second->ToString();

  return result;
}

void VP1MbtsHelper::selectionUpdated(const VP1Interval& interval)
{
  refreshGraph(interval);
}

void VP1MbtsHelper::outlineUpdate(const bool& outline)
{
  m_outline = outline;
  VP1CC_SoNode2MbtsMap::iterator it = m_clockwork->node2mbtsMap.begin();
  for(; it!=m_clockwork->node2mbtsMap.end(); it++) {
    SoGenericBox* genBox = dynamic_cast<SoGenericBox*>(it->first);
    if(genBox)
      genBox->drawEdgeLines = m_outline;
  }
}
