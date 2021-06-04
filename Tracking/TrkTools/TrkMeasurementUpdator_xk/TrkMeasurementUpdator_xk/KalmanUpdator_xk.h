/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanUpdator_xk.h
//   Header file for Kalman Fitter Updator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 7/6/2005 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRK_KALMANUPDATOR_XK_H
#define TRK_KALMANUPDATOR_XK_H

#include "TrkToolInterfaces/IUpdator.h"
#include "TrkToolInterfaces/IPatternParametersUpdator.h"
#include "AthenaBaseComps/AthAlgTool.h"

namespace Trk {

  /**
     @class KalmanUpdator_xk

     Trk::KalmanUpdator_xk is a set of tools for adding and removing
     measurements to/from the state vector using xKalman algorithms
     @author Igor.Gavrilenko@cern.ch
  */

  class LocalParameters       ;
  class PatternTrackParameters;

  class KalmanUpdator_xk : virtual public IUpdator,
    virtual public IPatternParametersUpdator,
    public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////

    public:

      ///////////////////////////////////////////////////////////////////
      // Standard Athena tool methods
      ///////////////////////////////////////////////////////////////////

      KalmanUpdator_xk
	(const std::string&,const std::string&,const IInterface*);
      virtual ~KalmanUpdator_xk    ();
      virtual StatusCode initialize() override final;
      virtual StatusCode finalize  () override final;

      // /////////////////////////////////////////////////////////////////
      // Main public methods for kalman filter updator tool
      // /////////////////////////////////////////////////////////////////

      // /////////////////////////////////////////////////////////////////
      // Add and remove without Xi2 calculation
      // /////////////////////////////////////////////////////////////////

      //! add without chi2 calculation, PRD-level, EDM track parameters
      virtual std::unique_ptr<TrackParameters> addToState
        (const TrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&) const override final;
      //! add without chi2 calculation, ROT-level, EDM track parameters
      virtual std::unique_ptr<TrackParameters> addToState
        (const TrackParameters&,const LocalParameters&,const Amg::MatrixX&) const override final;

      //! add without chi2 calculation, PRD-level, pattern track parameters
      virtual bool                   addToState
        (PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
         PatternTrackParameters&) const override final;
      //! add without chi2 calculation, ROT-level, pattern track parameters
      virtual bool                   addToState
        (PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,
         PatternTrackParameters&) const override final;
      //! add without chi2 calculation, PRD-level, pattern track parameters, specifically 1D
      virtual bool                   addToStateOneDimension
        (PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
         PatternTrackParameters&) const override final;

      ///////////////////////////////////////////////////////////////////
      // Remove without Xi2 calculation
      ///////////////////////////////////////////////////////////////////

      virtual std::unique_ptr<TrackParameters> removeFromState
	(const TrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&) const override final;
      virtual std::unique_ptr<TrackParameters> removeFromState
	(const TrackParameters&,const LocalParameters&,const Amg::MatrixX&) const override final;

      virtual bool removeFromState
	(PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
	 PatternTrackParameters&) const override final;
      virtual bool removeFromState
	(PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,
	 PatternTrackParameters&) const override final;

      ///////////////////////////////////////////////////////////////////
      // Add  with Xi2 calculation
      ///////////////////////////////////////////////////////////////////

      virtual std::unique_ptr<TrackParameters> addToState
	(const TrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
	 FitQualityOnSurface*&) const override final;
      virtual std::unique_ptr<TrackParameters> addToState
	(const TrackParameters&,const LocalParameters&,const Amg::MatrixX&,
	 FitQualityOnSurface*&) const override final;
      virtual std::pair<AmgVector(5),AmgSymMatrix(5)>* updateParameterDifference
	(const AmgVector(5)&,const AmgSymMatrix(5)&, const Amg::VectorX&, const Amg::MatrixX&,
	 const int&,Trk::FitQualityOnSurface*&,bool) const override final;

