/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

// Filename: TruthTools.cxx
// Description:
// Author: Fabian Wilk
// Created: Wed Feb  4 11:05:43 2015

#include "TopParticleLevel/TruthTools.h"
#include "TopEvent/EventTools.h"

#include "xAODTruth/TruthVertex.h"
#include "AthContainers/AuxElement.h"
#include "xAODTruth/xAODTruthHelpers.h"

#include "TruthUtils/PIDHelpers.h"

#include <list>
#include <string>
#include <sstream>
#include <functional>
#include <iostream>

#include "AsgTools/MessageCheck.h"

namespace top {
  namespace truth {
    
    void getRecoMuonHistory(const xAOD::Muon* muon, bool doPartonHistory, bool verbose)
    {
      using namespace asg::msgUserCode;
      
      if(verbose) ATH_MSG_INFO("getRecoMuonHistory:: ---------------entering in function-----------------");
      if(!muon) return;
      
      if(verbose)
      {
        //we retrieve the origin from the MCClassifier, it's useful for crosschecks; see https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/MCTruthClassifier/trunk/MCTruthClassifier/MCTruthClassifierDefs.h
        int origin=0;
        const static SG::AuxElement::Accessor<int> origmu("truthOrigin");
        if (origmu.isAvailable(*muon)) origin = origmu(*muon);
        
        ATH_MSG_INFO("getRecoMuonHistory::called on muon with pt="<<muon->pt()<<" origin="<<origin);
      }
      
      if(muon->isAvailable<const xAOD::TruthParticle*>("truthMuonLink")) return; //this means we already ran on this muon in some way
      const xAOD::TruthParticle* truthmu = 0; //pointer to the truth muon associated with the reco muon
      
      //first we retrieve the truth muon associated with the reco one
      const xAOD::TrackParticle* track = muon->primaryTrackParticle();
      if(track) truthmu = xAOD::TruthHelpers::getTruthParticle(*track); //note that the truth particle associated with a muon can be e.g. a pion/kaon in some cases (since pion/kaon decays are not done at generator level, but at simulation level); getTruthMuonHistory will take care of this case
      
      if(verbose) ATH_MSG_INFO("getRecoMuonHistory:: retrieved truth muon pointer="<<truthmu);
      
      if(truthmu) getTruthMuonHistory(truthmu, doPartonHistory, verbose);
      
      //we attach the truth muon to the reco muon
      muon->auxdecor<const xAOD::TruthParticle*>("truthMuonLink") = truthmu;
      muon->auxdecor<top::LepParticleOriginFlag>("LepParticleOriginFlag") = truthmu ? truthmu->auxdecor<top::LepParticleOriginFlag>("LepParticleOriginFlag") : top::LepParticleOriginFlag::Unknown;
      muon->auxdecor<const xAOD::TruthParticle*>("truthMotherLink") = truthmu ? truthmu->auxdecor<const xAOD::TruthParticle*>("truthMotherLink") : 0;
      muon->auxdecor<const xAOD::TruthParticle*>("truthBMotherLink") = truthmu ? truthmu->auxdecor<const xAOD::TruthParticle*>("truthBMotherLink") : 0;
      muon->auxdecor<const xAOD::TruthParticle*>("truthCMotherLink") = truthmu ? truthmu->auxdecor<const xAOD::TruthParticle*>("truthCMotherLink") : 0;
      muon->auxdecor<const xAOD::TruthParticle*>("truthDirectBosonMotherLink") = truthmu ? truthmu->auxdecor<const xAOD::TruthParticle*>("truthDirectBosonMotherLink")  : 0;
      
      if(doPartonHistory)
      {
        muon->auxdecor<top::LepPartonOriginFlag>("LepPartonOriginFlag") = truthmu ? truthmu->auxdecor<top::LepPartonOriginFlag>("LepPartonOriginFlag") : top::LepPartonOriginFlag::Unknown;
        muon->auxdecor<const xAOD::TruthParticle*>("truthPartonMotherLink") = truthmu ? truthmu->auxdecor<const xAOD::TruthParticle*>("truthPartonMotherLink") : 0;
        muon->auxdecor<const xAOD::TruthParticle*>("truthTopMotherLink") = truthmu ? truthmu->auxdecor<const xAOD::TruthParticle*>("truthTopMotherLink") : 0;
      }
      
      if(verbose) ATH_MSG_INFO("getRecoMuonHistory:: exiting function");
      return;
    }
    
