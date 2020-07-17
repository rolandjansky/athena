/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNEVENT_ALIGNTSOS_H
#define TRKALIGNEVENT_ALIGNTSOS_H

#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "Identifier/Identifier.h"
#include "TrkAlignEvent/Residual.h"

#include <vector>

/**
   @file AlignTSOS.h
   @class AlignTSOS

   @brief AlignTSOS is a TSOS with extra variables useful for alignment.  It is created using an implementation of IAlignTrackCreator and stored on an AlignTrack.

   @author Robert Harrington <roberth@bu.edu>
   @author Daniel Kollar <daniel.kollar@cern.ch>
   @date 10/14/2008
*/

namespace CLHEP {
  class HepVector;
}

namespace Trk {
  
  class CompetingRIOsOnTrack;
  class RIO_OnTrack;
  class AlignModule;
  class TrkDetElementBase;

  class AlignTSOS : public TrackStateOnSurface {
    
  public:

    /** constructor takes:
        1) the TSOS from the original track, 
        2) the AlignModule containing the TrkDetElementBase in which the TSOS was measured (NULL for a scatterer),
        3) nresdim, the dimensionality of the measurement,
        4) the RIO if the TSOS is a RIO, or the leading RIO if a CompetingRIOsOnTrack,
        5) and the detector type (from TrackStateDefs.h)
    */
    AlignTSOS(const TrackStateOnSurface& tsos, const AlignModule * module, 
	      const RIO_OnTrack * rio, 
	      TrackState::MeasurementType detType);	      

    AlignTSOS(const TrackStateOnSurface& tsos, const AlignModule * module, 
	      const CompetingRIOsOnTrack * crio, 
	      TrackState::MeasurementType detType);

    AlignTSOS(const TrackStateOnSurface& tsos, 
	      TrackState::MeasurementType measType);

    /** copy constructor */
    AlignTSOS(const AlignTSOS& atsos);

    /** destructor */
    virtual ~AlignTSOS();

    /** asignment operator */
    AlignTSOS& operator=(const AlignTSOS& atsos);

    /** accessor method for AlignModule to which tsos belongs */
    const AlignModule* module() const { return m_module; }

    /** set and retrieve flag to indicate whether hit valid for alignment */
    void setValid(bool isvalid) { m_isvalid=isvalid; }    
    bool isValid() const { return m_isvalid; }
    
    /** returns number of measurement residual + scatterer residual dimensions */
    int nResDim() const { return m_residuals.size(); }

    /** returns measurement type enum */
    TrackState::MeasurementType measType() const { return m_measType; }

    /** returns Identifier of RIO */
    Identifier identify() const;

    /** returns RIO_OnTrack or leading RIO of CompetingRIOsOnTrack (assigned by c'tor) */
    const RIO_OnTrack* rio() const;

    /** sets CompetingRIOsOnTrack */
    void setCompetingRIOsOnTrack(const CompetingRIOsOnTrack* crio) { m_crio=crio; }

    /** returns CompetingRIOsOnTrack */
    const CompetingRIOsOnTrack* crio() const { return m_crio; }

    /** pushes back vector of Residuals to alignTSOS residuals */
    void addResidual(const Residual& residual) { m_residuals.push_back(residual); }
   
    /** returns vector of Residuals*/
    const std::vector<Residual>& residuals() const { return m_residuals; }

    /** clears vector of residuals */
    void clearResiduals() { m_residuals.clear(); }

    /** returns first Residual iterator */
    std::vector<Residual>::const_iterator firstResidual() const { return m_residuals.begin(); }

    /** returns last Residual iterator */
    std::vector<Residual>::const_iterator lastResidual() const { return m_residuals.end(); }

    /** method for retrieving detector elements (one if RIO, more if CRIO) */
    const TrkDetElementBase* detelement(int i=0) const { return m_detelements[i]; }

    /** method for retrieving number of detector elements */
    int nDetElements() const { return m_detelements.size(); }

    /** returns string corresponding to the measurement type */
    std::string dumpMeasType() const;

    /** returns strip angle for fan-out structured modules (SCT endcap) */
    void setAlphaStrip(double alpha) { m_alphaStrip = alpha; }

    /** returns strip angle for fan-out structured modules (SCT endcap) */
    double alphaStrip() const { return m_alphaStrip; }

    /** set the measurement direction */
    void setMeasDir(Trk::ParamDefs dir) { m_measDir = dir; }

    /** retrieve the measurement direction */
    Trk::ParamDefs measDir() const { return m_measDir; }

    /** returns string corresponding to the measurement direction */
    std::string dumpMeasDir() const;

    /** returns pointer to unbiased track parameters if present */
    const TrackParameters * unbiasedTrackPars() const { return m_unbiasedTrackPars; }

    /** setter for unbiased track parameters */
    void setUnbiasedTrackPars(const TrackParameters * trkPars) { m_unbiasedTrackPars = trkPars; }

    /** returns pointer to vector of derivatives */
    const std::vector<Amg::VectorX> * derivatives() const { return m_derivatives; }

    /** returns pointer to vector of derivatives w.r.t. the vertex */
    const std::vector<Amg::VectorX> * derivativesVtx() const { return m_derivativesVtx; }

    /** setter for the derivatives */
    void setDerivatives(std::vector<Amg::VectorX> * derivs) { m_derivatives = derivs; }

    /** setter for the derivatives w.r.t. the vertex */
    void setDerivativesVtx(std::vector<Amg::VectorX> * derivs) { m_derivativesVtx = derivs; }

    virtual TrackStateOnSurface::Variety variety() const override final {
      return TrackStateOnSurface::Align;
    }
   private:
    // private methods
    AlignTSOS();  // don't use this one

    // private variables    
    const AlignModule * m_module;
    const RIO_OnTrack * m_rio;   
    const CompetingRIOsOnTrack * m_crio;

    const TrackParameters * m_unbiasedTrackPars;

    std::vector<Residual> m_residuals;

    std::vector<const TrkDetElementBase *> m_detelements;

    std::vector<Amg::VectorX> * m_derivatives;

    bool   m_isvalid;

    TrackState::MeasurementType m_measType;

    Trk::ParamDefs m_measDir; //!< direction of measurement (set to Trk::x by default)

    double m_alphaStrip;

    std::vector<Amg::VectorX> * m_derivativesVtx;

  }; // end class
  
  /**Overload of << operator for MsgStream for debug output*/ 
  MsgStream& operator << ( MsgStream& sl, const AlignTSOS& atsos);
  
} // end namespace

#endif // TRKALIGNEVENT_ALIGNTSOS_H
