/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IComputePointingQuantities.h, (c) ATLAS Detector software 2011
///////////////////////////////////////////////////////////////////

#ifndef ICOMPUTEPOINTINGQUANTITIES_H
#define ICOMPUTEPOINTINGQUANTITIES_H

#include "egammaEvent/egamma.h"
#include "egammaEvent/Photon.h"

/** @class IComputePointingQuantities
  Interface for the PhotonAnalysisUtils/ComputePointingQuantities

  @author Jean-Francois Marchand <jean-francois.marchand@cern.ch>

CREATED : Dec 2011
MODIFIED :
*/

// Gaudi
#include "GaudiKernel/IAlgTool.h"


static const InterfaceID IID_IComputePointingQuantities("IComputePointingQuantities", 1, 0);
        
class IComputePointingQuantities : virtual public IAlgTool {
     
 public:
  virtual ~IComputePointingQuantities(){};
  
  static const InterfaceID& interfaceID(); 
  
  virtual StatusCode execute(const egamma *eg) = 0;
  
  virtual double return_CaloPointing_eta(void)           const = 0; 
  virtual double return_CaloPointing_sigma_eta(void)     const = 0; 
  virtual double return_CaloPointing_zvertex(void)       const = 0; 
  virtual double return_CaloPointing_sigma_zvertex(void) const = 0; 

  virtual double return_HPV_eta(void)           const = 0; 
  virtual double return_HPV_sigma_eta(void)     const = 0; 
  virtual double return_HPV_zvertex(void)       const = 0; 
  virtual double return_HPV_sigma_zvertex(void) const = 0; 

};

inline const InterfaceID& IComputePointingQuantities::interfaceID()
{
  return IID_IComputePointingQuantities;
}

#endif
