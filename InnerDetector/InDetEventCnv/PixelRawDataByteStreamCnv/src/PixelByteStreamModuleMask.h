/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELRAWDATABYTESTREAMCNV_PIXELBYTESTREAMMODULEMASK_H
#define PIXELRAWDATABYTESTREAMCNV_PIXELBYTESTREAMMODULEMASK_H

//#include <cstdint>
// Pixel module header word
static const uint32_t PRB_BCIDmask          = 0xFF;   // BCID header word mask
static const uint32_t PRB_BCIDskip          = 0;   // BCID header word skip
static const uint32_t PRB_L1IDmask          = 0xF;   // L1ID header word mask
static const uint32_t PRB_L1IDskip          = 8;   // L1ID header word skip
static const uint32_t PRB_L1IDSKIPmask      = 0xF;   // L1ID skip header word mask
static const uint32_t PRB_L1IDSKIPskip      = 12;   // L1ID skip header word skip
static const uint32_t PRB_MODULEmask        = 0x7F;   // module link header word mask
static const uint32_t PRB_MODULEskip        = 16;   // module link header word skip
//static const uint32_t PRB_MODULEerrormask   = 0x4; // module link header boundary check mask
static const uint32_t PRB_HEADERERRORSmask  = 0xF;   // header errors header word mask
static const uint32_t PRB_HEADERERRORSskip  = 25;   // header errors header word skip
static const uint32_t PRB_HEADERBITFLIPmask = 0x3;
static const uint32_t PRB_HEADERBITFLIPskip = 23;

// IBL module header word
static const uint32_t PRB_BCIDskip_IBL       = 0; // IBL BCID header word skip => B
static const uint32_t PRB_BCIDmask_IBL       = 0x3FF; // IBL BCID header word mask
static const uint32_t PRB_L1IDskip_IBL       = 10; // IBL L1ID header word skip => L
static const uint32_t PRB_L1IDmask_IBL       = 0x1FFF; // IBL L1ID header word mask
static const uint32_t PRB_FeI4BFLAGskip_IBL = 23; // IBL FeI4B flag bit skip => F
static const uint32_t PRB_FeI4BFLAGmask_IBL = 0x1; // IBL FeI4 flag bit mask
static const uint32_t PRB_ZERO_skip_IBL      = 16; // skip to go to the 0's in the header of the IBL // OBSOLETE! REMOVE IT
static const uint32_t PRB_ZERO_mask_IBL      = 0xFF; // mask needed to check if the 0's are in the right position // OBSOLETE! REMOVE IT (or modify it)
static const uint32_t PRB_MODULEskip_IBL     = 24; // IBL module link header word skip => n
static const uint32_t PRB_MODULEmask_IBL     = 0x1F; // IBL module link header word mask
 

// Pixel module hit word
static const uint32_t PRB_ROWmask        = 0xFF;   // row hit word mask
static const uint32_t PRB_ROWskip        = 0;   // row hit word skip
static const uint32_t PRB_COLUMNmask     = 0x1F;   // column hit word mask
static const uint32_t PRB_COLUMNskip     = 8;   // column hit word skip
static const uint32_t PRB_TOTmask        = 0xFF;  // TOT hit word mask
static const uint32_t PRB_TOTskip        = 16;   // TOT hit word skip
static const uint32_t PRB_FEmask         = 0xF;   // FE hit word mask
static const uint32_t PRB_FEskip         = 24;   // FE hit word skip
static const uint32_t PRB_HITBITFLIPmask = 0x8007;
static const uint32_t PRB_HITBITFLIPskip = 13;

// IBL module (condensed) hit word 
static const uint32_t PRB_FIRSTHITCONDENSEDWORD  = 0xA0000000; // mask for the first hit condensed
//static const uint32_t PRB_HITDATAMASK            = 0xE0000000; // probe first and last hit condensed words
static const uint32_t PRB_MIDDLEHITCONDENSEDWORD = 0x80000000; // probe second and third hit condensed words
static const uint32_t PRB_LASTHITCONDENSEDWORD  = 0xE0000000; // probe to last of 4 hit condensed words (IBL)

static const uint32_t nHits = 5; // number of consecutive IBL hits that are condensed into 4 ROD words
static const int nCondensedWords = 4; // number of condensed hit words sent to the ROD
static const uint32_t mask3      = 0x7; // mask to select first 3 bits
//static const uint32_t mask4      = 0xF; // mask to select first 4 bits
static const uint32_t mask5      = 0x1F; //  mask to select first 5 bits
static const uint32_t mask12     = 0x00000FFF; // mask to select 12 bits
static const uint32_t mask19     = 0x0007FFFF; // mask to select 19 bits
static const uint32_t mask24     = 0x00FFFFFF; // mask to select 24 bits = bits Row+bits Column + bits TOT; 

