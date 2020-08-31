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
#include "xAODTruth/TruthParticleContainer.h"

#include "TruthUtils/PIDHelpers.h"

#include <list>
#include <string>
#include <sstream>
#include <functional>
#include <iostream>

#include "TopParticleLevel/MsgCategory.h"

using namespace TopParticleLevel;

namespace top {
  namespace truth {
    
    
    void initCommonMuonHistoryInfo(const xAOD::IParticle* muon, bool doPartonHistory)
    {
      muon->auxdecor<top::LepParticleOriginFlag>("LepParticleOriginFlag") = top::LepParticleOriginFlag::MissingTruthInfo;
      muon->auxdecor<const xAOD::TruthParticle*>("truthMotherLink") = 0;
      muon->auxdecor<const xAOD::TruthParticle*>("truthFirstNonLeptonMotherLink") = 0;
      muon->auxdecor<const xAOD::TruthParticle*>("truthBMotherLink") = 0;
      muon->auxdecor<const xAOD::TruthParticle*>("truthCMotherLink") = 0;
      
      if(doPartonHistory)
      {
        muon->auxdecor<top::LepPartonOriginFlag>("LepPartonOriginFlag") = top::LepPartonOriginFlag::MissingTruthInfo;
        muon->auxdecor<const xAOD::TruthParticle*>("truthPartonMotherLink") = 0;
        muon->auxdecor<const xAOD::TruthParticle*>("truthTopMotherLink") = 0;
        muon->auxdecor<const xAOD::TruthParticle*>("truthWMotherLink") = 0;
        muon->auxdecor<const xAOD::TruthParticle*>("truthZMotherLink") = 0;
        muon->auxdecor<const xAOD::TruthParticle*>("truthPhotonMotherLink") = 0;
        muon->auxdecor<const xAOD::TruthParticle*>("truthHiggsMotherLink") = 0;
        muon->auxdecor<const xAOD::TruthParticle*>("truthBSMMotherLink") = 0;
      }
    }
    
    void initRecoMuonHistoryInfo(const xAOD::Muon* muon, bool doPartonHistory)
    {
      muon->auxdecor<bool>("hasRecoMuonHistoryInfo")=false;
      muon->auxdecor<const xAOD::TruthParticle*>("truthMuonLink") = 0;
    initCommonMuonHistoryInfo(muon,doPartonHistory);
    }
    void initTruthMuonHistoryInfo(const xAOD::TruthParticle* truthmu, bool doPartonHistory)
    {
      truthmu->auxdecor<bool>("hasTruthMuonHistoryInfo")=false;
      if(doPartonHistory) truthmu->auxdecor<bool>("hasTruthMuonPartonHistoryInfo")=false;
      initCommonMuonHistoryInfo(truthmu,doPartonHistory);
      
    }
    
    void copyRecoMuonHistoryInfo(const xAOD::Muon* m_origin, const xAOD::Muon* m_target)
    {
      copyCommonMuonHistoryInfo(m_origin,m_target);
      m_target->auxdecor<bool>("hasRecoMuonHistoryInfo") = m_origin->auxdecor<bool>("hasRecoMuonHistoryInfo");
      m_target->auxdecor<const xAOD::TruthParticle*>("truthMuonLink") = m_origin->auxdecor<const xAOD::TruthParticle*>("truthMuonLink");
    }
    
    void copyTruthMuonHistoryInfo(const xAOD::TruthParticle* tm_origin, const xAOD::TruthParticle* tm_target)
    {
      copyCommonMuonHistoryInfo(tm_origin,tm_target);
      tm_target->auxdecor<bool>("hasTruthMuonHistoryInfo")= tm_origin->auxdecor<bool>("hasTruthMuonHistoryInfo");
      tm_target->auxdecor<bool>("hasTruthMuonPartonHistoryInfo")= tm_origin->auxdecor<bool>("hasTruthMuonPartonHistoryInfo");
    }
    
    void copyCommonMuonHistoryInfo(const xAOD::IParticle*  m_origin, const xAOD::IParticle* m_target)
    {
      if(!m_origin->isAvailable<top::LepParticleOriginFlag>("LepParticleOriginFlag"))
      {
        ATH_MSG_ERROR(" top::truth::copyCommonMuonHistoryInfo called with an origin muon which has no truth info!");
        throw;
      }
      m_target->auxdecor<top::LepParticleOriginFlag>("LepParticleOriginFlag") = m_origin->auxdecor<top::LepParticleOriginFlag>("LepParticleOriginFlag");
      m_target->auxdecor<const xAOD::TruthParticle*>("truthMotherLink") = m_origin->auxdecor<const xAOD::TruthParticle*>("truthMotherLink");
      m_target->auxdecor<const xAOD::TruthParticle*>("truthFirstNonLeptonMotherLink") = m_origin->auxdecor<const xAOD::TruthParticle*>("truthFirstNonLeptonMotherLink");
      m_target->auxdecor<const xAOD::TruthParticle*>("truthBMotherLink") = m_origin->auxdecor<const xAOD::TruthParticle*>("truthBMotherLink");
      m_target->auxdecor<const xAOD::TruthParticle*>("truthCMotherLink") = m_origin->auxdecor<const xAOD::TruthParticle*>("truthCMotherLink");
      
      if(m_origin->isAvailable<top::LepPartonOriginFlag>("LepPartonOriginFlag"))
      {
        m_target->auxdecor<top::LepPartonOriginFlag>("LepPartonOriginFlag") = m_origin->auxdecor<top::LepPartonOriginFlag>("LepPartonOriginFlag");
        m_target->auxdecor<const xAOD::TruthParticle*>("truthPartonMotherLink") = m_origin->auxdecor<const xAOD::TruthParticle*>("truthPartonMotherLink");
        m_target->auxdecor<const xAOD::TruthParticle*>("truthTopMotherLink") = m_origin->auxdecor<const xAOD::TruthParticle*>("truthTopMotherLink");
        m_target->auxdecor<const xAOD::TruthParticle*>("truthWMotherLink") = m_origin->auxdecor<const xAOD::TruthParticle*>("truthWMotherLink");
        m_target->auxdecor<const xAOD::TruthParticle*>("truthZMotherLink") = m_origin->auxdecor<const xAOD::TruthParticle*>("truthZMotherLink");
        m_target->auxdecor<const xAOD::TruthParticle*>("truthPhotonMotherLink") = m_origin->auxdecor<const xAOD::TruthParticle*>("truthPhotonMotherLink");
        m_target->auxdecor<const xAOD::TruthParticle*>("truthHiggsMotherLink") = m_origin->auxdecor<const xAOD::TruthParticle*>("truthHiggsMotherLink");
        m_target->auxdecor<const xAOD::TruthParticle*>("truthBSMMotherLink") = m_origin->auxdecor<const xAOD::TruthParticle*>("truthBSMMotherLink");
      }
    }
    
