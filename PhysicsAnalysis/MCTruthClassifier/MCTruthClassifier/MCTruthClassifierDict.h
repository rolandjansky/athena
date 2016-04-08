/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHCLASSIFIER_MCTRUTHCLASSIFIERDICT_H
#define MCTRUTHCLASSIFIER_MCTRUTHCLASSIFIERDICT_H
	
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "MCTruthClassifier/MCTruthClassifier.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
	
 namespace MCTruthClassifierEnumsDict {
	
   struct tmp {
     std::pair<MCTruthPartClassifier::ParticleType,
       MCTruthPartClassifier::ParticleOrigin> m_1;
   };
 }
#endif // MCTRUTHCLASSIFIER_MCTRUTHCLASSIFIERDICT_H
