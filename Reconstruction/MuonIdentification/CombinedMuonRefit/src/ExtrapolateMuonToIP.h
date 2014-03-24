/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolateMuonToIP.h, (c) ATLAS Detector Softwareop 
///////////////////////////////////////////////////////////////////

#ifndef COMBINEDMUONREFIT_EXTRAPOLATEMUONTOIP_H
#define COMBINEDMUONREFIT_EXTRAPOLATEMUONTOIP_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace Muon{
  class MuonEDMPrinterTool;
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

  ToolHandle<Muon::IMuonTrackExtrapolationTool>   m_backExtrapolator; //!< back extrapolator tool
  ToolHandle<Muon::MuonEDMPrinterTool>   m_printer;          //!< printer tool
}; 

 

#endif //COMBINEDMUONREFIT_EXTRAPOLATEMUONTOIP_H

