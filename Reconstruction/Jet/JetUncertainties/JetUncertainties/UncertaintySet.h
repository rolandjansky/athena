/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_UNCERTAINTYSET_H
#define JETUNCERTAINTIES_UNCERTAINTYSET_H

#include "JetUncertainties/UncertaintyGroup.h"

#include "xAODJet/Jet.h"
#include "xAODEventInfo/EventInfo.h"

#include "PATInterfaces/SystematicSet.h"
#include "AsgMessaging/AsgMessaging.h"
#include "AsgTools/IAsgTool.h"

#include <string>
#include <vector>
#include <utility>

namespace jet
{

class UncertaintySet : virtual public asg::AsgMessaging
{
    ASG_TOOL_CLASS0(UncertaintySet)

    public:
        // Constructor/destructor/initialization
        UncertaintySet(const std::string& name = "");
        virtual ~UncertaintySet();
        virtual StatusCode initialize(const CP::SystematicSet& systConfig, const std::vector<UncertaintyGroup*>& groups);
        
        virtual std::string getName() const;
        
        // Uncertainty retrieval methods
        // These only work when the user is asking for a single CompScaleVar (unknown is fine if there's only one type)
        virtual bool   getValidity(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar = CompScaleVar::UNKNOWN) const;
        virtual double getUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar = CompScaleVar::UNKNOWN) const;
        virtual bool   getValidUncertainty(double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar = CompScaleVar::UNKNOWN) const;

        // More advanced retrieval methods for generic sets
        virtual std::vector<CompScaleVar::TypeEnum> getScaleVars() const;
        virtual std::vector< std::pair<CompScaleVar::TypeEnum,bool> > getValiditySet(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual std::vector< std::pair<CompScaleVar::TypeEnum,double> > getUncertaintySet(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual std::vector< std::pair<CompScaleVar::TypeEnum,bool> > getValidUncertaintySet(std::vector< std::pair<CompScaleVar::TypeEnum,double> >& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        // Private members
        const std::string m_name;
        bool m_isInit;
        std::vector<UncertaintyGroup*> m_groups;
        std::vector<float> m_shiftWeights;


        // Helper methods
        CompScaleVar::TypeEnum getSingleVar() const;

};

} // end jet namespace

#endif
