/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 
***************************************************************************/
#ifndef TRIGBPHYSICSEVENT_TRIGJPSI_H
#define TRIGBPHYSICSEVENT_TRIGJPSI_H


#include "CLHEP/Vector/LorentzVector.h"
#include "CLIDSvc/CLASS_DEF.h"

class CombinedMuonFeature;

class TrigJpsi
{

public:
    
    TrigJpsi();
    TrigJpsi(const CombinedMuonFeature* jpsimuon1, const CombinedMuonFeature* jpsimuon2 );
    ~TrigJpsi();
  
    double mass() const { return m_hlvJ.m(); }
    double eta() const  { return m_hlvJ.pseudoRapidity(); }
    double phi() const  { return m_hlvJ.phi(); }
    double et() const   { return m_hlvJ.perp(); }
    const CombinedMuonFeature* getDaughter(const int i) const ;
    void setDaughters(const CombinedMuonFeature* jpsimuon1, const CombinedMuonFeature* jpsimuon2);

private:

    CLHEP::HepLorentzVector m_hlvJ;   // Jpsi 4 momentum
    const CombinedMuonFeature* m_jpsimuon[2];  // pointers to muon objects
};

CLASS_DEF(TrigJpsi, 164633928, 1)

#endif
