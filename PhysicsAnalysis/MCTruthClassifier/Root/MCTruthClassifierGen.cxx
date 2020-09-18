/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * Implementation file that mainly contains the code logic
 * dealing with Truth - record classification
 */

#include "MCTruthClassifier/MCTruthClassifier.h"
#include "AsgDataHandles/ReadHandle.h"
using namespace MCTruthPartClassifier;
using std::abs;

#ifndef XAOD_ANALYSIS
std::pair<ParticleType, ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(HepMC::ConstGenParticlePtr thePart, Info* info /*= nullptr*/) const
{
  //---------------------------------------------------------------------------------------
  ParticleType partType = Unknown;
  ParticleOrigin partOrig = NonDefined;

  if (!thePart)
    return std::make_pair(partType, partOrig);

  // Retrieve the links between HepMC and xAOD::TruthParticle
  SG::ReadHandle<xAODTruthParticleLinkVector> truthParticleLinkVecReadHandle(m_truthLinkVecReadHandleKey);
  if (!truthParticleLinkVecReadHandle.isValid()) {
    ATH_MSG_WARNING(
      " Invalid ReadHandle for xAODTruthParticleLinkVector with key: " << truthParticleLinkVecReadHandle.key());
    return std::make_pair(partType, partOrig);
  }

  for (const auto& entry : *truthParticleLinkVecReadHandle) {
    if (entry->first.isValid() && entry->second.isValid() && HepMC::barcode(entry->first.cptr()) == HepMC::barcode(thePart)) {
      const xAOD::TruthParticle* truthParticle = *entry->second;
      if (!compareTruthParticles(thePart, truthParticle)) {
        // if the barcode/pdg id / status of the pair does not match
        // return default
        return std::make_pair(partType, partOrig);
      }
      return particleTruthClassifier(truthParticle, info);
    }
  }

  return std::make_pair(partType, partOrig);
}
//------------------------------------------------------------------------
bool
MCTruthClassifier::compareTruthParticles(HepMC::ConstGenParticlePtr genPart, const xAOD::TruthParticle* truthPart) const
{
  //------------------------------------------------------------------------
  if (!genPart || !truthPart)
    return false;

  if (HepMC::barcode(genPart) != truthPart->barcode() || genPart->pdg_id() != truthPart->pdgId() ||
      genPart->status() != truthPart->status()) {
    ATH_MSG_DEBUG("HepMC::GenParticle and xAOD::TruthParticle do not match");
    return false;
  }

  return true;
}
#endif

//---------------------------------------------------------------------------------------
std::pair<ParticleType, ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::TruthParticle* thePart, Info* info /*= nullptr*/) const
{
  //---------------------------------------------------------------------------------------

  ATH_MSG_DEBUG("Executing particleTruthClassifier");

  ParticleType partType = Unknown;
  ParticleOrigin partOrig = NonDefined;

  if (info) {
    info->genPart = thePart;
  }

  // retrieve collection and get a pointer
  if (!thePart)
    return std::make_pair(partType, partOrig);

  SG::ReadHandle<xAOD::TruthParticleContainer> truthParticleContainerReadHandle(m_truthParticleContainerKey);

  if (!truthParticleContainerReadHandle.isValid()) {
    ATH_MSG_WARNING(
      " Invalid ReadHandle for xAOD::TruthParticleContainer with key: " << truthParticleContainerReadHandle.key());
    return std::make_pair(partType, partOrig);
  }

  ATH_MSG_DEBUG("xAODTruthParticleContainer with key  " << truthParticleContainerReadHandle.key()
                                                        << " has valid ReadHandle ");

  int iParticlePDG = thePart->pdgId();
  // status=10902 in Pythia?
  if (thePart->status() != 1 && thePart->status() != 2 && thePart->status() != 10902) {
    return std::make_pair(GenParticle, partOrig);
  }
  bool isPartHadr = isHadron(thePart);
  if (thePart->status() == 2 && (!MC::PID::isTau(iParticlePDG) && !isPartHadr)) {
    return std::make_pair(GenParticle, partOrig);
  }

  // SUSY datasets: tau(satus==2)->tau(satus==2)
  if (thePart->status() == 2 && MC::PID::isTau(iParticlePDG)) {
    const xAOD::TruthVertex* endVert = thePart->decayVtx();
    if (endVert != nullptr) {
      int numOfDaught = endVert->nOutgoingParticles();
      if (numOfDaught == 1 && abs(endVert->outgoingParticle(0)->pdgId()) == 15) {
        return std::make_pair(GenParticle, partOrig);
      }
    }
  }

  if (thePart->status() == 1 && MC::PID::isSUSY(iParticlePDG)) {
    return std::make_pair(SUSYParticle, partOrig);
  }

  if (thePart->status() == 1 && MC::PID::isBSM(iParticlePDG)) {
    return std::make_pair(OtherBSMParticle, partOrig);
  }

  if (thePart->status() == 10902 &&
      (abs(iParticlePDG) != 11 && abs(iParticlePDG) != 13 && abs(iParticlePDG) != 15 && abs(iParticlePDG) != 22) &&
      !isPartHadr) {
    return std::make_pair(GenParticle, partOrig);
  }

  if (abs(iParticlePDG) > 1000000000) {
    return std::make_pair(NuclFrag, partOrig);
  }
  // Note: MC::PID::isLepton includes BSM leptons
  if (abs(iParticlePDG) != 11 && abs(iParticlePDG) != 13 && abs(iParticlePDG) != 15 && abs(iParticlePDG) != 12 &&
      abs(iParticlePDG) != 14 && abs(iParticlePDG) != 16 && !MC::PID::isPhoton(iParticlePDG) && !isPartHadr) {
    return std::make_pair(partType, partOrig);
  }
  // don't consider  generator particles

  const xAOD::TruthVertex* partOriVert = thePart->hasProdVtx() ? thePart->prodVtx() : nullptr;

  long motherBarcode = 0;
  int motherStatus = 0;
  int motherPDG = 0;
  if (partOriVert != nullptr) {
    for (unsigned int ipIn = 0; ipIn < partOriVert->nIncomingParticles(); ++ipIn) {
      const xAOD::TruthParticle* theMoth = partOriVert->incomingParticle(ipIn);
      if (!theMoth)
        continue;
      motherPDG = theMoth->pdgId();
      motherStatus = theMoth->status();
      motherBarcode = theMoth->barcode();
      if (info) {
        info->motherPDG = motherPDG;
        info->motherBarcode = motherBarcode;
      }
    }
  }

  if (partOriVert == nullptr && thePart->barcode() > m_barcodeShift) {
    return std::make_pair(NonPrimary, partOrig);
  }
  if (partOriVert == nullptr && MC::PID::isElectron(iParticlePDG)) {
    // to define electron out come  status
    bool isPrompt = false;
    partOrig = defOrigOfElectron(truthParticleContainerReadHandle.ptr(), thePart, isPrompt, info);
    return std::make_pair(UnknownElectron, partOrig);
  } else if (partOriVert == nullptr && MC::PID::isMuon(iParticlePDG)) {
    // to define electron out come  status
    bool isPrompt = false;
    partOrig = defOrigOfMuon(truthParticleContainerReadHandle.ptr(), thePart, isPrompt, info);
    return std::make_pair(UnknownMuon, partOrig);
  } else if (partOriVert == nullptr && MC::PID::isTau(iParticlePDG)) {
    // to define electron out come  status
    partOrig = defOrigOfTau(truthParticleContainerReadHandle.ptr(), thePart, motherPDG, info);
    return std::make_pair(UnknownTau, partOrig);
  } else if (partOriVert == nullptr && MC::PID::isPhoton(iParticlePDG)) {
    // to define photon out come
    bool isPrompt = false;
    partOrig = defOrigOfPhoton(truthParticleContainerReadHandle.ptr(), thePart, isPrompt, info);
    return std::make_pair(UnknownPhoton, partOrig);
  } else if (partOriVert == nullptr && MC::PID::isNeutrino(iParticlePDG)) {
    // to define neutrino outcome
    if (info)
      info->particleOutCome = NonInteract;
    return std::make_pair(Neutrino, partOrig);
  }

  if (thePart->barcode() % m_barcodeShift == motherBarcode % m_barcodeShift)
    return std::make_pair(NonPrimary, partOrig);

  if (isPartHadr)
    return std::make_pair(Hadron, partOrig);

  if (partOriVert != nullptr && motherPDG == 0 && partOriVert->nOutgoingParticles() == 1 &&
      partOriVert->nIncomingParticles() == 0) {
    if (MC::PID::isElectron(iParticlePDG)) {
      if (info)
        info->particleOutCome = defOutComeOfElectron(thePart);
      return std::make_pair(IsoElectron, SingleElec);
    }
    if (MC::PID::isMuon(iParticlePDG)) {
      if (info)
        info->particleOutCome = defOutComeOfMuon(thePart);
      return std::make_pair(IsoMuon, SingleMuon);
    }
    if (MC::PID::isTau(iParticlePDG)) {
      if (info)
        info->particleOutCome = defOutComeOfTau(thePart, info);
      return std::make_pair(IsoTau, SingleTau);
    }
    if (MC::PID::isPhoton(iParticlePDG)) {
      if (info)
        info->particleOutCome = defOutComeOfPhoton(thePart);
      return std::make_pair(IsoPhoton, SinglePhot);
    }
  }

  if (motherPDG == iParticlePDG && motherStatus == 3 && thePart->status() == 10902)
    return std::make_pair(GenParticle, partOrig);

  if (MC::PID::isElectron(iParticlePDG)) {
    bool isPrompt = false;
    partOrig = defOrigOfElectron(truthParticleContainerReadHandle.ptr(), thePart, isPrompt, info);
    partType = defTypeOfElectron(partOrig, isPrompt);
  } else if (MC::PID::isMuon(iParticlePDG)) {
    bool isPrompt = false;
    partOrig = defOrigOfMuon(truthParticleContainerReadHandle.ptr(), thePart, isPrompt, info);
    partType = defTypeOfMuon(partOrig, isPrompt);
  } else if (MC::PID::isTau(iParticlePDG)) {
    partOrig = defOrigOfTau(truthParticleContainerReadHandle.ptr(), thePart, motherPDG, info);
    partType = defTypeOfTau(partOrig);
  } else if (MC::PID::isPhoton(iParticlePDG)) {
    bool isPrompt = false;
    partOrig = defOrigOfPhoton(truthParticleContainerReadHandle.ptr(), thePart, isPrompt, info);
    partType = defTypeOfPhoton(partOrig);
  } else if (MC::PID::isNeutrino(iParticlePDG)) {
    bool isPrompt = false;
    partOrig = defOrigOfNeutrino(truthParticleContainerReadHandle.ptr(), thePart, isPrompt, info);
    partType = Neutrino;
  }

  ATH_MSG_DEBUG("particleTruthClassifier  succeeded ");
  return std::make_pair(partType, partOrig);
}

//---------------------------------------------------------------------------------
ParticleOrigin
MCTruthClassifier::defJetOrig(const std::set<const xAOD::TruthParticle*>& allJetMothers) const
{
  //---------------------------------------------------------------------------------
  ParticleOrigin partOrig = NonDefined;
  std::set<const xAOD::TruthParticle*>::iterator it;

  for (it = allJetMothers.begin(); it != allJetMothers.end(); ++it) {
    int pdg = abs((*it)->pdgId());
    if (MC::PID::isTop(pdg)) {
      partOrig = top;
    }
    if (MC::PID::isZ(pdg)) {
      partOrig = ZBoson;
    }
    if (MC::PID::isW(pdg) && !(partOrig == top)) {
      partOrig = WBoson;
    }
    if ((pdg < 6 || MC::PID::isGluon(pdg)) && !(partOrig == top || partOrig == ZBoson || partOrig == WBoson)) {
      partOrig = QCD;
    }
    if (MC::PID::isHiggs(pdg)) {
      partOrig = Higgs;
      return partOrig;
    }
    if (pdg == 35 || pdg == 36 || pdg == 37) {
      partOrig = HiggsMSSM;
      return partOrig;
    }
    if (pdg == 32 || pdg == 33 || pdg == 34) {
      partOrig = HeavyBoson;
      return partOrig;
    }
    if (pdg == 42) {
      partOrig = LQ;
      return partOrig;
    }
    if (MC::PID::isSUSY(pdg)) {
      partOrig = SUSY;
      return partOrig;
    }
    if (MC::PID::isBSM(pdg)) {
      partOrig = OtherBSM;
      return partOrig;
    }
  }
  return partOrig;
}
//---------------------------------------------------------------------------------------------------------
void
MCTruthClassifier::findAllJetMothers(const xAOD::TruthParticle* thePart,
                                     std::set<const xAOD::TruthParticle*>& allJetMothers) const
{
  //---------------------------------------------------------------------------------------------------------
  const xAOD::TruthVertex* partOriVert = thePart->hasProdVtx() ? thePart->prodVtx() : nullptr;

  if (partOriVert != nullptr) {
    for (unsigned int ipIn = 0; ipIn < partOriVert->nIncomingParticles(); ipIn++) {
      const xAOD::TruthParticle* theMoth = partOriVert->incomingParticle(ipIn);
      if (!theMoth)
        continue;
      allJetMothers.insert(theMoth);
      findAllJetMothers(theMoth, allJetMothers);
    }
  }
}
//-------------------------------------------------------------------------------
ParticleType
MCTruthClassifier::defTypeOfElectron(ParticleOrigin EleOrig, bool isPrompt) const
{
  //-------------------------------------------------------------------------------

  if (EleOrig == NonDefined)
    return UnknownElectron;

  if (EleOrig == WBoson || EleOrig == ZBoson || EleOrig == top || EleOrig == SingleElec || EleOrig == Higgs ||
      EleOrig == HiggsMSSM || EleOrig == HeavyBoson || EleOrig == WBosonLRSM || EleOrig == NuREle || EleOrig == NuRMu ||
      EleOrig == NuRTau || EleOrig == LQ || EleOrig == SUSY || EleOrig == DiBoson || EleOrig == ZorHeavyBoson ||
      EleOrig == OtherBSM || EleOrig == MultiBoson || isPrompt) {
    return IsoElectron;
  }
  if (EleOrig == JPsi || EleOrig == BottomMeson || EleOrig == CharmedMeson || EleOrig == BottomBaryon ||
      EleOrig == CharmedBaryon || EleOrig == TauLep || EleOrig == Mu || EleOrig == QuarkWeakDec) {
    return NonIsoElectron;
  }
  return BkgElectron;
}

//-------------------------------------------------------------------------------
ParticleOrigin
MCTruthClassifier::defOrigOfElectron(const xAOD::TruthParticleContainer* mcTruthTES,
                                     const xAOD::TruthParticle* thePart,
                                     bool& isPrompt,
                                     Info* info) const