static const uint32_t skip3   = 3; // mask to skip first 3 bits
static const uint32_t skip4   = 4; // mask to skip first 4 bits
static const uint32_t skip5   = 5; // mask to skip first 5 bits
static const uint32_t skipRow = 9; // length of Row information in IBL
static const uint32_t skipCol = 7; // length of Column information in IBL
static const uint32_t skipTOT = 8; // length of TOT information in IBL

//static const uint32_t cond_IBL_first_mask  = 0x1FFFFFFF; //PRB_DATAWORD_SHORT_IBL_first    = 0xA0000000; // condensed first hit word - for IBL
//static const uint32_t cond_IBL_middle_mask = 0x7FFFFFFF; //const uint32_t PRB_DATAWORD_SHORT_IBL_middle   = 0x10000000; // condensed middle hit words - for IBL
//static const uint32_t cond_IBL_last_mask   = 0x1FFFFFFF; //const uint32_t PRB_DATAWORD_SHORT_IBL_last     = 0xE0000000; // condensed last hit word - for IBL
//static const uint32_t PRB_HIT_IBL               = 0x601F0000; // IBL not-condensed hit word mask
//static const uint32_t PRB_HIT_IBL_EMPTY         = 0x00000000; // IBL not condensed hit word probe

//static const uint32_t PRB_HIT_IBL_MASK          = 0xE0000000; // mask to recognise IBL not-condensed hit word
//static const uint32_t PRB_HIT_NOTCONDENSED      = 0x80000000; // 
static const uint32_t PRB_HIT_PIXEL_MASK        = 0xF000E000; // mask to recognise Pixel hit word

// IBL module (not-condensed) hit word
static const uint32_t PRB_ROWskip_IBL        = 0; // row not-condensed hit word skip
static const uint32_t PRB_ROWmask_IBL        = 0x1FF; // row not-condensed hit word mask
static const uint32_t PRB_COLUMNskip_IBL     = 9; // column not-condensed hit word skip => need to skip the Row bits
static const uint32_t PRB_COLUMNmask_IBL     = 0x7F; // column not-condensed hit word mask
static const uint32_t PRB_TOTskip_IBL        = 16; // TOT not-condensed hit word skip => need to skip the Row and the Column
static const uint32_t PRB_TOTmask_IBL        = 0xFF; // TOT non-condensed hit word mask
static const uint32_t PRB_LINKNUMHITskip_IBL = 24;
static const uint32_t PRB_LINKNUMHITmask_IBL = 0x1F; // link number for the IBl, containing the SLink (2 MSB) and the FE w.r.t. the SLink (3 LSB)
//static const uint32_t PRB_LINKNUMHITmask_IBL = 0x7; // link number for the IBL (= number of the FE w.r.t. the SLink)

// Pixel module trailer word
static const uint32_t PRB_TRAILERERRORSmask  = 0x7;   // trailer errors trailer word mask
static const uint32_t PRB_TRAILERERRORSskip  = 26;   // trailer errors trailer word skip
static const uint32_t PRB_TRAILERBITFLIPmask = 0x3FFFFFF;
static const uint32_t PRB_TRAILERBITFLIPskip = 0;

