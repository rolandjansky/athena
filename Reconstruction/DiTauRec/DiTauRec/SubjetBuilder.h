/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_SUBJETBUILDER_H
#define DITAUREC_SUBJETBUILDER_H

#include "DiTauToolBase.h"

#include "fastjet/tools/Filter.hh"

class SubjetBuilder : public DiTauToolBase {
public:

 //-------------------------------------------------------------
 //! Constructor
 //-------------------------------------------------------------
 SubjetBuilder(const std::string& type,
         const std::string& name,
         const IInterface * parent);

 //-------------------------------------------------------------
 //! Destructor
 //-------------------------------------------------------------
 virtual ~SubjetBuilder();

 virtual StatusCode initialize();

 virtual StatusCode execute(DiTauCandidateData * data);

 virtual StatusCode eventFinalize(DiTauCandidateData *data);

 virtual void cleanup(DiTauCandidateData *) { }


private:

 float m_Rsubjet;
 int m_Nsubjet;
 float m_ptmin;

 // fastjet::Filter m_SubjetFilter;

};

#endif  /* SUBJETBUILDER_H */