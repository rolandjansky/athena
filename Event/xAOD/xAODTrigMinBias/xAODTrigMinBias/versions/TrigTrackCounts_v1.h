/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigTrackCounts_V1_H
#define xAODTrigMinBias_TrigTrackCounts_V1_H

#include "AthContainers/AuxElement.h"

namespace xAOD {

  class TrigTrackCounts_v1 : public SG::AuxElement {
  public:
    TrigTrackCounts_v1();

  /** Destructor */
  ~TrigTrackCounts_v1();

  /*!getters */
  const std::vector<float>& z0_pt() const;

  const std::vector<float>& eta_phi() const;

  unsigned int z0Bins() const;
  
  float z0Min() const;
  
  float z0Max() const;
  
  unsigned int ptBins() const;
  
  float ptMin() const;
  
  float ptMax() const;
  
  unsigned int etaBins() const;
  
  float etaMin() const;
  
  float etaMax() const;
  
  unsigned int phiBins() const;
  
  float phiMin() const;
  
  float phiMax() const;
  /*setters*/
  
  void setZ0_pt(const std::vector<float> &z0_pt);

  void setEta_phi(const std::vector<float> &eta_phi);

  void setZ0Bins(unsigned int);
  
  void setZ0Min(float);
  
  void setZ0Max(float);
  
  void setPtBins(unsigned int);
  
  void setPtMin(float);
  
  void setPtMax(float);
  
  void setEtaBins(unsigned int);
  
  void setEtaMin(float);
  
  void setEtaMax(float);
  
  void setPhiBins(unsigned int);
  
  void setPhiMin(float);
  
  void setPhiMax(float);
  
  double z0_ptSumEntries(float value_x, float value_y, int cutType) const;
  
  double eta_phiSumEntries(float value_x, float value_y, int cutType) const;
  };

}

#endif