    const xAOD::TruthParticle* getTruthMuonAssociatedToRecoMuon(const xAOD::Muon* muon)
    {
      if(!muon) return 0;
      const xAOD::TrackParticle* track = muon->primaryTrackParticle();
      if(!track) return 0;
      
      const xAOD::TruthParticle* truthmu= xAOD::TruthHelpers::getTruthParticle(*track);      
      if(!truthmu) return 0;
      if(!truthmu->isMuon()) return 0; //note that the truth particle associated with a muon can be e.g. a pion/kaon in some cases (since pion/kaon decays are not done at generator level, but at simulation level), we ignore these cases
      
      return truthmu;
    }
    
    void getRecoMuonHistory(const xAOD::Muon* muon, bool doPartonHistory, SampleXsection::showering shAlgo, bool verbose)
    {

      if(verbose) ATH_MSG_INFO("getRecoMuonHistory:: ---------------entering in function-----------------");
      if(!muon) return;
      
      if(verbose)
      {
        //we retrieve the origin from the MCClassifier, it's useful for crosschecks; see https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/MCTruthClassifier/MCTruthClassifier/MCTruthClassifierDefs.h
        int origin=0;
        const static SG::AuxElement::Accessor<int> origmu("truthOrigin");
        if (origmu.isAvailable(*muon)) origin = origmu(*muon);
        
        ATH_MSG_INFO("getRecoMuonHistory::called on muon with pt="<<muon->pt()<<" origin="<<origin);
      }
      
      if(doPartonHistory && shAlgo!=SampleXsection::pythia8 && shAlgo!=SampleXsection::herwigpp)
      {
        static int nWarnings=0;
        if(nWarnings<10 || verbose)
        {
          ATH_MSG_WARNING("top::truth::getRecoMuonHistory : partonHistory can be checked only for pythia8 and herwigpp showering algorithms, SoftMuonAdditionalTruthInfoCheckPartonOrigin will be ignored for this sample");
          nWarnings++;
        }
        doPartonHistory=false;
      }
      
      if(!muon->isAvailable<bool>("hasRecoMuonHistoryInfo")) //add default values
      {
        initRecoMuonHistoryInfo(muon,doPartonHistory);
      }
      else if(muon->auxdecor<bool>("hasRecoMuonHistoryInfo"))
      {
         if(verbose) ATH_MSG_INFO("getRecoMuonHistory::we already ran on this muon, exiting");
         return; //this means we already ran on this muon in some way
      }
      muon->auxdecor<bool>("hasRecoMuonHistoryInfo")=true;
      
      //first we retrieve the truth muon associated with the reco one
      const xAOD::TruthParticle* truthmu = muon->auxdecor<const xAOD::TruthParticle*>("truthMuonLink"); //pointer to the truth muon associated with the reco muon
      
      if(!truthmu) truthmu=top::truth::getTruthMuonAssociatedToRecoMuon(muon);
      
      if(verbose) ATH_MSG_INFO("getRecoMuonHistory:: retrieved truth muon pointer="<<truthmu);
      if(!truthmu) return;
      
      muon->auxdecor<top::LepParticleOriginFlag>("LepParticleOriginFlag") = top::LepParticleOriginFlag::Unknown;
      if(doPartonHistory) muon->auxdecor<top::LepPartonOriginFlag>("LepPartonOriginFlag") = top::LepPartonOriginFlag::Unknown;
      
      if(verbose)
      {
        ATH_MSG_INFO("getRecoMuonHistory::---truth decay chain---");
        printDecayChain(truthmu, msg(MSG::Level::INFO));
      }
      
      getTruthMuonHistory(truthmu, doPartonHistory, shAlgo, verbose);
      //we attach the truth muon to the reco muon
      copyCommonMuonHistoryInfo(truthmu,muon);
      
      muon->auxdecor<const xAOD::TruthParticle*>("truthMuonLink") = truthmu;

      if(verbose)
      {
        ATH_MSG_INFO("getRecoMuonHistory:: written LepParticleOriginFlag decoration = "<<static_cast<int>(muon->auxdecor<top::LepParticleOriginFlag>("LepParticleOriginFlag")));
        if(doPartonHistory) ATH_MSG_INFO("getRecoMuonHistory:: written LepPartonOriginFlag decoration = "<<static_cast<int>(muon->auxdecor<top::LepPartonOriginFlag>("LepPartonOriginFlag")));
        ATH_MSG_INFO("getRecoMuonHistory:: exiting function");
      }
      return;
      
    }
    
