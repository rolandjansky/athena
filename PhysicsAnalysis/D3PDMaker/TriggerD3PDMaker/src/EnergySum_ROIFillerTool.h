// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySum_ROIFillerTool.h 572492 2013-11-26 15:56:48Z ssnyder $
#ifndef TRIGGERD3PDMAKER_ENERGYSUM_ROIFILLERTOOL_H
#define TRIGGERD3PDMAKER_ENERGYSUM_ROIFILLERTOOL_H

// EDM include(s):
#include "AnalysisTriggerEvent/EnergySum_ROI.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Filler tool for EnergySum_ROI objects
    *
    *         This filler tool can be used to save the properties of
    *         EnergySum_ROI objects into an output D3PD.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 572492 $
    * $Date: 2013-11-26 16:56:48 +0100 (Tue, 26 Nov 2013) $
    */
   class EnergySum_ROIFillerTool : public BlockFillerTool< EnergySum_ROI > {

   public:
      /// Regular AlgTool constructor
      EnergySum_ROIFillerTool( const std::string& type, const std::string& name,
                               const IInterface* parent );

      /// Function booking the variables in the output
      virtual StatusCode book();
      /// Function filling the variables in the output
      virtual StatusCode fill( const EnergySum_ROI& roi );

   private:
      bool m_writeThrInfo; ///< Write info about the passed thresholds
      bool m_writeRoIWord; ///< Write out the raw RoI word
      bool m_writeDecodedInfo; ///< Write out the decoded RoI information

      std::vector< std::string >* m_thresholds; ///< Names of the passed thresholds

      uint32_t* m_roiWord0; ///< First RoI word produced by the L1Calo hardware
      uint32_t* m_roiWord1; ///< Second RoI word produced by the L1Calo hardware
      uint32_t* m_roiWord2; ///< Third RoI word produced by the L1Calo hardware

      float* m_eXmiss; ///< Missing energy in the X direction
      float* m_eYmiss; ///< Missing energy in the Y direction
      float* m_energyT; ///< Total transverse energy deposited
      bool*  m_overflowX; ///< Overflow flag of the E<sub>x</sub> calculation
      bool*  m_overflowY; ///< Overflow flag of the E<sub>y</sub> calculation
      bool*  m_overflowT; ///< Overflow flag of the E<sub>T</sub> calculation

   }; // class EnergySum_ROIFillerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_ENERGYSUM_ROIFILLERTOOL_H
