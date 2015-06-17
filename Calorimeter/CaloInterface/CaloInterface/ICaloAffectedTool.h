/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
/// \brief ICaloAffectedTool is abstract interface for tools checking if 4 mom is in calo affected region
///
/// \author G.Unal
/// \date March 2010
///
///////////////////////////////////////////////////////////////////////////////

#ifndef CALOINTERFACE_ICaloAffectedTool_H
#define CALOINTERFACE_ICaloAffectedTool_H

// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include <vector>

namespace xAOD {
  class IParticle;
}

class ICaloAffectedTool: virtual public IAlgTool {
  
public:

  static const InterfaceID& interfaceID() { 
    static const InterfaceID IID_ICaloAffectedTool("ICaloAffectedTool", 1 , 0);
    return IID_ICaloAffectedTool; 
  }

  /*
    Return true if 4 momentum eta-phi is within deta,dphi of an affected region between layer min and layer max (all layers if layermax<layermin)
    and with a problemType (0= deadReadout, 1 = deadHV, 2 = affectedHV,  -1= all problems)
    
    @param p  4 momentum pointer to check
    @param deta = eta tolerance
    @param dphi = phi tolerance
    @param layer_min = min calo layer to check
    @param layer_max = max calo layer to check
    @param problemType = type of problem in affacted region
  */

  virtual bool isAffected(const xAOD::IParticle *p, float deta=0, float dphi=0, int layer_min=0, int layer_max=-1, int problemType=-1) = 0;

  /*
    Return list of layers affected with their problem for a given 4 momentum
    Return true is any problem is fonund

    @param p 4 momentum pointer to check
    @param deta = eta tolerance
    @param dphi = phi tolerance
    @param problemType = filters only a given type of problem (-1 : all problems)
    @param layer_list = reference of vector to store list of layer
    @param problem_list = reference of vector to store list of problems
   */

  virtual bool listAffected(const xAOD::IParticle* p, std::vector<int>& layer_list, std::vector<int>& problem_list, float deta=0, float dphi=0, int problemType=-1) =0;

 
};

#endif 
