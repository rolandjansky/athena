/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4ios.hh"
#include "Quirk.h"
#include "InfracolorForce.h"
#include "DebugSteppingAction.h"

#ifndef QUIRKS_STANDALONE
namespace G4UA{
#endif
  
#ifdef QUIRKS_STANDALONE
  DebugSteppingAction::DebugSteppingAction(G4double step, G4int numSteps) :
    G4UserSteppingAction(),
    m_step(step), m_numSteps(numSteps)
  {
    
#else
    
DebugSteppingAction::DebugSteppingAction(const Config& config):m_config(config)
  {
#endif 
    m_iStep[0] = 0;
    m_iStep[1] = 0;
  }
 
 DebugSteppingAction::~DebugSteppingAction() {}
 
 void DebugSteppingAction::UserSteppingAction(const G4Step* step) {
#ifndef QUIRKS_STANDALONE
   G4double m_step=m_config.step;
   G4int m_numSteps=m_config.numSteps;
#endif

   const G4Track* track = step->GetTrack();
   const G4StepPoint* ps = step->GetPostStepPoint();
   const G4ParticleDefinition* particle = track->GetParticleDefinition();
   const Quirk* quirk = dynamic_cast<const Quirk*>(particle);
   if (quirk == 0) return;
   
   G4int i = (particle->GetPDGEncoding() > 0) ? 0 : 1;
   m_xprev[i] = m_x[i];
   m_x[i] = G4LorentzVector(ps->GetPosition(), CLHEP::c_light * ps->GetGlobalTime());
   m_p[i] = track->GetDynamicParticle()->Get4Momentum();
   
   G4bool doPrint = (m_numSteps != 0 && track->GetCurrentStepNumber() % m_numSteps == 0);
   if (track->GetCurrentStepNumber() == 1) m_iStep[i] = 0;
   G4double progress = m_x[i].rho();
   if (progress >= m_iStep[i] * m_step) {
     m_iStep[i]++;
     doPrint = true;
   }
   
   if (doPrint) {
     InfracolorForce* string[2];
     string[i] = &(quirk->GetStringIn());
     string[1-i] = string[i]->GetReactionForce();
     
     G4cout.precision(14);
     G4cout << "quirk " << i << " step " << track->GetCurrentStepNumber() << " status " << track->GetTrackStatus() << G4endl;
     G4cout << "x0: " << m_x[0] << G4endl;
     G4cout << "x1: " << m_x[1] << G4endl;
     //G4cout << "x0':" << m_xprev[0] << G4endl;
     //G4cout << "x1':" << m_xprev[1] << G4endl;
     G4cout << "dx: " << m_x[1] - m_x[0] << G4endl;
     
     G4double force = string[0]->GetStringForce();
     G4LorentzVector ss[2];
     for(G4int i = 0; i < 2; i++) ss[i] = string[i]->GetSumStrings();
     G4LorentzVector dx = ss[0] - ss[1];
     G4LorentzVector ps = force * (ss[0] + ss[1]);
     G4LorentzVector ptot = ps + m_p[0] + m_p[1];
     G4cout << "dx: " << dx << G4endl;
     G4cout << "s0: " << ss[0] << G4endl;
     G4cout << "s1: " << ss[1] << G4endl;
     G4cout << "p0: " << m_p[0] << G4endl;
     G4cout << "p1: " << m_p[1] << G4endl;
     G4cout << "ps: " << ps << G4endl;
     G4cout << "p:  " << ptot << G4endl;
     
     G4LorentzVector p1s = m_p[1] + force * ss[1];
     G4ThreeVector L = dx.vect().cross(p1s);
     G4ThreeVector Excm = p1s.t() * dx.vect() - p1s.vect() * dx.t();
     for(G4int i = 0; i < 2; i++) {
       L += string[i]->GetAngMomentum();
       Excm += string[i]->GetMomentOfE();
     }
     L -= Excm.cross(ptot.vect() / ptot.t());
     Excm += ptot.t() * m_x[0].vect() - ptot.vect() * m_x[0].t();
     G4cout << "L:  " << L << G4endl;
     G4cout << "cm: " << Excm / ptot.t() << G4endl;
     
     G4LorentzVector u = ptot / ptot.m();
     G4LorentzVector L4(L * u.t(), L * u.vect());
     G4double p0 = sqrt(sqr(ptot.m()/2) - sqr(quirk->GetPDGMass()));
     G4cout << "p0: " << p0 << G4endl;
     G4cout << "min: " << - L4.m() / p0 << G4endl;
     G4cout << "max: " << (ptot.m() - 2*quirk->GetPDGMass()) / force << G4endl;
     
     G4cout << "n:  " << string[0]->GetNStrings() << "\t" << string[1]->GetNStrings() << "\t" << string[0]->GetNStrings() + string[1]->GetNStrings() << G4endl;
     G4cout << G4endl;
   }
   
   if (false) {
     G4double t1A = m_xprev[i].t();
     G4double t1B = m_xprev[1-i].t();
     G4double t2A = m_x[i].t();
     G4double t2B = m_x[1-i].t();
     G4double dtA = t2A - t1A;
     G4double dtB = t2B - t1B;
     if (t2B > t1A && t2A > t1B && dtA > 0 && dtB > 0) {
       G4ThreeVector vA = (m_x[i].vect() - m_xprev[i].vect()) / dtA;
       G4ThreeVector vB = (m_x[1-i].vect() - m_xprev[1-i].vect()) / dtB;
       G4ThreeVector xA = m_xprev[i].vect();
       G4ThreeVector xB = m_xprev[1-i].vect() + vB * (t1A - t1B);
       G4double dvsq = (vB - vA).mag2();
       if (dvsq > 0) {
	 G4double dt = - (vB - vA) * (xB - xA) / dvsq;
	 G4double t = t1A + dt;
	 if (t1A <= t && t <= t2A && t1B <= t && t <= t2B) {
	   G4double dist = ((xB - xA) + (vB - vA) * dt).mag();
	   G4cout << "mindist: " << dist << " " << t << G4endl;
	   G4cout << G4endl;
	 }
       }
     }
   }
 }
 
 
 
#ifndef QUIRKS_STANDALONE
  }//namespace G4UA
#endif
