/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

// $Id: CalcTopPartonHistory.cxx 800464 2017-03-13 18:06:24Z tpelzer $
#include "TopPartons/CalcTopPartonHistory.h"
#include "TopPartons/PartonHistory.h"
#include "TopConfiguration/TopConfig.h"
#include "TopPartons/CalcTtbarPartonHistory.h"
#include "xAODTruth/TruthVertex.h"

namespace top {
  CalcTopPartonHistory::CalcTopPartonHistory(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr) {
    declareProperty("config", m_config);
  }
  
  StatusCode CalcTopPartonHistory::buildContainerFromMultipleCollections(const std::vector<std::string> &collections, const std::string& out_contName)
  {
    ConstDataVector<DataVector<xAOD::TruthParticle_v1> > *out_cont = new ConstDataVector<DataVector<xAOD::TruthParticle_v1> > (SG::VIEW_ELEMENTS);
    
    for(const std::string& collection : collections)
    {
      const xAOD::TruthParticleContainer* cont=nullptr;
      ATH_CHECK(evtStore()->retrieve(cont,collection));
      for(const xAOD::TruthParticle* p : *cont) out_cont->push_back(p);
    }
    
    //we give control of the container to the store, because in this way we are able to retrieve it as a const data vector, see https://twiki.cern.ch/twiki/bin/view/AtlasComputing/DataVector#ConstDataVector
    xAOD::TReturnCode save = evtStore()->tds()->record(out_cont,out_contName);
    if (!save) return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
  }
  
  StatusCode CalcTopPartonHistory::linkBosonCollections() 
  {
    return decorateCollectionWithLinksToAnotherCollection("TruthBoson","TruthBosonsWithDecayParticles","AT_linkToTruthBosonsWithDecayParticles");
  }
  StatusCode CalcTopPartonHistory::decorateCollectionWithLinksToAnotherCollection(const std::string &collectionToDecorate, const std::string &collectionToLink, const std::string &nameOfDecoration)
  {
    const xAOD::TruthParticleContainer* cont1(nullptr);
    const xAOD::TruthParticleContainer* cont2(nullptr);
    ATH_CHECK(evtStore()->retrieve(cont1,collectionToDecorate));
    ATH_CHECK(evtStore()->retrieve(cont2,collectionToLink));

    for(const xAOD::TruthParticle *p : *cont1)
    {
      
      const xAOD::TruthParticle* link =0;
      for(const xAOD::TruthParticle *p2 : *cont2)
      {
        if(p->pdgId()==p2->pdgId() && p->barcode()==p2->barcode())
        {
          link=p2;
          break;
        }
      } 
      p->auxdecor<const xAOD::TruthParticle*>(nameOfDecoration)=link;
      
    }
    return StatusCode::SUCCESS;
  }
  
  const xAOD::TruthParticle* CalcTopPartonHistory::getTruthParticleLinkedFromDecoration(const xAOD::TruthParticle* part, const std::string &decorationName)
  {
    if(!part->isAvailable<const xAOD::TruthParticle*>(decorationName)) return part;
  
    const xAOD::TruthParticle* link=part->auxdecor<const xAOD::TruthParticle*>(decorationName);
    if(link) return link;
    
    return part;
  }

  ///Store the four-momentum of the post-FSR top or anti-top found using statusCodes
  ///This would only work if there is at most one "true" top of each charge (i.e. won't work for SS tops or 4 tops)
  ///This code was adapted from the 7TeV parton-level differential ttbar routine:
  // https://svnweb.cern.ch/trac/atlasphys-top/browser/Physics/Top/Software/MCvalidation/Rivet/Rivet2.X/trunk/routines/ATLAS_2014_I1304289/ATLAS_2014_I1304289.cc
  bool CalcTopPartonHistory::topAfterFSR_SC(const xAOD::TruthParticleContainer* truthParticles, int start,
                                            TLorentzVector& top_afterFSR_SC_p4) {
    /// Step1: create vectors of particles of each status codes
    // Vectors to hold any status=3 (anti-)top quarks (Pythia 6)
    std::vector<const xAOD::TruthParticle*> v_status3_top;
    // Vectors to hold any status=155 (anti-)top quarks (Herwig 6)
    std::vector<const xAOD::TruthParticle*> v_status155_top;
    // Vectors to hold any status=11 (anti-)top quarks for Herwig++
    std::vector<const xAOD::TruthParticle*> v_status11_top;
    // Vectors to hold any status=22 (anti-)top quarks
    std::vector<const xAOD::TruthParticle*> v_statusOther_top;

    /// Step2: loop on the container of particles and fill the above created vectors
    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle->pdgId() != start) continue; // only keep particles of a given pdgID (e.g. 6 or -6)

