/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLJDITOPS_H
#define TTBARLJDITOPS_H

#include "EventKernel/I4Momentum.h"
#include "FourMom/P4PxPyPzE.h"

#include "TopEvent/TTbarLJ.h"
#include "TopEvent/TTbarLeptonicTop.h"
#include "TopEvent/TTbarHadronicTopB.h"

#include <iostream>

namespace TopRec {

  // expose data members of TTbarHadronicTopB for copying
  class TTbarHadronicTopBAccessor {
  protected:
    // This function copies only data *added* in the TTbarHadronicTopB class.
    // One *must* copy the base part the class (TTbarHadronicTop) *before* calling
    // this function.
    inline static void copyTopBData(TTbarHadronicTopB *dst, const TTbarHadronicTopB& src) {
      dst->m_indexesW = src.m_indexesW;
      dst->m_indexB = src.m_indexB;
      dst->computeWfourMom();
    }
  };

  //================================================================
  //! Single lepton TTbar with both hadronic top and leptonic top reconstructed
  template<class LeptonCollection>
  class TTbarLJDiTops : public TTbarLJ<LeptonCollection>, protected TTbarHadronicTopBAccessor {

    P4PxPyPzE m_fourMomentum;
    TTbarHadronicTopB m_htop;
    TTbarLeptonicTop<LeptonCollection> m_ltop;

  protected:

    void copyHadronicTopB(TTbarHadronicTopB *dst, const TTbarHadronicTopB& src) const {
      this->copyHadronicTop(dst,src);
      this->copyTopBData(dst, src);
    }

    friend class TTbarLeptonicTop<LeptonCollection>;
    void copyLeptonicTop(TTbarLeptonicTop<LeptonCollection> *dst, const TTbarLeptonicTop<LeptonCollection>& src) const {
      dst->m_parent = this;
      dst->m_neutrino = src.m_neutrino;
      dst->m_indexB = src.m_indexB;
      dst->set4momenta();
    }

  public:
    TTbarLJDiTops(const TTbarLJ<LeptonCollection>& parent, const IndexSet& topJets, const size_type& index_hB, const P4PxPyPzE& neutrino, const size_type& index_lB) : TTbarLJ<LeptonCollection>(parent), m_htop(*this,topJets,index_hB), m_ltop(*this,neutrino,index_lB)
    {m_fourMomentum.set4Mom( m_htop.fourMom().hlv() + m_ltop.fourMom().hlv() );}

    //! Don't specify IndexSet& topJets (assume that topJets = all jets except leptonic b)
    TTbarLJDiTops(const TTbarLJ<LeptonCollection>& parent, const size_type& index_hB, const P4PxPyPzE& neutrino, const size_type& index_lB) : TTbarLJ<LeptonCollection>(parent), m_htop(*this,index_hB,index_lB), m_ltop(*this,neutrino,index_lB)
    {m_fourMomentum.set4Mom( m_htop.fourMom().hlv() + m_ltop.fourMom().hlv() );}

    //! default constructor is needed for POOL and dictionaries
    TTbarLJDiTops() 
    {} 

    //! Copy constructor
    TTbarLJDiTops(const TTbarLJDiTops& copy_LJDiTops)
      : TTbarLJ<LeptonCollection>(copy_LJDiTops)
	, m_htop()
	, m_ltop()
    { 
      if(this != &copy_LJDiTops) {
	if(copy_LJDiTops.htop().jetIndexes().size()!=0) {
	  TTbarLJ<LeptonCollection>::operator=(copy_LJDiTops);
	  this->copyHadronicTopB(&m_htop, copy_LJDiTops.m_htop);
	  copyLeptonicTop(&m_ltop, copy_LJDiTops.m_ltop);
	  m_fourMomentum.set4Mom( m_htop.fourMom().hlv() + m_ltop.fourMom().hlv() ); 
	}
      }
    }

    //! Assignment operator
    TTbarLJDiTops& operator=(const TTbarLJDiTops& copy_LJDiTops) {
      if(this != &copy_LJDiTops) {
	TTbarLJ<LeptonCollection>::operator=(copy_LJDiTops);
	this->copyHadronicTopB(&m_htop, copy_LJDiTops.m_htop);
	copyLeptonicTop(&m_ltop, copy_LJDiTops.m_ltop);
	m_fourMomentum.set4Mom( m_htop.fourMom().hlv() + m_ltop.fourMom().hlv() ); 
      }
      return *this;
    }
    
    const I4Momentum& fourMom() const { return m_fourMomentum; }
    
    const TTbarHadronicTopB& htop() const { return m_htop; }
    const TTbarLeptonicTop<LeptonCollection>& ltop() const { return m_ltop; }
  }; //class TTbarLJDiTops
  
  //================================================================

  typedef TTbarLJDiTops<ElectronContainer>         TTbarEJDiTops;
  typedef TTbarLJDiTops<Analysis::MuonContainer>   TTbarMuJDiTops;
  typedef TTbarLJDiTops<Analysis::TauJetContainer> TTbarTauJDiTops;
  typedef TTbarLJDiTops<TruthParticleContainer>    TTbarTruthJDiTops;
  typedef TTbarLJDiTops<JetCollection>    TTbarJeJDiTops;
 

} // namespace TopRec

#endif //TTBARLJDITOPS_H
