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
                    m_sfType(CP::MuonEfficiencyType::Undefined) {
    }

    EffiCollection::EffiCollection(const std::string &file_central, const std::string &file_calo, const std::string &file_forward, const std::string &file_lowpt_central, const std::string &file_lowpt_calo, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, double lowPtTransition) :
                    EffiCollection() {
        m_lowpt_transition = lowPtTransition;
        m_sfType = effType;
        bool KineSyst = effType == CP::MuonEfficiencyType::Reco || effType == CP::MuonEfficiencyType::BadMuonVeto;
        m_central_eff = new CollectionContainer(file_central, sysType, m_sfType, CollectionType::Central, false, KineSyst);
        m_calo_eff = new CollectionContainer(file_calo, sysType, m_sfType, CollectionType::Calo, false, KineSyst);
        m_forward_eff = new CollectionContainer(file_forward, sysType, m_sfType, CollectionType::Forward);

        // last argument tells this to respond to dedicated low pt systematic variations
        if (m_lowpt_transition > 0) {
            m_lowpt_central_eff = new CollectionContainer(file_lowpt_central, sysType, m_sfType, CollectionType::CentralLowPt, true);
            m_lowpt_calo_eff = new CollectionContainer(file_lowpt_calo, sysType, m_sfType, CollectionType::CaloLowPt, true);
        }
    }

    EffiCollection::EffiCollection(const EffiCollection* Nominal, const std::string &file_central, const std::string &file_calo, const std::string &file_forward, const std::string &file_lowpt_central, const std::string &file_lowpt_calo, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, double lowPtTransition) :
                    EffiCollection() {
        m_lowpt_transition = lowPtTransition;
        m_sfType = effType;
        bool KineSyst = effType == CP::MuonEfficiencyType::Reco || effType == CP::MuonEfficiencyType::BadMuonVeto;
        m_central_eff = new CollectionContainer(Nominal->retrieveContainer(CollectionType::Central), file_central, sysType, m_sfType, CollectionType::Central, false, KineSyst);
        m_calo_eff = new CollectionContainer(Nominal->retrieveContainer(CollectionType::Calo), file_calo, sysType, m_sfType, CollectionType::Calo, false, KineSyst);
        m_forward_eff = new CollectionContainer(Nominal->retrieveContainer(CollectionType::Forward), file_forward, sysType, m_sfType, CollectionType::Forward);

        // last argument tells this to respond to dedicated low pt systematic variations
        if (m_lowpt_transition > 0) {
            m_lowpt_central_eff = new CollectionContainer(Nominal->retrieveContainer(CollectionType::CentralLowPt), file_lowpt_central, sysType, m_sfType, CollectionType::CentralLowPt, true);
            m_lowpt_calo_eff = new CollectionContainer(Nominal->retrieveContainer(CollectionType::CaloLowPt), file_lowpt_calo, sysType, m_sfType, CollectionType::CaloLowPt, true);
        }
    }
    EffiCollection::CollectionContainer* EffiCollection::retrieveContainer(CollectionType Type) const {
        if (Type == CollectionType::Central) {
            return m_central_eff;
        }
        if (Type == CollectionType::Forward) {
            return m_forward_eff;
        }
        if (Type == CollectionType::Calo) {
            return m_calo_eff;
        }
        if (Type == CollectionType::CentralLowPt) {
            return m_lowpt_central_eff;
        }
        if (Type == CollectionType::CaloLowPt) {
            return m_lowpt_calo_eff;
        }
        return NULL;
    }
    bool EffiCollection::CheckConsistency() const {
        if (!m_central_eff || !m_central_eff->CheckConsistency()) {
            return false;
        }
        if (!m_calo_eff || !m_calo_eff->CheckConsistency()) {
            return false;
        }
        if (!m_forward_eff || !m_forward_eff->CheckConsistency()) {
            return false;
        }
        if (!m_central_eff || !m_central_eff->CheckConsistency()) {
            return false;
        }
        if (m_lowpt_transition < 0.) {
            return true;
        }
        if (!m_lowpt_central_eff || !m_lowpt_central_eff->CheckConsistency()) {
            return false;
        }
        if (!m_lowpt_calo_eff || !m_lowpt_calo_eff->CheckConsistency()) {
            return false;
        }
        return true;
    }

    EffiCollection::EffiCollection(const CP::EffiCollection & other) {

        m_central_eff = new CP::EffiCollection::CollectionContainer(*(other.m_central_eff));
        m_calo_eff = new CP::EffiCollection::CollectionContainer(*(other.m_calo_eff));
        m_forward_eff = new CP::EffiCollection::CollectionContainer(*(other.m_forward_eff));
        m_lowpt_transition = other.m_lowpt_transition;
        m_sfType = other.m_sfType;
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
        m_sfType = other.m_sfType;
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
            if (fabs(mu.eta()) >= 2.5) {
                return m_forward_eff->retrieve(RunNumber);
            }
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
    std::string EffiCollection::sysname(void) const {
        if (m_central_eff) return m_central_eff->sysname();
        else return "";
    }
    unsigned int EffiCollection::nBins() const {
        unsigned int Nbins = 0;
        if (m_central_eff) {
            Nbins += m_central_eff->nBins();
        }
        if (m_calo_eff) {
            Nbins += m_calo_eff->nBins();
        }
        if (m_forward_eff) {
            Nbins += m_forward_eff->nBins();
        }
        if (m_lowpt_central_eff) {
            Nbins += m_lowpt_central_eff->nBins();
        }
        if (m_lowpt_calo_eff) {
            Nbins += m_lowpt_calo_eff->nBins();
        }
        return Nbins;
    }
    bool EffiCollection::SetSystematicBin(unsigned int Bin) {
        if (Bin > nBins()) {
            return false;
        }
        if (DoesBinFitInRange(m_central_eff, Bin)) {
            return m_central_eff->SetSystematicBin(Bin);
        } else if (DoesBinFitInRange(m_calo_eff, Bin)) {
            return m_calo_eff->SetSystematicBin(Bin);
        } else if (DoesBinFitInRange(m_forward_eff, Bin)) {
            return m_forward_eff->SetSystematicBin(Bin);
        } else if (DoesBinFitInRange(m_lowpt_central_eff, Bin)) {
            return m_lowpt_central_eff->SetSystematicBin(Bin);
        } else if (DoesBinFitInRange(m_lowpt_calo_eff, Bin)) {
            return m_lowpt_calo_eff->SetSystematicBin(Bin);
        }
        return true;
    }
    bool EffiCollection::IsLowPtBin(unsigned int Bin) const {
        if (DoesBinFitInRange(m_central_eff, Bin)) {
            return false;
        } else if (DoesBinFitInRange(m_calo_eff, Bin)) {
            return false;
        } else if (DoesBinFitInRange(m_forward_eff, Bin)) {
            return false;
        }
        return true;
    }
    bool EffiCollection::IsForwardBin(unsigned int Bin) const {
        if (DoesBinFitInRange(m_central_eff, Bin)) {
            return false;
        } else if (DoesBinFitInRange(m_calo_eff, Bin)) {
            return false;
        }
        return DoesBinFitInRange(m_forward_eff, Bin);
    }
    bool EffiCollection::DoesBinFitInRange(CollectionContainer* Container, unsigned int & Bin) const {
        if (Container) {
            if (Bin <= Container->nBins()) {
                return true;
            } else {
                Bin -= Container->nBins();
            }
        }
        return false;
    }
    EffiCollection::~EffiCollection() {

        if (m_central_eff) delete m_central_eff;
        if (m_calo_eff) delete m_calo_eff;
        if (m_forward_eff) delete m_forward_eff;
        if (m_lowpt_central_eff) delete m_lowpt_central_eff;
        if (m_lowpt_calo_eff) delete m_lowpt_calo_eff;
    }
    EffiCollection::CollectionContainer::CollectionContainer(const std::string &FileName, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, EffiCollection::CollectionType FileType, bool isLowPt, bool hasPtDepSys) :
                    CollectionContainer(NULL, FileName, sysType, effType, FileType, isLowPt, hasPtDepSys) {
    }

    EffiCollection::CollectionContainer::CollectionContainer(CollectionContainer* Nominal, const std::string &FileName, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, EffiCollection::CollectionType FileType, bool isLowPt, bool hasPtDepSys) :
                    m_SF(),
                    m_currentSF(m_SF.end()),
                    m_FileType(FileType) {
        TFile* fin = TFile::Open(FileName.c_str());
        if (!fin) {
            Error("CollectionContainer", ("Unable to open file " + FileName).c_str());
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
            if (!Nominal) {
                m_SF.insert(std::pair<RunRanges, EfficiencyScaleFactor*>(period.second, new EfficiencyScaleFactor(FileName, period.first, sysType, effType, isLowPt, hasPtDepSys)));
            } else {
                m_SF.insert(std::pair<RunRanges, EfficiencyScaleFactor*>(period.second, new EfficiencyScaleFactor(Nominal->retrieve(period.second.first), FileName, period.first, sysType, effType, isLowPt, hasPtDepSys)));
            }
        }
        for (auto& period : m_SF)
            period.second->ApplySysVariation();
        m_currentSF = m_SF.end();
    }
    bool EffiCollection::CollectionContainer::CheckConsistency() const {
        if (m_SF.empty()) {
            Error("CollectionContainer", "Could not retrieve any SFs from the input file");
            return false;
        }
        for (std::map<RunRanges, EfficiencyScaleFactor*>::const_iterator Itr = m_SF.begin(); Itr != m_SF.end(); ++Itr) {
            if (!Itr->second->CheckConsistency()) return false;
            for (std::map<RunRanges, EfficiencyScaleFactor*>::const_iterator Itr1 = m_SF.begin(); Itr1 != Itr; ++Itr1) {
                if ((Itr1->first.first <= Itr->first.first && Itr1->first.second > Itr->first.first) || (Itr->first.first <= Itr1->first.first && Itr->first.second > Itr1->first.first)) {
                    Error("CollectionContainer", "Overlapping periods observed in filetype %i. As run %i is in period %i - %i. Please check your SF file!", (int) m_FileType, Itr1->first.first, Itr->first.first, Itr->first.second);
                    return false;
                }
            }
        }
        return true;
    }
    bool EffiCollection::CollectionContainer::LoadPeriod(unsigned int RunNumber) {
        m_currentSF = m_SF.end();
        if (m_currentSF == m_SF.end() || m_currentSF->first.first > RunNumber || m_currentSF->first.second < RunNumber) {
            for (m_currentSF = m_SF.begin(); m_currentSF != m_SF.end(); ++m_currentSF) {
                if (m_currentSF->first.first <= RunNumber && m_currentSF->first.second >= RunNumber) return true;
            }
        } else return true;
        Error("CollectionContainer", Form("Could not find any SF period matching the run number %u", RunNumber));
        return false;
    }
    EfficiencyScaleFactor* EffiCollection::CollectionContainer::retrieve(unsigned int RunNumber) {
        if (!LoadPeriod(RunNumber)) {
            return m_SF.begin()->second;
        }
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
    bool EffiCollection::CollectionContainer::SetSystematicBin(unsigned int Bin) {
        for (auto& Period : m_SF) {
            if (!Period.second->SetSystematicBin(Bin)) {
                return false;
            }
        }
        return true;
    }
    unsigned int EffiCollection::CollectionContainer::nBins() const {
        return m_SF.begin()->second->nBinsSF();
    }
}
