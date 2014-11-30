/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SRC_INDETRAWDATAFAKECREATOR_H
# define SRC_INDETRAWDATAFAKECREATOR_H

/**
 * @file InDetRawDataFakeCreator.h
 *
 * @brief Class which encapsulates the creation of an
 * InDetRawDataCollection
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: InDetRawDataFakeCreator.h,v 1.4 2005-11-01 18:43:27 schaffer Exp $
 *
 */

/**
 * @class InDetRawDataFakeCreator
 *
 * @brief Class which encapsulates the creation of an
 * InDetRawDataCollection
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "InDetRawData/PixelRDO_Collection.h"
#include "InDetRawData/SCT_RDO_Collection.h"
#include "InDetRawData/TRT_RDO_Collection.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "GaudiKernel/MsgStream.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class InDetRawDataFakeCreator
{
public:

    /// Default constructor
    InDetRawDataFakeCreator();

    /// Create PixelRawDataCollection
    const InDetRawDataCollection< PixelRDORawData >* 
    createPixelRawDataColl(IdentifierHash hashId, 
			    const PixelID* pixelId,
			    MsgStream& log);

    /// Create SCT_RawDataCollection
    const InDetRawDataCollection< SCT_RDORawData >* 
    createSCT_RawDataColl(IdentifierHash hashId, 
			  const SCT_ID* sctId,
			  MsgStream& log);

    /// Create TRT_RawDataCollection
    const InDetRawDataCollection< TRT_RDORawData >* 
    createTRT_RawDataColl(IdentifierHash hashId, 
			  const TRT_ID* trtId,
			  MsgStream& log);
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline InDetRawDataFakeCreator::InDetRawDataFakeCreator()
{
}


inline const InDetRawDataCollection< PixelRDORawData >* 
InDetRawDataFakeCreator::createPixelRawDataColl(IdentifierHash hashId, 
						 const PixelID* pixelId,
						 MsgStream& log)
{

    // create a new pixel RDO collection
    InDetRawDataCollection<PixelRDORawData>* rdocoll = 
	new InDetRawDataCollection< PixelRDORawData >(hashId);

    IdContext cntx(pixelId->wafer_context());

    // need the DE identifier
    Identifier waferId = pixelId->wafer_id (hashId);
    rdocoll->setIdentifier(waferId);

    log << MSG::VERBOSE << "hash, id1, id2: " 
	<< MSG::hex << (unsigned int)hashId << MSG::dec << " "
	<< MSG::hex << waferId << MSG::dec << " "
	<< MSG::hex << pixelId->wafer_id(hashId) << MSG::dec << " "
	<< pixelId->show_to_string(waferId) << " "
	<< pixelId->show_to_string(pixelId->wafer_id(hashId)) << " " 
	<< endreq;

    int deltaPhi = pixelId->phi_index_max(waferId)/6;
    int deltaEta = pixelId->eta_index_max(waferId)/6;
    log << MSG::VERBOSE << "phi max, eta max, dphi, deta: " 
	<< pixelId->phi_index_max(waferId) << " " 
	<< pixelId->eta_index_max(waferId) << " "
	<< deltaPhi << " " 
	<< deltaEta << " " 
	<< endreq;

    int phi = deltaPhi;
    // loop on 16 digits
    for (int j = 0; j < 4; ++j, phi += deltaPhi) {
	int eta = deltaEta;
	for (int k = 0; k < 4; ++k, eta += deltaEta) {
	
	    Identifier id_readout  = pixelId->pixel_id(waferId, phi, eta);
	    const unsigned int mo  = (1 & 0x1F) << 27;
	    const unsigned int fe2 =(1 & 0xF) << 21;
	    const unsigned int row2=(1  & 0xFF) << 13;
	    const unsigned int col2=(1 & 0x1F) << 8;
	    const unsigned int tot =(5 & 0xFF);
	    unsigned int pixelWord = (mo|fe2|row2|col2|tot);

	    Pixel1RawData *rdo= new Pixel1RawData(id_readout, pixelWord);
	    if (rdo) {
		rdocoll->push_back(rdo);

	    } else {
		log << MSG::ERROR << "Could not create Pixel1RawData object !" 
		    << endreq;
	    }
	    // Print out contents of RDO
	    log << MSG::VERBOSE << "Created RDO: " 
		<< pixelId->show_to_string(rdo->identify()) << " " 
		<< MSG::hex << rdo->getWord() << endreq;
	}
    }

    return (rdocoll);
}

inline const InDetRawDataCollection< SCT_RDORawData >* 
InDetRawDataFakeCreator::createSCT_RawDataColl(IdentifierHash hashId, 
					       const SCT_ID* sctId,
					       MsgStream& log)
{
    // create a new sct RDO collection
    InDetRawDataCollection<SCT_RDORawData>* rdocoll = 
	new InDetRawDataCollection< SCT_RDORawData >(hashId);

    IdContext cntx(sctId->wafer_context());

    // need the DE identifier
    Identifier waferId = sctId->wafer_id (hashId);
    rdocoll->setIdentifier(waferId);

    int deltaStrip = sctId->strip_max(waferId)/6;
    log << MSG::VERBOSE << "strip max, dstrip: " 
	<< sctId->strip_max(waferId) << " " 
	<< deltaStrip << " " 
	<< endreq;

    int strip = deltaStrip;
    // loop on 4 digits
    for (int j = 0; j < 4; ++j, strip += deltaStrip) {
	
	Identifier id_readout = sctId->strip_id(waferId, strip);
	const unsigned int str =(strip) << 16;
	const unsigned int group = 4;
	unsigned int word= (group|str);

	SCT1_RawData *rdo= new SCT1_RawData(id_readout, word);
	if (rdo) {
	    rdocoll->push_back(rdo);

	} else {
	    log << MSG::ERROR << "Could not create SCT1_RawData object !" 
		<< endreq;
	}
	// Print out contents of RDO
	log << MSG::VERBOSE << "Created RDO: " 
	    << sctId->show_to_string(rdo->identify()) << " " 
	    << MSG::hex << rdo->getWord() << endreq;
    }

    return (rdocoll);
}


inline const InDetRawDataCollection< TRT_RDORawData >* 
InDetRawDataFakeCreator::createTRT_RawDataColl(IdentifierHash hashId, 
						 const TRT_ID* trtId,
						 MsgStream& log)
{
    // create a new trt RDO collection
    InDetRawDataCollection<TRT_RDORawData>* rdocoll = 
	new InDetRawDataCollection< TRT_RDORawData >(hashId);

    IdContext cntx(trtId->straw_layer_context());

    // need the DE identifier
    Identifier strLayId = trtId->layer_id (hashId);
    rdocoll->setIdentifier(strLayId);

    int deltaStraw = trtId->straw_max(strLayId)/6;
    log << MSG::VERBOSE << "straw max, dstraw: " 
	<< trtId->straw_max(strLayId) << " " 
	<< deltaStraw << " " 
	<< endreq;

    int straw = deltaStraw;
    // loop on 16 digits
    for (int j = 0; j < 4; ++j, straw += deltaStraw) {

	Identifier id_readout = trtId->straw_id(strLayId, straw);
	unsigned int trtWord= (0xFFFFFFFF);

	TRT_LoLumRawData *rdo= new TRT_LoLumRawData(id_readout, trtWord);
	if (rdo) {
	    rdocoll->push_back(rdo);

	} else {
	    log << MSG::ERROR << "Could not create TRT_LoLumRawData object !" 
		<< endreq;
	}
	// Print out contents of RDO
	log << MSG::VERBOSE << "Created RDO: " 
	    << trtId->show_to_string(rdo->identify()) << " " 
	    << MSG::hex << rdo->getWord() << endreq;
    }
    return (rdocoll);
}



#endif // SRC_INDETRAWDATAFAKECREATOR_H

