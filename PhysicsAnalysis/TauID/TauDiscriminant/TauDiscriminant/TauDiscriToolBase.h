/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauDiscriToolBase.h
// package:     PhysicsAnalysis/TauID/TauDiscriminant
// authors:     M. Wolter, A. Kaczmarska
// date:        13 March 2008
//-----------------------------------------------------------------------------

#ifndef DISCRITOOLBASE_TAU_H
#define DISCRITOOLBASE_TAU_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "TauDiscriminant/TauDetailsManager.h"
#include "TauDiscriminant/FakeTauBits.h"
#include "TauDiscriminant/FakeTauScores.h"

class TauDiscriToolBase: public AthAlgTool
{
    public:
    
        TauDiscriToolBase( const std::string& type,
                           const std::string& name,
                           const IInterface * parent):
            AthAlgTool(type, name, parent)
        {}

        virtual ~TauDiscriToolBase(){}

        //-----------------------------------------------------------------
        //! InterfaceID implementation needed for ToolHandle
        //-----------------------------------------------------------------
        static const InterfaceID& interfaceID()
        {
            static const InterfaceID TauDiscriToolBaseID( "TauDiscriToolBase", 1, 0 );
            return TauDiscriToolBaseID;
        }

        //-----------------------------------------------------------------
        //! Tool initializer
        //-----------------------------------------------------------------
        virtual StatusCode prepare(const TauDetailsManager&) = 0;
        
        //-----------------------------------------------------------------
        //! Execute - called for each tau candidate
        //-----------------------------------------------------------------
        virtual StatusCode execute(xAOD::TauJet*, FakeTauBits*, FakeTauScores*) = 0;

        //-----------------------------------------------------------------
        //! Finalizer
        //-----------------------------------------------------------------
        virtual StatusCode finalize() = 0;
};

#endif // DISCRITOOLBASE_TAU_H
