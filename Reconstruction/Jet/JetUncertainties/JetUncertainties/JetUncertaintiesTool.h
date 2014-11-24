/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_JETUNCERTAINTIESTOOL_H
#define JETUNCERTAINTIES_JETUNCERTAINTIESTOOL_H

#include "JetCPInterfaces/ICPJetUncertaintiesTool.h"
#include "AsgTools/AsgTool.h"

#include <string>
#include <vector>
#include <boost/unordered_map.hpp>

namespace jet
{
    class UncertaintyComponent;
    class UncertaintySet;
    class ComponentHelper;
}
class TFile;

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

        // Initialization method
        virtual StatusCode initialize();


        virtual std::string getName()         const { return m_name;         }
        virtual std::string getJetDef()       const { return m_jetDef;       }
        virtual std::string getMCType()       const { return m_mcType;       }
        virtual std::string getConfigFile()   const { return m_configFile;   }
        virtual std::string getPath()         const { return m_path;         }
        virtual std::string getAnalysisFile() const { return m_analysisFile; }
        virtual float       getRefMu()        const { return m_refMu;        }
        virtual unsigned    getRefNPV()       const { return m_refNPV;       }

        // Inherited methods from IJetUncertaintiesTool to implement
        // Retrieve component information
        virtual size_t      getNumComponents() const;
        virtual size_t      getComponentIndex(const std::string& name) const;
        virtual size_t      getComponentIndex(const TString& name) const;
        virtual std::string getComponentName(const size_t index) const;
        virtual std::string getComponentDesc(const size_t index) const;
        // Retrieve component scaling information
        virtual bool        getComponentScalesFourVec(const size_t index) const;
        virtual bool        getComponentScalesPt(const size_t index) const;
        virtual bool        getComponentScalesMass(const size_t index) const;
        virtual bool        getComponentScalesD12(const size_t index) const;
        virtual bool        getComponentScalesD23(const size_t index) const;
        virtual bool        getComponentScalesTau21(const size_t index) const;
        virtual bool        getComponentScalesTau32(const size_t index) const;

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


    private:
        // Control variables
        bool m_isInit;
        const std::string m_name;

        // Properties for the python configuration
        std::string m_jetDef;
        std::string m_mcType;
        std::string m_configFile;
        std::string m_path;
        std::string m_analysisFile;

        // Information to read in and store from the config file
        float m_refNPV;
        float m_refMu;
        std::vector<jet::UncertaintyComponent*> m_components;

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
        boost::unordered_map<CP::SystematicSet,CP::SystematicSet> m_systFilterMap;
        boost::unordered_map<CP::SystematicSet,jet::UncertaintySet*> m_systSetMap;

        // Default prefix for each component name
        const std::string m_namePrefix;

        // Helper methods for this tool's functions
        StatusCode addUncertaintyComponent(TFile* histFile, const jet::ComponentHelper& component);
        std::pair<xAOD::EventInfo*,xAOD::ShallowAuxContainer*> getDefaultEventInfo() const;


        // Helper methods for CP::ISystematicsTool functions
        virtual CP::SystematicCode addAffectingSystematic(const CP::SystematicVariation& systematic, bool recommended);
        virtual CP::SystematicCode getFilteredSystematicSet(const CP::SystematicSet& systConfig, CP::SystematicSet& filteredSet);
        virtual CP::SystematicCode getUncertaintySet(const CP::SystematicSet& filteredSet, jet::UncertaintySet*& uncSet);
};
    

#endif