//-------------------------------------------------------------------------------
{

  ATH_MSG_DEBUG("Executing DefOrigOfElectron ");

  int PriBarcode = thePart->barcode() % m_barcodeShift;
  const xAOD::TruthParticle* thePriPart = barcode_to_particle(mcTruthTES, PriBarcode);
  if (!thePriPart)
    return NonDefined;
  if (abs(thePriPart->pdgId()) != 11)
    return NonDefined;

  const xAOD::TruthVertex* partOriVert = thePriPart->hasProdVtx() ? thePriPart->prodVtx() : nullptr;

  //-- to define electron outcome status
  if (info)
    info->particleOutCome = defOutComeOfElectron(thePriPart);

  if (!partOriVert)
    return NonDefined;

  int numOfParents = -1;
  numOfParents = partOriVert->nIncomingParticles();
  if (numOfParents > 1)
    ATH_MSG_DEBUG("DefOrigOfElectron:: electron  has more than one mother ");

  const xAOD::TruthParticle* mother = getMother(thePriPart);
  if (info)
    info->mother = mother;
  if (!mother) {
    return NonDefined;
  }
  int motherPDG = mother->pdgId();
  long motherBarcode = mother->barcode();
  if (info) {
    info->motherPDG = motherPDG;
    info->motherBarcode = motherBarcode;
  }
  const xAOD::TruthVertex* mothOriVert = mother->hasProdVtx() ? mother->prodVtx() : nullptr;

  //---patch to fix LQ dataset problem
  if (m_LQpatch) {
    if (abs(motherPDG) == 11 && mother->status() == 2 && mothOriVert == nullptr && motherPDG == thePriPart->pdgId() &&
        numOfParents == 1 && (partOriVert->nOutgoingParticles() == 1 || partOriVert->nOutgoingParticles() == 2)) {
      const xAOD::TruthParticle* theP(nullptr);
      int itr = 0;
      float dRMin(99999.);
      do {
        const xAOD::TruthParticle* thePtmp = barcode_to_particle(mcTruthTES, motherBarcode - (itr + 2));
        if (abs(thePtmp->pdgId()) == 11 && thePtmp->pdgId() == motherPDG && thePtmp->status() == 3 &&
            thePtmp->decayVtx() == nullptr) {
          float dphi = detPhi(mother->phi(), thePtmp->phi());
          float deta = mother->eta() - thePtmp->eta();
          float dR = rCone(dphi, deta);
          if (dR < dRMin) {
            dRMin = dR;
            theP = thePtmp;
          };
        }
        itr++;
      } while (itr < 4);

      if (dRMin > 0.1)
        theP = nullptr;
      if (theP != nullptr) {
        thePriPart = theP;
        mother = getMother(thePriPart);
        if (info) {
          info->mother = mother;
        }
        if (!mother) {
          return NonDefined;
        }
        motherPDG = mother->pdgId();
        motherBarcode = mother->barcode();
        mothOriVert = mother->hasProdVtx() ? mother->prodVtx() : nullptr;
        if (info) {
          info->motherPDG = motherPDG;
          info->motherBarcode = motherBarcode;
        }
      }
    }
  }
  //----

  // to exclude interactions mu(barcode<10^6)->mu(barcode10^6)+e
  bool samePart = false;
  for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles(); ++ipOut) {
    const xAOD::TruthParticle* theDaug = partOriVert->outgoingParticle(ipOut);
    if (!theDaug)
      continue;
    if (motherPDG == theDaug->pdgId() && motherBarcode % m_barcodeShift == theDaug->barcode() % m_barcodeShift)
      samePart = true;
  }

  // to resolve Sherpa loop
  if (mothOriVert && mothOriVert->barcode() == partOriVert->barcode())
    samePart = true;
  //

  if ((abs(motherPDG) == 13 || abs(motherPDG) == 15 || abs(motherPDG) == 24) && mothOriVert != nullptr && !samePart) {
    long pPDG(0);
    const xAOD::TruthParticle* MotherParent(nullptr);
    do {
      pPDG = 0;
      MotherParent = getMother(mother);
      // to prevent Sherpa loop
      const xAOD::TruthVertex* mother_prdVtx(nullptr);
      const xAOD::TruthVertex* mother_endVtx(nullptr);
      if (mother) {
        mother_prdVtx = mother->hasProdVtx() ? mother->prodVtx() : nullptr;
        mother_endVtx = mother->decayVtx();
      }
      const xAOD::TruthVertex* parent_prdVtx(nullptr);
      const xAOD::TruthVertex* parent_endVtx(nullptr);
      if (MotherParent) {
        parent_prdVtx = MotherParent->hasProdVtx() ? MotherParent->prodVtx() : nullptr;
        parent_endVtx = MotherParent->decayVtx();
      }
      if (mother_endVtx == parent_prdVtx && mother_prdVtx == parent_endVtx) {
        MotherParent = mother;
        break;
      }
      //
      if (MotherParent)
        pPDG = MotherParent->pdgId();
      // to prevent Sherpa loop
      if (mother == MotherParent)
        break;

      if (abs(pPDG) == 13 || abs(pPDG) == 15 || abs(pPDG) == 24)
        mother = MotherParent;

    } while ((abs(pPDG) == 13 || abs(pPDG) == 15 || abs(pPDG) == 24));

    if (abs(pPDG) == 13 || abs(pPDG) == 15 || abs(pPDG) == 24 || abs(pPDG) == 23 || abs(pPDG) == 25 ||
        abs(pPDG) == 35 || abs(pPDG) == 36 || abs(pPDG) == 37 || abs(pPDG) == 32 || abs(pPDG) == 33 ||
        abs(pPDG) == 34 || abs(pPDG) == 6 || abs(pPDG) == 9900024 || abs(pPDG) == 9900012 || abs(pPDG) == 9900014 ||
        abs(pPDG) == 9900016 || (abs(pPDG) < 2000040 && abs(pPDG) > 1000001))
      mother = MotherParent;
  }

  motherPDG = mother->pdgId();
  motherBarcode = mother->barcode();
  partOriVert = mother->decayVtx();
  mothOriVert = mother->hasProdVtx() ? mother->prodVtx() : nullptr;
  numOfParents = partOriVert->nIncomingParticles();
  int numOfDaug = partOriVert->nOutgoingParticles();

  if (info) {
    info->motherPDG = motherPDG;
    info->motherBarcode = motherBarcode;
  }

  int NumOfPhot(0);

  int NumOfEl(0);

  int NumOfPos(0);

  int NumOfNucFr(0);
  int NumOfquark(0);

  int NumOfgluon(0);

  int NumOfElNeut(0);

  int NumOfLQ(0);
  int NumOfMuPl(0);

  int NumOfMuMin(0);

  int NumOfMuNeut(0);
  int NumOfTau(0);

  int NumOfTauNeut(0);
  long DaugType(0);
  samePart = false;

  for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles(); ++ipOut) {
    const xAOD::TruthParticle* theDaug = partOriVert->outgoingParticle(ipOut);
    if (!theDaug)
      continue;
    DaugType = theDaug->pdgId();
    if (abs(DaugType) < 7)
      NumOfquark++;
    if (abs(DaugType) == 21)
      NumOfgluon++;
    if (abs(DaugType) == 12)
      NumOfElNeut++;
    if (abs(DaugType) == 14)
      NumOfMuNeut++;
    if (DaugType == 22)
      NumOfPhot++;
    if (DaugType == 11)
      NumOfEl++;
    if (DaugType == -11)
      NumOfPos++;
    if (DaugType == 13)
      NumOfMuMin++;
    if (DaugType == -13)
      NumOfMuPl++;
    if (abs(DaugType) == 15)
      NumOfTau++;
    if (abs(DaugType) == 16)
      NumOfTauNeut++;

    if (abs(DaugType) == 42)
      NumOfLQ++;
    if (abs(DaugType) == abs(motherPDG) && theDaug->barcode() % m_barcodeShift == motherBarcode % m_barcodeShift)
      samePart = true;
    if (numOfParents == 1 &&
        (motherPDG == 22 || abs(motherPDG) == 11 || abs(motherPDG) == 13 || abs(motherPDG) == 211) &&
        (DaugType > 1000000000 || DaugType == 0 || DaugType == 2212 || DaugType == 2112 || abs(DaugType) == 211 ||
         abs(DaugType) == 111))
      NumOfNucFr++;
  } // cycle itrDaug

  if (motherPDG == 22 && mothOriVert != nullptr) {
    // get mother of photon
    for (unsigned int ipIn = 0; ipIn < mothOriVert->nIncomingParticles(); ++ipIn) {
      const xAOD::TruthParticle* theMother = mothOriVert->incomingParticle(ipIn);
      if (!theMother)
        continue;
      if (info) {
        info->photonMother = theMother;
        info->photonMotherStatus = theMother->status();
        info->photonMotherBarcode = theMother->barcode();
        info->photonMotherPDG = theMother->pdgId();
      }
    }
  }

  // if ( numOfParents == 0 && numOfDaug == 1 )  return  SingleElec;
  if ((motherPDG == 22 && numOfDaug == 2 && NumOfEl == 1 && NumOfPos == 1) ||
      (motherPDG == 22 && numOfDaug == 1 && (NumOfEl == 1 || NumOfPos == 1))) {
    return PhotonConv;
  }

  // e,gamma,pi+Nuclear->NuclearFragments+nuclons+e
  if ((numOfParents == 1 && (abs(motherPDG) == 22 || abs(motherPDG) == 11 || abs(motherPDG) == 15)) && numOfDaug > 1 &&
      NumOfNucFr != 0)
    return ElMagProc;

  if (numOfParents == 1 && abs(motherPDG) == 211 && numOfDaug > 2 && NumOfNucFr != 0)
    return ElMagProc;

  // nuclear photo fission
  if (motherPDG == 22 && numOfDaug > 4 && NumOfNucFr != 0)
    return ElMagProc;

  // unknown process el(pos)->el+pos??
  if (abs(motherPDG) == 11 && numOfDaug == 2 && NumOfEl == 1 && NumOfPos == 1)
    return ElMagProc;

  // unknown process el->el+el??
  if (motherPDG == 11 && numOfDaug == 2 && NumOfEl == 2 && NumOfPos == 0)
    return ElMagProc;

  // unknown process pos->pos+pos??
  if (motherPDG == -11 && numOfDaug == 2 && NumOfEl == 0 && NumOfPos == 2)
    return ElMagProc;

  // unknown process pos/el->pos/el??
  if (abs(motherPDG) == 11 && mother->status() != 2 && motherPDG == thePriPart->pdgId() && numOfDaug == 1 && !samePart)
    return ElMagProc;

  // pi->pi+e+/e-; mu->mu+e+/e- ;
  // gamma+ atom->gamma(the same) + e (compton scattering)
  if (numOfDaug == 2 && (NumOfEl == 1 || NumOfPos == 1) && abs(motherPDG) != 11 && samePart)
    return ElMagProc;

  if ((motherPDG == 111 && numOfDaug == 3 && NumOfPhot == 1 && NumOfEl == 1 && NumOfPos == 1) ||
      (motherPDG == 111 && numOfDaug == 4 && NumOfPhot == 0 && NumOfEl == 2 && NumOfPos == 2))
    return DalitzDec;

  // Quark weak decay
  if (abs(motherPDG) < 7 && numOfParents == 1 && numOfDaug == 3 && NumOfquark == 1 && NumOfElNeut == 1)
    return QuarkWeakDec;

  if (abs(motherPDG) == 13 && NumOfNucFr != 0)
    return ElMagProc;

  if (abs(motherPDG) == 6)
    return top;

  if (abs(motherPDG) == 24 && mothOriVert != nullptr && mothOriVert->nIncomingParticles() != 0) {

    const xAOD::TruthVertex* prodVert = mothOriVert;
    const xAOD::TruthParticle* ptrPart;
    do {
      ptrPart = prodVert->incomingParticle(0);
      prodVert = ptrPart->hasProdVtx() ? ptrPart->prodVtx() : nullptr;
    } while (abs(ptrPart->pdgId()) == 24 && prodVert != nullptr);

    if (prodVert && prodVert->nIncomingParticles() == 1 && abs(ptrPart->pdgId()) == 9900012)
      return NuREle;
    else if (prodVert && prodVert->nIncomingParticles() == 1 && abs(ptrPart->pdgId()) == 9900014)
      return NuRMu;
    else if (prodVert && prodVert->nIncomingParticles() == 1 && abs(ptrPart->pdgId()) == 9900016)
      return NuRTau;
    return WBoson;
  } else if (abs(motherPDG) == 24)
    return WBoson;

  if (abs(motherPDG) == 23)
    return ZBoson;

  //-- Exotics
  if (abs(motherPDG) < 7 && numOfParents == 2 && numOfDaug == 2 && NumOfEl == 1 && NumOfPos == 1 &&
      partOriVert->barcode() == -5) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    const xAOD::TruthVertex* TmpVert = partOriVert->outgoingParticle(0)->decayVtx();

    if (abs(pdg1) == abs(pdg2) && TmpVert->nIncomingParticles() == 2)
      return ZorHeavyBoson;
  }

  if (abs(motherPDG) < 7 && numOfParents == 2 && numOfDaug == 2 && (NumOfEl == 1 || NumOfPos == 1) &&
      NumOfElNeut == 1 && partOriVert->barcode() == -5) {
    const xAOD::TruthVertex* TmpVert = partOriVert->outgoingParticle(0)->decayVtx();
    if (TmpVert->nIncomingParticles() == 2)
      return HeavyBoson;
  }

  // MadGraphPythia ZWW*->lllnulnu
  if (numOfParents == 1 && numOfDaug > 4 && (abs(motherPDG) < 7 || motherPDG == 21)) {

    const xAOD::TruthParticle* thePartToCheck = thePriPart;
    const xAOD::TruthParticle* theMother =
      thePriPart->hasProdVtx() ? thePriPart->prodVtx()->incomingParticle(0) : nullptr;
    if (theMother != nullptr && abs(theMother->pdgId()) == 11 && theMother->status() == 2)
      thePartToCheck = theMother;

    bool isZboson = false;
    bool isWboson = false;
    bool skipnext = false;

    for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles() - 1; ++ipOut) {
      const xAOD::TruthParticle* theDaug = partOriVert->outgoingParticle(ipOut);
      if (!theDaug)
        continue;
      const xAOD::TruthParticle* theNextDaug = nullptr;
      for (unsigned int ipOut1 = ipOut + 1; ipOut1 < partOriVert->nOutgoingParticles(); ipOut1++) {
        theNextDaug = partOriVert->outgoingParticle(ipOut1);
        if (theNextDaug != nullptr)
          break;
      }
      if (!theNextDaug)
        continue;
      if (skipnext) {
        skipnext = false;
        continue;
      }

      if (abs(theDaug->pdgId()) == 11 && abs(theNextDaug->pdgId()) == 11) {
        // Zboson
        if (thePartToCheck == theDaug || thePartToCheck == theNextDaug) {
          isZboson = true;
          break;
        }
        skipnext = true;
      } else if (abs(theDaug->pdgId()) == 11 && abs(theNextDaug->pdgId()) == 12) {
        // WBoson
        if (thePartToCheck == theDaug || thePartToCheck == theNextDaug) {
          isWboson = true;
          break;
        }
        skipnext = true;
      }
    }
    if (isWboson)
      return WBoson;
    if (isZboson)
      return ZBoson;
  }

  //--Sherpa Z->ee
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon) == 2 && NumOfEl == 1 && NumOfPos == 1)
    return ZBoson;

  //--Sherpa W->enu ??
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon) == 2 && (NumOfEl == 1 || NumOfPos == 1) &&
      NumOfElNeut == 1)
    return WBoson;

  //--Sherpa ZZ,ZW
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon) == 4 &&
      (NumOfEl + NumOfPos + NumOfMuPl + NumOfMuMin + NumOfTau + NumOfElNeut + NumOfMuNeut + NumOfTauNeut == 4)) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    if ((abs(pdg1) == 21 && abs(pdg2) == 21) || (abs(pdg1) < 7 && abs(pdg2) < 7) || (pdg1 == 21 && abs(pdg2) < 7) ||
        (pdg2 == 21 && abs(pdg1) < 7))
      return DiBoson;
  }

  //--Sherpa VVV -- Note, have to allow for prompt photon radiation or these get lost
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon - NumOfPhot) == 6 &&
      (NumOfEl + NumOfPos + NumOfMuPl + NumOfMuMin + NumOfTau + NumOfElNeut + NumOfMuNeut + NumOfTauNeut == 6)) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    if ((abs(pdg1) == 21 && abs(pdg2) == 21) || (abs(pdg1) < 7 && abs(pdg2) < 7) || (pdg1 == 21 && abs(pdg2) < 7) ||
        (pdg2 == 21 && abs(pdg1) < 7))
      return MultiBoson;
  }

  // New Sherpa Z->ee
  if (partOriVert == mothOriVert && partOriVert != nullptr) {
    int NumOfEleLoop = 0;
    int NumOfLepLoop = 0;
    int NumOfEleNeuLoop = 0;
    for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles(); ipOut++) {
      for (unsigned int ipIn = 0; ipIn < partOriVert->nIncomingParticles(); ipIn++) {
        if (!partOriVert->outgoingParticle(ipOut))
          continue;
        if (!partOriVert->incomingParticle(ipIn))
          continue;
        if (partOriVert->outgoingParticle(ipOut)->barcode() == partOriVert->incomingParticle(ipIn)->barcode()) {
          if (fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 11)
            NumOfEleLoop++;
          if (fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 12)
            NumOfEleNeuLoop++;
          if (fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 11 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 12 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 13 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 14 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 15 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 16)
            NumOfLepLoop++;
        }
      }
    }
    if (NumOfEleLoop == 2 && NumOfEleNeuLoop == 0)
      return ZBoson;
    if (NumOfEleLoop == 1 && NumOfEleNeuLoop == 1)
      return WBoson;
    if ((NumOfEleLoop == 4 && NumOfEleNeuLoop == 0) || (NumOfEleLoop == 3 && NumOfEleNeuLoop == 1) ||
        (NumOfEleLoop == 2 && NumOfEleNeuLoop == 2))
      return DiBoson;
    if (NumOfLepLoop == 4)
      return DiBoson;
  }

  //-- McAtNLo
  if (abs(motherPDG) < 7 && numOfParents == 2 && NumOfEl == 1 && NumOfPos == 1 && partOriVert->barcode() == -1) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    if (abs(pdg1) == abs(pdg2))
      return ZBoson;
  }

  if (abs(motherPDG) == 25)
    return Higgs;

  if (abs(motherPDG) == 35 || abs(motherPDG) == 36 || abs(motherPDG) == 37)
    return HiggsMSSM;

  if (abs(motherPDG) == 32 || abs(motherPDG) == 33 || abs(motherPDG) == 34)
    return HeavyBoson;

  if (abs(motherPDG) == 13)
    return Mu;
  if (abs(motherPDG) == 15) {

    ParticleOrigin tauOrig = defOrigOfTau(mcTruthTES, mother, motherPDG, info);
    ParticleType tautype = defTypeOfTau(tauOrig);
    if (tautype == IsoTau)
      return tauOrig;
    else
      return TauLep;
  }

  if (abs(motherPDG) == 9900024)
    return WBosonLRSM;
  if (abs(motherPDG) == 9900012)
    return NuREle;
  if (abs(motherPDG) == 9900014)
    return NuRMu;
  if (abs(motherPDG) == 9900016)
    return NuRTau;

  if (abs(motherPDG) == 42 || NumOfLQ != 0)
    return LQ;

  if (MC::PID::isSUSY(motherPDG))
    return SUSY;

  if (MC::PID::isBSM(motherPDG))
    return OtherBSM;

  ParticleType pType = defTypeOfHadron(motherPDG);
  if ((pType == BBbarMesonPart || pType == CCbarMesonPart) && mothOriVert != nullptr && isHardScatVrtx(mothOriVert))
    isPrompt = true;

  //----
  return convHadronTypeToOrig(pType, motherPDG);
}
//-------------------------------------------------------------------------------
ParticleType
MCTruthClassifier::defTypeOfMuon(ParticleOrigin MuOrig, bool isPrompt) const
{
  //-------------------------------------------------------------------------------

  if (MuOrig == NonDefined)
    return UnknownMuon;

  if (MuOrig == WBoson || MuOrig == ZBoson || MuOrig == top || MuOrig == SingleMuon || MuOrig == Higgs ||
      MuOrig == HiggsMSSM || MuOrig == HeavyBoson || MuOrig == WBosonLRSM || MuOrig == NuREle || MuOrig == NuRMu ||
      MuOrig == NuRTau || MuOrig == LQ || MuOrig == SUSY || MuOrig == DiBoson || MuOrig == ZorHeavyBoson ||
      MuOrig == OtherBSM || MuOrig == MultiBoson || isPrompt) {
    return IsoMuon;
  }
  if (MuOrig == JPsi || MuOrig == BottomMeson || MuOrig == CharmedMeson || MuOrig == BottomBaryon ||
      MuOrig == CharmedBaryon || MuOrig == TauLep || MuOrig == QuarkWeakDec) {
    return NonIsoMuon;
  }
  //  if (MuOrig == Pion  || MuOrig == Kaon ) return  DecayMuon;
  return BkgMuon;
}

