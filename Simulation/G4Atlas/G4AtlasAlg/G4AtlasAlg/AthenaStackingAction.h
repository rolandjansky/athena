/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaStackingAction_H
#define AthenaStackingAction_H

#include "FadsActions/ApplicationStackingAction.h"

class AthenaStackingAction:public FADS::ApplicationStackingAction {
public:
        AthenaStackingAction();
        ~AthenaStackingAction(){}

        G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
        void NewStage();
        void PrepareNewEvent();

        inline void KillAllNeutrinos(const bool a){ p_killAllNeutrinos=a; }
        inline bool KillAllNeutrinos() const { return p_killAllNeutrinos; }

        inline void KillLowEPhotons(const double a){ p_stackEnergyCut=a; }
        inline double KillLowEPhotons() const { return p_stackEnergyCut; }

private:

	bool p_killAllNeutrinos;
	double p_stackEnergyCut;
};

#endif