    ////getTruthMuonHistory/////
    void getTruthMuonHistory(const xAOD::TruthParticle* truthmu, bool doPartonHistory, SampleXsection::showering shAlgo, bool verbose)
    {
      if(verbose)  ATH_MSG_INFO("getTruthMuonHistory:: entering in function");
      
      if(!truthmu) 
      {
        if(verbose) ATH_MSG_INFO("getTruthMuonHistory:: -> called on empty truth muon");
        return;
      }
      if(!(truthmu->isMuon()))
      {
        if(verbose) ATH_MSG_INFO("getTruthMuonHistory:: -> called on non-muon particle");
        return;
      }
      
      if(verbose) ATH_MSG_INFO("getTruthMuonHistory:: -> called on truth particle with pdgId="<<truthmu->pdgId()<<" barcode="<<truthmu->barcode()<<" status="<<truthmu->status()<<" pt="<<truthmu->pt());
      
      if(!truthmu->isAvailable<bool>("hasTruthMuonHistoryInfo")) //add default values
      {
        initTruthMuonHistoryInfo(truthmu,doPartonHistory);
      }
      else if(truthmu->auxdecor<bool>("hasTruthMuonHistoryInfo")) 
      {
        if(verbose) ATH_MSG_INFO("getTruthMuonHistory:: -> we already ran on this truth muon, exiting");
        return;
      }
      truthmu->auxdecor<bool>("hasTruthMuonHistoryInfo")=true;
      
      top::LepParticleOriginFlag lepParticleOriginFlag=top::LepParticleOriginFlag::Unknown; //flag defined in TopEvent/EventTools.h
      const xAOD::TruthParticle* truthmu_mother=0; //direct mother of the muon
      const xAOD::TruthParticle* truthmu_firstNonLeptonMother=0; //in case of X->muon or X->tau->muon, this will always contain X
      const xAOD::TruthParticle* truthmu_Bmother=0; //first bottom hadron encountered when going through ancestors
      const xAOD::TruthParticle* truthmu_Cmother=0; //first charmed hadron encountered when going through ancestors
      
      const xAOD::TruthParticle* initial_mu = getInitialStateParticle(truthmu);
      if(initial_mu) truthmu_mother= initial_mu->parent(0); 
      
      if(truthmu_mother && abs(truthmu_mother->pdgId())!=13) //let's then look at the history of this muon
      {
        
        if(!truthmu_mother->isLepton()) truthmu_firstNonLeptonMother=truthmu_mother; //basically all cases apart from tau->muon
        
        if(verbose) ATH_MSG_INFO("getTruthMuonHistory::mother pdgId="<<truthmu_mother->pdgId()<<" isB="<<truthmu_mother->isBottomHadron()<<" isC="<<truthmu_mother->isCharmHadron()<<" isLight="<<truthmu_mother->isLightHadron());
        
        if(truthmu_mother->isLightHadron()) lepParticleOriginFlag=top::LepParticleOriginFlag::FromLightHadron; //in this case we set the muon as coming from a light hadron and we don't investigate further
        else if(truthmu_mother->isW()) //W->munu
        {
          lepParticleOriginFlag=top::LepParticleOriginFlag::FromLeptonicW;
        }
        else if(truthmu_mother->isZ()) //Z->mumu
        {
          lepParticleOriginFlag=top::LepParticleOriginFlag::FromLeptonicZ;
        }
        else if(truthmu_mother->isPhoton()) //gamma*->mumu decay
        {
          lepParticleOriginFlag=top::LepParticleOriginFlag::FromPhoton;
        }
        else if(truthmu_mother->isHiggs())
        {
          lepParticleOriginFlag=top::LepParticleOriginFlag::FromHiggs;
        }
        else if(truthmu_mother->isBSM())
        {
          lepParticleOriginFlag=top::LepParticleOriginFlag::FromBSM;
        }
        else if(truthmu_mother->isTau()) //muon from tau
        {
          lepParticleOriginFlag=top::truth::getTruthMuonFromTauHistory(truthmu_mother,truthmu_Bmother,truthmu_Cmother,truthmu_firstNonLeptonMother,verbose);
        }
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
        ATH_MSG_INFO("getTruthMuonHistory::LepParticleOriginFlag="<<static_cast<int>(lepParticleOriginFlag));
        if(truthmu) ATH_MSG_INFO("getTruthMuonHistory::-->truth muon pdgId="<<truthmu->pdgId()<<" barcode="<<truthmu->barcode()<<" pt="<<truthmu->pt());
        else ATH_MSG_INFO("getTruthMuonHistory::-->truth muon not found");
        if(truthmu_mother) ATH_MSG_INFO("getTruthMuonHistory::-->truth muon mother pdgId="<<truthmu_mother->pdgId()<<" barcode="<<truthmu_mother->barcode()<<" pt="<<truthmu_mother->pt());
        else ATH_MSG_INFO("getTruthMuonHistory::-->truth muon mother not found");
        if(truthmu_firstNonLeptonMother) ATH_MSG_INFO("getTruthMuonHistory::-->truthmu_firstNonLeptonMother pdgId="<<truthmu_firstNonLeptonMother->pdgId()<<" barcode="<<truthmu_firstNonLeptonMother->barcode()<<" pt="<<truthmu_firstNonLeptonMother->pt());
        else ATH_MSG_INFO("getTruthMuonHistory::-->truthmu_firstNonLeptonMother not found");
        if(truthmu_Bmother) ATH_MSG_INFO("getTruthMuonHistory::-->truth muon Bmother pdgId="<<truthmu_Bmother->pdgId()<<" barcode="<<truthmu_Bmother->barcode()<<" pt="<<truthmu_Bmother->pt());
        else ATH_MSG_INFO("getTruthMuonHistory::-->truth muon Bmother not found");
        if(truthmu_Cmother) ATH_MSG_INFO("getTruthMuonHistory::-->truth muon Cmother pdgId="<<truthmu_Cmother->pdgId()<<" barcode="<<truthmu_Cmother->barcode()<<" pt="<<truthmu_Cmother->pt());
        else ATH_MSG_INFO("getTruthMuonHistory::-->truth muon Cmother not found");
      }
      
      //we decorate also on the truth muon associated with the reco one, because in this way if the truth muon was already decorated in another copy of the muon (for systematics) we avoid losing time.
      truthmu->auxdecor<top::LepParticleOriginFlag>("LepParticleOriginFlag") = lepParticleOriginFlag;
      truthmu->auxdecor<const xAOD::TruthParticle*>("truthFirstNonLeptonMotherLink") = truthmu_firstNonLeptonMother;
      truthmu->auxdecor<const xAOD::TruthParticle*>("truthMotherLink") = truthmu_mother; 
      truthmu->auxdecor<const xAOD::TruthParticle*>("truthBMotherLink") = truthmu_Bmother;
      truthmu->auxdecor<const xAOD::TruthParticle*>("truthCMotherLink") = truthmu_Cmother;
      
      if(doPartonHistory) top::truth::getTruthMuonPartonHistory(truthmu, lepParticleOriginFlag, truthmu_Bmother, truthmu_Cmother, truthmu_firstNonLeptonMother, shAlgo, verbose);
      
      if(verbose) ATH_MSG_INFO("getTruthMuonHistory:: exiting from function");
      
      return;
    }
    
