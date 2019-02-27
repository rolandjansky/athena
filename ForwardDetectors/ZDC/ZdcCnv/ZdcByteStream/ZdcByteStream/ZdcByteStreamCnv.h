/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCBYTESTREAMCNV_H
#define ZDCBYTESTREAMCNV_H
/*
 * ZdcByteStreamCnv.h
 *
 *  Created on: May 7, 2009
 *      Author: leite
 *
 * This class is based on the code provided in the LAr ByteStream Converter
 * /offline/LArCalorimeter/LArCnv/LArByteStream/LArByteStream/ZdcRawChannelContByteStreamCnv.h
 *
 * It will:
 *     take an event fragment
 *     verify if it's a ZDC fragment (really)
 *     extract the ROD header
 *     put the PPM data into a collection
 *
 */

#include <stdint.h>
#include <map>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class DataObject;
class IByteStreamEventAccess;
class IOpaqueAddress;
class IROBDataProviderSvc;
class ISvcLocator;
class StatusCode;
class ZdcByteStreamTool;
class ZdcByteStreamReadV1V2Tool;

// Abstract factory to create the converter
template <typename > class CnvFactory;

/**
 * The converter class has the methods to create the collection objects from a file
 * (RDO to ByteStream) and ByteStream from RDO.
 */
class ZdcByteStreamCnv: public Converter
{
public:
	ZdcByteStreamCnv(ISvcLocator* svcloc);

	virtual ~ZdcByteStreamCnv();

	virtual StatusCode initialize() override;
	virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;
	//virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;

	/// Storage type and class ID
    virtual long repSvcType() const override { return i_repSvcType(); }

    static long storageType();
	static const CLID& classID();

private:
	/// Converter name
	std::string m_name;

	/// Tool that does the actual work
	//ToolHandle<ZdcByteStreamTool> m_tool; // old style
	ToolHandle<ZdcByteStreamReadV1V2Tool> m_tool; // new style

	/// Service for READING bytestream
	ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

	/// Service for WRITING bytestream
	ServiceHandle<IByteStreamEventAccess> m_ByteStreamEventAccess;

	/// Message log
	mutable MsgStream m_log;
	bool m_debug;
};

#endif /* ZDCBYTESTREAMCNV_H */