      if (particle->status() == 3) {
        v_status3_top.push_back(particle);
      } else if (particle->status() == 155) {
        v_status155_top.push_back(particle);
      } else if (particle->status() == 11) {// for Herwig++: take only the tops that decay into Wb!!!
        if (!particle->hasDecayVtx()) continue;
        const xAOD::TruthVertex* vertex = particle->decayVtx();
        if (vertex == nullptr) continue;
        if (vertex->nOutgoingParticles() == 2) v_status11_top.push_back(particle);
      } else {
        v_statusOther_top.push_back(particle);
      }
    }

    /// Step3: for some of the statuscodes, keep only the last of the vector
    // If there are more than 1 status 3 tops or anti-tops, only keep the last one put into the vector
    if (v_status3_top.size() > 1) {
      v_status3_top = std::vector<const xAOD::TruthParticle*>(v_status3_top.end() - 1, v_status3_top.end());
    }
    // If there are more than 1 status 11 tops or anti-tops, only keep the last one put into the vector
    if (v_status11_top.size() > 1) {
      v_status11_top = std::vector<const xAOD::TruthParticle*>(v_status11_top.end() - 1, v_status11_top.end());
    }
    // Rach: check for Pythia 8 as well
    // If there are more than 1 status 3 tops or anti-tops, only keep the last one put into the vector
    if (v_statusOther_top.size() > 1) {
      v_statusOther_top = std::vector<const xAOD::TruthParticle*>(v_statusOther_top.end() - 1, v_statusOther_top.end());
    }

    /// Step4: chose which statuscode to take according to what is found in the event
    const xAOD::TruthParticle* top = nullptr;
    // If there are status 3 tops and no status 155 tops this is probably a Pythia event, so used the status 3s.
    if (v_status3_top.size() == 1 && v_status155_top.size() == 0) {
      top = v_status3_top[0];
    }
    // If there are status 155 tops this must be a Herwig event, so use the status 155s.
    if (v_status155_top.size() == 1 && v_status3_top.size() == 0) {
      top = v_status155_top[0];
    }
    // If there are tops with other status this must be a Pythia8 event, so use them.
    if (v_statusOther_top.size() == 1 && v_status155_top.size() == 0 && v_status3_top.size() == 0) {
      top = v_statusOther_top[0];
    }
    // If there are status 155 tops this must be a Herwig event, so use the status 155s.
    if (v_status11_top.size() == 1 && v_status3_top.size() == 0) {
      top = v_status11_top[0];
    }

    /// Step5: if everything worked, set the 4-vector to its value and return true
    if (top != nullptr) {
      top_afterFSR_SC_p4 = top->p4();
      return true;
    }
    return false;
  }

  // for b coming from W'->tb
  bool CalcTopPartonHistory::b(const xAOD::TruthParticleContainer* truthParticles,
                               TLorentzVector& b_beforeFSR, TLorentzVector& b_afterFSR) {
    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (abs(particle->pdgId()) != 5) continue;

      bool skipit(false);
      for (size_t i = 0; i < particle->nParents(); i++) {
        const xAOD::TruthParticle* parent = particle->parent(i);
        if (parent && (parent->isTop() || abs(parent->pdgId()) == 5)) {
          skipit = true;
          break;
        }//if
      }//for

      if (skipit) continue;

      b_beforeFSR = particle->p4();
      b_afterFSR = findAfterFSR(particle)->p4();

      return true;
    }


    return false;
  }

  bool CalcTopPartonHistory::topWb(const xAOD::TruthParticleContainer* truthParticles,
                                   int start, TLorentzVector& t_beforeFSR_p4, TLorentzVector& t_afterFSR_p4,
                                   TLorentzVector& W_p4,
                                   TLorentzVector& b_p4, TLorentzVector& Wdecay1_p4,
                                   int& Wdecay1_pdgId, TLorentzVector& Wdecay2_p4, int& Wdecay2_pdgId) {
    bool hasT = false;
    bool hasW = false;
    bool hasB = false;
    bool hasWdecayProd1 = false;
    bool hasWdecayProd2 = false;

    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle->pdgId() != start) continue;

      if (hasParticleIdenticalParent(particle)) continue; // kepping only top before FSR

      t_beforeFSR_p4 = particle->p4(); // top before FSR
      hasT = true;

      // demanding the last tops after FSR
      particle = findAfterFSR(particle);
      t_afterFSR_p4 = particle->p4(); // top after FSR

      for (size_t k = 0; k < particle->nChildren(); k++) {
        const xAOD::TruthParticle* topChildren = particle->child(k);

        if (abs(topChildren->pdgId()) == 24) {
          W_p4 = topChildren->p4();  // W boson after FSR
          hasW = true;
          
          // demanding the last W after FSR
          topChildren = findAfterFSR(topChildren);
          
          //for DAOD_PHYS we have to use a special procedure to associate W bosons linked from the top to those in the TruthBosonsWithDecayParticles collection, which have the correct links for their decay products
          //this is better explained in the head; this will work only if the class calling this function has called linkBosonCollections() before
          if(m_config->getDerivationStream() == "PHYS") topChildren=getTruthParticleLinkedFromDecoration(topChildren,"AT_linkToTruthBosonsWithDecayParticles");
          
          for (size_t q = 0; q < topChildren->nChildren(); ++q) {
            const xAOD::TruthParticle* WChildren = topChildren->child(q);
            if (abs(WChildren->pdgId()) < 17) {
              if (WChildren->pdgId() > 0) {
                Wdecay1_p4 = WChildren->p4();
                Wdecay1_pdgId = WChildren->pdgId();
                hasWdecayProd1 = true;
              } else {
                Wdecay2_p4 = WChildren->p4();
                Wdecay2_pdgId = WChildren->pdgId();
                hasWdecayProd2 = true;
              }//else
            }//if
          }//for
        } else if (abs(topChildren->pdgId()) == 5) {
          b_p4 = topChildren->p4();
          hasB = true;
        } //else if
      } //for (size_t k=0; k < particle->nChildren(); k++)

      if (hasT && hasW && hasB && hasWdecayProd1 && hasWdecayProd2) return true;
      
    } //for (const xAOD::TruthParticle* particle : *truthParticles)
    
    return false;
  }

  bool CalcTopPartonHistory::topWq(const xAOD::TruthParticleContainer* truthParticles,
                                   int start, TLorentzVector& t_beforeFSR_p4, TLorentzVector& t_afterFSR_p4,
                                   TLorentzVector& W_p4,
                                   TLorentzVector& q_p4, int& q_pdgId, TLorentzVector& Wdecay1_p4,
                                   int& Wdecay1_pdgId, TLorentzVector& Wdecay2_p4, int& Wdecay2_pdgId) {
    bool hasT = false;
    bool hasW = false;
    bool hasQ = false;
    bool hasWdecayProd1 = false;
    bool hasWdecayProd2 = false;

    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle->pdgId() != start) continue;

      if (hasParticleIdenticalParent(particle)) continue; // kepping only top before FSR

      t_beforeFSR_p4 = particle->p4(); // top before FSR
      hasT = true;

      // demanding the last tops after FSR
      particle = findAfterFSR(particle);
      t_afterFSR_p4 = particle->p4(); // top after FSR

      for (size_t k = 0; k < particle->nChildren(); k++) {
        const xAOD::TruthParticle* topChildren = particle->child(k);

        if (abs(topChildren->pdgId()) == 24) {
          W_p4 = topChildren->p4();  // W boson after FSR
          hasW = true;

          // demanding the last W after FSR
          topChildren = findAfterFSR(topChildren);

          for (size_t q = 0; q < topChildren->nChildren(); ++q) {
            const xAOD::TruthParticle* WChildren = topChildren->child(q);
            if (abs(WChildren->pdgId()) < 17) {
              if (WChildren->pdgId() > 0) {
                Wdecay1_p4 = WChildren->p4();
                Wdecay1_pdgId = WChildren->pdgId();
                hasWdecayProd1 = true;
              } else {
                Wdecay2_p4 = WChildren->p4();
                Wdecay2_pdgId = WChildren->pdgId();
                hasWdecayProd2 = true;
              }//else
            }//if
          }//for
        } else if (abs(topChildren->pdgId()) == 5 || abs(topChildren->pdgId()) == 3 || abs(topChildren->pdgId()) == 1) {
          q_p4 = topChildren->p4();
          q_pdgId = topChildren->pdgId();
          hasQ = true;
        } //else if
      } //for (size_t k=0; k < particle->nChildren(); k++)
      if (hasT && hasW && hasQ && hasWdecayProd1 && hasWdecayProd2) return true;
    } //for (const xAOD::TruthParticle* particle : *truthParticles)

    return false;
  }

  bool CalcTopPartonHistory::Wlv(const xAOD::TruthParticleContainer* truthParticles,
                                 TLorentzVector& W_p4,
                                 TLorentzVector& Wdecay1_p4, int& Wdecay1_pdgId,
                                 TLorentzVector& Wdecay2_p4, int& Wdecay2_pdgId) {
    bool hasW = false;
    bool hasWdecayProd1 = false;
    bool hasWdecayProd2 = false;

    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (abs(particle->pdgId()) != 24) continue;
      //std::cout << "PDGID: " << particle->pdgId() << std::endl;

      // demanding the last W after FSR
      particle = findAfterFSR(particle);
      W_p4 = particle->p4();  // W boson after FSR
      hasW = true;

      for (size_t k = 0; k < particle->nChildren(); k++) {
        const xAOD::TruthParticle* WChildren = particle->child(k);
        if (abs(WChildren->pdgId()) < 17) {
          if (WChildren->pdgId() % 2 == 1) { // charged lepton in the Wlv case
            Wdecay1_p4 = WChildren->p4();
            Wdecay1_pdgId = WChildren->pdgId();
            hasWdecayProd1 = true;
          } else {// neutral lepton in the Wlv case
            Wdecay2_p4 = WChildren->p4();
            Wdecay2_pdgId = WChildren->pdgId();
            hasWdecayProd2 = true;
          }//else
        }//if
      } //for (size_t k=0; k < particle->nChildren(); k++)

      if (hasW && hasWdecayProd1 && hasWdecayProd2) return true;
    } //for (const xAOD::TruthParticle* particle : *truthParticles)


    return false;
  }

  // for Wt ST events, find W that is not from top
  bool CalcTopPartonHistory::Wt_W(const xAOD::TruthParticleContainer* truthParticles,
                                  TLorentzVector& W_p4, int& W_pdgId, TLorentzVector& Wdecay1_p4,
                                  int& Wdecay1_pdgId, TLorentzVector& Wdecay2_p4, int& Wdecay2_pdgId) {
    bool hasW = false;
    bool hasWdecayProd1 = false;
    bool hasWdecayProd2 = false;

    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle == nullptr) continue;
      if (abs(particle->pdgId()) != 24) continue; // W boson

      // need to check if the W is from top
      // identify the first in chain and check
      // if that particle has top as parent
      if (hasParticleIdenticalParent(particle)) continue; // kepping only W before FSR

      bool isFromTop = false;
      // now we should have only the first W in chain
      for (size_t iparent = 0; iparent < particle->nParents(); ++iparent) {
        if (particle->parent(iparent) == nullptr) continue;
        if (abs(particle->parent(iparent)->pdgId()) == 6) { // has top as parent
          isFromTop = true;
          break;
        }
      }

      if (isFromTop) continue;
      else {
        particle = findAfterFSR(particle);
        W_p4 = particle->p4();
        W_pdgId = particle->pdgId();
        hasW = true;
      }

      // check the decay products of the W
      for (size_t q = 0; q < particle->nChildren(); ++q) {
        const xAOD::TruthParticle* WChildren = particle->child(q);
        if (WChildren == nullptr) continue;
        if (abs(WChildren->pdgId()) < 17) {
          if (WChildren->pdgId() > 0) {
            Wdecay1_p4 = WChildren->p4();
            Wdecay1_pdgId = WChildren->pdgId();
            hasWdecayProd1 = true;
          } else {
            Wdecay2_p4 = WChildren->p4();
            Wdecay2_pdgId = WChildren->pdgId();
            hasWdecayProd2 = true;
          }//else
        }//if
      }//for

      if (hasW && hasWdecayProd1 && hasWdecayProd2) return true;
    } // loop over truth particles

    return false;
  }

  // for Wt ST events, find b that is not from top
  bool CalcTopPartonHistory::Wt_b(const xAOD::TruthParticleContainer* truthParticles,
                                  TLorentzVector& b_beforeFSR, TLorentzVector& b_afterFSR,
                                  int& b_pdgId) {
    bool hasB = false;

    // identify "other" b quark that is not from radiation but from ME (Wtb)
    // logic is simple: search for b quark that doesn't have top, proton, or
    // nullptr as parent

    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle == nullptr) continue;
      if (abs(particle->pdgId()) != 5) continue;

      for (size_t iparent = 0; iparent < particle->nParents(); ++iparent) {
        if (particle->parent(iparent) == nullptr) continue;

        // we dont want b-quarks that have b as parent
        if (abs(particle->parent(iparent)->pdgId()) == 5) continue;

        // we dont want b-quarks that come from top
        if (abs(particle->parent(iparent)->pdgId()) == 6) continue;

        // we dont want b-quarks that come from W
        if (abs(particle->parent(iparent)->pdgId()) == 24) continue;

        // we dont want b-quarks that come from proton
        if (abs(particle->parent(iparent)->pdgId()) == 2212) continue;

        hasB = true;
        b_beforeFSR = particle->p4();
        b_pdgId = particle->pdgId();

        // find after FSR
        particle = findAfterFSR(particle);
        b_afterFSR = particle->p4();
      }
    }


    if (hasB) return true;

    return false;
  }

  // for ttbar + photon events
  bool CalcTopPartonHistory::topPhWb(const xAOD::TruthParticleContainer* truthParticles, int topId,
                                     TLorentzVector& t_beforeFSR_p4, TLorentzVector& t_afterFSR_p4,
                                     TLorentzVector& Ph_p4, TLorentzVector& W_p4, TLorentzVector& b_p4,
                                     TLorentzVector& Wdecay1_p4, int& Wdecay1_pdgId, TLorentzVector& Wdecay2_p4,
                                     int& Wdecay2_pdgId, bool& has_ph, int& BranchType, int& IniPartonType,
                                     bool& missingTop) {
    bool hasT = false;
    bool hasW = false;
    bool hasAbsentW = false;
    bool hasB = false;

    has_ph = false;
    bool ph_W = false;
    bool ph_Top = false;
    bool ph_ISR = false;
    bool ph_b = false;
    bool hasWdecayProd1 = false;
    bool hasWdecayProd2 = false;
    missingTop = false;

    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle->pdgId() != topId) continue;

      if (hasParticleIdenticalParent(particle)) continue; // kepping only top before FSR
      BranchType = -1;// 10(50): leptonic(hadronic), 12(52):topRad, 14(54):Wrad, 15(55):ISR, 18(58):b
      IniPartonType = -1;

      // finding siblings
      for (size_t iparent = 0; iparent < particle->nParents(); iparent++) {
        if (abs(particle->parent(iparent)->pdgId()) == 21) {
          IniPartonType = 1;
        } // gg fusion
        else if (abs(particle->parent(iparent)->pdgId()) < 6) {
          IniPartonType = 2;
        } //qq annihilation

        for (size_t ichild = 0; ichild < particle->parent(iparent)->nChildren(); ichild++) {
          if (particle->parent(iparent)->child(ichild)->pdgId() == 22) {
            const xAOD::TruthParticle* photon = findAfterFSR(particle->parent(iparent)->child(ichild));
            Ph_p4 = photon->p4();
            has_ph = true;
            ph_ISR = true;
          }
          if (!missingTop &&
              (abs(particle->parent(iparent)->child(ichild)->pdgId()) == 5 ||
               abs(particle->parent(iparent)->child(ichild)->pdgId()) == 24)) {
            missingTop = true;
          }
        }
      }

      t_beforeFSR_p4 = particle->p4(); // top before FSR
      hasT = true;
      // demanding the last tops after FSR
      particle = findAfterFSR(particle);
      t_afterFSR_p4 = particle->p4(); // top after FSR

      for (size_t k = 0; k < particle->nChildren(); k++) {// top children
        const xAOD::TruthParticle* topChildren = particle->child(k);

        if (abs(topChildren->pdgId()) == 24) {
          W_p4 = topChildren->p4();  // W boson before FSR
          hasW = true;

          // demanding the last W after FSR
          topChildren = findAfterFSR(topChildren);

          for (size_t q = 0; q < topChildren->nChildren(); q++) {// W children
            const xAOD::TruthParticle* WChildren = topChildren->child(q);
            if (abs(WChildren->pdgId()) > 0 && abs(WChildren->pdgId()) < 17) {
              if (abs(WChildren->pdgId()) < 7) {
                BranchType = 50;
              }// hadronic
              else if (abs(WChildren->pdgId()) > 10 && abs(WChildren->pdgId()) < 17) {
                BranchType = 10;
              }// leptonic
              if (WChildren->pdgId() > 0) {
                WChildren = findAfterFSR(WChildren);
                Wdecay1_p4 = WChildren->p4();
                Wdecay1_pdgId = WChildren->pdgId();
                hasWdecayProd1 = true;
              } else {
                WChildren = findAfterFSR(WChildren);
                Wdecay2_p4 = WChildren->p4();
                Wdecay2_pdgId = WChildren->pdgId();
                hasWdecayProd2 = true;
              }//else
            } else if (abs(WChildren->pdgId()) == 22) {// photon
              // JUST FOR EXTRA SAFETY (not necessary)
              // check if there exists a photon already
              // if it does, check the photon's Pt
              // if found harder then consider, else do nothing
              if (has_ph) {
                if (WChildren->p4().Pt() > Ph_p4.Pt()) {
                  ph_W = true;
                  ph_ISR = false;
                  ph_Top = false;
                  ph_b = false;
                  WChildren = findAfterFSR(WChildren);
                  Ph_p4 = WChildren->p4();
                }
              } else {
                has_ph = true;
                ph_W = true;
                WChildren = findAfterFSR(WChildren);
                Ph_p4 = WChildren->p4();
              }
            }
          }// W children
        } else if (abs(topChildren->pdgId()) == 5) { // b
          hasB = true;
          topChildren = findAfterFSR(topChildren);// b After FSR
          b_p4 = topChildren->p4();
          // In MG5 generation of ttgamma it is not expected to have any b radiation 'recorded'
          for (size_t b = 0; b < topChildren->nChildren(); b++) {// b Children
            const xAOD::TruthParticle* bChildren = topChildren->child(b);
            if (bChildren && bChildren->pdgId() == 22) {
              // JUST FOR EXTRA SAFETY (not necessary)
              if (has_ph) {
                if (bChildren->p4().Pt() > Ph_p4.Pt()) {
                  ph_b = true;
                  ph_ISR = false;
                  ph_Top = false;
                  ph_W = false;
                  bChildren = findAfterFSR(bChildren);
                  Ph_p4 = bChildren->p4();
                }
              } else {
                has_ph = true;
                ph_b = true;
                bChildren = findAfterFSR(bChildren);
                Ph_p4 = bChildren->p4();
              }
            }
          }
        } else if (abs(topChildren->pdgId()) == 22) {
          // JUST FOR EXTRA SAFETY (not necessary)
          if (has_ph) {
            if (topChildren->p4().Pt() > Ph_p4.Pt()) {
              topChildren = findAfterFSR(topChildren);
              Ph_p4 = topChildren->p4();
              ph_Top = true;
            }
          } else {
            topChildren = findAfterFSR(topChildren);
            Ph_p4 = topChildren->p4();
            has_ph = true;
            ph_Top = true;
            ph_W = false;
            ph_ISR = false;
            ph_b = false;
          }
        }
        // sometimes the W is not recorded and the W products are recorded as top products
        else if (abs(topChildren->pdgId()) <= 4 || (abs(topChildren->pdgId()) > 10 && abs(topChildren->pdgId()) < 17)) {
          hasW = true;
          hasAbsentW = true;
          if (abs(topChildren->pdgId()) < 7) {
            BranchType = 50;
          }// hadronic
          else if (abs(topChildren->pdgId()) > 10 && abs(topChildren->pdgId()) < 17) {
            BranchType = 10;
          }// leptonic
          if (topChildren->pdgId() > 0) {
            topChildren = findAfterFSR(topChildren);
            Wdecay1_p4 = topChildren->p4();
            Wdecay1_pdgId = topChildren->pdgId();
            hasWdecayProd1 = true;
          } else {
            topChildren = findAfterFSR(topChildren);
            Wdecay2_p4 = topChildren->p4();
            Wdecay2_pdgId = topChildren->pdgId();
            hasWdecayProd2 = true;
          }//else
          W_p4 = W_p4 + topChildren->p4();
        }// if top children
      } // for top children

      // BranchType Determination if there is a photon
      if (hasAbsentW && (ph_Top || ph_W)) {
        BranchType = -1;
      }// if the W is not recorded and still the photon is from the top, the source of the photon is then ambiguous
       // among top and W. BranchType would be +1. Category would be 0.
      if (has_ph && ph_Top) {
        BranchType = BranchType + 2;
      }
      if (has_ph && ph_W) {
        BranchType = BranchType + 4;
      }
      if (has_ph && ph_ISR) {
        BranchType = BranchType + 5;
      }
      if (has_ph && ph_b) {
        BranchType = BranchType + 8;
      }

      if (hasT && hasW && hasB && hasWdecayProd1 && hasWdecayProd2 && BranchType != -1) return true;
    }// particle

    return false;
  }

  const xAOD::TruthParticle* CalcTopPartonHistory::findAfterFSR(const xAOD::TruthParticle* particle) {
    bool isAfterFSR(false);
    const int particle_ID = particle->pdgId();
    int forLoop = 0;

    while (!isAfterFSR) {
      forLoop = 0;
      for (size_t j = 0; j < particle->nChildren(); j++) {
        const xAOD::TruthParticle* tmp_children = particle->child(j);
        if (tmp_children && tmp_children->pdgId() == particle_ID) {
          particle = particle->child(j);
          forLoop++;
          break;
        }//if
      }//for

      if (forLoop == 0) isAfterFSR = true;
    }//while
    return particle;
  }

  bool CalcTopPartonHistory::hasParticleIdenticalParent(const xAOD::TruthParticle* particle) {
    bool skipit(false);

    for (size_t i = 0; i < particle->nParents(); i++) {
      const xAOD::TruthParticle* parent = particle->parent(i);
      if (parent && parent->pdgId() == particle->pdgId()) {
        skipit = true;
        break;
      }//if
    }//for
    return skipit;
  }

  StatusCode CalcTopPartonHistory::execute() {
    // Get the Truth Particles
    const xAOD::TruthParticleContainer* truthParticles(nullptr);

    ATH_CHECK(evtStore()->retrieve(truthParticles, m_config->sgKeyMCParticle()));

    // Create the partonHistory xAOD object
    xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer {};
    xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer {};
    partonCont->setStore(partonAuxCont);

    xAOD::PartonHistory* partonHistory = new xAOD::PartonHistory {};
    partonCont->push_back(partonHistory);

    // Save to StoreGate / TStore
    std::string outputSGKey = m_config->sgKeyTopPartonHistory();
    std::string outputSGKeyAux = outputSGKey + "Aux.";

    xAOD::TReturnCode save = evtStore()->tds()->record(partonCont, outputSGKey);
    xAOD::TReturnCode saveAux = evtStore()->tds()->record(partonAuxCont, outputSGKeyAux);
    if (!save || !saveAux) {
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  void CalcTopPartonHistory::fillEtaBranch(xAOD::PartonHistory* partonHistory, std::string branchName,
                                           TLorentzVector& tlv) {
    if (tlv.CosTheta() == 1.) partonHistory->auxdecor< float >(branchName) = 1000.;
    else if (tlv.CosTheta() == -1.) partonHistory->auxdecor< float >(branchName) = 1000.;
    else partonHistory->auxdecor< float >(branchName) = tlv.Eta();
    return;
  }
}