    ////getTruthMuonHistory/////
    void getTruthMuonHistory(const xAOD::TruthParticle* truthmu, bool doPartonHistory, bool verbose)
    {
      using namespace asg::msgUserCode;
      
      if(verbose)  ATH_MSG_INFO("getTruthMuonHistory:: entering in function");
      
      if(!truthmu && verbose) ATH_MSG_INFO("getTruthMuonHistory:: -> called on empty truth muon");
      if(truthmu && verbose) ATH_MSG_INFO("getTruthMuonHistory:: -> called on truth particle with pdgId="<<truthmu->pdgId()<<" barcode="<<truthmu->barcode()<<" status="<<truthmu->status()<<" pt="<<truthmu->pt());

      top::LepParticleOriginFlag lepParticleOriginFlag=top::LepParticleOriginFlag::Unknown; //flag defined in TopEvent/EventTools.h
      const xAOD::TruthParticle* truthmu_mother=0; //direct mother of the muon
      const xAOD::TruthParticle* truthmu_Bmother=0; //first bottom hadron encountered when going through ancestors
      const xAOD::TruthParticle* truthmu_Cmother=0; //first charmed hadron encountered when going through ancestors
      const xAOD::TruthParticle* truthmu_DirectBosonMother=0; //this will hold e.g. W->mu or W->tau->mu decays, BUT NOT W->hadrons->mu decays!
      
      if(truthmu && abs(truthmu->pdgId())!=13) //the truth particle associated with a muon can be e.g. a pion/kaon in some cases (since pion/kaon decays are not done at generator level, but at simulation level)
      {
         truthmu_mother=truthmu; 
         truthmu=0; //we don't have the truth muon in this case
      }
      
      bool alreadyDecorated=false; //let's check if we already ran on the truh muon associated with the reco one
      if(truthmu)
      {
        if(truthmu->isAvailable<top::LepParticleOriginFlag>("LepParticleOriginFlag")) //we already decorated this truth muon, let's not waste time and recover the info from there
        {
          if(verbose) ATH_MSG_INFO("getTruthMuonHistory::--> truth muon already decorated ");
          lepParticleOriginFlag=truthmu->auxdecor<top::LepParticleOriginFlag>("LepParticleOriginFlag");
          truthmu_mother=truthmu->auxdecor<const xAOD::TruthParticle*>("truthMotherLink");
          truthmu_Bmother=truthmu->auxdecor<const xAOD::TruthParticle*>("truthBMotherLink");
          truthmu_Cmother=truthmu->auxdecor<const xAOD::TruthParticle*>("truthCMotherLink");
          truthmu_DirectBosonMother=truthmu->auxdecor<const xAOD::TruthParticle*>("truthDirectBosonMotherLink");
          alreadyDecorated=true;
        }
        else truthmu_mother=top::truth::getFirstDifferentParent(truthmu,verbose); //in this case we retrieve the mother of the muon
      }//end if(truthmu)
      
      if(truthmu_mother && abs(truthmu_mother->pdgId())!=13 && !alreadyDecorated) //let's then look at the history of this muon
      {
        if(verbose) ATH_MSG_INFO("getTruthMuonHistory::mother pdgId="<<truthmu_mother->pdgId()<<" isB="<<truthmu_mother->isBottomHadron()<<" isC="<<truthmu_mother->isCharmHadron()<<" isLight="<<truthmu_mother->isLightHadron());
        
        if(truthmu_mother->isLightHadron()) lepParticleOriginFlag=top::LepParticleOriginFlag::FromLightHadron; //in this case we set the muon as coming from a light hadron and we don't investigate further
        else if(abs(truthmu_mother->pdgId())>=22 && abs(truthmu_mother->pdgId())<=25) //W->munu or Z/H/gamma*->mumu decay
        {
          lepParticleOriginFlag=top::LepParticleOriginFlag::FromLeptonicBoson;
          truthmu_DirectBosonMother=truthmu_mother;
        }
        else if(abs(truthmu_mother->pdgId())==15) //muon from tau
        {
          const xAOD::TruthParticle* tauMother = top::truth::getFirstDifferentParent(truthmu_mother,verbose);
          lepParticleOriginFlag=top::truth::getTruthMuonFromTauHistory(tauMother,truthmu_Bmother,truthmu_Cmother,truthmu_DirectBosonMother,verbose);
        }//end of muon from tau
        else if(truthmu_mother->isBottomHadron())  //muon from B->mu+X decay
        {
          lepParticleOriginFlag=top::LepParticleOriginFlag::FromB;
          truthmu_Bmother=truthmu_mother;
        }
        else if(truthmu_mother->isCharmHadron())  //muon from D->mu+X decay, now we have to understand where the D is coming from
        {
          truthmu_Cmother=truthmu_mother;
          lepParticleOriginFlag=top::truth::getTruthMuonFromCharmHistory(truthmu_Bmother,truthmu_Cmother,verbose);
        }//end of if
      }//end of if(truthmu_mother)
      
      if(verbose)
      {
        ATH_MSG_INFO("getTruthMuonHistory::---decay chain---");
        if(truthmu) printDecayChain(truthmu);
        else ATH_MSG_INFO("getTruthMuonHistory::->not available");
        ATH_MSG_INFO("getTruthMuonHistory::LepParticleOriginFlag="<<static_cast<int>(lepParticleOriginFlag));
        if(truthmu) ATH_MSG_INFO("getTruthMuonHistory::-->truth muon pdgId="<<truthmu->pdgId()<<" barcode="<<truthmu->barcode()<<" pt="<<truthmu->pt());
        else ATH_MSG_INFO("getTruthMuonHistory::-->truth muon not found");
        if(truthmu_mother) ATH_MSG_INFO("getTruthMuonHistory::-->truth muon mother pdgId="<<truthmu_mother->pdgId()<<" barcode="<<truthmu_mother->barcode()<<" pt="<<truthmu_mother->pt());
        else ATH_MSG_INFO("getTruthMuonHistory::-->truth muon mother not found");
        if(truthmu_Bmother) ATH_MSG_INFO("getTruthMuonHistory::-->truth muon Bmother pdgId="<<truthmu_Bmother->pdgId()<<" barcode="<<truthmu_Bmother->barcode()<<" pt="<<truthmu_Bmother->pt());
        else ATH_MSG_INFO("getTruthMuonHistory::-->truth muon Bmother not found");
        if(truthmu_Cmother) ATH_MSG_INFO("getTruthMuonHistory::-->truth muon Cmother pdgId="<<truthmu_Cmother->pdgId()<<" barcode="<<truthmu_Cmother->barcode()<<" pt="<<truthmu_Cmother->pt());
        else ATH_MSG_INFO("getTruthMuonHistory::-->truth muon Cmother not found");
        if(truthmu_DirectBosonMother) ATH_MSG_INFO("getTruthMuonHistory::-->truth muon Direct Boson mother pdgId="<<truthmu_DirectBosonMother->pdgId()<<" barcode="<<truthmu_DirectBosonMother->barcode()<<" pt="<<truthmu_DirectBosonMother->pt());
        else ATH_MSG_INFO("getTruthMuonHistory::->truth muon Direct Boson mother not found");
      }
      
      if(truthmu && !truthmu->isAvailable<top::LepParticleOriginFlag>("LepParticleOriginFlag"))
      {
        //we decorate also on the truth muon associated with the reco one, because in this way if the truth muon was already decorated in another copy of the muon (for systematics) we avoid losing time.
        truthmu->auxdecor<top::LepParticleOriginFlag>("LepParticleOriginFlag") = lepParticleOriginFlag;
        truthmu->auxdecor<const xAOD::TruthParticle*>("truthMotherLink") = truthmu_mother; 
        truthmu->auxdecor<const xAOD::TruthParticle*>("truthBMotherLink") = truthmu_Bmother;
        truthmu->auxdecor<const xAOD::TruthParticle*>("truthCMotherLink") = truthmu_Cmother;
        truthmu->auxdecor<const xAOD::TruthParticle*>("truthDirectBosonMotherLink") = truthmu_DirectBosonMother;
      }
      
      if(doPartonHistory && truthmu && !truthmu->isAvailable<const xAOD::TruthParticle*>("truthPartonMotherLink")) top::truth::getTruthMuonPartonHistory(truthmu, lepParticleOriginFlag, truthmu_mother, truthmu_Bmother, truthmu_Cmother, truthmu_DirectBosonMother, verbose);
      
      if(verbose) ATH_MSG_INFO("getTruthMuonHistory:: exiting from function");
      
      return;
    }
    
