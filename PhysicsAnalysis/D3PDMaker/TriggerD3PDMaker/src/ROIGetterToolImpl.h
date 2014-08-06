// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ROIGetterToolImpl.h 500205 2012-05-11 10:17:25Z krasznaa $
#ifndef TRIGGERD3PDMAKER_ROIGETTERTOOLIMPL_H
#define TRIGGERD3PDMAKER_ROIGETTERTOOLIMPL_H

// STL include(s):
#include <string>

// D3PD include(s):
#include "D3PDMakerUtils/CollectionGetterToolImpl.h"

// Forward declaration(s):
class LVL1_ROI;

namespace D3PD {

   /**
    *  @short Base class for the LVL1 RoI getter tools
    *
    *         The LVL1 RoI "analysis objects" are stored in a single
    *         LVL1_ROI object in StoreGate. This class serves as a basis
    *         for the getter tools extracting these RoI objects from
    *         LVL1_ROI.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 500205 $
    * $Date: 2012-05-11 12:17:25 +0200 (Fri, 11 May 2012) $
    */
   class ROIGetterToolImpl : public CollectionGetterToolImpl {

   public:
      /// Regular AlgTool constructor
      ROIGetterToolImpl( const std::string& type, const std::string& name,
                         const IInterface* parent );

      /// Get the LVL1_ROI object from StoreGate
      const LVL1_ROI* get( bool allowMissing = false );

      /// Function from IObjGetterTool
      virtual const std::type_info& typeinfo() const;
      /// Function from IObjGetterTool
      virtual const void* getUntyped( bool allowMissing = false );

   private:
      bool m_firstevent; ///< Internal flag for knowing when the tool is called first
      std::string m_key; ///< SG key of the LVL1_ROI object
      std::string m_type; ///< Dummy property for the class type

   }; // class ROIGetterToolImpl

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_ROIGETTERTOOLIMPL_H
