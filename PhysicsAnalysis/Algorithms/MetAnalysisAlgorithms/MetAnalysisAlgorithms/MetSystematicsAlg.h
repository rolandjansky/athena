/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef MET_ANALYSIS_ALGORITHMS__MET_SYSTEMATICS_ALG_H
#define MET_ANALYSIS_ALGORITHMS__MET_SYSTEMATICS_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <METInterface/IMETSystematicsTool.h>
#include <xAODMissingET/MissingETContainer.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IMetCalibrationAndSmearingTool

  class MetSystematicsAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    MetSystematicsAlg (const std::string& name, 
                 ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the smearing tool
  private:
    ToolHandle<IMETSystematicsTool> m_systematicsTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the met collection we run on
  private:
    SysCopyHandle<xAOD::MissingETContainer> m_metHandle {
      this, "met", "MissingET_%SYS%", "the met collection we run on"};

    /// \brief the key for the soft term
  private:
    std::string m_softTerm {"PVSoftTrk"};
  };
}

#endif
