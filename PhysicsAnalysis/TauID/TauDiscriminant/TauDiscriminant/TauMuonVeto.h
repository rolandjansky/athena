/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUMUONVETO_H
#define TAUMUONVETO_H

#include "TauDiscriminant/TauDiscriToolBase.h"
#include "TauDiscriminant/TauDetailsManager.h"
#include <string>

class TauMuonVeto: public TauDiscriToolBase
{
    public:

        //-----------------------------------------------------------------
        // Contructor and destructor
        //-----------------------------------------------------------------
        TauMuonVeto(const string& type,
                const string& name,
                const IInterface* parent):
            TauDiscriToolBase(type, name, parent),
            detailsManager(0)
        {
            declareInterface<TauDiscriToolBase>(this);
        }

        virtual ~TauMuonVeto() {}

        //-----------------------------------------------------------------
        // Gaudi algorithm hooks
        //-----------------------------------------------------------------
        virtual StatusCode prepare(const TauDetailsManager&);
        
        virtual StatusCode execute(xAOD::TauJet*, FakeTauBits*, FakeTauScores*);
        
        virtual StatusCode finalize();

    private:

        const TauDetailsManager*  detailsManager;
};

#endif // TAUMUONVETO_H
