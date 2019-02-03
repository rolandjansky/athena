/*
 Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ISOLATIONSELECTION_TESTMARCOHELPERS_H
#define ISOLATIONSELECTION_TESTMARCOHELPERS_H

#include <AsgTools/ToolHandle.h>

#include <IsolationSelection/IsoVariableHelper.h>
#include <IsolationSelection/Defs.h>


#include <xAODBase/IParticle.h>
#include <xAODBase/IParticleContainer.h>

#include <TTree.h>
#include <vector>
#include <memory>

namespace CP {
    std::string EraseWhiteSpaces(std::string str);

    class IsolationWP;
    
    class IsoCorrectionTestHelper {
        public:
            IsoCorrectionTestHelper(TTree* outTree, const std::string& ContainerName, const std::vector<IsolationWP*> &WP, int part_type = -1);
            StatusCode Fill(xAOD::IParticleContainer* Particles);
            void BackupPreFix(const std::string &PreFix);
            void DefaultIsolation(const std::string &DecorName);
            void CorrectedIsolation(const std::string &DecorName);
            
        private:

            template<typename T> bool AddBranch(const std::string &Name, T &Element);
            
            
            float Charge(const xAOD::IParticle* P) const;
            StatusCode FillIsolationBranches(const xAOD::IParticle* P ,  const IsoHelperPtr & Acc, std::vector<float> &Original, std::vector<float> &Corrected);
            
            TTree* m_tree;
            bool m_init;
            std::vector<float> m_pt;
            std::vector<float> m_eta;
            std::vector<float> m_phi;
            std::vector<float> m_e;
            std::vector<int> m_Q;

            std::vector<bool> m_orig_passIso;
            std::vector<bool> m_corr_passIso;
            
            std::vector<float> m_assoc_track_pt;
            
            std::vector<float> m_assoc_cluster_et;
            std::vector<float> m_assoc_cluster_eta;
            std::vector<float> m_assoc_cluster_phi;
            
            
            struct IsolationBranches {
                std::vector<float> original_cones;
                std::vector<float> corrected_cones;                
                IsoHelperPtr Accessor;
                IsolationBranches(IsoType T, const std::string& prefix):
                        original_cones(),
                        corrected_cones(),
                        Accessor(){
                    Accessor = std::make_unique<IsoVariableHelper>(T,prefix);
                }
            };
            std::vector<IsolationBranches> m_iso_branches;
            
            SelectionAccessor m_acc_used_for_corr;
            SelectionAccessor m_acc_passDefault;
            SelectionAccessor m_acc_passCorrected;
         
    };

}

#endif
