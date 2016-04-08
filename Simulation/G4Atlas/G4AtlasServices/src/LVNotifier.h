/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LVNotifier_H
#define LVNotifier_H

#include "G4VNotifier.hh"

class G4GeometryNotifierSvc;

  class LVNotifier: public G4VNotifier {
    friend class G4GeometryNotifierSvc;
  private:
    LVNotifier(G4GeometryNotifierSvc *);

    G4GeometryNotifierSvc* m_notifierSvc;
  public:
    void NotifyRegistration();
    void NotifyDeRegistration();
  };

#endif
