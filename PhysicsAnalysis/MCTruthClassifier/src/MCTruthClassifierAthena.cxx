/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

// File Including the Athena only methods of the MCTruthClassifier Class

#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE) // Can only be used in Athena

//
#include "MCTruthClassifier/MCTruthClassifier.h"
//
// xAOD EDM includes
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
//
// Athena only includes
#include "AthenaKernel/Units.h"
#include "GeneratorObjects/xAODTruthParticleLink.h"
#include "AtlasHepMC/GenParticle.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
//
// std includes
#include <cmath>

using Athena::Units::GeV;
using namespace MCTruthPartClassifier;
using std::abs;

namespace {

bool
EtaPhiCaloHelper(const Trk::CaloExtension* caloExtension,
                 const CaloSampling::CaloSample sample,
                 double& etaCalo,
                 double& phiCalo)
{
  bool isOK = ((caloExtension != nullptr) && (!caloExtension->caloLayerIntersections().empty()));

  if (isOK) {
    etaCalo = -99;
    phiCalo = -99;
    Trk::TrackParametersIdHelper parsIdHelper;

    // loop over calo layers
    for (auto cur : caloExtension->caloLayerIntersections()) {

      // only use entry layer
      if (!parsIdHelper.isEntryToVolume(cur->cIdentifier())) {
        continue;
      }

      CaloSampling::CaloSample sampleEx = parsIdHelper.caloSample(cur->cIdentifier());
      if (sampleEx != CaloSampling::EMB2 && sampleEx != CaloSampling::EME2 && sampleEx != CaloSampling::FCAL2) {
        continue;
      }

      if (sampleEx == sample || etaCalo == -99) {
        etaCalo = cur->position().eta();
        phiCalo = cur->position().phi();
        if (sampleEx == sample)
          break;
      }
    }
  }
  return isOK;
}

}

