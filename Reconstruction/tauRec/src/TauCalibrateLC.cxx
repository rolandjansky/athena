/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PathResolver/PathResolver.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

// root
#include "TFile.h"
#include "TF1.h"
#include "TH1D.h"

//tau
#include "xAODTau/TauJet.h"
#include "tauRec/TauCandidateData.h"
#include "tauRec/TauCalibrateLC.h"

using CLHEP::GeV;

/********************************************************************/
TauCalibrateLC::TauCalibrateLC(const std::string& type,
    const std::string& name,
    const IInterface* parent) :
TauToolBase(type, name, parent),
m_doEnergyCorr(false),
m_doAxisCorr(false),
m_printMissingContainerINFO(true),
m_clusterCone(0.2)  //not used
{
    declareInterface<TauToolBase > (this);
    declareProperty("tauContainerKey", tauContainerKey = "TauRecContainer");
    declareProperty("calibrationFile", calibrationFile = "EnergyCalibrationLC2012.root");
    declareProperty("vertexContainerKey", vertexContainerKey = "PrimaryVertices");
    declareProperty("doEnergyCorrection", m_doEnergyCorr);
    declareProperty("doAxisCorrection",    m_doAxisCorr);
    declareProperty("ClusterCone", m_clusterCone); //not used
}

/********************************************************************/
TauCalibrateLC::~TauCalibrateLC() {
}

/********************************************************************/
StatusCode TauCalibrateLC::initialize() {
    std::string fullPath = PathResolver::find_file(calibrationFile, "DATAPATH");
    TFile * file = TFile::Open(fullPath.c_str(), "READ");

    if (!file) {
        ATH_MSG_FATAL("Failed to open " << fullPath);
        return StatusCode::FAILURE;
    }

    // get the histogram defining eta binning
    std::string key = "etaBinning";
    TObject * obj = file->Get(key.c_str());
    if (obj) {
        etaBinHist = dynamic_cast<TH1 *> (obj);
        TH1 * tmp = const_cast<TH1*> (etaBinHist);
        tmp->SetDirectory(0);
    } else {
        ATH_MSG_FATAL("Failed to get an object with  key " << key);
        return StatusCode::FAILURE;
    }

    //retrieve number of eta bins from file
    m_nEtaBins = etaBinHist->GetNbinsX(); //member var
    if (m_nEtaBins==6)
        ATH_MSG_INFO("using 2011 tau energy calibration");
    else if (m_nEtaBins==5)
        ATH_MSG_INFO("using 2012 tau energy calibration");
    else {
        ATH_MSG_FATAL("Wrong or broken tau energy calibration file");
        return StatusCode::FAILURE;
    }
    
    // get the histogram with eta corrections
    key = "etaCorrection";
    obj = file->Get(key.c_str());
    if (obj) {
        etaCorrectionHist = dynamic_cast<TH1 *> (obj);
        TH1 * tmp = const_cast<TH1*> (etaCorrectionHist);
        tmp->SetDirectory(0);
    } else {
        ATH_MSG_FATAL("Failed to get an object with  key " << key);
        return StatusCode::FAILURE;
    }

    TString tmpSlopKey[nProngBins] = {"slopeNPV1P", "slopeNPV3P"};
    TString tmpFuncBase[nProngBins] = {"OneP_Eta_", "MultiP_Eta_"};

    for (int i = 0; i < nProngBins; i++) {
        obj = file->Get(tmpSlopKey[i]); // get pile-up slope histograms
        if (obj) {
            slopeNPVHist[i] = dynamic_cast<TH1 *> (obj);
            TH1 * tmp = const_cast<TH1*> (slopeNPVHist[i]);
            tmp->SetDirectory(0);
        } else {
            ATH_MSG_FATAL("Failed to get an object with  key " << tmpSlopKey[i]);
            return StatusCode::FAILURE;
        }

        for (int j = 0; j < m_nEtaBins; j++) {
            TString key = tmpFuncBase[i];
            key += j;
            TObject * obj = file->Get(key);
            if (obj) {
                calibFunc[i][j] = dynamic_cast<TF1*> (obj);
            } else {
                ATH_MSG_FATAL("Failed to get an object with  key " << key);
                return StatusCode::FAILURE;
            }
        }
    }
    m_averageNPV = slopeNPVHist[0]->GetBinContent(0); // underflow is the average number of reconstructed primary vertices
    m_minNTrackAtVertex = static_cast<unsigned int> (slopeNPVHist[0]->GetBinContent(slopeNPVHist[0]->GetNbinsX() + 1)); //overflow is the minimum number of tracks at vertex 

    ATH_MSG_DEBUG("averageNPV                                 = " << m_averageNPV);
    ATH_MSG_DEBUG("minimum number of tracks at primary vertex = " << m_minNTrackAtVertex);

    file->Close();

    return StatusCode::SUCCESS;
}

