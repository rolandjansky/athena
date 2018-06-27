/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef MET_ANALYSIS_ALGORITHMS__MET_SIGNIFICANCE_ALG_H
#define MET_ANALYSIS_ALGORITHMS__MET_SIGNIFICANCE_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <SystematicsHandles/SysWriteHandle.h>
#include <METInterface/IMETSignificance.h>
#include <xAODMissingET/MissingETContainer.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IMETSignificanceTool

  class MetSignificanceAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    MetSignificanceAlg (const std::string& name, 
                 ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the smearing tool
  private:
    ToolHandle<IMETSignificance> m_significanceTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the met collection we run on
  private:
    SysCopyHandle<xAOD::MissingETContainer> m_metHandle {
      this, "met", "MissingET_%SYS%", "the met collection we run on"};

    /// \brief the key for the final met term
  private:
    std::string m_totalMETName {"Final"};

    /// \brief the key for the jets term
  private:
    std::string m_jetTermName {"RefJet"};

    /// \brief the key for the soft term
  private:
    std::string m_softTermName {"PVSoftTrk"};

    /// \brief the decoration for the significance
  private:
    std::string m_significanceDecoration {"significance"};

    /// \brief the accessor for \ref m_selectionDecoration
  private:
    std::unique_ptr<const SG::AuxElement::Accessor<float> > m_significanceAccessor;
  };
}

#endif
