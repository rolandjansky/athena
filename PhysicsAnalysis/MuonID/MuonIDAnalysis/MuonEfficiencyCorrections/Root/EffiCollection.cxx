/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include <MuonEfficiencyCorrections/EffiCollection.h>
#include <MuonEfficiencyCorrections/EfficiencyScaleFactor.h>
#include <MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h>

#include <TTree.h>
namespace CP {
    const std::vector<std::string> ToRemove { "GeV", "MeV", "[", "]", "{", "}", "(", ")", "#", " " };
    EffiCollection::EffiCollection(const MuonEfficiencyScaleFactors& ref_tool) :
            m_ref_tool(ref_tool),
            m_central_eff(),
            m_calo_eff(),
            m_forward_eff(),
            m_lowpt_central_eff(),
            m_lowpt_calo_eff() {

        m_central_eff = std::make_shared<CollectionContainer>(CollectionType::Central, m_ref_tool.filename_Central());
        if (m_ref_tool.filename_Calo() != m_ref_tool.filename_Central()){
            m_calo_eff = std::make_shared<CollectionContainer>(CollectionType::Calo, m_ref_tool.filename_Calo());
        } else m_calo_eff = m_central_eff;
        
        if (m_ref_tool.filename_HighEta() != m_ref_tool.filename_Central()){
            m_forward_eff = std::make_shared<CollectionContainer>(CollectionType::Forward, m_ref_tool.filename_HighEta());
        } else m_forward_eff = m_central_eff;
        
        if (m_ref_tool.lowPtTransition() > 0  && m_ref_tool.filename_LowPt() != m_ref_tool.filename_Central()){
            m_lowpt_central_eff = std::make_shared<CollectionContainer>(CollectionType::CentralLowPt, m_ref_tool.filename_LowPt());
        } else m_lowpt_central_eff = m_central_eff;
       
         if (m_ref_tool.lowPtTransition() > 0  && m_ref_tool.filename_LowPtCalo() != m_ref_tool.filename_Central()){
            m_lowpt_central_eff = std::make_shared<CollectionContainer>(CollectionType::CentralLowPt, m_ref_tool.filename_LowPtCalo());
        } else m_lowpt_calo_eff = m_central_eff;
       
        //  bool KineSyst = effType == CP::MuonEfficiencyType::Reco || effType == CP::MuonEfficiencyType::BadMuonVeto;
       
    }
    EffiCollection::EffiCollection(const EffiCollection* Nominal, const MuonEfficiencyScaleFactors& ref_tool, const std::string& syst, int syst_bit_map, bool is_up):
            m_ref_tool(ref_tool),
            m_central_eff(),
            m_calo_eff(),
            m_forward_eff(),
            m_lowpt_central_eff(),
            m_lowpt_calo_eff(){
        
    }

    EffiCollection::CollectionContainer* EffiCollection::retrieveContainer(CollectionType Type) const {
        if (Type == CollectionType::Central) return m_central_eff.get();
        if (Type == CollectionType::Forward) return m_forward_eff.get();
        if (Type == CollectionType::Calo) return m_calo_eff.get();
        if (Type == CollectionType::CentralLowPt) return m_lowpt_central_eff.get();
        if (Type == CollectionType::CaloLowPt) return m_lowpt_calo_eff.get();
        return nullptr;
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
        if (!m_lowpt_central_eff || !m_lowpt_central_eff->CheckConsistency()) {
            return false;
        }
        if (!m_lowpt_calo_eff || !m_lowpt_calo_eff->CheckConsistency()) {
            return false;
        }
        return true;
    }

    EffiCollection::CollectionContainer* EffiCollection::FindContainer(const xAOD::Muon& mu) const {
        // yes, we need the second check - negative pt does happen!
        if (mu.pt() <  m_ref_tool.lowPtTransition()) {
            if (fabs(mu.eta()) >= 2.5) {
                return m_forward_eff;
            }
            if (mu.author() == xAOD::Muon::CaloTag) {
                return m_lowpt_calo_eff;
            }
            return m_lowpt_central_eff;
        }
        if (mu.author() == xAOD::Muon::CaloTag) {
            return m_calo_eff;
        } else if (fabs(mu.eta()) < 2.5) {
            return m_central_eff;
        } else {
            return m_forward_eff;
        }
    }