/********************************************************************/
StatusCode TauCalibrateLC::execute(TauCandidateData *data) 
{ 
    xAOD::TauJet *pTau = data->xAODTau;

    if (pTau == NULL) {
        ATH_MSG_ERROR("no candidate given");
        return StatusCode::FAILURE;
    }
       
    // energy calibration depends on number of tracks - 1p or Mp
    int prongBin = 1; //Mp
    if (pTau->nTracks() <= 1) prongBin = 0; //1p

    // set tau energy scale 
    if (m_doEnergyCorr) {

        // get detector axis values
	double eta = pTau->etaDetectorAxis();
        double absEta = std::abs(eta);
        int etaBin = etaBinHist->GetXaxis()->FindBin(absEta) - 1;
        
        if (etaBin>=m_nEtaBins) etaBin = m_nEtaBins-1; // correction from last bin should be applied on all taus outside stored eta range

        // get primary vertex container
        StatusCode sc;
        const xAOD::VertexContainer * vxContainer = 0;

        // for tau trigger
        bool inTrigger = false;
        if (data->hasObject("InTrigger?")) sc = data->getObject("InTrigger?", inTrigger);
        // FF: March, 2014
        // this is for later purpose. At the moment no offset correction is used in case of tau trigger (see below)
        if (sc.isSuccess() && inTrigger)   sc = data->getObject("VxPrimaryCandidate", vxContainer);
        
        if (!inTrigger || !vxContainer || sc.isFailure() ) {
            // try standard 
            if (evtStore()->retrieve(vxContainer, vertexContainerKey).isFailure() || !vxContainer) {
              if (m_printMissingContainerINFO) {
                ATH_MSG_WARNING(vertexContainerKey << " container not found --> skip TauEnergyCalibrationLC (no further info) ");
                m_printMissingContainerINFO=false;
              }
                return StatusCode::SUCCESS;
            }
        }
    
        xAOD::VertexContainer::const_iterator vx_iter = vxContainer->begin();
        xAOD::VertexContainer::const_iterator vx_end = vxContainer->end();
        int nVertex = 0;
        for (; vx_iter != vx_end; ++vx_iter) {
            if ((*vx_iter)->nTrackParticles() >= m_minNTrackAtVertex)
                ++nVertex;
        }
                
	ATH_MSG_DEBUG("calculated nVertex " << nVertex );           


        // get detector axis energy
        // was saved by TauAxisSetter
        double energyLC = pTau->p4(xAOD::TauJetParameters::DetectorAxis).E() / GeV;            //was sumClusterVector.e() / GeV;
	
        if (energyLC <= 0) {
            ATH_MSG_DEBUG("tau energy at LC scale is " << energyLC << "--> set energy=0.001");           
            //TODO: we can not set tau energy to 0 due to bug in P4Helpers during deltaR calculation
            //will set it to 0.001 MeV
	    pTau->setP4(0.001, pTau->eta(), pTau->phi(), pTau->m());
            return StatusCode::SUCCESS;
        }

        double slopeNPV = slopeNPVHist[prongBin]->GetBinContent(etaBin + 1);
        double offset = slopeNPV * (nVertex - m_averageNPV);

        // FF: March,2014
        // no offset correction for trigger        
        if (inTrigger) offset = 0.;

        if (energyLC - offset <= 0) {
            ATH_MSG_DEBUG("after pile-up correction energy would be = " << energyLC - offset << " --> setting offset=0 now!");
            offset = 0;
        }

        // apply offset correction
        double energyPileupCorr = energyLC - offset;

        double calibConst = 1.0;
        if (energyPileupCorr > 0 and energyPileupCorr < 10000) // from 0 to 10 TeV
        {
            calibConst = calibFunc[prongBin][etaBin]->Eval(energyPileupCorr);

            if (calibConst <= 0) {
                ATH_MSG_DEBUG("calibration constant = " << calibConst);
                ATH_MSG_DEBUG("prongBin             = " << prongBin);
                ATH_MSG_DEBUG("etaBin               = " << etaBin);
                ATH_MSG_DEBUG("energyPileupCorr     = " << energyPileupCorr);
                ATH_MSG_DEBUG("energyLC             = " << energyLC);
                calibConst = 1.0;
            }
        }

        double energyFinal = energyPileupCorr / calibConst;

	pTau->setP4(energyFinal * GeV / cosh( pTau->eta() ), pTau->eta(), pTau->phi(), pTau->m());

	pTau->setP4(xAOD::TauJetParameters::TauEnergyScale, pTau->pt(), pTau->eta(), pTau->phi(), pTau->m());
      
 	pTau->setDetail(xAOD::TauJetParameters::TESCalibConstant, static_cast<float>( calibConst ) );
	pTau->setDetail(xAOD::TauJetParameters::TESOffset, static_cast<float>( offset * GeV ) );
      
        ATH_MSG_DEBUG("Energy at LC scale = " << energyLC << " pile-up offset " << offset << " calib. const. = " << calibConst << " final energy = " << energyFinal);
    }
    
    // final tau axis
    if (m_doAxisCorr) {

        // get tau intermediate axis values
        double eta = pTau->eta();
        double absEta = std::abs(eta);
        double etaCorr = eta;

        if (absEta)
            etaCorr = (eta / absEta)*(absEta - etaCorrectionHist->GetBinContent(etaCorrectionHist->GetXaxis()->FindBin(absEta)));

        ATH_MSG_DEBUG("eta " << eta << "; corrected eta = " << etaCorr);

	pTau->setP4( pTau->e() / cosh( etaCorr ), etaCorr, pTau->phi(), pTau->m());

	pTau->setP4(xAOD::TauJetParameters::TauEtaCalib, pTau->pt(), pTau->eta(), pTau->phi(), pTau->m());
     
    }

    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalize
//-----------------------------------------------------------------------------

StatusCode TauCalibrateLC::finalize() {
    return StatusCode::SUCCESS;
}