      virtual bool addToState
	(PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
	 PatternTrackParameters&,double&,int&) const override final;
      virtual bool addToState
	(PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,
	 PatternTrackParameters&,double&,int&) const override final;
      virtual bool                   addToStateOneDimension
	(PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
	 PatternTrackParameters&,double&,int&) const override final;

      ///////////////////////////////////////////////////////////////////
      // Remove with Xi2 calculation
      ///////////////////////////////////////////////////////////////////

      virtual std::unique_ptr<TrackParameters> removeFromState
	(const TrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
	 FitQualityOnSurface*&) const override final;
      virtual std::unique_ptr<TrackParameters> removeFromState
	(const TrackParameters&,const LocalParameters&,const Amg::MatrixX&,
	 FitQualityOnSurface*&) const override final;

      virtual bool removeFromState
	(PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
	 PatternTrackParameters&,double&,int&) const override final;
      virtual bool removeFromState
	(PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,
	 PatternTrackParameters&,double&,int&) const override;

      ///////////////////////////////////////////////////////////////////
      // Combine two state with or without Xi2 calculation
      ///////////////////////////////////////////////////////////////////

      virtual std::unique_ptr<TrackParameters> combineStates
	(const TrackParameters&, const TrackParameters&) const override final;
      virtual std::unique_ptr<TrackParameters> combineStates
	(const TrackParameters&, const TrackParameters&,
	 FitQualityOnSurface*&) const override final;

      virtual bool combineStates
	(PatternTrackParameters&,PatternTrackParameters&,PatternTrackParameters&) const override final;
      virtual bool combineStates
	(PatternTrackParameters&,PatternTrackParameters&,PatternTrackParameters&,
	 double&) const override final;

      ///////////////////////////////////////////////////////////////////
      // Xi2 calculation
      ///////////////////////////////////////////////////////////////////

      virtual const FitQualityOnSurface* predictedStateFitQuality
	(const TrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&) const override final;
      virtual const FitQualityOnSurface* predictedStateFitQuality
	(const TrackParameters&,const LocalParameters&,const Amg::MatrixX&) const override final;
      virtual const FitQualityOnSurface* fullStateFitQuality
	(const TrackParameters&,const Amg::Vector2D&,  const Amg::MatrixX&) const override final;
      virtual const FitQualityOnSurface* fullStateFitQuality
	(const TrackParameters&,const LocalParameters&,const Amg::MatrixX&) const override final;
      virtual const FitQualityOnSurface*  predictedStateFitQuality
	(const TrackParameters&,const TrackParameters&) const override final;

      virtual bool predictedStateFitQuality
	(const PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,int&,double&)
	const override final;
      virtual bool predictedStateFitQuality
	(const PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,int&,double&)
	const override final;
      virtual bool fullStateFitQuality
	(const PatternTrackParameters&,const Amg::Vector2D&,  const Amg::MatrixX&,int&,double&)
	const override final;
      virtual bool fullStateFitQuality
	(const PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,int&,double&)
	const override final;
      virtual bool  predictedStateFitQuality
	(const PatternTrackParameters&,const PatternTrackParameters&,double&) const override final;

      ///////////////////////////////////////////////////////////////////
      // let the client tools know how the assumptions on the initial
      // precision for non-measured track parameters are configured
      ///////////////////////////////////////////////////////////////////

      virtual std::vector<double> initialErrors() const override final;

    protected:

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      ///////////////////////////////////////////////////////////////////
      // Updators
      ///////////////////////////////////////////////////////////////////

      std::unique_ptr<TrackParameters> update
	(const TrackParameters&,const Amg::Vector2D&,const Amg::MatrixX&,
	 FitQualityOnSurface*&,int,bool) const;

      std::unique_ptr<TrackParameters> update
	(const TrackParameters&,const LocalParameters&,const Amg::MatrixX&,
	 FitQualityOnSurface*&,int,bool) const;

