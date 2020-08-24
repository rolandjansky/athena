/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonAGDDTool_H
#define MuonAGDDTool_H

#include "AGDDControl/AGDDToolBase.h"
#include "GaudiKernel/StatusCode.h"

class IInterface;
class ISvcLocator;
class IToolSvc;

#ifndef SIMULATIONBASE
#include "AmdcAth/AmdcsimrecAthenaSvc.h"
#endif


class MuonAGDDTool: public AGDDToolBase
{
public:
	MuonAGDDTool(const std::string& type, const std::string& name, const IInterface* parent);
	virtual StatusCode construct();
	
	virtual StatusCode initialize();
			
private:
	
	std::vector<std::string> m_structuresToBuild;
	std::vector<std::string> m_structuresFromFlags;
	std::string m_outFileType;
	std::string m_outPREsqlName;
	std::string m_DBFileName;
	std::string m_agdd2GeoSvcName;

	bool m_readAGDD;
	bool m_dumpAGDD;
	bool m_overrideConfiguration;
	
	bool m_buildNSW;

#ifndef SIMULATIONBASE
	ServiceHandle<AmdcsimrecAthenaSvc> p_AmdcsimrecAthenaSvc;
#endif

	void BuildMuonSpectrometerFromFlags();
	bool WritePREsqlFile() const;

};

#endif
