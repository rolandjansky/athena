/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
#ifndef EFFICOLLECTION_H_
#define EFFICOLLECTION_H_

// @class EffiCollection
/// @brief Utility class to collect the separate histos
/// for Calo, Central and High Eta muon SF


#include <MuonEfficiencyCorrections/MuonEfficiencyType.h>
#include <MuonEfficiencyCorrections/EfficiencyScaleFactor.h>
#include <xAODMuon/Muon.h>
#include <map>
#include <string>
#include <iostream>
namespace asg{
    class AsgTool;
}

namespace CP {
    class MuonEfficiencyScaleFactors;

    class EffiCollection {
        public:
            EffiCollection(const MuonEfficiencyScaleFactors& ref_tool);
            //Constructor with nominal as fallback..
            EffiCollection(const EffiCollection* Nominal, const MuonEfficiencyScaleFactors& ref_tool, const std::string& syst, int syst_bit_map, bool is_up);
          
            /// return the correct SF type to provide, depending on eta and the author
            EfficiencyScaleFactor_Ptr retrieveSF(const xAOD::Muon & mu, unsigned int RunNumber) const;
            enum CollectionType {
                Central = 1, 
                Calo   = 1<<1, 
                Forward = 1<<2, 
                CentralLowPt = 1<<3, 
                CaloLowPt = 1<<4
            };
            enum Systematic{
                Symmetric = 1<<5,
                PtDependent = 1<<6,
                UnCorrelated = 1<<7,
                
            };
            static std::string FileTypeName(EffiCollection::CollectionType T);

            // return the name of the systematic variation being run
            std::string sysname() const;

            // a consistency check
            bool CheckConsistency() const;

            virtual ~EffiCollection();
            // Get the number of all bins in the scale-factor maps including
            // the overflow & underflow bins
            unsigned int nBins() const;
            
            // If systematic decorrelation is activated then the user needs to loop
            // manually over the syst bins. This method activates the i-th bin to 
            // be active. For the remaining bins the nominal scale-factor is returned
            // instead.
            bool SetSystematicBin(unsigned int Bin);
            
            // Checks whether the i-th bin belongs to the low-pt map...
            bool IsLowPtBin(unsigned int Bin) const;
            // Checks whether the i-th bin belongs to the forward map
            bool IsForwardBin(unsigned int Bin) const;

            
            std::string GetBinName(unsigned int bin) const;
            
            // Returns the bin number from which the scale-factor of the muon
            // is going to be retrieved...
            int getUnCorrelatedSystBin(const xAOD::Muon& mu) const;

        protected:
            class CollectionContainer {
                public:
                    // Nominal constructor... Only needs to know about it's type and the file to load
                    CollectionContainer(EffiCollection::CollectionType FileType, const std::string &file_name);
                   //CollectionContainer(EffiCollection::CollectionType FileType, const std::string &file_name);
                  

                   
                    EfficiencyScaleFactor* retrieve(unsigned int RunNumer) const;
                    bool CheckConsistency() const;
                    std::string sysname() const;
                    
                    bool SetSystematicBin(unsigned int Bin);
                    void SetGlobalOffSet(unsigned int OffSet);
                    
                    unsigned int nBins() const;
                    
                    std::string GetBinName(unsigned int Bin) const;
                    
                    int FindBinSF(const xAOD::Muon &mu) const;

                    EffiCollection::CollectionType type() const;
                    
                private:
                    bool LoadPeriod(unsigned int RunNumber) const;
                   
                    std::vector<std::unique_ptr<EfficiencyScaleFactor>> m_SF;
                    mutable EfficiencyScaleFactor* m_currentSF;
                    
                    EffiCollection::CollectionType m_FileType;
                    /// Offset to translate between the bin-numbers in the bin numbers
                    /// of each file against the global bin-number
                    unsigned int m_binOffSet;
                
            };

        private:
            bool DoesBinFitInRange(CollectionContainer_Ptr Container, unsigned int & Bin) const;
            bool DoesMuonEnterBin(CollectionType Type, const xAOD::Muon mu, int &Bin) const;

            CollectionContainer* retrieveContainer(CollectionType Type) const;
            CollectionContainer* FindContainerFromBin(unsigned int &bin) const;
            CollectionContainer* FindContainer(const xAOD::Muon& mu) const;
            
            const MuonEfficiencyScaleFactors& m_ref_tool;
            
            /// Make the collection container shared ptr to allow that a systematic EffiCollection
            /// can use the same container as the nominal one if the current systematic has no
            /// effect on that particular container....
            std::shared_ptr<CollectionContainer> m_central_eff;
            std::shared_ptr<CollectionContainer> m_calo_eff;
            std::shared_ptr<CollectionContainer> m_forward_eff;
            std::shared_ptr<CollectionContainer> m_lowpt_central_eff;
            std::shared_ptr<CollectionContainer> m_lowpt_calo_eff;

            
        
    };
}
#endif /* EFFICOLLECTION_H_ */