// Methods using directly the extrapolator usable only from Athena
//-----------------------------------------------------------------------------------------
const xAOD::TruthParticle*
MCTruthClassifier::egammaClusMatch(const xAOD::CaloCluster* clus, bool isFwrdEle, Info* info) const
{
  //-----------------------------------------------------------------------------------------

  ATH_MSG_DEBUG("Executing egammaClusMatch ");

  const xAOD::TruthParticle* theMatchPart = nullptr;

  // retrieve collection and get a pointer
  SG::ReadHandle<xAOD::TruthParticleContainer> truthParticleContainerReadHandle(m_truthParticleContainerKey);

  if (!truthParticleContainerReadHandle.isValid()) {
    ATH_MSG_WARNING(
      " Invalid ReadHandle for xAOD::TruthParticleContainer with key: " << truthParticleContainerReadHandle.key());
    return theMatchPart;
  }

  ATH_MSG_DEBUG("xAODTruthParticleContainer with key  " << truthParticleContainerReadHandle.key()
                                                        << " has valid ReadHandle ");

  const xAOD::TruthParticle* theEgamma(nullptr);
  const xAOD::TruthParticle* theLeadingPartInCone(nullptr);
  const xAOD::TruthParticle* theBestPartOutCone(nullptr);
  const xAOD::TruthParticle* theBestPartdR(nullptr);
  double LeadingPhtPT(0);

  double LeadingPartPT(0);
  double LeadingPhtdR(999.);

  double LeadingPartdR(999.);

  double BestPartdR(999.);

  double etaClus = clus->etaBE(2);
  double phiClus = clus->phiBE(2);
  if (etaClus < -900)
    etaClus = clus->eta();
  if (phiClus < -900)
    phiClus = clus->phi();

  std::vector<const xAOD::TruthParticle*> tps;
  if (!m_truthInConeTool->particlesInCone(etaClus, phiClus, 0.5, tps)) {
    ATH_MSG_WARNING("Truth Particle in Cone failed");
    return theMatchPart;
  }

  for (const auto thePart : tps) {
    // loop over the stable particle
    if (thePart->status() != 1)
      continue;
    // excluding G4 particle
    if (thePart->barcode() >= m_barcodeG4Shift)
      continue;
    long iParticlePDG = thePart->pdgId();
    // excluding neutrino
    if (abs(iParticlePDG) == 12 || abs(iParticlePDG) == 14 || abs(iParticlePDG) == 16)
      continue;

    double pt = thePart->pt() / GeV;
    double q = partCharge(thePart);
    // exclude charged particles with pT<1 GeV
    if (q != 0 && pt < m_pTChargePartCut)
      continue;
    if (q == 0 && pt < m_pTNeutralPartCut)
      continue;

    // eleptical cone  for extrapolations m_partExtrConePhi X m_partExtrConeEta
    if (!isFwrdEle && m_ROICone &&
        std::pow((detPhi(phiClus, thePart->phi()) / m_partExtrConePhi), 2) +
            std::pow((detEta(etaClus, thePart->eta()) / m_partExtrConeEta), 2) >
          1.0)
      continue;

    // Also check if the clus and true have different sign , i they need both to be <0 or >0
    if (isFwrdEle &&                                 // It is forward and
        (((etaClus < 0) - (thePart->eta() < 0) != 0) // The truth eta has different sign wrt to the fwd electron
         || (std::fabs(thePart->eta()) < m_FwdElectronTruthExtrEtaCut) // or the truth is less than 2.4 (default cut)
         || (std::fabs(thePart->eta() - etaClus) >
             m_FwdElectronTruthExtrEtaWindowCut) // or if the delta Eta between el and truth is  > 0.15
         )                                       // then do no extrapolate this truth Particle for this fwd electron
    )
      continue;

    double dR(-999.);
    bool isNCone = false;
    bool isExt = genPartToCalo(clus, thePart, isFwrdEle, dR, isNCone, info->extrapolationCache);
    if (!isExt)
      continue;

    theMatchPart = barcode_to_particle(truthParticleContainerReadHandle.ptr(), thePart->barcode() % m_barcodeShift);

    if (info) {
      info->egPartPtr.push_back(thePart);
      info->egPartdR.push_back(dR);
      info->egPartClas.push_back(particleTruthClassifier(theMatchPart));
    }

    // Gen particles
    // Not forward
    if (!isFwrdEle) {
      // the leading photon or electron  inside narrow eleptical cone m_phtClasConePhi  X m_phtClasConeEta
      if ((iParticlePDG == 22 || abs(iParticlePDG) == 11) && isNCone && pt > LeadingPhtPT) {
        theEgamma = thePart;
        LeadingPhtPT = pt;
        LeadingPhtdR = dR;
      }
      // leading particle (excluding photon and electron) inside narrow eleptic cone m_phtClasConePhi  X
      // m_phtClasConeEta
      if ((iParticlePDG != 22 && abs(iParticlePDG) != 11) && isNCone && pt > LeadingPartPT) {
        theLeadingPartInCone = thePart;
        LeadingPartPT = pt;
        LeadingPartdR = dR;
      };
      // the best dR matched particle outside  narrow eleptic cone cone m_phtClasConePhi  X m_phtClasConeEta
      if (!isNCone && dR < BestPartdR) {
        theBestPartOutCone = thePart;
        BestPartdR = dR;
      };
    } else {
      if (dR < BestPartdR) {
        theBestPartdR = thePart;
        BestPartdR = dR;
      };
    }
  } // end cycle for Gen particle

  if (theEgamma != nullptr) {
    theMatchPart = barcode_to_particle(truthParticleContainerReadHandle.ptr(), theEgamma->barcode() % m_barcodeShift);
    if (info)
      info->deltaRMatch = LeadingPhtdR;
  } else if (theLeadingPartInCone != nullptr) {
    theMatchPart =
      barcode_to_particle(truthParticleContainerReadHandle.ptr(), theLeadingPartInCone->barcode() % m_barcodeShift);
    if (info)
      info->deltaRMatch = LeadingPartdR;
  } else if (theBestPartOutCone != nullptr) {
    theMatchPart =
      barcode_to_particle(truthParticleContainerReadHandle.ptr(), theBestPartOutCone->barcode() % m_barcodeShift);
    if (info)
      info->deltaRMatch = BestPartdR;
  } else if (isFwrdEle && theBestPartdR != nullptr) {
    theMatchPart =
      barcode_to_particle(truthParticleContainerReadHandle.ptr(), theBestPartdR->barcode() % m_barcodeShift);
    if (info)
      info->deltaRMatch = BestPartdR;
  } else {
    theMatchPart = nullptr;
  }
  if (isFwrdEle || theMatchPart != nullptr || !m_inclG4part) {
    return theMatchPart;
  }

  // additional loop over G4 particles,
  for (const auto thePart : tps) {
    // loop over the stable particle
    if (thePart->status() != 1)
      continue;
    // only G4 particle including None Primary with barcode > 10^6
    if (thePart->barcode() < m_barcodeG4Shift)
      continue;
    long iParticlePDG = thePart->pdgId();
    // exclude neutrino
    if (abs(iParticlePDG) == 12 || abs(iParticlePDG) == 14 || abs(iParticlePDG) == 16)
      continue;
    // exclude particles interacting into the detector volume
    if (thePart->decayVtx() != nullptr)
      continue;

    if (std::pow((detPhi(phiClus, thePart->phi()) / m_partExtrConePhi), 2) +
          std::pow((detEta(etaClus, thePart->eta()) / m_partExtrConeEta), 2) >
        1.0)
      continue;

    double pt = thePart->pt() / GeV;
    double q = partCharge(thePart);
    // exclude charged particles with pT<1 GeV
    if (q != 0 && pt < m_pTChargePartCut)
      continue;
    if (q == 0 && pt < m_pTNeutralPartCut)
      continue;

    double dR(-999.);
    bool isNCone = false;
    bool isExt = genPartToCalo(clus, thePart, isFwrdEle, dR, isNCone, info->extrapolationCache);
    if (!isExt)
      continue;

    theMatchPart = barcode_to_particle(truthParticleContainerReadHandle.ptr(), thePart->barcode() % m_barcodeShift);

    if (info) {
      info->egPartPtr.push_back(thePart);
      info->egPartdR.push_back(dR);
      info->egPartClas.push_back(particleTruthClassifier(theMatchPart));
    }

    // the leading photon or electron  inside narrow eleptical cone m_phtClasConePhi  X m_phtClasConeEta
    if ((iParticlePDG == 22 || abs(iParticlePDG) == 11) && isNCone && pt > LeadingPhtPT) {
      theEgamma = thePart;
      LeadingPhtPT = pt;
      LeadingPhtdR = dR;
    }

    // leading particle (excluding photon or electron) inside narrow eleptic cone m_phtClasConePhi  X m_phtClasConeEta
    if ((iParticlePDG != 22 && abs(iParticlePDG) != 11) && isNCone && pt > LeadingPartPT) {
      theLeadingPartInCone = thePart;
      LeadingPartPT = pt;
      LeadingPartdR = dR;
    };
    // the best dR matched particle outside  narrow eleptic cone cone m_phtClasConePhi  X m_phtClasConeEta
    if (!isNCone && dR < BestPartdR) {
      theBestPartOutCone = thePart;
      BestPartdR = dR;
    };
  } // end cycle for G4 particle

  if (theEgamma != nullptr) {
    theMatchPart = barcode_to_particle(truthParticleContainerReadHandle.ptr(), theEgamma->barcode() % m_barcodeShift);
    if (info)
      info->deltaRMatch = LeadingPhtdR;
  } else if (theLeadingPartInCone != nullptr) {
    theMatchPart =
      barcode_to_particle(truthParticleContainerReadHandle.ptr(), theLeadingPartInCone->barcode() % m_barcodeShift);
    if (info)
      info->deltaRMatch = LeadingPartdR;
  } else if (theBestPartOutCone != nullptr) {
    theMatchPart =
      barcode_to_particle(truthParticleContainerReadHandle.ptr(), theBestPartOutCone->barcode() % m_barcodeShift);
    if (info)
      info->deltaRMatch = BestPartdR;
  } else {
    theMatchPart = nullptr;
  }

  ATH_MSG_DEBUG("succeeded  egammaClusMatch ");
  return theMatchPart;
}

