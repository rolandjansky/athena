/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IApproachDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_IAPPROACHDESCRIPTOR_H
#define TRKGEOMETRY_IAPPROACHDESCRIPTOR_H

// Trk
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkSurfaces/Surface.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include <memory>
namespace Trk {
    
    /**
      @class ApproachSurfaces
      just implement the delete on the objects    
    */
    
    class ApproachSurfaces : public std::vector<const Surface*> {
        public :
        // Default constructor
          ApproachSurfaces() :
            std::vector<const Surface*>()
          {}
                            
        // Desctructur with cleanup
        ~ApproachSurfaces() 
         { 
             for (auto& sf : (*this)){
               //delete when not owned by
               //Tracking Geometry
               if (sf && sf->isFree()) {
                 delete sf;
               }
             }
         }
    };
    
     /**
     @class IApproachDescriptor
     
     CVirtual class to decide and return which approaching surface to be taken,
     it either has a 
     
     @author Andreas.Salzburger@cern.ch, made virtual by Remi.Lafaye@cern.ch
    */

    class IApproachDescriptor {
      public: 
        // Default constructor
        IApproachDescriptor(std::unique_ptr<ApproachSurfaces> aSurfaces,
                            bool rebuild = true)
          : m_approachSurfaces(std::move(aSurfaces))
          , m_approachSurfaceArraySurface(nullptr)
          , m_approachSurfaceArray(nullptr)
          , m_rebuild(rebuild)
        {}
        
        // Default constructor
        IApproachDescriptor(
          std::unique_ptr<BinnedArray<ApproachSurfaces>> aSurfaceArray,
          Surface* aSurfaceArraySurface = nullptr)
          : m_approachSurfaces(nullptr)
          , m_approachSurfaceArraySurface(aSurfaceArraySurface)
          , m_approachSurfaceArray(std::move(aSurfaceArray))
          , m_rebuild(false)
        {}
        
        virtual ~IApproachDescriptor() {
          //Delet if not free 
          if (m_approachSurfaceArraySurface &&
              m_approachSurfaceArraySurface->isFree()) {
            delete m_approachSurfaceArraySurface;
          }
        }

        // register Layer
        void registerLayer(const Layer& lay);
        
        // Telling you if it needs to be rebuilt
        bool rebuild() const;        
    
        /** get the compatible surfaces 
            - return : a boolean indicating if an actual intersection had been tried
            - fill vector of intersections
            - primary bin surface : sf
            - position & direction : pos, dir
        */
        virtual const ApproachSurfaces* approachSurfaces(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const = 0;

      private :
        // register the Layer
        void registerLayerToSurfaces(const Layer& lay, const ApproachSurfaces& aSurfaces);
      
        // Copy constructor - private
        IApproachDescriptor(const IApproachDescriptor&):
            m_approachSurfaces(nullptr),
            m_approachSurfaceArraySurface(nullptr),
            m_approachSurfaceArray(nullptr)
        {}
        
      protected:
        std::unique_ptr<ApproachSurfaces> m_approachSurfaces;
        Surface* m_approachSurfaceArraySurface;
        std::unique_ptr<BinnedArray<ApproachSurfaces>> m_approachSurfaceArray;
        bool m_rebuild;
    };

    inline bool IApproachDescriptor::rebuild() const { return m_rebuild; }    
    
    inline void IApproachDescriptor::registerLayer(const Layer& lay) {
        if (m_approachSurfaces)
            registerLayerToSurfaces(lay, *m_approachSurfaces);
        if (m_approachSurfaceArraySurface){
            m_approachSurfaceArraySurface->associateLayer(lay);
            m_approachSurfaceArraySurface->setOwner(Trk::TGOwn);
        }
        if (m_approachSurfaceArray){
            const std::vector<const ApproachSurfaces*>& aSurfaceObjects = m_approachSurfaceArray->arrayObjects();
            for (auto& aSurfaces : aSurfaceObjects)
                registerLayerToSurfaces(lay, *aSurfaces);
        }
    }
        
    inline void IApproachDescriptor::registerLayerToSurfaces(const Layer& lay, const ApproachSurfaces& aSurfaces) {
        for (auto& aSurface : aSurfaces){
            aSurface->associateLayer(lay);
            aSurface->setOwner(Trk::TGOwn);
        }
    }
}

#endif // TRKGEOMETRY_IAPPROACHDESCRIPTOR_H

