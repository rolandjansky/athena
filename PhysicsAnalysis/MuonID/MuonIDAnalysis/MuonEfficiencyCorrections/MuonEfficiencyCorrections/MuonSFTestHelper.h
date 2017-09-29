/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONEFFICIENCYCORRECTION_MUONSFTESTHELPER_H
#define MUONEFFICIENCYCORRECTION_MUONSFTESTHELPER_H

// EDM include(s):
#include "xAODMuon/Muon.h"

// Tool Includes
#include <MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h>
#include <MuonEfficiencyCorrections/MuonTriggerScaleFactors.h>

#include <AsgTools/ToolHandle.h>
#include <AsgTools/AnaToolHandle.h>

#include <AsgTools/IAsgTool.h>
#include <AsgTools/AsgTool.h>

//General includes
#include <memory>
#include <map>
#include <TTree.h>
#include <TH1.h>


//Helper class to test the Muon efficiency SFs plus their systematics
namespace TestMuonSF {
    //#########################
    //      HelperFunctions
    //#########################
    void WriteHistogram(TFile* File, TH1* &Histo);
    
    template <typename T> T getProperty(const asg::IAsgTool* interface_tool, const std::string& prop_name){
        const asg::AsgTool* asg_tool = dynamic_cast<const asg::AsgTool*>(interface_tool);
        T prop;
        const T* HandlePtr = asg_tool->getProperty <T> (prop_name);
        if (!HandlePtr) Error("getProperty()", "Failed to retrieve property %s ", prop_name.c_str());
        else prop = (*HandlePtr);                
        return prop;
    }
    //########################################
    //      Template class to connect the
    //      Variables with the TTree
    //########################################
    class SFBranches{
          public:
            SFBranches(TTree* tree);
            virtual ~SFBranches();
            virtual std::string name() const =0;
            virtual bool init()=0;
        protected:
            template <typename T> bool initBranch(T& Var, const std::string& Syst){
               std::string bName = name()+ (Syst.empty() ? std::string("") : Syst) ;
                if (m_tree->FindBranch(bName.c_str())) {
                    Error("SFBranches::initBranch()", "The branch %s already exists in TTree %s", bName.c_str(), m_tree->GetName());
                    return false;
                }
                if (m_tree->Branch(bName.c_str(), &Var) == nullptr) {
                    Error("SFBranches::initBranch()", "Could not create the branch %s in TTree %s", bName.c_str(), m_tree->GetName());
                    return false;
                } else Info ("SFBranches::initBranch()", "Created the branch %s in TTree %s", bName.c_str(), m_tree->GetName());
                return true;                
            }
        private:
            TTree* m_tree;
            
    };
    //####################################################
    //      Helper class to write ntuples to test the 
    //      MuonTriggerScaleFactors
    //###################################################
    class TriggerSFBranches: public SFBranches {
        public:
            TriggerSFBranches(TTree* tree,const  ToolHandle<CP::IMuonTriggerScaleFactors>& Handle, const std::string& Trigger);
            virtual ~TriggerSFBranches();
            CP::CorrectionCode fill(const xAOD::MuonContainer* Muons);
            virtual bool init();
            virtual std::string name() const;
        private:
            CP::CorrectionCode getSF(const xAOD::MuonContainer* muons, double &Var, const CP::SystematicVariation &syst);
            ToolHandle<CP::IMuonTriggerScaleFactors> m_handle;
            std::string m_trigger;
            double m_nominal_SF;
            double m_stat_up_SF;
            double m_stat_down_SF;
            double m_sys_up_SF;
            double m_sys_down_SF;
    };
    typedef std::unique_ptr<TriggerSFBranches> TriggerSFBranch_Ptr;
    
    //###################################################
    //      General interface for the Reconstruction    #
    //      Isolation / TTVA scalefactors               #
    //###################################################
    class MuonEffiBranches: public SFBranches{
        public:
            MuonEffiBranches(TTree* tree);
            virtual CP::CorrectionCode fill (const xAOD::Muon& muon)=0;
            virtual ~MuonEffiBranches();
            
    };
    typedef std::unique_ptr<MuonEffiBranches> EffiBranch_Ptr;
    //###################################################################
    //      Helper class to write the scale-factor ntuples to test the  #
    //      MuonReconstruction/ Isolation/ TTVA scalefactors            #
    //###################################################################
    class MuonSFBranches: public MuonEffiBranches{
        public:
            MuonSFBranches(TTree* tree, const ToolHandle<CP::IMuonEfficiencyScaleFactors> &handle, const std::string& rel_name = "");
            virtual CP::CorrectionCode fill (const xAOD::Muon& muon);
            
            virtual bool init();
            virtual std::string name() const;
        private:
            ToolHandle<CP::IMuonEfficiencyScaleFactors> m_handle;
            std::string m_release;
           
            //SF's
            struct SFSet{
                SFSet(){
                    scale_factor = mc_eff = data_eff= 1.;
                }
                
                float scale_factor;
                float mc_eff;
                float data_eff;
            };
            bool AddToTree(const CP::SystematicSet& syst, MuonSFBranches::SFSet& ScaleFactor);
           
            std::map<CP::SystematicSet, SFSet> m_SFs;
            
    };
    //###################################################
    //      Helper class to write                       #
    //      The muon properties for the efficiency sfs  #
    //###################################################
    class MuonInfoBranches: public MuonEffiBranches{
        public:
            MuonInfoBranches(TTree* tree);
            virtual ~MuonInfoBranches();
            virtual bool init();
            virtual std::string name() const;
            virtual CP::CorrectionCode fill (const xAOD::Muon& muon);
           
        private:
            float m_pt;
            float m_eta;
            float m_phi;
            unsigned int m_quality;
            unsigned int m_author;
            unsigned int m_type;
    };
    
    class MuonSFTestHelper {
        public:
            //Standard Constructor
            MuonSFTestHelper(TTree* tree, const std::string& name="", bool write_muons = true);
            MuonSFTestHelper(const std::string& name = "", bool write_muons = true);
            ~MuonSFTestHelper();
            //############################
            //      Initialize the tool
            //############################
            bool init();
            void addTool(const asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> &handle)
            void addTool(const ToolHandle<CP::IMuonEfficiencyScaleFactors>& handle);


            CP::CorrectionCode fill(const xAOD::MuonContainer* muons);
            CP::CorrectionCode fill(const xAOD::Muon* mu);
            CP::CorrectionCode fill(const xAOD::Muon& mu);
            
            
            
        private:
            std::string m_name;
            std::shared_ptr<TTree> m_tree_ptr;
            TTree* m_tree;
            std::vector<EffiBranch_Ptr> m_Branches;

    };
    class MuonSFReleaseComparer {
        public:
            MuonSFReleaseComparer(MuonSFTestHelper& Rel1, MuonSFTestHelper& Rel2);
            StatusCode initialize();
            CP::CorrectionCode TestSF(const xAOD::Muon& mu);
            bool WriteHistosToFile(TFile* file);
            ~MuonSFReleaseComparer();
            protected:
            void CreateHistogram(const CP::SystematicSet &Set);

            MuonSFTestHelper& m_Rel1;
            MuonSFTestHelper& m_Rel2;

            struct ComparingHistos {
                    TH1* SF;
                    TH1* eff;
            };
            std::map<CP::SystematicSet, ComparingHistos> m_Histos;

    };

}

#endif
