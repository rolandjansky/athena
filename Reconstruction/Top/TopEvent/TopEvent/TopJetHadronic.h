/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TOPJETHADRONIC_H
#define TOPJETHADRONIC_H

#include "TopEvent/TopJet.h"

namespace TopRec {

 // Class for fully hadronic "monojet" top candidates
 class TopJetHadronic : public TopJet {

 public:
   // constructors
   TopJetHadronic(const JetCollection* jc, const unsigned & index,
		  const float & split12, const float & split23, const float & split34, const float & scalefactor, const int & nsubjet,
		  const float & zcut12 ,const float & zcut23, const float & zcut34, const float & qw, const bool & use_zcuts)
		: TopJet(jc,index), m_split12(split12), m_split23(split23), m_split34(split34), m_scalefactor(scalefactor), m_nsubjet(nsubjet),
		  m_zcut12(zcut12), m_zcut23(zcut23), m_zcut34(zcut34), m_qw(qw) , m_use_zcuts(use_zcuts)
   {}

   TopJetHadronic(const ElementLink<JetCollection>& jet,
                  const float & split12, const float & split23, const float & split34, const float & scalefactor, const int & nsubjet,
                  const float & zcut12 ,const float & zcut23, const float & zcut34, const float & qw,const bool & use_zcuts)
		: TopJet(jet), m_split12(split12), m_split23(split23), m_split34(split34), m_scalefactor(scalefactor), m_nsubjet(nsubjet),
		  m_zcut12(zcut12), m_zcut23(zcut23), m_zcut34(zcut34), m_qw(qw),m_use_zcuts(use_zcuts)
   {}

   // needed for POOL and dictionaries
   TopJetHadronic()
   {}

   // Splitting scales
   float split12() const {return m_split12;}
   float split23() const {return m_split23;}
   float split34() const {return m_split34;}
   float scalefactor() const {return m_scalefactor;}
   int nsubjet() const {return m_nsubjet;}

   // z variables introduced in JHEP0807:092,2008
   float zcut12() const {return m_zcut12;}
   float zcut23() const {return m_zcut23;}
   float zcut34() const {return m_zcut34;}

   // Qw, also in JHEP0807:092,2008
   float Qw() const {return m_qw;}

   //if true, selection was done using the zcutXY variables,
   //otherwise, splitXY variables were used.
   bool use_zcuts() const {return m_use_zcuts;}

 private:
   float m_split12;
   float m_split23;
   float m_split34;
   float m_scalefactor;
   int m_nsubjet;
   float m_zcut12;
   float m_zcut23;
   float m_zcut34;
   float m_qw;
   bool m_use_zcuts;

 };

} // namespace

#endif //TOPJETHADRONIC_H