    /////getTruthMuonPartonHistory/////
    void getTruthMuonPartonHistory(const xAOD::TruthParticle* truthmu, top::LepParticleOriginFlag lepParticleOriginFlag, const xAOD::TruthParticle* truthmu_Bmother, const xAOD::TruthParticle* truthmu_Cmother,  const xAOD::TruthParticle* truthmu_firstNonLeptonMother, SampleXsection::showering shAlgo, bool verbose)
    {
      if(verbose)  ATH_MSG_INFO("getTruthMuonPartonHistory:: entering in function");
      
      if(!truthmu && verbose) 
      {
        ATH_MSG_INFO("getTruthMuonPartonHistory:: -> called on empty truth muon");
        return;
      }
      
      if(!truthmu->isAvailable<bool>("hasTruthMuonPartonHistoryInfo"))//default values
      {
        initTruthMuonHistoryInfo(truthmu,true);
      }
      else if(truthmu->auxdecor<bool>("hasTruthMuonPartonHistoryInfo")) 
      {
        if(verbose) ATH_MSG_INFO("getTruthMuonHistory:: -> we already ran on this truth muon, exiting");
        return;
      }
      truthmu->auxdecor<bool>("hasTruthMuonPartonHistoryInfo")=true;
      
      const xAOD::TruthParticle* truthmu_mother=truthmu_firstNonLeptonMother;
      
      if(!truthmu_mother)
      {
        if(verbose) ATH_MSG_INFO("getTruthMuonPartonHistory:: -> no muon mother available, returning");
        return;
      }
      if(lepParticleOriginFlag==top::LepParticleOriginFlag::MissingTruthInfo || lepParticleOriginFlag==top::LepParticleOriginFlag::FromLightHadron)
      {
        
        if(verbose) ATH_MSG_INFO("getTruthMuonPartonHistory:: -> muon from light hadron, will not fill info");
        return;
      }
      
      top::LepPartonOriginFlag lepPartonOriginFlag=top::LepPartonOriginFlag::Unknown;
      
      //this is the priority we use to look for parents in order: top, higgs, BSM, W/Z, gamma*
      auto get_priority = [](const xAOD::TruthParticle* p) -> int {
              if(!p) return 999;
              if(p->isTop()) return 0;
              if(p->isHiggs()) return 10;
              if(p->isBSM()) return 15;
              if(p->isW() || p->isZ()) return 20;
              if(p->isPhoton()) return 25;
              return 100;
      };
      
      if(truthmu_mother->isTop()) // shouldn't happen, but maybe some generators don't store intermediate bosons
      {
        truthmu->auxdecor<const xAOD::TruthParticle*>("truthTopMotherLink") = truthmu_mother;
        truthmu->auxdecor<const xAOD::TruthParticle*>("truthPartonMotherLink") = truthmu_mother;
        if(truthmu_mother->pdgId()>0) lepPartonOriginFlag=top::LepPartonOriginFlag::FromTopViaLeptonicBoson;
        else lepPartonOriginFlag=top::LepPartonOriginFlag::FromAntiTopViaLeptonicBoson;
      }
      else if(truthmu_mother->isHiggs())
      {
        truthmu->auxdecor<const xAOD::TruthParticle*>("truthHiggsMotherLink") = truthmu_mother;
        lepPartonOriginFlag=top::LepPartonOriginFlag::FromHiggs;
      }
      else if(truthmu_mother->isBSM())
      {
        truthmu->auxdecor<const xAOD::TruthParticle*>("truthBSMMotherLink") = truthmu_mother;
        lepPartonOriginFlag=top::LepPartonOriginFlag::FromBSM;
      }
      else if(truthmu_mother->isW() || truthmu_mother->isZ() || truthmu_mother->isPhoton() ) //the muon is coming from a leptonically decaying boson (eventually via a boson->tau->mu)
      {
        lepPartonOriginFlag=top::LepPartonOriginFlag::FromLeptonicBoson; //this is the basic info if we don't find further infromation
        
        if(truthmu_mother->isW()) truthmu->auxdecor<const xAOD::TruthParticle*>("truthWMotherLink") = truthmu_mother;
        else if(truthmu_mother->isZ()) truthmu->auxdecor<const xAOD::TruthParticle*>("truthZMotherLink") = truthmu_mother;
        else if(truthmu_mother->isPhoton()) truthmu->auxdecor<const xAOD::TruthParticle*>("truthPhotonMotherLink") = truthmu_mother;
        
        //then we check in case we can find the mother of the boson
        const xAOD::TruthParticle* initialStateBoson=top::truth::getInitialStateParticle(truthmu_mother,verbose);
        const xAOD::TruthParticle* bosonOrigin=0;
        unsigned int bosonOrigin_priority=999;
        
        if(initialStateBoson) 
        {
          for(unsigned int ip=0; ip<initialStateBoson->nParents(); ip++)
          {
            const xAOD::TruthParticle* parent=initialStateBoson->parent(ip);
            if(!parent) continue;
            
            unsigned int parent_priority=get_priority(parent);
            if(parent_priority<bosonOrigin_priority)
            {
              bosonOrigin_priority=parent_priority;
              bosonOrigin=parent;
            }
          }
        }
        if(bosonOrigin && bosonOrigin->isTop())
        {
          if(verbose) ATH_MSG_INFO("getTruthMuonPartonHistory:: -> muon from boson from top");
          
          truthmu->auxdecor<const xAOD::TruthParticle*>("truthTopMotherLink") = bosonOrigin;
          if(bosonOrigin->pdgId()>0) lepPartonOriginFlag=top::LepPartonOriginFlag::FromTopViaLeptonicBoson;
          else lepPartonOriginFlag=top::LepPartonOriginFlag::FromAntiTopViaLeptonicBoson;
        }
        else if(bosonOrigin && bosonOrigin->isHiggs())
        {
          if(verbose) ATH_MSG_INFO("getTruthMuonPartonHistory:: -> muon from boson from higgs");
          truthmu->auxdecor<const xAOD::TruthParticle*>("truthHiggsMotherLink") = bosonOrigin;
          lepPartonOriginFlag=top::LepPartonOriginFlag::FromHiggsViaLeptonicBosonToHF;
        }
        else if(bosonOrigin && bosonOrigin->isBSM())
        {
          if(verbose) ATH_MSG_INFO("getTruthMuonPartonHistory:: -> muon from boson from BSM");
          truthmu->auxdecor<const xAOD::TruthParticle*>("truthBSMMotherLink") = bosonOrigin;
          lepPartonOriginFlag=top::LepPartonOriginFlag::FromBSMViaLeptonicBosonToHF;
        }
        else
        {
          if(verbose) ATH_MSG_INFO("getTruthMuonPartonHistory:: -> muon from boson not from top or higgs");
          lepPartonOriginFlag=top::LepPartonOriginFlag::FromLeptonicBoson; 
        }
      }
      else if(truthmu_Bmother || truthmu_Cmother) //in this case the muon is not coming from a boson direct decay, we treat only cases from HF hadrons
      {
        lepPartonOriginFlag=top::LepPartonOriginFlag::FromHFHadronOfUnkownOrigin; //default case
        
        bool isBHadronMother=truthmu_Bmother;
        const xAOD::TruthParticle* hadronMother= isBHadronMother ? truthmu_Bmother : truthmu_Cmother;
        const xAOD::TruthParticle* firstHadronMother=getFirstHFHadronOfSameFlavour(hadronMother,verbose);
        
        if(verbose)
        {
          ATH_MSG_INFO("getTruthMuonPartonHistory:: isBHadronmother="<<isBHadronMother<<" Bmom="<<truthmu_Bmother<<" Cmom="<<truthmu_Cmother<<" hadMom="<<hadronMother<<" firstHadMom="<<firstHadronMother);
          ATH_MSG_INFO("getTruthMuonPartonHistory:: -> muon from hadron pdgId="<<(hadronMother ? hadronMother->pdgId() : 0)<<", first hadron in chain pdgId="<<(firstHadronMother ? firstHadronMother->pdgId() : 0));
        }
        if(firstHadronMother)
        {
          
          const xAOD::TruthParticle* hf_parton_parent=0;
          
          if(shAlgo == SampleXsection::herwigpp)  //in herwig we have pdgId==81 for clusters as hadron parents
          {
            if(firstHadronMother->parent(0) && firstHadronMother->parent(0)->pdgId()==81)
            {
              firstHadronMother=getInitialStateParticle(firstHadronMother->parent(0));
            }
          }
          
          //first we find the HF parton that originated the HF hadron
          for(unsigned int ip=0; ip<firstHadronMother->nParents(); ip++)
          {
            const xAOD::TruthParticle* parent=firstHadronMother->parent(ip);
            if(!parent) continue;
            if(isBHadronMother && abs(parent->pdgId())==5)
            {
              hf_parton_parent=parent;
              break;
            }
            else if(!isBHadronMother && abs(parent->pdgId())==4)
            {
              hf_parton_parent=parent;
              break;
            }
            
          }
          
          truthmu->auxdecor<const xAOD::TruthParticle*>("truthPartonMotherLink") =hf_parton_parent;
          
          //the we get the first HF parton of the evolution chain
          const xAOD::TruthParticle* initial_hf_parton_parent=0;
          if(hf_parton_parent) initial_hf_parton_parent = top::truth::getInitialStateParticle(hf_parton_parent,verbose);
          
          //parton ancestor will be the particle/resonance originating the first HF quark
          const xAOD::TruthParticle* parton_ancestor=0;
          unsigned int parton_ancestor_priority=999;
          
          if(initial_hf_parton_parent)
          {
            for(unsigned int ip=0; ip<initial_hf_parton_parent->nParents(); ip++)
            {
              const xAOD::TruthParticle* parent=initial_hf_parton_parent->parent(ip);
              if(!parent) continue;
              if(!parton_ancestor) parton_ancestor=parent;
              else
              { 
                unsigned int priority =get_priority(parent);
                if(priority < parton_ancestor_priority)
                {
                  parton_ancestor_priority=priority;
                  parton_ancestor=parent;
                }
              }
            }
          }
          
          //finally we (hopefully) know where the HF hadron is originally coming from
          if(parton_ancestor && parton_ancestor->isTop())
          {
            truthmu->auxdecor<const xAOD::TruthParticle*>("truthTopMotherLink") = parton_ancestor;
            if(verbose) ATH_MSG_INFO("getTruthMuonPartonHistory:: -> muon from HF from top");
            
            if(parton_ancestor->pdgId()>0) lepPartonOriginFlag=top::LepPartonOriginFlag::FromTopViaQuarkToHF;
            else lepPartonOriginFlag=top::LepPartonOriginFlag::FromAntiTopViaQuarkToHF;
          }
          else if(parton_ancestor && parton_ancestor->isHiggs())
          {
            truthmu->auxdecor<const xAOD::TruthParticle*>("truthHiggsMotherLink") = parton_ancestor;
            lepPartonOriginFlag=top::LepPartonOriginFlag::FromHiggsToHF;
          }
          else if(parton_ancestor && parton_ancestor->isBSM())
          {
            truthmu->auxdecor<const xAOD::TruthParticle*>("truthBSMMotherLink") = parton_ancestor;
            lepPartonOriginFlag=top::LepPartonOriginFlag::FromBSMToHF;
          }
          else if(parton_ancestor && (parton_ancestor->isW() || parton_ancestor->isZ() || parton_ancestor->isPhoton()))
          {
            const xAOD::TruthParticle* first_bosonMotherOfHfQuark=top::truth::getInitialStateParticle(parton_ancestor);
            
            if(parton_ancestor->isW()) truthmu->auxdecor<const xAOD::TruthParticle*>("truthWMotherLink") = parton_ancestor;
            else if(parton_ancestor->isZ()) truthmu->auxdecor<const xAOD::TruthParticle*>("truthZMotherLink") = parton_ancestor;
            else if(parton_ancestor->isPhoton()) truthmu->auxdecor<const xAOD::TruthParticle*>("truthPhotonMotherLink") = parton_ancestor;
            
            const xAOD::TruthParticle* bosonOrigin=0;
            unsigned int bosonOrigin_priority=999;
            
            if(first_bosonMotherOfHfQuark)
            {
              for(unsigned int ip=0; ip<first_bosonMotherOfHfQuark->nParents(); ip++)
              {
                const xAOD::TruthParticle* parent=first_bosonMotherOfHfQuark->parent(ip);
                if(!parent) continue;
                
                unsigned int priority =get_priority(parent);
                if(priority<bosonOrigin_priority)
                {
                  bosonOrigin=parent;
                  bosonOrigin_priority=priority;
                }
              }
              if(bosonOrigin && bosonOrigin->isTop())
              {
                if(verbose) ATH_MSG_INFO("getTruthMuonPartonHistory:: -> muon from HF from boson from top");
                truthmu->auxdecor<const xAOD::TruthParticle*>("truthTopMotherLink") = bosonOrigin;
                
                if(bosonOrigin->pdgId()>0) lepPartonOriginFlag=top::LepPartonOriginFlag::FromTopViaHadronicBosonToHF;
                else lepPartonOriginFlag=top::LepPartonOriginFlag::FromAntiTopViaHadronicBosonToHF;
              }
              else if(bosonOrigin && bosonOrigin->isHiggs())
              {
                if(verbose) ATH_MSG_INFO("getTruthMuonPartonHistory:: -> muon from HF from boson from higgs");
                truthmu->auxdecor<const xAOD::TruthParticle*>("truthHiggsMotherLink") = bosonOrigin;
                lepPartonOriginFlag=top::LepPartonOriginFlag::FromHiggsViaHadronicBosonToHF;
              }
              else if(bosonOrigin && bosonOrigin->isBSM())
              {
                if(verbose) ATH_MSG_INFO("getTruthMuonPartonHistory:: -> muon from HF from boson from BSM");
                truthmu->auxdecor<const xAOD::TruthParticle*>("truthBSMMotherLink") = bosonOrigin;
                lepPartonOriginFlag=top::LepPartonOriginFlag::FromBSMViaHadronicBosonToHF;
              }
              else
              {
                lepPartonOriginFlag=top::LepPartonOriginFlag::FromHadronicBosonToHF; //not sure where this is coming from
              }
            }
          }//end of V->HF->muon case
          
        }//end of HF->muon case
        
      }//end of non-bosonic case
      
      truthmu->auxdecor<top::LepPartonOriginFlag>("LepPartonOriginFlag") = lepPartonOriginFlag;
      
      if(verbose) ATH_MSG_INFO("getTruthMuonHistory::------> LepPartonOriginFlag="<<static_cast<int>(lepPartonOriginFlag));
      
      return;
      
    }//end of getTruthMuonPartonHistory
    
