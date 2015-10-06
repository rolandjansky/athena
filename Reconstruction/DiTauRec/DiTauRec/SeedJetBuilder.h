/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_SEEDJETBUILDER_H
#define DITAUREC_SEEDJETBUILDER_H

#include "DiTauToolBase.h"


class SeedJetBuilder : public DiTauToolBase {
public:

 //-------------------------------------------------------------
 //! Constructor
 //-------------------------------------------------------------
 SeedJetBuilder(const std::string& type,
         const std::string& name,
         const IInterface * parent);

 //-------------------------------------------------------------
 //! Destructor
 //-------------------------------------------------------------
 virtual ~SeedJetBuilder();

 virtual StatusCode initialize();

 virtual StatusCode execute(DiTauCandidateData * data);

 virtual StatusCode eventFinalize(DiTauCandidateData *data);

 virtual void cleanup(DiTauCandidateData *) { }

private:
	std::string m_jetContainerName;
};

#endif  /* SEEDJETBUILDER_H */