/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// InDetAlignTrackSelTool.h

// Sergio Gonzalez Sevilla, started 04/7/05
// Miguel Olivo Gomez, extended 07/6/06

// AlgTool to select high quality tracks for the inner detector 
// (Pixel+SCT) alignment algorithms.
// This AlgTool provides a track selection based on the following cut variables:
// Momentum, pt, number of shared hits, number of holes and chi2 probability. 
// Returns 0 in case a track is not accepted, otherwise 1

/** @class InDetAlignTrackSelTool
    @brief The InDetAlignTrackSelTool serves to select high quality tracks for the inner detector 
           (Pixel+SCT) alignment algorithms. This AlgTool provides a track selection based on the 
           following cut variables:
           Momentum, pt, number of shared hits, number of holes and chi2 probability.
           Returns 0 in case a track is not accepted, otherwise 1
    @author Sergio Gonzalez Sevilla, Miguel Olivo Gomez <http://consult.cern.ch/xwho>
    */

#ifndef INDETALIGNGENTOOLS_ALIGNTRACKSELTOOL_H
#define INDETALIGNGENTOOLS_ALIGNTRACKSELTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "InDetAlignGenTools/IInDetAlignTrackSelTool.h"

namespace Trk {
  class Track;
  class ITrackSummaryTool;
}

class InDetAlignTrackSelTool: virtual public IInDetAlignTrackSelTool, public AthAlgTool {
 public:
  InDetAlignTrackSelTool(const std::string& type, const std::string& name,
			 const IInterface* parent);
  virtual ~InDetAlignTrackSelTool();
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual int getStatus(const Trk::Track&) const;

 private:
  double Momentum(const Trk::Track&) const;
  double Pt(const Trk::Track&) const;
  int nShared(const Trk::Track&) const;
  int nHoles(const Trk::Track&) const;
  double chi2Prob(const Trk::Track&) const;
  
  ToolHandle < Trk::ITrackSummaryTool >  m_trackSumTool;  //!<  Pointer to Trk::ITrackSummaryTool

  double m_minMomentum; //!< Minimum value of the momentum of the track
  double m_minPt;  //!< Minimum value of the transverse momentum of the track
  int m_maxShared; //!< Maximum number of shared hits of the track
  int m_maxHoles;  //!< Maximum number of holes of the track
  double m_minChi2Prob;  //!< Minimum value of the chi2 Probality of the track
};

#endif // INDETALIGNTOOLS_ALIGNTRACKSELTOOL_H
