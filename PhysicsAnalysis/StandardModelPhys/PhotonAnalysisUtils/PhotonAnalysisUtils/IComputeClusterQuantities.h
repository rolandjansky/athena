/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IComputeClusterQuantities.h, (c) ATLAS Detector software 2011
///////////////////////////////////////////////////////////////////

#ifndef ICOMPUTECLUSTERQUANTITIES_H
#define ICOMPUTECLUSTERQUANTITIES_H

/** @class IComputeClusterQuantities
  Interface for the PhotonAnalysisUtils/ComputeClusterQuantities

  @author Jean-Francois Marchand <jean-francois.marchand@cern.ch>

CREATED : Dec 2011
MODIFIED :
*/

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "egammaEvent/egamma.h"


static const InterfaceID IID_IComputeClusterQuantities("IComputeClusterQuantities", 1, 0);
        
class IComputeClusterQuantities : virtual public IAlgTool {
     
 public:
  virtual ~IComputeClusterQuantities(){};
  
  static const InterfaceID& interfaceID(); 
  virtual StatusCode execute(const egamma *eg) = 0;
  
  virtual double return_eS0_real(void)  const = 0; 
  virtual double return_eS1_real(void)  const = 0; 
  virtual double return_eS2_real(void)  const = 0; 
  virtual double return_eS3_real(void)  const = 0; 
  virtual double return_EcellS0(void)   const = 0; 
  virtual double return_etacellS0(void) const = 0; 

};

inline const InterfaceID& IComputeClusterQuantities::interfaceID()
{
  return IID_IComputeClusterQuantities;
}

#endif
