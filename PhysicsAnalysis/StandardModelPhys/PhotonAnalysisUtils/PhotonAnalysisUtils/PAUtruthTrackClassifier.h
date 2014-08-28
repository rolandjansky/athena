///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PAUtruthTrackClassifier.h 
// Header file for class PAUtruthTrackClassifier
// Author: H.Abreu<abreu@lal.in2p3.fr>
/////////////////////////////////////////////////////////////////// 
#ifndef PHOTONANALYSISUTILS_PAUTRUTHTRACKCLASSIFIER_H
#define PHOTONANALYSISUTILS_PAUTRUTHTRACKCLASSIFIER_H 1

// STL includes
#include <string>

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "PhotonAnalysisUtils/IPAUtruthTrackClassifier.h"
#include "PhotonAnalysisUtils/PAUnamespace.h" //to define PAU::gamma

// GaudiKernel includes
#include "GaudiKernel/IAlgTool.h"

#include "MCTruthClassifier/IMCTruthClassifier.h"

// Forward declaration
class StoreGateSvc;



class PAUtruthTrackClassifier
  : virtual public ::IPAUtruthTrackClassifier,
            public ::AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  PAUtruthTrackClassifier( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );
 PAUtruthTrackClassifier();
 
 /// Destructor: 
  virtual ~PAUtruthTrackClassifier(); 

  // Athena algtool's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  finalize();

  void ConvPhotClassifier( PAU::egamma* g );

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  // =============================	
  int vector_pdgID[2];
  int vector_pdgMOTHER[2];
  int vector_barcodeMOTHER[2];
  int vector_pdgGrandMOTHER[2];
  int vector_Type[2];
  int vector_Origin[2];
  int vector_phtType[2];
  int vector_phtOrigin[2];
  // ============================

  ToolHandle<IMCTruthClassifier> m_TruthClassifierTool;

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  /// Pointer to the StoreGate service
  StoreGateSvc_t m_storeGate;

  // Containers
  

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !PHOTONANALYSISUTILS_PAUTRUTHTRACKCLASSIFIER_H
