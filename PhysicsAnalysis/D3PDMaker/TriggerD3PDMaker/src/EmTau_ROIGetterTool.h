// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTau_ROIGetterTool.h 272868 2010-01-12 17:10:09Z krasznaa $
#ifndef TRIGGERD3PDMAKER_EMTAU_ROIGETTERTOOL_H
#define TRIGGERD3PDMAKER_EMTAU_ROIGETTERTOOL_H

// EDM include(s):
#include "AnalysisTriggerEvent/EmTau_ROI.h"

// Local include(s):
#include "ROIGetterTool.h"

namespace D3PD {

   /**
    *  @short Getter tool for EmTau_ROI objects
    *
    *         This getter tool can be used to loop over all the EmTau_ROI
    *         objects in the current event.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 272868 $
    * $Date: 2010-01-12 18:10:09 +0100 (Tue, 12 Jan 2010) $
    */
   class EmTau_ROIGetterTool : public ROIGetterTool< EmTau_ROI > {

   public:
      /// Regular AlgTool constructor
      EmTau_ROIGetterTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

   }; // class EmTau_ROIGetterTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_EMTAU_ROIGETTERTOOL_H
