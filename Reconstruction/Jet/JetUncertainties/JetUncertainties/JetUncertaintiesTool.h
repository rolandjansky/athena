/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_JETUNCERTAINTIESTOOL_H
#define JETUNCERTAINTIES_JETUNCERTAINTIESTOOL_H

#include "JetCPInterfaces/ICPJetUncertaintiesTool.h"
#include "AsgTools/AsgTool.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace jet
{
    class UncertaintyHistogram;
    class UncertaintyComponent;
    class UncertaintyComponentGroup;
    class UncertaintySet;
    class ComponentHelper;
}

class TFile;
class TH2D;

namespace xAOD
{
    class ShallowAuxContainer;
}

class JetUncertaintiesTool :    virtual public ICPJetUncertaintiesTool,
                                virtual public asg::AsgTool
{
    ASG_TOOL_CLASS(JetUncertaintiesTool,ICPJetUncertaintiesTool)

    public:
        // Constructor/destructor
        JetUncertaintiesTool(const std::string& name = "JetUncertaintiesTool");
        JetUncertaintiesTool(const JetUncertaintiesTool& toCopy);
        virtual ~JetUncertaintiesTool();

        // Pre-initialization methods
        // Default scale is MeV per ATLAS policy
        virtual StatusCode setScaleToMeV();
        virtual StatusCode setScaleToGeV();
        
        // Initialization method
        virtual StatusCode initialize();

        
        // Tool information retrieval methods
        virtual std::string getName()         const { return m_name;         }
        virtual std::string getRelease()      const { return m_release;      }
        virtual std::string getJetDef()       const { return m_jetDef;       }
        virtual std::string getMCType()       const { return m_mcType;       }
        virtual std::string getConfigFile()   const { return m_configFile;   }
        virtual std::string getPath()         const { return m_path;         }
        virtual std::string getAnalysisFile() const { return m_analysisFile; }
        virtual float       getSqrtS()        const;

        // Tool information retrieval methods that require input
        virtual float getRefMu()  const;
        virtual float getRefNPV() const;
        virtual float getRefMu(const xAOD::Jet& jet)  const;
        virtual float getRefNPV(const xAOD::Jet& jet) const;

        // Inherited methods from IJetUncertaintiesTool to implement
        // Retrieve component information
        virtual size_t      getNumComponents() const;
        virtual size_t      getComponentIndex(const std::string& name) const;
        virtual size_t      getComponentIndex(const TString& name)     const;
        virtual std::string getComponentName(const size_t index)       const;
        virtual std::string getComponentDesc(const size_t index)       const;
        virtual std::string getComponentCategory(const size_t index)   const;
        virtual bool        getComponentIsReducible(const size_t index) const;
        // Retrieve component scaling information
        virtual bool        getComponentScalesFourVec(const size_t index) const;
        virtual bool        getComponentScalesPt(const size_t index)      const;
        virtual bool        getComponentScalesMass(const size_t index)    const;
        virtual bool        getComponentScalesD12(const size_t index)     const;
        virtual bool        getComponentScalesD23(const size_t index)     const;
        virtual bool        getComponentScalesTau21(const size_t index)   const;
        virtual bool        getComponentScalesTau32(const size_t index)   const;
        // Retrieve multi-component information
        virtual std::vector<std::string> getComponentCategories() const;
        virtual std::vector<size_t>      getComponentsInCategory(const std::string& category) const;
        virtual std::vector<std::string> getComponentNamesInCategory(const std::string& category) const;

        // Inherited methods from IJetUncertaintiesTool to implement
        // Retrieve uncertainty and validity information for a given component
        virtual bool   getValidity(size_t index, const xAOD::Jet& jet) const;
        virtual bool   getValidity(size_t index, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual double getUncertainty(size_t index, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual double getUncertainty(size_t index, const xAOD::Jet& jet) const;
        virtual bool   getValidUncertainty(size_t index, double& unc, const xAOD::Jet& jet) const;
        virtual bool   getValidUncertainty(size_t index, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

        // Inherited methods from CP::IJetUncertaintiesTool to implement
        // Apply a systematic variation or get a new copy
        virtual CP::CorrectionCode applyCorrection(xAOD::Jet& jet) const;
        virtual CP::CorrectionCode applyCorrection(xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual CP::CorrectionCode correctedCopy(const xAOD::Jet& input, xAOD::Jet*& output) const;
        virtual CP::CorrectionCode correctedCopy(const xAOD::Jet& input, xAOD::Jet*& output, const xAOD::EventInfo& eInfo) const;
        virtual CP::CorrectionCode applyContainerCorrection(xAOD::JetContainer& inputs) const;
        virtual CP::CorrectionCode applyContainerCorrection(xAOD::JetContainer& inputs, const xAOD::EventInfo& eInfo) const;

    
        // Inherited methods from CP::ISystematicsTool to implement
        // Changes internal state to the specified SystematicSet
        virtual bool isAffectedBySystematic(const CP::SystematicVariation& systematic) const;
        virtual CP::SystematicSet affectingSystematics() const;
        virtual CP::SystematicSet recommendedSystematics() const;
        virtual CP::SystematicSet appliedSystematics() const;
        virtual CP::SystematicCode applySystematicVariation(const CP::SystematicSet& systConfig);

        
        // Retrieve the correlation matrix for the loaded configuration
        // Returns NULL on failure
        virtual TH2D* getPtCorrelationMatrix(   const int numBins,
                                                const double minPt,
                                                const double maxPt,
                                                const double valEta );
        virtual TH2D* getPtCorrelationMatrix(   const int numBins,
                                                const double minPt,
                                                const double maxPt,
                                                const double valEta1,
                                                const double valEta2);
        virtual TH2D* getEtaCorrelationMatrix(  const int numBins,
                                                const double minEta,
                                                const double maxEta,
                                                const double valPt  );
        virtual TH2D* getEtaCorrelationMatrix(  const int numBins,
                                                const double minEta,
                                                const double maxEta,
                                                const double valPt1,
                                                const double valPt2 );

    private:
        // Control variables
        bool m_isInit;
        const std::string m_name;
        float m_energyScale;

        // Properties for the python configuration
        std::string m_release;
        std::string m_jetDef;
        std::string m_mcType;
        std::string m_configFile;
        std::string m_path;
        std::string m_analysisFile;

        // Information to read in and store from the config file
        float m_refNPV;
        float m_refMu;
        jet::UncertaintyHistogram* m_refNPVHist;
        jet::UncertaintyHistogram* m_refMuHist;
        std::vector<jet::UncertaintyComponent*> m_components;
        std::vector<jet::UncertaintyComponentGroup*> m_groups;

        // Accessors
        SG::AuxElement::Accessor<float> m_NPVAccessor;
        SG::AuxElement::Accessor<float> m_D12Accessor;
        SG::AuxElement::Accessor<float> m_D23Accessor;
        SG::AuxElement::Accessor<float> m_Tau21Accessor;
        SG::AuxElement::Accessor<float> m_Tau32Accessor;
        SG::AuxElement::Accessor<float> m_Tau1Accessor;
        SG::AuxElement::Accessor<float> m_Tau2Accessor;
        SG::AuxElement::Accessor<float> m_Tau3Accessor;
        
        // The SystematicSet known sets, maps, and current state
        CP::SystematicSet    m_recognizedSystematics;
        CP::SystematicSet    m_recommendedSystematics;
        CP::SystematicSet    m_currentSystSet;
        jet::UncertaintySet* m_currentUncSet;
        std::unordered_map<CP::SystematicSet,CP::SystematicSet> m_systFilterMap;
        std::unordered_map<CP::SystematicSet,jet::UncertaintySet*> m_systSetMap;

        // Default prefix for each component name
        const std::string m_namePrefix;

        // Helper methods for this tool's functions
        StatusCode addUncertaintyComponent(TFile* histFile, const jet::ComponentHelper& component);
        const xAOD::EventInfo* getDefaultEventInfo() const;


        // Helper methods for CP::ISystematicsTool functions
        virtual CP::SystematicCode addAffectingSystematic(const CP::SystematicVariation& systematic, bool recommended);
        virtual CP::SystematicCode getFilteredSystematicSet(const CP::SystematicSet& systConfig, CP::SystematicSet& filteredSet);
        virtual CP::SystematicCode getUncertaintySet(const CP::SystematicSet& filteredSet, jet::UncertaintySet*& uncSet);
};
    

#endif
