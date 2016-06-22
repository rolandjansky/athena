/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_PILEUPUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_PILEUPUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class PileupUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor
        PileupUncertaintyComponent( const ComponentHelper& component,
                                    const float refNPV,
                                    const float refMu
                                    );
        PileupUncertaintyComponent( const ComponentHelper& component,
                                    const UncertaintyHistogram* refNPV,
                                    const UncertaintyHistogram* refMu
                                    );
        PileupUncertaintyComponent( const ComponentHelper& component,
                                    const UncertaintyHistogram* refNPV,
                                    const float refMu
                                    );
        PileupUncertaintyComponent( const ComponentHelper& component,
                                    const float refNPV,
                                    const UncertaintyHistogram* refMu
                                    );
        PileupUncertaintyComponent(const PileupUncertaintyComponent& toCopy);
        virtual PileupUncertaintyComponent* clone() const;
        virtual ~PileupUncertaintyComponent();
        virtual StatusCode initialize(TFile* histFile);
        
        // Extra information retrieval methods
        virtual PileupComp::TypeEnum    getPileupType() const   { return m_pileupType; }
        virtual float                   getNPVRef()     const   { return m_refNPV;     }
        virtual float                   getMuRef()      const   { return m_refMu;      }

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        // Default constructor is forbidden
        PileupUncertaintyComponent(const std::string& name = "");

        enum PileupRefType { PileupRef_UNKNOWN, PileupRef_NONE, PileupRef_NPV, PileupRef_MU };
        
        // Additional private members
        const PileupComp::TypeEnum m_pileupType;
        const float m_refNPV;
        const float m_refMu;
        const UncertaintyHistogram* m_refNPVHist;
        const UncertaintyHistogram* m_refMuHist;
        const bool m_absEta;
        const TString m_secondUncName;
        
        UncertaintyHistogram* m_secondUncHist;
        PileupRefType m_refType;
        PileupRefType m_secondRefType;

        // Helper indices/methods
        double getPileupWeight(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const PileupRefType refType) const;
};

} // end jet namespace

#endif
