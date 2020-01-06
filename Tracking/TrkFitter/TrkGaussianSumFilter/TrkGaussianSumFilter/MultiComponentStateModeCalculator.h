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

#include "GaudiKernel/MsgStream.h"
#include "EventPrimitives/EventPrimitives.h"
#include <array>

namespace Trk{

class MultiComponentState;

namespace MultiComponentStateModeCalculator{

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

 //!< IMultiComponentStateModeCalculator interface method to calculate mode
  Amg::VectorX calculateMode( const MultiComponentState&, MsgStream &log ) ;

  //!< Private method to extract the weight, mean and sigma values from the multi-component state
  void fillMixture( const MultiComponentState&, std::array<std::vector< Mixture >,5>&  ) ;

  //!< Private method to find the mode using the Newton-Raphson method based on a starting guess
  double findMode( double, int, std::array<std::vector< Mixture >,5>& mixture, MsgStream &log ) ;

  //!< Private method to determine the pdf of the cashed mixture at a given value
  double pdf( double, int, std::array<std::vector< Mixture >,5>& mixture ) ;

  //!< Private method to determine the first order derivative of the pdf at a given value
  double d1pdf( double, int, std::array<std::vector< Mixture >,5>& mixture ) ;

  //!< Private method to determine the second order derivative of the pdf at a given value
  double d2pdf( double, int, std::array<std::vector< Mixture >,5>& mixture) ;

  //!< Private method to determine the value of the a gaussian distribution at a given value
  double gaus( double x, double mean, double sigma ) ;

  double findModeGlobal(double, int,std::array<std::vector< Mixture >,5>& mixture) ;

  double width( int i, std::array<std::vector< Mixture >,5>& mixture)  ;

  double findRoot(double &result, double xlo, double xhi, double value, double i,std::array<std::vector< Mixture >,5>& mixture, MsgStream &log) ;

} // end MultiComponentStateModeCalculator namespace

} // end Trk namespace

#endif
