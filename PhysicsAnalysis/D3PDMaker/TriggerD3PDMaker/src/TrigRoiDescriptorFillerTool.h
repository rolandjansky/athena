// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigRoiDescriptorFillerTool.h 288060 2010-03-25 21:19:59Z ssnyder $
#ifndef TRIGGERD3PDMAKER_TRIGROIDESCRIPTORFILLERTOOL_H
#define TRIGGERD3PDMAKER_TRIGROIDESCRIPTORFILLERTOOL_H

// EDM include(s):
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool saving information about HLT RoIs
    *
    *         This tool can be used to save the properties of TrigRoiDescriptor
    *         objects, which are used by the HLT steering to guide the algorithms
    *         where they should be trying to reconstruct objects.
    *
    * @author Attila Krasznahorkay
    *
    * $Revision: 288060 $
    * $Date: 2010-03-25 22:19:59 +0100 (Thu, 25 Mar 2010) $
    */
   class TrigRoiDescriptorFillerTool : public BlockFillerTool< TrigRoiDescriptor > {

   public:
      /// Regular AlgTool constructor
      TrigRoiDescriptorFillerTool( const std::string& type, const std::string& name,
                                   const IInterface* parent );

      /// Function creating the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const TrigRoiDescriptor& roi );

   private:
      bool m_savePosition; ///< Save the position information
      bool m_saveSize; ///< Save the size information
      bool m_saveRoIWord; ///< Save information about the LVL1 RoI input

      float* m_phi; ///< Phi position of the RoI
      float* m_eta; ///< Eta position of the RoI
      float* m_zed0; ///< Z coordinate of the interaction vertex

      float* m_phiHalfWidth; ///< Size of the RoI window in phi
      float* m_etaHalfWidth; ///< Size of the RoI window in eta
      float* m_zedHalfWidth; ///< Uncertainty on the interaction vertex position
      float* m_etaPlus;  ///< Eta at z+zwidth
      float* m_etaMinus; ///< Eta at z-zwidth

      unsigned int* m_roiId; ///< Internal number of the RoI given from the steering
      unsigned int* m_l1Id; ///< LVL1 Id of the event that produced the RoI
      unsigned int* m_roiWord; ///< The full 32-bit RoI word

   }; // class TrigRoiDescriptorFillerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_TRIGROIDESCRIPTORFILLERTOOL_H