// IBL module trailer word
static const uint32_t PRB_SKIPPEDTRIGGERTRAILERskip_IBL = 5; // IBL L1IDskip in trailer word skip
static const uint32_t PRB_SKIPPEDTRIGGERTRAILERmask_IBL = 0x3FF; // IBL L1IDskip in trailer word mask
static const uint32_t PRB_TRAILERERRORSskip_IBL         = 15; // IBL skipped trigger counter skip in the trailer word. In the data format, they're called z,h and v.
static const uint32_t PRB_TRAILERERRORSmask_IBL         = 0x1FF; // IBL skipped trigger counter mask in the trailer word
static const uint32_t PRB_LINKNUMTRAILERskip_IBL        = 24;  // IBL link number (= number of the FE chip linked to the S-Link that is sending the stream, and corresponding SLink) in the trailer word
static const uint32_t PRB_LINKNUMTRAILERmask_IBL        = 0x1F; // IBL link number (= number of the FE chip linked to the S-Link that is sending the stream) in the trailer word
//static const uint32_t PRB_LINKNUMTRAILERmask_IBL        = 0x7; // IBL link number (= number of the FE chip linked to the S-Link that is sending the stream) in the trailer word
static const uint32_t PRB_BITFLIPSTRAILERskip_IBL       = 0; // IBL bitflips skip in the trailer word
static const uint32_t PRB_BITFLIPSTRAILERmask_IBL       = 0x1F; // IBL bitflips mask in the trailer word
// the following words at the moment are used for the encoder, but not for the decoder. 
//In the decoder, no separate classification of the errors has been done yet.
static const uint32_t PRB_BCIDSKIPTRAILERskip_IBL      = 0x0; // IBL BCIDskip in trailer word skip
static const uint32_t PRB_BCIDSKIPTRAILERmask_IBL      = 0x1F; // IBL BCIDskip in trailer word mask
static const uint32_t PRB_L1IDSKIPTRAILERskip_IBL      = 19; // IBL L1IDskip in trailer word skip
static const uint32_t PRB_L1IDSKIPTRAILERmask_IBL      = 0x1; // IBL L1IDskip in trailer word mask
static const uint32_t PRB_PREAMBLEERRORTRAILERskip_IBL = 20; // IBL L1IDskip in trailer word skip
static const uint32_t PRB_PREAMBLEERRORTRAILERmask_IBL = 0x1; // IBL L1IDskip in trailer word mask
static const uint32_t PRB_LINKMASKEDskip_IBL           = 21;
static const uint32_t PRB_LINKMASKEDmask_IBL           = 0x1;
static const uint32_t PRB_CONDENSEDMODEskip_IBL        = 22;
static const uint32_t PRB_CONDENSEDMODEmask_IBL        = 0x1;
static const uint32_t PRB_TIMEOUTERRORskip_IBL         = 23;
static const uint32_t PRB_TIMEOUTERRORmask_IBL         = 0x1;


// OBSOLETE. Flag type 1 is not used anymore in the code
// Pixel module flag type 1 words
//static const uint32_t PRB_FEFlagmask1        = 0xF;   // FE flags type 1 word mask
//static const uint32_t PRB_FEFlagskip1        = 0;   // FE flags type 1 word skip
//static const uint32_t PRB_FE2mask            = 0xF;   // FE flag second FE number mask (to compare with first Circuitnumber)
//static const uint32_t PRB_FE2skip            = 4;   // FE flag second FE number skip
//static const uint32_t PRB_FEFLAG1BITFLIPmask = 0x7FF;
//static const uint32_t PRB_FEFLAG1BITFLIPskip = 13;

// Pixel module flag type 2 words
static const uint32_t PRB_FEFlagmask2        = 0xFF;   // FE flags type 2 word mask
static const uint32_t PRB_FEFlagskip2        = 0;   // FE flags type 2 word skip
static const uint32_t PRB_MCCFlagmask        = 0x1F;   // FE flags type 2 word mask
static const uint32_t PRB_MCCFlagskip        = 8;   // FE flags type 2 word skip
static const uint32_t PRB_FEFLAG2BITFLIPmask = 0x7;
static const uint32_t PRB_FEFLAG2BITFLIPskip = 21;

// IBL module FE flag  words 
static const uint32_t PRB_SERVICECODECOUNTERskip_IBL = 0; // Service code counter in the IBL FE flag word
static const uint32_t PRB_SERVICECODECOUNTERmask_IBL = 0x3FF; // Service code counter in the IBL FE flag word
static const uint32_t PRB_BITFLIPSFEFLAGskip_IBL     = 10; // bit flips in the IBL FE flag word
static const uint32_t PRB_BITFLIPSFEFLAGmask_IBL     = 0x207F; // bit flips in the IBL FE flag word
static const uint32_t PRB_SERVICECODEskip_IBL        = 17; // Service code in the IBL FE Flag word
static const uint32_t PRB_SERVICECODEmask_IBL        = 0x3F; // Service code in the IBL FE Flag word
static const uint32_t PRB_LINKNUMBERFEFLAGskip_IBL   = 24; // Link number (= FE number linked to the S-Link sending the stream and S-Link itself) in the IBL FE Flag word
static const uint32_t PRB_LINKNUMBERFEFLAGmask_IBL   = 0x1F; // Link number (= FE number linked to the S-Link sending the stream) in the IBL FE Flag word


// Pixel RawData word // Not used at the moment
//static const uint32_t PRB_RAWDATAskip = 0;  // Raw data word skip
//static const uint32_t PRB_RAWDATAmask = 0x1FFFFFFF; // Raw Data word mask


