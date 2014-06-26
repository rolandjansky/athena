/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**                                       
 * file: TauLLH.h
 *                                        
 * Author: Martin Flechl (mflechl@cern.ch)
 */

#ifndef TAULLHRERUN_H
#define TAULLHRERUN_H

#include "TauDiscriminant/TauDiscriToolBase.h"
#include "TauDiscriminant/MethodLLH.h"
#include "TauDiscriminant/TauDetailsManager.h"
#include <string>

using namespace TauID;

class TauLLH: public TauDiscriToolBase
{
    public:

        //-----------------------------------------------------------------
        // Contructor and destructor
        //-----------------------------------------------------------------
        TauLLH(const string& type,
               const string& name,
               const IInterface* parent):
            TauDiscriToolBase(type, name, parent)
        {
            declareInterface<TauDiscriToolBase>(this);
            declareProperty( "FileNameTauPDF", m_fileNameTauPDF = "pdfs_tau.root" );
            declareProperty( "FileNameJetPDF", m_fileNameJetPDF = "pdfs_jet.root");
            declareProperty( "FileNameLMTCuts", m_fileNameLMTCuts = "LMTCutsLLH.root");
        }    
        
        virtual ~TauLLH() {}

        //-----------------------------------------------------------------
        // Gaudi algorithm hooks
        //-----------------------------------------------------------------
        virtual StatusCode prepare(const TauDetailsManager&);
        
        virtual StatusCode execute(xAOD::TauJet*, FakeTauBits*, FakeTauScores*);
        
        virtual StatusCode finalize();

    private:
        
        std::string m_fileNameTauPDF;
        std::string m_fileNameJetPDF;
        std::string m_fileNameLMTCuts;

        MethodLLH* m_defllh; 
        MethodLLH* m_safellh; 
};

#endif // TAULLHRERUN_H