    /////getTruthMuonPartonHistory/////
    top::LepPartonOriginFlag getTruthMuonPartonHistory(const xAOD::TruthParticle* truthmu, top::LepParticleOriginFlag lepParticleOriginFlag, const xAOD::TruthParticle* truthmu_mother, const xAOD::TruthParticle* truthmu_Bmother, const xAOD::TruthParticle* truthmu_Cmother,  const xAOD::TruthParticle* truthmu_DirectBosonMother, bool verbose)
    {
      using namespace asg::msgUserCode;
      
      const xAOD::TruthParticle* truthmu_PartonMother=0;
      const xAOD::TruthParticle* truthmu_TopMother=0;
      top::LepPartonOriginFlag lepPartonOriginFlag=top::LepPartonOriginFlag::Unknown;
      
      bool alreadyDecorated=false;
      
      if(truthmu && truthmu->isAvailable<const xAOD::TruthParticle*>("truthPartonMotherLink")) //we already decorated the truth muon, let's read from there
      {
        truthmu_PartonMother=truthmu->auxdecor<const xAOD::TruthParticle*>("truthPartonMotherLink") ;
        truthmu_TopMother=truthmu->auxdecor<const xAOD::TruthParticle*>("truthTopMotherLink");
        lepPartonOriginFlag=truthmu->auxdecor<top::LepPartonOriginFlag>("LepPartonOriginFlag");
        alreadyDecorated=true;
      }
      
      if(!alreadyDecorated)
      {
        if(lepParticleOriginFlag==top::LepParticleOriginFlag::Unknown || lepParticleOriginFlag==top::LepParticleOriginFlag::FromLightHadron) lepPartonOriginFlag=top::LepPartonOriginFlag::Unknown; //we always consider muons from light hadrons to be from unknown origin, to avoid problems
        else if(truthmu_mother)
        {
          if(truthmu_DirectBosonMother) //the muon is coming from a leptonically decaying boson (eventually via a boson->tau->mu)
          {
            truthmu_PartonMother=truthmu_DirectBosonMother;
            const xAOD::TruthParticle* bosonMother=top::truth::getFirstDifferentParent(truthmu_DirectBosonMother,verbose);
            if(!bosonMother || abs(bosonMother->pdgId())!=6) lepPartonOriginFlag=top::LepPartonOriginFlag::FromLeptonicBoson;
            else
            {
              if(bosonMother->pdgId()==6) lepPartonOriginFlag=top::LepPartonOriginFlag::FromTopViaLeptonicBoson;
              if(bosonMother->pdgId()==(-6)) lepPartonOriginFlag=top::LepPartonOriginFlag::FromAntiTopViaLeptonicBoson;
              truthmu_TopMother=bosonMother;
            }
          }
          else  //in this case the muon is not coming from a boson
          {
            truthmu_PartonMother=getFirstPartonParent(truthmu_mother,verbose);
            truthmu_TopMother=getFirstSpecificParent(truthmu_PartonMother,6,true,verbose);
            
            const xAOD::TruthParticle* firstNonLeptonMother=0;
            if(!truthmu_mother->isLepton()) firstNonLeptonMother=truthmu_mother;
            else firstNonLeptonMother=top::truth::getFirstNonLeptonParent(truthmu_mother,verbose);
            
            if(firstNonLeptonMother)
            {
              if(abs(firstNonLeptonMother->pdgId())>=22 && abs(firstNonLeptonMother->pdgId())<=25 && verbose) ATH_MSG_WARNING("getTruthMuonHistory:: we should have already found leptonically decaying boson...");
              
              if(firstNonLeptonMother->isBottomHadron() || firstNonLeptonMother->isCharmHadron())
              {
                const xAOD::TruthParticle* hadron=0;
                if(truthmu_Bmother) hadron=truthmu_Bmother;
                else if(truthmu_Cmother) hadron=truthmu_Cmother;
                else if(verbose) ATH_MSG_WARNING("getTruthMuonHistory:: we were expecting a HF hadron here...");
                
                if(hadron)
                {
                  std::vector<int> possiblePdgIds = {6,21,22,23,24,25}; //top, gluon, photon, Z, W, higgs
                  const xAOD::TruthParticle* motherOfHadron=getFirstParentAmongList(hadron,possiblePdgIds,true,verbose);
                  if(motherOfHadron)
                  {
                    if(abs(motherOfHadron->pdgId())==21) lepPartonOriginFlag=top::LepPartonOriginFlag::FromGluonToHFHadron;
                    if(motherOfHadron->pdgId()==6) lepPartonOriginFlag=top::LepPartonOriginFlag::FromTopViaQuarkToHF;
                    if(motherOfHadron->pdgId()==(-6)) lepPartonOriginFlag=top::LepPartonOriginFlag::FromAntiTopViaQuarkToHF;
                    if(abs(motherOfHadron->pdgId())>=22 && abs(motherOfHadron->pdgId())<=25)
                    {
                      const xAOD::TruthParticle* motherOfBoson=getFirstDifferentParent(motherOfHadron,verbose);
                      if(motherOfBoson && motherOfBoson->pdgId()==6) lepPartonOriginFlag=top::LepPartonOriginFlag::FromTopViaHadronicBosonToHF;
                      else if(motherOfBoson && motherOfBoson->pdgId()==(-6)) lepPartonOriginFlag=top::LepPartonOriginFlag::FromAntiTopViaHadronicBosonToHF;
                      else{
                        lepPartonOriginFlag=top::LepPartonOriginFlag::FromHadronicBosonToHF;
                      }
                    }
                  }//end of if(motherOfHadron)
                }//end of if(hadron)
              }//end of else if(firstNonLeptonMother->isBottomHadron() || firstNonLeptonMother->isCharmHadron())
              else{
                if(verbose) ATH_MSG_INFO("getTruthMuonHistory:: uncathegorized non-lepton parent has pgdId="<<firstNonLeptonMother->pdgId());
              }
            }//end of if(firstNonLeptonMother)
            
          }//end of non-bosonic case
          
        }//end of else if(truthmu_mother)
      }//end of if(!alreadyDecorated)
      
      if(truthmu && !truthmu->isAvailable<const xAOD::TruthParticle*>("truthPartonMotherLink"))
      {
        truthmu->auxdecor<const xAOD::TruthParticle*>("truthPartonMotherLink") = truthmu_PartonMother; 
        truthmu->auxdecor<const xAOD::TruthParticle*>("truthTopMotherLink") = truthmu_TopMother;
        truthmu->auxdecor<top::LepPartonOriginFlag>("LepPartonOriginFlag") = lepPartonOriginFlag;
      }
      
      if(verbose)
      {
        if(truthmu_PartonMother) ATH_MSG_INFO("getTruthMuonHistory::------> parton mother="<<truthmu_PartonMother->pdgId());
        else ATH_MSG_INFO("getTruthMuonHistory::------> parton mother not available");
        if(truthmu_TopMother) ATH_MSG_INFO("getTruthMuonHistory::------> top mother="<<truthmu_TopMother->pdgId());
        else ATH_MSG_INFO("getTruthMuonHistory::------> top mother not available");
        ATH_MSG_INFO("getTruthMuonHistory::------> LepPartonOriginFlag="<<static_cast<int>(lepPartonOriginFlag));
      }
      
      return lepPartonOriginFlag;
      
    }//end of getTruthMuonPartonHistory
    
