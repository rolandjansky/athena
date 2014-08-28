/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IExtrapolCaloConversion.h, (c) ATLAS Detector software 2008
///////////////////////////////////////////////////////////////////

#ifndef IEXTRAPOLCALOCONVERSION_H
#define IEXTRAPOLCALOCONVERSION_H

#include "egammaEvent/Photon.h"

/** @class IExtrapolCaloConversion
  Interface for the Reconstruction/egamma/egammaCaloTools/ExtrapolCaloConversion

  @author Marc Escalier <escalier@lal.in2p3.fr>

CREATED : oct 2009
MODIFIED :
*/

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class CaloCluster;

static const InterfaceID IID_IExtrapolCaloConversion("IExtrapolCaloConversion", 1, 0);
        
class IExtrapolCaloConversion : virtual public IAlgTool {
     
 public:
  virtual ~IExtrapolCaloConversion(){};
  
  static const InterfaceID& interfaceID(); 
  
  virtual StatusCode execute(const egamma *eg,const Trk::VxCandidate* myvxcandidate,double error_etaS1)=0;
  
  virtual double returnEta(void)          const = 0; 
  virtual double returnErrorEta(void)     const = 0; 
  virtual double returnZvertex(void)      const = 0; 
  virtual double returnErrorZvertex(void) const = 0; 

};

inline const InterfaceID& IExtrapolCaloConversion::interfaceID()
{
  return IID_IExtrapolCaloConversion;
}

#endif