// 2345678901234567890123456789012345678901234567890123456789012345678901234567890
//-------------------------------------------------------------------------------
ParticleOrigin
MCTruthClassifier::defOrigOfMuon(const xAOD::TruthParticleContainer* mcTruthTES,
                                 const xAOD::TruthParticle* thePart,
                                 bool& isPrompt,
                                 Info* info) const
{
  //-------------------------------------------------------------------------------

  ATH_MSG_DEBUG("Executing DefOrigOfMuon ");

  int PriBarcode = thePart->barcode() % m_barcodeShift;
  const xAOD::TruthParticle* thePriPart = barcode_to_particle(mcTruthTES, PriBarcode);
  if (!thePriPart)
    return NonDefined;
  if (abs(thePriPart->pdgId()) != 13)
    return NonDefined;

  const xAOD::TruthVertex* partOriVert = thePriPart->hasProdVtx() ? thePriPart->prodVtx() : nullptr;

  //-- to define muon  outcome status
  if (info)
    info->particleOutCome = defOutComeOfMuon(thePriPart);

  if (!partOriVert)
    return NonDefined;

  int numOfParents = partOriVert->nIncomingParticles();
  if (numOfParents > 1)
    ATH_MSG_DEBUG("DefOrigOfMuon:: muon  has more than one mother ");

  const xAOD::TruthParticle* mother = getMother(thePriPart);
  if (info)
    info->mother = mother;
  if (!mother) {
    return NonDefined;
  }

  const xAOD::TruthVertex* mothOriVert = mother->hasProdVtx() ? mother->prodVtx() : nullptr;
  int motherPDG = mother->pdgId();
  if (info) {
    info->motherPDG = motherPDG;
    info->motherBarcode = mother->barcode();
  }

  if ((abs(motherPDG) == 15 || abs(motherPDG) == 24) && mothOriVert != nullptr) {
    long pPDG(0);
    const xAOD::TruthParticle* MotherParent(nullptr);
    do {
      //
      pPDG = 0;
      //
      const xAOD::TruthVertex* mother_prdVtx(nullptr);
      const xAOD::TruthVertex* mother_endVtx(nullptr);
      MotherParent = getMother(mother);
      // to prevent Sherpa loop
      mother_prdVtx = mother->hasProdVtx() ? mother->prodVtx() : nullptr;
      mother_endVtx = mother->decayVtx();
      //
      const xAOD::TruthVertex* parent_prdVtx(nullptr);
      const xAOD::TruthVertex* parent_endVtx(nullptr);
      if (MotherParent) {
        parent_prdVtx = MotherParent->hasProdVtx() ? MotherParent->prodVtx() : nullptr;
        parent_endVtx = MotherParent->decayVtx();
      }
      //
      if (mother_endVtx == parent_prdVtx && mother_prdVtx == parent_endVtx) {
        MotherParent = mother;
        break;
      }
      //
      // to prevent Sherpa loop
      if (mother == MotherParent) {
        break;
      }

      if (MotherParent) {
        pPDG = MotherParent->pdgId();
        if (abs(pPDG) == 13 || abs(pPDG) == 15 || abs(pPDG) == 24) {
          mother = MotherParent;
          if (info)
            info->mother = mother;
        }
      }
    } while ((abs(pPDG) == 13 || abs(pPDG) == 15 || abs(pPDG) == 24));

    if (abs(pPDG) == 15 || abs(pPDG) == 24 || abs(pPDG) == 23 || abs(pPDG) == 25 || abs(pPDG) == 35 ||
        abs(pPDG) == 36 || abs(pPDG) == 37 || abs(pPDG) == 32 || abs(pPDG) == 33 || abs(pPDG) == 34 || abs(pPDG) == 6 ||
        abs(pPDG) == 9900024 || abs(pPDG) == 9900012 || abs(pPDG) == 9900014 || abs(pPDG) == 9900016 ||
        (abs(pPDG) < 2000040 && abs(pPDG) > 1000001)) {
      if (info)
        info->mother = mother;
    }
  }

  motherPDG = mother->pdgId();
  mothOriVert = mother->hasProdVtx() ? mother->prodVtx() : nullptr;
  partOriVert = mother->decayVtx();
  numOfParents = partOriVert->nIncomingParticles();
  int numOfDaug = partOriVert->nOutgoingParticles();

  if (info) {
    info->motherPDG = motherPDG;
    info->motherBarcode = mother->barcode();
  }

  //---
  long DaugType(0);
  int NumOfPhot(0);

  int NumOfEl(0);

  int NumOfPos(0);
  int NumOfElNeut(0);

  int NumOfMuNeut(0);

  int NumOfLQ(0);

  int NumOfquark(0);

  int NumOfgluon(0);
  int NumOfMuPl(0);

  int NumOfMuMin(0);
  int NumOfTau(0);

  int NumOfTauNeut(0);
  for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles(); ipOut++) {
    const xAOD::TruthParticle* theDaug = partOriVert->outgoingParticle(ipOut);
    if (!theDaug)
      continue;
    DaugType = theDaug->pdgId();
    if (DaugType == 11)
      NumOfEl++;
    else if (DaugType == -11)
      NumOfPos++;
    else if (DaugType == 13)
      NumOfMuMin++;
    else if (DaugType == -13)
      NumOfMuPl++;
    else if (abs(DaugType) == 12)
      NumOfElNeut++;
    else if (abs(DaugType) == 14)
      NumOfMuNeut++;
    else if (abs(DaugType) == 15)
      NumOfTau++;
    else if (abs(DaugType) == 16)
      NumOfTauNeut++;
    else if (abs(DaugType) == 42)
      NumOfLQ++;
    else if (abs(DaugType) < 7)
      NumOfquark++;
    else if (abs(DaugType) == 21)
      NumOfgluon++;
    else if (DaugType == 22)
      NumOfPhot++;
  } // cycle itrDaug

  // if ( numOfParents == 0 && numOfDaug == 1 )   return  SingleMuon;

  if (abs(motherPDG) == 211 && numOfDaug == 2 && NumOfMuNeut == 1)
    return PionDecay;

  if (abs(motherPDG) == 321 && numOfDaug == 2 && NumOfMuNeut == 1)
    return KaonDecay;

  if (abs(motherPDG) == 15) {
    ParticleOrigin tauOrig = defOrigOfTau(mcTruthTES, mother, motherPDG, info);
    ParticleType tautype = defTypeOfTau(tauOrig);
    if (tautype == IsoTau)
      return tauOrig;
    else
      return TauLep;
  }

  if (abs(motherPDG) == 6)
    return top;

  // Quark weak decay
  if (abs(motherPDG) < 7 && numOfParents == 1 && numOfDaug == 3 && NumOfquark == 1 && NumOfMuNeut == 1)
    return QuarkWeakDec;

  if (abs(motherPDG) == 24 && mothOriVert != nullptr && mothOriVert->nIncomingParticles() != 0) {
    const xAOD::TruthVertex* prodVert = mothOriVert;
    const xAOD::TruthParticle* itrP;
    do {
      itrP = prodVert->incomingParticle(0);
      prodVert = itrP->hasProdVtx() ? itrP->prodVtx() : nullptr;
    } while (abs(itrP->pdgId()) == 24 && prodVert != nullptr);

    if (prodVert && prodVert->nIncomingParticles() == 1 && abs(itrP->pdgId()) == 9900012)
      return NuREle;
    else if (prodVert && prodVert->nIncomingParticles() == 1 && abs(itrP->pdgId()) == 9900014)
      return NuRMu;
    else if (prodVert && prodVert->nIncomingParticles() == 1 && abs(itrP->pdgId()) == 9900016)
      return NuRTau;
    return WBoson;
  } else if (abs(motherPDG) == 24)
    return WBoson;

  if (abs(motherPDG) == 23)
    return ZBoson;

  if (motherPDG == 22 && numOfDaug == 2 && NumOfMuMin == 1 && NumOfMuPl == 1) {
    return PhotonConv;
  }

  //-- Exotics
  if (abs(motherPDG) < 7 && numOfParents == 2 && NumOfMuPl == 1 && NumOfMuMin == 1 && NumOfEl + NumOfPos == 0 &&
      NumOfTau == 0 && partOriVert->barcode() == -5) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    const xAOD::TruthParticle* theDaug = partOriVert->outgoingParticle(0);
    const xAOD::TruthVertex* TmpVert = theDaug->decayVtx();
    if (abs(pdg1) == abs(pdg2) && TmpVert->nIncomingParticles() == 2)
      return ZorHeavyBoson;
  }

  if (abs(motherPDG) < 7 && numOfParents == 2 && (NumOfMuMin == 1 || NumOfMuPl == 1) && NumOfMuNeut == 1 &&
      NumOfEl + NumOfPos == 0 && NumOfTau == 0 && partOriVert->barcode() == -5) {
    const xAOD::TruthParticle* theDaug = partOriVert->outgoingParticle(0);
    const xAOD::TruthVertex* TmpVert = theDaug->decayVtx();
    if (TmpVert->nIncomingParticles() == 2)
      return HeavyBoson;
  }

  // MadGraphPythia ZWW*->lllnulnu

  if (numOfParents == 1 && numOfDaug > 4 && (abs(motherPDG) < 7 || motherPDG == 21)) {
    bool isZboson = false;
    bool isWboson = false;
    bool skipnext = false;
    for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles() - 1; ipOut++) {
      if (skipnext) {
        skipnext = false;
        continue;
      }
      const xAOD::TruthParticle* theDaug = partOriVert->outgoingParticle(ipOut);
      if (!theDaug)
        continue;
      const xAOD::TruthParticle* theNextDaug = nullptr;
      for (unsigned int ipOut1 = ipOut + 1; ipOut1 < partOriVert->nOutgoingParticles(); ipOut1++) {
        theNextDaug = partOriVert->outgoingParticle(ipOut1);
        if (theNextDaug != nullptr)
          break;
      }
      if (!theNextDaug)
        continue;
      if (abs(theDaug->pdgId()) == 13 && abs(theNextDaug->pdgId()) == 13) {
        // Zboson
        if (thePriPart == theDaug || thePriPart == theNextDaug) {
          isZboson = true;
          break;
        }
        skipnext = true;
      } else if (abs(theDaug->pdgId()) == 13 && abs(theNextDaug->pdgId()) == 14) {
        // WBoson
        if (thePriPart == theDaug || thePriPart == theNextDaug) {
          isWboson = true;
          break;
        }
        skipnext = true;
      }
    }
    if (isWboson)
      return WBoson;
    if (isZboson)
      return ZBoson;
  }

  //--Sherpa Z->mumu
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon) == 2 && NumOfMuPl == 1 && NumOfMuMin == 1)
    return ZBoson;

  //--Sherpa W->munu ??
  // if(numOfParents==2&&(numOfDaug-NumOfquark-NumOfgluon)==2&&(NumOfEl==1||NumOfPos==1)&&NumOfElNeut==1) return WBoson;
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon) == 2 && (NumOfMuPl == 1 || NumOfMuMin == 1) &&
      NumOfMuNeut == 1)
    return WBoson;

  //--Sherpa ZZ,ZW
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon) == 4 &&
      (NumOfEl + NumOfPos + NumOfMuPl + NumOfMuMin + NumOfTau + NumOfElNeut + NumOfMuNeut + NumOfTauNeut == 4)) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    if ((abs(pdg1) == 21 && abs(pdg2) == 21) || (abs(pdg1) < 7 && abs(pdg2) < 7) || (pdg1 == 21 && abs(pdg2) < 7) ||
        (pdg2 == 21 && abs(pdg1) < 7))
      return DiBoson;
  }

  //--Sherpa VVV -- Note, have to allow for prompt photon radiation or these get lost
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon - NumOfPhot) == 6 &&
      (NumOfEl + NumOfPos + NumOfMuPl + NumOfMuMin + NumOfTau + NumOfElNeut + NumOfMuNeut + NumOfTauNeut == 6)) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    if ((abs(pdg1) == 21 && abs(pdg2) == 21) || (abs(pdg1) < 7 && abs(pdg2) < 7) || (pdg1 == 21 && abs(pdg2) < 7) ||
        (pdg2 == 21 && abs(pdg1) < 7))
      return MultiBoson;
  }

  //--New Sherpa Z->mumu
  if (partOriVert == mothOriVert) {
    int NumOfMuLoop = 0;
    int NumOfMuNeuLoop = 0;
    int NumOfLepLoop = 0;
    for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles(); ipOut++) {
      for (unsigned int ipIn = 0; ipIn < partOriVert->nIncomingParticles(); ipIn++) {
        if (!partOriVert->outgoingParticle(ipOut))
          continue;
        if (!partOriVert->incomingParticle(ipIn))
          continue;
        if (partOriVert->outgoingParticle(ipOut)->barcode() == partOriVert->incomingParticle(ipIn)->barcode()) {
          if (fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 13)
            NumOfMuLoop++;
          if (fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 14)
            NumOfMuNeuLoop++;
          if (fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 11 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 12 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 13 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 14 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 15 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 16)
            NumOfLepLoop++;
        }
      }
    }
    if (NumOfMuLoop == 2 && NumOfMuNeuLoop == 0)
      return ZBoson;
    if (NumOfMuLoop == 1 && NumOfMuNeuLoop == 1)
      return WBoson;
    if ((NumOfMuLoop == 4 && NumOfMuNeuLoop == 0) || (NumOfMuLoop == 3 && NumOfMuNeuLoop == 1) ||
        (NumOfMuLoop == 2 && NumOfMuNeuLoop == 2))
      return DiBoson;
    if (NumOfLepLoop == 4)
      return DiBoson;
  }

  //-- McAtNLo
  if (abs(motherPDG) < 7 && numOfParents == 2 && NumOfMuPl == 1 && NumOfMuMin == 1 && partOriVert->barcode() == -1) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(0)->pdgId();
    if (abs(pdg1) == abs(pdg2))
      return ZBoson;
  }

  if (abs(motherPDG) == 25)
    return Higgs;

  if (abs(motherPDG) == 35 || abs(motherPDG) == 36 || abs(motherPDG) == 37)
    return HiggsMSSM;

  if (abs(motherPDG) == 32 || abs(motherPDG) == 33 || abs(motherPDG) == 34)
    return HeavyBoson;

  if (abs(motherPDG) == 9900024)
    return WBosonLRSM;
  if (abs(motherPDG) == 9900012)
    return NuREle;
  if (abs(motherPDG) == 9900014)
    return NuRMu;
  if (abs(motherPDG) == 9900016)
    return NuRTau;

  if (abs(motherPDG) == 42 || NumOfLQ != 0)
    return LQ;
  if (MC::PID::isSUSY(motherPDG))
    return SUSY;
  if (MC::PID::isBSM(motherPDG))
    return OtherBSM;

  ParticleType pType = defTypeOfHadron(motherPDG);
  if ((pType == BBbarMesonPart || pType == CCbarMesonPart) && mothOriVert != nullptr && isHardScatVrtx(mothOriVert))
    isPrompt = true;

  return convHadronTypeToOrig(pType, motherPDG);
}
//-------------------------------------------------------------------------------
ParticleType
MCTruthClassifier::defTypeOfTau(ParticleOrigin TauOrig)
{
  //-------------------------------------------------------------------------------

  if (TauOrig == NonDefined)
    return UnknownTau;

  if (TauOrig == WBoson || TauOrig == ZBoson || TauOrig == top || TauOrig == SingleMuon || TauOrig == Higgs ||
      TauOrig == HiggsMSSM || TauOrig == HeavyBoson || TauOrig == WBosonLRSM || TauOrig == NuREle || TauOrig == NuRMu ||
      TauOrig == NuRTau || TauOrig == SUSY || TauOrig == DiBoson || TauOrig == ZorHeavyBoson || TauOrig == OtherBSM ||
      TauOrig == MultiBoson)
    return IsoTau;

  if (TauOrig == JPsi || TauOrig == BottomMeson || TauOrig == CharmedMeson || TauOrig == BottomBaryon ||
      TauOrig == CharmedBaryon || TauOrig == QuarkWeakDec)
    return NonIsoTau;

  return BkgTau;
}

