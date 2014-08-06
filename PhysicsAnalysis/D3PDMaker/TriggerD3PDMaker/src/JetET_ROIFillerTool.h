// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetET_ROIFillerTool.h 269963 2009-12-15 16:05:08Z krasznaa $
#ifndef TRIGGERD3PDMAKER_JETET_ROIFILLERTOOL_H
#define TRIGGERD3PDMAKER_JETET_ROIFILLERTOOL_H

// STL include(s):
#include <vector>
#include <string>

// EDM include(s):
#include "AnalysisTriggerEvent/JetET_ROI.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Filler tool for JetET_ROI objects
    *
    *         This filler tool can be used to save the properties of JetET_ROI
    *         objects into the output D3PD.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 269963 $
    * $Date: 2009-12-15 17:05:08 +0100 (Tue, 15 Dec 2009) $
    */
   class JetET_ROIFillerTool : public BlockFillerTool< JetET_ROI > {

   public:
      /// Regular AlgTool constructor
      JetET_ROIFillerTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

      /// Function booking the variables in the output
      virtual StatusCode book();
      /// Function filling the variables for the output
      virtual StatusCode fill( const JetET_ROI& roi );

   private:
      bool m_writeThrInfo;
      bool m_writeRoIWord;
      bool m_writeDecodedInfo;

      std::vector< std::string >* m_thresholds;

      uint32_t* m_roiWord;

      uint32_t* m_thrPattern;

   }; // class JetET_ROIFillerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_JETET_ROIFILLERTOOL_H
