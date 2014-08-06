// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Muon_ROIGetterTool.h 272868 2010-01-12 17:10:09Z krasznaa $
#ifndef TRIGGERD3PDMAKER_MUON_ROIGETTERTOOL_H
#define TRIGGERD3PDMAKER_MUON_ROIGETTERTOOL_H

// EDM include(s):
#include "AnalysisTriggerEvent/Muon_ROI.h"

// Local include(s):
#include "ROIGetterTool.h"

namespace D3PD {

   /**
    *  @short Getter tool for Muon_ROI objects
    *
    *         This getter tool can be used to loop over all the Muon_ROI
    *         objects in the current event.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 272868 $
    * $Date: 2010-01-12 18:10:09 +0100 (Tue, 12 Jan 2010) $
    */
   class Muon_ROIGetterTool : public ROIGetterTool< Muon_ROI > {

   public:
      /// Regular AlgTool constructor
      Muon_ROIGetterTool( const std::string& type, const std::string& name,
                          const IInterface* parent );

   }; // class Muon_ROIGetterTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_MUON_ROIGETTERTOOL_H