//-------------------------------------------------------------------------------
ParticleOrigin
MCTruthClassifier::defOrigOfTau(const xAOD::TruthParticleContainer* mcTruthTES,
                                const xAOD::TruthParticle* thePart,
                                int motherPDG,
                                Info* info) const
{
  //-------------------------------------------------------------------------------

  ATH_MSG_DEBUG("Executing DefOrigOfTau ");

  int PriBarcode = thePart->barcode() % m_barcodeShift;
  const xAOD::TruthParticle* thePriPart = barcode_to_particle(mcTruthTES, PriBarcode);
  if (!thePriPart)
    return NonDefined;
  if (abs(thePriPart->pdgId()) != 15)
    return NonDefined;

  const xAOD::TruthVertex* partOriVert = thePriPart->hasProdVtx() ? thePriPart->prodVtx() : nullptr;

  //-- to define tau  outcome status
  if (thePriPart->status() < 3) {
    if (info)
      info->particleOutCome = defOutComeOfTau(thePriPart, info);
  }

  if (!partOriVert)
    return NonDefined;

  int numOfParents = partOriVert->nIncomingParticles();
  if (numOfParents > 1)
    ATH_MSG_DEBUG("DefOrigOfTau:: tau  has more than one mother ");

  const xAOD::TruthParticle* mother = getMother(thePriPart);
  if (info)
    info->mother = mother;
  if (!mother) {
    return NonDefined;
  }

  const xAOD::TruthVertex* mothOriVert = mother->hasProdVtx() ? mother->prodVtx() : nullptr;

  const xAOD::TruthParticle* MotherParent(nullptr);

  if (abs(motherPDG) == 24 && mothOriVert != nullptr) {
    MotherParent = getMother(mother);
    long pPDG(0);
    if (MotherParent) {
      pPDG = MotherParent->pdgId();
      if (abs(pPDG) == 6) {
        mother = MotherParent;
        if (info)
          info->mother = mother;
      }
    }
  }

  motherPDG = mother->pdgId();
  if (info) {
    info->motherPDG = motherPDG;
    info->motherBarcode = mother->barcode();
  }
  mothOriVert = mother->hasProdVtx() ? mother->prodVtx() : nullptr;
  partOriVert = mother->decayVtx();
  if (!partOriVert) {
    return NonDefined;
  }

  numOfParents = partOriVert->nIncomingParticles();
  int numOfDaug = partOriVert->nOutgoingParticles();

  int NumOfPhot(0);

  int NumOfEl(0);

  int NumOfPos(0);
  int NumOfquark(0);

  int NumOfgluon(0);

  int NumOfElNeut(0);
  int NumOfMuPl(0);

  int NumOfMuMin(0);

  int NumOfMuNeut(0);
  int NumOfTau(0);

  int NumOfTauNeut(0);
  long DaugType(0);

  for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles(); ipOut++) {
    if (!partOriVert->outgoingParticle(ipOut))
      continue;
    DaugType = partOriVert->outgoingParticle(ipOut)->pdgId();
    if (abs(DaugType) < 7)
      NumOfquark++;
    if (abs(DaugType) == 21)
      NumOfgluon++;
    if (abs(DaugType) == 12)
      NumOfElNeut++;
    if (abs(DaugType) == 14)
      NumOfMuNeut++;
    if (DaugType == 22)
      NumOfPhot++;
    if (DaugType == 11)
      NumOfEl++;
    if (DaugType == -11)
      NumOfPos++;
    if (DaugType == 13)
      NumOfMuMin++;
    if (DaugType == -13)
      NumOfMuPl++;
    if (abs(DaugType) == 15)
      NumOfTau++;
    if (abs(DaugType) == 16)
      NumOfTauNeut++;
  }

  if (abs(motherPDG) == 6)
    return top;

  if (abs(motherPDG) == 24 && mothOriVert != nullptr && mothOriVert->nIncomingParticles() != 0) {
    const xAOD::TruthVertex* prodVert = mothOriVert;
    const xAOD::TruthParticle* itrP;
    do {
      itrP = prodVert->incomingParticle(0);
      prodVert = itrP->hasProdVtx() ? itrP->prodVtx() : nullptr;
    } while (abs(itrP->pdgId()) == 24 && prodVert != nullptr);

    if (prodVert && prodVert->nIncomingParticles() == 1 && abs(itrP->pdgId()) == 9900012)
      return NuREle;
    else if (prodVert && prodVert->nIncomingParticles() == 1 && abs(itrP->pdgId()) == 9900014)
      return NuRMu;
    else if (prodVert && prodVert->nIncomingParticles() == 1 && abs(itrP->pdgId()) == 9900016)
      return NuRTau;
    return WBoson;
  } else if (abs(motherPDG) == 24) {
    return WBoson;
  }

  if (abs(motherPDG) == 23) {
    return ZBoson;
  }
  if (numOfParents == 1 && numOfDaug > 4 && (abs(motherPDG) < 7 || motherPDG == 21)) {
    bool isZboson = false;
    bool isWboson = false;
    bool skipnext = false;
    for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles() - 1; ipOut++) {
      if (skipnext) {
        skipnext = false;
        continue;
      }
      const xAOD::TruthParticle* theDaug = partOriVert->outgoingParticle(ipOut);
      if (!theDaug)
        continue;
      const xAOD::TruthParticle* theNextDaug = nullptr;
      for (unsigned int ipOut1 = ipOut + 1; ipOut1 < partOriVert->nOutgoingParticles(); ipOut1++) {
        theNextDaug = partOriVert->outgoingParticle(ipOut1);
        if (theNextDaug != nullptr)
          break;
      }
      if (!theNextDaug) {
        continue;
      }
      if (abs(theDaug->pdgId()) == 15 && abs(theNextDaug->pdgId()) == 15) {
        // Zboson
        if (thePriPart == theDaug || thePriPart == theNextDaug) {
          isZboson = true;
          break;
        }
        skipnext = true;
      } else if (abs(theDaug->pdgId()) == 15 && abs(theNextDaug->pdgId()) == 16) {
        // WBoson
        if (thePriPart == theDaug || thePriPart == theNextDaug) {
          isWboson = true;
          break;
        }
        skipnext = true;
      }
    }
    if (isWboson)
      return WBoson;
    if (isZboson)
      return ZBoson;
  }
  //--Sherpa Z->tautau
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon) == 2 && NumOfTau == 2) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    if ((abs(pdg1) == 21 && abs(pdg2) == 21) || (abs(pdg1) < 7 && abs(pdg2) < 7) || (pdg1 == 21 && abs(pdg2) < 7) ||
        (pdg2 == 21 && abs(pdg1) < 7))
      return ZBoson;
  }

  //--Sherpa W->taunu  new
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon) == 2 && NumOfTau == 1 && NumOfTauNeut == 1)
    return WBoson;

  //--Sherpa ZZ,ZW
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon) == 4 &&
      (NumOfEl + NumOfPos + NumOfMuPl + NumOfMuMin + NumOfTau + NumOfElNeut + NumOfMuNeut + NumOfTauNeut == 4)) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    if ((abs(pdg1) == 21 && abs(pdg2) == 21) || (abs(pdg1) < 7 && abs(pdg2) < 7) || (pdg1 == 21 && abs(pdg2) < 7) ||
        (pdg2 == 21 && abs(pdg1) < 7))
      return DiBoson;
  }

  //--Sherpa VVV -- Note, have to allow for prompt photon radiation or these get lost
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon - NumOfPhot) == 6 &&
      (NumOfEl + NumOfPos + NumOfMuPl + NumOfMuMin + NumOfTau + NumOfElNeut + NumOfMuNeut + NumOfTauNeut == 6)) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    if ((abs(pdg1) == 21 && abs(pdg2) == 21) || (abs(pdg1) < 7 && abs(pdg2) < 7) || (pdg1 == 21 && abs(pdg2) < 7) ||
        (pdg2 == 21 && abs(pdg1) < 7))
      return MultiBoson;
  }

  // New Sherpa Z->tautau
  if (partOriVert == mothOriVert) {
    int NumOfTauLoop = 0;
    int NumOfTauNeuLoop = 0;
    int NumOfLepLoop = 0;
    for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles(); ipOut++) {
      for (unsigned int ipIn = 0; ipIn < partOriVert->nIncomingParticles(); ipIn++) {
        if (!partOriVert->outgoingParticle(ipOut))
          continue;
        if (!partOriVert->incomingParticle(ipIn))
          continue;
        if (partOriVert->outgoingParticle(ipOut)->barcode() == partOriVert->incomingParticle(ipIn)->barcode()) {
          if (fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 15)
            NumOfTauLoop++;
          if (fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 16)
            NumOfTauNeuLoop++;
          if (fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 11 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 12 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 13 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 14 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 15 ||
              fabs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 16)
            NumOfLepLoop++;
        }
      }
    }
    if (NumOfTauLoop == 2 && NumOfTauNeuLoop == 0)
      return ZBoson;
    if (NumOfTauLoop == 1 && NumOfTauNeuLoop == 1)
      return WBoson;
    if ((NumOfTauLoop == 4 && NumOfTauNeuLoop == 0) || (NumOfTauLoop == 3 && NumOfTauNeuLoop == 1) ||
        (NumOfTauLoop == 2 && NumOfTauNeuLoop == 2))
      return DiBoson;
    if (NumOfLepLoop == 4)
      return DiBoson;
  }

  if (abs(motherPDG) == 25)
    return Higgs;

  if (abs(motherPDG) == 35 || abs(motherPDG) == 36 || abs(motherPDG) == 37)
    return HiggsMSSM;

  if (abs(motherPDG) == 32 || abs(motherPDG) == 33 || abs(motherPDG) == 34)
    return HeavyBoson;

  if (abs(motherPDG) == 9900024)
    return WBosonLRSM;
  if (abs(motherPDG) == 9900016)
    return NuRTau;

  if (MC::PID::isSUSY(motherPDG))
    return SUSY;
  if (MC::PID::isBSM(motherPDG))
    return OtherBSM;

  if (abs(motherPDG) == 443)
    return JPsi;

  ParticleType pType = defTypeOfHadron(motherPDG);
  return convHadronTypeToOrig(pType, motherPDG);
}

