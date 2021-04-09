/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcByteZtreamTool.h
 *
 *  Created on: May 27, 2009
 *      Author: leite
 *              based on the L1Calo ByteStream Tool
 *
 */

#ifndef ZDCBYTESTREAMTOOL_H
#define ZDCBYTESTREAMTOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "AthContainers/DataVector.h"
#include "eformat/SourceIdentifier.h"
#include "GaudiKernel/ToolHandle.h"

class ZdcID;

class IInterface;
class InterfaceID;
class StatusCode;

template<class T> class FullEventAssembler;

//Use ZdcDigits onde it's fixed
//class ZdcRdo;
//class ZdcRdoCollection;
class ZdcDigits;
class ZdcDigitsCollection;

class ZdcSrcIdMap;
class ZdcPpmSubBlock;

/** Tool to perform ROB fragments to ZdcDigits and from ZdcDigits
 *  to raw data conversions.
 *
 *  Based on ROD Tool from Level1 from  Peter Faulkner
 */

class ZdcByteStreamTool: public AthAlgTool
{

public:
	ZdcByteStreamTool(const std::string& type,
						  const std::string& name,
						  const IInterface* parent);

	/// AlgTool InterfaceID
	static const InterfaceID& interfaceID();

	virtual StatusCode initialize() override;
	virtual StatusCode finalize() override;

	/// Convert ROB fragments to ZdcCollection
	StatusCode convert(	const IROBDataProviderSvc::VROBFRAG& robFrags,
							ZdcDigitsCollection* ttCollection);

	/// Convert ZdcCollection to ROB fragments
	StatusCode convert(const ZdcDigitsCollection* ttCollection,
	                       RawEventWrite* re);

	/// Return reference to vector with all possible Source Identifiers
	const std::vector<uint32_t>& sourceIDs();

private:
//	typedef DataVector<ZdcDigits> ZdcDigitsCollection;
	typedef std::map<unsigned int, ZdcDigits*> ZdcDigitsMap;
	typedef std::map<unsigned int, const ZdcDigits*> ZdcDigitsMapConst;
	typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
	typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType RODPointer;

	/// Add compression stats to totals
	void addCompStats(const std::vector<uint32_t>& stats);
	/// Print compression stats
	void printCompStats() const;
	/// Print a vector
	void printVec(const std::vector<int>& vec) const;

	/// Sub_block header version
	//int m_version;
	/// Data compression format
	int m_dataFormat;
	/// Compression version
	//int m_compVers;
	/// Compression statistics print flag
	int m_printCompStats;
	/// Number of channels per module (may not all be used)
	int m_channels;
	/// Number of crates
	int m_crates;
	/// Number of modules per crate (may not all exist)
	//int m_modules;
	/// Number of slinks per crate when writing out bytestream
	int m_slinks;
	/// Default number of LUT slices in simulation
	int m_dfltSlicesLut;
	/// Default number of FADC slices in simulation
	int m_dfltSlicesFadc;
	/// Force number of LUT slices in bytestream
	int m_forceSlicesLut;
	/// Force number of FADC slices in bytestream
	int m_forceSlicesFadc;
	/// Pedestal value
	int m_pedestal;
	/// FADC baseline lower bound
	int m_fadcBaseline;
	/// FADC threshold for super-compressed format
	int m_fadcThreshold;
	/// Zero suppression on input
	int m_zeroSuppress;
	/// ROB source IDs
	std::vector<uint32_t> m_sourceIDs;
	/// Sub-detector type
	eformat::SubDetector m_subDetector;
	/// Source ID converter
	ZdcSrcIdMap* m_srcIdMap;
	/// Trigger tower key provider
	ZdcPpmSubBlock* m_errorBlock;
	/// Vector for current PPM sub-blocks
	DataVector<ZdcPpmSubBlock> m_ppmBlocks;
	/// Vector for compression statistics
	std::vector<uint32_t> m_compStats;
	/// ROD Status words
	std::vector<uint32_t>* m_rodStatus;
	/// ROD status map
	std::map<uint32_t, std::vector<uint32_t>*> m_rodStatusMap;

	const ZdcID* m_zdcID;

};

#endif /* ZDCBYTESTREAMTOOL_H*/
