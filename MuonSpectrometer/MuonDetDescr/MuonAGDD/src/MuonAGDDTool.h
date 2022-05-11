/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
        virtual StatusCode construct ATLAS_NOT_THREAD_SAFE () override;
	virtual StatusCode initialize ATLAS_NOT_THREAD_SAFE () override;
			
private:
    Gaudi::Property<bool> m_buildNSW{this,"BuildNSW",true};
    Gaudi::Property<bool> m_overrideConfiguration{this,"OverrideConfiguration",false};

    Gaudi::Property<std::vector<std::string>> m_structuresToBuild{this,"Structures",{}};

    Gaudi::Property<std::string> m_outFileType{this,"OutputFileType","AGDD","Name for database table"};
    Gaudi::Property<std::string> m_amdcName{this,"AmdcName","R.08","specify amdc name to be put into AGDD table (is overwritten by amdb layout name in case amdc blob is read)"};

	std::vector<std::string> m_structuresFromFlags;
	std::string m_outPREsqlName;

    // The following ServiceHandle<AmdcsimrecAthenaSvc> is only needed for creating AGDD muon layouts (muon passive material layouts)
    // *IF* the AGDD xml block is still stored in the amdb file (https://twiki.cern.ch/twiki/bin/viewauth/Atlas/AmdbSimrecFiles)
    // Note that this was the case until amdb_simrec.r.08.01, from amdb_simrec.r.08.02 onwards, the AGDD xml is stored independently
    // of the amdb file in MuonSpectrometer/MuonG4/MuonPassiveMaterials.
    // Thus, this ServiceHandle is only kept for backwards compatibility (in case someone wants to create muon layouts from amdb files
    // older than amdb_simrec.r.08.02)
#ifndef SIMULATIONBASE
	ServiceHandle<AmdcsimrecAthenaSvc> p_AmdcsimrecAthenaSvc;
#endif

	void BuildMuonSpectrometerFromFlags();
	bool WritePREsqlFile() const;

};

#endif
