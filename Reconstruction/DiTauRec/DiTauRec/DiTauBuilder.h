/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_DITAUBUILDER_H
#define DITAUREC_DITAUBUILDER_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "DiTauToolBase.h"
#include "GaudiKernel/ToolHandle.h"


class DiTauBuilder: public ::AthAlgorithm { 
	public: 
		DiTauBuilder( const std::string& name, ISvcLocator* pSvcLocator );
		virtual ~DiTauBuilder(); 

		virtual StatusCode  initialize();
		virtual StatusCode  execute();
		virtual StatusCode  finalize();

	private: 
		std::string m_diTauContainerName;  // ditau output container name
		std::string m_diTauAuxContainerName;  // ditau output aux container name
		std::string m_seedJetName;  // name for seed jet collection name
		float m_minPt;  // minimal jet seed pt
		float m_maxEta;  // maximal jet seed eta
		float m_Rjet;   // jet radius
		float m_Rsubjet;  // subjet radius
		float m_Rcore;  // core subjet radius
		ToolHandleArray<DiTauToolBase> m_tools;

}; 

#endif //> !DITAUREC_DITAUBUILDER_H