    /////getTruthMuonFromCharmHistory/////
    top::LepParticleOriginFlag getTruthMuonFromCharmHistory(const xAOD::TruthParticle* &truthmu_Bmother, const xAOD::TruthParticle* truthmu_Cmother, bool verbose)
    {
      
      const xAOD::TruthParticle* mother_of_C_hadron=getFirstDifferentParentExcludingHadrons(truthmu_Cmother,4,verbose); //we find the first non-Chadron ancestor of the C-hadron
      
      if(!mother_of_C_hadron) return top::LepParticleOriginFlag::FromC; //we don't know where the C-hadron is coming from
      if(mother_of_C_hadron->isBottomHadron()) //B->D->mu case
      {
        truthmu_Bmother=mother_of_C_hadron;
        return top::LepParticleOriginFlag::FromBtoC;
      }
      return top::LepParticleOriginFlag::FromC; //in other cases this is coming from D->mu+X or something
      
    } //end of getTruthMuonFromCharmHistory method
    
    /////getTruthMuonFromTauHistory/////
    top::LepParticleOriginFlag getTruthMuonFromTauHistory(const xAOD::TruthParticle* tauMother, const xAOD::TruthParticle* &truthmu_Bmother, const xAOD::TruthParticle* &truthmu_Cmother, const xAOD::TruthParticle* &truthmu_DirectBosonMother, bool verbose)
    {
      if(!tauMother) return top::LepParticleOriginFlag::FromTau; //muon from tau, we don't know where the tau is coming from
      if(abs(tauMother->pdgId())>=22 && abs(tauMother->pdgId())<=25) //e.g. W->taunu->mununu decay
      {
        truthmu_DirectBosonMother=tauMother;
        return top::LepParticleOriginFlag::FromLeptonicBosonToTau;
      }
      if(tauMother->isBottomHadron()) //the muon is from a B->tau+X with tau->mununu decay
      {
        truthmu_Bmother=tauMother;
        return top::LepParticleOriginFlag::FromBtoTau;
      }
      if(tauMother->isCharmHadron()) //now this is getting complicated, the muon is from e.g. D->tau+X with tau->mununu decay, now we have to understand where the D is coming from...
      {
        truthmu_Cmother=tauMother;
        const xAOD::TruthParticle* mother_of_C_hadron=getFirstDifferentParentExcludingHadrons(truthmu_Cmother,4,verbose); //we find the first non-Chadron ancestor of the C-hadron
      
        if(!mother_of_C_hadron) return top::LepParticleOriginFlag::FromCtoTau; // we don't know where the C-hadron is coming from
        if(mother_of_C_hadron->isBottomHadron()) //B->D->mu case
        {
          truthmu_Bmother=mother_of_C_hadron;
          return top::LepParticleOriginFlag::FromBtoCtoTau;
        }
        else  //in this case this is coming from D->tau->mu+X or something with no B-hadron ancestor
        {
          return top::LepParticleOriginFlag::FromCtoTau;
        }
      }//end of if(tauMother->isCharmHadron())
      
      return top::LepParticleOriginFlag::FromTau; //in all other cases we don't know anything apart from the fact that the muon is coming from a tau
    }//end of getTruthMuonFromTauHistory method
    
