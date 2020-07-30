/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRKMUIDTOOLS_ICALOMUONSCORETOOL_H
#define CALOTRKMUIDTOOLS_ICALOMUONSCORETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

static const InterfaceID IID_ICaloMuonScoreTool("ICaloMuonScoreTool",1,0);

class ICaloMuonScoreTool : virtual public IAlgTool
{
 public:

  virtual ~ICaloMuonScoreTool(){}

  static const InterfaceID& interfaceID(){return IID_ICaloMuonScoreTool;}
  
  virtual double getMuonScore(const xAOD::TrackParticle* trk) const = 0;
  
};

#endif
