/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ScoringPlane_H
#define ScoringPlane_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include <string>

class TTree;

class ScoringPlane: public FADS::ActionsBase , public FADS::UserAction {

  public:

   ScoringPlane(std::string s): FADS::ActionsBase(s),FADS::UserAction(s),m_tree0(0),m_tree1(0),m_evt(0),m_ntr(0),
    m_pdg(0),m_cha(0),m_ene(0),m_vx(0),m_vy(0),m_vz(0),m_x0(0),m_y0(0),m_z0(0),m_t0(0),
    m_px0(0),m_py0(0),m_pz0(0),m_x1(0),m_y1(0),m_z1(0),m_t1(0),m_px1(0),m_py1(0),
    m_pz1(0),m_x(0),m_y(0),m_z(0),m_plane(0),m_pkill(0) {}
 
   void BeginOfEventAction(const G4Event*);
   void EndOfEventAction  (const G4Event*);
   void BeginOfRunAction  (const G4Run*);
   void EndOfRunAction    (const G4Run*);
   void SteppingAction    (const G4Step*);

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