    const xAOD::TruthParticle* getFirstNonLeptonParent(const xAOD::TruthParticle* truthPart, bool verbose)
    {
      using namespace asg::msgUserCode;
      
      if(!truthPart) return 0;
      
      const xAOD::TruthParticle* parent=getFirstDifferentParent(truthPart);
      int niterations=0;
      while(parent)
      {
        if((niterations++)>100)
        {
          if(verbose) ATH_MSG_INFO("getFirstNonLeptonParent:: too many iterations");
          return 0;
        }
        if(!parent->isLepton()) break;        
        parent=getFirstDifferentParent(parent);
      }
      
      return parent;
    }
    
    const xAOD::TruthParticle* getFirstSpecificParent(const xAOD::TruthParticle* truthPart, int pdgId, bool absolute, bool verbose)
    {
      using namespace asg::msgUserCode;
      
      if(!truthPart) return 0;
      
      const xAOD::TruthParticle* parent=getFirstDifferentParent(truthPart);
      int niterations=0;
      while(parent)
      {
        if((niterations++)>100)
        {
          if(verbose) ATH_MSG_INFO("getFirstSpecificParent:: too many iterations; requested pdgId="<<pdgId);
          return 0;
        }
        if(absolute)
        {
          if(abs(parent->pdgId())==abs(pdgId)) break;
        }
        else
        {
          if(parent->pdgId()==pdgId) break;
        }
        parent=getFirstDifferentParent(parent);
      }
      
      return parent;
    }
    
