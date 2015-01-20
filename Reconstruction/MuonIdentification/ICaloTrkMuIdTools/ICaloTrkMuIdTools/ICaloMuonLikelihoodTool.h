/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRKMUIDTOOLS_ICALOMUONLIKELIHOODTOOL_H
#define CALOTRKMUIDTOOLS_ICALOMUONLIKELIHOODTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

static const InterfaceID IID_ICaloMuonLikelihoodTool("ICaloMuonLikelihoodTool",1,0);

class ICaloMuonLikelihoodTool : virtual public IAlgTool
{
 public:

  virtual ~ICaloMuonLikelihoodTool(){}

  static const InterfaceID& interfaceID(){return IID_ICaloMuonLikelihoodTool;}
  
  virtual double getLHR(const Trk::TrackParameters* trkpar, 
                        const xAOD::CaloClusterContainer* ClusCollection = nullptr,
                        const double dR_CUT=0.3) const = 0;

  virtual double getLHR(const xAOD::CaloClusterContainer* ClusCollection, 
			const double eta_trk, 
			const double p_trk, 
			const double eta_trkAtCalo, 
			const double phi_trkAtCalo, 
			const double dR_CUT=0.3) const = 0;
  
};

#endif
