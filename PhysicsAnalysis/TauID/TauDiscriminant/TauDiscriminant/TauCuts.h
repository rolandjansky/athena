/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUCUTS_H
#define TAUCUTS_H

#include "TauDiscriminant/TauDiscriToolBase.h"
#include "TauDiscriminant/MethodCuts.h"
#include "TauDiscriminant/TauDetailsManager.h"
#include <string>

using namespace TauID;

class TauCuts: public TauDiscriToolBase
{
    public:

        //-----------------------------------------------------------------
        // Contructor and destructor
        //-----------------------------------------------------------------
        TauCuts(const string& type,
                const string& name,
                const IInterface* parent):
            TauDiscriToolBase(type, name, parent)
        {
            declareInterface<TauDiscriToolBase>(this);
            declareProperty( "cuts", m_fileName);
        }

        virtual ~TauCuts() {}

        //-----------------------------------------------------------------
        // Gaudi algorithm hooks
        //-----------------------------------------------------------------
        virtual StatusCode prepare(const TauDetailsManager&);
        
        virtual StatusCode execute(xAOD::TauJet*, FakeTauBits*, FakeTauScores*);
        
        virtual StatusCode finalize();

    private:

        std::string         m_fileName;
        MethodCuts*         m_cutsManager;
};

#endif // TAUCUTS_H
