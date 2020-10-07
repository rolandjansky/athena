/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_CORRELATIONMATRIX_H
#define JETUNCERTAINTIES_CORRELATIONMATRIX_H

#include "AsgMessaging/AsgMessaging.h"
#include "JetUncertainties/JetUncertaintiesTool.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfoContainer.h"
#include "xAODEventInfo/EventInfoAuxContainer.h"

#include "TString.h"

class TH2D;

namespace jet
{

class CorrelationMatrix : public asg::AsgMessaging
{
    public:
        // Constructor/destructor
        CorrelationMatrix(  const TString name, 
                            const int numBins, 
                            const double minVal, 
                            const double maxVal, 
                            const double fixedVal1,
                            const double fixedVal2 );
        virtual ~CorrelationMatrix();

        // Initialization
        virtual StatusCode initializeForPt(const JetUncertaintiesTool& uncTool);
        virtual StatusCode initializeForEta(const JetUncertaintiesTool& uncTool);

        // Information retrieval methods
        virtual TString getName()   const { return m_name; }
        virtual TH2D*   getMatrix() const { return m_corrMat; }


    private:
        CorrelationMatrix(const std::string& name = "");
        
        // Private members
        bool m_isInit;
        const TString m_name;

        const int    m_numBins;
        const double m_minVal;
        const double m_maxVal;
        const double m_fixedVal1;
        const double m_fixedVal2;
        
        TH2D*  m_corrMat;

        // Required xAOD variables for manipulation of kinematics to fill the matrix
        xAOD::JetContainer* m_jets;
        xAOD::EventInfoContainer* m_eInfos;

        xAOD::Jet* m_jet1;
        xAOD::Jet* m_jet2;
        xAOD::EventInfo* m_eInfo;

        // Private helper methods
        StatusCode checkInitialization(const JetUncertaintiesTool& uncTool) const;
        StatusCode createStore();
        StatusCode clearStore();
        StatusCode setDefaultProperties(const JetUncertaintiesTool& uncTool);
        TH2D* buildMatrix(const std::vector<double>& bins) const;
        double getCorrelation(const JetUncertaintiesTool& uncTool) const;
        double getCovariance(const JetUncertaintiesTool& uncTool, const xAOD::Jet* jet1, const xAOD::Jet* jet2) const;
};

} // end jet namespace

#endif
