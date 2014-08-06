// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ROIGetterTool.h 275246 2010-01-26 16:53:19Z ssnyder $
#ifndef TRIGGERD3PDMAKER_ROIGETTERTOOL_H
#define TRIGGERD3PDMAKER_ROIGETTERTOOL_H

// EDM include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"

// Local include(s):
#include "ROIGetterToolImpl.h"

namespace D3PD {

   /**
    *  @short Common base class for the "particle-like RoI" getters
    *
    *         The concrete LVL1 RoI getter tools inherit from this templated
    *         base class, and don't have to implement any functions themselves.
    *         After all they all have to do the same thing, just with different
    *         classes and functions.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 275246 $
    * $Date: 2010-01-26 17:53:19 +0100 (Tue, 26 Jan 2010) $
    */
   template< class ROIType >
   class ROIGetterTool : public ROIGetterToolImpl {

   public:
      /// Constructor specifying a getter function
      ROIGetterTool( const std::string& type, const std::string& name,
                     const IInterface* parent,
                     const std::vector< ROIType >& ( LVL1_ROI::*getterFunction )() const );

      /// Function from ICollectionGetterTool
      virtual const std::type_info& elementTypeinfo() const;
      /// Function from ICollectionGetterTool
      virtual StatusCode reset( bool allowMissing = false );
      /// Function from ICollectionGetterTool
      virtual const void* nextUntyped();
      /// Function from ICollectionGetterTool
      virtual size_t sizeHint( bool allowMissing = false );

   private:
      /// The getter function used to retrieve RoIs from the LVL1_ROI object
      const std::vector< ROIType >& ( LVL1_ROI::*m_getterFunction )() const;

      bool m_itr_valid; ///< Flag showing if the iterators are valid
      /// Iterator pointing to the current element
      typename std::vector< ROIType >::const_iterator m_itr;
      /// Iterator pointing at the end of the coll.
      typename std::vector< ROIType >::const_iterator m_end;

   }; // class ROIGetterTool

} // namespace D3PD

// Include the implementation:
#include "ROIGetterTool.icc"

#endif // TRIGGERD3PDMAKER_ROIGETTERTOOL_H