    /////getTruthMuonFromCharmHistory/////
    top::LepParticleOriginFlag getTruthMuonFromCharmHistory(const xAOD::TruthParticle* &truthmu_Bmother, const xAOD::TruthParticle* truthmu_Cmother, bool verbose)
    {
      if(verbose) ATH_MSG_INFO("called getTruthMuonFromCharmHistory");
      const xAOD::TruthParticle* initial_C_hadron=getFirstHFHadronOfSameFlavour(truthmu_Cmother); //we find the first non-Chadron ancestor of the C-hadron
      
      if(initial_C_hadron)
      {
        bool hasBHadronParent=false;
        for(unsigned int ip=0; ip<initial_C_hadron->nParents(); ip++)
        {
          const xAOD::TruthParticle* parent=initial_C_hadron->parent(ip);
          if(!parent) continue;
          if(parent->isBottomHadron()) 
          {
            hasBHadronParent=true;
            truthmu_Bmother=parent;
            break;
          }
        }
        if(hasBHadronParent) return top::LepParticleOriginFlag::FromBtoC;
      }
      
      return top::LepParticleOriginFlag::FromC; //we don't know where the C-hadron is coming from
    } //end of getTruthMuonFromCharmHistory method
    
    /////getTruthMuonFromTauHistory/////
    top::LepParticleOriginFlag getTruthMuonFromTauHistory(const xAOD::TruthParticle* tau, const xAOD::TruthParticle* &truthmu_Bmother, const xAOD::TruthParticle* &truthmu_Cmother, const xAOD::TruthParticle* &truthmu_firstNonLeptonMother, bool verbose)
    {
      if(!tau && verbose) 
      {
        ATH_MSG_WARNING("getTruthMuonFromTauHistory called on empty tau");
        return top::LepParticleOriginFlag::FromTau; 
      }
      if(!tau->isTau())
      {
        ATH_MSG_ERROR("getTruthMuonFromTauHistory called on non-tau, pdgId"<<(tau->pdgId()));
        throw;
      }
      const xAOD::TruthParticle* initialTau = top::truth::getInitialStateParticle(tau);
      if(!initialTau)
      {
        if(verbose)ATH_MSG_INFO("getTruthMuonFromTauHistory:: could not find initial state tau");
        return top::LepParticleOriginFlag::FromTau; 
      }
      
      const xAOD::TruthParticle* tauMother=initialTau->parent(0);
      
      if(!tauMother) return top::LepParticleOriginFlag::FromTau; //muon from tau, we don't know where the tau is coming from
      
      if(!tauMother->isLepton()) truthmu_firstNonLeptonMother=tauMother;
      
      if(tauMother->isW()) //W->taunu with tau->mununu decay
      {
        return top::LepParticleOriginFlag::FromLeptonicWToTau;
      }
      if(tauMother->isZ()) //Z->tautau with tau->mununu
      {
        return top::LepParticleOriginFlag::FromLeptonicZToTau;
      }
      if(tauMother->isPhoton())
      {
        return top::LepParticleOriginFlag::FromPhotonToTau;
      }
      if(tauMother->isHiggs())
      {
        return top::LepParticleOriginFlag::FromHiggsToTau;
      }
      if(tauMother->isBSM())
      {
        return top::LepParticleOriginFlag::FromBSMToTau;
      }
      if(tauMother->isBottomHadron()) //the muon is from a B->tau+X with tau->mununu decay
      {
        truthmu_Bmother=tauMother;
        return top::LepParticleOriginFlag::FromBtoTau;
      }
      if(tauMother->isCharmHadron()) //now this is getting complicated, the muon is from e.g. D->tau+X with tau->mununu decay, now we have to understand where the D is coming from...
      {
        truthmu_Cmother=tauMother;
        const xAOD::TruthParticle* initial_C_hadron=getFirstHFHadronOfSameFlavour(truthmu_Cmother); //we find the first non-Chadron ancestor of the C-hadron
        
        bool hasBHadronParent=false;
        
        for(unsigned int ip=0; ip<initial_C_hadron->nParents(); ip++)
        {
          const xAOD::TruthParticle* parent=initial_C_hadron->parent(ip);
          if(!parent) continue;
          if(parent->isBottomHadron())
          {
            hasBHadronParent=true;
            truthmu_Bmother=parent;
            break;
          }
        }
        if(hasBHadronParent) return top::LepParticleOriginFlag::FromBtoCtoTau;
        else  //in this case this is coming from D->tau->mu+X or something with no B-hadron ancestor
        {
          return top::LepParticleOriginFlag::FromCtoTau;
        }
      }//end of if(tauMother->isCharmHadron())
      
      return top::LepParticleOriginFlag::FromTau; //in all other cases we don't know anything apart from the fact that the muon is coming from a tau
    }//end of getTruthMuonFromTauHistory method
    
