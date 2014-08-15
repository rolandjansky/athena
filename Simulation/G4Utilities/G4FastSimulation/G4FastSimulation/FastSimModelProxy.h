/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FastSimModelProxy_H
#define FastSimModelProxy_H

#include "G4FastSimulation/FastSimModelCatalog.h"

#include <iostream>

template <class T> class FastSimModelProxy: public FastSimModel
{
 public:
  FastSimModelProxy(const G4String& aName): FastSimModel(aName) 
  {
    Register();
    fastSimModel = new T(aName);
  }

  void Register()
  {
    FastSimModelCatalog* catalog = FastSimModelCatalog::GetInstance();
    catalog->RegisterModel(this);
  }

  void CreateFastSimModel() {fastSimModel=new T("");}

  G4bool IsApplicable(const G4ParticleDefinition& partDef)
  {
    if(!fastSimModel) 
      CreateFastSimModel();
    return fastSimModel->IsApplicable(partDef);
  }

  G4bool ModelTrigger(const G4FastTrack& fastTrack)
  {
    if(!fastSimModel) 
      CreateFastSimModel();
    return fastSimModel->ModelTrigger(fastTrack);
  }

  void DoIt(const G4FastTrack& fastTrack, G4FastStep& fastStep)
  {
    if(!fastSimModel) 
      CreateFastSimModel();
    fastSimModel->DoIt(fastTrack,fastStep);
  }

  G4bool AtRestModelTrigger(const G4FastTrack& fastTrack)
  {
    if(!fastSimModel) 
      CreateFastSimModel();
    return fastSimModel->AtRestModelTrigger(fastTrack);
  }

  void   AtRestDoIt  (const G4FastTrack& fastTrack, G4FastStep& fastStep)
  {
    if(!fastSimModel) 
      CreateFastSimModel();
    fastSimModel->AtRestDoIt(fastTrack, fastStep);
  }

 private:
  T* fastSimModel;
};

#endif