//-------------------------------------------------------------------------------
ParticleType
MCTruthClassifier::defTypeOfPhoton(ParticleOrigin PhotOrig) const
{
  //-------------------------------------------------------------------------------

  if (PhotOrig == NonDefined)
    return UnknownPhoton;

  if (PhotOrig == WBoson || PhotOrig == ZBoson || PhotOrig == SinglePhot || PhotOrig == Higgs ||
      PhotOrig == HiggsMSSM || PhotOrig == HeavyBoson || PhotOrig == PromptPhot || PhotOrig == SUSY ||
      PhotOrig == OtherBSM)
    return IsoPhoton;

  if (PhotOrig == ISRPhot || PhotOrig == FSRPhot || PhotOrig == TauLep || PhotOrig == Mu || PhotOrig == NuREle ||
      PhotOrig == NuRMu || PhotOrig == NuRTau)
    return NonIsoPhoton;

  return BkgPhoton;
}

//-------------------------------------------------------------------------------
ParticleOrigin
MCTruthClassifier::defOrigOfPhoton(const xAOD::TruthParticleContainer* mcTruthTES,
                                   const xAOD::TruthParticle* thePart,
                                   bool& isPrompt,
                                   Info* info) const
//-------------------------------------------------------------------------------
{

  ATH_MSG_DEBUG("Executing DefOrigOfPhoton ");

  if (info) {
    info->mother = nullptr;
    info->photonMotherBarcode = 0;
    info->photonMotherPDG = 0;
    info->photonMotherStatus = 0;
    info->motherBarcode = 0;
  }

  int PriBarcode = thePart->barcode() % m_barcodeShift;

  const xAOD::TruthParticle* thePriPart = barcode_to_particle(mcTruthTES, PriBarcode);
  if (!thePriPart)
    return NonDefined;
  if (abs(thePriPart->pdgId()) != 22)
    return NonDefined;

  const xAOD::TruthVertex* partOriVert = thePriPart->hasProdVtx() ? thePriPart->prodVtx() : nullptr;

  //-- to define photon outcome status
  if (info)
    info->particleOutCome = defOutComeOfPhoton(thePriPart);

  if (!partOriVert) {
    return NonDefined;
  }

  int numOfParents = partOriVert->nIncomingParticles();
  if (partOriVert->nIncomingParticles() > 1)
    ATH_MSG_DEBUG("DefOrigOfPhoton:: photon  has more than one mother ");

  const xAOD::TruthParticle* mother = getMother(thePriPart);
  if (info)
    info->mother = mother;
  if (!mother)
    return NonDefined;
  int motherPDG = mother->pdgId();
  const xAOD::TruthVertex* mothOriVert = mother->hasProdVtx() ? mother->prodVtx() : nullptr;
  int motherStatus = mother->status();
  long motherBarcode = mother->barcode();
  if (info) {
    info->motherPDG = motherPDG;
    info->motherBarcode = motherBarcode;
  }
  partOriVert = mother->decayVtx();
  numOfParents = partOriVert->nIncomingParticles();
  int numOfDaug = partOriVert->nOutgoingParticles();

  int NumOfNucFr(0);

  int NumOfEl(0);

  int NumOfPos(0);

  int NumOfMu(0);

  int NumOfTau(0);

  int NumOfPht(0);

  int NumOfLQ(0);
  long DaugBarcode(0);

  long DaugType(0);

  long NumOfLep(0);

  long NumOfNeut(0);

  long NumOfPartons(0);
  for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles(); ipOut++) {
    if (!partOriVert->outgoingParticle(ipOut))
      continue;
    DaugType = partOriVert->outgoingParticle(ipOut)->pdgId();
    if (numOfParents == 1 && (motherPDG == 22 || abs(motherPDG) == 11 || abs(motherPDG) == 211) &&
        (DaugType > 1000000000 || DaugType == 0 || DaugType == 2212 || DaugType == 2112))
      NumOfNucFr++;
    if (DaugType == 22)
      NumOfPht++;
    if (DaugType == 11)
      NumOfEl++;
    if (DaugType == -11)
      NumOfPos++;
    if (abs(DaugType) == 13)
      NumOfMu++;
    if (abs(DaugType) == 15)
      NumOfTau++;
    if (abs(DaugType) == 42)
      NumOfLQ++;
    if (abs(DaugType) == 11 || abs(DaugType) == 13 || abs(DaugType) == 15)
      NumOfLep++;
    if (abs(DaugType) == 12 || abs(DaugType) == 14 || abs(DaugType) == 16)
      NumOfNeut++;
    if (abs(DaugType) < 11 || (abs(DaugType) > 16 && abs(DaugType) < 43 && abs(DaugType) != 22))
      NumOfPartons++;

    if (DaugType == motherPDG)
      DaugBarcode = partOriVert->outgoingParticle(ipOut)->barcode();
  } // cycle itrDaug

  bool foundISR = false;
  bool foundFSR = false;

  if (numOfParents == 1 && numOfDaug == 2 && DaugBarcode % m_barcodeShift == motherBarcode % m_barcodeShift)
    return BremPhot;

  if (numOfParents == 1 && numOfDaug == 2 && abs(motherPDG) == 11 && NumOfPht == 2)
    return ElMagProc;

  // decay of W,Z and Higgs to lepton with FSR generated by Pythia
  if (numOfParents == 1 && numOfDaug == 2 && (abs(motherPDG) == 11 || abs(motherPDG) == 13 || abs(motherPDG) == 15) &&
      DaugBarcode % m_barcodeShift != motherBarcode % m_barcodeShift && mothOriVert != nullptr &&
      mothOriVert->nIncomingParticles() == 1) {
    int itr = 0;
    long PartPDG = 0;
    const xAOD::TruthVertex* prodVert = mothOriVert;
    const xAOD::TruthVertex* Vert = nullptr;
    do {
      Vert = prodVert;
      for (unsigned int ipIn = 0; ipIn < Vert->nIncomingParticles(); ipIn++) {
        if (!Vert->incomingParticle(ipIn))
          continue;
        PartPDG = abs(Vert->incomingParticle(ipIn)->pdgId());
        prodVert = Vert->incomingParticle(ipIn)->hasProdVtx() ? Vert->incomingParticle(ipIn)->prodVtx() : nullptr;
        if (PartPDG == 23 || PartPDG == 24 || PartPDG == 25)
          foundFSR = true;
      } // cycle itrMother
      itr++;
      if (itr > 100) {
        ATH_MSG_WARNING("DefOrigOfPhoton:: infinite while");
        break;
      }
    } while (prodVert != nullptr && abs(motherPDG) == PartPDG);

    if (foundFSR)
      return FSRPhot;
  }

  // Nucl reaction
  // gamma+Nuclear=>gamma+Nucl.Fr+Nuclons+pions
  // e+Nuclear=>e+gamma+Nucl.Fr+Nuclons+pions
  // pi+Nuclear=>gamma+Nucl.Fr+Nuclons+pions

  if ((numOfParents == 1 && (abs(motherPDG) == 22 || abs(motherPDG) == 11) && numOfDaug > 2 && NumOfNucFr != 0) ||
      (numOfParents == 1 && abs(motherPDG) == 211 && numOfDaug > 10 && NumOfNucFr != 0) ||
      (numOfParents == 1 && motherPDG == 22 && numOfDaug > 10 && motherStatus == 1) ||
      (numOfParents == 1 && motherPDG > 1000000000))
    return NucReact;

  if (abs(motherPDG) == 13 && NumOfMu == 0)
    return Mu;

  if (abs(motherPDG) == 15 && NumOfTau == 0)
    return TauLep;

  //-- to find prompt photon
  if (abs(partOriVert->barcode()) == 5) {
    int nqin(0);

    int ngin(0);

    int nqout(0);

    int ngout(0);

    int npartin(0);

    int npartout(0);

    int nphtout(0);
    if (partOriVert != nullptr) {
      npartin = partOriVert->nIncomingParticles();
      for (unsigned int ipIn = 0; ipIn < partOriVert->nIncomingParticles(); ipIn++) {
        if (!partOriVert->incomingParticle(ipIn))
          continue;
        if (abs(partOriVert->incomingParticle(ipIn)->pdgId()) < 7)
          nqin++;
        if (abs(partOriVert->incomingParticle(ipIn)->pdgId()) == 21)
          ngin++;
      }

      npartout = partOriVert->nOutgoingParticles();
      for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles(); ipOut++) {
        if (!partOriVert->outgoingParticle(ipOut))
          continue;
        if (abs(partOriVert->outgoingParticle(ipOut)->pdgId()) < 7)
          nqout++;
        if (abs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 21)
          ngout++;
        if (abs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 22)
          nphtout++;
      }
    }

    if (npartout == 2 && npartin == 2 &&
        (((nqin == 2 && ngin == 0) && (nqout == 0 && ngout == 1 && nphtout == 1)) ||
         ((nqin == 2 && ngin == 0) && (nqout == 0 && ngout == 0 && nphtout == 2)) ||
         ((nqin == 1 && ngin == 1) && (nqout == 1 && ngout == 0 && nphtout == 1)) ||
         ((nqin == 0 && ngin == 2) && (nqout == 0 && ngout == 0 && nphtout == 2)) ||
         ((nqin == 0 && ngin == 2) && (nqout == 0 && ngout == 1 && nphtout == 1))))
      return PromptPhot;
  }

  //-- to find initial and final state raiation and underline photons
  //-- dijets and min bias
  if (numOfParents == 1 && motherStatus == 3) {

    //  const xAOD::TruthParticle* thePhotMoth = (*mcEventItr)->barcode_to_particle(motherBarcode);
    //  const xAOD::TruthVertex*   MothOriVert = thePhotMoth->prodVtx();
    if (mothOriVert != nullptr && abs(mothOriVert->barcode()) == 5)
      return FSRPhot;

    for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles(); ipOut++) {
      if (!partOriVert->outgoingParticle(ipOut))
        continue;
      if (partOriVert->outgoingParticle(ipOut)->status() != 3 ||
          motherPDG != partOriVert->outgoingParticle(ipOut)->pdgId())
        continue;
      const xAOD::TruthVertex* Vrtx = partOriVert->outgoingParticle(ipOut)->decayVtx();
      if (!Vrtx)
        continue;
      if (abs(Vrtx->barcode()) == 5)
        foundISR = true;
    }

    if (foundISR)
      return ISRPhot;
    else
      return UndrPhot;
  }

  //-- to find initial and final state raiation and underline photons
  //-- SUSY
  if (numOfParents == 1 && (abs(motherPDG) < 7 || motherPDG == 21) &&
      !(numOfDaug == NumOfPht + NumOfPartons &&
        (motherStatus == 62 || motherStatus == 52 || motherStatus == 21 || motherStatus == 22))) {
    for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles(); ipOut++) {
      if (!partOriVert->outgoingParticle(ipOut))
        continue;
      if (motherPDG != partOriVert->outgoingParticle(ipOut)->pdgId())
        continue;
      const xAOD::TruthVertex* Vrtx = partOriVert->outgoingParticle(ipOut)->decayVtx();
      if (!Vrtx)
        continue;
      if (Vrtx->nOutgoingParticles() != 1 && Vrtx->nIncomingParticles() == 1)
        continue;
      if (!Vrtx->outgoingParticle(0))
        continue;
      if (Vrtx->outgoingParticle(0)->pdgId() == 91)
        foundISR = true;
    }
    if (foundISR)
      return ISRPhot;
    else
      return UndrPhot;
  }

  //-- to find final  state radiation
  //-- Exotics
  if (numOfParents == 1 && abs(motherPDG) == 11 && motherStatus == 2 && mothOriVert != nullptr &&
      mothOriVert->nIncomingParticles() != 0) {
    const xAOD::TruthParticle* itrP = mothOriVert->incomingParticle(0);
    const xAOD::TruthVertex* Vrtx = itrP->hasProdVtx() ? itrP->prodVtx() : nullptr;
    if (mothOriVert->nIncomingParticles() == 1 && abs(itrP->pdgId()) == 11 && Vrtx != nullptr &&
        abs(Vrtx->barcode()) == 5 && itrP->status() == 3)
      return FSRPhot;
  }

  // FSR  from Photos
  //-- Exotics- CompHep
  if (numOfParents == 2 && ((abs(motherPDG) == 11 && NumOfEl == 1 && NumOfPos == 1) ||
                            (abs(motherPDG) == 13 && NumOfMu == 2) || (abs(motherPDG) == 15 && NumOfTau == 2))) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    if (abs(pdg1) == abs(pdg2))
      return FSRPhot;
  }

  if (numOfParents == 2 && NumOfLep == 1 && NumOfNeut == 1 && (abs(motherPDG) == 11 || abs(motherPDG) == 12))
    return FSRPhot;

  //-- Exotics - CompHep
  if (abs(motherPDG) == 11 && numOfParents == 1 && numOfDaug == 2 && (NumOfEl == 1 || NumOfPos == 1) && NumOfPht == 1 &&
      DaugBarcode % m_barcodeShift != motherBarcode % m_barcodeShift && DaugBarcode < 20000 && motherBarcode < 20000)
    return FSRPhot;

  // FSR  from Photos
  if (abs(motherPDG) == 23 &&
      ((NumOfEl + NumOfPos == 2 || NumOfEl + NumOfPos == 4) || (NumOfMu == 2 || NumOfMu == 4) ||
       (NumOfTau == 2 || NumOfTau == 4)) &&
      NumOfPht > 0)
    return FSRPhot;

  if (abs(motherPDG) == 9900024 && NumOfPht > 0)
    return FSRPhot;

  if (abs(motherPDG) == 9900012 && NumOfPht > 0)
    return FSRPhot;
  if (abs(motherPDG) == 9900014 && NumOfPht > 0)
    return FSRPhot;
  if (abs(motherPDG) == 9900016 && NumOfPht > 0)
    return FSRPhot;

  if (numOfParents == 2 && NumOfLQ == 1)
    return FSRPhot;

  //--- other process

  if (abs(motherPDG) == 23)
    return ZBoson;

  if (abs(motherPDG) == 24) {

    if (NumOfLep == 1 && NumOfNeut == 1 && numOfDaug == NumOfLep + NumOfNeut + NumOfPht)
      return FSRPhot;

    if (mothOriVert != nullptr && mothOriVert->nIncomingParticles() != 0) {
      const xAOD::TruthVertex* prodVert = mothOriVert;
      const xAOD::TruthParticle* itrP;
      do {
        itrP = prodVert->incomingParticle(0);
        prodVert = itrP->hasProdVtx() ? itrP->prodVtx() : nullptr;
      } while (abs(itrP->pdgId()) == 24 && prodVert != nullptr);

      if (prodVert && prodVert->nIncomingParticles() == 1 && abs(itrP->pdgId()) == 15)
        return TauLep;
      else if (prodVert && prodVert->nIncomingParticles() == 1 && abs(itrP->pdgId()) == 13)
        return Mu;
      else if (prodVert && prodVert->nIncomingParticles() == 1 && abs(itrP->pdgId()) == 9900012)
        return NuREle;
      else if (prodVert && prodVert->nIncomingParticles() == 1 && abs(itrP->pdgId()) == 9900014)
        return NuRMu;
      else if (prodVert && prodVert->nIncomingParticles() == 1 && abs(itrP->pdgId()) == 9900016)
        return NuRTau;

    } else
      return WBoson;
  }

  // MadGraphPythia ZWW*->lllnulnu
  if (numOfParents == 1 && numOfDaug > 4 && (abs(motherPDG) < 7 || motherPDG == 21)) {
    bool isZboson = false;
    bool isWboson = false;
    bool skipnext = false;
    for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles() - 1; ipOut++) {
      if (skipnext) {
        skipnext = false;
        continue;
      }
      const xAOD::TruthParticle* theDaug = partOriVert->outgoingParticle(ipOut);
      if (!theDaug)
        continue;
      const xAOD::TruthParticle* theNextDaug = nullptr;
      for (unsigned int ipOut1 = ipOut + 1; ipOut1 < partOriVert->nOutgoingParticles(); ipOut1++) {
        theNextDaug = partOriVert->outgoingParticle(ipOut1);
        if (theNextDaug != nullptr)
          break;
      }
      if (!theNextDaug)
        continue;

      if (abs(theDaug->pdgId()) == 15 && abs(theNextDaug->pdgId()) == 15) {
        // Zboson
        if (thePriPart == theDaug || thePriPart == theNextDaug) {
          isZboson = true;
          break;
        }
        skipnext = true;
      } else if (abs(theDaug->pdgId()) == 15 && abs(theNextDaug->pdgId()) == 16) {
        // WBoson
        if (thePriPart == theDaug || thePriPart == theNextDaug) {
          isWboson = true;
          break;
        }
        skipnext = true;
      }
    }
    if (isWboson)
      return WBoson;
    if (isZboson)
      return ZBoson;
  }

  //--Sherpa ZZ,ZW+FSR
  if (numOfParents == 4 && (numOfDaug - NumOfPht) == 4 && (NumOfLep + NumOfNeut == 4)) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    int pdg3 = partOriVert->incomingParticle(2)->pdgId();
    int pdg4 = partOriVert->incomingParticle(3)->pdgId();
    if (abs(pdg1) < 17 && abs(pdg1) > 10 && abs(pdg2) < 17 && abs(pdg2) > 10 && abs(pdg3) < 17 && abs(pdg3) > 10 &&
        abs(pdg4) < 17 && abs(pdg4) > 10)
      return FSRPhot;
  }

  //--New Sherpa single photon
  if (partOriVert == mothOriVert && partOriVert != nullptr) {
    int NumOfPhtLoop = 0;
    for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles(); ipOut++) {
      if (!partOriVert->outgoingParticle(ipOut))
        continue;
      for (unsigned int ipIn = 0; ipIn < partOriVert->nIncomingParticles(); ipIn++) {
        if (!partOriVert->incomingParticle(ipIn))
          continue;
        if (partOriVert->outgoingParticle(ipOut)->barcode() == partOriVert->incomingParticle(ipIn)->barcode() &&
            std::abs(partOriVert->outgoingParticle(ipOut)->pdgId()) == 22)
          NumOfPhtLoop++;
        if (NumOfPhtLoop == 1)
          return SinglePhot;
      }
    }
  }

  if (abs(motherPDG) == 25)
    return Higgs;

  if (abs(motherPDG) == 111)
    return PiZero;

  if (abs(motherPDG) == 35 || abs(motherPDG) == 36 || abs(motherPDG) == 37)
    return HiggsMSSM;

  if (abs(motherPDG) == 32 || abs(motherPDG) == 33 || abs(motherPDG) == 34 || abs(motherPDG) == 5100039 // KK graviton
  )
    return HeavyBoson;

  if (MC::PID::isSUSY(motherPDG))
    return SUSY;
  if (MC::PID::isBSM(motherPDG))
    return OtherBSM;

  // Pythia8 gamma+jet samples
  if ((motherStatus == 62 || motherStatus == 52 || motherStatus == 21 || motherStatus == 22) &&
      thePriPart->status() == 1 && NumOfPht == 1 && numOfDaug == (NumOfPht + NumOfPartons)) {
    return PromptPhot;
  }

  ParticleType pType = defTypeOfHadron(motherPDG);
  if ((pType == BBbarMesonPart || pType == CCbarMesonPart) && mothOriVert != nullptr && isHardScatVrtx(mothOriVert))
    isPrompt = true;
  return convHadronTypeToOrig(pType, motherPDG);
}
// 2345678901234567890123456789012345678901234567890123456789012345678901234567890
//-------------------------------------------------------------------------------
ParticleOrigin
MCTruthClassifier::defOrigOfNeutrino(const xAOD::TruthParticleContainer* mcTruthTES,
                                     const xAOD::TruthParticle* thePart,
                                     bool& isPrompt,
                                     Info* info) const
