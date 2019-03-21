/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__MET_TOOL_H
#define QUICK_ANA__MET_TOOL_H

//
// This module declares the MetTool for calculating the MissingET and its
// associated systematics in QuickAna. Latest recommendations come from
// the following page:
// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetEtmissRecommendationsMC15
//


#include <QuickAna/Global.h>

#include <AthContainers/AuxElement.h>
#include <QuickAna/AnaTool.h>
#include <AsgTools/AnaToolHandle.h>

class IMETMaker;
class IMETSystematicsTool;
class IJetModifier;
class IMETSignificance;

namespace ana
{

  /// @brief An AnaTool used to calculate MET and the MET systematics.
  ///
  class MetTool : virtual public AnaTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (MetTool, ana::IAnaTool)

  public:

    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
    MetTool (const std::string& name);

    /// \copydoc IAnaTool::setObjectType
    virtual StatusCode
    setObjectType (ObjectType type, const std::string& workingPoint) override;

    /// returns: the processing step we are working on
    /// guarantee: no-fail
    virtual AnalysisStep step () const override;

    /// \copydoc IAnaTool::inputTypes
    virtual unsigned inputTypes () const override;

    /// \copydoc IAnaTool::outputTypes
    virtual unsigned outputTypes () const override;

    /// \copydoc IAnaTool::useInitialConfiguration
    virtual StatusCode
    useInitialConfiguration (const InternalConfiguration& conf) override;

    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
    StatusCode initialize() override;

    /// \copydoc IAnaTool::useConfiguration
    virtual StatusCode
    useConfiguration (const InternalConfiguration& configuration) override;

    /// effects: apply this analysis tool to the objects
    /// guarantee: basic
    /// failures: tool dependent
    virtual StatusCode execute (IEventObjects& objects) override;

  private:

    /// Configurations -- through InternalConfiguration

    /// \brief whether we run on data
    bool m_isData = false;
    /// \brief whether we are using AF2
    bool m_isAF2 = false;
    /// \brief the name of the jet input container
    std::string m_jetContainer;

    /// \brief the object type we work on
  private:
    ObjectType m_type = OBJECT_MET;

    /// Configurations -- tool properties

    bool m_doPFlow;
    bool m_includeTauTerm;
    bool m_doTST;
    bool m_doJVTCut;
    bool m_doTrackMET;
    bool m_doORCaloTaggedMuons;
    bool m_doMuJetEMScale;
    std::string m_jetSelection;
    double m_uniqueFrac;
    double m_jetCut;
    bool m_doFJVT;
    bool m_doPUmetsig;

  private:

    /// description: the MET tool
    asg::AnaToolHandle<IMETMaker> m_metutil;
    asg::AnaToolHandle<IMETSystematicsTool> m_metSystTool;
    asg::AnaToolHandle<IMETSignificance> m_metSigni;
    /// description: the fJVT tool
    asg::AnaToolHandle<IJetModifier> m_fjvtTool;

    /// \brief the accessor to the selection
    SG::AuxElement::Accessor<SelectType> m_accessor;
  };

}

#endif
