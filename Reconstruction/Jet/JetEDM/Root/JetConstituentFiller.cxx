/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetConstituentFiller.cxx

#include "JetEDM/JetConstituentFiller.h"
#include <map>
#include "JetEDM/IConstituentUserInfo.h"
#include "JetEDM/IndexedTConstituentUserInfo.h"
#include "JetEDM/LabelIndex.h"
#include "xAODJet/JetContainer.h"
#include "fastjet/PseudoJet.hh"
#include "xAODMuon/MuonSegmentContainer.h"
#include <iostream>

typedef std::vector<fastjet::PseudoJet> PseudoJetVector;
using xAOD::IParticle;
using jet::IConstituentUserInfo;
typedef IConstituentUserInfo::Label Label;
typedef std::vector<const IParticle*> ParticleVector;
typedef std::vector<ParticleVector> ParticleMap;
typedef std::vector<double> PtMap;
using jet::JetConstituentFiller;
using xAOD::MuonSegment;

using jet::IndexedTConstituentUserInfo;

typedef std::vector<const MuonSegment*> MuonSegmentVector;
typedef IndexedTConstituentUserInfo<MuonSegment> MuonSegmentCUI;
typedef std::vector<MuonSegmentVector> MuonSegmentMap;


//**********************************************************************

namespace {
  enum ParType { UNDEF, IPART, MUSEG };
}
typedef std::vector<ParType> ParTypeVector;

//**********************************************************************

int JetConstituentFiller::
extractConstituents(xAOD::Jet& jet, const NameList* pghostlabs,
                    const fastjet::PseudoJet* ppj2) {
  const fastjet::PseudoJet* ppseudojet = jet.getPseudoJet();
  if ( ppseudojet == 0 ) {
    ppseudojet = ppj2;
  }
  if ( ppseudojet == 0 ) return -1;
  int nbad = 0;
  const PseudoJetVector& cons = ppseudojet->constituents();
  ParticleMap out;
  MuonSegmentMap outms;

  PtMap ptout;
  ParTypeVector partypes;
  const LabelIndex* pli = nullptr;
  for ( PseudoJetVector::const_iterator icon=cons.begin(); icon!=cons.end(); ++icon ) {
    if ( icon->has_user_info<IConstituentUserInfo>() ) {
      const IConstituentUserInfo& cui = icon->user_info<IConstituentUserInfo>();
      // Initialize with the first constituent.
      if ( pli == nullptr ) {
        pli = cui.labelMap();
        if ( pli == nullptr ) return -2;
        unsigned int maxli = pli->size() + 1;  // +1 because LabelIndex starts at 1;
        out.resize(maxli);  // +1 because LabelIndex starts at 1
        outms.resize(maxli);
        for ( unsigned int ili=0; ili<maxli; ++ili ) {
          ptout.push_back(0.0);
        }
        for ( ParType& partype : partypes ) partype = UNDEF;
        for ( unsigned int idx=0; idx<maxli; ++idx ) {
          std::string lab = pli->label(idx);
          if ( lab.size() == 0 ) {
            partypes.push_back(UNDEF);
            if ( idx ) return -3;
          } else if ( lab.find("MuonSegment") != std::string::npos ) {
            partypes.push_back(MUSEG);
          } else {
            partypes.push_back(IPART);
          }
        }
      }
      if ( pli != cui.labelMap() ) return -4;
      const IParticle* ppar = cui.particle();

      const MuonSegment* pms = nullptr;

      // If this is not an IParticle, find the full type.
      unsigned int icui = cui.index();
      ParType partype = partypes[icui];
      if ( ppar == nullptr ) {
        const MuonSegmentCUI* pmscui = dynamic_cast<const MuonSegmentCUI*>(&cui);
        if ( pmscui == 0 ) return -5;
        pms = pmscui->object();
        if ( partype != MUSEG ) return -6;
      } else {
        if ( partype != IPART ) return -7;
        ptout[icui] += ppar->pt();
      }
      Label lab = cui.label();
      // Add to constituent list or associated object list.
      if ( ! cui.isGhost() ) {
        jet.addConstituent(ppar);
        jet.setConstituentsSignalState( cui.constitScale() );
      } else {
        if ( partype == MUSEG ) {
          outms[icui].push_back(pms); 
        } else if ( partype == IPART ) {
          out[icui].push_back(ppar); 
        } else {
          return -8;
        }
      }
    } else {
      ++nbad;
    }
  }
  
  // Set ghost associated particles:
  for ( size_t i=1; i<out.size(); ++i ) {
    if ( pghostlabs != nullptr ) {
      const NameList& ghostlabs = *pghostlabs;
      if ( find(ghostlabs.begin(), ghostlabs.end(), pli->label(i)) == ghostlabs.end() ) {
        nbad += out[i].size();
        continue;
      }
    }
    ParType& partype = partypes[i];
    std::string cname = pli->label(i) + "Count";
    std::string ptname = pli->label(i) + "Pt";
    // Check if this is in the parent jet
    int count_test; // dummy var to retrieve into -- we don't care about the value
    const static SG::AuxElement::ConstAccessor<ElementLink<xAOD::JetContainer> > cacc_parent("Parent");
    if(!m_isTrigger) {
      if(cacc_parent.isAvailable(jet) && cacc_parent(jet).isValid()) {
        if(!(*cacc_parent(jet))->getAttribute(cname,count_test)) {
          nbad += out[i].size(); // Skip if the parent does not have this
          continue;
        }
      }
    }
    if ( partype == MUSEG ) {
      // Record associated muons.
      jet.setAssociatedObjects(pli->label(i) , outms[i]);
      jet.setAttribute<int>(cname, outms[i].size());
    } else if ( partype == IPART ) {
      // Record associated particles.
      jet.setAssociatedObjects(pli->label(i), out[i]);
      jet.setAttribute<int>(cname, out[i].size());
      jet.setAttribute<float>(ptname, ptout[i]);
      if ( ! outms[i].empty() ) return -9;
    } else {
      return -10;
    }
  }
  
  return nbad;
}

