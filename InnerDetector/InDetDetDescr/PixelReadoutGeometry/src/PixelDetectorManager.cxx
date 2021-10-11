/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DetDescrConditions/AlignableTransformContainer.h"
#include "DetDescrConditions/AlignableTransform.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "InDetIdentifier/PixelID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/ExtendedAlignableTransform.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "StoreGate/StoreGateSvc.h"

#include "GeoModelKernel/GeoVAlignmentStore.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "InDetGeoModelUtils/GeoNodePtr.h"

namespace InDetDD {

  const int FIRST_HIGHER_LEVEL = 1;


  PixelDetectorManager::PixelDetectorManager(StoreGateSvc* detStore,
                                             const std::string& name)
    : SiDetectorManager(detStore,name),
      m_idHelper(nullptr),
      m_isLogical(false) // Change to true to change the definition of local module corrections
  {
    //
    // Initialized the Identifier helper.
    //

    StatusCode sc = detStore->retrieve(m_idHelper,"PixelID");
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not retrieve Pixel id helper");
    }

    // Initialize the collections.
    if (m_idHelper) {
      m_elementCollection.resize(m_idHelper->wafer_hash_max());
      m_alignableTransforms.resize(m_idHelper->wafer_hash_max());
    }
  }

  PixelDetectorManager::PixelDetectorManager(StoreGateSvc* detStore)
     : PixelDetectorManager(detStore, "Pixel"){ }


  PixelDetectorManager::~PixelDetectorManager()
  {
    // Clean up
    for (size_t i=0; i < m_volume.size(); i++) {
      m_volume[i]->unref();
    }
  }



  unsigned int PixelDetectorManager::getNumTreeTops() const
  {
    return m_volume.size();
  }

  PVConstLink PixelDetectorManager::getTreeTop(unsigned int i) const
  {
    return m_volume[i];
  }

  void PixelDetectorManager::addTreeTop(PVLink vol){
    vol->ref();
    m_volume.push_back(vol);
  }


  SiDetectorElement* PixelDetectorManager::getDetectorElement(const Identifier & id) const
  {
  // NB the id helpers implementation for getting a hash is not optimal.
  // Essentially does a binary search.
  // Make sure it is a wafer Id
    Identifier waferId = m_idHelper->wafer_id(id);
    IdentifierHash idHash = m_idHelper->wafer_hash(waferId);
    if (idHash.is_valid()) {
      return m_elementCollection[idHash];
    } else {
      return nullptr;
    }
  }


  SiDetectorElement*  PixelDetectorManager::getDetectorElement(const IdentifierHash & idHash) const
  {
    return m_elementCollection[idHash];
  }

  SiDetectorElement* PixelDetectorManager::getDetectorElement(int barrel_endcap, int layer_wheel, int phi_module, int eta_module) const
  {
    return getDetectorElement(m_idHelper->wafer_id(barrel_endcap, layer_wheel, phi_module, eta_module));
  }

  const SiDetectorElementCollection* PixelDetectorManager::getDetectorElementCollection() const
  {
    return &m_elementCollection;
  }

  SiDetectorElementCollection::const_iterator PixelDetectorManager::getDetectorElementBegin() const
  {
    return m_elementCollection.begin();
  }

  SiDetectorElementCollection::const_iterator PixelDetectorManager::getDetectorElementEnd() const
  {
    return m_elementCollection.end();
  }


  void PixelDetectorManager::addDetectorElement(SiDetectorElement * element)
  {
    IdentifierHash idHash = element->identifyHash();
    if (idHash >=  m_elementCollection.size())
      throw std::runtime_error("PixelDetectorManager: Error adding detector element.");
    m_elementCollection[idHash] = element;
  }

  void PixelDetectorManager::initNeighbours()
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
      }
    }
  }


  const PixelID* PixelDetectorManager::getIdHelper() const
  {
    return m_idHelper;
  }

  unsigned int PixelDetectorManager::getBarrelLayers() const
  {
    unsigned int barrelLayers = 0;
    for (int i = numerology().numLayers() - 1; i >= 0; --i) {
      barrelLayers *= 10;
      barrelLayers += numerology().useLayer(i);
    }
    return barrelLayers;
  }

  unsigned int PixelDetectorManager::getEndcapLayers() const
  {
    unsigned int endcapLayers = 0;
    for (int i = numerology().numDisks() - 1; i >= 0; --i) {
      endcapLayers *= 10;
      endcapLayers += numerology().useDisk(i);
    }
    return endcapLayers;
  }


  bool PixelDetectorManager::setAlignableTransformDelta(int level,
                                                        const Identifier & id,
                                                        const Amg::Transform3D & delta,
                                                        FrameType frame,
                                                        GeoVAlignmentStore* alignStore) const
  {

    if (level == 0) { // At the element level - local shift

      // We retrieve it via a hashId.
      IdentifierHash idHash = m_idHelper->wafer_hash(id);
      if (!idHash.is_valid()) return false;

      if (frame == InDetDD::global) {

        return setAlignableTransformGlobalDelta(m_alignableTransforms[idHash].get(), delta, alignStore);

      } else if (frame == InDetDD::local) {

        SiDetectorElement * element =  m_elementCollection[idHash];
        if (!element) return false;

        // Its a local transform
        //See header file for definition of m_isLogical
        if( m_isLogical ){
    //Ensure cache is up to date and use the alignment corrected local to global transform
    element->setCache();
    return setAlignableTransformLocalDelta(m_alignableTransforms[idHash].get(), element->transform(), delta, alignStore);
        } else
    //Use default local to global transform
    return setAlignableTransformLocalDelta(m_alignableTransforms[idHash].get(), element->defTransform(), delta, alignStore);
      } else {
        // other not supported
        ATH_MSG_WARNING("Frames other than global or local are not supported.");
        return false;
      }

    } else { // higher level

      if (frame != InDetDD::global) {
        ATH_MSG_WARNING("Non global shift at higher levels is not possible.");
        return false;
      }

      int index = level - FIRST_HIGHER_LEVEL; // level 0 is treated separately.
      if (index >=  static_cast<int>(m_higherAlignableTransforms.size())) return false;

      // We retrieve it from a map.
      AlignableTransformMap::const_iterator iter;
      iter = m_higherAlignableTransforms[index].find(id);
      if (iter == m_higherAlignableTransforms[index].end()) return false;

      // Its a global transform
      return setAlignableTransformGlobalDelta((iter->second).get(), delta, alignStore);
    }

  }


  void PixelDetectorManager::addAlignableTransform (int level,
                            const Identifier & id,
                            GeoAlignableTransform *transform,
                            const GeoVPhysVol * child)
  {
    GeoNodePtr<GeoAlignableTransform> tmp_transform(transform);
    if (m_idHelper) {

      const GeoVFullPhysVol * childFPV = dynamic_cast<const GeoVFullPhysVol *>(child);
      if (!childFPV) {
        ATH_MSG_ERROR("Child of alignable transform is not a full physical volume");
      } else {
        addAlignableTransform (level, id, transform, childFPV);
      }
    }
  }

  void PixelDetectorManager::addAlignableTransform (int level,
                            const Identifier & id,
                            GeoAlignableTransform *transform,
                            const GeoVFullPhysVol * child)
  {
    GeoNodePtr<GeoAlignableTransform> tmp_transform(transform);
    if (m_idHelper) {
      if (level == 0) {
        IdentifierHash idHash = m_idHelper->wafer_hash(id);
        if (idHash.is_valid()) {
          m_alignableTransforms[idHash]= std::make_unique<ExtendedAlignableTransform>(transform, child);
        }
      } else {
        // Higher levels are saved in a map. NB the index is level-1 as level=0 is treated above.
        int index = level - FIRST_HIGHER_LEVEL;
        if (index >= static_cast<int>(m_higherAlignableTransforms.size())) m_higherAlignableTransforms.resize(index+1);
        m_higherAlignableTransforms[index][id] = std::make_unique<ExtendedAlignableTransform>(transform, child);
      }
    }
  }

  bool PixelDetectorManager::identifierBelongs(const Identifier & id) const
  {
    return getIdHelper()->is_pixel(id);
  }



  const PixelModuleDesign* PixelDetectorManager::getPixelDesign(int i) const
  {
    return dynamic_cast<const PixelModuleDesign *>(getDesign(i));
  }



  // The implementation of the new IBLDist DB;
  // Specific for IBL -> maybe make it different function in future to be more general
  bool PixelDetectorManager::processSpecialAlignment(const std::string & key, InDetDD::AlignFolderType alignfolderType) const
  {

  bool alignmentChange = false;

  std::string alignfolder;
  if (alignfolderType == InDetDD::static_run1) alignfolder = "/Indet/Align";
  if (alignfolderType == InDetDD::timedependent_run2) alignfolder = "/Indet/AlignL3";

  ATH_MSG_INFO("Processing IBLDist alignment container with key (" << key << ")"
               << "and alignment folder pointing to " << alignfolder);

  int nstaves = 0;
  if (numerology().numPhiModulesForLayer(0)<14) nstaves = 14;
  else nstaves = numerology().numPhiModulesForLayer(0);

  std::vector<float> ibldist;
  std::vector<float> iblbaseline;
  ibldist.resize(nstaves);
  iblbaseline.resize(nstaves);

  const CondAttrListCollection* atrlistcol=nullptr;
  if (StatusCode::SUCCESS==m_detStore->retrieve(atrlistcol,key)) {
    // loop over objects in collection
    for (CondAttrListCollection::const_iterator citr=atrlistcol->begin(); citr!=atrlistcol->end();++citr) {
      const coral::AttributeList& atrlist=citr->second;
      ibldist[atrlist["stave"].data<int>()] = atrlist["mag"].data<float>();
      iblbaseline[atrlist["stave"].data<int>()] = atrlist["base"].data<float>();

      ATH_MSG_VERBOSE("IBLDist DB -- channel: " << citr->first
                      << " ,stave: " << atrlist["stave"].data<int>()
                      << " ,mag: " << atrlist["mag"].data<float>()
                      << " ,base: " << atrlist["base"].data<float>());
    }
  }
  else {
    ATH_MSG_WARNING("Cannot find IBLDist Container for key "
                    << key << " - no IBL bowing alignment");
    return alignmentChange;
  }

  /**
    Matthias D. (Oct. 2016):
    The idea of this first implementation is to get the AlignTransforms for IBL modules from their folder;
    Calculate from the new DB entries the Tx displacement for this module;
    Make a simple transfrom in local frame;
    Add the Tx to the aligntransform;
    Apply this new transform with the setAlignableTransformDelta() function;
    This is non-optimal as it simply overrides the previous Delta (these are included in total Delta);
    Possibly we could extend this in future to add tweak() functionality to GeoAlignableTransform?
  **/

  const AlignableTransformContainer* container;
  if (StatusCode::SUCCESS!=m_detStore->retrieve(container, alignfolder)) {
    ATH_MSG_ERROR("Cannot find AlignableTransformContainer for key "
                  << key << " - no misalignment");
    // This should not occur in normal situations so we force job to abort.
    throw std::runtime_error("Unable to apply Inner Detector alignments");
  }
  // Check if container is empty - this can occur if it is an invalid IOV.
  if (container->empty()) {
    ATH_MSG_ERROR("AlignableTransformContainer for key "
                  << key << " is empty. Probably due to out of range IOV");
    // This should not occur in normal situations so we force job to abort.
    throw std::runtime_error("Unable to apply Inner Detector alignments.");
  }
  // loop over all the AlignableTransform objects in the collection
  std::string IBLalignfolder = alignfolder;
  IBLalignfolder.append("/PIXB1");// "/Indet/Align/PIXB1"
  for (DataVector<AlignableTransform>::const_iterator pat=container->begin();
       pat!=container->end();++pat)
  {
    if (!( (*pat)->tag()==IBLalignfolder &&
     numerology().numPhiModulesForLayer(0)==14 &&
     numerology().numLayers()==4) ){  // hard-coded to IBL for now; no other geometry should really apply this!
      ATH_MSG_DEBUG("IBLDist; ignoring collections " << (*pat)->tag());
    }
    else{
      const AlignableTransform* transformCollection = *pat;
      for (AlignableTransform::AlignTransMem_citr trans_iter = transformCollection->begin();
         trans_iter != transformCollection->end();
         ++trans_iter)
      {
        ATH_MSG_DEBUG("IBLDist alignment for identifier "
                      << getIdHelper()->show_to_string(trans_iter->identify()));

        IdentifierHash idHash = getIdHelper()->wafer_hash(trans_iter->identify());
        if (!idHash.is_valid()){
          ATH_MSG_WARNING("Invalid HashID for identifier "
                          << getIdHelper()->show_to_string(trans_iter->identify()));
          ATH_MSG_WARNING("No IBLDist corrections can be applied for invalid HashID's - exiting");
          return false;
        }
        SiDetectorElement * sielem = m_elementCollection[idHash];
        //This should work as Bowing is in L3 frame, i.e. local module frame
        Amg::Vector3D center = sielem->defTransform() * Amg::Vector3D{0, 0, 0};
        double z = center[2];
        const double  y0y0  = 366.5*366.5;

        double bowx = ibldist[getIdHelper()->phi_module(trans_iter->identify())] * ( z*z - y0y0 ) / y0y0;
        double basex= iblbaseline[getIdHelper()->phi_module(trans_iter->identify())];
        // This is in the module frame, as bowing corrections are directly L3

        ATH_MSG_DEBUG("Total IBL-module Tx shift (baseline+bowing): " << basex+bowx);
        if ( (basex+bowx)==0 ) continue; // make sure we ignore NULL corrections

        Amg::Transform3D shift = Amg::Translation3D(basex+bowx,0,0) * Amg::RotationMatrix3D::Identity();

        const AlignableTransform* cpat = *pat;
        AlignableTransform::AlignTransMem_citr this_trans=cpat->findIdent(trans_iter->identify());
        HepGeom::Transform3D newtrans = Amg::EigenTransformToCLHEP(shift)*this_trans->transform();

        /** Verbose level debug section for transforms **/
        if (msgLvl(MSG::VERBOSE)) {
          ATH_MSG_LVL_NOCHK(MSG::VERBOSE, "Bowing Transformation only:");
          printTransform(MSG::VERBOSE, shift);
          ATH_MSG_LVL_NOCHK(MSG::VERBOSE, "Original alignable Transformation from StoreGate:");
          printTransform(MSG::VERBOSE, Amg::CLHEPTransformToEigen(this_trans->transform()));
          ATH_MSG_LVL_NOCHK(MSG::VERBOSE, "Final mModified Transformation:");
          printTransform(MSG::VERBOSE, Amg::CLHEPTransformToEigen(newtrans));
        }
        /** End of verbose level debug section **/

        // Set the new transform; Will replace existing one with updated transform
        bool status = setAlignableTransformDelta(0,
                                                 trans_iter->identify(),
                                                 Amg::CLHEPTransformToEigen(newtrans),
                                                 InDetDD::local,
                                                 nullptr);

        if (!status) {
          ATH_MSG_DEBUG("Cannot set AlignableTransform for identifier."
                        << getIdHelper()->show_to_string(trans_iter->identify())
                        << " at level 0 for IBLDist bowing deformation");
        }

        alignmentChange = (alignmentChange || status);
      }
    }
  }

  return alignmentChange;
  }

  bool PixelDetectorManager::processSpecialAlignment(const std::string& key,
                                                     const CondAttrListCollection* obj,
                                                     GeoVAlignmentStore* alignStore) const {
    bool alignmentChange = false;

    ATH_MSG_INFO("Processing IBLDist alignment container with key " << key);
    if(numerology().numLayers() != 4) {
      // this doesn't appear to be Run 2, i.e. the IBL isn't there. Bailing
      return alignmentChange;
    }

    int nstaves = 0;
    if (numerology().numPhiModulesForLayer(0) < 14)
      nstaves = 14;
    else
      nstaves = numerology().numPhiModulesForLayer(0);

    std::vector<float> ibldist;
    std::vector<float> iblbaseline;
    ibldist.resize(nstaves);
    iblbaseline.resize(nstaves);

    // loop over objects in collection
    for (CondAttrListCollection::const_iterator citr = obj->begin();
        citr != obj->end(); ++citr) {

      const coral::AttributeList &atrlist = citr->second;
      ibldist[atrlist["stave"].data<int>()] = atrlist["mag"].data<float>();
      iblbaseline[atrlist["stave"].data<int>()] = atrlist["base"].data<float>();

      ATH_MSG_VERBOSE("IBLDist DB -- channel: " << citr->first
                      << ", stave: " << atrlist["stave"].data<int>()
                      << ", mag: " << atrlist["mag"].data<float>()
                      << ", base: " << atrlist["base"].data<float>());
    }

    /**
     * Paul Gessinger (Jun 2019): For MT, we won't retrieve the underlying actual
     * alignable transform container, since that's hard to come by.
     * We loop over all detector elements, check if they match the
     * detector elements we have IBL dist info for, retrieve the actual transform
     * from the provided GeoAlignmentStore, and re-set it to the corrected
     * value including IBL bowing.
     *
     * The calculation of the bowing is taken from the non-MT implementation above.
     */


    for(const auto* detElem : *getDetectorElementCollection()) {
      if(!detElem->isInnermostPixelLayer()) {
        // skip non-IBL elements. This only works if the innermost pixel layer is in fact
        // the IBL. That should be the case for Run2 until replacement of the ID.
        continue;
      }
      std::string repr = getIdHelper()->show_to_string(detElem->identify());
      ATH_MSG_DEBUG("IBLDist alignment for identifier " << repr);


      IdentifierHash idHash = getIdHelper()->wafer_hash(detElem->identify());
      if (!idHash.is_valid()) {
        ATH_MSG_WARNING("Invalid HashID for identifier " << repr);
        ATH_MSG_WARNING("No IBLDist corrections can be applied for "
                        "invalid HashID's - exiting ");
        return false;
      }

      // extract the stave number
      int stave = getIdHelper()->phi_module(detElem->identify());

      Amg::Vector3D center = detElem->defTransform() * Amg::Vector3D{0, 0, 0};
      double z = center[2];
      const double y0y0 = 366.5 * 366.5;

      double bowx = ibldist[stave] * (z * z - y0y0) / y0y0;
      double basex = iblbaseline[stave];
      // This is in the module frame, as bowing corrections are directly L3

      ATH_MSG_DEBUG("Total IBL-module Tx shift (baseline+bowing): " << basex + bowx);

      if ((basex + bowx) == 0) {
        continue; // make sure we ignore NULL corrections
      }

      Amg::Transform3D shift = Amg::Translation3D(basex + bowx, 0, 0) *
                               Amg::RotationMatrix3D::Identity();

      // now we need to get the original alignment delta to apply this additional
      // shift to
      ExtendedAlignableTransform* eat = m_alignableTransforms[idHash].get();
      const GeoTrf::Transform3D* currentDelta = alignStore->getDelta(eat->alignableTransform());
      if (currentDelta == nullptr) {
        ATH_MSG_ERROR("Have IBL Dist for element which does not have an alignment delta."
                      << " This indicates inconsistent alignment data");
        return false;
      }

      HepGeom::Transform3D recoToHitTransform = detElem->recoToHitTransform();
      Amg::Transform3D r2h = Amg::CLHEPTransformToEigen(recoToHitTransform);

      ATH_MSG_VERBOSE("Previous delta for " << repr << ":\n" << currentDelta->matrix());
      ATH_MSG_VERBOSE("Bowing-only delta for " << repr << ":\n" << shift.matrix());
      Amg::Transform3D newDelta = shift * r2h.inverse() * (*currentDelta) * r2h;

      // We can probably just write it back to the geo alignment store.
      // The IBL bowing is always at the module level, and that's the delta that
      // we retrieved from the geo alignment store.

      bool status = setAlignableTransformDelta(0,
                                               detElem->identify(),
                                               newDelta,
                                               InDetDD::local,
                                               alignStore);


      ATH_MSG_VERBOSE("New delta for " << repr << ":\n"
                      << alignStore->getDelta(eat->alignableTransform())->matrix());

      alignmentChange |= status;
    }

    return alignmentChange;
  }

  // New global alignment folders
  bool PixelDetectorManager::processGlobalAlignment(const std::string & key, int level, FrameType frame, const CondAttrListCollection* /*obj*/, GeoVAlignmentStore* alignStore) const
  {

    bool alignmentChange = false;

    ATH_MSG_INFO("Processing new global alignment containers with key " << key
                 << " in the " << frame << " frame at level " << level);

    Identifier ident=Identifier();
    const CondAttrListCollection* atrlistcol=nullptr;
    if (StatusCode::SUCCESS==m_detStore->retrieve(atrlistcol,key)) {
      // loop over objects in collection
      for (CondAttrListCollection::const_iterator citr=atrlistcol->begin(); citr!=atrlistcol->end();++citr) {
        const coral::AttributeList& atrlist=citr->second;
        // We are in the Pixel manager, therefore ignore all that is not Pixel Identifier
        if (atrlist["det"].data<int>()!=1) continue;

        ident = getIdHelper()->wafer_id(atrlist["bec"].data<int>(),
                atrlist["layer"].data<int>(),
                atrlist["ring"].data<int>(),
                atrlist["sector"].data<int>());

        // Make sure we have valid HashID (This ONLY works here as the 0-modules exist)
        // Precaution which does not work for e.g. SCT
        if (!(getIdHelper()->wafer_hash(ident)).is_valid()){
          ATH_MSG_WARNING("Invalid HashID for identifier " << getIdHelper()->show_to_string(ident));
          ATH_MSG_WARNING("No global alignment corrections can be applied for invalid HashID's - exiting");
          return false;
        }

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
                        << " at level " << level << " for new global DB");
        }

        alignmentChange = (alignmentChange || status);
      }
    } else {
      ATH_MSG_WARNING("Cannot find new global align Container for key "
                      << key << " - no new global alignment");
      return alignmentChange;
    }
    return alignmentChange;
  }


  // Helpful function for debugging of transforms
  void PixelDetectorManager::printTransform(MSG::Level level,
                                            const Amg::Transform3D & tr) const
  {
    ATH_MSG_LVL_NOCHK(level, " - translation: " << tr.translation().x() << "  " <<tr.translation().y() << "  " <<tr.translation().z());
    ATH_MSG_LVL_NOCHK(level, " - rotation:");
    ATH_MSG_LVL_NOCHK(level, "    " << tr(0,0) << "  " << tr(0,1) << "  " << tr(0,2));
    ATH_MSG_LVL_NOCHK(level, "    " << tr(1,0) << "  " << tr(1,1) << "  " << tr(1,2));
    ATH_MSG_LVL_NOCHK(level, "    " << tr(2,0) << "  " << tr(2,1) << "  " << tr(2,2));
    return;
  }


} // namespace InDetDD
