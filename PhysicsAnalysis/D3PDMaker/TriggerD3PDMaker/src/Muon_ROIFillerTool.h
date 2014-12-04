// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Muon_ROIFillerTool.h 620244 2014-10-06 19:02:48Z ssnyder $
#ifndef TRIGGERD3PDMAKER_MUON_ROIFILLERTOOL_H
#define TRIGGERD3PDMAKER_MUON_ROIFILLERTOOL_H

// STL include(s):
#include <string>

// EDM include(s):
#include "xAODTrigger/MuonRoI.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Filler tool for Muon_ROI objects
    *
    *         This filler tool can be used to save the properties of Muon_ROI
    *         objects into the output D3PD.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 620244 $
    * $Date: 2014-10-06 21:02:48 +0200 (Mon, 06 Oct 2014) $
    */
   class Muon_ROIFillerTool : public BlockFillerTool< xAOD::MuonRoI > {

   public:
      /// Regular AlgTool constructor
      Muon_ROIFillerTool( const std::string& type, const std::string& name,
                          const IInterface* parent );

      /// Function booking the variables in the output
      virtual StatusCode book();
      /// Function filling the variables for the output
      virtual StatusCode fill( const xAOD::MuonRoI& roi );

   private:
      short* m_thrNumber; ///< The value of the highest threshold this RoI passed
      short* m_roi; ///< The "RoI number" (position inside the sector)
      short* m_sectorAddress; ///< The full sector address
      bool* m_firstCandidate; ///< Returns if the candidate had the highest p<sub>T</sub> in the sector
      bool* m_moreCandInRoI; ///< Returns if there were other muons detected in the same RoI
      bool* m_moreCandInSector; ///< Returns if there were other muons detected in the same sector
      short* m_source; ///< Returns the system that detected the muon candidate
      short* m_hemisphere; ///< Returns the hemisphere that detected the muon candidate
      short* m_charge; ///< Returns the charge sign of the muon candidate
      bool* m_vetoed; ///< Returns the veto flag for the candidate

   }; // class Muon_ROIFillerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_MUON_ROIFILLERTOOL_H