// word type masks (just use bits 29-31)
static const uint32_t PRB_DATAMASK    = 0xE0000000;   // word type mask
static const uint32_t PRB_HITMASK     = 0x80000000;  // word hit mask
static const uint32_t PRB_LINKHEADER  = 0x20000000;   // module header word mask
static const uint32_t PRB_LINKTRAILER = 0x40000000;   // module trailer word mask
static const uint32_t PRB_RAWDATA     = 0x60000000;   // raw data word mask 
static const uint32_t PRB_DATAWORD    = 0x80000000;   // hit word mask 
// special word type masks
static const uint32_t PRB_TIMEOUT     = 0x20004000;   // timeout word mask - does not use word type mask
static const uint32_t PRB_FEERRORMASK = 0xF0000000;   // flag word type mask -- for Pixel and IBL
//static const uint32_t PRB_FEERROR1    = 0x00000000;   // type 1 flag word -- for Pixel and IBL: for the IBL, it is used for the FE Flag error Short
static const uint32_t PRB_FEERROR2    = 0x10000000;   // type 2 flag word  -- for Pixel and IBL: for the IBL, it is used for the FE Flag error (normal)

static const uint32_t PRB_UNKNOWNWORD = 0xFFFFFFFF;   // unknown word
// word empty bits (filled by ROD with 0's) masks
//static const uint32_t PRB_LINKHEADEREMPTY        = 0x01800000;   // module header word empty bits (filled by ROD with 0's) mask - for Pixel
static const uint32_t PRB_DATAWORDEMPTY       = 0x1000E000;   // hit word empty bits (filled by ROD with 0's) mask - for Pixel
static const uint32_t PRB_LINKTRAILEREMPTY    = 0x03FFFFFF;   // module trailer word empty bits (filled by ROD with 0's) mask - for Pixel
//static const uint32_t PRB_FEERROR1CHECK       = 0x00FFFF00;   // flag word type 1 determined bits mask - for Pixel
static const uint32_t PRB_FEERROR2CHECK       = 0x001F0000;   // flag word type 2 determined bits mask - for Pixel

//static const uint32_t PRB_FEERRORCHECK_IBL       = 0x0F81FC00;   // flag word determined bits mask - for IBL
//static const uint32_t PRB_FEERROR2CHECK_IBL = 0xF81FC00;
static const uint32_t PRB_FEERROR_IBL = 0x00000000;
//static const uint32_t PRB_FEERROR1CHECK_IBL = 0x0F800000; 
//static const uint32_t PRB_FEERROR1CHECKRESULT = 0x00001E00;   // flag word type 1 determined bits mask result - for Pixel
//static const uint32_t PRB_FEERROR2CHECKRESULT = 0x001F0000;   // flag word type 2 determined bits mask result - for Pixel
//static const uint32_t PRB_FEERRORCHECKRESULT_IBL    = 0x00000000;   // flag word type normal and Short - for IBL



const uint32_t PRB_LINKTRAILEREMPTY_IBL = 0x0000001F; // trailer word empty bits (filled by ROD with 0's) mask - for IBL

// words for the IBL which are different from those for the Pixel detector
/*const uint32_t PRB_LINKHEADER_IBL      = 0x200000A0; // header word for the IBL found
const uint32_t PRB_DATAWORD_IBL        = 0x800000A0; // hit word for the IBL found
const uint32_t PRB_LINKTRAILER_IBL     = 0x400000A0; // trailer word for the IBL found
const uint32_t PRB_DATAWORD_SHORT_IBL_first    = 0xA0000000; // condensed first hit word - for IBL
const uint32_t PRB_DATAWORD_SHORT_IBL_middle   = 0x10000000; // condensed middle hit words - for IBL
const uint32_t PRB_DATAWORD_SHORT_IBL_last     = 0xE0000000; // condensed last hit word - for IBL
const uint32_t PRB_FEERROR_CHECK_IBL_short     = 0xFF800000; // FE flag error short word mask - for IBL
const uint32_t PRB_FERROR2CHECK_IBL            = 0xEF81FC00; // FE flag error word mask - for IBL
const uint32_t PRB_LINKTRAILEREMPTY_IBL = 0x000007FF; // trailer word empty bits (filled by ROD with 0's) mask - for IBL
const uint32_t PRB_DATAWORDEMPTY_IBL    = 0x001F0000; // hit word empty bits (filled by ROD with 0's) mask - for IBL
*/
#endif
