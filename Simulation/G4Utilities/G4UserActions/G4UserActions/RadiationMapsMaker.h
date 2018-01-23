/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_RadiationMapsMaker_H
#define G4UserActions_RadiationMapsMaker_H

#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserSteppingAction.hh"

class TH1D;
class TH2D;
class TH3D;
class TGraph;
class TFile;

namespace G4UA
{
  
  class RadiationMapsMaker: 
    public G4UserRunAction,
    public G4UserEventAction,
    public G4UserSteppingAction
    {
    
    public:
      RadiationMapsMaker();
      virtual void BeginOfRunAction(const G4Run*) override;
      virtual void EndOfRunAction(const G4Run*) override;
      virtual void EndOfEventAction(const G4Event*) override;
      virtual void UserSteppingAction(const G4Step*) override;
    private:
      TFile * m_f;

      TH2D * m_rz_tid;
      TH2D * m_rz_eion;
      TH2D * m_rz_niel;
      TH2D * m_rz_h20;
      TH2D * m_full_rz_tid;
      TH2D * m_full_rz_eion;
      TH2D * m_full_rz_niel;
      TH2D * m_full_rz_h20;
      TH3D * m_3d_tid;
      TH3D * m_3d_eion;
      TH3D * m_3d_niel;
      TH3D * m_3d_h20;
      TGraph * m_tgpSiA;
      TGraph * m_tgpSiB;
      TGraph * m_tgnSiA;
      TGraph * m_tgnSiB;
      TGraph * m_tgpiSi;
      
      double m_nev;

    }; // class RadiationMapsMaker
  
  
} // namespace G4UA 

#endif
