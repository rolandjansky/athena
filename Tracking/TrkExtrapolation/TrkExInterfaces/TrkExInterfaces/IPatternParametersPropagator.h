/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPatternParametersPropagator.h
//   Main public methods for track state propagation throughout atlas using
// internal pattern recognition objects for track parameters 
// (the Trk::PatternTrackParameters)
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_IPATTERNPARAMETERSPROPAGATOR_H
#define TRK_IPATTERNPARAMETERSPROPAGATOR_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

/** Interface ID for IPropagators*/  
static const InterfaceID IID_IPatternParametersPropagator("IPatternParametersPropagator", 1, 0);

namespace Trk {

  class PatternTrackParameters;
  class Surface;
  class CylinderBounds;
  class MagneticFieldProperties;

  /** @class IPatternParametersPropagator

      @brief interface for track parameter propagation through the magnetic field, using
             the Trk::PatternTrackParameters data model for fast, internal use.

      @author Igor.Gavrilenko -at- cern.ch, Wolfgang.Liebig -at- cern.ch
  */

  class IPatternParametersPropagator : virtual public IAlgTool {
  public:
    /////////////////////////////////////////////////////////////////////////////////
    // Public method for Trk::PatternTrackParameters (not in IPropagator)
    /////////////////////////////////////////////////////////////////////////////////

    //! framework method providing the abstract interface
    static const InterfaceID& interfaceID( ) ;

    /** Interfaces WITH EventContext (new) */
      
    /** Main propagation method */
    virtual bool propagate
      (const ::EventContext&          ctx,
       PatternTrackParameters         &,
       const Surface                  &,
       PatternTrackParameters         &,
       PropDirection                   ,
       const MagneticFieldProperties  &, 
       ParticleHypothesis particle=pion) const = 0;

    /** Main propagation method with step to surface calculation*/
    virtual bool propagate
      (const ::EventContext&          ctx,
       PatternTrackParameters         &,
       const Surface                  &,
       PatternTrackParameters         &,
       PropDirection                   ,
       const MagneticFieldProperties  &,
       double                         &,
       ParticleHypothesis particle=pion) const = 0;
      
      /** Main propagation method for parameters only */
    virtual bool propagateParameters
      (const ::EventContext&          ctx,
       PatternTrackParameters         &,
       const Surface                  &,
       PatternTrackParameters         &,
       PropDirection                   ,
       const MagneticFieldProperties  &,
       ParticleHypothesis particle=pion) const = 0;
      
      /** Main propagation method for parameters only with step to surface calculation*/
    virtual bool propagateParameters
      (const ::EventContext&          ctx,
       PatternTrackParameters         &,
       const Surface                  &,
       PatternTrackParameters         &,
       PropDirection                   ,
       const MagneticFieldProperties  &,
       double                         &,
       ParticleHypothesis particle=pion) const = 0;

      /** GlobalPositions list interface */
    virtual void globalPositions
      (const ::EventContext&          ctx,
       std::list<Amg::Vector3D>       &,
       const PatternTrackParameters   &,
       const MagneticFieldProperties  &,
       const CylinderBounds           &,
       double                          ,
       ParticleHypothesis particle=pion) const = 0;

      /** GlobalPostions and steps for set surfaces */
    virtual void globalPositions
      (const ::EventContext&          ctx,
       const PatternTrackParameters                 &,
       std::list<const Surface*>                    &,
       std::list< std::pair<Amg::Vector3D,double> > &,
       const MagneticFieldProperties                &,
       ParticleHypothesis particle=pion              ) const = 0;
      

    /** Interfaces WITHOUT EventContext (old) */

    /** Main propagation method */
    virtual bool propagate
      (PatternTrackParameters         &,
       const Surface                  &,
       PatternTrackParameters         &,
       PropDirection                   ,
       const MagneticFieldProperties  &, 
       ParticleHypothesis particle=pion) const;

    /** Main propagation method with step to surface calculation*/
    virtual bool propagate
      (PatternTrackParameters         &,
       const Surface                  &,
       PatternTrackParameters         &,
       PropDirection                   ,
       const MagneticFieldProperties  &,
       double                         &,
       ParticleHypothesis particle=pion) const;
      
      /** Main propagation method for parameters only */
    virtual bool propagateParameters
      (PatternTrackParameters         &,
       const Surface                  &,
       PatternTrackParameters         &,
       PropDirection                   ,
       const MagneticFieldProperties  &,
       ParticleHypothesis particle=pion) const;
      
