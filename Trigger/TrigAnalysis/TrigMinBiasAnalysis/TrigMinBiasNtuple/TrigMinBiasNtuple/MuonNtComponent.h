/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_MUONNTCOMPONENT_H
#define TRIGMINBIASNTUPLE_MUONNTCOMPONENT_H

#include "GaudiKernel/ToolHandle.h"
#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

namespace NtComponent {
  class MuonNtComponent: public NtupleComponent {
  public:
    MuonNtComponent(NtupleAlgorithm *mainAlg,
		    NtComponentParameters parameters);
    virtual ~MuonNtComponent();
    
    StatusCode setupServices(void);
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_CaloMuonContName;
    std::string m_MuSpecMuonContName;
	bool useCaloMuon;
	bool useMuSpecMuon;

    // NTUPLE variables
    
	// TileMuonFitter (CaloMuons)
	int m_calo_ncell;
    std::vector<double>* m_calo_time;
    std::vector<double>* m_calo_theta;
    std::vector<double>* m_calo_phi;
    std::vector<double>* m_calo_x0;
    std::vector<double>* m_calo_y0;
    std::vector<double>* m_calo_z0;
    std::vector<double>* m_calo_quality;

	// MuonSpectrometer Combined Muons
	std::vector<double>* m_muspec_a0;
	std::vector<double>* m_muspec_x0;
	std::vector<double>* m_muspec_y0;
	std::vector<double>* m_muspec_z0;
	std::vector<double>* m_muspec_phi;
	std::vector<double>* m_muspec_theta;
	std::vector<double>* m_muspec_chi2;
	std::vector<double>* m_muspec_dof;
	
  };
}

#endif
