/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTAGTOOL_H 
#define MUONTAGTOOL_H 

/*****************************************************************************
Name    : MuonTagTool.h
Package : offline/PhysicsAnalysis/MuonID/MuonTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the Muon Tag objects - MuonTagCollection.h. 
	  The Muon Tag fragment is built here
*****************************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "GaudiKernel/ToolHandle.h"
#include <inttypes.h>

#include <map>

namespace Analysis {
  class MuonContainer;
  class Muon;
}
namespace Reco {
  class ITrackToVertex;
}

namespace Trk {
  class ITrackToVertexIPEstimator;
}

namespace Muon {
  class MuonIdHelperTool;
}

/** Interface ID for MuonTagTool*/  
static const InterfaceID IID_MuonTagTool("MuonTagTool", 1, 0);

class MuonTagTool : public AthAlgTool {

public:
  
  /** Standard Constructor */
  MuonTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_MuonTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int max);
  virtual StatusCode execute(TagFragmentCollection& muonTagCol, const int max);
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~MuonTagTool( );

private:

   inline int bit2int(int b) const { return 1<<b; };

  /** private function to get impact parameter */
   /*     void getMuonImpactParameter (const xAOD::Muon*,
				//const Analysis::Muon*,
				double& d0, double& z0, double& dd0, double& dz0, double& theta, bool biased);
   */


  /** Properties */
  std::vector<std::string> m_containerNames;
  double m_cut_Et;
  double m_DRcut;
  std::vector<float> m_caloisocutvalues;
  std::vector<float> m_caloisorelcutvalues;
  std::vector<float> m_trackisocutvalues;
  std::vector<float> m_trackisorelcutvalues;
  double m_maxD0preselection;

  bool m_cosmics;
  bool m_doInDet;
  double m_maxD0tight, m_maxZ0tight, m_maxD0signLoose, m_maxD0signTight;

  /** the attribute names */
  std::vector<std::string> m_ptStr;
  std::vector<std::string> m_etaStr;
  std::vector<std::string> m_phiStr;
  std::vector<std::string> m_tightStr;
  std::vector<std::string> m_momBalStr;
  std::vector<std::string> m_nprecLayStr;
  std::vector<std::string> m_nprecHoleLayStr;
  std::vector<std::string> m_nphiLayStr;
  std::vector<std::string> m_isoStr;

  /** PV StoreGate key */
  std::string m_vxCandidate;
  /** track to vertex extrapolator */
  ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
  ToolHandle< Trk::ITrackToVertexIPEstimator > m_trackToVertexIPEstimator;
  ToolHandle<Muon::MuonIdHelperTool>    m_idHelper;
 };

#endif // MUONTAGTOOL_H