//-------------------------------------------------------------------------------
{
  // author - Pierre-Antoine Delsart
  //
  ATH_MSG_DEBUG("Executing DefOrigOfNeutrino ");

  int PriBarcode = thePart->barcode() % m_barcodeShift;
  int nuFlav = abs(thePart->pdgId());
  const xAOD::TruthParticle* thePriPart = barcode_to_particle(mcTruthTES, PriBarcode);
  if (!thePriPart)
    return NonDefined;
  if (abs(thePriPart->pdgId()) != nuFlav)
    return NonDefined;

  const xAOD::TruthVertex* partOriVert = thePriPart->hasProdVtx() ? thePriPart->prodVtx() : nullptr;

  //-- to define neutrino outcome status
  if (info)
    info->particleOutCome = NonInteract;

  if (!partOriVert)
    return NonDefined;

  int numOfParents = -1;
  numOfParents = partOriVert->nIncomingParticles();
  if (numOfParents > 1)
    ATH_MSG_DEBUG("DefOrigOfNeutrino:: neutrino  has more than one mother ");

  const xAOD::TruthParticle* mother = getMother(thePriPart);
  if (info)
    info->mother = mother;
  if (!mother) {
    return NonDefined;
  }
  int motherPDG = mother->pdgId();
  long motherBarcode = mother->barcode();
  if (info) {
    info->motherPDG = motherPDG;
    info->motherBarcode = motherBarcode;
  }
  const xAOD::TruthVertex* mothOriVert = mother->hasProdVtx() ? mother->prodVtx() : nullptr;

  // to resolve Sherpa loop
  bool samePart = false;
  if (mothOriVert && mothOriVert->barcode() == partOriVert->barcode())
    samePart = true;
  //
  if ((abs(motherPDG) == nuFlav || abs(motherPDG) == 15 || abs(motherPDG) == 24) && mothOriVert != nullptr &&
      !samePart) {
    long pPDG(0);
    const xAOD::TruthParticle* MotherParent(nullptr);
    do {
      pPDG = 0;
      MotherParent = getMother(mother);
      // to prevent Sherpa loop
      const xAOD::TruthVertex* mother_prdVtx(nullptr);
      const xAOD::TruthVertex* mother_endVtx(nullptr);
      if (mother) {
        mother_prdVtx = mother->hasProdVtx() ? mother->prodVtx() : nullptr;
        mother_endVtx = mother->decayVtx();
      }
      const xAOD::TruthVertex* parent_prdVtx(nullptr);
      const xAOD::TruthVertex* parent_endVtx(nullptr);
      if (MotherParent) {
        parent_prdVtx = MotherParent->hasProdVtx() ? MotherParent->prodVtx() : nullptr;
        parent_endVtx = MotherParent->decayVtx();
      }
      if (mother_endVtx == parent_prdVtx && mother_prdVtx == parent_endVtx) {
        MotherParent = mother;
        break;
      }
      //
      if (MotherParent) {
        pPDG = MotherParent->pdgId();
      }
      // to prevent Sherpa loop
      if (mother == MotherParent) {
        break;
      }
      if (abs(pPDG) == nuFlav || abs(pPDG) == 15 || abs(pPDG) == 24) {
        mother = MotherParent;
        if (info)
          info->mother = mother;
      }

    } while ((abs(pPDG) == nuFlav || abs(pPDG) == 15 || abs(pPDG) == 24));

    if (abs(pPDG) == nuFlav || abs(pPDG) == 15 || abs(pPDG) == 24 || abs(pPDG) == 23 || abs(pPDG) == 25 ||
        abs(pPDG) == 35 || abs(pPDG) == 36 || abs(pPDG) == 37 || abs(pPDG) == 32 || abs(pPDG) == 33 ||
        abs(pPDG) == 34 || abs(pPDG) == 6 || abs(pPDG) == 9900024 || abs(pPDG) == 9900012 || abs(pPDG) == 9900014 ||
        abs(pPDG) == 9900016 || (abs(pPDG) < 2000040 && abs(pPDG) > 1000001)) {
      mother = MotherParent;
      if (info)
        info->mother = mother;
    }
  }

  motherPDG = mother->pdgId();
  motherBarcode = mother->barcode();
  partOriVert = mother->decayVtx();
  mothOriVert = mother->hasProdVtx() ? mother->prodVtx() : nullptr;
  numOfParents = partOriVert->nIncomingParticles();
  int numOfDaug = partOriVert->nOutgoingParticles();

  if (info) {
    info->motherPDG = motherPDG;
    info->motherBarcode = motherBarcode;
  }

  int NumOfPhot(0);

  int NumOfNucFr(0);
  int NumOfquark(0);

  int NumOfgluon(0);

  int NumOfLQ(0);
  int NumOfElNeut(0);

  int NumOfMuNeut(0);

  int NumOfTauNeut(0);
  int NumOfEl(0);

  int NumOfMu(0);

  int NumOfTau(0);
  long DaugType(0);
  samePart = false;

  for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles(); ++ipOut) {
    const xAOD::TruthParticle* theDaug = partOriVert->outgoingParticle(ipOut);
    if (!theDaug)
      continue;
    DaugType = theDaug->pdgId();
    if (abs(DaugType) < 7)
      NumOfquark++;
    if (abs(DaugType) == 21)
      NumOfgluon++;
    if (abs(DaugType) == 12)
      NumOfElNeut++;
    if (abs(DaugType) == 14)
      NumOfMuNeut++;
    if (abs(DaugType) == 16)
      NumOfTauNeut++;
    if (DaugType == 22)
      NumOfPhot++;
    if (abs(DaugType) == 11)
      NumOfEl++;
    if (abs(DaugType) == 13)
      NumOfMu++;
    if (abs(DaugType) == 15)
      NumOfTau++;

    if (abs(DaugType) == 42)
      NumOfLQ++;
    if (abs(DaugType) == abs(motherPDG) && theDaug->barcode() % m_barcodeShift == motherBarcode % m_barcodeShift)
      samePart = true;
    if (numOfParents == 1 &&
        (motherPDG == 22 || abs(motherPDG) == 11 || abs(motherPDG) == 13 || abs(motherPDG) == 211) &&
        (DaugType > 1000000000 || DaugType == 0 || DaugType == 2212 || DaugType == 2112 || abs(DaugType) == 211 ||
         abs(DaugType) == 111))
      NumOfNucFr++;
  } // cycle itrDaug

  // Quark weak decay
  if (abs(motherPDG) < 7 && numOfParents == 1 && numOfDaug == 3 && NumOfquark == 1 &&
      (NumOfEl == 1 || NumOfMu == 1 || NumOfTau == 1))
    return QuarkWeakDec;

  if (abs(motherPDG) == 6)
    return top;

  if (abs(motherPDG) == 24 && mothOriVert != nullptr && mothOriVert->nIncomingParticles() != 0) {

    const xAOD::TruthVertex* prodVert = mothOriVert;
    const xAOD::TruthParticle* ptrPart;
    do {
      ptrPart = prodVert->incomingParticle(0);
      prodVert = ptrPart->hasProdVtx() ? ptrPart->prodVtx() : nullptr;
    } while (abs(ptrPart->pdgId()) == 24 && prodVert != nullptr);

    if (prodVert && prodVert->nIncomingParticles() == 1 && abs(ptrPart->pdgId()) == 9900012)
      return NuREle;
    else if (prodVert && prodVert->nIncomingParticles() == 1 && abs(ptrPart->pdgId()) == 9900014)
      return NuRMu;
    else if (prodVert && prodVert->nIncomingParticles() == 1 && abs(ptrPart->pdgId()) == 9900016)
      return NuRTau;
    return WBoson;
  } else if (abs(motherPDG) == 24)
    return WBoson;

  if (abs(motherPDG) == 23)
    return ZBoson;

  //-- Exotics
  if (abs(motherPDG) < 7 && numOfParents == 2 && numOfDaug == 2 && (NumOfEl == 1 || NumOfMu == 1 || NumOfTau == 1) &&
      partOriVert->barcode() == -5) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    const xAOD::TruthVertex* TmpVert = partOriVert->outgoingParticle(0)->decayVtx();

    if (abs(pdg1) == abs(pdg2) && TmpVert->nIncomingParticles() == 2)
      return ZorHeavyBoson;
  }

  if (abs(motherPDG) < 7 && numOfParents == 2 && numOfDaug == 2 && (NumOfEl == 1 || NumOfMu == 1 || NumOfTau == 1) &&
      (NumOfElNeut == 1 || NumOfMuNeut == 1 || NumOfTauNeut == 1) && partOriVert->barcode() == -5) {
    const xAOD::TruthVertex* TmpVert = partOriVert->outgoingParticle(0)->decayVtx();
    if (TmpVert->nIncomingParticles() == 2)
      return HeavyBoson;
  }

  // MadGraphPythia ZWW*->lllnulnu or ZWW*->nunulnulnu (don't even know if the latter is generated)
  if (numOfParents == 1 && numOfDaug > 4 && (abs(motherPDG) < 7 || motherPDG == 21)) {

    const xAOD::TruthParticle* thePartToCheck = thePriPart;
    const xAOD::TruthParticle* theMother =
      thePriPart->hasProdVtx() ? thePriPart->prodVtx()->incomingParticle(0) : nullptr;

    if (abs(theMother->pdgId()) == 11 && theMother->status() == 2)
      thePartToCheck = theMother;

    bool isZboson = false;
    bool isWboson = false;
    bool skipnext = false;

    for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles() - 1; ++ipOut) {
      const xAOD::TruthParticle* theDaug = partOriVert->outgoingParticle(ipOut);
      if (!theDaug)
        continue;
      const xAOD::TruthParticle* theNextDaug = nullptr;
      for (unsigned int ipOut1 = ipOut + 1; ipOut1 < partOriVert->nOutgoingParticles(); ipOut1++) {
        theNextDaug = partOriVert->outgoingParticle(ipOut1);
        if (theNextDaug != nullptr)
          break;
      }
      if (!theNextDaug)
        continue;

      if (skipnext) {
        skipnext = false;
        continue;
      }

      int apdgID1 = abs(theDaug->pdgId());
      int apdgID2 = abs(theNextDaug->pdgId());
      if ((apdgID1 == 12 && apdgID2 == 12) || (apdgID1 == 14 && apdgID2 == 14) || (apdgID1 == 16 && apdgID2 == 16)) {
        // Zboson
        if (thePartToCheck == theDaug || thePartToCheck == theNextDaug) {
          isZboson = true;
          break;
        }
        skipnext = true;
      } else if ((apdgID1 == 11 && apdgID2 == 12) || (apdgID1 == 14 && apdgID2 == 14) ||
                 (apdgID1 == 16 && apdgID2 == 16)) {
        // WBoson
        if (thePartToCheck == theDaug || thePartToCheck == theNextDaug) {
          isWboson = true;
          break;
        }
        skipnext = true;
      }
    }
    if (isWboson)
      return WBoson;
    if (isZboson)
      return ZBoson;
  }

  //--Sherpa Z->nunu
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon) == 2 &&
      (NumOfElNeut == 2 || NumOfMuNeut == 2 || NumOfTauNeut == 2))
    return ZBoson;

  //--Sherpa W->enu ??
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon) == 2 &&
      ((NumOfEl == 1 && NumOfElNeut == 1) || (NumOfMu == 1 && NumOfMuNeut == 1) ||
       (NumOfTau == 1 && NumOfTauNeut == 1)))
    return WBoson;

  //--Sherpa ZZ,ZW
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon) == 4 &&
      (NumOfEl + NumOfMu + NumOfTau + NumOfElNeut + NumOfMuNeut + NumOfTauNeut == 4)) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    if ((abs(pdg1) == 21 && abs(pdg2) == 21) || (abs(pdg1) < 7 && abs(pdg2) < 7) || (pdg1 == 21 && abs(pdg2) < 7) ||
        (pdg2 == 21 && abs(pdg1) < 7))
      return DiBoson;
  }

  //--Sherpa VVV -- Note, have to allow for prompt photon radiation or these get lost
  if (numOfParents == 2 && (numOfDaug - NumOfquark - NumOfgluon - NumOfPhot) == 6 &&
      (NumOfEl + NumOfMu + NumOfTau + NumOfElNeut + NumOfMuNeut + NumOfTauNeut == 6)) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    if ((abs(pdg1) == 21 && abs(pdg2) == 21) || (abs(pdg1) < 7 && abs(pdg2) < 7) || (pdg1 == 21 && abs(pdg2) < 7) ||
        (pdg2 == 21 && abs(pdg1) < 7))
      return MultiBoson;
  }

  // New Sherpa Z->nunu
  if (partOriVert == mothOriVert && partOriVert != nullptr) {
    int NumOfLepLoop = 0;
    int NumOfNeuLoop = 0;
    for (unsigned int ipOut = 0; ipOut < partOriVert->nOutgoingParticles(); ipOut++) {
      if (!partOriVert->outgoingParticle(ipOut))
        continue;
      for (unsigned int ipIn = 0; ipIn < partOriVert->nIncomingParticles(); ipIn++) {
        if (!partOriVert->incomingParticle(ipIn))
          continue;
        if (partOriVert->outgoingParticle(ipOut)->barcode() == partOriVert->incomingParticle(ipIn)->barcode()) {
          int apdgid = abs(partOriVert->outgoingParticle(ipOut)->pdgId());
          if (apdgid == 12 || apdgid == 14 || apdgid == 16)
            NumOfNeuLoop++;
          if (apdgid == 11 || apdgid == 13 || apdgid == 15)
            NumOfLepLoop++;
        }
      }
    }
    if (NumOfNeuLoop == 2 && NumOfLepLoop == 0)
      return ZBoson;
    if (NumOfNeuLoop == 1 && NumOfLepLoop == 1)
      return WBoson;
    if (NumOfNeuLoop + NumOfLepLoop == 4)
      return DiBoson;
  }

  //-- McAtNLo
  if (abs(motherPDG) < 7 && numOfParents == 2 && (NumOfElNeut == 2 || NumOfMuNeut == 2 || NumOfTauNeut == 2) &&
      partOriVert->barcode() == -1) {
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    if (abs(pdg1) == abs(pdg2))
      return ZBoson;
  }

  if (abs(motherPDG) == 25)
    return Higgs;

  if (abs(motherPDG) == 35 || abs(motherPDG) == 36 || abs(motherPDG) == 37)
    return HiggsMSSM;

  if (abs(motherPDG) == 32 || abs(motherPDG) == 33 || abs(motherPDG) == 34)
    return HeavyBoson;

  if (abs(motherPDG) == 15) {

    ParticleOrigin tauOrig = defOrigOfTau(mcTruthTES, mother, motherPDG, info);
    ParticleType tautype = defTypeOfTau(tauOrig);
    if (tautype == IsoTau)
      return tauOrig;
    else
      return TauLep;
  }

  if (abs(motherPDG) == 9900024)
    return WBosonLRSM;
  if (abs(motherPDG) == 9900012)
    return NuREle;
  if (abs(motherPDG) == 9900014)
    return NuRMu;
  if (abs(motherPDG) == 9900016)
    return NuRTau;

  if (abs(motherPDG) == 42 || NumOfLQ != 0)
    return LQ;

  if (MC::PID::isSUSY(motherPDG))
    return SUSY;

  if (MC::PID::isBSM(motherPDG))
    return OtherBSM;

  ParticleType pType = defTypeOfHadron(motherPDG);
  if ((pType == BBbarMesonPart || pType == CCbarMesonPart) && mothOriVert != nullptr && isHardScatVrtx(mothOriVert))
    isPrompt = true;

  //----
  return convHadronTypeToOrig(pType, motherPDG);
}

