/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_CSCDATAPREPARATOR_H
#define  TRIGL2MUONSA_CSCDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "CscData.h"
#include "MuonRoad.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonSA {
  
  class CscDataPreparator: public AthAlgTool
  {
  public:

    CscDataPreparator(const std::string& type,
		      const std::string& name,
		      const IInterface*  parent);

    virtual StatusCode initialize() override;
    
  public:

    StatusCode prepareData(TrigL2MuonSA::MuonRoad&  muonRoad,
			   TrigL2MuonSA::CscHits&   cscHits) const;

    void setRoIBasedDataAccess(bool use_RoIBasedDataAccess) {m_use_RoIBasedDataAccess = use_RoIBasedDataAccess;};

  private:

    double calc_residual(double aw,
			 double bw,
			 double x,
			 double y) const;

    double calc_residual_phi(double aw, double bw, double phiw, double hitphi, double hitz) const;

  private:

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    //If we don't do the decoding in the algorithm, we need to read in the cluster container
    SG::ReadHandleKey<Muon::CscPrepDataContainer> m_cscPrepContainerKey{ this, "CSCPrepDataContainer", "CSC_Clusters", "Name of the CSCContainer to read in"};

    bool m_use_RoIBasedDataAccess;
  };

} // namespace TrigL2MuonSA

#endif
