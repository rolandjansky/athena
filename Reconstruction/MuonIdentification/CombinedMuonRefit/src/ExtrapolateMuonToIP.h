/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolateMuonToIP.h, (c) ATLAS Detector Softwareop
///////////////////////////////////////////////////////////////////

#ifndef COMBINEDMUONREFIT_EXTRAPOLATEMUONTOIP_H
#define COMBINEDMUONREFIT_EXTRAPOLATEMUONTOIP_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

namespace Muon{
  class IMuonTrackExtrapolationTool;
}


class ExtrapolateMuonToIP : public AthAlgorithm  {
 public:

  /** Constructor with parameters: */
  ExtrapolateMuonToIP(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  std::string m_muonTrackLocation;               //!< Name of the input Trackcollection
  std::string m_extrapolateMuonTrackLocation;    //!< Name of the output Trackcollection

  ToolHandle<Muon::IMuonTrackExtrapolationTool> m_backExtrapolator {
    this, "BackExtrapolator", "ExtrapolateMuonToIPTool/ExtrapolateMuonToIPTool", "back extrapolator tool",
  }; //!< back extrapolator tool
  ToolHandle<Muon::MuonEDMPrinterTool> m_printer {
    this, "Printer", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool", "printer tool",
  }; //!< printer tool
};



#endif //COMBINEDMUONREFIT_EXTRAPOLATEMUONTOIP_H

