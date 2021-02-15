/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// JetConstituentsRetriever.cxx

#include "JetRec/JetConstituentsRetriever.h"
#include "fastjet/PseudoJet.hh"
#include "JetEDM/LabelIndex.h"
#include "JetEDM/IndexedConstituentUserInfo.h"
#include "JetEDM/IndexedTConstituentUserInfo.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"

using std::string;
using xAOD::IParticle;
using xAOD::MuonSegment;

typedef std::vector<const IParticle*> IParticleVector;
typedef std::vector<const MuonSegment*> MuonSegmentVector;
typedef IJetConstituentsRetriever::PseudoJetVector PseudoJetVector;
typedef IJetConstituentsRetriever::NameList NameList;

//**********************************************************************

JetConstituentsRetriever::JetConstituentsRetriever(std::string name)
  : AsgTool(name), m_hpjr("",this) {
  declareProperty("UsePseudojet", m_usepj =true);
  declareProperty("UseJetConstituents", m_usejetcons =true);
  declareProperty("PseudojetRetriever", m_hpjr);
  declareProperty("GhostLabels", m_glabs);
  declareProperty("GhostScale", m_gscale);
}

//**********************************************************************

StatusCode JetConstituentsRetriever::initialize() {
  if ( m_usepj ) {
    if ( m_hpjr.empty() ) {
      ATH_MSG_ERROR("  PseudojetRetriever is missing.");
    }
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetConstituentsRetriever::
constituents(const xAOD::Jet& jet, PseudoJetVector& constits,
             NameList* pghostlabs) const {
  // If flag is set, try to retrieve constituents from jet pseudojet.
  if ( m_usepj ) {
    ATH_MSG_VERBOSE("Using pseudojet to search for constituents.");
    if ( m_hpjr.empty() ) {
      ATH_MSG_WARNING("Pseudojet retriever not found.");
    } else {
      const fastjet::PseudoJet* ppj = m_hpjr->pseudojet(jet);
      if ( ppj == nullptr ) {
        ATH_MSG_VERBOSE("Pseudojet not found.");
      } else {
        if ( ! ppj->has_constituents() ) {
          ATH_MSG_WARNING("Jet pseudojet does not have constituents.");
        } else {
          PseudoJetVector pjcons = ppj->constituents();
          constits.insert(constits.end(), pjcons.begin(), pjcons.end());
          return 0;
        } 
      }
    }
  }
  // Remainder requires access to the label index map;
  jet::LabelIndex* pli = nullptr;
  const string liname = "PseudoJetLabelMap";
  if ( evtStore()->contains<jet::LabelIndex>(liname) ) {
    ATH_MSG_DEBUG("Fetching existing pseudojet label map.");
    pli = evtStore()->retrieve<jet::LabelIndex>(liname);
    if ( pli == 0 ) {
      ATH_MSG_ERROR("Unable to fetch pseudojet label map.");
      return 1;
    }
  } else {
    ATH_MSG_DEBUG("Creating new pseudojet label map.");
    pli = new jet::LabelIndex(liname);
    if ( evtStore()->record(pli, liname).isFailure() ) {
    ATH_MSG_ERROR("Unable to store pseudojet label map.");
      return 2;
    }
  }
  // Add ATLAS jet constituents.
  if ( m_usejetcons ) {
    xAOD::JetInput::Type intype = jet.getInputType();
    string lab = xAOD::JetInput::typeName(intype);
    int labidx = pli->addLabel(lab);
    ATH_MSG_DEBUG("Jet input type is " << lab);
    ATH_MSG_VERBOSE("Looping over " << jet.getConstituents().size()
                    << " real constituents.");
    for ( const auto pjetcon : jet.getConstituents() ) {
      if ( pjetcon == nullptr ) {
        ATH_MSG_WARNING("Jet has null constituent");
      } else {
        //fastjet::PseudoJet psj(*pjetcon);
        fastjet::PseudoJet psj(pjetcon->px(), pjetcon->py(), pjetcon->pz(), pjetcon->e());
        const xAOD::IParticle* ppar = pjetcon->rawConstituent();
        jet::IConstituentUserInfo* pcui =
          new jet::IndexedConstituentUserInfo(*ppar, labidx, pli);
        psj.set_user_info(pcui);
        constits.push_back(psj);
        ATH_MSG_VERBOSE("  Real user info: particle=" << pcui->particle()
                        << ", index=" << pcui->index()
                        << ", label=" << pcui->label()
                        << ", isGhost=" << pcui->isGhost());
      }
    }
  }
  // Add ghosts.
  NameList glabs;
  ATH_MSG_VERBOSE("Looping over " << m_glabs.size() << " ghost labels.");
  for ( std::string glab : m_glabs ) {
    string gname = "Ghost" + glab;
    double fac = m_gscale;
    if ( glab == "MuonSegment" ) {
      MuonSegmentVector psegs;
      if ( jet.getAssociatedObjects(gname, psegs) ) {
        ATH_MSG_VERBOSE("  Found Muon segment vector " << gname << " with "
                            << psegs.size() << " entries.");
        int labidx = pli->addLabel(glab);
        glabs.push_back(glab);
        unsigned int iseg = 0;
        for ( const MuonSegment* pseg : psegs ) {
          if ( pseg == nullptr ) {
            ATH_MSG_DEBUG("Muon segment not found: " << gname << "[" << iseg << "]");
          } else {
            // Cut and paste from MuonSegmentPseudoJetGetter.
            double pt = 1.0e-20;   // Should be  property?
            double x = pseg->x();
            double y = pseg->y();
            double z = pseg->z();
            double xy = sqrt(x*x + y*y);
            double r = xy*xy + z*z;
            double pfac = pt/xy;
            double px = pfac*x;
            double py = pfac*y;
            double pz = pfac*z;
            double  e = pfac*r;
            fastjet::PseudoJet psj(px, py, pz, e);
            jet::IConstituentUserInfo* pcui =
              new jet::IndexedTConstituentUserInfo<MuonSegment>(*pseg, -labidx, pli);
            psj.set_user_info(pcui);
            constits.push_back(psj);
            ATH_MSG_VERBOSE("    Muon ghost user info: particle=" << pcui->particle()
                            << ", index=" << pcui->index()
                            << ", label=" << pcui->label()
                            << ", isGhost=" << pcui->isGhost());
            ATH_MSG_VERBOSE("    Adding " << gname << "[" << iseg << "]");
          }
          ++iseg;
        }
      } else {
        ATH_MSG_VERBOSE("  Muon segment vector " << gname << " is not found on jet.");
      }

    } else {
      IParticleVector ppars;
      if ( jet.getAssociatedObjects(gname, ppars) ) {
        ATH_MSG_VERBOSE("  Found IParticle vector " << gname << " with "
                            << ppars.size() << " entries.");
        int labidx = pli->addLabel(glab);
        glabs.push_back(glab);
        unsigned int ipar = 0;
        for ( const IParticle* ppar : ppars ) {
          if ( ppar == nullptr ) {
            ATH_MSG_DEBUG("IParticle not found: " << gname << "[" << ipar << "]");
          } else {
            auto p4 = ppar->p4();
            fastjet::PseudoJet psj(fac*p4.Px(), fac*p4.Py(), fac*p4.Pz(), fac*p4.E());
            jet::IConstituentUserInfo* pcui =
              new jet::IndexedConstituentUserInfo(*ppar, -labidx, pli);
            psj.set_user_info(pcui);
            constits.push_back(psj);
            ATH_MSG_VERBOSE("    Ghost user info: particle=" << pcui->particle()
                            << ", index=" << pcui->index()
                            << ", label=" << pcui->label()
                            << ", isGhost=" << pcui->isGhost());
            ATH_MSG_VERBOSE("    Adding " << gname << "[" << ipar << "]");
          }
          ++ipar;
        }
      } else {
        ATH_MSG_VERBOSE("  IParticle vector " << gname << " is not found on jet.");
      }
    }
  }
  if ( pghostlabs != nullptr ) {
    pghostlabs->insert(pghostlabs->end(), glabs.begin(), glabs.end());
  }
  return 1;
}

//**********************************************************************

void JetConstituentsRetriever::print() const {
  ATH_MSG_INFO("JetConstituentsRetriever");
  ATH_MSG_INFO("  UsePseudojet: " << m_usepj);
  if ( m_usepj ) {
    if ( m_hpjr.empty() ) {
      ATH_MSG_INFO("  PseudojetRetriever is missing.");
    } else {
      ATH_MSG_INFO("  PseudojetRetriever: " << m_hpjr->name());
    }
  }
  ATH_MSG_INFO("  UsePseudojet: " << m_usepj);
  if ( m_glabs.size() == 0 ) {
    ATH_MSG_INFO("  GhostLabels:");
    for ( std::string glab : m_glabs ) {
      ATH_MSG_INFO("    " << glab);
    }
  }
  ATH_MSG_INFO("  GhostScale: " << m_gscale);
}

//**********************************************************************
