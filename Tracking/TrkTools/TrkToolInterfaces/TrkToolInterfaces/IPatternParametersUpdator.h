/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPatternParametersUpdator.h
//   Main public methods for measurement updator using internal pattern
//   recognition objects for track parameters (Trk::PatternTrackParameters)
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_IPATTERNPARAMETERSUPDATOR_H
#define TRK_IPATTERNPARAMETERSUPDATOR_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IPatternParametersUpdator("Trk::IPatternParametersUpdator", 1, 0);

namespace Trk {

  class PatternTrackParameters;
  class LocalParameters;

  /** @class IPatternParametersUpdator

      @brief Interface for updating Trk::PatternTrackParameters,
             the fast internal representation of track parameters in the
             (inner detector) pattern recognition tools, with a measurement.

             The interface provides methods for adding or removing a measurement
             to/from a track state as well as calculating chi2 contributions.
             The methods are the same tasks as the standard Trk::IUpdator, but
             instead of the EDM Trk::TrackParameters it works with Trk::PatternTrackParameters.
             The interface is aimed at an implementation using the Kalman filter gain formalism.
 
      @author Igor.Gavrilenko -at- cern.ch, Wolfgang.Liebig -at- cern.ch
  */

  class IPatternParametersUpdator : virtual public IAlgTool {
  public:

    static const InterfaceID& interfaceID( ) ;
    // /////////////////////////////////////////////////////////////////
    // Add and remove without Xi2 calculation
    // /////////////////////////////////////////////////////////////////

    /** add a PRD-level local position to a track state given by pattern track pars (no chi2 calculated).
        The last PatternTrackPar argument is the output, returns false if the calculation fails. */
    virtual bool                   addToState 
      (PatternTrackParameters&,const Amg::Vector2D&,   const Amg::MatrixX&,
       PatternTrackParameters&) const = 0;
    /** add a ROT-level measurement to a track state given by pattern track pars (no chi2 calculated).
        The last PatternTrackPar argument is the output, returns false if the calculation fails. */
    virtual bool                   addToState 
      (PatternTrackParameters&,const LocalParameters&, const Amg::MatrixX&,
       PatternTrackParameters&) const = 0;
    /** add an explicitly one-dimensional measurement without chi2 calculation to pattern pars.
        The last PatternTrackPar argument is the output, returns false if the calculation fails. */
    virtual bool                   addToStateOneDimension 
      (PatternTrackParameters&,const Amg::Vector2D&,   const Amg::MatrixX&,
       PatternTrackParameters&) const = 0;

    // /////////////////////////////////////////////////////////////////
    // Remove without Xi2 calculation
    // /////////////////////////////////////////////////////////////////

    /** remove a PRD-level local position from a track state given by pattern track pars
        (no chi2 calculated).
        The last PatternTrackPar argument is the output, returns false if the calculation fails. */
    virtual bool removeFromState 
      (PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
       PatternTrackParameters&) const = 0;
    /** remove a ROT-level measurement from a track state given by pattern track pars (no chi2 calculated).
        The last PatternTrackPar argument is the output, returns false if the calculation fails. */
    virtual bool removeFromState 
      (PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,
       PatternTrackParameters&) const = 0;

    // /////////////////////////////////////////////////////////////////
    // Add  with Xi2 calculation
    // /////////////////////////////////////////////////////////////////

    /** add a PRD-level local position to a track state given by pattern track pars (chi2 calculated).
        The last PatternTrackPar argument is the output, returns false if the calculation fails. */
    virtual bool addToState 
      (PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
       PatternTrackParameters&,double&,int&) const = 0;
    /** add a ROT-level measurement to a track state given by pattern track pars (chi2 calculated).
        The last PatternTrackPar argument is the output, returns false if the calculation fails. */
    virtual bool addToState 
      (PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,
       PatternTrackParameters&,double&,int&) const = 0;
    /** add an explicitly one-dimensional measurement to pattern pars and calculate chi2 contribution.
        The last PatternTrackPar argument is the output, returns false if the calculation fails. */
    virtual bool                   addToStateOneDimension 
      (PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
       PatternTrackParameters&,double&,int&) const = 0;

    // /////////////////////////////////////////////////////////////////
    // Remove with Xi2 calculation
    // /////////////////////////////////////////////////////////////////

    /** remove a PRD-level local position from a track state given by pattern track pars
        (chi2 calculated).
        The last PatternTrackPar argument is the output, returns false if the calculation fails. */
    virtual bool removeFromState 
      (PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
       PatternTrackParameters&,double&,int&) const = 0;
    /** remove a ROT-level measurement from a track state given by pattern track pars
        (chi2 calculated).
        The last PatternTrackPar argument is the output, returns false if the calculation fails. */
    virtual bool removeFromState 
      (PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,
       PatternTrackParameters&,double&,int&) const = 0;

    // /////////////////////////////////////////////////////////////////
    // Combine two state with or without Xi2 calculation
    // /////////////////////////////////////////////////////////////////

    /** combine two track states into a resulting state.
        The third PatternTrackPar argument is the output, returns false if the calculation fails. */
    virtual bool combineStates   
      (PatternTrackParameters&,PatternTrackParameters&,PatternTrackParameters&) const = 0;
    /** combine two track states into a resulting state and calculate chi2 contribution.
        The third PatternTrackPar argument is the output, returns false if the calculation fails. */
    virtual bool combineStates   
      (PatternTrackParameters&,PatternTrackParameters&,PatternTrackParameters&,
       double&) const = 0;

    // /////////////////////////////////////////////////////////////////
    // Xi2 calculation
    // /////////////////////////////////////////////////////////////////
      
    /** calculate fit quality in terms of chi2 assuming a track state which does not include
        information from the current measurement (given as PRD-level local position).
        The last parameters are the output ndof and chi2, returns false if the calculation fails. */
    virtual bool predictedStateFitQuality  
      (const PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,int&,double&) 
      const = 0;
    /** calculate fit quality in terms of chi2 assuming a track state which does not include
        information from the current measurement.
        The last parameters are the output ndof and chi2, returns false if the calculation fails. */
    virtual bool predictedStateFitQuality  
      (const PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,int&,double&) 
      const = 0;
    /** calculate fit quality in terms of chi2 assuming a track state which includes
        information from the current measurement (given as PRD-level local position).
        The last parameters are the output ndof and chi2, returns false if the calculation fails. */
    virtual bool fullStateFitQuality 
      (const PatternTrackParameters&,const Amg::Vector2D&,  const Amg::MatrixX&,int&,double&)
      const = 0;
    /** calculate fit quality in terms of chi2 assuming a track state which includes
        information from the current measurement.
        The last parameters are the output ndof and chi2, returns false if the calculation fails. */
    virtual bool fullStateFitQuality 
      (const PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,int&,double&)
      const = 0;
    /** calculate fit quality in terms of chi2 between two track states.
        The last parameters is the output chi2, ndof is 5. */
    virtual bool  predictedStateFitQuality
      (const PatternTrackParameters&,const PatternTrackParameters&,double&) const = 0;

  };

} // end namespace

inline const InterfaceID& Trk::IPatternParametersUpdator::interfaceID()
{
        return IID_IPatternParametersUpdator;
}

#endif // TRK_IPATTERNPARAMETERSUPDATOR_H


