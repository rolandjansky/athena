// -*- c++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSYSTEMATICSTOOLS_INDETTRACKTRUTHFILTERTOOL_H
#define INDETTRACKSYSTEMATICSTOOLS_INDETTRACKTRUTHFILTERTOOL_H

#include "InDetTrackSystematicsTools/IInDetTrackTruthFilterTool.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicSet.h"
#include "PathResolver/PathResolver.h"
#include "InDetTrackSystematicsTools/InDetTrackSystematicsTool.h"

#include "xAODTracking/TrackParticle.h"
#include <string>

class TH2;
class TRandom3;
class TFile;

namespace InDet {

  class IInDetTrackTruthOriginTool;

  /// @class InDetTrackTruthFilterTool
  /// This class selects tracks based on their truth origin
  /// @author Remi Zaidan (remi.zaidan@cern.ch)
  /// @author Felix Clark (michael.ryan.clark@cern.ch)

  class InDetTrackTruthFilterTool
    : public virtual IInDetTrackTruthFilterTool
    , public virtual InDetTrackSystematicsTool
  //    , public asg::AsgTool 
  {
    
    // create constructor for Athena
    ASG_TOOL_CLASS( InDetTrackTruthFilterTool,
		    InDet::IInDetTrackTruthFilterTool )
    
  public:
    // create constructor for standalone Root
    InDetTrackTruthFilterTool( const std::string& name );
    virtual ~InDetTrackTruthFilterTool() = default;
    
    //  static const InterfaceID& interfaceID();
    virtual StatusCode initialize() override;
    virtual void prepare() override {};
    virtual StatusCode finalize() override;

    // right now this returns a bool; if we want to implement the ASG selection tool interface then this will need to change to a TAccept

     //"standard" accept method - if appropriate systematic is activated, will call mu-dependent version (below) using mu value from EventInfo, otherwise will use truth info
    virtual bool accept(const xAOD::TrackParticle* track) const override;

     //This is a version of the accept method that takes a value of mu (i.e. mean interactions per crossing) in order to calculate a probability that a givent track at that mu is a fake, and so should be considered for being dropped for the fake systematic variation - this version does not rely on truth information
    virtual bool accept(const xAOD::TrackParticle* track, float mu) const override;  

    /// returns: whether the tool is affected by the systematic
    virtual bool isAffectedBySystematic( const CP::SystematicVariation& ) const override;
    /// returns: list of systematics this tool can be affected by
    virtual CP::SystematicSet affectingSystematics() const override;
    /// returns: list of recommended systematics to use with this tool
    virtual CP::SystematicSet recommendedSystematics() const override;
    /// configure the tool to apply a given list of systematic variations
    virtual CP::SystematicCode applySystematicVariation( const CP::SystematicSet& ) override;

  private:

    StatusCode initTrkEffSystHistogram(float scale, TH2 *&histogram, std::string rootFileName, std::string histogramName) const;
    float getFractionDropped(float fDefault, TH2 *histogram, float pt, float eta) const;
    float pseudoFakeProbability(const xAOD::TrackParticle* track, float mu) const;
    bool dropPseudoFake(float prob) const;

    int m_seed = 42;
    std::unique_ptr<TRandom3> m_rnd; //!
    
    float m_fPrim = 1.;
    float m_fSec = 1.;
    float m_fFakeLoose = 0.10;
    float m_fFakeTight = 1.00; // this method breaks down for uncertainties > 1.00 (as was present in previous iterations)
    float m_fPU = 1.;
    float m_fFrag = 1.;
    float m_fFromC = 1.;
    float m_fFromB = 1.;
    float m_trkEffSystScale = 1.;

    TH2 *m_fPrimHistogram = nullptr;
    TH2 *m_fSecHistogram = nullptr;
    //TH2 *m_fFakeHistogram = nullptr;
    TH2 *m_fPUHistogram = nullptr;
    TH2 *m_fFragHistogram = nullptr;
    TH2 *m_fFromCHistogram = nullptr;
    TH2 *m_fFromBHistogram = nullptr;


    TH2* m_trkEffHistLooseGlobal = nullptr;
    TH2* m_trkEffHistLooseIBL = nullptr;
    TH2* m_trkEffHistLoosePP0 = nullptr;
    TH2* m_trkEffHistLoosePhysModel = nullptr;
    TH2* m_trkEffHistTightGlobal = nullptr;
    TH2* m_trkEffHistTightIBL = nullptr;
    TH2* m_trkEffHistTightPP0 = nullptr;
    TH2* m_trkEffHistTightPhysModel = nullptr;

    // allow the user to configure which calibration files to use if desired
    std::string m_calibFileNomEff;

    ToolHandle< IInDetTrackTruthOriginTool > m_trackOriginTool;
    

}; // class InDetTrackTruthFilterTool

} // namespace InDet

#endif