int JetConstituentFiller::extractConstituents(xAOD::Jet& jet, const fastjet::PseudoJet* ppj2) {
  return extractConstituents(jet, 0, ppj2);
}

//**********************************************************************

PseudoJetVector JetConstituentFiller::constituentPseudoJets(const xAOD::Jet& jet, bool ignoreGhosts){

  const fastjet::PseudoJet* jet_pj = jet.getPseudoJet();

  PseudoJetVector constituents;

  if( jet_pj ) { // Retrieve constituents from the PseudoJet
    
    if( ! ignoreGhosts) constituents = jet_pj->constituents(); // all constituents, including ghosts
    else{  // remove ghosts
      PseudoJetVector constituents_tmp = jet_pj->constituents();
      constituents.reserve(constituents_tmp.size());
      PseudoJetVector::iterator it = constituents_tmp.begin();
      PseudoJetVector::iterator itE = constituents_tmp.end();
      for( ; it !=itE; ++it) if(it->has_user_info<IConstituentUserInfo>() ) {
          if( ! it->user_info<IConstituentUserInfo>().isGhost() ) constituents.push_back( *it );
        }
    }

  } else { // no PseudoJet in jet. Build them from constituents.
    xAOD::JetConstituentVector constituents_tmp = jet.getConstituents();
    constituents.reserve( jet.numConstituents() );
    xAOD::JetConstituentVector::iterator it = constituents_tmp.begin();
    xAOD::JetConstituentVector::iterator itE = constituents_tmp.end();
    for( ; it !=itE; ++it) constituents.push_back( fastjet::PseudoJet( it->Px(), it->Py(), it->Pz(), it->E() ) );    

  }
  return constituents;
}
//**********************************************************************

