/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDD/NSWAGDDTool.h"

#include "MuonAGDDToolHelper.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDControl/AGDD2GeoModelBuilder.h"
#include "AGDD2GeoSvc/IAGDD2GeoSvc.h"
#include "AGDDModel/AGDDParameterStore.h"
#include "AGDDKernel/AGDDDetector.h"
#include "AGDDKernel/AGDDDetectorStore.h"

#include <fstream>

using namespace MuonGM;

NSWAGDDTool::NSWAGDDTool(const std::string& type, const std::string& name, const IInterface* parent) :
    AGDDToolBase(type,name,parent),
    m_outPREsqlName("")
{
	declareProperty( "ReadAGDD",   		m_readAGDD    = true,           "read description from DB");
	declareProperty( "DumpAGDD",		m_dumpAGDD    = false,          "write out parsed XML");
	declareProperty( "OutputFileACTVERS",	m_outFileActV = 0,		"active version number");
	declareProperty( "OutputFileACTVNAME",	m_outFileActN = "",		"active version string");
	declareProperty( "OutputFileALGVERS",	m_outFileAlgV = 0,		"alignment version number");
	declareProperty( "OutputFileALGVNAME",	m_outFileAlgN = "",		"alignment version string");
	declareProperty( "OutputFilePASVERS",	m_outFilePasV = 0,		"passive structure version number");
	declareProperty( "OutputFilePASVNAME",	m_outFilePasN = "",		"passive structure version string");
	declareProperty( "OutputFileFORMAT",	m_outFileForm = "AGDDXML",	"format of output file");
	declareProperty( "OutputFileType",	m_outFileType = "NSWD", 	"name for database table");
	declareProperty( "OutputFileName",	m_DBFileName = "", "specify name for DB text file");
	declareProperty( "AGDDtoGeoSvcName", m_agdd2GeoSvcName = "AGDDtoGeoSvc", "specify name of AGDDtoGeoSvc");
}

StatusCode NSWAGDDTool::initialize()
{
	ATH_MSG_INFO("this is NSWAGDDTool::initialize()!!!!");

	if( m_xmlFiles.size() == 1 && m_writeDBfile )
	{
		std::size_t found = m_xmlFiles[0].find_last_of("/");
		m_outFileInName = m_xmlFiles[0].substr(found+1);
	}
	else if ( m_writeDBfile ) ATH_MSG_ERROR("writing data base files currently only supported if just a single input XML is given!");

	m_outFileName = "Out.AmdcOracle.AM." + m_outFileType + "temp.data";
	m_outPREsqlName = "Out.AmdcOracle.AM." + m_outFileType + ".PREsql";

    ATH_CHECK(AGDDToolBase::initialize());

	if (m_DBFileName.empty()) {
		m_DBFileName = "Generated_" + m_outFileType + "_pool.txt";
	}
	
	static int iEntries=0;
	
	if (!iEntries) 
	{
		iEntries=1;
		MuonAGDDToolHelper theHelper;
		theHelper.setAGDDtoGeoSvcName(m_agdd2GeoSvcName);
		theHelper.SetNSWComponents();
	}

        ATH_CHECK(construct());
	return StatusCode::SUCCESS;
}

StatusCode NSWAGDDTool::construct() 
{
	ATH_MSG_INFO("this is NSWAGDDTool::construct()!!!!");
	ATH_MSG_INFO(" Name = "<<name());
	
	MuonAGDDToolHelper theHelper;
	theHelper.setAGDDtoGeoSvcName(m_agdd2GeoSvcName);
	if (!m_readAGDD)
	{
		ATH_MSG_INFO(" trying to parse files ");
		m_controller->ParseFiles();
	}
	else
	{
		ATH_MSG_INFO(" trying to parse data base content ");
		std::string AGDDfile = theHelper.GetAGDD(m_dumpAGDD, m_outFileType, m_DBFileName);
		m_controller->ParseString(AGDDfile);
	}
	
	if (m_printSections) 
	{
		ATH_MSG_INFO("\t Printing all sections ");
		m_controller->PrintSections();
	}
	
	m_controller->UseGeoModelDetector("Muon");
	
	m_controller->BuildAll();
	
	// part needed to build the NSW RO geometry
	
	ATH_MSG_INFO("\t Building NSW Readout Geometry ");
	bool testRet=theHelper.BuildMScomponents();
	if (!testRet)
	{
		ATH_MSG_ERROR("something went wrong building the RO geometry!!! ");
		return StatusCode::FAILURE;
	}

	if(m_writeDBfile)
	{
		// build model before writing blob - if Athena crashes the XML is not good and should not become a blob
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

bool NSWAGDDTool::WritePREsqlFile() const
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

	std::ofstream prefile;
	prefile.open (m_outPREsqlName.c_str());
	prefile << "insert into NSWD_data (\n";
	prefile << "NSWD_data_id,\n";
	prefile << "ACTVERS,\n";
	prefile << "ACTVNAME,\n";
	prefile << "ALGVERS,\n";
	prefile << "ALGVNAME,\n";
	prefile << "PASVERS,\n";
	prefile << "PASVNAME,\n";
	prefile << "FORMAT,\n";
	prefile << "FNAME,\n";
	prefile << "LENNSW,\n";
	prefile << "NLINE,\n";
	prefile << "data\n";
	prefile << ") values (XXX_DATA_ID_KOUNTER,\n";
	prefile << m_outFileActV << ",'" << m_outFileActN << "',";
	prefile << m_outFileAlgV << ",'" << m_outFileAlgN << "',";
	prefile << m_outFilePasV << ",'" << m_outFilePasN << "',\n";
	prefile << "'" << m_outFileForm <<"','" << m_outFileInName << "',"<< fileSize-1 << ","<< int( (fileSize + 2) / 4 )<<",\n";
	prefile << "empty_clob()\n";
	prefile << ");\n";
	prefile << "insert into NSWD_data2tag values (XXX_DATA2TAG_KOUNTER,XXX_DATA_ID_KOUNTER);\n";
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
	prefile << "  FROM   NSWD_data\n";
	prefile << "  WHERE  NSWD_data_id =  XXX_DATA_ID_KOUNTER FOR UPDATE;\n";
	prefile << "  offset := DBMS_LOB.GETLENGTH(lobloc)+2;\n";
	prefile << "     req := utl_http.begin_request(\n";
	prefile << "     'WEB_ADDRESS_FOR_TEMP_DATA_FILENSWDtemp.data');\n";
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