//---------------------------------------------------------------------------------
float
MCTruthClassifier::detPhi(float x, float y)
{
  //---------------------------------------------------------------------------------
  float det;
  det = x - y;
  if (det > M_PI)
    det = det - 2. * M_PI;
  if (det < -M_PI)
    det = det + 2. * M_PI;
  return std::fabs(det);
}
//---------------------------------------------------------------------------------
ParticleOrigin
MCTruthClassifier::convHadronTypeToOrig(ParticleType pType, int motherPDG)
{
  //---------------------------------------------------------------------------------
  if (pType == BBbarMesonPart && abs(motherPDG) == MC::PID::JPSI)
    return JPsi;
  else if (pType == BBbarMesonPart)
    return BBbarMeson;
  else if (pType == BottomMesonPart)
    return BottomMeson;
  else if (pType == BottomBaryonPart)
    return BottomBaryon;
  else if (pType == CCbarMesonPart)
    return CCbarMeson;
  else if (pType == CharmedMesonPart)
    return CharmedMeson;
  else if (pType == CharmedBaryonPart)
    return CharmedBaryon;
  else if (pType == StrangeBaryonPart)
    return StrangeBaryon;
  else if (pType == StrangeMesonPart)
    return StrangeMeson;
  else if (pType == LightBaryonPart)
    return LightBaryon;
  else if (pType == LightMesonPart)
    return LightMeson;
  else
    return NonDefined;
}
//---------------------------------------------------------------------------------
ParticleOrigin
MCTruthClassifier::defHadronType(long pdg)
{
  //---------------------------------------------------------------------------------
  // Special case
  if (abs(pdg) == MC::PID::JPSI)
    return JPsi;

  int q1 = (pdg / 1000) % 10;
  int q2 = (pdg / 100) % 10;
  int q3 = (pdg / 10) % 10;

  // di quark
  // if( q3 == 0 && q2 >=q3 )   cout<<"di quark"<<endl;

  if (q1 == 0 && MC::PID::BQUARK == q2 && MC::PID::BQUARK == q3)
    return BBbarMeson;
  else if (q1 == 0 && MC::PID::CQUARK == q3 && MC::PID::CQUARK == q2)
    return CCbarMeson;
  // Now just use the central helper functions
  else if (MC::PID::isBottomMeson(pdg))
    return BottomMeson;
  else if (MC::PID::isCharmMeson(pdg))
    return CharmedMeson;
  else if (MC::PID::isBottomBaryon(pdg))
    return BottomBaryon;
  else if (MC::PID::isCharmBaryon(pdg))
    return CharmedBaryon;
  else if (MC::PID::isStrangeBaryon(pdg))
    return StrangeBaryon;
  else if (MC::PID::isLightBaryon(pdg))
    return LightBaryon;
  else if (MC::PID::isStrangeMeson(pdg))
    return StrangeMeson;
  else if (MC::PID::isLightMeson(pdg))
    return LightMeson;
  else
    return NonDefined;
}

//---------------------------------------------------------------------------------
ParticleType
MCTruthClassifier::defTypeOfHadron(long pdg)
{
  //---------------------------------------------------------------------------------
  // Note that this differs from the above by return type -- should we be more clear?
  int q1 = (abs(pdg) / 1000) % 10;
  int q2 = (abs(pdg) / 100) % 10;
  int q3 = (abs(pdg) / 10) % 10;

  // di quark
  // if( q3 == 0 && q2 >=q3 )   cout<<"di quark"<<endl;
  // First two do not have obvious helpers in MCUtils
  if (q1 == 0 && MC::PID::BQUARK == q2 && MC::PID::BQUARK == q3)
    return BBbarMesonPart;
  else if (q1 == 0 && MC::PID::CQUARK == q3 && MC::PID::CQUARK == q2)
    return CCbarMesonPart;
  // Now just use the central helper functions
  else if (MC::PID::isBottomMeson(pdg))
    return BottomMesonPart;
  else if (MC::PID::isCharmMeson(pdg))
    return CharmedMesonPart;
  else if (MC::PID::isBottomBaryon(pdg))
    return BottomBaryonPart;
  else if (MC::PID::isCharmBaryon(pdg))
    return CharmedBaryonPart;
  else if (MC::PID::isStrangeBaryon(pdg))
    return StrangeBaryonPart;
  else if (MC::PID::isLightBaryon(pdg))
    return LightBaryonPart;
  else if (MC::PID::isStrangeMeson(pdg))
    return StrangeMesonPart;
  else if (MC::PID::isLightMeson(pdg))
    return LightMesonPart;
  else
    return Unknown;
}

//-------------------------------------------------------------------------------
bool
MCTruthClassifier::isHardScatVrtx(const xAOD::TruthVertex* pVert)
{
  //-------------------------------------------------------------------------------

  if (pVert == nullptr)
    return false;

  const xAOD::TruthVertex* pV = pVert;
  int numOfPartIn(0);

  int pdg(0);

  int NumOfParton(0);

  do {
    pVert = pV;
    numOfPartIn = pVert->nIncomingParticles();
    pdg = numOfPartIn != 0 && pVert->incomingParticle(0) != nullptr ? pVert->incomingParticle(0)->pdgId() : 0;
    pV = numOfPartIn != 0 && pVert->incomingParticle(0) != nullptr && pVert->incomingParticle(0)->hasProdVtx()
           ? pVert->incomingParticle(0)->prodVtx()
           : nullptr;

  } while (numOfPartIn == 1 && (abs(pdg) < 81 || abs(pdg) > 100) && pV != nullptr);

  if (numOfPartIn == 2) {
    for (unsigned int ipIn = 0; ipIn < pVert->nIncomingParticles(); ipIn++) {
      if (!pVert->incomingParticle(ipIn))
        continue;
      if (abs(pVert->incomingParticle(ipIn)->pdgId()) < 7 || pVert->incomingParticle(ipIn)->pdgId() == 21)
        NumOfParton++;
    }
    if (NumOfParton == 2)
      return true;
  }

  return false;
}

//---------------------------------------------------------------------------------
bool
MCTruthClassifier::isHadron(const xAOD::TruthParticle* thePart)
{
  //---------------------------------------------------------------------------------

  bool isPartHadron = false;
  if (thePart != nullptr) {
    long pdg = thePart->pdgId();
    isPartHadron = MC::PID::isHadron(pdg);
    //--exclude protons from beam
    if (pdg == 2212 && thePart->status() == 3)
      isPartHadron = false;
  }
  return isPartHadron;
}

//---------------------------------------------------------------------------------
const xAOD::TruthParticle*
MCTruthClassifier::getMother(const xAOD::TruthParticle* thePart) const
{
  //---------------------------------------------------------------------------------
  ATH_MSG_DEBUG("executing getMother");

  const xAOD::TruthVertex* partOriVert = thePart->hasProdVtx() ? thePart->prodVtx() : nullptr;

  long partPDG = thePart->pdgId();
  int partBarcode = thePart->barcode() % m_barcodeShift;
  long MotherPDG(0);

  const xAOD::TruthVertex* MothOriVert(nullptr);
  const xAOD::TruthParticle* theMoth(nullptr);

  if (!partOriVert)
    return theMoth;

  int itr = 0;
  do {
    if (itr != 0)
      partOriVert = MothOriVert;
    for (unsigned int ipIn = 0; ipIn < partOriVert->nIncomingParticles(); ipIn++) {
      theMoth = partOriVert->incomingParticle(ipIn);
      if (!theMoth)
        continue;
      MotherPDG = theMoth->pdgId();
      MothOriVert = theMoth->hasProdVtx() ? theMoth->prodVtx() : nullptr;
      if (MotherPDG == partPDG)
        break;
    }
    itr++;
    if (itr > 100) {
      ATH_MSG_WARNING("getMother:: infinite while");
      break;
    }
  } while (MothOriVert != nullptr && MotherPDG == partPDG && partBarcode < m_barcodeG4Shift &&
           MothOriVert != partOriVert);

  ATH_MSG_DEBUG("succeded getMother");
  return theMoth;
}

