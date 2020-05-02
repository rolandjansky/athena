/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef MET_ANALYSIS_ALGORITHMS__MET_BUILDER_ALG_H
#define MET_ANALYSIS_ALGORITHMS__MET_BUILDER_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <METInterface/IMETMaker.h>
#include <xAODMissingET/MissingETContainer.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IMetCalibrationAndSmearingTool

  class MetBuilderAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    MetBuilderAlg (const std::string& name, 
                 ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the met collection we run on
  private:
    SysCopyHandle<xAOD::MissingETContainer> m_metHandle {
      this, "met", "MissingET_%SYS%", "the met collection we run on"};

    /// \brief the key for the final met term
  private:
    std::string m_finalKey {"Final"};

    /// \brief the key for the soft term
  private:
    std::string m_softTerm {"PVSoftTrk"};
  };
}

#endif
