/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
#ifndef EFFICOLLECTION_H_
#define EFFICOLLECTION_H_

// @class EffiCollection
/// @brief Utility class to collect the separate histos
/// for Calo, Central and High Eta muon SF


#include <MuonEfficiencyCorrections/MuonEfficiencyType.h>
#include "xAODMuon/Muon.h"
#include <map>
#include <string>
#include <iostream>

namespace CP {
    class EfficiencyScaleFactor;
    class MuonEfficiencyScaleFactors;

    typedef std::shared_ptr<EfficiencyScaleFactor> EfficiencyScaleFactor_Ptr;

    class EffiCollection {
        public:
            
            EffiCollection(const MuonEfficiencyScaleFactors& ref_tool);
            
            //Constructor with nominal as fallback..
            EffiCollection(const EffiCollection* Nominal, const MuonEfficiencyScaleFactors* ref_asg_tool , MuonEfficiencySystType sysType);
            EffiCollection(const EffiCollection& other);
            EffiCollection & operator =(const EffiCollection & other);

            /// return the correct SF type to provide, depending on eta and the author
            EfficiencyScaleFactor_Ptr retrieveSF(const xAOD::Muon & mu, unsigned int RunNumber) const;
            enum CollectionType {
                Central, Calo, Forward, CentralLowPt, CaloLowPt
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

                    CollectionContainer(const asg::AsgTool* ref_asg_tool, const std::string &FileName, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, EffiCollection::CollectionType FileType, bool isLowPt = false, bool hasPtDepSys = false);
                    CollectionContainer(CollectionContainer_Ptr Nominal, const asg::AsgTool* ref_asg_tool, const std::string &FileName, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, EffiCollection::CollectionType FileType, bool isLowPt = false, bool hasPtDepSys = false);


                    CollectionContainer & operator =(const CollectionContainer & other);
                    CollectionContainer(const CollectionContainer & other);
                    virtual ~CollectionContainer();

                    EfficiencyScaleFactor_Ptr retrieve(unsigned int RunNumer) const;
                    bool CheckConsistency() const;
                    std::string sysname() const;
                    bool SetSystematicBin(unsigned int Bin);
                    unsigned int nBins() const;
                    std::string GetBinName(unsigned int Bin) const;
                    int FindBinSF(const xAOD::Muon &mu) const;

                    EffiCollection::CollectionType type() const;
                    
                private:
                    bool LoadPeriod(unsigned int RunNumber) const;
                    typedef std::pair<unsigned int, unsigned int> RunRanges;

                    std::vector<EfficiencyScaleFactor_Ptr> m_SF;
                    mutable EfficiencyScaleFactor_Ptr m_currentSF;
                    EffiCollection::CollectionType m_FileType;
            };

        private:
            bool DoesBinFitInRange(CollectionContainer_Ptr Container, unsigned int & Bin) const;
            bool DoesMuonEnterBin(CollectionType Type, const xAOD::Muon mu, int &Bin) const;

            CollectionContainer* retrieveContainer(CollectionType Type) const;
            CollectionContainer* FindContainerFromBin(unsigned int &bin) const;
            CollectionContainer* FindContainer(const xAOD::Muon& mu) const;

            const CP::MuonEfficiencyScaleFactors& m_ref_tool;
            std::unique_ptr<CollectionContainer> m_central_eff;
            std::unique_ptr<CollectionContainer> m_calo_eff;
            std::unique_ptr<CollectionContainer> m_forward_eff;
            std::unique_ptr<CollectionContainer> m_lowpt_central_eff;
            std::unique_ptr<CollectionContainer> m_lowpt_calo_eff;

    };
}
#endif /* EFFICOLLECTION_H_ */
