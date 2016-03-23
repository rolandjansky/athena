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
  const TileTBID* tiletb_id;

  // Event data
  const TileCellContainer* mbts_container;

  // VP1Mbts objects of this event
  std::vector<VP1Mbts*> vp1_mbts;

  // Maps
  VP1CC_MbtsScinInfoMap  scinInfoMap;
  VP1CC_SoNode2MbtsMap   node2mbtsMap;

  // Separator helper
  SoSeparator* separator;
  
	VP1CaloCellController* controller;
};


VP1MbtsHelper::VP1MbtsHelper(bool outline):
  _clockwork(new Clockwork()),
  m_outline(outline),
  m_clipRadius(10e8),
  m_run2Geo(false)
{
  _clockwork->tiletb_id = 0;
  _clockwork->mbts_container = 0;
  _clockwork->separator = 0;
}


VP1MbtsHelper::~VP1MbtsHelper()
{
  delete _clockwork;
  _clockwork = 0;
}

void VP1MbtsHelper::systemcreate(StoreGateSvc* detstore)
{
  // Initialize identifier helper
  StatusCode status = detstore->retrieve(_clockwork->tiletb_id);
  if(status.isFailure() || _clockwork->tiletb_id==0)
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
  GeoFullPhysVol* _pvLArECA = storedLArECA->getPhysVol();
  GeoFullPhysVol* _pvLArECC = storedLArECC->getPhysVol();

  HepGeom::Transform3D _xfLArECA = _pvLArECA->getAbsoluteTransform();
  HepGeom::Transform3D _xfLArECC = _pvLArECC->getAbsoluteTransform();

  // Find a pointer to MBTS mother volume and its local transform
  HepGeom::Transform3D _xfMbtsMother;
  PVConstLink _pvMbtsMother;
  bool mbtsExists = false;

  GeoVolumeCursor _cursor1(_pvLArECA);
  while(!_cursor1.atEnd())
  {
    PVConstLink _child = _cursor1.getVolume();
    if(_child->getLogVol()->getName() == "MBTS_mother")
    {
      _pvMbtsMother = _child;
      _xfMbtsMother = _cursor1.getTransform();
      mbtsExists = true;
      break;
    }
    _cursor1.next();
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
                                                                                                                   
  GeoVolumeCursor _cursor2(_pvMbtsMother);

  PVConstLink pvScin1;
  PVConstLink pvScin2;

  bool scin1Exists = false;
  bool scin2Exists = false;

  while(!_cursor2.atEnd())
  {
    PVConstLink _child = _cursor2.getVolume();
    std::string _childName = _child->getLogVol()->getName();
    if(_childName.find("MBTSAirEnv")!=std::string::npos) {                                                         
      m_run2Geo = true;                                                                                               
      break;                                                                                                       
    }                                                                                                              
    if(_childName == "MBTS1")
    {
      scin1Exists = true;
      pvScin1 = _child;
      int _copyNo=  _cursor2.getId();

      aTransforms1[_copyNo] = _xfLArECA * _xfMbtsMother * _cursor2.getTransform();
      cTransforms1[_copyNo] = _xfLArECC * _xfMbtsMother * _cursor2.getTransform();
    }
    else if(_childName == "MBTS2")
    {
      scin2Exists = true;
      pvScin2 = _child;
      int _copyNo=  _cursor2.getId();

      aTransforms2[_copyNo] = _xfLArECA * _xfMbtsMother * _cursor2.getTransform();
      cTransforms2[_copyNo] = _xfLArECC * _xfMbtsMother * _cursor2.getTransform();
    }
    _cursor2.next();
  }

  if(m_run2Geo) {                                                                                                     
    GeoVolumeCursor _cursor2a(_pvMbtsMother);                                                                      
    while(!_cursor2a.atEnd()) {                                                                                    
      PVConstLink _pvAirEnv = _cursor2a.getVolume();                                                               
      if(_pvAirEnv->getLogVol()->getName().find("MBTSAirEnv")!=std::string::npos) {                                
       int _copyNo =  _cursor2a.getId();                                                                           
                                                                                                                   
       // **** Find Aluminun Envelope ****                                                                         
       GeoVolumeCursor _cursor3(_pvAirEnv);                                                                        
       bool aluEnvExists(false);                                                                                   
       while(!_cursor3.atEnd()) {                                                                                  
         if(_cursor3.getVolume()->getLogVol()->getName().find("MBTSAluEnv")!=std::string::npos) {                  
           aluEnvExists = true;                                                                                    
           break;                                                                                                  
         }                                                                                                         
         _cursor3.next();                                                                                          
       }                                                                                                           
       if(!aluEnvExists)                                                                                           
         throw std::runtime_error("Problems with MBTS geometry: Cannot find MBTSAluEnv!");                         
                                                                                                                   
       PVConstLink _pvAluEnv = _cursor3.getVolume();
                                                                                                                   
       // **** Find "Air in Aluminum" ****                                                                         
       GeoVolumeCursor _cursor4(_pvAluEnv);                                                                        
       bool airInAluExists(false);                                                                                 
       while(!_cursor4.atEnd()) {
         if(_cursor4.getVolume()->getLogVol()->getName().find("MBTSAirInAlu")!=std::string::npos) {
           airInAluExists = true;
           break;
         }
         _cursor4.next();
       }
       if(!airInAluExists)
         throw std::runtime_error("Problems with MBTS geometry: Cannot find MBTSAirInAlu!");

       PVConstLink _pvAirInAluEnv = _cursor4.getVolume();

       // **** Find scintillators ****
       GeoVolumeCursor _cursor5(_pvAirInAluEnv);
       while(!_cursor5.atEnd()) {
         PVConstLink _child = _cursor5.getVolume();
         if(_child->getLogVol()->getName()=="MBTS1") {
           scin1Exists = true;
           pvScin1 = _child;
           aTransforms1[_copyNo] = _xfLArECA * _xfMbtsMother * _cursor2a.getTransform() * _cursor3.getTransform() * _cursor4.getTransform() * _cursor5.getTransform();
           cTransforms1[_copyNo] = _xfLArECC * _xfMbtsMother * _cursor2a.getTransform() * _cursor3.getTransform() * _cursor4.getTransform() * _cursor5.getTransform();
         }
         if(_child->getLogVol()->getName()=="MBTS2") {
           scin2Exists = true;
           pvScin2 = _child;
           aTransforms2[_copyNo] = _xfLArECA * _xfMbtsMother * _cursor2a.getTransform() * _cursor3.getTransform() * _cursor4.getTransform() * _cursor5.getTransform();
           cTransforms2[_copyNo] = _xfLArECC * _xfMbtsMother * _cursor2a.getTransform() * _cursor3.getTransform() * _cursor4.getTransform() * _cursor5.getTransform();
         }
         _cursor5.next();
       }
      } // Dealing with MBTSAirEnv
      _cursor2a.next();
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


  _clockwork->scinInfoMap[0] = scinInfo1;
  _clockwork->scinInfoMap[1] = scinInfo2;
}

void VP1MbtsHelper::buildEventSceneGraph(StoreGateSvc* sg, SoSeparator* root)
{
  // This method is called only once per event
  // It does not actually build anything for 3D scene
  // The method retrieves MBTS Container and builds corresponding VP1Mbts objects
  _clockwork->separator = root;

  StatusCode status = sg->retrieve(_clockwork->mbts_container,"MBTSContainer");
  if(status.isFailure() || _clockwork->mbts_container == 0)
  {
    _clockwork->mbts_container = 0;
    throw std::runtime_error("Unable to retrieve MBTS Container");
  }

  TileCellContainer::const_iterator it = _clockwork->mbts_container->begin();

  for(; it!=_clockwork->mbts_container->end(); it++)
  {
    VP1Mbts* _mbts = new VP1Mbts(*it,_clockwork->tiletb_id,root,m_run2Geo);
    _clockwork->vp1_mbts.push_back(_mbts);
  }
}

void VP1MbtsHelper::systemerase()
{
 if(_clockwork->separator)
    _clockwork->separator->removeAllChildren();
  _clockwork->node2mbtsMap.clear();

  // Clear VP1Mbts vector
  for(size_t ii=0; ii < _clockwork->vp1_mbts.size(); ii++)
    delete _clockwork->vp1_mbts[ii];
  _clockwork->vp1_mbts.clear();

  _clockwork->mbts_container = 0;
  _clockwork->separator = 0;

}

void VP1MbtsHelper::refreshGraph(const VP1Interval& interval)
{
  // Remove all previously created objects
 if(_clockwork->separator)
    _clockwork->separator->removeAllChildren();
  _clockwork->node2mbtsMap.clear();

  // Loop over all VP1Mbts elements
  if(!interval.isEmpty())
    for(size_t i=0; i<_clockwork->vp1_mbts.size(); i++)
      _clockwork->vp1_mbts[i]->UpdateScene(&_clockwork->scinInfoMap, &_clockwork->node2mbtsMap, interval.lower(),m_outline, m_clipRadius);
}

void VP1MbtsHelper::setController( VP1CaloCellController* controller ){
  _clockwork->controller = controller;
}

void VP1MbtsHelper::clipVolumeRadiusChanged(double radius)
{
  std::cout<<" clipVolumeRadiusChanged to "<<radius<<std::endl;
  m_clipRadius=radius; // Adjust radius to match new value
  refreshGraph(_clockwork->controller->selectionMbts());
}

std::vector<std::string> VP1MbtsHelper::userPickedNode(SoNode* pickedNode)
{
  // std::cout << "VP1MbtsHelper::userPickedNode()..." << std::endl;

  std::vector<std::string> result;
  VP1CC_SoNode2MbtsMap::iterator it = _clockwork->node2mbtsMap.find(pickedNode);
  if(it!=_clockwork->node2mbtsMap.end())
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
  VP1CC_SoNode2MbtsMap::iterator it = _clockwork->node2mbtsMap.begin();
  for(; it!=_clockwork->node2mbtsMap.end(); it++) {
    SoGenericBox* _genBox = dynamic_cast<SoGenericBox*>(it->first);
    if(_genBox)
      _genBox->drawEdgeLines = m_outline;
  }
}