    const xAOD::TruthParticle* getFirstParentAmongList(const xAOD::TruthParticle* truthPart, std::vector<int> &pdgIds, bool absolute, bool verbose)
    {
      using namespace asg::msgUserCode;
      
      if(!truthPart) return 0;
      
      const xAOD::TruthParticle* parent=getFirstDifferentParent(truthPart);
      int niterations=0;
      while(parent)
      {
        if((niterations++)>100)
        {
          if(verbose) ATH_MSG_INFO("getFirstParentAmongList:: too many iterations");
          return 0;
        }
        bool toBreak=false;
        for(int pdgId : pdgIds)
        {
          int id1=pdgId;
          int id2=parent->pdgId();
          if(absolute)
          {
            id1=abs(id1);
            id2=abs(id2);
          }
          if(id1==id2)
          {
            toBreak=true;
            break;
          }
        }
        if(toBreak) break;
        parent=getFirstDifferentParent(parent);
      }
      
      return parent;
    }
    
    const xAOD::TruthParticle* getFirstPartonParent(const xAOD::TruthParticle* truthPart, bool verbose)
    {
      using namespace asg::msgUserCode;
      
      if(!truthPart) return 0;
      
      const xAOD::TruthParticle* parent=getFirstDifferentParent(truthPart);
      int niterations=0;
      while(parent)
      {
        if((niterations++)>100)
        {
          if(verbose) ATH_MSG_INFO("getFirstPartonParent:: too many iterations");
          return 0;
        }
        if(parent->isParton()) break;        
        parent=getFirstDifferentParent(parent);
      }
      
      return parent;
    }
    
    const xAOD::TruthParticle* getFirstDifferentParentExcludingHadrons(const xAOD::TruthParticle* truthPart, int whichHadrons, bool verbose)
    {
      using namespace asg::msgUserCode;
      
      if(!truthPart) return 0;
      const xAOD::TruthParticle* parent=getFirstDifferentParent(truthPart);
      int niterations=0;
      while(parent)
      {
        if((niterations++)>100)
        {
          if(verbose) ATH_MSG_INFO("getFirstDifferentParentExcludingHadrons:: too many iterations");
          return 0;
        }
        if(whichHadrons<0  && !parent->isHadron()) break;
        if(whichHadrons==4  && !parent->isCharmHadron()) break;
        if(whichHadrons==5  && !parent->isBottomHadron()) break;
        if(whichHadrons==45  && !parent->isCharmHadron() && !parent->isBottomHadron()) break;
        
        parent=getFirstDifferentParent(parent);
      }
      
      return parent;
      
    }
    
    const xAOD::TruthParticle* getFirstDifferentParent(const xAOD::TruthParticle* truthPart, bool verbose)
    {
      using namespace asg::msgUserCode;
      
      if(!truthPart)
      {
        ATH_MSG_WARNING(" called top::truth::getFirstDifferentParent on a null pointer");
        return 0;
      }
      
      int pdgId=truthPart->pdgId();
      
      const xAOD::TruthParticle* parent=truthPart;
      int parentPdgId=pdgId;
      int niterations=0;
      
      while(parentPdgId==pdgId)
      {
        if((niterations++) >100)
        {
          if(verbose) ATH_MSG_INFO("getFirstDifferentParent:: too many iterations");
          return 0;
        }

        if(parent->nParents()>=1) parent=parent->parent(0);
        else break; //we cannot go further

        if(!parent) break; 
        parentPdgId=parent->pdgId();
        
      }
      return parent;
    }
    
    
    void printDecayChain(const xAOD::TruthParticle* truthPart,
                         std::ostream& os /* = std::cout */,
                         const std::string& prefix /* = "" */) {
      // Protect against a faulty production vertex reference.
      if (!truthPart || !truthPart->prodVtx()) {
        os << prefix << "[" << (truthPart ? truthPart->pdgId() :0x0) << " <- NULL]" << '\n';
        return;
      }

      const xAOD::TruthParticle* parent {
        truthPart->prodVtx()->incomingParticle(0)
      };
      os << prefix << "[" << truthPart->pdgId();
      int niterations=0;
      while (parent) {
        if((niterations++)>30)
        {
          os<<" STOP! too many iterations ";
          break;
        }
        os << " <- " << parent->pdgId();
        
        if (parent->prodVtx()) {
          parent = parent->prodVtx()->incomingParticle(0);
        } else {
          break;
        }
        
      }
      os << "]" << '\n';
    }

