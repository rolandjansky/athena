/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelDectorManager.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef InDetReadoutGeometry_PixelDetectorManager_h
#define InDetReadoutGeometry_PixelDetectorManager_h 1

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoVPhysVol.h"

#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"

#include "InDetIdentifier/PixelID.h"

#include <map>

class StoreGateSvc;
class Identifier; 
class IdentifierHash;
class GeoAlignableTransform;
class GeoVFullPhysVol;
class GeoVPhysVol;
class GeoVAlignmentStore;
class CondAttrListCollection;

namespace InDetDD {

    class PixelModuleDesign;
    class SiDetectorElement;
    class ExtendedAlignableTransform;
    
    /** @class PixelDetectorManager
    
        Dedicated detector manager extending the functionality of the SiDetectorManager
        with dedicated pixel information, access.
        
        @author: Grant Gorfine
        - modified and maintained by Nick Styles & Andreas Salzburger
        */
    
    class PixelDetectorManager : public SiDetectorManager  {
    
     public:
      
      /** Constructor */
      PixelDetectorManager(StoreGateSvc* detStore);
    
      /** Destructor */
      ~PixelDetectorManager();
    
      /** Access to raw geometry: */
      virtual unsigned int getNumTreeTops() const override;
      virtual PVConstLink getTreeTop(unsigned int i) const override;
    
      /** Add a Tree top: */
      void addTreeTop(PVLink);
    
      //
      // Access Readout Elements
      //
      /** access to individual elements : via Identifier */
      virtual SiDetectorElement * getDetectorElement(const Identifier &id) const override;

      /** access to individual elements : via IdentifierHash */
      virtual SiDetectorElement * getDetectorElement(const IdentifierHash &idHash) const override;

      /** access to individual elements : via element identification */
      SiDetectorElement * getDetectorElement(int barrel_endcap,
                                             int layer_wheel,
                                             int phi_module, 
    					                     int eta_module) const;

      /** access to whole collection via Iterators */
      virtual const SiDetectorElementCollection * getDetectorElementCollection() const override;
      virtual SiDetectorElementCollection::const_iterator getDetectorElementBegin() const override;
      virtual SiDetectorElementCollection::const_iterator getDetectorElementEnd() const override;
    
      /** Add elememts */
      virtual void addDetectorElement(SiDetectorElement * element) override;
    
      /** Add alignable transforms. No access to these, they will be changed by manager: */
      virtual void addAlignableTransform (int level,
    				                      const Identifier &id, 
    				                      GeoAlignableTransform *xf,
    				                      const GeoVFullPhysVol * child);
    				                      
      /** As above but does a dynamic_cast to GeoVFullPhysVol */
      virtual void addAlignableTransform (int level,
    				                      const Identifier &id, 
    				                      GeoAlignableTransform *xf,
    				                      const GeoVPhysVol * child);
    
      // DEPRECATED
      virtual void addAlignableTransform (int,
                                          const Identifier &, 
                                          GeoAlignableTransform *) override {}; // For backward compatibility
    
    
      /** Initialize the neighbours. This can only be done when all elements are built. */
      virtual void initNeighbours() override;
        
      /** Methods to query which manager we have */
      virtual bool isPixel() const override {return true;}
    
      /** Check identifier is for this detector */
      virtual bool identifierBelongs(const Identifier & id) const override;
    
      
      /** Layers built. Deprecated. Recommend to access via numerology (see SiDetectorManager) 
          Warning: decodes in "human-readable", e.g. using 3 out of 3 pixel layers yiels 111, the laset two out of 3 in 110, etc.
         */
      unsigned int getBarrelLayers() const ;

      /** Layers built. Deprecated. Recommend to access via numerology (see SiDetectorManager) 
          Warning: decodes in "human-readable", e.g. using 3 out of 3 pixel layers yiels 111, the laset two out of 3 in 110, etc.
      */
      unsigned int getEndcapLayers() const;
    
      /** Access to module design, Casts to PixelModuleDesign */
      virtual const PixelModuleDesign * getPixelDesign(int i) const;

      /** Process new IBLDist DB folder **/
      bool processSpecialAlignment(const std::string &, InDetDD::AlignFolderType) const override;

      bool processSpecialAlignment(const std::string& key,
                                   const CondAttrListCollection* obj=nullptr,
                                   GeoVAlignmentStore* alignStore=nullptr) const override;

      /** Process new global DB folders for L1 and L2 **/
      virtual
      bool processGlobalAlignment(const std::string &, int level, FrameType frame,
                                  const CondAttrListCollection* obj,
                                  GeoVAlignmentStore* alignStore) const override;
    
     private:  
    
      /** implements the main alignment update for delta transforms in different frames,
          it translates into the LocalDelta or GlobalDelta function of SiDetectorManager
      */
      virtual bool setAlignableTransformDelta(int level, 
                                              const Identifier & id, 
                                              const Amg::Transform3D & delta,
                                              FrameType frame,
                                              GeoVAlignmentStore* alignStore) const override;
      
    
      /** prevent copy and assignment */
      const PixelDetectorManager & operator=(const PixelDetectorManager &right);
      PixelDetectorManager(const PixelDetectorManager &right);
    
    
      virtual const PixelID  * getIdHelper() const override;
    
      // data members
      std::vector<PVLink>                                           m_volume;
      SiDetectorElementCollection                                   m_elementCollection;
      typedef std::map<Identifier, std::unique_ptr<ExtendedAlignableTransform>> AlignableTransformMap;
      std::vector< AlignableTransformMap >                          m_higherAlignableTransforms;
      std::vector< std::unique_ptr<ExtendedAlignableTransform> >    m_alignableTransforms;
      const PixelID*                                                m_idHelper;

      // Helpful function for debugging of transforms
      void printTransform(const Amg::Transform3D & tr) const;


      /** This variable switches the how the local alignment corrections are applied
          If true they will be calcualted on top  of all of other corrections but in the default reference frame
          If false they will be calcualted  on top  of all of other corrections but in the globally aligned reference frame    
      */
      bool                                                          m_isLogical;    

    };

} // namespace InDetDD

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h" 
CLASS_DEF(InDetDD::PixelDetectorManager, 18429566, 1)

#endif

#endif





