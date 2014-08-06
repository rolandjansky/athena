// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySum_ROIGetterTool.h 500205 2012-05-11 10:17:25Z krasznaa $
#ifndef TRIGGERD3PDMAKER_ENERGYSUM_ROIGETTERTOOL_H
#define TRIGGERD3PDMAKER_ENERGYSUM_ROIGETTERTOOL_H

// EDM include(s):
#include "AnalysisTriggerEvent/EnergySum_ROI.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/ObjGetterTool.h"

namespace D3PD {

   /**
    *  @short Getter tool for EnergySum_ROI objects
    *
    *         This getter tool can be used to retrieve the one EnergySum_ROI
    *         object in the current event. (A check is done on the number of
    *         objects found...)
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 500205 $
    * $Date: 2012-05-11 12:17:25 +0200 (Fri, 11 May 2012) $
    */
   class EnergySum_ROIGetterTool : public ObjGetterTool< EnergySum_ROI > {

   public:
      /// Regular AlgTool constructor
      EnergySum_ROIGetterTool( const std::string& type, const std::string& name,
                               const IInterface* parent );

      /// Function returning the EnergySum RoI
      virtual const EnergySum_ROI* get( bool allowMissing = false );

   private:
      bool m_firstevent; ///< Internal flag for knowing when the tool is called first
      std::string m_key; ///< SG key of the LVL1_ROI object
      std::string m_type; ///< Dummy property for the class type

   }; // class EnergySum_ROIGetterTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_ENERGYSUM_ROIGETTERTOOL_H
