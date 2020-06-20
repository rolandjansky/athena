/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWAGDDTool_H
#define NSWAGDDTool_H

#include "AGDDControl/AGDDToolBase.h"
#include "GaudiKernel/StatusCode.h"

namespace MuonGM
{
	class MuonDetectorManager;
}


class NSWAGDDTool: public AGDDToolBase
{
public:
	NSWAGDDTool(const std::string& type, const std::string& name, const IInterface* parent);
	virtual StatusCode construct();
	
	virtual StatusCode initialize();

private:
	
	int m_outFileActV;
	std::string m_outFileActN;
	int m_outFileAlgV;
	std::string m_outFileAlgN;
	int m_outFilePasV;
	std::string m_outFilePasN;
	std::string m_outFileForm;
	std::string m_outFileInName;

	std::string m_outFileType;
	std::string m_outPREsqlName;
	std::string m_DBFileName;
	std::string m_agdd2GeoSvcName;

	bool m_readAGDD;
	bool m_dumpAGDD;

	bool WritePREsqlFile() const;
};

#endif
