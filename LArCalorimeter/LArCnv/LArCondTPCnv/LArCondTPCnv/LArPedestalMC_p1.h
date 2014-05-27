/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArPedestalMC_p1.h
 *
 * @brief This file defines the class used for persistent container of
 * LArPedestalMC
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArPedestalMC_p1.h,v 1.1 2008-01-23 13:52:08 schaffer Exp $
 */
#ifndef LARCONDTPCNV_LARPEDESTALMC_P1
#define LARCONDTPCNV_LARPEDESTALMC_P1

/**
 * @class LArPedestalMC_p1
 *
 * @brief persistent class container of LArConditionsMC for
 * LArPedestal data.
 *
 *
**/

class LArPedestalMC_p1
{

public:
    LArPedestalMC_p1()  {} 
    std::vector<float>      m_vPedestal;
    std::vector<float>      m_vPedestalRMS;
};

     
#endif // LARCONDTPCNV_LARPEDESTALMC_P1
