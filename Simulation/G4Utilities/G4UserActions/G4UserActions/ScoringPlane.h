/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ScoringPlane_H
#define ScoringPlane_H

#include "G4AtlasTools/UserActionBase.h"


#include <string>

class TTree;

class ScoringPlane final: public UserActionBase {

  public:

  ScoringPlane(const std::string& type, const std::string& name, const IInterface* parent);
 
   virtual void BeginOfEvent(const G4Event*) override;
   virtual void EndOfEvent(const G4Event*) override;
   virtual void Step(const G4Step*) override;

   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;


   virtual StatusCode queryInterface(const InterfaceID&, void**) override;

   

  private:

   TTree* m_tree0;
   TTree* m_tree1;
 
   int    m_evt;
   int    m_ntr;
 
   int    m_pdg;
   double m_cha;
   double m_ene;
   double m_vx;
   double m_vy;
   double m_vz;
   double m_x0;
   double m_y0;
   double m_z0;
   double m_t0;
   double m_px0;
   double m_py0;
   double m_pz0;
   double m_x1;
   double m_y1;
   double m_z1;
   double m_t1;
   double m_px1;
   double m_py1;
   double m_pz1;
   double m_x;
   double m_y;
   double m_z;
   double      m_plane;
   int         m_pkill;
   std::string m_fname;

};

#endif
