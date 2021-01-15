/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTSIM_HTTOFFLINETRACK_H
#define TRIGHTTSIM_HTTOFFLINETRACK_H

#include "HTTHit.h"
#include <vector>
#include <TObject.h>
#include <TMath.h>

#include "TrigHTTObjects/HTTOfflineHit.h"

class HTTOfflineTrack : public TObject {
 public:
   HTTOfflineTrack()  { } 
   HTTOfflineTrack(double eta, double phi, double pt) {m_pt=pt; m_eta=eta;m_phi=phi;};

   double setPt(double pt) { return m_pt = pt; }
   double setEta(double e) { return m_eta = e; } 
   double setPhi(double p) { return m_phi = p; }

   double eta() const { return m_eta; }
   double pt() const  { return m_pt; }
   double phi() const { return m_phi; }

     //  handling hits
  const std::vector<HTTOfflineHit>& hits()  const { return m_hits; }   
  int  nHits() const { return m_hits.size(); } 
  void addHit(HTTOfflineHit s) { m_hits.push_back(s); }

 
 private:
  double m_pt;
  double m_eta;
  double m_phi;
  std::vector<HTTOfflineHit> m_hits;
 
  
  ClassDef(HTTOfflineTrack,1)
};

std::ostream& operator<<(std::ostream& , const HTTOfflineTrack& );
#endif // TRIGHTTSIM_HTTOFFLINETRACK_H