    //getFirstHadronOfSameFlavour//
    const xAOD::TruthParticle* getFirstHFHadronOfSameFlavour(const xAOD::TruthParticle* truthParticle, bool verbose)
    {
      if(!truthParticle) return 0;
      bool isBottomHadron=truthParticle->isBottomHadron();
      bool isCharmHadron=truthParticle->isCharmHadron();
      
      if(!(isBottomHadron) && !(isCharmHadron))
      {
        if(verbose) ATH_MSG_INFO("getFirstHFHadronOfSameFlavour:: this can be called only on HF hadrons...");
        return 0;
      }
      const xAOD::TruthParticle* parent=truthParticle;
      int niterations=0;
      while(parent)
      {
        if((niterations++)>100)
        {
          if(verbose) ATH_MSG_INFO("getFirstHFHadronOfSameFlavour:: too many iterations on particle "<<(truthParticle->pdgId()));
          return 0;
        }
        
        parent=getInitialStateParticle(parent,verbose);
        if(!parent)
        {
          if(verbose) ATH_MSG_INFO("getFirstHFHadronOfSameFlavour:: initial state particle not found");
          break;
        }
        
        if(verbose) ATH_MSG_INFO("getFirstHFHadronOfSameFlavour:: initial state particle found="<<(parent->pdgId()));
        
        bool hasSameFlavourParent=false;
        for(unsigned int ip=0; ip<parent->nParents(); ip++)
        {
          if(parent->parent(ip))
          {
            if((isBottomHadron && parent->parent(ip)->isBottomHadron())||(isCharmHadron && parent->parent(ip)->isCharmHadron()))
            {
              parent=parent->parent(ip);
              hasSameFlavourParent=true;
              break;
            }
          }
        }
        
        if(!hasSameFlavourParent)
        {
          if(verbose) ATH_MSG_INFO("getFirstHFHadronOfSameFlavour:: parent="<<(parent->pdgId())<<" has no same flavour ancestor");
           break;
        }
        else
        {
          if(verbose) ATH_MSG_INFO("getFirstHFHadronOfSameFlavour:: found same flavour parent="<<(parent->pdgId()));
        }
        
      }
      return parent;
    }
    //end of getFirstHadronOfSameFlavour method//
    
