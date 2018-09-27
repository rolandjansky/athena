/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
      MultiComponentStateModeCalculator.h  -  description
      ---------------------------------------------------
begin                : Thursday 6th July 2006
author               : atkinson
email                : Tom.Atkinson@cern.ch
description          : Class to calculate the mode (q/p) of a gaussian mixtureArray
***********************************************************************************/

#ifndef Trk_MultiComponentStateModeCalculator_H
#define Trk_MultiComponentStateModeCalculator_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkGaussianSumFilter/IMultiComponentStateModeCalculator.h"
#include <atomic>
#include <array>
#include <vector>

namespace Trk{

class MultiComponentState;

class MultiComponentStateModeCalculator : public AthAlgTool, virtual public IMultiComponentStateModeCalculator{

 public:

  //!< Constructor with AlgTool parameters
  MultiComponentStateModeCalculator( const std::string&, const std::string&, const IInterface* );

  //!< Virtual destructor
  virtual ~MultiComponentStateModeCalculator();

  //!< AlgTool initialise method
  StatusCode initialize();

  //!< AlgTool finalise method
  StatusCode finalize();

  //!< IMultiComponentStateModeCalculator interface method to calculate mode
  virtual Amg::VectorX calculateMode( const MultiComponentState& ) const override;

 private:

  //!< Private Mixture structure
  struct Mixture{

    // Default constructor
    Mixture():
      weight(0.),
      mean(0.),
      sigma(0.)
    {}

    // Constructor with arguments
    Mixture( double aWeight, double aMean, double aSigma ):
      weight( aWeight ),
      mean( aMean ),
      sigma( aSigma )
    {}
    double weight;
    double mean;
    double sigma;
  };

 
  //!< Private method to extract the weight, mean and sigma values from the multi-component state
  void fillMixture( std::array<std::vector< Mixture >,5>& mixtureArray, 
                    const MultiComponentState& ) const;

  //!< Private method to find the mode using the Newton-Raphson method based on a starting guess
  double findMode( const std::array<std::vector< Mixture >,5>& mixtureArray,
                   double, int ) const;

  //!< Private method to determine the pdf of the cashed mixtureArray at a given value
  double pdf( const std::array<std::vector< Mixture >,5>& mixtureArray,
              double, int ) const;

  //!< Private method to determine the first order derivative of the pdf at a given value
  double d1pdf( const std::array<std::vector< Mixture >,5>& mixtureArray,
                double, int ) const;

  //!< Private method to determine the second order derivative of the pdf at a given value
  double d2pdf( const std::array<std::vector< Mixture >,5>& mixtureArray,
                double, int) const;

  //!< Private method to determine the value of the a gaussian distribution at a given value
  double gaus( double x, double mean, double sigma ) const;

  double findModeGlobal(const std::array<std::vector< Mixture >,5>& mixtureArray,
                        double, int) const;
  
  double width( const std::array<std::vector< Mixture >,5>& mixtureArray,
                int i)  const;
  
  double findRoot(const std::array<std::vector< Mixture >,5>& mixtureArray,
                  double &result, double xlo, double xhi, double value, double i) const;


 private:

  int                              m_outputlevel;                      //!< to cache current output level
  
  
  //ModeCalualtor Stats
  mutable std::atomic<int>                      m_NumberOfCalls;
  mutable std::atomic<int>                      m_ConverganceFilures;
  mutable std::atomic<int>                      m_NoErrorMatrix;
  mutable std::atomic<int>                      m_MixtureSizeZero;
  
  
};

} // end Trk namespace

#endif
