// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERD3PDMAKER_JET_ROIFILLERTOOL_H
#define TRIGGERD3PDMAKER_JET_ROIFILLERTOOL_H

// STL include(s):
#include <vector>
#include <string>

// EDM include(s):
#include "AnalysisTriggerEvent/Jet_ROI.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Filler tool for Jet_ROI objects
    *
    *         This filler tool can be used to save the properties of Jet_ROI
    *         objects into the output D3PD.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 252763 $
    * $Date: 2009-12-04 18:54:05 +0100 (Fri, 04 Dec 2009) $
    */
   class Jet_ROIFillerTool : public BlockFillerTool< Jet_ROI > {

   public:
      /// Regular AlgTool constructor
      Jet_ROIFillerTool( const std::string& type, const std::string& name,
                         const IInterface* parent );

      /// Function booking the variables in the output
      virtual StatusCode book();
      /// Function filling the variables for the output
      virtual StatusCode fill( const Jet_ROI& roi );

   private:
      bool m_writeThrInfo;
      bool m_writeRoIWord;
      bool m_writeDecodedInfo;

      std::vector< std::string >* m_thrNames;
      std::vector< float >* m_thrValues;
      uint32_t* m_thrPattern;

      uint32_t* m_roiWord;

      float* m_et4x4;
      float* m_et6x6;
      float* m_et8x8;

   }; // class Jet_ROIFillerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_JET_ROIFILLERTOOL_H
