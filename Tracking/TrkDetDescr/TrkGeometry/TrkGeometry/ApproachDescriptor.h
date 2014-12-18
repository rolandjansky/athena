/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ApproachDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_APPROPACHDESCRIPTOR_H
#define TRKGEOMETRY_APPROPACHDESCRIPTOR_H

// Trk
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkSurfaces/Surface.h"
#include "TrkDetDescrUtils/BinnedArray.h"


namespace Trk {
    
    /**
      @class ApproachSurfaces
      just implement the delete on the objects    
    */
    
    class ApproachSurfaces : public std::vector<const Surface*> {
        public :
        //* Default constructor */
          ApproachSurfaces() :
            std::vector<const Surface*>()
          {}
                            
        //* Desctructur with cleanup*/
        ~ApproachSurfaces() 
         { 
             for (auto& sf : (*this))
                 delete sf;
         }
        
    };
    
    /**
     @class ApproachDescriptor
     
     Class to decide and return which approaching surface to be taken,
     it either has a 
     
     @author Andreas.Salzburger@cern.ch
    */

    class ApproachDescriptor {
      public: 
        /**Default constructor*/
        ApproachDescriptor(ApproachSurfaces* aSurfaces, bool rebuild=true) :
            m_approachSurfaces(aSurfaces),
            m_approachSurfaceArraySurface(0),
            m_approachSurfaceArray(0),
            m_rebuild(rebuild)
        {}
        
        /**Default constructor*/
        ApproachDescriptor(BinnedArray<ApproachSurfaces>* aSurfaceArray, Surface* aSurfaceArraySurface = 0):
            m_approachSurfaces(0),
            m_approachSurfaceArraySurface(aSurfaceArraySurface),
            m_approachSurfaceArray(aSurfaceArray),
            m_rebuild(false)
        {}
        
        /** register Layer */
        void registerLayer(const Layer& lay);
                
        /**Virtual destructor*/
        virtual ~ApproachDescriptor(){}
        
        /** Telling you if it needs to be rebuilt */
        bool rebuild() const;        
    
        /** get the compatible surfaces 
            - return : a boolean indicating if an actual intersection had been tried
            - fill vector of intersections
            - primary bin surface : sf
            - position & direction : pos, dir
        */
        const ApproachSurfaces* approachSurfaces(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const;

      private :
        /** register the Layer */
        void registerLayerToSurfaces(const Layer& lay, const ApproachSurfaces& aSurfaces);
      
        /** Copy constructor - private */
        ApproachDescriptor(const ApproachDescriptor&):
            m_approachSurfaces(0),
            m_approachSurfaceArraySurface(0),
            m_approachSurfaceArray(0)
        {}
        
        ApproachSurfaces*                m_approachSurfaces;
        Surface*                         m_approachSurfaceArraySurface;
        BinnedArray<ApproachSurfaces>*   m_approachSurfaceArray;
        bool                             m_rebuild;
    };
    
    inline const ApproachSurfaces* ApproachDescriptor::approachSurfaces(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const {
        // return the single approach surfaces
        if (m_approachSurfaces)
            return (m_approachSurfaces);
        // there's new surface given
        if (m_approachSurfaceArraySurface){
            Trk::Intersection asInter = m_approachSurfaceArraySurface->straightLineIntersection(pos,dir);
            if (asInter.valid)
                return (m_approachSurfaceArray->object(asInter.position));
        }
        // get the bin
        return (m_approachSurfaceArray->object(pos));
        
    }

    inline bool ApproachDescriptor::rebuild() const { return m_rebuild; }    
    
    inline void ApproachDescriptor::registerLayer(const Layer& lay) {
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
        
    inline void ApproachDescriptor::registerLayerToSurfaces(const Layer& lay, const ApproachSurfaces& aSurfaces) {
        for (auto& aSurface : aSurfaces){
            aSurface->associateLayer(lay);
            aSurface->setOwner(Trk::TGOwn);
        }
    }
        

}

#endif

