/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

/*
 * EffiCollection.h
 *
 *  Created on: Jun 21, 2014
 *      Author: goblirsc
 */

#ifndef EFFICOLLECTION_H_
#define EFFICOLLECTION_H_

// @class EffiCollection
/// @brief Utility class to collect the separate histos
/// for Calo, Central and High Eta muon SF


#include <MuonEfficiencyCorrections/MuonEfficiencyType.h>
#include <AsgTools/AsgTool.h>

#include "xAODMuon/Muon.h"

#include <map>
#include <string>
#include <iostream>

namespace CP {
    class EfficiencyScaleFactor;

    typedef std::shared_ptr<EfficiencyScaleFactor> EfficiencyScaleFactor_Ptr;

    class EffiCollection {
        public:
            EffiCollection();
            EffiCollection(const asg::AsgTool* ref_asg_tool, const std::string &file_central, const std::string &file_calo, const std::string &file_forward, const std::string &file_lowpt_central, const std::string &file_lowpt_calo, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, double lowPtTransition = 20000.);
            //Constructor with nominal as fallback..
            EffiCollection(const EffiCollection* Nominal, const asg::AsgTool* ref_asg_tool, const std::string &file_central, const std::string &file_calo, const std::string &file_forward, const std::string &file_lowpt_central, const std::string &file_lowpt_calo, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, double lowPtTransition = 20000.);
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
            unsigned int nBins() const;
            bool SetSystematicBin(unsigned int Bin);
            bool IsLowPtBin(unsigned int Bin) const;
            bool IsForwardBin(unsigned int Bin) const;

            std::string GetBinName(unsigned int bin) const;
            int getUnCorrelatedSystBin(const xAOD::Muon& mu) const;

        protected:
            //Forward declaration for the shared_ptr
            class CollectionContainer;
            typedef std::shared_ptr<CollectionContainer> CollectionContainer_Ptr;
            //Create a subclass to handle the periods

            class CollectionContainer {
                public:

                    CollectionContainer(const asg::AsgTool* ref_asg_tool, const std::string &FileName, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, EffiCollection::CollectionType FileType, bool isLowPt = false, bool hasPtDepSys = false);
                    CollectionContainer(CollectionContainer_Ptr Nominal, const asg::AsgTool* ref_asg_tool, const std::string &FileName, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, EffiCollection::CollectionType FileType, bool isLowPt = false, bool hasPtDepSys = false);


                    CollectionContainer & operator =(const CollectionContainer & other);
                    CollectionContainer(const CollectionContainer & other);
                    virtual ~CollectionContainer();

                    EfficiencyScaleFactor_Ptr retrieve(unsigned int RunNumer);
                    bool CheckConsistency() const;
                    std::string sysname();
                    bool SetSystematicBin(unsigned int Bin);
                    unsigned int nBins() const;
                    std::string GetBinName(unsigned int Bin) const;
                    int FindBinSF(const xAOD::Muon &mu) const;

                    EffiCollection::CollectionType type() const;

                protected:
                    bool LoadPeriod(unsigned int RunNumber);
                    typedef std::pair<unsigned int, unsigned int> RunRanges;

                    std::map<RunRanges, EfficiencyScaleFactor_Ptr> m_SF;
                    std::map<RunRanges, EfficiencyScaleFactor_Ptr>::const_iterator m_currentSF;
                    EffiCollection::CollectionType m_FileType;
            };

        private:


            bool DoesBinFitInRange(CollectionContainer_Ptr Container, unsigned int & Bin) const;
            bool DoesMuonEnterBin(CollectionType Type, const xAOD::Muon mu, int &Bin) const;
            std::string ReplaceExpInString(std::string str, const std::string &exp, const std::string &rep) const;

            CollectionContainer_Ptr retrieveContainer(CollectionType Type) const;
            CollectionContainer_Ptr FindContainerFromBin(unsigned int &bin) const;
            CollectionContainer_Ptr FindContainer(const xAOD::Muon& mu) const;

            CollectionContainer_Ptr m_central_eff;
            CollectionContainer_Ptr m_calo_eff;
            CollectionContainer_Ptr m_forward_eff;
            CollectionContainer_Ptr m_lowpt_central_eff;
            CollectionContainer_Ptr m_lowpt_calo_eff;

            double m_lowpt_transition;
            CP::MuonEfficiencyType m_sfType;
    };
}
#endif /* EFFICOLLECTION_H_ */
