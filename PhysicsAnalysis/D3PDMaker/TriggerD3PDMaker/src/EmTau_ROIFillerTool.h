// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTau_ROIFillerTool.h 280512 2010-02-19 16:37:46Z krasznaa $
#ifndef TRIGGERD3PDMAKER_EMTAU_ROIFILLERTOOL_H
#define TRIGGERD3PDMAKER_EMTAU_ROIFILLERTOOL_H

// STL include(s):
#include <string>

// EDM include(s):
#include "AnalysisTriggerEvent/EmTau_ROI.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Filler tool for EmTau_ROI objects
    *
    *         This filler tool can be used to save the properties of EmTau_ROI
    *         objects into the output D3PD.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 280512 $
    * $Date: 2010-02-19 17:37:46 +0100 (Fri, 19 Feb 2010) $
    */
   class EmTau_ROIFillerTool : public BlockFillerTool< EmTau_ROI > {

   public:
      /// Regular AlgTool constructor
      EmTau_ROIFillerTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

      /// Function booking the variables in the output
      virtual StatusCode book();
      /// Function filling the variables for the output
      virtual StatusCode fill( const EmTau_ROI& roi );

   private:
      bool m_writeThrInfo;
      bool m_writeRoIWord;
      bool m_writeDecodedInfo;

      std::vector< std::string >* m_thrNames;
      std::vector< float >*       m_thrValues;

      uint32_t* m_roiWord;

      float* m_core;
      float* m_emClus;
      float* m_tauClus;
      float* m_emIsol;
      float* m_hadIsol;
      float* m_hadCore;

      uint32_t* m_thrPattern;

   }; // class EmTau_ROIFillerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_EMTAU_ROIFILLERTOOL_H
