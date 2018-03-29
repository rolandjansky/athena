/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LVNotifier_H
#define LVNotifier_H

#include "G4VNotifier.hh"

class G4GeometryNotifierSvc;

/// @class LVNotifier
/// @todo NEEDS DOCUMENTATION
class LVNotifier : public G4VNotifier
{
  friend class G4GeometryNotifierSvc;

  public:

    void NotifyRegistration();
    void NotifyDeRegistration();

  private:

    LVNotifier(G4GeometryNotifierSvc*);

    G4GeometryNotifierSvc* m_notifierSvc;
};

#endif
