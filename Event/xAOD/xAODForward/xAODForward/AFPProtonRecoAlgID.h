/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPProtonRecoAlgID.h
 * @author Petr Balek <petr.balek@cern.ch>
 * @date   2021-12-17
 * 
 * @brief  Definitions of identification numbers of AFP proton reconstruction algorithms
 * 
 */

#ifndef XAODFORWARD_AFPPROTONRECOALGID_H
#define XAODFORWARD_AFPPROTONRECOALGID_H

namespace xAOD
{
  
/**
 * @brief Helper class to store ID numbers of AFP proton reconstruction algorithms.
 *
 * This class contains public static const variables which can be used
 * instead of raw numbers for refering to algorithms used for
 * AFP proton reconstruction. Enum is not used to allow adding new numbers
 * without the need of modifying the header file.
 *
 * An example of usage
 * @code{.cpp}
 * xAOD::AFPProton* proton;
 * if (proton->algID() == xAOD::AFPProtonRecoAlgID::analytical) {
 * // do something if analytical algorithm is used
 * }
 * @endcode
 * 
 * Algorithms indexing follows the scheme
 *
 * | ID  | Algorithm                                                      | Comments |
 * | :-: | -------------------------------------------------------------- | -------- |
 * | 0   | Analytical algorithm (AFP_GlobReco/AFP_ProtonRecoAnalytical)   |          |
 * | 1   | Chi^2 Minuit algorithm (AFP_GlobReco/AFP_ProtonRecoChi2Minuit) |          |
 * 
 */
  class AFPProtonRecoAlgID
  {
  public:
    static constexpr int analytical {0}; ///< analytical algorithm id=0
    static constexpr int chi2minuit {1}; ///< chi2minuit algorithm id=1
  };

}
#endif // XAODFORWARD_AFPPROTONRECOALGID_H

