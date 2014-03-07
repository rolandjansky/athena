/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TOPJET_H
#define TOPJET_H

#include "DataModel/ElementLink.h"
#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"

namespace TopRec {

 // This is the base class for candidate high pT top decays

 class TopJet {

 public:
   // constructor: give the jet collection and index
   TopJet(const JetCollection* jc, const unsigned & index)
	 : m_jet(ElementLink<JetCollection>(*jc,index)) 
   {}
   //  or directly the link to the jet:
   TopJet(const ElementLink<JetCollection>& jet)
	 : m_jet(jet) 
   {} 

   // needed for POOL and dictionaries
   TopJet()
   {}

   // The only common thing is really the jet we start from
   const Jet& jet() const {return **m_jet;}
   ElementLink<JetCollection> jetLink() const {return m_jet; }   

 private:
   // store the elementlink to the jet
   ElementLink<JetCollection>  m_jet;

 };

} // namespace

#endif // TOPJET_H
