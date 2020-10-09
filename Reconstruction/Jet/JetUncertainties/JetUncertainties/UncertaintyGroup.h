/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_UNCERTAINTYGROUP_H
#define JETUNCERTAINTIES_UNCERTAINTYGROUP_H

#include "JetUncertainties/UncertaintyEnum.h"
#include "JetUncertainties/UncertaintyComponent.h"
#include "JetUncertainties/ConfigHelper.h"

#include "xAODJet/Jet.h"
#include "xAODEventInfo/EventInfo.h"

#include "AsgMessaging/AsgMessaging.h"

#include "TString.h"

#include <set>
#include <map>

class TFile;

namespace jet
{

class UncertaintyGroup : public asg::AsgMessaging
{
    public:
        // Constructor/destructor/initialization
        UncertaintyGroup(const GroupHelper& group);
        UncertaintyGroup(const UncertaintyGroup& toCopy);
        //virtual UncertaintyGroup* clone() const = 0;
        UncertaintyGroup & operator=(const UncertaintyGroup &) = delete;
        virtual ~UncertaintyGroup();
        virtual StatusCode addComponent(UncertaintyComponent* component);
        virtual StatusCode addSubgroup(UncertaintyGroup* subgroup);
        virtual StatusCode initialize(TFile* histFile);

        // Information retrieval methods
        virtual TString                   getName()          const { return m_name;              }
        virtual TString                   getDesc()          const { return m_desc;              }
        virtual int                       getGroupNum()      const { return m_groupNum;          }
        virtual int                       getSubgroupNum()   const { return m_subgroupNum;       }
        virtual CompCategory::TypeEnum    getCategory()      const { return m_category;          }
        virtual CompCorrelation::TypeEnum getCorrType()      const { return m_corrType;          }
        virtual bool 			  getIsReducible()   const { return m_isReducible;       }
        virtual size_t                    getNumRawComps()   const { return m_subgroups.size();  }
        virtual size_t                    getNumSubgroups()  const { return m_subgroups.size();  }
        virtual size_t                    getNumComponents() const;

        virtual std::vector<UncertaintyComponent*> getComponents() const { return m_components;  }
        virtual std::vector<UncertaintyGroup*> getSubgroups() const { return m_subgroups;        }
  
        // Access to information on the constituent components
        virtual std::set<CompScaleVar::TypeEnum> getScaleVars() const;

        // Methods to check for special situations
        virtual bool isAlwaysZero()  const;

        // Uncertainty retrieval methods (wrappers)
        virtual bool   getValidity(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar) const;
        virtual double getUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar) const;
        virtual bool   getValidUncertainty(double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar) const;


    protected:
        // Protected members
        bool  m_isInit;
        const TString m_name;
        const TString m_desc;
        const int m_groupNum;
        const int m_subgroupNum;
        const CompCategory::TypeEnum m_category;
        const CompCorrelation::TypeEnum m_corrType;
        const bool m_isReducible;
        std::vector<UncertaintyComponent*> m_components;
        std::vector<UncertaintyGroup*> m_subgroups;
        std::map<CompScaleVar::TypeEnum,std::vector<UncertaintyComponent*>*> m_compScaleMap;

    private:
        UncertaintyGroup(const std::string& name = "");
        
};

// Sorting operators
bool operator <  (const UncertaintyGroup& groupA, const UncertaintyGroup& groupB);
bool operator == (const UncertaintyGroup& groupA, const UncertaintyGroup& groupB);

} // end jet namespace


#endif
