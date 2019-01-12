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
    class EfficiencyScaleFactor;

    class EffiCollection {
        public:
            EffiCollection(const MuonEfficiencyScaleFactors& ref_tool);
            //Constructor with nominal as fallback..
            EffiCollection(const EffiCollection* Nominal, const MuonEfficiencyScaleFactors& ref_tool, const std::string& syst, int syst_bit_map, bool is_up);
          
            /// return the correct SF type to provide, depending on eta and the author
            EfficiencyScaleFactor* retrieveSF(const xAOD::Muon & mu, unsigned int RunNumber) const;
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
                UpVariation = 1<<8,
            };
            static std::string FileTypeName(EffiCollection::CollectionType T);
            
            
            /// a consistency check of the scale-factor maps. All scale-factor maps must be present
            /// and there must no overlapping periods to pass this test.
            bool CheckConsistency();

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
                    CollectionContainer(const MuonEfficiencyScaleFactors& ref_tool, EffiCollection::CollectionType FileType);
                    CollectionContainer(const MuonEfficiencyScaleFactors& ref_tool, CollectionContainer* Nominal, const std::string& syst_name, unsigned int syst_bit_map);
                    
                  

                   
                    EfficiencyScaleFactor* retrieve(unsigned int RunNumer) const;
                    bool isBinInMap (unsigned int bin) const;
                    
                    bool CheckConsistency();
                    std::string sysname() const;
                    
                    bool SetSystematicBin(unsigned int Bin);
                    void SetGlobalOffSet(unsigned int OffSet);
                    
                    unsigned int nBins() const;
                    
                    std::string GetBinName(unsigned int Bin) const;
                    
                    int FindBinSF(const xAOD::Muon &mu) const;

                    EffiCollection::CollectionType type() const;
                    
                    
                private:
                    std::map<std::string, std::pair<unsigned int, unsigned int>> findPeriods(const MuonEfficiencyScaleFactors& ref_tool) const;
                    std::string fileName(const MuonEfficiencyScaleFactors& ref_tool) const;
                  
                    bool LoadPeriod(unsigned int RunNumber) const;
                   
                    std::vector<std::shared_ptr<EfficiencyScaleFactor>> m_SF;
                    mutable EfficiencyScaleFactor* m_currentSF;
                    
                    EffiCollection::CollectionType m_FileType;
                    /// Offset to translate between the bin-numbers in the bin numbers
                    /// of each file against the global bin-number
                    unsigned int m_binOffSet;
                
            };

            /// Method to retrieve a container from the class ordered by a collection type
            /// This method is mainly used to propagate the nominal maps to the variations
            /// as fallback maps if no variation has been defined in this situation
            std::shared_ptr<CollectionContainer> retrieveContainer(CollectionType Type) const;

        private:
            CollectionContainer* FindContainer(unsigned int bin) const;
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
