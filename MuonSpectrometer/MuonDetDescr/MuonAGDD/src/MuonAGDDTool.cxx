/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDD/MuonAGDDTool.h"

#include "MuonAGDDToolHelper.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDControl/AGDD2GeoModelBuilder.h"
#include "AGDDKernel/AliasStore.h"

#ifndef SIMULATIONBASE
#include "AmdcAth/AmdcsimrecAthenaSvc.h"
#endif

#include <fstream>

MuonAGDDTool::MuonAGDDTool(const std::string& type, const std::string& name, const IInterface* parent) :
    AGDDToolBase(type,name,parent),
    m_structuresFromFlags(),
    m_outPREsqlName("")
#ifndef SIMULATIONBASE
    ,p_AmdcsimrecAthenaSvc("AmdcsimrecAthenaSvc",name)
#endif
{}

StatusCode MuonAGDDTool::initialize ATLAS_NOT_THREAD_SAFE ()
{
	ATH_CHECK(AGDDToolBase::initialize());
    ATH_MSG_INFO("MuonAGDDTool::initialize");
	m_outFileName = "Out.AmdcOracle.AM." + m_outFileType + "temp.data";
	m_outPREsqlName = "Out.AmdcOracle.AM." + m_outFileType + ".PREsql";

	if (m_DBFileName.empty()) {
		m_DBFileName = "Generated_" + m_outFileType + "_pool.txt";
	}

#ifndef SIMULATIONBASE
	if(m_writeDBfile && !m_xmlFiles.size()) ATH_CHECK(p_AmdcsimrecAthenaSvc.retrieve());
#endif

	if (m_buildNSW) 
	{
		MuonAGDDToolHelper theHelper;
		theHelper.setAGDDtoGeoSvcName(m_agdd2GeoSvcName);
		theHelper.SetNSWComponents();
	}
	
	ATH_CHECK(construct());
	return StatusCode::SUCCESS;
}

// Base class method is also marked not thread-safe.
// Uses unsafe function UseGeoModelDetector
StatusCode MuonAGDDTool::construct  ATLAS_NOT_THREAD_SAFE  () 
{
	ATH_MSG_INFO(name()<<"::construct()");
	MuonAGDDToolHelper theHelper;
	theHelper.setAGDDtoGeoSvcName(m_agdd2GeoSvcName);

        IAGDDtoGeoSvc::LockedController controller = m_svc->getController();
	controller->UseGeoModelDetector("Muon");
	
	if (!m_locked)
	{
		ATH_MSG_INFO(" Reading AGDD2GeoSwitches flags ");
		m_structuresFromFlags=theHelper.ReadAGDDFlags();
		for (const auto &structure: m_structuresFromFlags) {
			ATH_MSG_INFO(" ----> "<<structure);
		}
	}
	
	// reading from a local AGDD xml file
	if (!m_readAGDD) {
		ATH_MSG_INFO(" Parsing local xml file ");
		controller->ParseFiles();
    // reading the AGDD xml blob from the ATLAS geometry database
	} else {
	    ATH_MSG_INFO(" now reading AGDD blob ");

	    std::string AGDDfile=theHelper.GetAGDD(m_dumpAGDD, m_outFileType, m_DBFileName);
#ifndef SIMULATIONBASE
	    if(m_writeDBfile && !m_xmlFiles.size()) AGDDfile = p_AmdcsimrecAthenaSvc->GetAgddString();
#endif
	    if( AGDDfile.empty() ) {
		    ATH_MSG_ERROR("\t-- empty AGDDfile - this cannot be correct " );
		    return StatusCode::FAILURE;
	    }
	    controller->ParseString(AGDDfile);
	}

	if (m_printSections) {	
		ATH_MSG_INFO("\t Printing all sections");
		controller->PrintSections();
	}

    // when reading from a local AGDD xml file and not creating a layout (i.e. running simulation from a local xml file),
    // only build those volumes that are specified at the 'Volumes' property (analogously to the AGDD2GeoSwitches when reading the blob)
    if (!m_readAGDD && !m_writeDBfile) {
        for (const auto &vol:m_volumesToBuild) {
            controller->GetBuilder()->BuildFromVolume(vol);
        }
    } else {
    // when reading the AGDD xml blob, only build the volumes specified via the AGDD2GeoSwitches
	    for (const auto &structure: m_structuresFromFlags) {
            if (!m_buildNSW && structure=="NewSmallWheel") continue;
            controller->GetBuilder()->BuildFromVolume(structure);
        }
    }

	if(m_writeDBfile)
	{
		// build model before writing blob - if Athena crashes the XML is not good and should not become a blob
		((AGDD2GeoModelBuilder*)controller->GetBuilder())->BuildAllVolumes();
		ATH_MSG_INFO("\t-- attempting to write output to "<< m_outFileName );
		if( !m_outFileName.empty() )
		{
			if(!controller->WriteAGDDtoDBFile( m_outFileName ))
			{
				ATH_MSG_ERROR("\t-- something went wrong during writing AGDD file - crashing" );
				return StatusCode::FAILURE;
			}
			else {
				ATH_MSG_INFO("\t-- AGDD successfully dumped to "<< m_outFileName);
			}
			if( !WritePREsqlFile() )
			{
				ATH_MSG_ERROR("\t-- something went wrong during writing PREsql file - crashing" );
				return StatusCode::FAILURE;
			}
			else {
				ATH_MSG_INFO("\t-- AGDD successfully wrote PREsql file "<< m_outPREsqlName);
			}
		}
		else {
			ATH_MSG_ERROR("\t-- no output file name provided - crashing " );
			return StatusCode::FAILURE;
		}
	}

	controller->Clean();

	return StatusCode::SUCCESS;
}

