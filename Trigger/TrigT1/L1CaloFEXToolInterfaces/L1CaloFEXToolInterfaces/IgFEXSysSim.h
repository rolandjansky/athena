/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IgFEXSysSim_H
#define IgFEXSysSim_H

#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/gTowerContainer.h"
#include "L1CaloFEXSim/gFEXOutputCollection.h"

namespace LVL1 {

/*
Interface definition for gFEXSysSim
*/

  static const InterfaceID IID_IgFEXSysSim("LVL1::IgFEXSysSim", 1, 0);

  class IgFEXSysSim : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual StatusCode execute(gFEXOutputCollection* gFEXOutputs) = 0;

    virtual void cleanup() = 0;

    virtual int calcTowerID(int eta, int phi, int nphi, int mod) = 0 ;

    virtual StatusCode fillgRhoEDM(uint32_t tobWord, int scale) = 0;

    virtual StatusCode fillgBlockEDM(uint32_t tobWord, int scale) = 0;

    virtual StatusCode fillgJetEDM(uint32_t tobWord, int scale) = 0;

    virtual StatusCode fillgScalarEJwojEDM(uint32_t tobWord, int scale) = 0;

    virtual StatusCode fillgMETComponentsJwojEDM(uint32_t tobWord, int scale) = 0;

    virtual StatusCode fillgMHTComponentsJwojEDM(uint32_t tobWord, int scale) = 0;

    virtual StatusCode fillgMSTComponentsJwojEDM(uint32_t tobWord, int scale) = 0;



  private:

  };

  inline const InterfaceID& LVL1::IgFEXSysSim::interfaceID()
  {
    return IID_IgFEXSysSim;
  }

} // end of namespace

#endif
