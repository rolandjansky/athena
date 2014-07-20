/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_EMTauInputProvider
#define L1TopoSimulation_EMTauInputProvider


#include "L1TopoSimulation/IInputTOBConverter.h"

namespace LVL1 {

   class EMTauInputProvider : public extends1<AthAlgTool, IInputTOBConverter> {
   public:
      EMTauInputProvider(const std::string& type, const std::string& name, 
                         const IInterface* parent);
      
      virtual ~EMTauInputProvider();

      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const; 

   private:

      /** \brief calculates eta and phi from roiWord*/
      void CalculateCoordinates(int32_t roiWord, double & eta, double & phi) const;

      StringProperty m_emTauLocation;    //!<  EMTAU ROI SG key

   };
}

#endif