      /** Main propagation method for parameters only with step to surface calculation*/
    virtual bool propagateParameters
      (PatternTrackParameters         &,
       const Surface                  &,
       PatternTrackParameters         &,
       PropDirection                   ,
       const MagneticFieldProperties  &,
       double                         &,
       ParticleHypothesis particle=pion) const;

      /** GlobalPositions list interface */
    virtual void globalPositions
      (std::list<Amg::Vector3D>       &,
       const PatternTrackParameters   &,
       const MagneticFieldProperties  &,
       const CylinderBounds           &,
       double                          ,
       ParticleHypothesis particle=pion) const;

      /** GlobalPostions and steps for set surfaces */
    virtual void globalPositions
      (const PatternTrackParameters                 &,
       std::list<const Surface*>                    &,
       std::list< std::pair<Amg::Vector3D,double> > &,
       const MagneticFieldProperties                &,
       ParticleHypothesis particle=pion              ) const;
      
  };
} // end namespace

inline const InterfaceID& Trk::IPatternParametersPropagator::interfaceID()
{
        return IID_IPatternParametersPropagator;
}

/** temporarily default to old interfaces, taking the EventContext from GaudiHive */


    /** Main propagation method */
inline bool Trk::IPatternParametersPropagator::propagate
      (Trk::PatternTrackParameters         &ptp1,
       const Trk::Surface                  &surf,
       Trk::PatternTrackParameters         &ptp2,
       Trk::PropDirection                   pd,
       const Trk::MagneticFieldProperties  &mf, 
       Trk::ParticleHypothesis             particle) const
{
    return propagate(Gaudi::Hive::currentContext(), ptp1, surf, ptp2, pd, mf, particle);
}


    /** Main propagation method with step to surface calculation*/
inline bool Trk::IPatternParametersPropagator::propagate
      (Trk::PatternTrackParameters         &ptp1,
       const Trk::Surface                  &surf,
       Trk::PatternTrackParameters         &ptp2,
       Trk::PropDirection                   pd,
       const Trk::MagneticFieldProperties  &mf,
       double                              &step,
       Trk::ParticleHypothesis              particle) const
{
    return propagate(Gaudi::Hive::currentContext(), ptp1, surf, ptp2, pd, mf, step, particle);
}

      /** Main propagation method for parameters only */
inline bool Trk::IPatternParametersPropagator::propagateParameters
      (Trk::PatternTrackParameters         &ptp1,
       const Trk::Surface                  &surf,
       Trk::PatternTrackParameters         &ptp2,
       Trk::PropDirection                   pd,
       const Trk::MagneticFieldProperties  &mf,
       Trk::ParticleHypothesis              particle) const
{
    return propagateParameters(Gaudi::Hive::currentContext(), ptp1, surf, ptp2, pd, mf, particle);
}

      
      /** Main propagation method for parameters only with step to surface calculation*/
inline bool Trk::IPatternParametersPropagator::propagateParameters
      (Trk::PatternTrackParameters         &ptp1,
       const Trk::Surface                  &surf,
       Trk::PatternTrackParameters         &ptp2,
       Trk::PropDirection                   pd,  
       const Trk::MagneticFieldProperties  &mf,  
       double                              &step,
       Trk::ParticleHypothesis             particle) const
{
    return propagateParameters(Gaudi::Hive::currentContext(), ptp1, surf, ptp2, pd, mf, step, particle);
}


      /** GlobalPositions list interface */
inline void Trk::IPatternParametersPropagator::globalPositions
      (std::list<Amg::Vector3D>            &gp, 
       const Trk::PatternTrackParameters   &ptp,
       const Trk::MagneticFieldProperties  &mf,
       const Trk::CylinderBounds           &cb,
       double                               step,
       Trk::ParticleHypothesis              particle) const
{
    return globalPositions(Gaudi::Hive::currentContext(), gp, ptp, mf, cb, step, particle);
}

      /** GlobalPostions and steps for set surfaces */
inline void Trk::IPatternParametersPropagator::globalPositions
      (const Trk::PatternTrackParameters                 &ptp,  
       std::list<const Trk::Surface*>                    &surfs,
       std::list< std::pair<Amg::Vector3D,double> >      &gp,
       const Trk::MagneticFieldProperties                &mf,
       Trk::ParticleHypothesis                           particle) const
{
    return globalPositions(Gaudi::Hive::currentContext(), ptp, surfs, gp, mf, particle);
}


#endif // TRK_IPATTERNPARAMETERSPROPAGATOR_H
