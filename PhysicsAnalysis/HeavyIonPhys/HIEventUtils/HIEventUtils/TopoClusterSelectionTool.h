/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/



////////////////////////////////////////////////////////////////////////////
/// \class TopoClusterSelectionTool
/// \author Aaron Angerami <angerami@cern.ch>
/// \brief dual-use tool implementing IAsgSelectionTool interface that 
/// provides selection of topoclusters
/// selection motivated by topocluster use in UPC gap quantity calculation
/// includes eta-dependent minimum cut on CELL_SIGNIFICANCE moment
/// designed to ensure eta-independent probability of pure noise cluster
////////////////////////////////////////////////////////////////////////////

#ifndef HI_TOPOCLUSTERSELECTION_TOOL_H
#define HI_TOPOCLUSTERSELECTION_TOOL_H

#include "xAODCaloEvent/CaloCluster.h"
#include "PATCore/IAsgSelectionTool.h"
#include "AsgTools/AsgTool.h"
#include "PATCore/TAccept.h"

class TH1F;
namespace HI
{

  ///
  /// Interface class for TopoClusterSelectionTool
  ///
  class ITopoClusterSelectionTool : virtual public IAsgSelectionTool
  {
    ASG_TOOL_INTERFACE(ITopoClusterSelectionTool)
  public:
    virtual ~ITopoClusterSelectionTool() {};

    ///
    /// In addition to IAsgSelectionTool interface, add method to evaluate directly on CaloCluster
    ///
    virtual const Root::TAccept& accept(const xAOD::CaloCluster& ) const = 0;

    ///
    /// IAsgSelectionTool interface method, declaration needed here to prevent above
    /// method from hiding IAsgSelectionTool::accept(const xAOD::IParticle*), which generates compiler warning
    ///
    virtual const Root::TAccept& accept( const xAOD::IParticle* /*part*/ ) const = 0;
    
  };

  ///
  /// Tool to do topocluster selection
  ///
  class TopoClusterSelectionTool : public asg::AsgTool, virtual public ITopoClusterSelectionTool
  {
    ASG_TOOL_CLASS(TopoClusterSelectionTool,ITopoClusterSelectionTool)
  public:

    ///
    /// Constructor
    ///
    TopoClusterSelectionTool(const std::string& name);
    virtual ~TopoClusterSelectionTool() {};

    ///
    /// Initialize tool
    ///
    virtual StatusCode initialize() override;

    ///
    /// IAsgSelectionTool interface method to return accept object
    ///
    const Root::TAccept& getTAccept() const override;

    ///
    /// IAsgSelectionTool interface method to evaluate whether particle passes selection
    ///
    virtual const Root::TAccept& accept(const xAOD::IParticle* p) const override;

    ///
    /// ITopoClusterTool interface method to evaluate on cluster directly
    ///
    virtual const Root::TAccept& accept(const xAOD::CaloCluster& cl) const override;

    ///
    /// Method to check whether cluster has significance moments set
    ///
    StatusCode checkClusterForMoments(const xAOD::CaloCluster& cl) const;
    
  private:

    ///
    /// Internal helper method to look up value of significance cut
    ///
    float lookupCellSignificanceCut(float eta, int clCellSigSamp) const;
    bool m_isInitialized; //True if initialize has been called

    //Calibration file/histogram contains information about significance cuts vs eta
    std::string m_calibFileName; //!< name of TFile containing calibration histogram
    std::string m_calibHistoName; //!< name of TH1 in calibration file
    float m_minPt; //!< minimum pT for cluster to pass selection
    float m_minEnergy; //!< minimum energy for cluster to pass selection
    float  m_minEta; //!< minimum eta for cluster to pass selection
    float m_maxEta; //!< maximum eta for cluster to pass selection
    bool m_doAbsEta; //!< if true, max/min eta cuts applied to |cluster eta|
    bool m_doSigCuts; //!< if true, cluster required to pass eta-dependent max cell siginificance cut
    TH1F* m_h1TCSigCut; //!< histogram containing eta-dependent significance cuts

    //indices of various cuts in TAccept object
    unsigned int m_minPtCutIndex;
    unsigned int m_minEnergyCutIndex; 
    unsigned int m_minEtaCutIndex;
    unsigned int m_maxEtaCutIndex;
    unsigned int m_minCellSignificanceCutIndex;

    mutable Root::TAccept m_accept;


  };
}
#endif
