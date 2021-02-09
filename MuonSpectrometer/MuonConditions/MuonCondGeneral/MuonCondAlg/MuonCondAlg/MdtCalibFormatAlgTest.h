/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MdtCalibFormatAlgTest_H
#define MdtCalibFormatAlgTest_H 

//STL
#include <stdio.h>
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include "zlib.h"
#include "vector"
#include <boost/date_time/posix_time/posix_time.hpp>

//Gaudi
#include "CoralBase/Blob.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ICondSvc.h"

//Athena
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVInfiniteRange.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandleKey.h"


//Forward declarations
class ISvcLocator;
class StatusCode;

namespace coral	{
  class Blob;
}


class MdtCalibFormatAlgTest : public AthReentrantAlgorithm {

public:
	MdtCalibFormatAlgTest(const std::string &name,ISvcLocator *pSvcLocator) ;
	virtual ~MdtCalibFormatAlgTest() override;
	
	virtual StatusCode initialize() override;
	virtual StatusCode execute(const EventContext&) const override;
	virtual StatusCode finalize() override;
   
private:
	StatusCode retrieve(const EventContext&, std::string, std::string, std::chrono::duration<double>&) const;
	StatusCode processBlob(std::string, std::string, std::string) const;

	Bytef*  uncompressInMyBuffer(const coral::Blob &blob) const;
	StatusCode extractString(std::string &input, std::string &output, std::string separator) const;

	std::string timestamp() const; 

	SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_RT_old{this, "ReadKey_RT_old", "/MDT/RTBLOB", "Key to old-style /MDT/RTBLOB folder"};
	SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_RT_new{this, "ReadKey_RT_new", "/MDT/RTBLOB", "Key to new-style /MDT/RTBLOB folder"};
	SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_T0_old{this, "ReadKey_T0_old", "/MDT/RTBLOB", "Key to old-style /MDT/T0BLOB folder"};
	SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_T0_new{this, "ReadKey_T0_new", "/MDT/RTBLOB", "Key to new-style /MDT/T0BLOB folder"};

	//std::map<std::string, std::map<std::string, std::chrono::duration<double> > > m_retrieving;
	//std::chrono::duration<double> m_retrieving_RT_old;
	//std::chrono::duration<double> m_retrieving_RT_new;
	//std::chrono::duration<double> m_retrieving_T0_old;
	//std::chrono::duration<double> m_retrieving_T0_new;
	//std::chrono::duration<double> m_retrieving   ;
	//std::chrono::duration<double> m_uncompressing;
	//std::chrono::duration<double> m_unwrapping   ;

}; //end of class

#endif
