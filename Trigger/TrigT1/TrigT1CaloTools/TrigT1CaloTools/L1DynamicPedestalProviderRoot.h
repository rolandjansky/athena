/** -*- C++ -*- */
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigT1CaloTools/L1DynamicPedestalProviderRoot.h
 * @author V. Scharf <vscharf@kip.uni-heidelberg.de>
 * @date June 2014
 * @brief Tool to retrieve dynamic pedestal values from an external root-file.
 */

#ifndef TRIGT1CALOTOOLS_L1DYNAMICPEDESTALPROVIDERROOT_H
#define TRIGT1CALOTOOLS_L1DYNAMICPEDESTALPROVIDERROOT_H

#include "TrigT1CaloToolInterfaces/IL1DynamicPedestalProvider.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

namespace LVL1
{

  /**
   * @class L1DynamicPedestalProviderRoot
   * @brief Tool to retrieve dynamic pedestal values from externally provided ROOT files.
   *
   * This tool retrieves the dynamic pedestal, given the trigger tower location and the bcid from 
   * an external ROOT file.
   *
   * Caveat: Currently the mu-dependency of the dynamic pedestal is not taken into account.
   */  
  class L1DynamicPedestalProviderRoot : virtual public IL1DynamicPedestalProvider, public AthAlgTool
  {
  public:
    /** constructor */
    L1DynamicPedestalProviderRoot(const std::string&,const std::string&,const IInterface*);

    /** default destructor */
    virtual ~L1DynamicPedestalProviderRoot();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();

    /** retrieve the bcidCorrection value */
    virtual int dynamicPedestal(int iEta, int layer, int pedestal, int iBCID, float mu);
    
  private:
    std::string m_firRefFileName;

    static const int s_nElement = 33;
    static const int s_nBcid = 350; // mc12: max 350 // all: 3564
    int m_firRefEm[s_nElement][s_nBcid];
    int m_firRefHad[s_nElement][s_nBcid];
  }; // end of class L1DynamicPedestalProviderRoot

} // end of namespace

#endif //TRIGT1CALOTOOLS_L1DYNAMICPEDESTALPROVIDERROOT_H
