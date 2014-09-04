/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef ANALYSISUTILS_ELECTRONIDSELECTORCORE_H 
#define ANALYSISUTILS_ELECTRONIDSELECTORCORE_H 

/**
   @Project: AnalysisUtils
 
   @class ElectronIDSelectorCore
   
   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>
   
   @date August 2010
   
   @brief Dual use tool (athena/ARA) for specialized electron identification cuts.
*/


// STL includes
#include <string>
#include <cfloat>

// Atlas includes
#include "GaudiKernel/ToolHandle.h"
#include "AraTool/AraToolBase.h"
#include "AnalysisUtils/ISelectorCore.h"
#include "EventKernel/INavigable4Momentum.h"



static const InterfaceID IID_ElectronIDSelectorCore("ElectronIDSelectorCore", 1, 0);


class ElectronIDSelectorCore : public ISelectorCore, virtual public AraToolBase
{
public:
  /** Default contructor */
  ElectronIDSelectorCore(PropertyMgr *pmgr=0);
  
  /** Default destructor */
  virtual ~ElectronIDSelectorCore() {}

public:
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_ElectronIDSelectorCore; };

public:
  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();



public:
  /** Main method, all cuts are applied */
  bool accept( const INavigable4Momentum* part ) const;


  // Private member functions
private:
  /** Gets the Reta cut given eT (MeV) and eta */
  double getREtaCut( double eT, double eta) const;

  /** Gets the w2 cut given eT (MeV) and eta */
  double getW2Cut(double eT, double eta) const;


  /** Gets the Eta bin [0-9] given the eta */
  unsigned int getEtaBin(double eta) const;

  /** Gets the Et bin [0-10] given the et (MeV) */
  unsigned int getEtBin(double eT) const;




  // Private class variables  
private:
  /** Name of the EMShower container */
  std::string m_showerContainerName;

  /** IsEM cut */
  std::string m_isEMCut;

  /** IsEM from the electron */
  unsigned int m_isEM;

  /** IsEM from the electron for robusterTight with b-layer check */
  unsigned int m_isEM_NoConvCut;

  

};

#endif // ANALYSISUTILS_ELECTRONIDSELECTORCORE_H 
