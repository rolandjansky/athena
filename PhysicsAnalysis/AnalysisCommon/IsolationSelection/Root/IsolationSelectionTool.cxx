/*
 Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include <xAODPrimitives/IsolationType.h>
#include <IsolationSelection/IsolationSelectionTool.h>
#include <IsolationSelection/IsolationConditionHist.h>
#include <IsolationSelection/IsolationConditionFormula.h>
#include <IsolationSelection/Interp3D.h>
#include "PathResolver/PathResolver.h"
#include <TROOT.h>
#include <TFile.h>
#include <TObjString.h>
#include <TH3.h>

namespace CP {
    IsolationSelectionTool::IsolationSelectionTool(const std::string& name) :
                asg::AsgTool(name),
                /// input file
                m_calibFile(nullptr),
                m_photonAcceptInfo("IsolationSelectionToolPhotonAccept"),
                m_electronAcceptInfo("IsolationSelectionToolElectronAccept"),
                m_muonAcceptInfo("IsolationSelectionToolMuonAccept"),
                m_objAcceptInfo("IsolationSelectionToolObjAccept"),
                m_iparWPs(nullptr),
                m_iparAcceptInfo(nullptr),
                m_Interp(nullptr),
                m_TwikiLoc("https://twiki.cern.ch/twiki/bin/view/AtlasProtected/IsolationSelectionTool#List_of_current_official_working") {
        declareProperty("CalibFileName", m_calibFileName = "IsolationSelection/v2/MC15_Z_Jpsi_cutMap.root", "The config file to use");
        declareProperty("MuonWP", m_muWPname = "Undefined", "Working point for muon");
        declareProperty("ElectronWP", m_elWPname = "Undefined", "Working point for electron");
        declareProperty("PhotonWP", m_phWPname = "Undefined", "Working point for photon");
        declareProperty("MuonKey", m_muWPKey = "/Muons/DFCommonGoodMuon/mu_cutValues_", "path of the cut map for muon");
        declareProperty("ElectronKey", m_elWPKey = "/ElectronPhoton/LHTight/el_cutValues_", "path of the cut map for electron");
        declareProperty("PhotonKey", m_phWPKey = "/ElectronPhoton/LHTight/el_cutValues_", "path of the cut map for photon");

        declareProperty("doCutInterpolationMuon", m_doInterpM = false, "flag to perform cut interpolation, muon");
        declareProperty("doCutInterpolationElec", m_doInterpE = true, "flag to perform cut interpolation, electron");
    }
    void IsolationSelectionTool::clearWPs(std::vector<IsolationWP*>& WP) {
        for (auto& c : WP) {
            if (c) delete c;
        }
        WP.clear();
    }

    void IsolationSelectionTool::clearPhotonWPs() {
        clearWPs(m_phWPs);
    }
    void IsolationSelectionTool::clearElectronWPs() {
        clearWPs(m_elWPs);
    }
    void IsolationSelectionTool::clearMuonWPs() {
        clearWPs(m_muWPs);
    }
    void IsolationSelectionTool::clearObjWPs() {
        clearWPs(m_objWPs);
    }
    const std::vector<IsolationWP*>& IsolationSelectionTool::getMuonWPs() const {
        return m_muWPs;
    }
    const std::vector<IsolationWP*>& IsolationSelectionTool::getElectronWPs() const {
        return m_elWPs;
    }
    const std::vector<IsolationWP*>& IsolationSelectionTool::getPhotonWPs() const {
        return m_phWPs;
    }
    const std::vector<IsolationWP*>& IsolationSelectionTool::getObjWPs() const {
        return m_objWPs;
    }
    IsolationSelectionTool::~IsolationSelectionTool() {
        clearMuonWPs();
        clearPhotonWPs();
        clearElectronWPs();
        clearObjWPs();
    }

    StatusCode IsolationSelectionTool::initialize() {
        /// Greet the user:
        ATH_MSG_INFO("Initialising...");
        std::string filename = PathResolverFindCalibFile(m_calibFileName);

        ATH_MSG_INFO("Reading input file " << m_calibFileName << " from " << filename);
        m_calibFile = std::shared_ptr < TFile > (new TFile(filename.c_str(), "READ"));

        TObjString* versionInfo = (TObjString*) m_calibFile->Get("VersionInfo");
        if (versionInfo) ATH_MSG_INFO("VersionInfo:" << versionInfo->String());
        else ATH_MSG_WARNING("VersionInfo of input file (" << filename << ") is missing.");

        if (m_doInterpE || m_doInterpM) {
            // special setting for electrons
            // do not apply interpolation in crack vicinity for topoetcone
            std::vector<std::pair<double, double>> rangeEtaNoInt;
            std::pair<double, double> apair(1.26, 1.665);
            rangeEtaNoInt.push_back(apair);
            // do not apply interpolation between Z defined and J/Psi defined cuts (pT < > 15 GeV/c) for both calo and track iso
            std::vector<std::pair<double, double>> rangePtNoInt;
            apair.first = 12.5;
            apair.second = 17.5;
            rangePtNoInt.push_back(apair);
            std::map<std::string, Interp3D::VetoInterp> amap;
            Interp3D::VetoInterp veto;
            veto.xRange = rangePtNoInt;
            veto.yRange = std::vector<std::pair<double, double>>();
            amap.insert(std::make_pair(std::string("el_cutValues_ptvarcone20"), veto));
            veto.yRange = rangeEtaNoInt;
            amap.insert(std::make_pair(std::string("el_cutValues_topoetcone20"), veto));
            m_Interp = new Interp3D(amap);
            m_Interp->debug(false);
            //m_Interp->debug();
        }

        /// setup working points
        if (m_phWPname != "Undefined") ATH_CHECK(addPhotonWP(m_phWPname));
        if (m_elWPname != "Undefined") ATH_CHECK(addElectronWP(m_elWPname));
        if (m_muWPname != "Undefined") ATH_CHECK(addMuonWP(m_muWPname));

        /// Return gracefully:
        return StatusCode::SUCCESS;
    }

    StatusCode IsolationSelectionTool::setIParticleCutsFrom(xAOD::Type::ObjectType ObjType) {
        if (ObjType == xAOD::Type::Electron) {
            m_iparAcceptInfo = &m_electronAcceptInfo;
            m_iparWPs = &m_elWPs;
        } else if (ObjType == xAOD::Type::Muon) {
            m_iparAcceptInfo = &m_muonAcceptInfo;
            m_iparWPs = &m_muWPs;
        } else if (ObjType == xAOD::Type::Photon) {
            m_iparAcceptInfo = &m_photonAcceptInfo;
            m_iparWPs = &m_phWPs;
        } else {
            return StatusCode::FAILURE;
        }

        return StatusCode::SUCCESS;
    }

    StatusCode IsolationSelectionTool::finalize() {
        /// Return gracefully:
        return StatusCode::SUCCESS;
    }

    void IsolationSelectionTool::addCutToWP(IsolationWP* wp, std::string key, const xAOD::Iso::IsolationType t, const std::string expression) {
        std::string varname(xAOD::Iso::toCString(t));
        key += varname;

        // Stupid ROOT, this is silly, why do I need to do this????? why????
        // Create a unique directory in memory to hold the histograms:
        TDirectory* tempDir = getTemporaryDirectory();
        tempDir->cd();

        std::shared_ptr<TH3F> histogram((TH3F*) m_calibFile->Get(key.c_str())->Clone());
        IsolationConditionHist *ich = new IsolationConditionHist(varname, t, expression, histogram);
        if ((m_doInterpM && key.find("Muon") != std::string::npos) || (m_doInterpE && key.find("Electron") != std::string::npos)) ich->setInterp(m_Interp);
        wp->addCut(ich);
    }

    StatusCode IsolationSelectionTool::addMuonWP(std::string muWPname) {
        IsolationWP* wp = new IsolationWP(muWPname);
        // For flat efficiency in (pT,eta)
        if (muWPname == "LooseTrackOnly") {
            addCutToWP(wp, m_muWPKey, xAOD::Iso::ptvarcone30, "99");
        } else if (muWPname == "Loose") {
            addCutToWP(wp, m_muWPKey, xAOD::Iso::ptvarcone30, "99");
            //addCutToWP(wp, m_muWPKey, xAOD::Iso::topoetcone20, "99");
        } else if (muWPname == "Tight") {
            ATH_MSG_WARNING("Obsolete muon WP " << muWPname << ". Check Twiki " << m_TwikiLoc);
            addCutToWP(wp, m_muWPKey, xAOD::Iso::ptvarcone30, "99");
            //addCutToWP(wp, m_muWPKey, xAOD::Iso::topoetcone20, "96");
            // For gradient efficiency in pT
        } else if (muWPname == "Gradient") {
            addCutToWP(wp, m_muWPKey, xAOD::Iso::ptvarcone30, "0.1143*x+92.14");
            //addCutToWP(wp, m_muWPKey, xAOD::Iso::topoetcone20, "0.1143*x+92.14");
        } else if (muWPname == "GradientLoose") {
            addCutToWP(wp, m_muWPKey, xAOD::Iso::ptvarcone30, "0.057*x+95.57");
            ///addCutToWP(wp, m_muWPKey, xAOD::Iso::topoetcone20, "0.057*x+95.57");
        } else if (muWPname == "GradientT1") {
            addCutToWP(wp, m_muWPKey, xAOD::Iso::ptvarcone30, "0.1713*x+88.71");
            //addCutToWP(wp, m_muWPKey, xAOD::Iso::topoetcone20, "0.1713*x+88.71");
            ATH_MSG_WARNING("Obsolete muon WP " << muWPname << ". Check Twiki " << m_TwikiLoc);
        } else if (muWPname == "GradientT2") {
            addCutToWP(wp, m_muWPKey, xAOD::Iso::ptvarcone30, "0.2283*x+85.28");
            //addCutToWP(wp, m_muWPKey, xAOD::Iso::topoetcone20, "0.2283*x+85.28");
            ATH_MSG_WARNING("Obsolete muon WP " << muWPname << ". Check Twiki " << m_TwikiLoc);
            // Using fixed cuts
        } else if (muWPname == "FixedCutTightTrackOnly") {
            wp->addCut(new IsolationConditionFormula("ptvarcone30R0p06", xAOD::Iso::ptvarcone30, "0.06*x"));
        } else if (muWPname == "FixedCutLoose") {
            wp->addCut(new IsolationConditionFormula("MuonFixedCutLoose_track", xAOD::Iso::ptvarcone30, "0.15*x"));
            wp->addCut(new IsolationConditionFormula("MuonFixedCutLoose_calo", xAOD::Iso::topoetcone20, "0.30*x"));
        } else {
            ATH_MSG_ERROR("Unknown muon isolation WP: " << muWPname);
            delete wp;
            return StatusCode::FAILURE;
        }
        m_muWPs.push_back(wp);
        m_muonAcceptInfo.addCut(wp->name(), wp->name());
        return StatusCode::SUCCESS;
    }

    StatusCode IsolationSelectionTool::addPhotonWP(std::string phWPname) {
        auto wp = new IsolationWP(phWPname);
        // The old ones (kept here for backward compatibility for the time being)
        if (phWPname == "Cone40CaloOnly") {
            wp->addCut(new IsolationConditionFormula("topoetcone40", xAOD::Iso::topoetcone40, "0.024*x+2900"));
            ATH_MSG_WARNING("Obsolete photon WP " << phWPname << ". Check Twiki " << m_TwikiLoc);
        } else if (phWPname == "Cone40") {
            wp->addCut(new IsolationConditionFormula("Cone40_calo", xAOD::Iso::topoetcone40, "0.028*x+3400"));
            wp->addCut(new IsolationConditionFormula("Cone40_track", xAOD::Iso::ptcone40, "0.12*x"));
            ATH_MSG_WARNING("Obsolete photon WP " << phWPname << ". Check Twiki " << m_TwikiLoc);
        } else if (phWPname == "Cone20") {
            wp->addCut(new IsolationConditionFormula("Cone20_calo", xAOD::Iso::topoetcone20, "0.015*x+1500"));
            wp->addCut(new IsolationConditionFormula("Cone20_track", xAOD::Iso::ptcone20, "0.04*x"));
            ATH_MSG_WARNING("Obsolete photon WP " << phWPname << ". Check Twiki " << m_TwikiLoc);
            // The new ones, 08/10/2015
        } else if (phWPname == "FixedCutTightCaloOnly") {
            wp->addCut(new IsolationConditionFormula("PhFixedCut_calo40", xAOD::Iso::topoetcone40, "0.022*x+2450"));
        } else if (phWPname == "FixedCutTight") {
            wp->addCut(new IsolationConditionFormula("PhFixedCut_calo40", xAOD::Iso::topoetcone40, "0.022*x+2450"));
            wp->addCut(new IsolationConditionFormula("PhFixedCut_track20", xAOD::Iso::ptcone20, "0.05*x"));
        } else if (phWPname == "FixedCutLoose") {
            wp->addCut(new IsolationConditionFormula("PhFixedCut_calo20", xAOD::Iso::topoetcone20, "0.065*x"));
            wp->addCut(new IsolationConditionFormula("PhFixedCut_track20", xAOD::Iso::ptcone20, "0.05*x"));
        } else {
            ATH_MSG_ERROR("Unknown photon isolation WP: " << phWPname);
            delete wp;
            return StatusCode::FAILURE;
        }

        m_phWPs.push_back(wp);
        m_photonAcceptInfo.addCut(wp->name(), wp->name());

        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    StatusCode IsolationSelectionTool::addElectronWP(std::string elWPname) {
        auto wp = new IsolationWP(elWPname);

        // For flat efficiency in (pT,eta)
        if (elWPname == "LooseTrackOnly") {
            addCutToWP(wp, m_elWPKey, xAOD::Iso::ptvarcone20, "99");
        } else if (elWPname == "Loose") {
            addCutToWP(wp, m_elWPKey, xAOD::Iso::ptvarcone20, "99");
            addCutToWP(wp, m_elWPKey, xAOD::Iso::topoetcone20, "99");
        } else if (elWPname == "Tight") {
            ATH_MSG_WARNING("Obsolete electron WP " << elWPname << ". Check Twiki " << m_TwikiLoc);
            addCutToWP(wp, m_elWPKey, xAOD::Iso::ptvarcone20, "99");
            addCutToWP(wp, m_elWPKey, xAOD::Iso::topoetcone20, "96");
            // For gradient efficiency in pT
        } else if (elWPname == "Gradient") {
            addCutToWP(wp, m_elWPKey, xAOD::Iso::ptvarcone20, "0.1143*x+92.14");
            addCutToWP(wp, m_elWPKey, xAOD::Iso::topoetcone20, "0.1143*x+92.14");
        } else if (elWPname == "GradientLoose") {
            addCutToWP(wp, m_elWPKey, xAOD::Iso::ptvarcone20, "0.057*x+95.57");
            addCutToWP(wp, m_elWPKey, xAOD::Iso::topoetcone20, "0.057*x+95.57");
        } else if (elWPname == "GradientT1") {
            addCutToWP(wp, m_elWPKey, xAOD::Iso::ptvarcone20, "0.1713*x+88.71");
            addCutToWP(wp, m_elWPKey, xAOD::Iso::topoetcone20, "0.1713*x+88.71");
            ATH_MSG_WARNING("Obsolete electron WP " << elWPname << ". Check Twiki " << m_TwikiLoc);
        } else if (elWPname == "GradientT2") {
            addCutToWP(wp, m_elWPKey, xAOD::Iso::ptvarcone20, "0.2283*x+85.28");
            addCutToWP(wp, m_elWPKey, xAOD::Iso::topoetcone20, "0.2283*x+85.28");
            ATH_MSG_WARNING("Obsolete electron WP " << elWPname << ". Check Twiki " << m_TwikiLoc);
            // Using fixed cuts
        } else if (elWPname == "FixedCutTight") {
            wp->addCut(new IsolationConditionFormula("ptvarcone20R0p06", xAOD::Iso::ptvarcone20, "0.06*x"));
            wp->addCut(new IsolationConditionFormula("topoetcone20R0p06", xAOD::Iso::topoetcone20, "0.06*x"));
        } else if (elWPname == "FixedCutTightTrackOnly") {
            wp->addCut(new IsolationConditionFormula("ptvarcone20R0p06", xAOD::Iso::ptvarcone20, "0.06*x"));
        } else if (elWPname == "FixedCutLoose") {
            wp->addCut(new IsolationConditionFormula("FixedCutLoose_track", xAOD::Iso::ptvarcone20, "0.15*x"));
            wp->addCut(new IsolationConditionFormula("FixedCutLoose_calo", xAOD::Iso::topoetcone20, "0.20*x"));
        } else {
            ATH_MSG_ERROR("Unknown electron isolation WP: " << elWPname);
            delete wp;
            return StatusCode::FAILURE;
        }

        m_elWPs.push_back(wp);
        m_electronAcceptInfo.addCut(wp->name(), wp->name());

        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    StatusCode IsolationSelectionTool::addUserDefinedWP(std::string WPname, xAOD::Type::ObjectType ObjType, 
                                                        std::vector<std::pair<xAOD::Iso::IsolationType, 
                                                        std::string> >& cuts, std::string key, IsoWPType type) {
        std::vector<IsolationWP*>* wps(nullptr);
        asg::AcceptInfo* ac(nullptr);
        if (ObjType == xAOD::Type::Electron) {
            if (key == "") key = m_elWPKey;
            wps = &m_elWPs;
            ac = &m_electronAcceptInfo;
        } else if (ObjType == xAOD::Type::Muon) {
            if (key == "") key = m_muWPKey;
            wps = &m_muWPs;
            ac = &m_muonAcceptInfo;
        } else if (ObjType == xAOD::Type::Photon) {
            if (key == "") key = m_phWPKey;
            wps = &m_phWPs;
            ac = &m_photonAcceptInfo;
        } else if (ObjType == xAOD::Type::Other) {
            if (key == "") return StatusCode::FAILURE;
            wps = &m_objWPs;
            ac = &m_objAcceptInfo;
        } else {
            return StatusCode::FAILURE;
        }

        auto wp = new IsolationWP(WPname);
        if (type == Efficiency) {
            for (auto& c : cuts)
                addCutToWP(wp, key, c.first, c.second);
        } else if (type == Cut) {
            for (auto& c : cuts)
                wp->addCut(new IsolationConditionFormula(xAOD::Iso::toCString(c.first), c.first, c.second));
        } else {
            ATH_MSG_ERROR("Unknown isolation WP type -- should not happen.");
            delete wp;
            return StatusCode::FAILURE;
        }
        wps->push_back(wp);
        ac->addCut(wp->name(), wp->name());
        return StatusCode::SUCCESS;
    }

    StatusCode IsolationSelectionTool::addWP(std::string WP, xAOD::Type::ObjectType ObjType) {
        if (ObjType == xAOD::Type::Electron) {
            return addElectronWP(WP);
        } else if (ObjType == xAOD::Type::Muon) {
            return addMuonWP(WP);
        } else if (ObjType == xAOD::Type::Photon) {
            return addPhotonWP(WP);
        }

        return StatusCode::FAILURE;
    }
    StatusCode IsolationSelectionTool::addWP(IsolationWP* wp, xAOD::Type::ObjectType ObjType) {
        if (ObjType == xAOD::Type::Electron) {
            m_elWPs.push_back(wp);
            m_electronAcceptInfo.addCut(wp->name(), wp->name());
        } else if (ObjType == xAOD::Type::Muon) {
            m_muWPs.push_back(wp);
            m_muonAcceptInfo.addCut(wp->name(), wp->name());
        } else if (ObjType == xAOD::Type::Photon) {
            m_phWPs.push_back(wp);
            m_photonAcceptInfo.addCut(wp->name(), wp->name());
        } else if (ObjType == xAOD::Type::Other) {
            m_objWPs.push_back(wp);
            m_objAcceptInfo.addCut(wp->name(), wp->name());
        } else {
            return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
    }
    
    template<typename T> void IsolationSelectionTool::evaluateWP(const T& x, 
                                                                 const std::vector<IsolationWP*>& WP, 
                                                                 asg::AcceptData& accept) const {
        accept.clear();
        for (auto& i : WP) {
            if (i->accept(x)) {
              accept.setCutResult(i->name(), true);
            }
        }
    }

    const asg::AcceptData IsolationSelectionTool::accept(const xAOD::Photon& x) const {
      asg::AcceptData acceptData(&m_photonAcceptInfo);
      evaluateWP(x, m_phWPs, acceptData);
      return acceptData;
    }

    const asg::AcceptData IsolationSelectionTool::accept(const xAOD::Electron& x) const {
      asg::AcceptData acceptData(&m_electronAcceptInfo);  
      evaluateWP(x, m_elWPs, acceptData);
      return acceptData;
    }

    const asg::AcceptData IsolationSelectionTool::accept(const xAOD::Muon& x) const {
      asg::AcceptData acceptData(&m_muonAcceptInfo);  
      evaluateWP(x, m_muWPs, acceptData);
      return acceptData;
    }

    const asg::AcceptData IsolationSelectionTool::accept(const xAOD::IParticle& x) const {

      if (x.type() == xAOD::Type::Electron) {
        const xAOD::Electron& el = static_cast<const xAOD::Electron&> (x);  
        return accept(el);
      } else if (x.type() == xAOD::Type::Muon) {
        const xAOD::Muon& muon = static_cast<const xAOD::Muon&> (x);
        return accept(muon);
      } else if (x.type() == xAOD::Type::Photon) {
        const xAOD::Photon& ph = static_cast<const xAOD::Photon&> (x);
        return accept(ph);  
      }
      else if (m_iparAcceptInfo && m_iparWPs) {
        asg::AcceptData acceptData(m_iparAcceptInfo);
        evaluateWP(x, *m_iparWPs, acceptData);
        return acceptData;
      }
      ATH_MSG_ERROR("Someting here makes really no  sense");
      return asg::AcceptData(&m_objAcceptInfo);
    }

    const asg::AcceptData IsolationSelectionTool::accept(const strObj& x) const {
      if (x.type == xAOD::Type::Electron) {
        asg::AcceptData acceptData(&m_electronAcceptInfo);  
        evaluateWP(x, m_elWPs, acceptData);
        return acceptData;
      } else if (x.type == xAOD::Type::Muon) {
        asg::AcceptData acceptData(&m_muonAcceptInfo); 
        evaluateWP(x, m_muWPs, acceptData);
        return acceptData;
      } else if (x.type == xAOD::Type::Photon) {
        asg::AcceptData acceptData(&m_photonAcceptInfo); 
        evaluateWP(x, m_phWPs, acceptData);
        return acceptData;
      } 
      asg::AcceptData acceptData(&m_objAcceptInfo);
      evaluateWP(x, m_objWPs, acceptData);
      return acceptData;
    }

    const asg::AcceptInfo& IsolationSelectionTool::getPhotonAcceptInfo() const {
        return m_photonAcceptInfo;
    }

    const asg::AcceptInfo& IsolationSelectionTool::getElectronAcceptInfo() const {
        return m_electronAcceptInfo;
    }

    const asg::AcceptInfo& IsolationSelectionTool::getMuonAcceptInfo() const {
        return m_muonAcceptInfo;
    }
    const asg::AcceptInfo& IsolationSelectionTool::getObjAcceptInfo() const {
        return m_objAcceptInfo;
    }

    TDirectory* IsolationSelectionTool::getTemporaryDirectory(void) const {
        //
        // Create a unique directory in memory to hold the histograms:
        //
        gROOT->cd();
        TDirectory* tempDir = 0;
        int counter = 0;
        while (not tempDir) {
            // First, let's find a directory name that doesn't exist yet:
            std::stringstream dirname;
            dirname << "IsolationSelectionTempDir_%i" << counter;
            if (gROOT->GetDirectory((dirname.str()).c_str())) {
                ++counter;
                continue;
            }
            // Let's try to make this directory:
            tempDir = gROOT->mkdir((dirname.str()).c_str());
            if (not tempDir) {
                //      ::Fatal("IsolationSelectionTool::getTemporaryDirectory",
                //      ERROR_MESSAGE("Temporary directory could not be created"));
                throw std::runtime_error("Temporary directory could not be created");
            }
        }

        return tempDir;
    }
}
