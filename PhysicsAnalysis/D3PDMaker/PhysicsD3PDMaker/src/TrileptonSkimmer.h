// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************************************
 *  TrileptonSkimmer.h
 *
 *  authors: Steven Kaneti <Steven.KanetiSPAMNOT.cern.ch>
 *           Nick Edwards
 *  
 *  version: $Id: TrileptonSkimmer.h 606012 2014-07-11 09:40:36Z krasznaa $
 *
 *
 ******************************************************************************/

#ifndef PHYSICSD3PDMAKER_TRILEPTONSKIMMER_H
#define PHYSICSD3PDMAKER_TRILEPTONSKIMMER_H

// System include(s):
#include <string>

// Athena includes
#include "AthenaBaseComps/AthFilterAlgorithm.h"

class TrileptonSkimmer : public AthFilterAlgorithm {

public:
   /// Constructor with parameters: 
   TrileptonSkimmer( const std::string& name, ISvcLocator* pSvcLocator );

   /// @name Athena algorithm Hooks
   /// @{

   /// Function initialising the algorithm
   virtual StatusCode initialize();
   /// Function executing the algorithm
   virtual StatusCode execute();

   /// @}
        
private:
   /// Function doing the heavy lifting
   bool passEvent();

   float         m_lowPtCut;
	float         m_highPtCut;
	float         m_caloMuPtCut;
	float         m_fwdElPtCut;
	bool          m_allowMultiLepton;

   //storegate input keys
   std::string m_inCollKey_muons;
   std::string m_inCollKey_elecs;

};  //end class definition

#endif // not PHYSICSD3PDMAKER_TRILEPTONSKIMMER_H
