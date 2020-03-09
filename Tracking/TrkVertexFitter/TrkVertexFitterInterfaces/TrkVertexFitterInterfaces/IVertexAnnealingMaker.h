/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexAnnealingMaker.h, (c) ATLAS Detector software 2006
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXFITTERINTERFACES_IVERTEXANNEALINGMAKER_H
#define TRKVERTEXFITTERINTERFACES_IVERTEXANNEALINGMAKER_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>

/**
   * @class Trk::IVertexAnnealingMaker
   *
   * Interface class responsible for an annealing process.
   * You define a chi squared as function of the weight (due to 
   * the compatibility of the track with the vertex), but also as function 
   * of the temperature, which will go down step by step according to 
   * the concrete implementation of the annealing.
   * @author Giacinto.Piacquadio@physik.uni-freiburg.de
   */


    
namespace Trk
{
  class IVertexAnnealingMaker : virtual public IAlgTool {

  public:
    DeclareInterfaceID (IVertexAnnealingMaker, 1, 0);

    typedef unsigned int AnnealingState;

   /** 
    * Virtual destructor 
    */
    virtual ~IVertexAnnealingMaker() = default;
    
   /**
    * Starts the annealing from scratch
    */
    virtual void reset(AnnealingState& state) const =0;
    
   /**
    * Goes one step further in the annealing
    */
    virtual void anneal(AnnealingState& state) const =0;
    
   /**
    * Calculates the weight according to the given chi2
    */ 
    virtual double getWeight(const AnnealingState& state,
                             double chisq,
                             const std::vector<double>& allchisq) const =0;
   
   /**
    * Calculates the weight according to the given chi2
    */ 
    virtual double getWeight(const AnnealingState& state,
                             double chisq) const =0;
   
   /**
    * Checks whether the equilibrium is reached.
    */ 
    virtual bool isEquilibrium(const AnnealingState& state) const =0;
    
   /** 
    * Gets the actual temperature
    */
    virtual double actualTemp(const AnnealingState& state) const =0;
    
  };
}

#endif
