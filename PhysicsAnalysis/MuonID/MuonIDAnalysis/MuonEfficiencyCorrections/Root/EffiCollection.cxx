/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * EffiCollection.cxx
 *
 *  Created on: Jun 21, 2014
 *      Author: goblirsc
 */

#include "MuonEfficiencyCorrections/EffiCollection.h"
#include <TTree.h>
namespace CP {

    EffiCollection::EffiCollection() :
                m_central_eff(0),
                m_calo_eff(0),
                m_forward_eff(0),
                m_lowpt_central_eff(0),
                m_lowpt_calo_eff(0),
                m_lowpt_transition(20000.),
                m_Type(CP::MuonEfficiencyType::Undefined) {
    }

    EffiCollection::EffiCollection(const std::string &file_central, const std::string &file_calo, const std::string &file_forward, const std::string &file_lowpt_central, const std::string &file_lowpt_calo, SystematicSet sys, CP::MuonEfficiencyType effType, double lowPtTransition) :
                EffiCollection() {
        m_lowpt_transition = lowPtTransition;
        m_Type = effType;
        m_central_eff = new CollectionContainer(file_central, sys, m_Type, false, effType == CP::MuonEfficiencyType::Reco);
        m_calo_eff = new CollectionContainer(file_calo, sys, m_Type, false, effType == CP::MuonEfficiencyType::Reco);
        m_forward_eff = new CollectionContainer(file_forward, sys, m_Type);

        // last argument tells this to respond to dedicated low pt systematic variations
        if (m_lowpt_transition > 0) {
            m_lowpt_central_eff = new CollectionContainer(file_lowpt_central, sys, m_Type, true);
            m_lowpt_calo_eff = new CollectionContainer(file_lowpt_calo, sys, m_Type, true);
        }
    }

    bool EffiCollection::CheckConsistency() {

        return (m_central_eff && m_central_eff->CheckConsistency() && m_calo_eff && m_calo_eff->CheckConsistency() && m_forward_eff && m_forward_eff->CheckConsistency() && (m_lowpt_transition < 0 || (m_lowpt_central_eff && m_lowpt_central_eff->CheckConsistency() && m_lowpt_calo_eff && m_lowpt_calo_eff->CheckConsistency())));
    }

    EffiCollection::EffiCollection(const CP::EffiCollection & other) {

        m_central_eff = new CP::EffiCollection::CollectionContainer(*(other.m_central_eff));
        m_calo_eff = new CP::EffiCollection::CollectionContainer(*(other.m_calo_eff));
        m_forward_eff = new CP::EffiCollection::CollectionContainer(*(other.m_forward_eff));
        m_lowpt_transition = other.m_lowpt_transition;
        m_Type = other.m_Type;
        if (m_lowpt_transition > 0) {
            m_lowpt_central_eff = new CP::EffiCollection::CollectionContainer(*(other.m_lowpt_central_eff));
            m_lowpt_calo_eff = new CP::EffiCollection::CollectionContainer(*(other.m_lowpt_calo_eff));
        } else {
            m_lowpt_central_eff = 0;
            m_lowpt_calo_eff = 0;
        }

    }
    CP::EffiCollection & EffiCollection::operator =(const CP::EffiCollection & other) {
        if (this == &other) {
            return *this;
        }
        m_lowpt_transition = other.m_lowpt_transition;
        m_Type = other.m_Type;
        if (m_central_eff) delete m_central_eff;
        if (m_calo_eff) delete m_calo_eff;
        if (m_forward_eff) delete m_forward_eff;
        if (m_lowpt_central_eff) delete m_lowpt_central_eff;
        if (m_lowpt_calo_eff) delete m_lowpt_calo_eff;
        m_central_eff = new CP::EffiCollection::CollectionContainer(*(other.m_central_eff));
        m_calo_eff = new CP::EffiCollection::CollectionContainer(*(other.m_calo_eff));
        m_forward_eff = new CP::EffiCollection::CollectionContainer(*(other.m_forward_eff));
        if (m_lowpt_transition > 0) {
            m_lowpt_central_eff = new CP::EffiCollection::CollectionContainer(*(other.m_lowpt_central_eff));
            m_lowpt_calo_eff = new CP::EffiCollection::CollectionContainer(*(other.m_lowpt_calo_eff));
        } else {
            m_lowpt_central_eff = 0;
            m_lowpt_calo_eff = 0;
        }
        return *this;

    }
    CP::EfficiencyScaleFactor* EffiCollection::retrieveSF(const xAOD::Muon& mu, unsigned int RunNumber) {

        // yes, we need the second check - negative pt does happen!
        if (mu.pt() < m_lowpt_transition && m_lowpt_transition > 0) {
            if (mu.author() == xAOD::Muon::CaloTag) {
                return m_lowpt_calo_eff->retrieve(RunNumber);
            }
            return m_lowpt_central_eff->retrieve(RunNumber);
        }
        if (mu.author() == xAOD::Muon::CaloTag) {
            return m_calo_eff->retrieve(RunNumber);
        } else if (fabs(mu.eta()) < 2.5) {
            return m_central_eff->retrieve(RunNumber);
        } else {
            return m_forward_eff->retrieve(RunNumber);
        }
    }
    std::string EffiCollection::sysname(void) {
        if (m_central_eff) return m_central_eff->sysname();
        else return "";
    }

