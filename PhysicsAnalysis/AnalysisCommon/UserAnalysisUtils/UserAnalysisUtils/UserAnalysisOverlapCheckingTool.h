/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef USERANALYSISUTILS_USERANALYSISOVERLAPCHECKINGTOOL_H  
#define USERANALYSISUTILS_USERANALYSISOVERLAPCHECKINGTOOL_H 

/*****************************************************************************
Name    : UserAnalysisOverlapCheckingTool.h
Package : offline/PhysicsAnalysis/AnalysisCommon/UserAnalysisUtils
Author  : Ketevi A. Assamagan
Created : November 2007
Purpose : User tools for checking overlaps at deltaR, TrackParticle/Cluster and Hit/Cell levels
*****************************************************************************/

//#include "GaudiKernel/AlgTool.h"
//#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "VxVertex/VxContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "TrkSegment/SegmentCollection.h"

#include "muonEvent/MuonContainer.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/PhotonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "JetEvent/JetCollection.h"
#include "MissingETEvent/MissingET.h"

#include "NavFourMom/IParticleContainer.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "AnalysisTools/AnalysisTools.h"

#include <string>
#include <map>
#include <vector>

class MsgStream;

/** Interface ID */  
static const InterfaceID IID_UserAnalysisOverlapCheckingTool("UserAnalysisOverlapCheckingTool", 1, 0);

class UserAnalysisOverlapCheckingTool : public AthAlgTool {

public:

  /** Standard Constructor */
  UserAnalysisOverlapCheckingTool(const std::string& type, const std::string& name,
	                          const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_UserAnalysisOverlapCheckingTool; };

  /** Overriding initialize, finalize, and execute */
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /** overlaps */
  template<class Object1, class Object2>
  bool overlap(const Object1* object1, const Object2* object2) const ;

  template<class Object1, class Object2>
  bool overlap(const Object1* object1, const Object2* object2, double& deltaR) const ;

protected:

   /** Standard destructor */
   virtual ~UserAnalysisOverlapCheckingTool();

private:


  /** get a handle to the tool helper */
  ToolHandle<AnalysisTools> m_analysisTools;

  /** deltaR overlap */
  double m_deltaR;
  double m_deltaRWithJets;

};

/** check for oeverlap in deltaR and return as well the deltaR value */
template<class Object1, class Object2>
inline bool UserAnalysisOverlapCheckingTool::overlap(const Object1* object1,
                                                     const Object2* object2) const {
  double dr=-1.0;
  bool areOverlapping = false;
  areOverlapping = this->overlap( object1, object2, dr);

  return areOverlapping;
}

template<class Object1, class Object2>
inline bool UserAnalysisOverlapCheckingTool::overlap(const Object1* object1,
                                                     const Object2* object2,
                                                     double& deltaR) const
{
   deltaR=1000.0;
   bool areOverlapping = false;
   if ( object1 && object2) {
      deltaR = m_analysisTools->deltaR( object1, object2 );
      areOverlapping = deltaR < m_deltaR;
      const Jet * jet1 = dynamic_cast<const Jet*> (object1);
      const Jet * jet2 = dynamic_cast<const Jet*> (object2);
      if ( jet1 || jet2 ) areOverlapping = deltaR < m_deltaRWithJets;
   }
   return areOverlapping;
}

#endif // USERANALYSISUTILS_USERANALYSISOVERLAPCHECKINGTOOL_H 




