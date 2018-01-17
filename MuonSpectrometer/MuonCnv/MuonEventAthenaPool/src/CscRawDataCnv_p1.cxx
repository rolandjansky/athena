/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/CscRawData.h"
#include "CscRawDataCnv_p1.h"

#include "GaudiKernel/GaudiException.h"

void
CscRawDataCnv_p1::transToPers(const CscRawData* /*trans*/, CscRawData_p1* /*pers*/, MsgStream &) 
{
  throw GaudiException("Writing to CscRawData_p1 is not supported ", "CscRawDataCnv_p1::transToPers(...)", StatusCode::FAILURE);
}

void
CscRawDataCnv_p1::persToTrans(const CscRawData_p1* pers, CscRawData* trans, MsgStream &) 
{

   /** we did not have the pedestal before
       now add that during conversion : pedestal set to 2048 ADC counts
       Ketevi A. Assamagan - April 11, 2007 */
   unsigned int size = (pers->m_amps).size();
   std::vector<uint16_t> amps(size);
   for (unsigned int i=0; i<size; ++i)
     amps[i] = pers->m_amps[i] + 2048;


  /** conversion of the chamberLayer index into the new format */
  int stationName =  ( ( pers->m_address & 0x00010000) >> 16 ) + 50;
  int stationEta  =  ( ((pers->m_address & 0x00001000) >> 12 ) == 0x0) ? -1 : 1;
  int stationPhi  =  ( ( pers->m_address & 0x0000E000) >> 13 ) + 1;
  int chamberLayer = ( ( pers->m_address & 0x00000800) >> 11) + 1;
  int wireLayer    = ( ( pers->m_address & 0x00000600) >>  9) + 1;
  int measuresPhi  = ( ( pers->m_address & 0x00000100) >>  8);
  int strip        = (   pers->m_address & 0x000000FF) + 1; // 1 ~ 48
   
  // redefine the ranges
  uint32_t nameIndex    = uint32_t(stationName-50);
  uint32_t etaIndex     = (stationEta  == -1) ? 0 : 1;
  uint32_t phiIndex     = uint32_t (stationPhi-1);
  /// ??? This looks wrong!  chamberLayer is either 1 or 2, but then this
  ///      gets packed into a 1-bit field.  Should this be -1?
  uint32_t chamberIndex = uint32_t (chamberLayer-0);
  uint32_t layerIndex   = uint32_t (wireLayer-1);
  uint32_t stripType    = uint32_t (measuresPhi);
  uint32_t stripNumber  = uint32_t (strip-1); // 0 ~ 47


  // schema evolution for case a) below https://savannah.cern.ch/bugs/?56002
  // There can be two cases:
  // (a)  Reco in 15.5.X.Y or after reading RDO made in 15.3.X.Y and before.
  // (b)  both reco and simulation in 15.5.X.Y or after
  // This is for ATLAS-CSC-XXX tag...
  
  if (stationEta==1 && measuresPhi==1) { // for Wheel A phi strips
    stripNumber = uint32_t(47 - stripNumber); // 0 ~ 47 
  }
  
  // build the address
  uint32_t address = nameIndex    << 16 |
                     phiIndex     << 13 |
                     etaIndex     << 12 |
                     chamberIndex << 11 |
                     layerIndex   <<  9 |
                     stripType    <<  8 |
                     stripNumber;


  /** there 10 SPU - Sparsifier Processing Units, one for each gas layer, except
      for the non-precision strips where all the layers map to one SPU
      note that the "-50" is because stationId = 50 (CSS) or 51 (CSL) */
  uint16_t spuID = 0x0;
  if ( stripType == 0 ) spuID = static_cast<uint16_t>( nameIndex*5 + layerIndex );
  else spuID = static_cast<uint16_t>( (nameIndex+1)*5-1 );

  *trans = CscRawData (amps, address, pers->m_id,
                       spuID, 1);
}

