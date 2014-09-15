/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IEMExtrapolCaloConversion.h, (c) ATLAS Detector software 2008
///////////////////////////////////////////////////////////////////

#ifndef IEMEXTRAPOLCALOCONVERSION_H
#define IEMEXTRAPOLCALOCONVERSION_H

/** @class IEMExtrapolCaloConversion
  Interface for the Reconstruction/egamma/egammaCaloTools/ExtrapolCaloConversion

  @author Marc Escalier <escalier@lal.in2p3.fr>

CREATED : oct 2009
MODIFIED :
*/

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "xAODTracking/VertexFwd.h"
#include "xAODEgamma/EgammaFwd.h"

static const InterfaceID IID_IEMExtrapolCaloConversion("IEMExtrapolCaloConversion", 1, 0);
        
class IEMExtrapolCaloConversion : virtual public IAlgTool {
     
 public:
  virtual ~IEMExtrapolCaloConversion(){};
  
  static const InterfaceID& interfaceID(); 
  
  virtual StatusCode execute(const xAOD::Egamma *eg,const xAOD::Vertex* myvxcandidate, double error_etaS1)=0;
  
  virtual double returnEta(void)     const = 0; 
  virtual double returnErrorEta(void)     const = 0; 
};

inline const InterfaceID& IEMExtrapolCaloConversion::interfaceID()
{
  return IID_IEMExtrapolCaloConversion;
}

#endif