bool MuonAGDDTool::WritePREsqlFile() const
{

	std::ifstream outfile(m_outFileName.value().c_str(), std::ifstream::in | std::ifstream::binary);

	std::vector<std::string> newoutfilelines;
	std::string outfileline;
	while( getline(outfile, outfileline) )
		if( outfileline != "\n" && outfileline != "\r" && outfileline.size() )
		{
			const auto strBegin = outfileline.find_first_not_of(" \t");
			const auto strEnd = outfileline.find_last_not_of(" \t");
			const auto strRange = strEnd - strBegin + 1;
			if (strBegin != std::string::npos) outfileline = outfileline.substr(strBegin, strRange);
			newoutfilelines.push_back(outfileline);
		}
	outfile.close();

	std::ofstream newoutfile(m_outFileName.value().c_str(), std::ofstream::out | std::ofstream::trunc);
	for(auto it = newoutfilelines.begin(); it != newoutfilelines.end(); ++it)
	{
		if(it != newoutfilelines.begin()) newoutfile << "\n";
		newoutfile << *it;
	}
	newoutfile.close();
	outfile.open(m_outFileName.value().c_str(), std::ifstream::in | std::ifstream::binary);

	int fileSize = 0;
	if(outfile.is_open())
	{
		outfile.seekg(0, std::ios::end);
		fileSize = outfile.tellg();
		outfile.close();
	}
	else {
		ATH_MSG_ERROR("\t-- cannot get size of file " << m_outFileName );
		return false;
	}

	std::string TheAmdcName = m_amdcName;
	// The following is only needed for creating AGDD muon (passive material) layouts
    // *IF* the AGDD xml block is still stored in the amdb file (https://twiki.cern.ch/twiki/bin/viewauth/Atlas/AmdbSimrecFiles)
    // Note that this was the case until amdb_simrec.r.08.01, from amdb_simrec.r.08.02 onwards, the AGDD xml is stored independently
    // of the amdb file in MuonSpectrometer/MuonG4/MuonPassiveMaterials.
    // Thus, this retrieval of the AmdcName is only kept for backwards compatibility (in case someone wants to create muon layouts 
    // from amdb files older than amdb_simrec.r.08.02) to be fully consistent with previous table generations.
#ifndef SIMULATIONBASE
    if (!m_xmlFiles.size()) {
        Amdcsimrec* pAmdcsimrec = p_AmdcsimrecAthenaSvc->GetAmdcsimrec();
        TheAmdcName = pAmdcsimrec->AmdcName();
    }
#endif

	std::ofstream prefile;
	prefile.open (m_outPREsqlName.c_str());
	prefile << "insert into AGDD_data (\n";
	prefile << "AGDD_data_id,\n";
	prefile << "VERS,\n";
	prefile << "VNAME,\n";
	prefile << "LENAGDD,\n";
	prefile << "NLINE,\n";
	prefile << "data\n";
	prefile << ") values (XXX_DATA_ID_KOUNTER,\n";
	// see constructor AmdcDbSvcMakerFromAmdc::AmdcDbSvcMakerFromAmdc
	prefile << "     7,'";
	// see method AmdcDbSvcMakerFromAmdc::AGDD (no idea why "-1", but it's needed to be consistent)
	prefile << TheAmdcName.substr(0,4) <<"'," << fileSize-1 << ","<< int( (fileSize + 2) / 4 )<<",\n";
	prefile << "empty_clob()\n";
	prefile << ");\n";
	prefile << "insert into AGDD_data2tag values (XXX_DATA2TAG_KOUNTER,XXX_DATA_ID_KOUNTER);\n";
	prefile << "DECLARE\n";
	prefile << "  lobloc  CLOB;\n";
	prefile << "  req     utl_http.req;\n";
	prefile << "  resp    utl_http.resp;\n";
	prefile << "  text    VARCHAR2(32767);\n";
	prefile << "  amount  INTEGER(10) := 0;\n";
	prefile << "  offset  INTEGER(10) := 0;\n";
	prefile << "  TRUE    BOOLEAN;\n";
	prefile << "BEGIN\n";
	prefile << "  SELECT data INTO lobloc\n";
	prefile << "  FROM   AGDD_data\n";
	prefile << "  WHERE  AGDD_data_id =  XXX_DATA_ID_KOUNTER FOR UPDATE;\n";
	prefile << "  offset := DBMS_LOB.GETLENGTH(lobloc)+2;\n";
	prefile << "     req := utl_http.begin_request(\n";
	prefile << "     'WEB_ADDRESS_FOR_TEMP_DATA_FILEAGDDtemp.data');\n";
	prefile << "  resp := utl_http.get_response(req);\n";
	prefile << "  LOOP\n";
	prefile << "     text := ' ';\n";
	prefile << "     UTL_HTTP.READ_TEXT(resp, text, NULL);\n";
	prefile << "     /* DBMS_OUTPUT.PUT_LINE(text); */\n";
	prefile << "     amount := length(text);\n";
	prefile << "     DBMS_LOB.WRITEAPPEND(lobloc,amount,text);\n";
	prefile << "  END LOOP;\n";
	prefile << "    utl_http.end_response(resp);\n";
	prefile << "  EXCEPTION\n";
	prefile << "    WHEN utl_http.end_of_body\n";
	prefile << "    THEN utl_http.end_response(resp);\n";
	prefile << "END;\n";
	prefile << "/\n";
	prefile.close();

	return true;

}
