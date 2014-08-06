// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetET_ROIGetterTool.h 500205 2012-05-11 10:17:25Z krasznaa $
#ifndef TRIGGERD3PDMAKER_JETET_ROIGETTERTOOL_H
#define TRIGGERD3PDMAKER_JETET_ROIGETTERTOOL_H

// STL include(s):
#include <string>

// EDM include(s):
#include "AnalysisTriggerEvent/JetET_ROI.h"

// D3PD include(s):
#include "D3PDMakerUtils/ObjGetterTool.h"

namespace D3PD {

   /**
    *  @short Special tool for getting the JetET_ROI object
    *
    *         The JetET_ROI object is a bit special. It is stored
    *         in a vector in LVL1_ROI, but there is either 0 or 1 of
    *         it. (Depending on a few things.) So it doesn't make sense
    *         to implement a ICollectionGetterTool for this type.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 500205 $
    * $Date: 2012-05-11 12:17:25 +0200 (Fri, 11 May 2012) $
    */
   class JetET_ROIGetterTool : public ObjGetterTool< JetET_ROI > {

   public:
      /// Regular AlgTool constructor
      JetET_ROIGetterTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

      /// Function retrieving the JetET_ROI object
      virtual const JetET_ROI* get( bool allowMissing = false );

   private:
      bool m_firstevent; ///< Internal flag for knowing when the tool is called first
      std::string m_key; ///< SG key of the LVL1_ROI object
      std::string m_type; ///< Dummy property for the class type

   }; // class JetET_ROIGetterTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_JETET_ROIGETTERTOOL_H