    bool isFrom(const xAOD::TruthParticle* truthParticle,
                const std::vector<int>& parentPDGIds,
                bool bOnlyDirect /* = false */) {
      // If the input does not have aproper production vertex reference or
      // there are no incoming particle to the production vertex, directly
      // return false
      if (!truthParticle->hasProdVtx() ||
          !truthParticle->prodVtx() ||
          truthParticle->prodVtx()->nIncomingParticles() == 0) {
        return false;
      }

      // The current production vertex
      const xAOD::TruthVertex* prodVtx = truthParticle->prodVtx();
      // The previous production vertex (initialised to the current
      // production vertex)
      const xAOD::TruthVertex* prevProdVtx = prodVtx;

      auto numIncoming = prodVtx->nIncomingParticles();
      for (std::size_t motherIndex = 0;
           motherIndex < numIncoming;
           ++motherIndex) {
        const xAOD::TruthParticle* mother = nullptr;
        int motherPdgId = truthParticle->pdgId();

        // Ascend through the decay chain until we find the "actual"
        // decay, i.e. a change in the PDG ID. This skips all the
        // non-decay truth table entries which come from "particle
        // evolution" rather than physical decay
        while (truthParticle->pdgId() == motherPdgId) {
          mother = prodVtx->incomingParticle(motherIndex);
          if (mother) {
            motherPdgId = mother->pdgId();
          } else {
            break;
          }

          if (truthParticle->pdgId() != motherPdgId) {
            break;
          }

          // Include protection against cyclic or broken reference
          // chains which can occur in SHERPA samples
          if (!mother->hasProdVtx()) {
            break;
          }
          if (prevProdVtx == mother->prodVtx()) {
            break;
          }

          // Update the previous / current production vertex references
          prevProdVtx = prodVtx;
          prodVtx = mother->prodVtx();

          // safeguard
          if (!prodVtx || prodVtx->nIncomingParticles() == 0) {
            break;
          }
        }

        // Check that the mother pointer is valid. If it is not valid, then
        // the particle could not possibly come from any of the requested
        // PDG Ids, hence return false
        if (!mother) {
          return false;
        }

        // If the mother PDG ID is in the parentPDGIds collection, then return true
        if (std::find(parentPDGIds.begin(), parentPDGIds.end(), motherPdgId) != parentPDGIds.end()) {
          return true;
        }

        // If we allow chained matching (I.e. not only _direct_ decays) and
        // the mother particle does come from the request PDG ID(s), return true
        if (!bOnlyDirect && isFrom(mother, parentPDGIds, bOnlyDirect)) {
          return true;
        }
      }

      // If we did't find an ancestor with the requested pdg id, return false
      return false;
    }

    bool isFromWZDecay(const xAOD::TruthParticle* truthParticle,
                       bool bOnlyDirect /* = false */) {
      // If the input does not have aproper production vertex reference or
      // there are no incoming particle to the production vertex, directly
      // return false
      if (!truthParticle->hasProdVtx() ||
          !truthParticle->prodVtx() ||
          truthParticle->prodVtx()->nIncomingParticles() == 0) {
        return false;
      }

      // The current production vertex
      const xAOD::TruthVertex* prodVtx = truthParticle->prodVtx();
      // The previous production vertex (initialised to the current
      // production vertex)
      const xAOD::TruthVertex* prevProdVtx = prodVtx;


      auto numIncoming = prodVtx->nIncomingParticles();
      for (std::size_t motherIndex = 0;
           motherIndex < numIncoming;
           ++motherIndex) {
        const xAOD::TruthParticle* mother = nullptr;
        int motherAbsPdgId = truthParticle->absPdgId();

        // Ascend through the decay chain until we find the "actual"
        // decay, i.e. a change in the PDG ID. This skips all the
        // non-decay truth table entries which come from "particle
        // evolution" rather than physical decay
        while (truthParticle->absPdgId() == motherAbsPdgId) {
          mother = prodVtx->incomingParticle(motherIndex);
          if (mother) {
            motherAbsPdgId = mother->absPdgId();
          } else {
            break;
          }

          if (truthParticle->absPdgId() != motherAbsPdgId) {
            break;
          }

          // Include protection against cyclic or broken reference
          // chains which can occur in SHERPA samples
          if (!mother->hasProdVtx()) {
            break;
          }
          if (prevProdVtx == mother->prodVtx()) {
            break;
          }

          // Update the previous / current production vertex references
          prevProdVtx = prodVtx;
          prodVtx = mother->prodVtx();

          // safeguard
          if (!prodVtx || prodVtx->nIncomingParticles() == 0) {
            break;
          }
        }

        // Check that the mother pointer is valid. If it is not valid, then
        // the particle could not possibly come from any of the requested
        // PDG Ids, hence return false
        if (!mother) {
          return false;
        }

        // If the direct physical decay in the MC record was from a Z or W
        // boson to the truth particle then return true
        // Otherwise perform a vertex-based identification of W/Z's
        if (motherAbsPdgId == 23 || motherAbsPdgId == 24) {
          return true;
        }

        // The following vertex-based identification of W/Z's is needed
        // for SHERPA samples where the W/Z particle is not explicitly
        // in the particle record. At this point if we have a valid
        // vertex, it should be a true decay vertex.
        //
        // If it is a W or Z then two of those decay products should be
        // lepton / neutrino pair corresponding to the transitions
        //    W+ -> l+ nu
        //    W- -> l- nu~
        //    Z  -> l+ l-
        //    Z  -> nu nu~
        // Hence loop through the outgoing particles of the truth vertex
        // and check that the vertex matches the requirement. We
        // simplify this check by just counting the number of leptonic
        // outgoing particles which should be equal to 2. It is assumed
        // that the MC generator does not produce unphysical decays.
        //
        // Furthermore, prompt W/Z's should come from a vertex with more
        // than one incoming particle. Consequently, test this
        // requirement prior to checking the outgoing states.
        int nDecay = 0;
        if (prodVtx && prodVtx->nIncomingParticles() > 1) {
          for (const auto& child : prodVtx->outgoingParticleLinks()) {
            if ((*child)->absPdgId() > 10 &&
                (*child)->absPdgId() < 17) {
              nDecay++;
            }
          }
        }

        // There must be exactly two leptonic outgoing particles in that vertex
        if (nDecay == 2) {
          return true;
        }

        // If we allow chained matching (I.e. not only _direct_ decays) and
        // the mother particle does come from a W / Z, return true
        if (!bOnlyDirect && isFromWZDecay(mother, bOnlyDirect)) {
          return true;
        }
      }

      // We did not find any W / Z in the ancestry of the input particle ->
      // return false
      return false;
    }

