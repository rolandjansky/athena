/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetIsoTrackSelectorTool_InDetIsoTrackSelectorTool_H
#define InDetIsoTrackSelectorTool_InDetIsoTrackSelectorTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/IIsoTrackSelectorTool.h"
#include "TrkParameters/TrackParameters.h"

/**
 * @file InDetIsoTrackSelectorTool.h
 * @class InDetIsoTrackSelectorTool
 *
 * @brief A tool to be used for track preselection in isolation,
 * the given AtaLine is the transported lepton to the BeamLine
 * tracks can be checkec w.r.t to it
 *
 * @author Andreas Salzburger
*/

namespace Trk {
  class IExtrapolator;
  class ITrackSelectorTool;
  class Track;
  class TrackParticleBase;
}

namespace InDet
{
  class InDetIsoTrackSelectorTool : virtual public Trk::IIsoTrackSelectorTool, public AthAlgTool
  {

    public:
      /** Athena AlgTool methods */
      virtual StatusCode initialize() override;

      /** Constructor / Destructor */
      InDetIsoTrackSelectorTool(const std::string& t, const std::string& n, const IInterface*  p);
      ~InDetIsoTrackSelectorTool();

      /** ESD type interface */
      virtual bool decision(const Trk::AtaStraightLine&, const Trk::Track& track) const override;
      
      /** AOD type interface */
      virtual bool decision(const Trk::AtaStraightLine&, const Trk::TrackParticleBase& trackParticle) const override;
      
      /** Work-horse interface - will ignore TrackSelector */
      virtual bool decision(const Trk::AtaStraightLine&, const Trk::TrackParameters& trackPars) const override;

    private:
      /** Robust cut window setting */
      bool                                  m_robustCuts;
      bool                                  m_applySinThetaCorrection;
      double                                m_d0max;
      double                                m_z0stMax;
      /** Sophisticated cut window setting : d0/z0 significance - only when robustCuts off*/
      double                                m_d0Significance;
      double                                m_z0Significance;
      
      ToolHandle<Trk::IExtrapolator>        m_extrapolator;      //<! Extrapolator tool
      /** Extra checks on hits & holes */
      ToolHandle<Trk::ITrackSelectorTool>   m_trackSelector;     //!< track selector tool
      

  }; //end of class definitions
} //end of namespace definitions

#endif //TrkMultipleVertexSeedFinders_PVFindingTrackSelectoTool_H
