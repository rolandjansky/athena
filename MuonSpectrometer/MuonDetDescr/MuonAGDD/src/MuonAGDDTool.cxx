/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

StatusCode MuonAGDDTool::initialize()
{
	ATH_CHECK(AGDDToolBase::initialize());
    ATH_MSG_INFO("MuonAGDDTool::initialize");
	m_outFileName = "Out.AmdcOracle.AM." + m_outFileType + "temp.data";
	m_outPREsqlName = "Out.AmdcOracle.AM." + m_outFileType + ".PREsql";

	if (m_DBFileName.empty()) {
		m_DBFileName = "Generated_" + m_outFileType + "_pool.txt";
	}

	// please see more details on regarding the dependency on AMDB on ATLASSIM-3636
	// and the CMakeLists.txt . the NSWAGDDTool avoids the dependency already
#ifndef SIMULATIONBASE
	if(m_writeDBfile && !m_xmlFiles.size()) CHECK( p_AmdcsimrecAthenaSvc.retrieve() );
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

StatusCode MuonAGDDTool::construct() 
{
	ATH_MSG_INFO(name()<<"::construct()");
	MuonAGDDToolHelper theHelper;
	theHelper.setAGDDtoGeoSvcName(m_agdd2GeoSvcName);

	m_controller->UseGeoModelDetector("Muon");
	
	if (!m_locked)
	{
		ATH_MSG_INFO(" Reading AGDD2GeoSwitches flags ");
		m_structuresFromFlags=theHelper.ReadAGDDFlags();
		for (unsigned int i=0;i<m_structuresFromFlags.size();i++)
			ATH_MSG_INFO(" ----> "<<m_structuresFromFlags[i]);
	}
	
	if (!m_readAGDD)
	{
		ATH_MSG_INFO(" trying to parse files ");
		m_controller->ParseFiles();
		if(!m_writeDBfile) return StatusCode::SUCCESS;
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
	    m_controller->ParseString(AGDDfile);
	}

	if (m_printSections)
	{	
		ATH_MSG_INFO("\t Printing all sections");
		m_controller->PrintSections();
	}
	
	ATH_MSG_INFO(" now dumping the flags ");
	for (unsigned int i =0;i<m_structuresFromFlags.size();i++)
	{
		ATH_MSG_INFO("\t\t----- "<<m_structuresFromFlags[i]<<" "<<ALIAS(m_structuresFromFlags[i]));
		if (!m_buildNSW && m_structuresFromFlags[i]=="NewSmallWheel") continue;
		m_controller->GetBuilder()->BuildFromVolume(m_structuresFromFlags[i]);
	}

	if(m_writeDBfile)
	{
		// build model before writing blob - if Athena crashes the XML is not good and should not become a blob
		((AGDD2GeoModelBuilder*)m_controller->GetBuilder())->BuildAllVolumes();
		ATH_MSG_INFO("\t-- attempting to write output to "<< m_outFileName );
		if( !m_outFileName.empty() )
		{
			if(!m_controller->WriteAGDDtoDBFile( m_outFileName ))
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

	m_controller->Clean();

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
	// in principle this information could also be accessed differently and the
	// dependency on AMDB could be avoided. for the moment it's kept to be fully
	// consistent with previous table generations
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
