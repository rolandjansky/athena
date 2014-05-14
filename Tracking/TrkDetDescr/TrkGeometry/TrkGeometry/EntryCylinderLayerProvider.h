/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EntryCylinderLayerProvider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_ENTRYCYLINDERLAYERPROVIDER_H
#define TRKGEOMETRY_ENTRYCYLINDERLAYERPROVIDER_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkGeometry/EntryLayerProvider.h"
#include "TrkGeometry/CylinderLayer.h"
// ATH_MSG macros
#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"


namespace Trk {
    
   /**
     @class EntryCylinderLayerProvider

     provides 2 cylindrical entry/exit layers     

     @author Andreas.Salzburger@cern.ch
    */

    class EntryCylinderLayerProvider : public EntryLayerProvider {

      public: 
        /** Default constructor */
        EntryCylinderLayerProvider(CylinderLayer* layerAtInnerR,
                                   CylinderLayer* layerAtOuterR,
                                   double iTolerance = 0.02);       

        /**Virtual destructor*/
        ~EntryCylinderLayerProvider();

        /** returns the assoicated layer  */
        const CylinderLayer* associatedLayer(const Amg::Vector3D&) const;
    
        /** returns the most probable entryLayer  */
        const CylinderLayer* entryLayer(const Amg::Vector3D&, const Amg::Vector3D&) const;

        /** returns the most probable exit Layer  */
        const CylinderLayer* exitLayer(const Amg::Vector3D&, const Amg::Vector3D&) const;
  
        /** the layers for the outside world - in base class */
        const std::vector<const Layer*>& layers() const;

      private:
        /** Copy constructor - forbidden */
        EntryCylinderLayerProvider(const EntryCylinderLayerProvider&)
        : EntryLayerProvider() {}


        double                  m_rMin;                //!< dimensions in r for the cylinder layers
        double                  m_rMax;                //!< dimensions in r for the cylinder layers
        double                  m_zMin;                //!< dimensions in z for the cylinder layers                
        double                  m_zMax;                //!< dimensions in z for the cylinder layers
        
        double                  m_tolerance;           //!< intersection tolerance

        CylinderLayer*          m_layerAtInnerRadius;  //!< the inner entry layer
        CylinderLayer*          m_layerAtOuterRadius;  //!< the outer entry layer

        std::vector<const Trk::Layer*> m_cachedBaseClassLayers;

      public:
        /// Log a message using the Athena controlled logging system
        MsgStream& msg( MSG::Level lvl ) const { return m_msg.get() << lvl ; }
        /// Check whether the logging system is active at the provided verbosity level
        bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
      private:
        /// Private message stream member
        mutable Athena::MsgStreamMember m_msg;

    };

    inline const std::vector<const Trk::Layer*>& EntryCylinderLayerProvider::layers() const
    { return m_cachedBaseClassLayers; }

}

#endif // TRKGEOMETRY_ENTRYLAYERPROVIDER_H


