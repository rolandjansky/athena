/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRKMUIDTOOLS_ICALOMUONTAG_H
#define CALOTRKMUIDTOOLS_ICALOMUONTAG_H

#include "GaudiKernel/IAlgTool.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "muonEvent/DepositInCalo.h"
#include "TrkParameters/TrackParameters.h"

static const InterfaceID IID_ICaloMuonTag("ICaloMuonTag", 1, 0);

class ICaloMuonTag : virtual public IAlgTool{
  /** @class ICaloMuonTag
      @author  Gustavo.Ordonez.Sanz <Gustavo.Ordonez.Sanz@cern.ch>
  */ 

 public:
  virtual ~ICaloMuonTag(){}

  static const InterfaceID& interfaceID(){return IID_ICaloMuonTag;}


  virtual int caloMuonTag(const std::vector<DepositInCalo>& deposits, double eta, double pt) const = 0;

};

 
#endif