      bool                   update
	(PatternTrackParameters&,const Amg::Vector2D&,const Amg::MatrixX&,
	 int,bool,PatternTrackParameters&,double&,int&) const;

      bool                   updateOneDimension
	(PatternTrackParameters&,const Amg::Vector2D&,const Amg::MatrixX&,
	 int,bool,PatternTrackParameters&,double&) const;

      bool                   update
	(PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,
	 int,bool,PatternTrackParameters&,double&,int&) const;

      ///////////////////////////////////////////////////////////////////
      // Xi2 calculation
      ///////////////////////////////////////////////////////////////////

      static bool predictedStateFitQuality
	(const double*,const Amg::Vector2D&,const Amg::MatrixX&,int&,double&) ;

      static bool fullStateFitQuality
	(const double*,const Amg::Vector2D&,const Amg::MatrixX&,int&,double&) ;

      ///////////////////////////////////////////////////////////////////
      // Converters
      ///////////////////////////////////////////////////////////////////

      static bool trackParametersToUpdator
	(const        TrackParameters&,double*,double*) ;

      static bool trackParametersToUpdator
	(const PatternTrackParameters&,double*,double*) ;

      static bool localParametersToUpdator
	(const LocalParameters&,const Amg::MatrixX&,int&,int&,double*,double*) ;

    static std::unique_ptr<TrackParameters> updatorToTrackParameters
	(const TrackParameters&,double*,double*) ;

      ///////////////////////////////////////////////////////////////////
      // Update no measured track parameters
      ///////////////////////////////////////////////////////////////////

      bool updateNoMeasuredWithOneDim
	(const double*,const double*,double*,double*) const;

      bool updateNoMeasuredWithTwoDim
	(const double*,const double*,double*,double*) const;

      bool updateNoMeasuredWithAnyDim
	(const double*,const double*,double*,double*,int) const;

      ///////////////////////////////////////////////////////////////////
      // Update measured track parameters
      ///////////////////////////////////////////////////////////////////

      static bool updateWithOneDim
	(int,bool,const double*,const double*,double*,double*,double&) ;

      static bool updateWithTwoDim
	(int,bool,const double*,const double*,double*,double*,double&) ;

      static bool updateWithTwoDimParameters
	(int,bool,const double*,const double*,double*,const double*,double&) ;

      static bool updateWithFiveDim
	(    bool,double*,double*,double*,double*,double&) ;

      bool updateWithAnyDim
	(int,bool,double*,const double*,double*,double*,double&,
	 int,int) const;

      bool updateWithOneDimWithBoundary
	(int,bool,double*,double*,double*,double*,double&) const;

      bool updateWithTwoDimWithBoundary
	(int,bool,double*,double*,double*,double*,double&) const;

      static bool invert (int,double*,double*) ;
      static bool invert2(    const double*,double*) ;
      static bool invert3(    const double*,double*) ;
      static bool invert4(    const double*,double*) ;
      static bool invert5(    const double*,double*) ;

      static double Xi2    (int,double*,double*) ;
      static double Xi2for1(    const double*,const double*) ;
      static double Xi2for2(    const double*,const double*) ;
      static double Xi2for3(    const double*,const double*) ;
      static double Xi2for4(    const double*,const double*) ;
      static double Xi2for5(    const double*,const double*) ;

      static int  differenceParLoc(int,const double*,const double*,double*) ;
      static void differenceLocPar(int,const double*,const double*,double*) ;
      static void testAngles(double*,double*) ;

      void mapKeyProduction();

      ///////////////////////////////////////////////////////////////////
      // Protected data members
      ///////////////////////////////////////////////////////////////////

      std::vector<double> m_cov0;
      unsigned int m_key[ 33]{};
      unsigned int m_map[160]{};
      double   m_covBoundary;
    };

  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

} // end of namespace

#endif // TRK_KALMANUPDATOR_XK_H
