// this file is -*- C++ -*-



#ifndef PseudoJetContainer_H
#define PseudoJetContainer_H

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// PseudoJetContainer - maintains a list of Pseudovctors, and the the
// EDM objects used to create them. It is used to supply Pseudojets
// for clustering, and to identify the EDM objects with the Pseudojets
// in a Pseudojet cluster.

// - maintains a list of pseudojets to be used for clustering by fastjet
//   fastjet returns a clustered set of pseudojets in the form of a single
//   pseudojet with components. The components are the pseudojets in the cluster
//   Note: fastjet may also add its own pseudojets to the cluster.
//
// - has association objects (Extractors) which allow the 
//   identification of the EDM object for a given pseudojet.
//   When PseudoJetContainer::extractConstituents() is passed a jet and a 
//   Pseudojet, it finds the components of the pseudojet (which are
//   also held in the PseudoJetContainer pseudojet list), sorts them by
//   type. For each type, it asks the appropriate Extractor to add
//   the EDM objects to the jet.
//
//   PseudoJetContainers can coelesce with other PseudoJetContainers via the 
//   append() method. 

// Johannes Elmsheuser, April 2016
// P Sherwood, December 2107

#include "fastjet/PseudoJet.hh"
#include "xAODJet/Jet.h"
#include "JetRec/IConstituentExtractor.h"
#include <vector>
#include <string>
#include <set>
#include <memory>

typedef std::vector<fastjet::PseudoJet> PseudoJetVector;

class PseudoJetContainer {
public:
  typedef fastjet::PseudoJet PseudoJet;

  PseudoJetContainer();

  // Constructor from a ConstituentExtractor and vector<PseudoJet>
  // PseudoJet OWNS their ConstituentExtractors
  PseudoJetContainer(std::unique_ptr<const IConstituentExtractor> c, 
                     const std::vector<PseudoJet> & vecPJ,
                     bool debug=false);

  // fill xAOD jet with constit&ghosts extracted from final PSeudoJet
  bool extractConstituents(xAOD::Jet&, const std::vector<PseudoJet>&) const;
  bool extractConstituents(xAOD::Jet& jet, const PseudoJet &finalPJ) const;

  // returns the list of input constituents
  // typically to give to a fastjet::ClusterSequence 
  const std::vector<PseudoJet>* casVectorPseudoJet() const;

  // combine the contents of a PseudoJetContainer with the current container.
  void append(const PseudoJetContainer*);

  // dump function with levels of detail. the second arg gives the
  // level of detail for the contained extractors
  std::string toString(int level, int extLevel=0) const;
  std::size_t size() const;
  std::string dumpPseudoJets() const;

  bool  debug() const;
  void  debug(bool b) const;
  friend std::ostream& operator<<(std::ostream&, const PseudoJetContainer&);  

private:


  bool checkInvariants(const std::string&) const;
  bool bad_invariants_exit(const std::ostringstream&) const;

  bool checkPseudoJetVector(const std::vector<PseudoJet>&,
                            const std::string&) const;

  bool checkInConstituents(const std::vector<PseudoJet>&,
                           const std::string&) const;

  // all pseudojets, used by fastjet
  std::vector<PseudoJet> m_allConstituents; 


  // List of associated Extractors, 
  // which contains the EDM objects whose momentum 
  // is used to a pseudojet. Note that only some of the  EDM objects
  // give rise to pseudojets, an the pseudojet index range the are valid for.
  // Mapping of pseudojet indices to Extractor
  
  struct ExtractorRange {
    ExtractorRange(unsigned int lo, 
                   unsigned int hi, 
                   std::unique_ptr<const IConstituentExtractor> e):
      m_lo(lo), m_hi(hi), m_e(std::move(e)){
    }

    ExtractorRange(const ExtractorRange& other)
      : m_lo(other.m_lo),
        m_hi(other.m_hi),
        m_e(other.m_e->clone()){
    }

    friend void swap(ExtractorRange& first, ExtractorRange& second){
      using std::swap;

      swap(first.m_lo, second.m_lo);
      swap(first.m_hi, second.m_hi);
      swap(first.m_e, second.m_e);
    }

    ExtractorRange& operator= (ExtractorRange other){
      swap(*this, other);
      return *this;
    }
    
    ExtractorRange bump(int step) const {
      return ExtractorRange(m_lo + step, m_hi + step, std::unique_ptr<const IConstituentExtractor>(m_e->clone()));
    }

    int m_lo;
    int m_hi;
    std::unique_ptr<const IConstituentExtractor> m_e{};
  };

  std::vector<ExtractorRange> m_extractorRanges;

  //Empty extractors are those with EDM objects for which no Pseudojet was 
  // created by the creating PseudoJetGetter. We need to keep track
  // of the empty extractors to fill zero information (such as 0 counts)
  // into the jets.
  std::set<std::unique_ptr<const IConstituentExtractor>> m_emptyExtractors;

  mutable bool m_debug{false};
};

std::ostream& operator << (std::ostream&, const PseudoJetContainer&);

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( PseudoJetContainer , 147270789 , 1 )

#endif