//--------------------------------------------------------------
bool
MCTruthClassifier::genPartToCalo(const xAOD::CaloCluster* clus,
                                 const xAOD::TruthParticle* thePart,
                                 bool isFwrdEle,
                                 double& dRmatch,
                                 bool& isNarrowCone,
                                 Cache* cache) const
{
  dRmatch = -999.;
  isNarrowCone = false;

  if (thePart == nullptr)
    return false;

  // define calo sample
  CaloSampling::CaloSample sample = CaloSampling::EMB2;
  if ((clus->inBarrel() && !clus->inEndcap()) ||
      (clus->inBarrel() && clus->inEndcap() &&
       clus->eSample(CaloSampling::EMB2) >= clus->eSample(CaloSampling::EME2))) {
    // Barrel
    sample = CaloSampling::EMB2;

  } else if ((!clus->inBarrel() && clus->inEndcap() && !isFwrdEle) ||
             (clus->inBarrel() && clus->inEndcap() &&
              clus->eSample(CaloSampling::EME2) > clus->eSample(CaloSampling::EMB2))) {
    // End-cap
    sample = CaloSampling::EME2;
  } else if (isFwrdEle && clus->inEndcap()) {
    // FCAL
    sample = CaloSampling::FCAL2;

  } else
    return false;

  double etaCalo = -99;
  double phiCalo = -99;
  bool extensionOK = false;
  if (cache) {
    const Trk::CaloExtension* caloExtension = m_caloExtensionTool->caloExtension(*thePart, *cache);
    extensionOK = EtaPhiCaloHelper(caloExtension, sample, etaCalo, phiCalo);
  } else {
    std::unique_ptr<Trk::CaloExtension> caloExtension = m_caloExtensionTool->caloExtension(*thePart);
    extensionOK = EtaPhiCaloHelper(caloExtension.get(), sample, etaCalo, phiCalo);
  }
  if (!extensionOK) {
    ATH_MSG_WARNING("extrapolation of Truth Particle with eta  " << thePart->eta() << " and Pt " << thePart->pt()
                                                                 << " to calo failed");
    return false;
  }

  double phiClus = clus->phiBE(2);
  double etaClus = clus->etaBE(2);
  if (etaClus < -900)
    etaClus = clus->eta();
  if (phiClus < -900)
    phiClus = clus->phi();

  //--FixMe
  if (isFwrdEle || (etaClus == 0. && phiClus == 0.)) {
    phiClus = clus->phi();
    etaClus = clus->eta();
  }
  double dPhi = detPhi(phiCalo, phiClus);
  double dEta = detEta(etaCalo, etaClus);
  dRmatch = rCone(dPhi, dEta);

  if ((!isFwrdEle && dRmatch > m_phtdRtoTrCut) || (isFwrdEle && dRmatch > m_fwrdEledRtoTrCut))
    return false;

  if (!isFwrdEle && std::pow(dPhi / m_phtClasConePhi, 2) + std::pow(dEta / m_phtClasConeEta, 2) <= 1.0)
    isNarrowCone = true;

  return true;
}
// End of methods using directly the extrapolator usable only from Athena
#endif