    //getInitialStateParticle//
    const xAOD::TruthParticle* getInitialStateParticle(const xAOD::TruthParticle* truthParticle, bool verbose)
    {
      if(!truthParticle) return 0;
      int pdgId=truthParticle->pdgId();
      int niterations=0;
      const xAOD::TruthParticle* parent=truthParticle;
      
      while(parent)
      {
        if((niterations++)>100)
        {
          if(verbose) ATH_MSG_INFO("getInitialStateParticle:: too many iterations for particle "<<(truthParticle->pdgId()));
          return 0;
        }
        
        bool hasParentWithSamePdgId=false;
        for(unsigned int ip=0; ip<parent->nParents(); ip++)
        {
          if(parent->parent(ip) && (parent->parent(ip)->pdgId()==pdgId))
          {
            parent=parent->parent(ip);
            hasParentWithSamePdgId=true;
            break;
          }
        }
        if(!hasParentWithSamePdgId) break;
      }
      return parent;
    }
    //end of getInitialStateParticle method//
    
    void printDecayChain(const xAOD::TruthParticle* truthPart,
                         std::ostream& os /* = std::cout */,
                         const std::string& prefix /* = "" */) {
      // Protect against a faulty production vertex reference.
      if (!truthPart || !truthPart->prodVtx()) {
        os << prefix << "[" << (truthPart ? truthPart->pdgId() :0x0) << " <- NULL]" << '\n';
        return;
      }

      os << prefix << "[" << truthPart->pdgId();
      int niterations=0;
      
      while (truthPart) {
        if((niterations++)>30)
        {
          os<<" STOP! too many iterations ";
          break;
        }
        if(truthPart->nParents()<1) break;
        
        os << " <- ";
        const xAOD::TruthParticle* firstWithSamePdgId=0;
        const xAOD::TruthParticle* firstNonParton=0;
        const xAOD::TruthParticle* firstCorrectFlavorQuark=0;
        
        for(unsigned int ip=0; ip<truthPart->nParents(); ip++)
        {
          if(ip>0) os<<", ";
          os<<" "<<(truthPart->parent(ip) ? truthPart->parent(ip)->pdgId() : 0);
          if(!truthPart->parent(ip)) continue;
          
          if(!firstWithSamePdgId && truthPart->parent(ip)->pdgId()==truthPart->pdgId())
          {
            firstWithSamePdgId = truthPart->parent(ip);
          }
          if(!firstNonParton && !truthPart->parent(ip)->isParton())
          {
            firstNonParton = truthPart->parent(ip);
          }
          if(truthPart->isBottomHadron() && abs(truthPart->parent(ip)->pdgId())==5)
          {
            firstCorrectFlavorQuark = truthPart->parent(ip);
          }
          if(truthPart->isCharmHadron() && abs(truthPart->parent(ip)->pdgId())==4)
          {
            firstCorrectFlavorQuark = truthPart->parent(ip);
          }
          
        }
        if(firstWithSamePdgId) truthPart=firstWithSamePdgId;
        else if(firstNonParton) truthPart=firstNonParton;
        else if(firstCorrectFlavorQuark) truthPart=firstCorrectFlavorQuark;
        else truthPart=truthPart->parent(0);
        
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
