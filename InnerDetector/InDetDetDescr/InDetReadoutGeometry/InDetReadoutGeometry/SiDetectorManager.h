/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiDetectorManager.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SIDETECTORMANAGER_H
#define INDETREADOUTGEOMETRY_SIDETECTORMANAGER_H

#include "InDetReadoutGeometry/InDetDetectorManager.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/SiNumerology.h"

// Amg stuff
#include "GeoPrimitives/GeoPrimitives.h"

#include "CLHEP/Geometry/Transform3D.h"

#include <map>
#include <memory>
#include <string>
  
class StoreGateSvc;
class Identifier; 
class IdentifierHash;
class AtlasDetectorID;
class GeoAlignableTransform;
class GeoVAlignmentStore;

namespace InDetDD {

class SiCommonItems;
class SiDetectorElement;
class SiDetectorDesign;
class ExtendedAlignableTransform;
class SiNumerology;

  /** @class SiDetectorManager
  
        Base class for Pixel and SCT Detector managers.
        
        The Detector manager has methods to retrieve the Identifier
        helper and methods to retrieve the detector elements.  It also
        manages the alignment with methods to register the call backs
        and infrastructure to associate the alignment transforms with
        the appropriate alignable transform in GeoModel.
        There is also access to the layout (Initial, Final, TestBeam) and
        and version information.
  
       @author: Grant Gorfine
       - modified and maintained by Nick Styles & Andreas Salzburger 
       */

    class SiDetectorManager : public InDetDetectorManager  {
    
    
    public:
    
      // Constructor
      SiDetectorManager(StoreGateSvc * detStore, const std::string & name);
     
      // Destructor
      virtual ~SiDetectorManager();
    
    
      //
      // Access Readout Elements
      //
    
      /** access to individual elements using Identifier or IdentiferHash */
      virtual SiDetectorElement * getDetectorElement(const Identifier &id) const = 0;
      virtual SiDetectorElement * getDetectorElement(const IdentifierHash &idHash) const = 0;
    
      /** access to whole collectiom */
      virtual const SiDetectorElementCollection * getDetectorElementCollection() const = 0;
      virtual SiDetectorElementCollection::const_iterator getDetectorElementBegin() const = 0;
      virtual SiDetectorElementCollection::const_iterator getDetectorElementEnd() const = 0;
    
    
      /** Add elememts */
      virtual void addDetectorElement(SiDetectorElement * element) = 0;
    
      /** Initialize the neighbours. This can only be done when all elements are built */
      virtual void initNeighbours() = 0;
    
      /** Get tag used in dictionary */
      const std::string & tag() const; 
    
      /** Methods to query which manager we have */
      virtual bool isPixel() const = 0;
      bool isSCT() const {return !isPixel();}
     
      /** Add alignable transforms. No access to these, they will be changed by manager: */
      virtual void addAlignableTransform (int level, const Identifier &id, GeoAlignableTransform *xf) = 0;
    
      /** Invalidate cache for all detector elements */
      virtual void invalidateAll() const;
    
      /** Update all caches */
      virtual void updateAll() const;
    
      
      /** Helper method to set delta transform from a global delta - Amg interface*/
      bool setAlignableTransformGlobalDelta(ExtendedAlignableTransform * extXF, 
                                            const Amg::Transform3D & delta,
                                            GeoVAlignmentStore* alignStore=nullptr) const;
    
      /** Helper method to set delta transform from a local delta - Amg interface */
      bool setAlignableTransformLocalDelta(ExtendedAlignableTransform * extXF, 
                                           const Amg::Transform3D & localToGlobalXF,
                                           const Amg::Transform3D & delta,
                                           GeoVAlignmentStore* alignStore=nullptr) const;
    
      /** Access to module design */
      void addDesign(std::unique_ptr<const SiDetectorDesign>&&);
      int numDesigns() const;
      const SiDetectorDesign * getDesign(int i) const;
    
      /** Access Numerology */
      const SiNumerology & numerology() const {return m_numerology;}
      SiNumerology & numerology() {return m_numerology;}

      /** Set SiCommonItems */
      void setCommonItems(std::unique_ptr<const SiCommonItems>&& commonItems);

    private:
      //** Prevent copy and assignment */
      const SiDetectorManager & operator=(const SiDetectorManager &right);
      SiDetectorManager(const SiDetectorManager &right); 
    
      /** This method is called by the InDetDetectorManager */
      virtual bool setAlignableTransformDelta(int level, 
                                              const Identifier & id, 
                                              const Amg::Transform3D & delta,
                                              FrameType frame,
                                              GeoVAlignmentStore* alignStore) const = 0;
    
    
    
      std::string                               m_tag;
      SiNumerology                              m_numerology;
      std::vector<std::unique_ptr<const SiDetectorDesign>> m_designs;
      std::unique_ptr<const SiCommonItems>      m_commonItems;
    
    };


} // namespace InDetDD

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h"

CLASS_DEF(InDetDD::SiDetectorManager, 1441401, 1)
#endif

#endif // INDETREADOUTGEOMETRY_SIDETECTORMANAGER_H
