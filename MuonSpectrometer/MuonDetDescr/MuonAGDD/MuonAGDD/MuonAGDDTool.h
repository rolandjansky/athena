/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonAGDDTool_H
#define MuonAGDDTool_H

#include "AGDDControl/AGDDToolBase.h"

#ifndef SIMULATIONBASE
#include "AmdcAth/AmdcsimrecAthenaSvc.h"
#endif

class MuonAGDDTool: public AGDDToolBase
{
public:
	MuonAGDDTool(const std::string& type, const std::string& name, const IInterface* parent);
	~MuonAGDDTool()=default;
	virtual StatusCode construct() override;
	virtual StatusCode initialize() override;
			
private:
    Gaudi::Property<bool> m_buildNSW{this,"BuildNSW",true};
    Gaudi::Property<bool> m_overrideConfiguration{this,"OverrideConfiguration",false};

    Gaudi::Property<std::vector<std::string>> m_structuresToBuild{this,"Structures",{}};

    Gaudi::Property<std::string> m_outFileType{this,"OutputFileType","AGDD","Name for database table"};
    Gaudi::Property<std::string> m_amdcName{this,"AmdcName","R.08","specify amdc name to be put into AGDD table (is overwritten by amdb layout name in case amdc blob is read)"};

	std::vector<std::string> m_structuresFromFlags;
	std::string m_outPREsqlName;

#ifndef SIMULATIONBASE
	ServiceHandle<AmdcsimrecAthenaSvc> p_AmdcsimrecAthenaSvc;
#endif

	void BuildMuonSpectrometerFromFlags();
	bool WritePREsqlFile() const;

};

#endif
