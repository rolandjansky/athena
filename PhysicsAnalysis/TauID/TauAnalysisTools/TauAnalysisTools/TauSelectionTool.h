// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUSELECTIONTOOL_H
#define TAUSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "PATCore/IAsgSelectionTool.h"

// Local include(s):
#include "TauAnalysisTools/ITauSelectionTool.h"

namespace TauAnalysisTools {

  enum SelectionCuts{
    NoCut           = 0,   	// 000000000
    CutPt           = 1,   	// 000000001
    CutAbsEta       = 1<<1,	// 000000010
    CutPhi          = 1<<2,	// 000000100
    CutNTrack       = 1<<3,	// 000001000
    CutAbsCharge    = 1<<4,	// 000010000
    CutJetBDTScore  = 1<<5,	// 000100000
    CutJetIDWP     = 1<<6,	// 001000000
    CutEleBDTScore  = 1<<7,	// 010000000
    CutEleBDTWP     = 1<<8	// 100000000
  };

  enum JetIDWP{
    JetBDTNONE           = 0,
    JetBDTLoose          = 1,
    JetBDTMedium         = 2,
    JetBDTTight          = 3,
    JetBDTLooseNotTight  = 4,
    JetBDTLooseNotMedium = 5,
    JetBDTMediumNotTight = 6
  };

  enum EleBDTWP{
    EleBDTNONE   = 0,
    EleBDTLoose  = 1,
    EleBDTMedium = 2,
    EleBDTTight  = 3
  };

  class TauSelectionTool : public virtual IAsgSelectionTool,
			   public virtual ITauSelectionTool,
			   public asg::AsgTool {

    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS2( TauSelectionTool,
		     IAsgSelectionTool,
		     TauAnalysisTools::ITauSelectionTool )

    public:
    /// Constructor for standalone usage
    TauSelectionTool( const std::string& name );

    /// Function implementing the asg::IAsgTool interface

    /// Function initialising the tool
    virtual StatusCode initialize();

    /// Functions implementing the IAsgSelectionTool interface

    /// Get an object describing the "selection steps" of the tool
    virtual const Root::TAccept& getTAccept() const;

    /// Get the decision using a generic IParticle pointer
    virtual const Root::TAccept& accept( const xAOD::IParticle* p ) const;

    /// Function implementing the ITauSelectorTool interface

    /// Get the decision for a specific TauJet object
    virtual const Root::TAccept& accept( const xAOD::TauJet& tau ) const;

    // Set default recommended properties 
    void setRecommendedProperties();

  private:
    template<typename T, typename U>
    void FillRegionVector(std::vector<T>* vRegion, U tMin, U tMax);
    template<typename T, typename U>
    void FillValueVector(std::vector<T>* vRegion, U tVal);
    template<typename T>
    void PrintConfigRegion(std::string sCutName, std::vector<T>* vRegion);
    template<typename T>
    void PrintConfigValue(std::string sCutName, std::vector<T>* vRegion);
    
    // /// Maximum pseudorapidity for the selected taus
    // double m_dMaxEta;
    // /// Minumum transverse momentum for the selected taus
    // bitmask of tau selection cuts 
    int m_iSelectionCuts;
    // vector of transverse momentum cut regions
    std::vector<double> m_vPtRegion;
    // vector of absolute eta cut regions
    std::vector<double> m_vAbsEtaRegion;
    // vector of absolute charge requirements
    std::vector<int> m_vAbsCharges;
    // vector of number of track requirements
    std::vector<unsigned int> m_vNTracks;
    // vector of JetBDT cut regions
    std::vector<double> m_vJetBDTRegion;
    // JetID working point
    int m_iJetIDWP;
    // vector of EleBDT cut regions
    std::vector<double> m_vEleBDTRegion;
    // EleBDT working point
    int m_iEleBDTWP;
    
    double m_dPtMin;
    double m_dPtMax;
    double m_dAbsEtaMin;
    double m_dAbsEtaMax;
    double m_iAbsCharge;
    double m_iNTrack;
    double m_dJetBDTMin;
    double m_dJetBDTMax;
    double m_dEleBDTMin;
    double m_dEleBDTMax;

    bool checkPtRegion( const xAOD::TauJet& xTau ) const;
    bool checkAbsEtaRegion( const xAOD::TauJet& xTau ) const;
    bool checkAbsCharge( const xAOD::TauJet& xTau ) const;
    bool checkNTracks( const xAOD::TauJet& xTau ) const;
    bool checkJetBDTScore( const xAOD::TauJet& xTau ) const;
    bool checkJetIDWP( const xAOD::TauJet& xTau ) const;
    bool checkEleBDTScore( const xAOD::TauJet& xTau ) const;
    bool checkEleBDTWP( const xAOD::TauJet& xTau ) const;

    void declarePropertiesBaseline();

    /// Object used to store the last decision
    mutable Root::TAccept m_aAccept;

    

  }; // class TauSelectionTool

} // namespace TauAnalysisTools

#endif // TAUSELECTIONTOOL_H
