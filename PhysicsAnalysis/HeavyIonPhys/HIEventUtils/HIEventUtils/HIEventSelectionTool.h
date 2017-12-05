/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HIEVENTSELECTIONTOOL_H__
#define __HIEVENTSELECTIONTOOL_H__

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "HIEventUtils/IHIEventSelectionTool.h"
#include "HIEventUtils/IHIVertexSelectionTool.h"
#include "HIEventUtils/IHIPileupTool.h"
//#include "HIEventUtils/HIPileupTool.h"
//#include "HIPileupTool/IHIPileupTool.h"
//#include "HIPileupTool/HIPileupTool.h"

//#include "xAODForward/MBTSModuleContainer.h"
//#include "xAODForward/ForwardEventInfoContainer.h"

namespace HI
{
  
  class HIEventSelectionTool :
  public virtual HI::IHIEventSelectionTool, public asg::AsgTool
      {
	
	ASG_TOOL_CLASS( HIEventSelectionTool, IHIEventSelectionTool )
	  
	  public:
	
	static std::string MBTS_CNAME;
	static std::string FWD_CNAME;
	static std::string VERTEX_CNAME;
        static std::string SHAPE_CNAME;
        static std::string ZDC_CNAME;
	
	HIEventSelectionTool(const std::string& name = "HIEventSelection");
	virtual ~HIEventSelectionTool();
	const std::string m_name;
	mutable Root::TAccept m_accept; //!
	virtual StatusCode initialize();
	//virtual StatusCode finalize() {return StatusCode::SUCCESS;}
	virtual StatusCode finalize();
        virtual const Root::TAccept& getTAccept() const;
	virtual const Root::TAccept& accept() const { return getTAccept(); }
	
	bool m_init; 
	
	// MBTS external parameters
	bool m_checkTime = false;
	float m_timeCut = -1;
	// MBTS member functions 
	void checkMBTSTime() const;
	bool isGoodMBTSTime() const;
	
	///////////
	// Vertex external parameters
	ToolHandle<HI::IHIVertexSelectionTool> m_vertexSelectionTool;
        ToolHandle<HI::IHIPileupTool> m_hiPileupTool;
	float m_vtx_min_sumPt = -1;
	int m_vtx_min_nTrk = -1;
	bool m_exclusive_mode = false;
	bool m_reject_pileup = true;
	std::string m_track_cutlevel = "NoCut";
	
	// Vertex internal variables, including helper tools
	
	// Vertex member functions
	void checkVertex() const;
	unsigned int getNPrimaryVertices() const;
	
	std::string dumpSelection() const;
      };
  
}
#endif
