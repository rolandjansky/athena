// this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEDMINCLUDE_H
#define JETEDMINCLUDE_H

#ifdef JETEDM_STANDALONE
#include "JetAnalysisEDM/Particle.h"
typedef JetAnalysisEDM::Particle Jet;
typedef JetAnalysisEDM::ParticleContainer JetCollection;
typedef JetAnalysisEDM::Particle CaloCluster;
typedef JetAnalysisEDM::ParticleContainer CaloClusterContainer;
#else
#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"
#endif

#endif
