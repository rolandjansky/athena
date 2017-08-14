/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#ifndef INDETVERTEXVALIDATION_INDETVERTEXVALIDATIONTOOL_H
#define INDETVERTEXVALIDATION_INDETVERTEXVALIDATIONTOOL_H

// STL includes
#include <string>

// Local includes
#include "InDetVertexValidation/VertexAnalysis.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

// Tools
//#include "PileupReweighting/PileupReweightingTool.h"
//#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
//#include "TrackVertexAssociationTool/LooseTrackVertexAssociationTool.h"
//#include "MCTruthClassifier/MCTruthClassifierDefs.h"
//#include "MCTruthClassifier/IMCTruthClassifier.h"

// Framework include(s):
#include "AsgTools/AsgTool.h"

#include <vector>

//fwd declaration
//class InDetVertexPlots;

class InDetVertexValidationTool:public ManagedMonitorToolBase{
public:
	// Constructor with parameters
	InDetVertexValidationTool(const std::string & type, const std::string & name, const IInterface* parent);
	// Destructor
	virtual ~InDetVertexValidationTool();
	
	//virtual StatusCode initialize();
	virtual StatusCode bookHistograms();
	virtual StatusCode fillHistograms();
	virtual StatusCode procHistograms();

private:
	// Pevent default construction
	InDetVertexValidationTool();

	// Primary vertex container's name
	std::string m_vertexContainerName;
	// Truth vertex container's name
	std::string m_truthvertexContainerName;
	// EventInfo container name
	std::string m_eventInfoContainerName;
	// Track container name
	std::string m_trackContainerName;
	// Truth Event container name
	std::string m_trutheventContainerName;
	// Truth Pileup Event container name
	std::string m_trutheventPUContainerName;

	// Directory name
	std::string m_dirName;

	float m_MaxLayoutEta;
	double m_muCorrected;
	double m_muWeight;

	// Histograms
	//std::unique_ptr< InDetVertexPlots > m_monPlots;

	//std::string m_folder;

	// Create the analyzer object
	VertexAnalysis analyzer;

	// Tools
	//ToolHandle<CP::IPileupReweightingTool> m_prwTool;
	//ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectionTool;
	ToolHandle<CP::ITrackVertexAssociationTool> m_tighttrackvertexassoTool;
	ToolHandle<CP::ITrackVertexAssociationTool> m_loosetrackvertexassoTool;
	
	//ToolHandle<IMCTruthClassifier> m_truthClassifier;

	template<class T>
	  const T* getContainer( const std::string & containerName);
};

template<class T>
const T* InDetVertexValidationTool::getContainer(const std::string & containerName){
  const T * ptr = evtStore()->retrieve< const T >( containerName );
  if (!ptr) {
    ATH_MSG_ERROR("Container '"<<containerName<<"' could not be retrieved");
  }
  return ptr;
}

#endif

#endif
