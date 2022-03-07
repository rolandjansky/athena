/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTOOLS_TRIG_VSI_CONST
#define TRIGTOOLS_TRIG_VSI_CONST

/**
* @file   Constants.h
* @brief  Constants for algorithm.
* @author Takane Sano <takane.sano@cern.ch>
* @date   July 15th, 2021
*/

#include "TMath.h"

namespace TrigVSI {

   //! Namespace for constants.
   namespace AlgConsts {
      constexpr float IDr       = 299.;   //!< @brief Max R of ID volume.
      constexpr float IDrInner  = 20.;    //!< @brief Minimum R of ID volume.
      constexpr float IDz       = 1800.;  //!< @brief Max Z of ID volume.
      constexpr float mapBinWid = 10.;    //!< @brief Default bin width for vertex map.

      constexpr float maxEta = 3.5;  //!< @brief Max eta for ID volume.
      constexpr int   nEta   = 70;   //!< @brief Default bin number of eta for vertex map.
      constexpr int   nPhi   = 60;   //!< @brief Default bin number of phi for vertex map.

      constexpr int   binNumR = static_cast<int>( ( (IDr - IDrInner) - 1)/mapBinWid + 1 ); //!< @brief Default bin number of R for vertex map. Ceil ID_R/WID
      constexpr int   binNumZ = static_cast<int>( (IDz - 1)/mapBinWid + 1 );                 //!< @brief Default bin number of Z for vertex map. Ceil IDz/WID

   }//end of namespace TrigVSI::AlgConsts

} //end of namespace TrigVSI

#endif