    bool isLeptonFromTau(const xAOD::TruthParticle* truthParticle) {
      // If the input particle is not a lepton directly terminate the
      // algorithm retunring false
      if (truthParticle->absPdgId() != 11 &&
          truthParticle->absPdgId() != 12 &&
          truthParticle->absPdgId() != 13 &&
          truthParticle->absPdgId() != 14 &&
          truthParticle->absPdgId() != 15 &&
          truthParticle->absPdgId() != 16) {
        return false;
      }
      return isFrom(truthParticle, {15, -15}, true);
    }

    bool isNotFromHadron(const xAOD::TruthParticle* truthParticle,
                         bool bOnlyDirect /* = false */,
                         bool bTauIsHadron /* = false */) {
      // If there is no production vertex, either something is wrong, or
      // we are at the beginning of the record. However, either way, the
      // particle does not come from a hadron, hence return true.
      if (!truthParticle->hasProdVtx() ||
          !truthParticle->prodVtx() ||
          truthParticle->prodVtx()->nIncomingParticles() == 0) {
        return true;
      }

      // The current production vertex
      const xAOD::TruthVertex* prodVtx = truthParticle->prodVtx();
      // The previous production vertex (initialised to the current
      // production vertex)
      const xAOD::TruthVertex* prevProdVtx = prodVtx;

      // Loop all the particles going into the production vertex of the
      // current truth particle.
      //   * If the mother of any of the is a physical hadron return false
      //   * If the mother of any of them descends from a physical return false
      //   * Otherwise return true
      auto numIncoming = prodVtx->nIncomingParticles();
      for (std::size_t motherIndex = 0;
           motherIndex < numIncoming;
           ++motherIndex) {
        int motherPdgId = truthParticle->pdgId();
        const xAOD::TruthParticle* mother = nullptr;

        // Ascend through the decay chain until we find the "actual"
        // decay, i.e. a change in the PDG ID. This skips all the
        // non-decay truth table entries which come from "particle
        // evolution" rather than physical decay
        while (truthParticle->pdgId() == motherPdgId) {
          mother = prodVtx->incomingParticle(motherIndex);
          if (mother) {
            motherPdgId = mother->pdgId();
          } else {
            break;
          }

          // Include protection against cyclic or broken reference
          // chains which can occur in SHERPA samples
          if (!mother->hasProdVtx()) {
            break;
          }
          if (prevProdVtx == mother->prodVtx()) {
            break;
          }

          // Update the previous / current production vertex references
          prevProdVtx = prodVtx;
          prodVtx = mother->prodVtx();

          // safeguard
          if (!prodVtx || prodVtx->nIncomingParticles() == 0) {
            break;
          }
        }

        // Check that the mother pointer is valid. If it is not valid, then
        // the particle could not possibly come from a hadron, hence return
        // true
        if (!mother) {
          return true;
        }

        // If the mother particle is a "physical decayed" particle (HepMC
        //   status code) and a hadron, then return false.
        if (mother->status() == 2 &&
            (MC::PID::isHadron(motherPdgId) ||
             (bTauIsHadron && MC::PID::isTau(motherPdgId)))) {
          return false;
        }
        // } else {
        //     std::cout << "Mother is: " << motherPdgId << ", " << mother->status() << std::endl;
        // }

        // If we allow chained matching (I.e. not only _direct_ decays) and
        // the mother particle does come from a hadron, return false
        if (!bOnlyDirect && !isNotFromHadron(mother, bOnlyDirect, bTauIsHadron)) {
          return false;
        }
      }
      // No hadronic && physical parents
      return true;
    }
  }
}