    EfficiencyScaleFactor* EffiCollection::retrieveSF(const xAOD::Muon& mu, unsigned int RunNumber) const {
        EffiCollection::CollectionContainer* Cont = FindContainer(mu);
        if (Cont.get() != nullptr) return Cont->retrieve(RunNumber);
        Warning("EffiCollection::retrieveSF()", "Invalid muon");
        return nullptr;
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
        EffiCollection::CollectionContainer_Ptr Cont = FindContainerFromBin(Bin);
        if (!Cont) return false;
        return Cont->SetSystematicBin(Bin);
    }
    EffiCollection::CollectionContainer_Ptr EffiCollection::FindContainerFromBin(unsigned int &Bin) const {
        if (Bin > nBins()) {
            return nullptr;
        }
        if (DoesBinFitInRange(m_central_eff, Bin)) {
            return m_central_eff;
        } else if (DoesBinFitInRange(m_calo_eff, Bin)) {
            return m_calo_eff;
        } else if (DoesBinFitInRange(m_forward_eff, Bin)) {
            return m_forward_eff;
        } else if (DoesBinFitInRange(m_lowpt_central_eff, Bin)) {
            return m_lowpt_central_eff;
        } else if (DoesBinFitInRange(m_lowpt_calo_eff, Bin)) {
            return m_lowpt_calo_eff;
        }
        return nullptr;
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
    bool EffiCollection::DoesBinFitInRange(CollectionContainer_Ptr Container, unsigned int & Bin) const {
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
   }
    std::string EffiCollection::FileTypeName(EffiCollection::CollectionType T) {
        if (T == CollectionType::Central) return "Central ";
        if (T == CollectionType::Calo) return "Calo ";
        if (T == CollectionType::Forward) return "Forward ";
        if (T == CollectionType::CentralLowPt) return "CentralLowPt ";
        if (T == CollectionType::CaloLowPt) return "CaloLowPt ";
        return "EffiCollection::FileTypeName() - WARNING: Unknown EffiCollection::CollectionType!";
    }

    std::string EffiCollection::GetBinName(unsigned int bin) const {
        EffiCollection::CollectionContainer_Ptr Cont = FindContainerFromBin(bin);
        if (Cont) {
            std::string BinName = FileTypeName(Cont->type()) + Cont->GetBinName(bin);
            for (auto R : ToRemove) {
                BinName = ReplaceExpInString(BinName, R, "");
            }
            return BinName;
        }
        Warning("EffiCollection::GetBinName()", "Unknown bin %u", bin);
        return "UNKNOWN_BIN";
    }
    int EffiCollection::getUnCorrelatedSystBin(const xAOD::Muon& mu) const {
        int Bin = 0;
        if (DoesMuonEnterBin(CollectionType::Central, mu, Bin)) return Bin;
        if (DoesMuonEnterBin(CollectionType::Calo, mu, Bin)) return Bin;
        if (DoesMuonEnterBin(CollectionType::Forward, mu, Bin)) return Bin;
        if (DoesMuonEnterBin(CollectionType::CentralLowPt, mu, Bin)) return Bin;
        if (DoesMuonEnterBin(CollectionType::CaloLowPt, mu, Bin)) return Bin;
        return Bin;
    }
    bool EffiCollection::DoesMuonEnterBin(CollectionType Type, const xAOD::Muon mu, int &Bin) const {
        EffiCollection::CollectionContainer_Ptr MuonCont = FindContainer(mu);
        EffiCollection::CollectionContainer_Ptr TypeCont = retrieveContainer(Type);
        if (!TypeCont || TypeCont != MuonCont) {
            if (TypeCont) Bin += TypeCont->nBins();
            return false;
        }
        int sfBin = TypeCont->FindBinSF(mu);
        if (sfBin < 0) Bin = sfBin;
        else Bin += sfBin;
        return true;
    }
    //################################################################################
    //                               EffiCollection::CollectionContainer
    //################################################################################
    EffiCollection::CollectionContainer::CollectionContainer(EffiCollection::CollectionType FileType, const std::string &file_name) :
                m_SF(),
                m_currentSF(),
                m_FileType(FileType),
                m_binOffSet(0){
        std::unique_ptr<TFile> fin (TFile::Open(FileName.c_str()));
        if (!fin || !fin->IsOpen()) {
            Error("CollectionContainer","Unable to open file %s", FileName.c_str());
            return;
        }
        TTree* intree = 0;
        fin->GetObject("LumiData", intree);
        
        std::map<std::string, std::pair<int,int>> map;
        // if no Lumi tree is found, we assume that the SF are not binned in time
        if (!intree) {
            map["All"] = std::pair<int,int>(1, 999999);
        } else {
            std::string *period = 0;
            unsigned int firstRun = 0;
            unsigned int lastRun = 0;
            intree->SetBranchAddress("Period", &period);
            intree->SetBranchAddress("FirstRun", &firstRun);
            intree->SetBranchAddress("LastRun", &lastRun);
            for (int t = 0; intree->GetEntry(t); t++) {
                map[*period] = std::pair<int,int>(firstRun, lastRun);
            }
        }
        fin->Close();
//         for (auto& period : map) {
//             if (!Nominal) {
//                 effi_factor = EfficiencyScaleFactor_Ptr (new EfficiencyScaleFactor(ref_asg_tool->name()+FileTypeName(m_FileType), FileName, period.first, sysType, effType, isLowPt, hasPtDepSys));
//             } else {
//                 effi_factor = EfficiencyScaleFactor_Ptr (new EfficiencyScaleFactor(Nominal->retrieve(period.second.first).get(),ref_asg_tool->name()+FileTypeName(m_FileType), FileName, period.first, sysType, effType, isLowPt, hasPtDepSys));
//             }
//             effi_factor->setFirstLastRun(period.second.first, period.second.second);
//             m_SF.push_back(effi_factor);
//         }
//         for (auto& period : m_SF)
//             period->ApplySysVariation();
    }
    bool EffiCollection::CollectionContainer::CheckConsistency() const {
        if (m_SF.empty()) {
            Error("CollectionContainer", "Could not retrieve any SFs from the input file");
            return false;
        }
        
        for (std::vector< std::unique_ptr<EfficiencyScaleFactor>>::iterator first_sf = m_SF.begin() ; first_sf != m_SF.end(); ++first_sf)  {
            if (!(*first_SF)->CheckConsistency()) return false;
            for (std::vector< std::unique_ptr<EfficiencyScaleFactor>>::iterator second_sf = m_SF.begin(); second_sf != first_sf; ++second_sf) {
                if ( (*first_SF)->coversRunNumber( (*second_SF)->firstRun()) ||(*first_SF)->coversRunNumber( (*second_SF)->lastRun()) || 
                    (*second_SF)->coversRunNumber( (*first_SF)->firstRun()) || (*second_SF)->coversRunNumber( (*first_SF)->lastRun())){
                    Error("CollectionContainer", "Overlapping periods observed in file type %s. As run %i is in period %i - %i. Please check your SF file!",  
                           FileTypeName(m_FileType),c_str(), (*first_SF)->firstRun(), (*second_SF)->firstRun(), (*second_SF)->lastRun());
                    return false;
                }
            }
        }
        return true;
    }
    bool EffiCollection::CollectionContainer::LoadPeriod(unsigned int RunNumber) const {
        if (!m_currentSF || !m_currentSF->coversRunNumber(RunNumber)) {
            for (auto& period : m_SF) {
                if (period->coversRunNumber(RunNumber)) {
                    m_currentSF = period.get();
                    return true;
                }
            }
        } else return true;
        Error("CollectionContainer", "Could not find any SF period matching the run number %u", RunNumber);
        return false;
    }
    std::shared_ptr<EfficiencyScaleFactor> EffiCollection::CollectionContainer::retrieve(unsigned int RunNumber) const {
        if (!LoadPeriod(RunNumber)) {
            return (*m_SF.begin());
        }
        return m_currentSF;
    }
    std::string EffiCollection::CollectionContainer::sysname() const {
        if (m_SF.empty()) return "";
        return (*m_SF.begin())->sysname();
    }
    EffiCollection::CollectionContainer::~CollectionContainer() {

    }
    bool EffiCollection::CollectionContainer::SetSystematicBin(unsigned int Bin) {
        for (auto& Period : m_SF) {
            if (!Period->SetSystematicBin(Bin- m_binOffSet)) {
                return false;
            }
        }
        return true;
    }
    unsigned int EffiCollection::CollectionContainer::nBins() const {
        return (*m_SF.begin())->nBinsSF();
    }
    EffiCollection::CollectionType EffiCollection::CollectionContainer::type() const {
        return m_FileType;
    }
    std::string EffiCollection::CollectionContainer::GetBinName(unsigned int Bin) const {
        return (*m_SF.begin())->GetBinName(Bin- m_binOffSet);
    }
    int EffiCollection::CollectionContainer::FindBinSF(const xAOD::Muon &mu) const {
        if (m_SF.empty() ) return -1;
        int bin =  (*m_SF.begin())->FindBinSF(mu);
        return bin > 0 ? m_binOffSet + bin : bin;
    }
    void EffiCollection::CollectionContainer::SetGlobalOffSet(unsigned int OffSet){
        m_binOffSet = OffSet;
    }
   

}
