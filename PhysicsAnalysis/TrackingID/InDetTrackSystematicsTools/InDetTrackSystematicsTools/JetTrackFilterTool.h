// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSYSTEMATICSTOOLS_JETTRACKFILTERTOOL_H
#define INDETTRACKSYSTEMATICSTOOLS_JETTRACKFILTERTOOL_H

#include "InDetTrackSystematicsTools/IJetTrackFilterTool.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicSet.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODJet/JetContainer.h"
#include "InDetTrackSystematicsTools/InDetTrackSystematicsTool.h"

#include <string>

class TH1;
class TH2;
class TRandom3;
class TFile;

namespace InDet {

  /// @class JetTrackFilterTool
  /// This tool randomly discards tracks in the core of a jet
  /// @author Felix Clark (michael.ryan.clark@cern.ch)

  class JetTrackFilterTool
    : public virtual IJetTrackFilterTool
    , public virtual InDetTrackSystematicsTool
  //    , public asg::AsgTool 
  {
    
    // create constructor for Athena
    ASG_TOOL_CLASS( JetTrackFilterTool,
		    InDet::IJetTrackFilterTool )
    
  public:
    // create constructor for standalone Root
    JetTrackFilterTool( const std::string& name );
    virtual ~JetTrackFilterTool();
    
    //  static const InterfaceID& interfaceID();
    virtual StatusCode initialize() override;
    virtual void prepare() override {};

    // right now this returns a bool; if we want to implement the ASG selection tool interface then this will need to change to a TAccept
    virtual bool accept( const xAOD::TrackParticle*, const xAOD::Jet* ) const override;
    virtual bool accept( const xAOD::TrackParticle*, const xAOD::JetContainer* ) const override;

    /// returns: whether the tool is affected by the systematic
    virtual bool isAffectedBySystematic( const CP::SystematicVariation& ) const override;
    /// returns: list of systematics this tool can be affected by
    virtual CP::SystematicSet affectingSystematics() const override;
    /// returns: list of recommended systematics to use with this tool
    virtual CP::SystematicSet recommendedSystematics() const override;
    /// configure the tool to apply a given list of systematic variations
    virtual CP::SystematicCode applySystematicVariation( const CP::SystematicSet& ) override;

  private:

    StatusCode initTIDEEffSystHistogram(float scale, TH1 *&histogram, std::string rootFileName, std::string histogramName) const;
    float getFractionDropped(float fDefault, TH1 *histogram, const xAOD::Jet*) const;
    float getNomTrkEff(const xAOD::TrackParticle*) const;

    int m_seed = 0;
    std::unique_ptr<TRandom3> m_rnd; //!
    double m_deltaR = 0.1;
    float m_trkEffSystScale = 1.0;
    
    TH1* m_effForJetPt = nullptr; //!
    TH2* m_trkNomEff = nullptr; //!

}; // class JetTrackFilterTool

} // namespace InDet

#endif