//---------------------------------------------------------------------------------
const xAOD::TruthVertex*
MCTruthClassifier::findEndVert(const xAOD::TruthParticle* thePart) const
{
  //---------------------------------------------------------------------------------
  const xAOD::TruthVertex* EndVert = thePart->decayVtx();
  const xAOD::TruthVertex* pVert(nullptr);
  if (EndVert != nullptr) {
    do {
      bool samePart = false;
      pVert = nullptr;
      for (unsigned int ipOut = 0; ipOut < EndVert->nOutgoingParticles(); ipOut++) {
        const xAOD::TruthParticle* itrDaug = EndVert->outgoingParticle(ipOut);
        if (!itrDaug)
          continue;
        if (((itrDaug->barcode() % m_barcodeShift == thePart->barcode() % m_barcodeShift) ||
             // brem on generator level for tau
             (EndVert->nOutgoingParticles() == 1 && EndVert->nIncomingParticles() == 1 &&
              itrDaug->barcode() < m_barcodeG4Shift && thePart->barcode() < m_barcodeG4Shift)) &&
            itrDaug->pdgId() == thePart->pdgId()) {
          samePart = true;
          pVert = itrDaug->decayVtx();
        }
      } // cycle itrDaug
      if (samePart)
        EndVert = pVert;
    } while (pVert != nullptr && pVert != EndVert); // pVert!=EndVert to prevent Sherpa loop

  } // EndVert

  return EndVert;
}
//---------------------------------------------------------------------------------
ParticleOutCome
MCTruthClassifier::defOutComeOfElectron(const xAOD::TruthParticle* thePart) const
{
  //---------------------------------------------------------------------------------

  ParticleOutCome PartOutCome = UnknownOutCome;

  const xAOD::TruthVertex* EndVert = findEndVert(thePart);

  if (EndVert == nullptr)
    return NonInteract;

  int NumOfElecDaug(0);

  int ElecOutNumOfNucFr(0);

  int ElecOutNumOfElec(0);

  int NumOfHadr(0);
  long EndDaugType(0);
  NumOfElecDaug = EndVert->nOutgoingParticles();
  for (unsigned int ipOut = 0; ipOut < EndVert->nOutgoingParticles(); ipOut++) {
    if (!EndVert->outgoingParticle(ipOut))
      continue;
    EndDaugType = EndVert->outgoingParticle(ipOut)->pdgId();
    if (MC::PID::isElectron(EndDaugType))
      ElecOutNumOfElec++;
    if (isHadron(EndVert->outgoingParticle(ipOut)))
      NumOfHadr++;
    if (EndDaugType > 1000000000 || EndDaugType == 0 || abs(EndDaugType) == 2212 || abs(EndDaugType) == 2112)
      ElecOutNumOfNucFr++;
  } // cycle itrDaug

  if (ElecOutNumOfNucFr != 0 || NumOfHadr != 0)
    PartOutCome = NuclInteraction;
  if (ElecOutNumOfElec == 1 && NumOfElecDaug == 1)
    PartOutCome = ElectrMagInter;

  return PartOutCome;
}
//---------------------------------------------------------------------------------
ParticleOutCome
MCTruthClassifier::defOutComeOfMuon(const xAOD::TruthParticle* thePart) const
{
  //---------------------------------------------------------------------------------

  ParticleOutCome PartOutCome = UnknownOutCome;

  const xAOD::TruthVertex* EndVert = findEndVert(thePart);

  if (EndVert == nullptr)
    return NonInteract;
  int NumOfMuDaug(0);

  int MuOutNumOfNucFr(0);

  int NumOfHadr(0);
  int NumOfEleNeutr(0);

  int NumOfMuonNeutr(0);

  int NumOfElec(0);
  long EndDaugType(0);

  NumOfMuDaug = EndVert->nOutgoingParticles();
  for (unsigned int ipOut = 0; ipOut < EndVert->nOutgoingParticles(); ipOut++) {
    if (!EndVert->outgoingParticle(ipOut))
      continue;
    EndDaugType = EndVert->outgoingParticle(ipOut)->pdgId();
    if (MC::PID::isElectron(EndDaugType))
      NumOfElec++;
    if (abs(EndDaugType) == 12)
      NumOfEleNeutr++;
    if (abs(EndDaugType) == 14)
      NumOfMuonNeutr++;
    if (isHadron(EndVert->outgoingParticle(ipOut)))
      NumOfHadr++;
    if (EndDaugType > 1000000000 || EndDaugType == 0 || abs(EndDaugType) == 2212 || abs(EndDaugType) == 2112)
      MuOutNumOfNucFr++;
  } // cycle itrDaug

  if (MuOutNumOfNucFr != 0 || NumOfHadr != 0)
    PartOutCome = NuclInteraction;
  if (NumOfMuDaug == 3 && NumOfElec == 1 && NumOfEleNeutr == 1 && NumOfMuonNeutr == 1)
    PartOutCome = DecaytoElectron;

  return PartOutCome;
}
//---------------------------------------------------------------------------------
ParticleOutCome
MCTruthClassifier::defOutComeOfTau(const xAOD::TruthParticle* thePart, Info* info) const
{
  //---------------------------------------------------------------------------------

  ATH_MSG_DEBUG("Executing defOutComeOfTau");

  ParticleOutCome PartOutCome = UnknownOutCome;

  const xAOD::TruthVertex* EndVert = findEndVert(thePart);

  if (EndVert == nullptr)
    return NonInteract;
  int NumOfTauDaug(0);

  int NumOfNucFr(0);
  int NumOfElec(0);

  int NumOfElecNeut(0);

  int NumOfTauNeut(0);
  int NumOfMuon(0);

  int NumOfMuonNeut(0);

  int NumOfPhot(0);

  int NumOfPi(0);

  int NumOfKaon(0);

  NumOfTauDaug = EndVert->nOutgoingParticles();
  std::vector<const xAOD::TruthParticle*> tauFinalStatePart = findFinalStatePart(EndVert);

  for (auto& i : tauFinalStatePart) {
    long pdg = i->pdgId();
    if (MC::PID::isElectron(pdg))
      NumOfElec++;
    else if (MC::PID::isMuon(pdg))
      NumOfMuon++;
    else if (abs(pdg) == 12)
      NumOfElecNeut++;
    else if (abs(pdg) == 14)
      NumOfMuonNeut++;
    else if (abs(pdg) == 16)
      NumOfTauNeut++;
    else if (MC::PID::isPhoton(pdg))
      NumOfPhot++;
    else if (abs(pdg) == 211)
      NumOfPi++;
    else if (abs(pdg) == 321)
      NumOfKaon++;
    else if (pdg > 1000000000 || pdg == 0)
      NumOfNucFr++;
  }

  if (info)
    info->tauFinalStatePart = std::move(tauFinalStatePart);

  if (NumOfNucFr != 0)
    PartOutCome = NuclInteraction;
  if ((NumOfTauDaug == 3 && NumOfElec == 1 && NumOfElecNeut == 1) ||
      (NumOfTauDaug == (3 + NumOfPhot) && NumOfElecNeut == 1))
    PartOutCome = DecaytoElectron;
  if ((NumOfTauDaug == 3 && NumOfMuon == 1 && NumOfMuonNeut == 1) ||
      (NumOfTauDaug == (3 + NumOfPhot) && NumOfMuonNeut == 1))
    PartOutCome = DecaytoMuon;

  if (NumOfPi == 1 || NumOfKaon == 1)
    PartOutCome = OneProng;
  if (NumOfPi + NumOfKaon == 3)
    PartOutCome = ThreeProng;
  if (NumOfPi + NumOfKaon == 5)
    PartOutCome = FiveProng;

  ATH_MSG_DEBUG("defOutComeOfTau succeeded");

  return PartOutCome;
}
//---------------------------------------------------------------------------------
std::vector<const xAOD::TruthParticle*>
MCTruthClassifier::findFinalStatePart(const xAOD::TruthVertex* EndVert) const
{
  //---------------------------------------------------------------------------------

  std::vector<const xAOD::TruthParticle*> finalStatePart;

  if (EndVert == nullptr)
    return finalStatePart;

  for (unsigned int ipOut = 0; ipOut < EndVert->nOutgoingParticles(); ipOut++) {

    const xAOD::TruthParticle* thePart = EndVert->outgoingParticle(ipOut);
    if (!thePart)
      continue;

    finalStatePart.push_back(thePart);
    if (thePart->status() != 1) {

      const xAOD::TruthVertex* pVert = findEndVert(thePart);
      if (pVert == EndVert)
        break; // to prevent Sherpa  loop
      std::vector<const xAOD::TruthParticle*> vecPart;
      if (pVert != nullptr) {
        vecPart = findFinalStatePart(pVert);
        if (!vecPart.empty())
          for (auto i : vecPart)
            finalStatePart.push_back(i);
      }
    }

  } // cycle itrDaug

  return finalStatePart;
}
//---------------------------------------------------------------------------------
ParticleOutCome
MCTruthClassifier::defOutComeOfPhoton(const xAOD::TruthParticle* thePart) const
{
  //---------------------------------------------------------------------------------

  ParticleOutCome PartOutCome = UnknownOutCome;
  const xAOD::TruthVertex* EndVert = findEndVert(thePart);
  if (EndVert == nullptr)
    return UnConverted;

  int PhtOutNumOfNucFr(0);

  int PhtOutNumOfEl(0);

  int PhtOutNumOfPos(0);
  int NumOfPhtDaug(0);

  int PhtOutNumOfHadr(0);
  // int  PhtOutNumOfPi(0);
  long EndDaugType(0);

  NumOfPhtDaug = EndVert->nOutgoingParticles();
  for (unsigned int ipOut = 0; ipOut < EndVert->nOutgoingParticles(); ipOut++) {
    if (!EndVert->outgoingParticle(ipOut))
      continue;
    EndDaugType = EndVert->outgoingParticle(ipOut)->pdgId();
    if (EndDaugType > 1000000000 || EndDaugType == 0 || abs(EndDaugType) == 2212 || abs(EndDaugType) == 2112)
      PhtOutNumOfNucFr++;
    if (EndDaugType == 11)
      PhtOutNumOfEl++;
    if (EndDaugType == -11)
      PhtOutNumOfPos++;
    // if(abs(EndDaugType)==211||EndDaugType==111) PhtOutNumOfPi++;
    if (isHadron(EndVert->outgoingParticle(ipOut)))
      PhtOutNumOfHadr++;
  } // cycle itrDaug

  if (PhtOutNumOfEl == 1 && PhtOutNumOfPos == 1 && NumOfPhtDaug == 2)
    PartOutCome = Converted;
  if ((NumOfPhtDaug > 1 && PhtOutNumOfNucFr != 0) || PhtOutNumOfHadr > 0)
    PartOutCome = NuclInteraction;

  return PartOutCome;
}

//---------------------------------------------------------------------------------
std::pair<ParticleType, ParticleOrigin>
MCTruthClassifier::checkOrigOfBkgElec(const xAOD::TruthParticle* theEle, Info* info /*= nullptr*/) const
{
  //---------------------------------------------------------------------------------

  ATH_MSG_DEBUG("executing CheckOrigOfBkgElec  " << theEle);

  std::pair<ParticleType, ParticleOrigin> part;
  part.first = Unknown;
  part.second = NonDefined;

  if (theEle == nullptr)
    return part;

  SG::ReadHandle<xAOD::TruthParticleContainer> truthParticleContainerReadHandle(m_truthParticleContainerKey);

  if (!truthParticleContainerReadHandle.isValid()) {
    ATH_MSG_WARNING(
      " Invalid ReadHandle for xAOD::TruthParticleContainer with key: " << truthParticleContainerReadHandle.key());
    return part;
  }

  ATH_MSG_DEBUG("xAODTruthParticleContainer with key  " << truthParticleContainerReadHandle.key()
                                                        << " has valid ReadHandle ");

  Info tmpinfo;
  if (!info)
    info = &tmpinfo;
  part = particleTruthClassifier(theEle, info);

  if (part.first != BkgElectron || part.second != PhotonConv)
    return part;

  const xAOD::TruthParticle* thePart(nullptr);

  if ((MC::PID::isElectron(info->photonMotherPDG) || MC::PID::isMuon(info->photonMotherPDG) ||
       MC::PID::isTau(info->photonMotherPDG) ||
       isHadron(info->photonMother)) // Note: local isHadron rejects beam protons
      && info->photonMotherStatus < 3) {
    do {
      const xAOD::TruthParticle* theMotherPart =
        barcode_to_particle(truthParticleContainerReadHandle.ptr(), info->photonMotherBarcode % m_barcodeShift);
      if (theMotherPart == nullptr || theMotherPart == thePart)
        break;
      thePart = theMotherPart;

      part.first = Unknown;
      part.second = NonDefined;
      part = particleTruthClassifier(thePart, info);
    } while (part.first == BkgElectron && part.second == PhotonConv &&
             (MC::PID::isElectron(info->photonMotherPDG) || MC::PID::isMuon(info->photonMotherPDG) ||
              MC::PID::isTau(info->photonMotherPDG) || isHadron(info->photonMother)));

    if (part.first == BkgElectron && part.second == PhotonConv) {
      // in case of photon from gen particle  classify photon
      // part=particleTruthClassifier(mother);
      thePart = barcode_to_particle(truthParticleContainerReadHandle.ptr(), info->motherBarcode % m_barcodeShift);
      if (thePart != nullptr)
        part = particleTruthClassifier(thePart, info);

    } else if (part.first == GenParticle && isHadron(thePart)) {
      // to fix Alpgen hadrons with status code !=1 (>100)
      part.first = Hadron;
      part.second = NonDefined;
    }

  } else {
    // in case of photon from gen particle  classify photon
    thePart = barcode_to_particle(truthParticleContainerReadHandle.ptr(), info->motherBarcode % m_barcodeShift);
    if (thePart != nullptr)
      part = particleTruthClassifier(thePart, info);
  }

  info->bkgElecMother = thePart;
  return part;
}
//------------------------------------------------------------------------
double
MCTruthClassifier::partCharge(const xAOD::TruthParticle* thePart)
{
  //------------------------------------------------------------------------
  if (thePart == nullptr)
    return 0.;
  return thePart->charge();
}

//------------------------------------------------------------------------
const xAOD::TruthParticle*
MCTruthClassifier::barcode_to_particle(const xAOD::TruthParticleContainer* TruthTES, int bc)
{
  //------------------------------------------------------------------------
  // temporary solution?
  const xAOD::TruthParticle* ptrPart = nullptr;

  for (const auto truthParticle : *TruthTES) {
    if (truthParticle->barcode() == bc) {
      ptrPart = truthParticle;
      break;
    }
  }
  return ptrPart;
}

//------------------------------------------------------------------------
const xAOD::TruthParticle*
MCTruthClassifier::isHadronFromB(const xAOD::TruthParticle* p) const
{
  // If we have reached a dead end, stop here
  if (!p)
    return nullptr;
  // If we have struck a bottom hadron or b-quark, then this is from a b and we return it
  int pid = abs(p->pdgId());
  if (p->isBottomHadron() || pid == MC::PID::BQUARK)
    return p;
  // End cases -- if we have hit anything fundamental other than a c-quark, stop
  if (pid == MC::PID::CQUARK && abs(p->pdgId()) < 100)
    return nullptr;
  // If we hit a BSM particle or nucleus, stop
  if (MC::PID::isNucleus(pid) || MC::PID::isBSM(pid))
    return nullptr;
  // Check for loops and dead-ends
  if (!p->hasProdVtx())
    return nullptr;
  if (p->prodVtx()->nIncomingParticles() == 0)
    return nullptr;
  if (p->hasDecayVtx() && abs(p->prodVtx()->barcode()) > abs(p->decayVtx()->barcode()))
    return nullptr;
  // Otherwise grab the mother and recurse - no need to deal with 2->1 vertices here
  return isHadronFromB(p->prodVtx()->incomingParticle(0));
}
void
MCTruthClassifier::findParticleDaughters(const xAOD::TruthParticle* thePart,
                                         std::set<const xAOD::TruthParticle*>& daughters) const
{

  // Get descendants
  const xAOD::TruthVertex* endVtx = thePart->decayVtx();
  if (endVtx != nullptr) {
    for (unsigned int i = 0; i < endVtx->nOutgoingParticles(); i++) {
      const xAOD::TruthParticle* theDaughter = endVtx->outgoingParticle(i);
      if (theDaughter == nullptr)
        continue;
      if (theDaughter->status() == 1 && theDaughter->barcode() < 200000) {
        // Add descendants with status code 1
        daughters.insert(theDaughter);
      }
      findParticleDaughters(theDaughter, daughters);
    }
  }
}

