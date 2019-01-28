/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/ExtendedAlignableTransform.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"
#include "StoreGate/StoreGateSvc.h"

#include <iostream>

namespace InDetDD {

  const int FIRST_HIGHER_LEVEL = 2;

  SCT_DetectorManager::SCT_DetectorManager( StoreGateSvc* detStore )
    : SiDetectorManager(detStore, "SCT"),
      m_idHelper(0),
      m_isLogical(false) // Change to true to change the definition of local module corrections
  {
    //  
    // Initialized the Identifier helper.
    //
    StatusCode sc = detStore->retrieve(m_idHelper, "SCT_ID");  
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not retrieve SCT id helper");
    }
    // Initialize the collections.
    if (m_idHelper) {
      m_elementCollection.resize(m_idHelper->wafer_hash_max());
      m_alignableTransforms.resize(m_idHelper->wafer_hash_max());
      m_moduleAlignableTransforms.resize(m_idHelper->wafer_hash_max()/2);
    } 
  }



  SCT_DetectorManager::~SCT_DetectorManager()
  {
    // Clean up
    for (size_t i=0; i < m_volume.size(); i++) {
      m_volume[i]->unref();
    }

    for (size_t j=0; j < m_higherAlignableTransforms.size(); j++){
      AlignableTransformMap::iterator iterMap;  
      for (iterMap = m_higherAlignableTransforms[j].begin(); 
           iterMap != m_higherAlignableTransforms[j].end();
           ++iterMap) {
        delete iterMap->second;
      }
    }

    for (size_t k=0; k < m_alignableTransforms.size(); k++){
      delete m_alignableTransforms[k];
    }

    for (size_t l=0; l < m_moduleAlignableTransforms.size(); l++){
      delete m_moduleAlignableTransforms[l];
    }
  }

  unsigned int SCT_DetectorManager::getNumTreeTops() const
  {
    return m_volume.size(); 
  }

  PVConstLink SCT_DetectorManager::getTreeTop(unsigned int i) const
  {
    return m_volume[i];
  }

  void SCT_DetectorManager::addTreeTop(PVLink vol){
    vol->ref();
    m_volume.push_back(vol);
  }


  SiDetectorElement* SCT_DetectorManager::getDetectorElement(const Identifier & id) const
  {  
    // NB the id helpers implementation for getting a hash is not optimal.
    // Essentially does a binary search.
    // Make sure it is a wafer Id
    Identifier waferId =  m_idHelper->wafer_id(id);
    IdentifierHash idHash = m_idHelper->wafer_hash(waferId);
    if (idHash.is_valid()) {
      return m_elementCollection[idHash];
    } else {
      return 0;
    }
  }

  SiDetectorElement* SCT_DetectorManager::getDetectorElement(const IdentifierHash & idHash) const
  {
    return m_elementCollection[idHash];
  }

  SiDetectorElement* SCT_DetectorManager::getDetectorElement(int barrel_endcap, int layer_wheel, int phi_module, int eta_module, int side) const
  {
    return getDetectorElement(m_idHelper->wafer_id(barrel_endcap, layer_wheel, phi_module, eta_module, side));
  }

  const SiDetectorElementCollection* SCT_DetectorManager::getDetectorElementCollection() const
  { 
    return &m_elementCollection;
  }

  SiDetectorElementCollection::const_iterator SCT_DetectorManager::getDetectorElementBegin() const
  {
    return m_elementCollection.begin();
  }

  SiDetectorElementCollection::const_iterator SCT_DetectorManager::getDetectorElementEnd() const
  {
    return m_elementCollection.end();
  }


  void SCT_DetectorManager::addDetectorElement(SiDetectorElement * element)
  {
    IdentifierHash idHash = element->identifyHash();
    if (idHash >=  m_elementCollection.size())
      throw std::runtime_error("SCT_DetectorManager: Error adding detector element.");
    m_elementCollection[idHash] = element;
  }

  void SCT_DetectorManager::initNeighbours()
  {
    SiDetectorElementCollection::iterator iter;

    // Loop over all elements and set the neighbours
    for (iter = m_elementCollection.begin(); iter != m_elementCollection.end(); ++iter){

      SiDetectorElement * element = *iter;
      if (element) {

        IdentifierHash idHash = element->identifyHash();
        IdentifierHash idHashOther;

        int result;
        // If no neighbour, result != 0 in which case we leave neighbour as null
        result = m_idHelper->get_next_in_eta(idHash, idHashOther);
        if (result==0) element->setNextInEta(m_elementCollection[idHashOther]);

        result = m_idHelper->get_prev_in_eta(idHash, idHashOther);
        if (result==0) element->setPrevInEta(m_elementCollection[idHashOther]);

        result = m_idHelper->get_next_in_phi(idHash, idHashOther);
        if (result==0) element->setNextInPhi(m_elementCollection[idHashOther]);

        result = m_idHelper->get_prev_in_phi(idHash, idHashOther);
        if (result==0) element->setPrevInPhi(m_elementCollection[idHashOther]);

        result = m_idHelper->get_other_side(idHash, idHashOther);
        if (result==0) element->setOtherSide(m_elementCollection[idHashOther]);
      }
    }
  }


  const SCT_ID* SCT_DetectorManager::getIdHelper() const
  {
    return m_idHelper;
  }


  bool SCT_DetectorManager::setAlignableTransformDelta(int level, 
                                                       const Identifier & id, 
                                                       const Amg::Transform3D & delta,
                                                       FrameType frame,
                                                       GeoVAlignmentStore* alignStore) const
  {

    if (level == 0) { // 0 - At the element level

      // We retrieve it via a hashId.
      IdentifierHash idHash = m_idHelper->wafer_hash(id);
      if (!idHash.is_valid()) return false;

      if (frame == InDetDD::global) { // global shift
        // Its a global transform
        return setAlignableTransformGlobalDelta(m_alignableTransforms[idHash], delta, alignStore);

      } else if (frame == InDetDD::local) { // local shift

        const SiDetectorElement * element =  m_elementCollection[idHash];
        if (!element) return false;


        // Its a local transform
        //See header file for definition of m_isLogical          
        if( m_isLogical ){
          //Ensure cache is up to date and use the alignment corrected local to global transform
          element->updateCache();
          return setAlignableTransformLocalDelta(m_alignableTransforms[idHash], element->transform(), delta, alignStore);
        } else 
          //Use default local to global transform
          return setAlignableTransformLocalDelta(m_alignableTransforms[idHash], element->defTransform(), delta, alignStore);

      } else {   
        // other not supported
        ATH_MSG_WARNING("Frames other than global or local are not supported.");
        return false;
      }

    } else if (level == 1) { // module level

      // We retrieve it via a hashId.
      IdentifierHash idHash = m_idHelper->wafer_hash(id);
      if (!idHash.is_valid()) return false;

      int idModuleHash = idHash / 2;

      if (idHash%2) {
        ATH_MSG_WARNING("Side 1 wafer id used for module id");
        return false;
      }

      if (frame == InDetDD::global) { // global shift
        // Its a global transform
        return setAlignableTransformGlobalDelta(m_moduleAlignableTransforms[idModuleHash], delta, alignStore);
      } else if (frame == InDetDD::local) { // local shift
        const SiDetectorElement * element =  m_elementCollection[idHash];
        if (!element) return false;
       
        // Its a local transform
        //See header file for definition of m_isLogical          
        if( m_isLogical ){
          //Ensure cache is up to date and use the alignment corrected local to global transform
          element->updateCache();
          return setAlignableTransformLocalDelta(m_moduleAlignableTransforms[idModuleHash], element->moduleTransform(), delta, alignStore);
        } else 
          //Use default local to global transform
          return setAlignableTransformLocalDelta(m_moduleAlignableTransforms[idModuleHash],element->defModuleTransform(), delta, alignStore);

      } else {
        // other not supported
        ATH_MSG_WARNING("Frames other than global or local are not supported.");
        return false;
      }

    } else { // higher level

      if (frame != InDetDD::global) {
        ATH_MSG_WARNING("Non global shift at higher levels is not supported.");
        return false;
      }

      int index = level - FIRST_HIGHER_LEVEL; // level 0 and 1 is treated separately.
      if (index  >=  static_cast<int>(m_higherAlignableTransforms.size())) return false;

      // We retrieve it from a map. 
      AlignableTransformMap::const_iterator iter;    
      iter = m_higherAlignableTransforms[index].find(id);
      if (iter == m_higherAlignableTransforms[index].end()) return false;      

      // Its a global transform
      return setAlignableTransformGlobalDelta(iter->second, delta, alignStore);
    }

  }

  void SCT_DetectorManager::addAlignableTransform (int level, 
                                                   const Identifier & id, 
                                                   GeoAlignableTransform *transform,
                                                   const GeoVPhysVol * child)
  {
    if (m_idHelper) {

      const GeoVFullPhysVol * childFPV = dynamic_cast<const GeoVFullPhysVol *>(child);
      if (!childFPV) { 
        ATH_MSG_ERROR("Child of alignable transform is not a full physical volume");
      } else {
        addAlignableTransform (level, id, transform, childFPV);
      }
    }
  }

  void SCT_DetectorManager::addAlignableTransform (int level, 
                                                   const Identifier & id, 
                                                   GeoAlignableTransform *transform,
                                                   const GeoVFullPhysVol * child)
  { 
    if (m_idHelper) {
      if (level == 0) { 
        // Element
        IdentifierHash idHash = m_idHelper->wafer_hash(id);
        if (idHash.is_valid()) {
          m_alignableTransforms[idHash]= new ExtendedAlignableTransform(transform, child);
        } 
      } else if (level == 1) {
        // Module
        IdentifierHash idHash = m_idHelper->wafer_hash(id);
        if (idHash.is_valid()) {
          m_moduleAlignableTransforms[idHash/2]=new ExtendedAlignableTransform(transform, child);
        } 

      } else {

        // Higher levels are saved in a map. NB level=0,1 is treated above.   
        int index = level - FIRST_HIGHER_LEVEL; // level 0 and 1 is treated separately.
        if (index >= static_cast<int>(m_higherAlignableTransforms.size())) m_higherAlignableTransforms.resize(index+1); 
        m_higherAlignableTransforms[index][id] = new ExtendedAlignableTransform(transform, child);
      }  
    }
  }

  bool
  SCT_DetectorManager::identifierBelongs(const Identifier & id) const
  {
    return getIdHelper()->is_sct(id);
  }


  const SCT_ModuleSideDesign* SCT_DetectorManager::getSCT_Design(int i) const
  {
    return dynamic_cast<const SCT_ModuleSideDesign *>(getDesign(i));
  }

  // New global alignment folders
  bool SCT_DetectorManager::processGlobalAlignment(const std::string & key, int level, FrameType frame,
                                                   const CondAttrListCollection* obj, GeoVAlignmentStore* alignStore) const
  {
    ATH_MSG_INFO("Processing new global alignment containers with key " << key << " in the " << frame << " frame at level ");

    const CondAttrListCollection* atrlistcol=obj;
    if(atrlistcol==nullptr and m_detStore->retrieve(atrlistcol,key)!=StatusCode::SUCCESS) {
      ATH_MSG_INFO("Cannot find new global align Container for key "
                   << key << " - no new global alignment ");
      return false;
    }

    bool alignmentChange = false;
    Identifier ident=Identifier();

    // loop over objects in collection
    for (CondAttrListCollection::const_iterator citr=atrlistcol->begin(); citr!=atrlistcol->end();++citr) {
      const coral::AttributeList& atrlist=citr->second;
      // SCT manager, therefore ignore all that is not a SCT Identifier
      if (atrlist["det"].data<int>()!=2) continue;

      ident = getIdHelper()->wafer_id(atrlist["bec"].data<int>(),
                                      atrlist["layer"].data<int>(),
                                      atrlist["ring"].data<int>(),
                                      atrlist["sector"].data<int>(),
                                      0); // The last is the module side which is at this ident-level always the 0-side

      // construct new transform
      // Order of rotations is defined as around z, then y, then x.
      Amg::Translation3D  newtranslation(atrlist["Tx"].data<float>(),atrlist["Ty"].data<float>(),atrlist["Tz"].data<float>());
      Amg::Transform3D newtrans = newtranslation * Amg::RotationMatrix3D::Identity();
      newtrans *= Amg::AngleAxis3D(atrlist["Rz"].data<float>()*CLHEP::mrad, Amg::Vector3D(0.,0.,1.));
      newtrans *= Amg::AngleAxis3D(atrlist["Ry"].data<float>()*CLHEP::mrad, Amg::Vector3D(0.,1.,0.));
      newtrans *= Amg::AngleAxis3D(atrlist["Rx"].data<float>()*CLHEP::mrad, Amg::Vector3D(1.,0.,0.));

      ATH_MSG_DEBUG("New global DB -- channel: " << citr->first
                    << " ,det: "    << atrlist["det"].data<int>()
                    << " ,bec: "    << atrlist["bec"].data<int>()
                    << " ,layer: "  << atrlist["layer"].data<int>()
                    << " ,ring: "   << atrlist["ring"].data<int>()
                    << " ,sector: " << atrlist["sector"].data<int>()
                    << " ,Tx: "     << atrlist["Tx"].data<float>()
                    << " ,Ty: "     << atrlist["Ty"].data<float>()
                    << " ,Tz: "     << atrlist["Tz"].data<float>()
                    << " ,Rx: "     << atrlist["Rx"].data<float>()
                    << " ,Ry: "     << atrlist["Ry"].data<float>()
                    << " ,Rz: "     << atrlist["Rz"].data<float>());

      // Set the new transform; Will replace existing one with updated transform
      bool status = setAlignableTransformDelta(level,
                                               ident,
                                               newtrans,
                                               frame,
                                               alignStore);

      if (!status) {
        ATH_MSG_DEBUG("Cannot set AlignableTransform for identifier."
                      << getIdHelper()->show_to_string(ident)
                      << " at level " << level << " for new global DB ");
      }

      alignmentChange = (alignmentChange || status);
    }
    return alignmentChange;
  }


} // namespace InDetDD
