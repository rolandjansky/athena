/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUCUTSELEVETO_H
#define TAUCUTSELEVETO_H

#include "TauDiscriminant/TauDiscriToolBase.h"
#include "TauDiscriminant/TauDetailsManager.h"
#include <string>

class TauCutsEleVeto: public TauDiscriToolBase
{
    public:

        //-----------------------------------------------------------------
        // Contructor and destructor
        //-----------------------------------------------------------------
        TauCutsEleVeto(const string& type,
                const string& name,
                const IInterface* parent):
            TauDiscriToolBase(type, name, parent),
            detailsManager(0),
            useLCscale(false)
        {
            declareInterface<TauDiscriToolBase>(this);

            declareProperty("useLCscale", this->useLCscale);
        }

        virtual ~TauCutsEleVeto() {}

        //-----------------------------------------------------------------
        // Gaudi algorithm hooks
        //-----------------------------------------------------------------
        virtual StatusCode prepare(const TauDetailsManager&);
        
        virtual StatusCode execute(xAOD::TauJet*, FakeTauBits*, FakeTauScores*);
        
        virtual StatusCode finalize();

    private:

        const TauDetailsManager*  detailsManager;
        bool useLCscale;
};

#endif // TAUCUTSELEVETO_H
