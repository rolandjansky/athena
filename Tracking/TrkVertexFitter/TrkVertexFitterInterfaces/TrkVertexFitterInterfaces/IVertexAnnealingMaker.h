/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  static const InterfaceID IID_IVertexAnnealingMaker("IVertexAnnealingMaker", 1, 0);

  class IVertexAnnealingMaker : virtual public IAlgTool {

  public:
   /** 
    * Virtual destructor 
    */
    virtual ~IVertexAnnealingMaker(){};
    
   /** 
    * AlgTool interface methods 
    */
    static const InterfaceID& interfaceID() { return IID_IVertexAnnealingMaker; };
    
   /**
    * Starts the annealing from scratch
    */
    virtual void reset()=0;
    
   /**
    * Goes one step further in the annealing
    */
    virtual void anneal()=0;
    
   /**
    * Calculates the weight according to the given chi2
    */ 
    virtual double getWeight(double chisq,const std::vector<double>& allchisq) const =0;
   
   /**
    * Calculates the weight according to the given chi2
    */ 
    virtual double getWeight(double chisq) const =0;
   
   /**
    * Checks whether the equilibrium is reached.
    */ 
    virtual bool isEquilibrium() const =0;
    
   /** 
    * Gets the actual temperature
    */
    virtual double actualTemp() const =0;
    
  };
}

#endif
