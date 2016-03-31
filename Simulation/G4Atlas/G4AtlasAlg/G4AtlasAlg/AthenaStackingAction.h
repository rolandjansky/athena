/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaStackingAction_H
#define AthenaStackingAction_H

//#include "FadsActions/ApplicationStackingAction.h"
#include "G4AtlasTools/UserActionBase.h"

class AthenaStackingAction:public UserActionBase {
public:
        AthenaStackingAction(const std::string& type, const std::string& name, const IInterface* parent);
        ~AthenaStackingAction(){}

        virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack) override;

	virtual StatusCode queryInterface(const InterfaceID&, void**) override;


        inline void KillAllNeutrinos(const bool a){ p_killAllNeutrinos=a; }
        inline bool KillAllNeutrinos() const { return p_killAllNeutrinos; }

        inline void KillLowEPhotons(const double a){ p_stackEnergyCut=a; }
        inline double KillLowEPhotons() const { return p_stackEnergyCut; }

private:

	bool p_killAllNeutrinos;
	double p_stackEnergyCut;
};

#endif
