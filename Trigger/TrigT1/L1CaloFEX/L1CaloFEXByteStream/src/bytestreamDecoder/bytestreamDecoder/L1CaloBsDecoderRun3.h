/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1CALO_BS_DECODER_RUN3_H
#define L1CALO_BS_DECODER_RUN3_H

#include <list>
#include <stdint.h>

#include "bytestreamDecoder/L1CaloRdoFexTob.h"

class L1CaloRdoEfexTob;
class L1CaloRdoEfexTower;
class L1CaloRdoGfexTob;
class L1CaloRdoGfexTower;
class L1CaloRdoJfexTob;
class L1CaloRdoJfexTower;
class L1CaloRdoMuonTob;     // **FIXME** Different class for run 3?
class L1CaloRdoRodInfo;

class L1CaloBsDecoderRun3
{
public:
   L1CaloBsDecoderRun3();

   void decodeEfexData( const uint32_t* beg, const uint32_t* end,
                        std::list<L1CaloRdoEfexTower>& dat,
                        std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );

   void decodeEfexTobs( const uint32_t* beg, const uint32_t* end,
                        std::list<L1CaloRdoEfexTob>& tob,
                        std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );
#ifndef OFFLINE_DECODER
   void decodeJfexData( const uint32_t* beg, const uint32_t* end,
                        std::list<L1CaloRdoJfexTower>& dat,
                        std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );

   void decodeJfexTobs( const uint32_t* beg, const uint32_t* end,
                        std::list<L1CaloRdoJfexTob>& tob,
                        std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );

   void decodeGfexData( const uint32_t* beg, const uint32_t* end,
                        std::list<L1CaloRdoGfexTower>& dat,
                        std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );

   void decodeGfexTobs( const uint32_t* beg, const uint32_t* end,
                        std::list<L1CaloRdoGfexTob>& dat,
                        std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );

   void decodePh1TopoData( const uint32_t* beg, const uint32_t* end,
                           std::list<L1CaloRdoEfexTob>& etob,
                           std::list<L1CaloRdoJfexTob>& jtob,
                           std::list<L1CaloRdoGfexTob>& gtob,
                           std::list<L1CaloRdoMuonTob>& mtob,
                           std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );

   //>>void decodePh1TopoHits( const uint32_t* beg, const uint32_t* end,
   //>>                     std::list<L1CaloRdoPh1TopoHit>& dat,
   //>>                     std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );
#endif
   void setVerbosity( bool verbosity );

private:

   uint32_t decodeEfexDataChan( const uint32_t payload[],
                                const uint32_t efexNumber,
                                const uint32_t shelfNumber,
                                const uint32_t errorMask,
                                std::list<L1CaloRdoEfexTower>& dat,
                                std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );

   bool decodeEfexTobSlice( const uint32_t payload[], size_t& index,
                            const uint32_t efexNumber, const uint32_t shelfNumber,
                            const uint32_t numSlices, const uint32_t errorMask,
                            std::list<L1CaloRdoEfexTob>& tob,
                            std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );
#ifndef OFFLINE_DECODER
   uint32_t decodeJfexDataChan( const uint32_t payload[],
                                const uint32_t jfexNumber,
                                const uint32_t fpgaNumber,
                                const uint32_t errorMask,
                                std::list<L1CaloRdoJfexTower>& dat,
                                std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );

   bool decodeJfexTobSlice( const uint32_t payload[], size_t blockSize, size_t& index,
                            const uint32_t jfexNumber, const uint32_t fpgaNumber,
                            const uint32_t sliceNumber, const uint32_t numSlices,
                            const uint32_t errorMask,
                            std::list<L1CaloRdoJfexTob>& tob,
                            std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );

   uint32_t decodeGfexDataChan( const uint32_t payload[],
                                const uint32_t fpgaNumber,
                                const uint32_t chanNumber,
                                const uint32_t errorMask,
                                std::list<L1CaloRdoGfexTower>& dat,
                                std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );

   bool decodeGfexTobSlice( const uint32_t payload[], const uint32_t blockType,
                            const uint32_t sliceNumber, const uint32_t numSlices,
                            const uint32_t errorMask,
                            std::list<L1CaloRdoGfexTob>& tob,
                            std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );
#endif
   void decodeOneEfexTob( const uint32_t word[], const uint32_t shelfNumber,
                          const uint32_t efexNumber, const uint32_t fpgaNumber,
                          const uint32_t errorMask,
                          const uint32_t numSlices, const uint32_t sliceNum,
                          L1CaloRdoFexTob::TobType tobType,
                          L1CaloRdoFexTob::TobSource tobSource,
                          std::list<L1CaloRdoEfexTob>& tob,
                          std::list<L1CaloRdoRodInfo>::const_iterator rodInfo );

   bool checkFibreCRC( std::vector<uint32_t>& data ) const;

   int m_verbosity;
};

#endif
