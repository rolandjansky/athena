/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef ANALYSISUTILS_CONTAINERFILTERCORE_H 
#define ANALYSISUTILS_CONTAINERFILTERCORE_H 

#include "GaudiKernel/ToolHandle.h"
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "AraTool/AraToolBase.h"
#include "AnalysisUtils/ISelector.h"
#include "DataModel/DataVector.h"

static const InterfaceID IID_ContainerFilterCore("ContainerFilterCore", 1, 0);

class ContainerFilterCore : virtual public AraToolBase
{
public:
  /** Default contructor */
  ContainerFilterCore(PropertyMgr *pmgr=0);
  
  /** Default destructor */
  virtual ~ContainerFilterCore() {}

  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_ContainerFilterCore; };

  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();
  
  void addSelector(ISelectorCore *selector);

  DataVector<INavigable4Momentum> *filter(DataVector<INavigable4Momentum> * in);

private:
  bool filterParticleAthena(INavigable4Momentum *part);
  bool filterParticleARA(INavigable4Momentum *part);

  ToolHandleArray< ISelector > m_selectorHandles;
  std::vector<ISelectorCore *> m_selectors;
};

#endif // ANALYSISUTILS_CONTAINERFILTERCORE_H 