    EffiCollection::~EffiCollection() {

        if (m_central_eff) delete m_central_eff;
        if (m_calo_eff) delete m_calo_eff;
        if (m_forward_eff) delete m_forward_eff;
        if (m_lowpt_central_eff) delete m_lowpt_central_eff;
        if (m_lowpt_calo_eff) delete m_lowpt_calo_eff;
    }
    EffiCollection::CollectionContainer::CollectionContainer(const std::string &FileName, SystematicSet sys, CP::MuonEfficiencyType effType, bool isLowPt, bool hasPtDepSys) :
                m_SF(),
                m_currentSF(m_SF.end()) {
        TFile* fin = TFile::Open(FileName.c_str());
        if (!fin) {
            Error("CollectionContainer", "%s", ("Unable to open file " + FileName).c_str());
            return;
        }
        TTree* intree = 0;
        fin->GetObject("LumiData", intree);
        std::map<std::string, RunRanges> map;
        // if no Lumi tree is found, we assume that the SF are not binned in time
        if (!intree) {
            map["All"] = RunRanges(1, 999999);

        } else {
            std::string *period = 0;
            unsigned int firstRun = 0;
            unsigned int lastRun = 0;
            intree->SetBranchAddress("Period", &period);
            intree->SetBranchAddress("FirstRun", &firstRun);
            intree->SetBranchAddress("LastRun", &lastRun);
            for (int t = 0; intree->GetEntry(t); t++) {
                map[*period] = RunRanges(firstRun, lastRun);
            }
        }
        fin->Close();
        delete fin;
        for (auto& period : map) {
            m_SF.insert(std::pair<RunRanges, EfficiencyScaleFactor*>(period.second, new EfficiencyScaleFactor(FileName, period.first, sys, effType, isLowPt, hasPtDepSys)));
        }
        for (auto& period : m_SF)
            period.second->ApplySysVariation();
        m_currentSF = m_SF.end();
    }
    bool EffiCollection::CollectionContainer::CheckConsistency() {
        if (m_SF.empty()) {
            Error("CollectionContainer", "Could not retrieve any SFs from the input file");
            return false;
        }
        for (m_currentSF = m_SF.begin(); m_currentSF != m_SF.end(); ++m_currentSF) {
            if (!m_currentSF->second->CheckConsistency()) return false;
        }
        return true;
    }
    bool EffiCollection::CollectionContainer::LoadPeriod(unsigned int RunNumber) {
        if (m_currentSF == m_SF.end() || m_currentSF->first.first > RunNumber || m_currentSF->first.second < RunNumber) {
            for (m_currentSF = m_SF.begin(); m_currentSF != m_SF.end(); ++m_currentSF) {
                if (m_currentSF->first.first <= RunNumber && m_currentSF->first.second >= RunNumber) return true;
            }
        } else return true;
        Error("CollectionContainer", "Could not find any SF period matching the run number %u", RunNumber);
        return false;
    }
    EfficiencyScaleFactor* EffiCollection::CollectionContainer::retrieve(unsigned int RunNumber) {
        if (!LoadPeriod(RunNumber)) return m_SF.begin()->second;
        return m_currentSF->second;
    }
    std::string EffiCollection::CollectionContainer::sysname() {
        if (m_SF.empty()) return "";
        return m_SF.begin()->second->sysname();
    }
    EffiCollection::CollectionContainer::~CollectionContainer() {
        for (auto& period : m_SF) {
            if (period.second) delete period.second;
            period.second = NULL;
        }
        m_SF.clear();
    }

    CP::EffiCollection::CollectionContainer& EffiCollection::CollectionContainer::operator =(const EffiCollection::CollectionContainer & other) {
        if (this == &other) {
            return *this;
        }
        for (auto& period : m_SF) {
            delete period.second;
        }
        m_SF.clear();
        for (auto& period : other.m_SF) {
            m_SF[period.first] = new EfficiencyScaleFactor(*period.second);
        }
        m_currentSF = m_SF.end();
        return *this;
    }
    EffiCollection::CollectionContainer::CollectionContainer(const CollectionContainer & other) {
        for (auto& period : other.m_SF) {
            m_SF[period.first] = new EfficiencyScaleFactor(*period.second);
        }
        m_currentSF = m_SF.end();
    }

}
