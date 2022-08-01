/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include <algorithm>
#include <bitset>
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>

#include "eformat/ROBFragment.h"

#include "bytestreamDecoder/L1CaloBsDecoderRun3.h"
#include "bytestreamDecoder/L1CaloBsDecoderUtil.h"
#include "bytestreamDecoder/L1CaloRdoEfexTob.h"
#include "bytestreamDecoder/L1CaloRdoEfexTower.h"
#include "channelMappings/EfexCellMapping.h"
#include "infraL1Calo/EfexLatomeFibrePacker.h"
#include "infraL1Calo/EfexTrexFibrePacker.h"
#include "defsL1Calo/CrateDefs.h"
#include "defsL1Calo/EfexDefs.h"
#include "infraL1Calo/GenericCrc.h"
#ifndef OFFLINE_DECODER
#include "bytestreamDecoder/L1CaloRdoGfexTob.h"
#include "bytestreamDecoder/L1CaloRdoGfexTower.h"
#include "bytestreamDecoder/L1CaloRdoJfexTob.h"
#include "bytestreamDecoder/L1CaloRdoJfexTower.h"
#include "bytestreamDecoder/L1CaloRdoMuonTob.h"     // **FIXME** Different class for run 3?
#include "channelMappings/GfexCellMapping.h"
#include "channelMappings/JfexCellMapping.h"
#include "defsL1Calo/FexDefs.h"
#include "infraL1Calo/GfexLatomeCentralFibrePacker.h"
#include "infraL1Calo/GfexLatomeForwardFibrePacker.h"
#include "infraL1Calo/GfexTrexFibrePacker.h"
#include "infraL1Calo/JfexLatomeFibrePacker.h"
#include "infraL1Calo/JfexTrexFibrePacker.h"
#endif

#include "ers/ers.h"
#define ERS_ERROR(message) { std::stringstream s; s << __FUNCTION__ << " - " << message;ers::error(ers::Message(ERS_HERE, s.str()));}


/*!
 * \class L1CaloBsDecoderRun3
 * Decoder for Run 3 data formats: eFEX, jFEX, gFEX and phase 1 Topo.
 * Utility methods shared with Runs 1 & 2 are now in L1CaloBsDecoderUtil.
 */

L1CaloBsDecoderRun3::L1CaloBsDecoderRun3()
: m_verbosity(0)
{

  // Force initialisation of mapping tables.
  EfexCellMapping dummyEfexMapping(0,0,0,0,0,0);
  if (!dummyEfexMapping.getDetectorRegion().getValidity()) {
    std::cerr << "L1CaloBsDecoderRun3::ctor: unexpected invalid eFEX mapping!?" << std::endl;
  }
#ifndef OFFLINE_DECODER
  JfexCellMapping dummyJfexMapping(0,1,0,0);                    // Processor number 1-4
  if (!dummyJfexMapping.getDetectorRegion().getValidity()) {
    std::cerr << "L1CaloBsDecoderRun3::ctor: unexpected invalid jFEX mapping!?" << std::endl;
  }
  GfexCellMapping dummyGfexMapping(0,0,0,0);                    // Processor number 0-2?
  if (!dummyGfexMapping.getDetectorRegion().getValidity()) {
    std::cerr << "L1CaloBsDecoderRun3::ctor: unexpected invalid gFEX mapping!?" << std::endl;
  }
#endif
}

/*!
 * Decode eFEX input fibre data.
 * For the EM layer we have up to 10 supercell values per tower
 * though there is still just a single hadronic value.
 * It seemed overkill to have an RDO per supercell, so instead
 * the main RDO "Value" is set to the sum of 10 EM supercells
 * with the individual supercell Ets saved in a separate vector.
 * These are always stored in 1441 order (PS, F0-F3, M0-M3, BK)
 * even in regions where some supercells are missing.
 * The flag field is used for status/error bits.
 * \param beg pointer to start of rod data payload
 * \param end pointer to end of rod data payload
 * \param tower list of RDO to be filled
 * \param rodInfo iterator to ROD information for this block
 */
void
L1CaloBsDecoderRun3::decodeEfexData( const uint32_t* beg, const uint32_t* end,
                                     std::list<L1CaloRdoEfexTower>& tower,
                                     std::list<L1CaloRdoRodInfo>::const_iterator rodInfo )
{
   const uint32_t* payload( beg );
   const size_t fragmentSize = end - beg;
   
   // The data block is optimised for production by firmware with the length
   // in a trailer at the end. The SWROD concatenates a number of such blocks
   // (removing the 2 word header used to fill the ROD fragment header).
   // So we need to find the end and work backwards.
   
   size_t numTowers = tower.size();
   size_t index = fragmentSize;
   
   // Loop looking backwards for eFEX processor input data blocks.
   while ( index > 0 ) {
      if ( index < 4 ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeEfexData: remaining block size " << index
                   << " is too small for the eFEX FPGA trailer" << std::endl;
         return;
      }
      const uint32_t ctrlTrailer2 = payload[--index];
      const uint32_t ctrlTrailer1 = payload[--index];
      const uint32_t fpgaTrailer2 = payload[--index];
      const uint32_t fpgaTrailer1 = payload[--index];

      const uint32_t ctrlErrors  =  ctrlTrailer2 & 0x3f;
      const uint32_t efexNumber  = (ctrlTrailer1 >> 12) & 0xf;
      const uint32_t shelfNumber = (ctrlTrailer1 >> 16) & 0x1;  // Just use 1 bit from 4 bit field
      const size_t   payloadSize = (ctrlTrailer1 & 0xfff) - 2;
      
      if ( payloadSize > index ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeEfexData: remaining eFEX block size "
                   << index << " is too small for the claimed payload size "
                   << payloadSize << std::endl;
         return;
      }
      
      if (efexNumber >= CrateDefs::numAtcaFexSlots() ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeEfexData: invalid eFEX number " << efexNumber
                   << " (out of range 0-" << CrateDefs::numAtcaFexSlots()-1 << ")" << std::endl;
         return;
      }
      
      // We can now work forwards from the start of this block
      // decoding each set of 8 words per input fibre channel.
      // The payloadSize has had two trailer words subtracted.
      index -= payloadSize;
      size_t chanIndex = 0;
      uint32_t chanErrorOR = 0;
      std::bitset<49> chansWithError;
      bool anyErrorBit = false;
      while ( chanIndex < payloadSize ) {
         if ( (payloadSize - chanIndex) < 8 ) {
            std::cerr << "L1CaloBsDecoderRun3::decodeEfexData: crate " << shelfNumber << " efex " << efexNumber
                      << ": remaining eFEX block size " << (payloadSize - chanIndex)
                      << " is too small for one eFEX input fibre block (8)" << std::endl;
            return;
         }
         const uint32_t chanNumber = payload[index+chanIndex+7] & 0xff;
         const uint32_t chanErrors = this->decodeEfexDataChan ( &payload[index+chanIndex], efexNumber, shelfNumber,
                                                                ctrlErrors, tower, rodInfo );
         if ( chanErrors ) {
            chanErrorOR |= ( chanErrors & 0x7 );   // Only three lowest bits get ORed
            chansWithError[chanNumber] = 1;
            anyErrorBit = true;
         }
         chanIndex += 8;
      }
      if ( anyErrorBit ) {
         chanErrorOR |= 0x8;   // Extra bit set if any of the three lower bits are set
      }
      const uint64_t fpgaErrorBits = ( (uint64_t)(fpgaTrailer2 & 0x1ffff) << 32 ) | fpgaTrailer1;
      const uint64_t chanErrorBits = chansWithError.to_ullong();
      const uint32_t fpgaErrorOR = ( fpgaTrailer2 >> 28 ) & 0xf;
      if ( fpgaErrorBits != chanErrorBits || fpgaErrorOR != chanErrorOR ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeEfexData: crate " << shelfNumber << " efex " << efexNumber
                   << ": mismatch between errors in FPGA trailer: "
                   << std::hex << fpgaErrorBits << " " << fpgaErrorOR
                   << " and those derived from channels: " << chanErrorBits << " " << chanErrorOR
                   << std::dec << std::endl;
      }
   }
   if ( m_verbosity > 0 )
   {
      std::cout << "L1CaloBsDecoderRun3::decodeEfexData: n.towers added="
                << tower.size() - numTowers << std::endl;
   }
}

/*!
 * Decode the data from one eFEX input fibre (only ever one slice).
 * \param payload payload vector starting at this 8 word block
 * \param efexNumber number of this eFEX in its shelf
 * \param shelfNumber shelf number (here 0 or 1)
 * \param errorMask global error bits set for this ROD fragment
 * \param tower list of RDO to be filled
 * \param rodInfo iterator to ROD information for this block
 * \return whether decoding succeeded (false to abort decoding)
 */
uint32_t
L1CaloBsDecoderRun3::decodeEfexDataChan( const uint32_t payload[],
                                         const uint32_t efexNumber, const uint32_t shelfNumber,
                                         const uint32_t errorMask,
                                         std::list<L1CaloRdoEfexTower>& tower,
                                         std::list<L1CaloRdoRodInfo>::const_iterator rodInfo )
{
   // The EM and hadronic fibres have different encoding and cover
   // different numbers of towers with more or less granularity.
   // Channel numbers 0-39 are EM, 40-48 are hadronic.
   const uint32_t chanNumber = ( payload[7]       ) & 0xff;
   const uint32_t fpgaNumber = ( payload[7] >>  8 ) &  0x3;
   const uint32_t errorBits  = ( payload[7] >> 28 ) &  0x7;
   const uint32_t disabled   = ( payload[7] >> 31 ) &  0x1;
   if ((int)chanNumber >= EfexDefs::numInputFibresPerFpga()) {
      std::cerr << "L1CaloBsDecoderRun3::decodeEfexDataChan: invalid channel " << chanNumber
                << " (out of range 0-" << EfexDefs::numInputFibresPerFpga()-1 << ")" << std::endl;
      return 0xffffffff;
   }
   
   // Some endcap channels are inactive and have the disabled bit set
   // if the full data is being read out. Just skip these.
   if (disabled) {
     return 0;
   }
   
   // The EfexCellMapping returns global coordinates but the RDO objects
   // expect local eta,phi within the module. So prepare these here.
   // Find local eta,phi from global EfexCellMapping and module offset.
   // There is no good way of handling the localEta. Below we add one
   // so the barrel eta has core towers in localEta 1-16 with 0 and 17
   // being core towers on C and A sides. Overlap towers extend further
   // and both localEta and localPhi may be negative.
   int moduleEta = -24 + (efexNumber % 3) * 16;
   int modulePhi =  2 + (shelfNumber * 32) + (efexNumber / 3) * 8;
   
   // The fibre packer decoder methods expect a vector of seven words.
   std::vector<FibrePackerBase::myDataWord> encodedData;
   for ( size_t i = 0; i < 7; i++ ) {
      encodedData.push_back( payload[i] );
   }
   const FibrePackerBase::InputDataFrameType frameType( FibrePackerBase::InputDataFrameType::Normal );
   
   // Build up error flag for towers on this fibre.
   // Bits  0-5 from ROD block trailer
   // Bit     7 fibre CRC error flag
   // Bits 8-10 from fibre trailer
   bool rightCRC = this->checkFibreCRC( encodedData );
   const uint32_t errorField = ( errorMask & 0x3f )
                             | ( rightCRC ? 0 : 0x80 )
                             | ( errorBits << 8 );
   
   if (chanNumber < 40) {
      // Each EM fibre has 10 supercells for each of two 0.1*0.1 trigger towers.
      // The EfexLatomeFibrePacker returns the complete set of 20 supercells.
      EfexLatomeFibrePacker packer;
      std::vector<FibrePackerBase::myDataWord> cells = packer.getUnpackedData( encodedData, frameType );
      
      // Create two tower RDOs, each containing half the supercells.
      // NB all the middle layer supercells come first, then the rest
      // for each tower: 4xM0, 4xM1, P0,4xF0,B0, P1,4xF1,B1.
      // Hard code this to avoid too many calls to EfexCellMapping.
      // We just call it with the first supercell word for each tower.
      const std::vector<uint32_t> cellIndex = { 0, 1, 2, 3, 8, 9,10,11,12,13,    // Tower0: MMMMPFFFFB
                                                4, 5, 6, 7,14,15,16,17,18,19 };  // Tower1: MMMMPFFFFB
      for ( size_t k = 0; k < 2; k++ ) {
         std::vector<uint32_t> towerCells( 10, 0 );
         uint32_t towerSumEt( 0 );
         uint32_t wordNumber = 0;
         for (size_t i = 0; i < 10; i++) {
            uint32_t iCell = cellIndex[i+k*10];
            if (i == 0) {
               wordNumber = iCell;
            }
            towerCells[i] = cells[iCell];
            towerSumEt += towerCells[i];
         }
         
         // **FIXME** Fibre packer does not yet return quality bits!
         //           There should be one per middle layer supercell.
         //           We can OR those with error bits for this fibre.
         uint32_t towerFlag = errorField;
         
         if ( towerSumEt || towerFlag ) {
            EfexCellMapping mapping( shelfNumber, efexNumber, fpgaNumber, chanNumber, wordNumber );
            L1CaloDetectorRegion region = mapping.getDetectorRegion();
            EfexHardwareInfo hwInfo( mapping.getHardwareInfo() );
            if ( region.getValidity() && hwInfo.getValidity() ) {
               int localEta = region.getEtaIndex() - moduleEta + 1;
               int localPhi = region.getPhiIndex() - modulePhi;

               const uint32_t layer = 0; // EM
               L1CaloRdoEfexTower newOne( shelfNumber, efexNumber, localEta, localPhi, layer, region );
               newOne.setRodInfo( rodInfo );
               L1CaloRdoEfexTower& rdo = L1CaloBsDecoderUtil::findRdo( newOne, tower );

               rdo.setHardwareInfo( fpgaNumber, chanNumber, wordNumber,
                                    hwInfo.getMpodNumber(), hwInfo.getFibreNumber(),
                                    hwInfo.getOverlap() );
               rdo.setValue( towerSumEt );
               rdo.setFlag( towerFlag );
               rdo.setSupercells( towerCells );
               if ( m_verbosity > 0 )
               {
                  std::cout << "L1CaloBsDecoderRun3::decodeEfexDataChan: EM"
                            << ", shelf=" << shelfNumber << ", module=" << efexNumber
                            << std::hex << ", Et=0x" << towerSumEt << ", flag=0x" << towerFlag
                            << std::dec << std::endl;
               }
            }
         }
      }
   }
      
   else {
      // Each hadronic fibre has a 4*4 array of 16 towers.
      EfexTrexFibrePacker packer;
      std::vector<FibrePackerBase::myDataWord> towerVals = packer.getUnpackedData( encodedData, frameType );
      size_t numHadTowers = std::min(towerVals.size(),(size_t)16);
      
      // Create an RDO for each tower.
      for ( size_t wordNumber = 0; wordNumber < numHadTowers; wordNumber++ ) {
         const uint32_t towerEt = ( towerVals[wordNumber] == 0x3fe ) ? 0 : towerVals[wordNumber];
         const uint32_t invalid = ( towerVals[wordNumber] == 0x3fe ) ? 1 : 0;
         const uint32_t towerFlag = errorMask | ( invalid << 31 );
         
         if ( towerEt || towerFlag ) {
            EfexCellMapping mapping( shelfNumber, efexNumber, fpgaNumber, chanNumber, wordNumber );
            L1CaloDetectorRegion region = mapping.getDetectorRegion();
            EfexHardwareInfo hwInfo( mapping.getHardwareInfo() );
            if ( region.getValidity() && hwInfo.getValidity() ) {
               // Need to decide if data is Tile/TREX or HEC/LATOME as TREX tower Et is still 
               // limited to 8 bits, at least for the start of Run 3, as in Runs 1 & 2.
               // After the legacy CP/JEP system is decommissioned this may change to enable
               // use of the full 10 bit field.
               if ( (region.getEtaIndex() >= -15) && (region.getEtaIndex() < 15) ) {  // Tile/TREX
                  towerVals[wordNumber] &= 0xff;
               }

               int localEta = region.getEtaIndex() - moduleEta + 1;
               int localPhi = region.getPhiIndex() - modulePhi;

               const uint32_t layer = 1; // Hadronic
               L1CaloRdoEfexTower newOne( shelfNumber, efexNumber, localEta, localPhi, layer, region );
               newOne.setRodInfo( rodInfo );
               L1CaloRdoEfexTower& rdo = L1CaloBsDecoderUtil::findRdo( newOne, tower );

               rdo.setHardwareInfo( fpgaNumber, chanNumber, wordNumber,
                                    hwInfo.getMpodNumber(), hwInfo.getFibreNumber(),
                                    hwInfo.getOverlap() );
               rdo.setValue( towerEt );
               rdo.setFlag( towerFlag );
               if ( m_verbosity > 0 )
               {
                  std::cout << "L1CaloBsDecoderRun3::decodeEfexDataChan: Had"
                            << ", shelf=" << shelfNumber << ", module=" << efexNumber
                            << std::hex << ", Et=0x" << towerEt << ", flag=0x" << towerFlag
                            << std::dec << std::endl;
               }
            }
         }
      }
   }
   
   return errorBits;
}

/*!
 * Decode eFEX TOBs and XTOBs.
 * The RDO value encodes the cluster Et in the value field
 * and the isolation bits and other information in the flag.
 * \param beg pointer to start of rod data payload
 * \param end pointer to end of rod data payload
 * \param tob list of RDO to be filled
 * \param rodInfo iterator to ROD information for this block
 */
void
L1CaloBsDecoderRun3::decodeEfexTobs( const uint32_t* beg, const uint32_t* end,
                                     std::list<L1CaloRdoEfexTob>& tob,
                                     std::list<L1CaloRdoRodInfo>::const_iterator rodInfo )
{
   const uint32_t* payload( beg );
   const size_t fragmentSize = end - beg;
   
   // The data block is optimised for production by firmware as a set
   // of nested blocks with the lengths at the end. So we need to find
   // the end and work backwards.

   if ( fragmentSize < 2 ) {
      std::cerr << "L1CaloBsDecoderRun3::decodeEfexTobs: fragment size " << fragmentSize
                << " is too small for the ROD trailer" << std::endl;
      return;
   }
   
   size_t index = fragmentSize;
   const uint32_t rodTrailer2 = payload[--index];
   const uint32_t rodTrailer1 = payload[--index];
   
   const uint32_t rodErrors = rodTrailer2 & 0x7f;
   const size_t payloadSize = rodTrailer1 & 0xffff;
   if ( (payloadSize + 2) != fragmentSize ) {
      // Actual ROD fragment payload size does not match that claimed in the trailer.
      std::cerr << "L1CaloBsDecoderRun3::decodeEfexTobs: payload size " << payloadSize
                << " inconsistent with ROD fragment size " << fragmentSize << std::endl;
      return;
   }
   
   // Loop looking backwards for eFEX module blocks.
   while ( index > 0 ) {
      if ( index < 2 ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeEfexTobs: remaining block size " << index
                   << " is too small for the eFEX trailer" << std::endl;
         return;
      }
      size_t efexIndex = index;
      const uint32_t efexTrailer2 = payload[--efexIndex];
      const uint32_t efexTrailer1 = payload[--efexIndex];
      const size_t efexBlockSize = efexTrailer1 & 0xfff;
      const uint32_t efexNumber  = (efexTrailer1 >> 12) & 0xf;
      const uint32_t shelfNumber = (efexTrailer1 >> 16) & 0x1;   // 4 bit field, but only 1 but used
      //??const uint32_t procErrMap  = (efexTrailer1 >> 20) & 0xf;  // Currently unused
      const uint32_t numSlices   = (efexTrailer1 >> 24) & 0xf;
      //??const uint32_t l1aSlice    = (efexTrailer1 >> 28) & 0xf;  // Currently unused
      const uint32_t efexErrors  = efexTrailer2 & 0x3f;
      if ( efexBlockSize > index ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeEfexTobs: eFEX block size " << efexBlockSize
                   << " exceeds remaining data size " << index
                   << " (shelf " << shelfNumber << " eFEX " << efexNumber << ")" << std::endl;
         return;
      }
      // Update index to previous eFEX block (if any).
      index = efexIndex - efexBlockSize;
      
      // Combine rod and efex error fields.
      const uint32_t errorMask = efexErrors | (rodErrors << 6);
      
      // Loop looking backwards for eFEX processor blocks.
      // There should be one block per slice per FPGA
      // (except for errors giving corrective trailers).
      while ( efexIndex > index ) {
         if ( (efexIndex - index) < 2 ) {
            std::cerr << "L1CaloBsDecoderRun3::decodeEfexTobs: remaining eFEX block size "
                      << (efexIndex - index)
                      << " is too small for the eFEX slice trailer" << std::endl;
            return;
         }
         size_t procIndex = efexIndex;
         bool ret = this->decodeEfexTobSlice ( payload, procIndex, efexNumber, shelfNumber,
                                               numSlices, errorMask, tob, rodInfo );
         if ( ! ret ) {
            return;
         }
         efexIndex = procIndex;
      }
   }
}

/*!
 * Decode one eFEX FPGA block of TOBs and XTOBs for one slice.
 * \param payload entire ROD fragment payload vector
 * \param index just past end of block for this slice
 *        (this method updates it to the start of the block)
 * \param efexNumber number of this eFEX in its shelf
 * \param shelfNumber shelf number
 * \param numSlices number of TOB slices read out in this event
 * \param errorMask global error bits set for this ROD fragment
 * \param tob list of RDO to be filled
 * \param rodInfo iterator to ROD information for this block
 * \return whether decoding succeeded (false to abort decoding
           in which case the returned index is not valid)
 */
bool
L1CaloBsDecoderRun3::decodeEfexTobSlice( const uint32_t payload[], size_t& index,
                                         const uint32_t efexNumber, const uint32_t shelfNumber,
                                         const uint32_t numSlices, const uint32_t errorMask,
                                         std::list<L1CaloRdoEfexTob>& tob,
                                         std::list<L1CaloRdoRodInfo>::const_iterator rodInfo )
{
   if ( index < 1 ) {
      return false;
   }
   
   const uint32_t sliceTrailer = payload[index-1];
   const uint32_t corrective  = (sliceTrailer >> 31) & 0x01;
   const uint32_t tauInputErr = (sliceTrailer >> 30) & 0x01;
   const uint32_t emInputErr  = (sliceTrailer >> 29) & 0x01;
   
   if ( corrective ) {
      // Corrective trailer: no data from this processor FPGA.
      // Not much we can do so just report it and carry on.
      size_t corrBlockSize = sliceTrailer & 0xfff;
      const uint32_t failingBCN  = (sliceTrailer >> 12) & 0xfff;
      const uint32_t fpgaNumber  = (sliceTrailer >> 24) &   0x3;
      std::cerr << "L1CaloBsDecoderRun3::decodeEfexTobs: corrective trailer from FPGA "
                << fpgaNumber << " eFEX " << efexNumber << " shelf " << shelfNumber
                << ", failing BCN " << failingBCN << ", input errors tau=" << tauInputErr
                << ", em=" << emInputErr << ", trailer size " << corrBlockSize << std::endl;
       // corrective trailer blockSize doesn't include the trailer itself (length of one)
       // and also doesn't include padding word (which occurs if the blockSize is even
       // because the corrective trailer adds one more word and then a padding word is needed
      corrBlockSize += ((corrBlockSize % 2) == 1) ? 1 : 2;
      index -= corrBlockSize;   // Ought to be 2
   }
   else {
      // October 2021: New format implemented! K.Char in lowest 8 bits.
      const uint32_t tobType     = (sliceTrailer >>  8) &   0x1;
      const uint32_t numTobs     = (sliceTrailer >>  9) &   0x7;
      const uint32_t numEmXtobs  = (sliceTrailer >> 12) &  0x3f;
      const uint32_t numTauXtobs = (sliceTrailer >> 18) &  0x3f;
      const uint32_t sliceNumber = (sliceTrailer >> 24) &   0x7;
      const uint32_t safeMode    = (sliceTrailer >> 27) &   0x1;
      const uint32_t fpgaNumber  = (sliceTrailer >> 28) &   0x3;

      // Work out how long this block should be. Each TOB is one word,
      // each XTOB is two words. If the total is odd (including one word
      // for the trailer) there should be an extra zero padding word.
      // However in safe mode all the TOBs and XTOBs are suppressed
      // and we only have the trailer and padding word.
      size_t tobSize = (safeMode == 0)
                     ? (1 + numTobs + 2 * (numEmXtobs + numTauXtobs))
                     : 2;
      tobSize += (tobSize % 2);

      // Move index to start of this block.
      index -= tobSize;

      if ( safeMode ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeEfexTobs: safe mode from FPGA "
                   << fpgaNumber << " eFEX " << efexNumber << " shelf " << shelfNumber
                   << ", missed " << numTobs << " TOBs, " << numEmXtobs << " EM XTOBs, "
                   << numTauXtobs << " Tau XTOBs" << std::endl;
      }
      else {
         // Luxury, we can now work forwards inside the slice block!
         // We expect first TOBs (if any) then EM xTOBs, then Tau xTOBs.
         const size_t totalTobs = numTobs + numEmXtobs + numTauXtobs;
         L1CaloRdoFexTob::TobType fexTobType = (tobType == 0)
                                             ? L1CaloRdoFexTob::TobType::EM
                                             : L1CaloRdoFexTob::TobType::Tau;
         L1CaloRdoFexTob::TobSource fexTobSource = L1CaloRdoFexTob::TobSource::EfexTob;
         size_t sliceIndex = index;
         for (size_t iTOB = 0; iTOB < totalTobs; iTOB++) {
            if (iTOB >= numTobs) {
               fexTobSource = L1CaloRdoFexTob::TobSource::EfexXtob;
               fexTobType = (iTOB < (numTobs + numEmXtobs))
                          ? L1CaloRdoFexTob::TobType::EM
                          : L1CaloRdoFexTob::TobType::Tau;
            }
            this->decodeOneEfexTob ( &payload[sliceIndex], shelfNumber, efexNumber, fpgaNumber,
                                     errorMask, numSlices, sliceNumber, fexTobType, fexTobSource,
                                     tob, rodInfo );
            // One or two words per TOB or xTOB.
            sliceIndex += (fexTobSource == L1CaloRdoFexTob::TobSource::EfexTob) ? 1 : 2;
         }
      }
   }
   return true;
}
#ifndef OFFLINE_DECODER
/*!
 * Decode jFEX input fibre data.
 * We create one RDO per tower using the value field for its Et.
 * The flag field is used for status/error bits.
 * \param beg pointer to start of rod data payload
 * \param end pointer to end of rod data payload
 * \param tower list of RDO to be filled
 * \param rodInfo iterator to ROD information for this block
 */
void
L1CaloBsDecoderRun3::decodeJfexData( const uint32_t* beg, const uint32_t* end,
                                     std::list<L1CaloRdoJfexTower>& tower,
                                     std::list<L1CaloRdoRodInfo>::const_iterator rodInfo )
{
   const uint32_t* payload( beg );
   const size_t fragmentSize = end - beg;
   
   // The data block is optimised for production by firmware with the length
   // in a trailer at the end. The SWROD concatenates a number of such blocks
   // (removing the 2 word header used to fill the ROD fragment header).
   // So we need to find the end and work backwards.
   
   size_t numTowers = tower.size();
   size_t index = fragmentSize;
   
   // Loop looking backwards for jFEX processor input data blocks.
   while ( index > 0 ) {
      if ( index < 2 ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeJfexData: remaining block size " << index
                   << " is too small for the jFEX FPGA trailer" << std::endl;
         return;
      }
      const uint32_t fpgaTrailer2 = payload[--index];
      const uint32_t fpgaTrailer1 = payload[--index];

      const uint32_t fpgaErrors  =  fpgaTrailer2 & 0x3f;
      const uint32_t jfexNumber  = (fpgaTrailer1 >> 20) & 0x7;
      const uint32_t fpgaNumber  = (fpgaTrailer1 >> 18) & 0x3;
      const size_t   payloadSize = (fpgaTrailer1 & 0xffff);
      
      if ( payloadSize > index ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeJfexData: remaining jFEX block size "
                   << index << " is too small for the claimed payload size "
                   << payloadSize << std::endl;
         return;
      }
      
      // We can now work forwards from the start of this block
      // decoding each set of 8 words per input fibre channel.
      index -= payloadSize;
      size_t chanIndex = 0;
      while ( chanIndex < payloadSize ) {
         if ( (payloadSize - chanIndex) < 8 ) {
            std::cerr << "L1CaloBsDecoderRun3::decodeJfexData: remaining jFEX block size "
                      << (payloadSize - chanIndex)
                      << " is too small for one jFEX input fibre block (8)" << std::endl;
            return;
         }
         this->decodeJfexDataChan ( &payload[index+chanIndex], jfexNumber, fpgaNumber,
                                    fpgaErrors, tower, rodInfo );
         chanIndex += 8;
      }
   }
   if ( m_verbosity > 0 )
   {
      std::cout << "L1CaloBsDecoderRun3::decodeJfexData: n.towers added="
                << tower.size() - numTowers << std::endl;
   }
}

/*!
 * Decode the data from one jFEX input fibre (only ever one slice).
 * \param payload payload vector starting at this 8 word block
 * \param jfexNumber number of this jFEX in its shelf
 * \param fpgaNumber FPGA number
 * \param errorMask global error bits set for this ROD fragment
 * \param tower list of RDO to be filled
 * \param rodInfo iterator to ROD information for this block
 * \return whether decoding succeeded (false to abort decoding)
 */
uint32_t
L1CaloBsDecoderRun3::decodeJfexDataChan( const uint32_t payload[],
                                         const uint32_t jfexNumber, const uint32_t fpgaNumber,
                                         const uint32_t errorMask,
                                         std::list<L1CaloRdoJfexTower>& tower,
                                         std::list<L1CaloRdoRodInfo>::const_iterator rodInfo )
{
   const uint32_t shelfNumber = 0x22;   // Hard code to P1 value (34)
   
   // The LATOME and TREX fibres have slightly different encoding.
   // Towards the forward regions and especially FCAL they cover
   // different numbers of towers with more or less granularity.
   // Channels 0-23 & 36-59 are standard inputs, 24-35 are overlap.
   // The HEC overlap minipod (AVR) is #3 of 1-5 in each FPGA.
   const uint32_t chanNumber = ( payload[7]       ) & 0xff;
   const uint32_t errorBits  = ( payload[7] >> 28 ) &  0xf;
   
   // **FIXME** Ignore overlap for the moment.
   if (chanNumber >= 24 &&  // **FIXME** Remove this!
       chanNumber <  36) {  // **FIXME** Remove this!
     return 0;              // **FIXME** Remove this!
   }                        // **FIXME** Remove this!
   // **FIXME** Ignore end jFEX modules for now.
   if (jfexNumber == 0 ||   // **FIXME** Remove this!
       jfexNumber == 5) {   // **FIXME** Remove this!
     return 0;              // **FIXME** Remove this!
   }                        // **FIXME** Remove this!
   
   // The JfexCellMapping returns global coordinates but the RDO objects
   // expect local eta within the module. So prepare this here.
   // **FIXME** Not sure how to handle internal eta for FCAL (esp C side).
   int moduleEta = -24 + (jfexNumber % 6) * 8;
   
   // The fibre packer decoder methods expect a vector of seven words.
   std::vector<FibrePackerBase::myDataWord> encodedData;
   for ( size_t i = 0; i < 7; i++ ) {
      encodedData.push_back( payload[i] );
   }
   const FibrePackerBase::InputDataFrameType frameType( FibrePackerBase::InputDataFrameType::Normal );
   
   // **FIXME** Need mapping to decide when to use Latome vs TREX decoders.
   // **FIXME** But they are identical apart from handling of saturation
   // **FIXME** so for the moment always use Latome.
   
   bool trex = false;  // **FIXME**
   
   // Unpack sixteen 0.1*0.1 trigger towers.
   std::vector<FibrePackerBase::myDataWord> towers;
   if (trex) {
      JfexTrexFibrePacker packer;
      towers = packer.getUnpackedData( encodedData, frameType );
   }
   else {
      // Unpack sixteen 0.1*0.1 trigger towers.
      JfexLatomeFibrePacker packer;
      towers = packer.getUnpackedData( encodedData, frameType );
   }

   // Create RDO per tower.
   for ( size_t iTower = 0; iTower < towers.size(); iTower++ ) {
      const uint32_t towerEt = towers[iTower];

      if ( towerEt || errorMask ) {
         // The JfexCellMapping needs the "unit number", ie U1-U4 not
         // the FPGA "processor" number as in the readout. Additionally
         // the readout uses 0-3 whereas JfexCellMapping expects 1-4.
         // We also want the fibre number. The (FPGA,chan) JfexCellMapping
         // constructor returns that (numbered with the whole module).
         // NB per FPGA minipod (AVR) 1, 2, 4 & 5 are full input data.
         // Minipod 3 is the overlap one, also used for TTC, IPbus, etc.
         // The channel (MGT) numbering in the readout is 0-59 which are
         // the direct fibres. But in firmware and mappings 0-59 are PMA
         // loopback channels. The direct fibres are actually 60-119.
         // But for the moment the PMA loopback inputs are not read out
         // so, except for the mapping lookup, we use 0-59.
         int unitNumber = JfexDefs::processorNumberToUnitNumber(fpgaNumber+1);
         JfexHardwareInfo hwInfoFpga( JfexCellMapping( unitNumber, chanNumber+60 ).getHardwareInfo() );
         int mgtFibreNumber = hwInfoFpga.getFibreNumber();
         JfexCellMapping mapping( jfexNumber, unitNumber, mgtFibreNumber, iTower );
         L1CaloDetectorRegion region = mapping.getDetectorRegion();
         JfexHardwareInfo hwInfo( mapping.getHardwareInfo() );
         JfexTriggerTowerInfo ttInfo( mapping.getTriggerTowerInfo() );
         
         if ( region.getValidity() && hwInfo.getValidity() ) {
            int layer = (region.getLayer() == L1CaloDetectorRegion::Electromagnetic) ? 0 : 1;
            int localEta = region.getEtaIndex() - moduleEta;
            int localPhi = region.getPhiIndex();

            L1CaloRdoJfexTower newOne( shelfNumber, jfexNumber, localEta, localPhi, layer, region );
            newOne.setRodInfo( rodInfo );
            L1CaloRdoJfexTower& rdo = L1CaloBsDecoderUtil::findRdo( newOne, tower );

            rdo.setHardwareInfo( fpgaNumber, chanNumber, iTower,
                                 hwInfo.getAvr(), hwInfo.getFibreNumber(),
                                 ttInfo.isCore() );
            rdo.setValue( towerEt );
            rdo.setFlag( errorMask );
            
            if ( m_verbosity > 0 )
            {
               std::cout << "L1CaloBsDecoderRun3::decodeJfexDataChan: "
                         << "module=" << jfexNumber
                         << std::hex << ", Et=0x" << towerEt << ", flag=0x" << errorMask
                         << std::dec << std::endl;
            }
            //std::cout << "L1CaloBsDecoderRun3::addJfexTower: fpga=" << fpgaNumber
            //          << ", chan=" << chanNumber << ", word=" << iTower
            //          << ", unit=" << unitNumber
            //          << ", mpod=" << hwInfo.getAvr()
            //          << ", modFibre=" << hwInfo.getFibreNumber()
            //          << ", mgtFibre=" << mgtFibreNumber
            //          << ", regionEta=" << localEta
            //          << ", regionPhi=" << localPhi
            //          << ", layer=" << layer
            //          << ", phiFpga=" << (localPhi/16)
            //          << ", hwFpga=" << hwInfo.getFpgaNumber()
            //          << ", validity=" << region.getValidity()
            //          << ", Et=" << towerEt
            //          << ", errors=0x" << std::hex << errorMask << std::dec
            //          << std::endl;
         }
      }
   }
   
   return errorBits;
}

/*!
 * Decode jFEX TOBs and XTOBs.
 * The RDO value encodes the cluster Et in the value field
 * and the isolation bits and other information in the flag.
 * \param beg pointer to start of rod data payload
 * \param end pointer to end of rod data payload
 * \param tob list of RDO to be filled
 * \param rodInfo iterator to ROD information for this block
 */
void
L1CaloBsDecoderRun3::decodeJfexTobs( const uint32_t* beg, const uint32_t* end,
                                     std::list<L1CaloRdoJfexTob>& tob,
                                     std::list<L1CaloRdoRodInfo>::const_iterator rodInfo )
{
   const uint32_t* payload( beg );
   const size_t fragmentSize = end - beg;
   
   // The data block is optimised for production by firmware as a set
   // of nested blocks with the lengths at the end. So we need to find
   // the end and work backwards.

   if ( fragmentSize < 2 ) {
      std::cerr << "L1CaloBsDecoderRun3::decodeJfexTobs: fragment size " << fragmentSize
                << " is too small for the ROD trailer" << std::endl;
      return;
   }
   
   size_t index = fragmentSize;
   const uint32_t rodTrailer2 = payload[--index];
   const uint32_t rodTrailer1 = payload[--index];
   
   const uint32_t rodErrors = rodTrailer2 & 0x7f;
   const size_t payloadSize = rodTrailer1 & 0xffff;
   if ( (payloadSize + 2) != fragmentSize ) {
      // Actual ROD fragment payload size does not match that claimed in the trailer.
      std::cerr << "L1CaloBsDecoderRun3::decodeJfexTobs: payload size " << payloadSize
                << " inconsistent with ROD fragment size " << fragmentSize << std::endl;
      return;
   }
   //??const uint32_t rodShelfNumber = (rodTrailer1 >> 18) & 0x3;
   
   // Loop looking backwards for jFEX FPGA blocks.
   while ( index > 0 ) {
      if ( index < 2 ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeJfexTobs: remaining block size " << index
                   << " is too small for the jFEX trailer" << std::endl;
         return;
      }
      size_t fpgaIndex = index;
      const uint32_t fpgaTrailer2 = payload[--fpgaIndex];
      const uint32_t fpgaTrailer1 = payload[--fpgaIndex];
      const size_t fpgaBlockSize  = fpgaTrailer1 & 0xffff;
      const uint32_t jfexNumber   = (fpgaTrailer1 >> 20) & 0x7;
      const uint32_t fpgaNumber   = (fpgaTrailer1 >> 18) & 0x3;
      const uint32_t numSlices    = (fpgaTrailer1 >> 24) & 0xf;
      const uint32_t sliceNumber  = (fpgaTrailer1 >> 28) & 0xf;
      const uint32_t fpgaErrors   = fpgaTrailer2 & 0x3f;
      if ( fpgaBlockSize > index ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeJfexTobs: jFEX FPGA block size "
                   << fpgaBlockSize << " exceeds remaining data size " << index
                   << " (jFEX " << jfexNumber << " FPGA " << fpgaNumber << ")" << std::endl;
         return;
      }
      // Update index to previous jFEX FPGA block (if any).
      index = fpgaIndex - fpgaBlockSize;
      
      // Combine rod and jfex error fields.
      const uint32_t errorMask = fpgaErrors | (rodErrors << 6);
      
      bool ret = this->decodeJfexTobSlice ( payload, fpgaBlockSize, fpgaIndex, jfexNumber, fpgaNumber,
                                            sliceNumber, numSlices, errorMask, tob, rodInfo );
      if ( ! ret ) {
         return;
      }
   }
}

/*!
 * Decode one jFEX FPGA block of TOBs and XTOBs for one slice.
 * \param payload entire ROD fragment payload vector
 * \param index just past end of block for this slice
 *        (this method updates it to the start of the block)
 * \param jfexNumber number of this jFEX in its shelf
 * \param fpgaNumber FPGA number
 * \param sliceNumber number of this readout slice
 * \param numSlices total number of TOB slices read out in this event
 * \param errorMask global error bits set for this ROD fragment
 * \param tob list of RDO to be filled
 * \param rodInfo iterator to ROD information for this block
 * \return whether decoding succeeded (false to abort decoding
           in which case the returned index is not valid)
 */
bool
L1CaloBsDecoderRun3::decodeJfexTobSlice( const uint32_t payload[], size_t blockSize, size_t& index,
                                         const uint32_t jfexNumber, const uint32_t fpgaNumber,
                                         const uint32_t sliceNumber, const uint32_t numSlices,
                                         const uint32_t errorMask,
                                         std::list<L1CaloRdoJfexTob>& tob,
                                         std::list<L1CaloRdoRodInfo>::const_iterator rodInfo )
{
   if ( index < 2 ) {
      return false;
   }
   
   // **FIXME** TEMP Ignore jfex00 and jfex05 due to data corruption.
   if ( jfexNumber == 0 || jfexNumber == 5 ) {
      return true;
   }

   const uint32_t countTrailer1 = payload[index-2];
   const uint32_t countTrailer2 = payload[index-1];
   
   // **FIXME** Not sure if jFEX will send corrective trailers?
   const uint32_t corrective = 0;
   if ( corrective ) {
      // **FIXME** To be implemented if required.
   }
   else {
      const uint32_t safeMode      = (countTrailer1      ) & 0x1;
      const uint32_t numSJetTobs   = (countTrailer1 >>  1) & 0x3f;
      const uint32_t numLJetTobs   = (countTrailer1 >>  7) & 0x3f;
      const uint32_t numTauTobs    = (countTrailer1 >> 13) & 0x3f;
      const uint32_t numElecTobs   = (countTrailer1 >> 19) & 0x3f;
      const uint32_t numSumEtTobs  = (countTrailer1 >> 25) & 0x1;
      const uint32_t numMissEtTobs = (countTrailer1 >> 26) & 0x1;
      const uint32_t numSJetXtobs  = (countTrailer2 >>  1) & 0x3f;
      const uint32_t numLJetXtobs  = (countTrailer2 >>  7) & 0x3f;
      const uint32_t numTauXtobs   = (countTrailer2 >> 13) & 0x3f;
      const uint32_t numElecXtobs  = (countTrailer2 >> 19) & 0x3f;

      // Work out how long this block should be. For jFEX both TOBs
      // and XTOBs are one word. Apart from the counts of small jets,
      // large jets, taus and (forward) electrons there are two TOBs
      // for SumEt and MissingEt.
      // If the total is odd there should be an extra zero padding word.
      // However in safe mode all the TOBs and XTOBs are suppressed
      // and we only have the trailer with the counts.
      
      // For ease of later decoding, fill a vector with the end index
      // of each type of TOB/XTOB word.
      std::vector<size_t> end;
      end.push_back( 0 ) ;
      if ( !safeMode ) {
         end.push_back( end.back() + numSJetTobs );
         end.push_back( end.back() + numLJetTobs );
         end.push_back( end.back() + numTauTobs );
         end.push_back( end.back() + numElecTobs );
         end.push_back( end.back() + numSumEtTobs );
         end.push_back( end.back() + numMissEtTobs );
         end.push_back( end.back() + numSJetXtobs );
         end.push_back( end.back() + numLJetXtobs );
         end.push_back( end.back() + numTauXtobs );
         end.push_back( end.back() + numElecXtobs );
      }
      
      size_t tobSize = end.back();    // End of TOBs/XTOBs
      tobSize += (tobSize % 2);       // Possible padding
      tobSize += 2;                   // Two count words

      if ( tobSize != blockSize ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeJfexTobSlice: TOB slice " << sliceNumber
                   << " has block size " << blockSize << " expected TOBs+counts " << tobSize
                   << " (jFEX " << jfexNumber << " FPGA " << fpgaNumber << ")" << std::endl;
      }
      if ( tobSize > index ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeJfexTobSlice: TOB size " << tobSize
                   << " is larger than index " << index << std::endl;
         return false;
      }
      
      size_t numTobs   = (safeMode) ? 0 : end[6];      // End index of MissEt TOBs
      size_t totalTobs = (safeMode) ? 0 : end.back();  // End index of TOBs+XTOBs

      // Move index to start of this block.
      index -= tobSize;

      // Luxury, we can now work forwards inside the slice block!
      size_t sliceIndex = index;
      L1CaloRdoFexTob::TobType fexTobType;
      L1CaloRdoFexTob::TobSource fexTobSource;
      for (size_t iTOB = 0; iTOB < totalTobs; iTOB++) {
         // Small jets, taus and electron TOBs and XTOBs
         // all have similar structure so extract common
         // values and override for other TOB types.
         // TOBs and xTOBs now have identical formats
         // so first find the TOB type, then process
         // the values afterwards.
         const uint32_t tobWord  = payload[sliceIndex++];
         uint32_t flagInfo = (tobWord >> 21) & 0xfff;
         uint32_t etValue  = (tobWord >> 10) & 0x7ff;
         uint32_t tobPhi   = (tobWord >>  1) &   0xf;
         uint32_t tobEta   = (tobWord >>  5) &  0x1f;
         fexTobSource = (iTOB < numTobs)
                      ? L1CaloRdoFexTob::TobSource::JfexTob
                      : L1CaloRdoFexTob::TobSource::JfexXtob;
         if        (iTOB < end[1]) {
            fexTobType = L1CaloRdoFexTob::TobType::SmallJet;
         } else if (iTOB < end[2]) {
            fexTobType = L1CaloRdoFexTob::TobType::LargeJet;
         } else if (iTOB < end[3]) {
            fexTobType = L1CaloRdoFexTob::TobType::Tau;
         } else if (iTOB < end[4]) {
            fexTobType = L1CaloRdoFexTob::TobType::EM;
         } else if (iTOB < end[6]) {
            fexTobType = L1CaloRdoFexTob::TobType::Energy;
         } else if (iTOB < end[7]) {
            fexTobType = L1CaloRdoFexTob::TobType::SmallJet;
         } else if (iTOB < end[8]) {
            fexTobType = L1CaloRdoFexTob::TobType::LargeJet;
         } else if (iTOB < end[9]) {
            fexTobType = L1CaloRdoFexTob::TobType::Tau;
         } else if (iTOB < end[10]) {
            fexTobType = L1CaloRdoFexTob::TobType::EM;
         } else {
            // Padding word: skip.
            continue;
         }

         if        (fexTobType == L1CaloRdoFexTob::TobType::SmallJet) {
            flagInfo = 0;  // None defined
         } else if (fexTobType == L1CaloRdoFexTob::TobType::LargeJet) {
            flagInfo = 0;  // None defined
            etValue  = (tobWord >> 10) & 0x1fff;
         } else if (fexTobType == L1CaloRdoFexTob::TobType::Energy) {
            // **FIXME** SumEt and MissEt to be implemented!
            flagInfo = (tobWord & 0x1) | (tobWord & 0x1000) >> 15;
            etValue  = (tobWord & 0x7ffffffe) >> 1;
            tobPhi = 0;
            tobEta = 0;
         }

         // **FIXME** REMOVE THIS TEMPORARY HACK ASAP!!
         // **FIXME** Ignore & fix  wrong slice numbers.
         uint32_t sliceNumberHacked = sliceNumber;
         if (numSlices == 1 && sliceNumber == 2) {
           sliceNumberHacked = 0;
         }
         //>>if ( sliceNumber >= numSlices ) {
         if ( sliceNumberHacked >= numSlices ) {
            std::cerr << "L1CaloBsDecoderRun3::decodeJfexTobSlice: TOB slice " << sliceNumber
                      << " exceeds number of slices " << numSlices << " in processor trailer"
                      << std::endl;
         }
         else if ( etValue || flagInfo ) {
            // Should already be zero suppressed in the readout
            // but no harm in making doubly sure.
            // Add the error bits from the jFEX control FPGA (low 6 bits)
            // and the error bits from the ROD output packet (low 7 bits)
            // to the isolation bits (shifted up 16 bits).
            // This probably needs more sophisticated treatment.
            // **FIXME** Check use of tobEta and adjusted tobPhi by FPGA.
            const uint32_t modulePhi = tobPhi + 16 * fpgaNumber;
            const uint32_t flagMask = (flagInfo << 16) | errorMask;
            const uint32_t shelfNumber = 0x22;   // Hard code to P1 value (34)
            L1CaloRdoJfexTob newOne( shelfNumber, jfexNumber, tobEta, modulePhi,
                                     numSlices, fexTobType, fexTobSource );
            newOne.setRodInfo( rodInfo );
            L1CaloRdoJfexTob& rdo = L1CaloBsDecoderUtil::findRdo( newOne, tob );
            //>>rdo.setValue( etValue, sliceNumber );
            //>>rdo.setFlag( flagMask, sliceNumber );
            rdo.setValue( etValue, sliceNumberHacked );
            rdo.setFlag( flagMask, sliceNumberHacked );
            if ( m_verbosity > 0 )
            {
               std::cout << "L1CaloBsDecoderRun3::decodeJfexTobSlice: tobType=" << fexTobType
                         << ", tobSource=" << fexTobSource << ", slice=" << sliceNumberHacked
                         << ", module=" << jfexNumber << ", fpga=" << fpgaNumber
                         << ", eta=" << tobEta << ", phi=" << modulePhi
                         << std::hex << ", Et=0x" << etValue << ", flag=0x" << flagMask
                         << std::dec << ", numSlices=" << numSlices << std::endl;
            }
         }
      }
   }
   return true;
}

/*!
 * Decode gFEX input fibre data.
 * We create one RDO per tower using the value field for its Et.
 * The flag field is used for status/error bits.
 * \param beg pointer to start of rod data payload
 * \param end pointer to end of rod data payload
 * \param tower list of RDO to be filled
 * \param rodInfo iterator to ROD information for this block
 */
void
L1CaloBsDecoderRun3::decodeGfexData( const uint32_t* beg, const uint32_t* end,
                                     std::list<L1CaloRdoGfexTower>& tower,
                                     std::list<L1CaloRdoRodInfo>::const_iterator rodInfo )
{
   const uint32_t* payload( beg );
   
   // The data block consists of up to three sections, one per processor FPGA.
   // Each block starts with a one word header that identifies the FPGA and has
   // the block length and a few other fields.
   
   size_t numTowers = tower.size();
   
   // Loop looking backwards for jFEX processor input data blocks.
   while ( payload < end ) {
      const uint32_t word = *payload++;
      
      // The first word must be the header.
      
      const uint32_t fpgaCode = ( word >> 28 ) & 0xf;
      if ( fpgaCode < 0xa || fpgaCode > 0xc ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeGfexData: invalid FPGA code 0x"
                   << std::hex << fpgaCode << std::dec << std::endl;
         return;
      }
      const uint32_t fpgaNumber = fpgaCode - 0xa;   // A=0, B=1, C=2
      const uint32_t headerVer = ( word >> 24 ) & 0xf;
      const uint32_t headerLen = ( word >> 22 ) & 0x3;
      if ( headerVer > 1 || headerLen > 1 ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeGfexData: header version " << headerVer
                   << " or length " << headerLen << " is not yet supported" << std::endl;
         return;
      }
      const uint32_t truncatedFlag = (word >> 12) & 0x1;
      if ( truncatedFlag ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeGfexData: WARNING data truncated" << std::endl;
      }
      const uint32_t numFpgaWords = word & 0xfff;
      if ( ( numFpgaWords % 7 ) != 0 ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeGfexData: input data size " << numFpgaWords
                   << " is not a multiple of 7" << std::endl;
         return;
      }
      const uint32_t numInputFibres = numFpgaWords / 7;
      
      // Loop over the input fibres and decode each block of 7 words.
      // **FIXME** Not sure if gFEX data will have error flags?
      // **FIXME** For the moment hard code to zero.
      
      const uint32_t fpgaErrors = 0;
      
      for ( size_t chanNumber = 0; chanNumber < numInputFibres; chanNumber++ ) {
         // Ignore the spare fibres for the moment.
         if ( chanNumber < 48 || chanNumber >= 52 ) {
            this->decodeGfexDataChan ( payload, fpgaNumber, chanNumber,
                                       fpgaErrors, tower, rodInfo );
         }
         payload += 7;
      }
   }
   if ( m_verbosity > 0 )
   {
      std::cout << "L1CaloBsDecoderRun3::decodeGfexData: n.towers added="
                << tower.size() - numTowers << std::endl;
   }
}

/*!
 * Decode the data from one gFEX input fibre (only ever one slice).
 * \param payload payload vector starting at this 7 word block
 * \param fpgaNumber FPGA number (0-2 for A-C)
 * \param errorMask global error bits set for this ROD fragment
 * \param tower list of RDO to be filled
 * \param rodInfo iterator to ROD information for this block
 * \return whether decoding succeeded (false to abort decoding)
 */
uint32_t
L1CaloBsDecoderRun3::decodeGfexDataChan( const uint32_t payload[],
                                         const uint32_t fpgaNumber,
                                         const uint32_t chanNumber,
                                         const uint32_t errorMask,
                                         std::list<L1CaloRdoGfexTower>& tower,
                                         std::list<L1CaloRdoRodInfo>::const_iterator rodInfo )
{
   const uint32_t shelfNumber = 0x23;   // Hard code to P1 value (35)
   const uint32_t gfexNumber = 0;       // **CHECK** What is used in COOL/OKS?
   
   // Ignore the spare fibres for the moment.
   if ( chanNumber >= 48 && chanNumber < 52 ) {
      return 0;
   }
   
   // Ignore FPGA C for the moment (different format).
   if ( fpgaNumber >= 2 ) {
      return 0;
   }
   
   // The fibre packer decoder methods expect a vector of seven words.
   std::vector<FibrePackerBase::myDataWord> encodedData;
   for ( size_t i = 0; i < 7; i++ ) {
      encodedData.push_back( payload[i] );
   }
   const FibrePackerBase::InputDataFrameType frameType( FibrePackerBase::InputDataFrameType::Normal );
   
   // Unpack up to sixteen gCaloTowers. NB central FPGAs only have eight
   // Et values per EM fibre but additionally have eight fine positions.
   // For the moment we ignore the latter.
   // We need the central Latome decoder for FPGAs A&B for both EM layer
   // and hadronic layer outside the central eight 0.2 eta bins (to 1.6).
   // The hadronic fibres within |eta|<1.6 are TREX.
   // However due to overlaps and potentially unused words on fibres
   // its safer to decode in all possible ways and work out which one
   // to use afterwards.
   GfexTrexFibrePacker trexPacker;
   GfexLatomeCentralFibrePacker clarPacker;
   GfexLatomeForwardFibrePacker flarPacker;
   std::vector<FibrePackerBase::myDataWord> trexTowers = trexPacker.getUnpackedData( encodedData, frameType );
   std::vector<FibrePackerBase::myDataWord> clarTowers = clarPacker.getUnpackedData( encodedData, frameType );
   std::vector<FibrePackerBase::myDataWord> flarTowers = flarPacker.getUnpackedData( encodedData, frameType );
   size_t numTowers = GfexDefs::maxGTowersPerFibre();
   size_t numCentral = numTowers / 2;  // 8

   // Create RDO per tower.
   for ( size_t iTower = 0; iTower < numTowers; iTower++ ) {
      GfexCellMapping mapping( 0, fpgaNumber, chanNumber, iTower );
      L1CaloDetectorRegion region = mapping.getDetectorRegion();
      GfexHardwareInfo hwInfo( mapping.getHardwareInfo() );
      if ( !region.getValidity() || !hwInfo.getValidity() ) {
         continue;
      }
      // Currently GfexCellMapping sets local eta,phi within each FPGA.
      // We want to convert to global values (whole module or system).
      int globalPhi = region.getPhiIndex() * 2;
      int globalEta = GfexDefs::localToGlobalEta(fpgaNumber,region.getEtaIndex());
      if (globalEta < -49 || globalEta >= 49) {
        continue;   // Invalid or not yet implemented
      }
      int layer = (region.getLayer() == L1CaloDetectorRegion::Electromagnetic) ? 0 : 1;

      // Work out which decoding we ought to use.
      // For the moment avoid regions of potential confusion, eg Tile/HEC
      // overlap, anything outside |eta|>2.4, etc.
      // **FIXME** Ideally this should definitively come from the mapping.
      int towerEt = 0;
      if ( layer == 1 && std::abs(globalEta) < 14 ) {
         // TREX within Tile/HEC overlap.
         towerEt = trexTowers[iTower];
      }
      else if ( layer == 1 && std::abs(globalEta) >= 16 && std::abs(globalEta) < 24 ) {
         // HEC outside Tile/HEC overlap.
         towerEt = flarTowers[iTower];
      }
      else if ( layer == 0 && std::abs(globalEta) < 14 && iTower < numCentral ) {
         // EM layer within barrel/endcap overlap.
         towerEt = clarTowers[iTower];
      }
      else if ( layer == 0 && std::abs(globalEta) >= 16 && std::abs(globalEta) < 24 && iTower < numCentral ) {
         // EM layer outside barrel/endcap overlap.
         towerEt = clarTowers[iTower];
      }
      else {
         continue;
      }

      if ( towerEt || errorMask ) {

         L1CaloRdoGfexTower newOne( shelfNumber, gfexNumber, globalEta, globalPhi, layer, region );
         newOne.setRodInfo( rodInfo );
         L1CaloRdoGfexTower& rdo = L1CaloBsDecoderUtil::findRdo( newOne, tower );

         rdo.setHardwareInfo( hwInfo.getFpgaNumber(), chanNumber, iTower,
                              GfexDefs::minipodNumFromName(hwInfo.getMpodName()),
                              hwInfo.getMpodChannel() );
         rdo.setValue( towerEt );
         rdo.setFlag( errorMask );

         if ( m_verbosity > 0 )
         {
            std::cout << "L1CaloBsDecoderRun3::decodeGfexDataChan: "
                      << "FPGA=" << fpgaNumber
                      << std::hex << ", Et=0x" << towerEt << ", flag=0x" << errorMask
                      << std::dec << std::endl;
         }
         //std::cout << "L1CaloBsDecoderRun3::decodeGfexDataChan: fpga=" << fpgaNumber
         //          << ", chan=" << chanNumber << ", word=" << iTower
         //          << ", mpod=" << GfexDefs::minipodNumFromName(hwInfo.getMpodName())
         //          << ", mpodFibre=" << hwInfo.getMpodChannel()
         //          << ", globalEta=" << globalEta
         //          << ", globalPhi=" << globalPhi
         //          << ", layer=" << layer
         //          << ", Et=" << towerEt
         //          << ", errors=0x" << std::hex << errorMask << std::dec
         //          << std::endl;
      }
   }
   
   return 0;   // No error bits yet
}

/*!
 * Decode gFEX TOBs.
 * We create one RDO per TOB using the value field for its Et.
 * The flag field is used for status/error bits.
 * **FIXME** We also create one RDO for the energy TOBs.
 * \param beg pointer to start of rod data payload
 * \param end pointer to end of rod data payload
 * \param tob list of RDO to be filled
 * \param rodInfo iterator to ROD information for this block
 */
void
L1CaloBsDecoderRun3::decodeGfexTobs( const uint32_t* beg, const uint32_t* end,
                                     std::list<L1CaloRdoGfexTob>& tob,
                                     std::list<L1CaloRdoRodInfo>::const_iterator rodInfo )
{
  // **FIXME** To be implemented!
   const uint32_t* payload( beg );
   const size_t fragmentSize = end - beg;
   
   // The data block is constructed by the SWROD. It consists of up to six
   // subblocks for Jets and MET TOBs from each of the three FPGAs.
   // Each subblock starts with a 1 word header which identifies the block
   // type and gives its length.
   // Each subblock contains two sets of seven data words that are (or for
   // MET could be) transmitted to Ph1Topo on the fibres. If multiple slices
   // are read out, each subblock should have multiples of 14 words.
   
   size_t index = 0;
   while ( index < fragmentSize ) {
      const uint32_t headerWord = payload[index];
      const uint32_t blockType  = (headerWord >> 28) &    0xf;
      //const uint32_t version    = (headerWord >> 24) &    0xf;  // Currently unused (1)
      const uint32_t headerSize = (headerWord >> 22) &    0x3;
      const uint32_t errorFlags = (headerWord >> 12) &    0x1;
      const uint32_t dataSize   =  headerWord        &  0xfff;
      
      const uint32_t blockSize  = headerSize + dataSize;
      if ( (index + blockSize) > fragmentSize ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeGfexTobs: remaining block size "
                   << (fragmentSize - index)
                   << " is too small for subblock of type " << blockType
                   << " with headerSize " << headerSize
                   << " and dataSize " << dataSize << std::endl;
         return;
      }
      index += headerSize;
      
      const uint32_t wordsPerSlice = 2 * FexDefs::num32BitWordsPerFibre();
      const uint32_t numSlices = dataSize / wordsPerSlice;
      if ( numSlices * wordsPerSlice != dataSize ) {
         std::cerr << "L1CaloBsDecoderRun3::decodeGfexTobs: subblock type " << blockType
                   << " with dataSize " << dataSize
                   << " is not a multiple of " << wordsPerSlice << " words"
                   << std::endl;
         return;
      }
      
      // Create TOB RDOs for each slice.
      for (size_t sliceNumber = 0; sliceNumber < numSlices; sliceNumber++) {
         bool ret = this->decodeGfexTobSlice( &payload[index], blockType, sliceNumber, numSlices,
                                              errorFlags, tob, rodInfo );
         if ( ! ret ) {
            return;
         }
         index += wordsPerSlice;
      }
   }
}

/*!
 * Decode one gFEX FPGA block of TOBs for one slice.
 * See https://edms.cern.ch/ui/file/1492098/1/L1CaloTOBFormats_v010.pdf
 * \param payload pointer to this slice in ROD fragment payload
 * \param blockType code for Jet or MET TOB
 * \param sliceNumber number of this readout slice
 * \param numSlices total number of TOB slices read out in this event
 * \param errorFlags global error bits set for this subblock (or ROD fragment?)
 * \param tob list of RDO to be filled
 * \param rodInfo iterator to ROD information for this block
 * \return whether decoding succeeded (false to abort decoding)
 */
bool
L1CaloBsDecoderRun3::decodeGfexTobSlice( const uint32_t payload[], uint32_t blockType,
                                         const uint32_t sliceNumber, const uint32_t numSlices,
                                         const uint32_t errorMask,
                                         std::list<L1CaloRdoGfexTob>& tob,
                                         std::list<L1CaloRdoRodInfo>::const_iterator rodInfo )
{
   // The subblock type is 0xA,B,C for jet TOBs from FPGA A,B,C
   // and 0x1,2,3 for global (MET) TOBs.
   bool isMet = (blockType >= 0x1 && blockType <= 0x3);
   bool isJet = (blockType >= 0xA && blockType <= 0xC);
   if ( !isJet && !isMet ) {
      std::cerr << "DecoderRun3::decodeGfexTobSlice: invalid block type "
                << blockType << std::endl;
      return false;
   }
   const uint32_t fpgaNumber = (isJet) ? (blockType - 0x1) : (blockType - 0xA);
   
   // We expect two fibres each of 7 words.
   size_t index = 0;
   for ( size_t iFibre = 0; iFibre < 2; iFibre++ ) {
      for ( size_t iWord = 0; iWord < (size_t)FexDefs::num32BitWordsPerFibre(); iWord++, index++ ) {
         // Jet TOBs.
         if (isJet) {
            const uint32_t tobID     =  payload[index]        &  0x1f;
            const uint32_t status    = (payload[index] >>  7) &   0x1;
            const uint32_t etValue   = (payload[index] >>  8) & 0xfff;
            const uint32_t tobEta    = (payload[index] >> 20) &  0x3f;
            const uint32_t tobPhi    = (payload[index] >> 26) &  0x1f;
            const uint32_t saturated = (payload[index] >> 31) &   0x1;
            
            L1CaloRdoFexTob::TobType fexTobType( L1CaloRdoFexTob::TobType::Invalid );
            L1CaloRdoFexTob::TobSource fexTobSource( L1CaloRdoFexTob::TobSource::GfexTob );
            if ( tobID >= 1 && tobID <= 4 ) {
               fexTobType = L1CaloRdoFexTob::TobType::SmallJet;
            }
            else if ( tobID >= 5 && tobID <= 6 ) {
               fexTobType = L1CaloRdoFexTob::TobType::LargeJet;
            }
            
            if ( ( etValue || saturated || errorMask ) && tobID ) {
               // Zero suppress (as gFEX always sends something).
               // **FIXME** Not sure what to do with status or satured bits.
               // **FIXME** For the moment set them in the error flags
               // **FIXME** Check use of tobEta and tobPhi (is it per FPGA?).
               const uint32_t flagMask = errorMask | (saturated << 31) | (status << 30);
               const uint32_t shelfNumber = 0x23;   // Hard code to P1 value (35)
               const uint32_t moduleNumber = 0;     // Hard code to P1 value (0) **FIXME** CHECK!
               L1CaloRdoGfexTob newOne( shelfNumber, moduleNumber, tobEta, tobPhi,
                                        numSlices, fexTobType, fexTobSource );
               newOne.setRodInfo( rodInfo );
               L1CaloRdoGfexTob& rdo = L1CaloBsDecoderUtil::findRdo( newOne, tob );
               rdo.setValue( etValue, sliceNumber );
               rdo.setFlag( flagMask, sliceNumber );
               if ( m_verbosity > 0 )
               {
                  std::cout << "L1CaloBsDecoderRun3::decodeGfexTobSlice: tobType=" << fexTobType
                            << ", tobSource=" << fexTobSource << ", slice=" << sliceNumber
                            << ", fpga=" << fpgaNumber << ", eta=" << tobEta << ", phi=" << tobPhi
                            << std::hex << ", Et=0x" << etValue << ", flag=0x" << flagMask
                            << std::dec << ", numSlices=" << numSlices << std::endl;
               }
            }
         }
         
         // MET TOBs. **FIXME** To be implemented!
         else if (isMet) {
         }
      }
   }
   
   return true;
}

/*!
 * Decode Ph1Topo input data: these are TOBs from FEXes and MuCTPI.
 * We create one RDO per TOB using the value field for its Et.
 * The flag field is used for status/error bits.
 * \param beg pointer to start of rod data payload
 * \param end pointer to end of rod data payload
 * \param etob list of eFEX TOB RDOs to be filled
 * \param jtob list of jFEX TOB RDOs to be filled
 * \param gtob list of gFEX TOB RDOs to be filled
 * \param mtob list of muon TOB RDOs to be filled
 * \param rodInfo iterator to ROD information for this block
 */
void
L1CaloBsDecoderRun3::decodePh1TopoData( const uint32_t* beg, const uint32_t* end,
                                        std::list<L1CaloRdoEfexTob>& etob,
                                        std::list<L1CaloRdoJfexTob>& jtob,
                                        std::list<L1CaloRdoGfexTob>& gtob,
                                        std::list<L1CaloRdoMuonTob>& mtob,
                                        std::list<L1CaloRdoRodInfo>::const_iterator rodInfo )
{
   const uint32_t* payload( beg );
   const size_t fragmentSize = end - beg;
   
   // Fibre types of Ph1Topo inputs. These are defined in the VHDL at:
   // https://gitlab.cern.ch/atlas-l1calo/l1topo/ph1topo/-/blob/master/src/infr/topo_mapping_pkg.vhd#L27
   // For our purposes we only need to distinguish different formats
   // and ignore those that do not appear in the readout.
   enum TopoInputFibreTypes { Unknown=0x00, IPB=0x01, TTC=0x02, EM1=0x03, EM2=0x04,
                              G1=0x05, G2=0x06, J1=0x07, J2=0x08, JF=0x09, JFXE=0x0a,
                              M0A=0x0b, M0C=0x0c, M1A=0x0d, M1C=0x0e, M2A=0x0f, M2C=0x10,
                              TAU1=0x11, TAU2=0x12, gJ1=0x13, gJ2=0x14 };
   
   // The data block is optimised for production by firmware with the length
   // in a trailer at the end. The SWROD concatenates a number of such blocks
   // (removing the 2 word header used to fill the ROD fragment header).
   // So we need to find the end and work backwards.
   
   //??size_t numTobs = tower.size();
   size_t index = fragmentSize;
   
   // Loop looking backwards for Ph1Topo processor input data blocks.
   while ( index > 0 ) {
      if ( index < 2 ) {
         std::cerr << "L1CaloBsDecoderRun3::decodePh1TopoData: remaining block size " << index
                   << " is too small for the Ph1Topo FPGA trailer" << std::endl;
         return;
      }
      const uint32_t fpgaTrailer2 = payload[--index];
      const uint32_t fpgaTrailer1 = payload[--index];

      const uint32_t fpgaErrors  =  fpgaTrailer2 & 0x3f;
      //??const uint32_t sliceNum    = (fpgaTrailer1 >> 28) & 0xf;
      //??const uint32_t numSlices   = (fpgaTrailer1 >> 24) & 0xf;
      const uint32_t sliceNum    = 0;  // **FIXME** Temporarily hard code until set properly
      const uint32_t numSlices   = 1;  // **FIXME** Temporarily hard code until set properly
      //>>const uint32_t topoNumber  = (fpgaTrailer1 >> 22) & 0x3;
      //>>const uint32_t fpgaNumber  = (fpgaTrailer1 >> 21) & 0x1;
      //>>const uint32_t fwType      = (fpgaTrailer1 >> 19) & 0x3; // 0: Mult, 1: Algo, 2: Beta
      const size_t   payloadSize = (fpgaTrailer1 & 0xffff);
      
      if ( payloadSize > index ) {
         std::cerr << "L1CaloBsDecoderRun3::decodePh1TopoData: remaining Ph1Topo block size "
                   << index << " is too small for the claimed payload size "
                   << payloadSize << std::endl;
         return;
      }
      
      // We can now work forwards from the start of this block
      // decoding each set of 8 words per input fibre channel.
      // In Ph1Topo the input data channels can be TOBs of many
      // different types. The type is identifed in the 8th word.
      // We then need the appropriate mapping to give us which
      // P1 FEX crate and module number is the source.
      // NB this may be incorrect for test rig setups.
      index -= payloadSize;
      size_t chanIndex = 0;
      uint32_t fexShelf = 0;
      uint32_t fexModule = 0;
      uint32_t fexFpga = 0;
      while ( chanIndex < payloadSize ) {
         if ( (payloadSize - chanIndex) < 8 ) {
            std::cerr << "L1CaloBsDecoderRun3::decodePh1TopoData: remaining Ph1Topo block size "
                      << (payloadSize - chanIndex)
                      << " is too small for one Ph1Topo input fibre block (8)" << std::endl;
            return;
         }
         //>>const uint32_t chanNumber = ( payload[7]       ) & 0xff;
         const uint32_t fibreType  = ( payload[7] >>  8 ) & 0x1f;
         //>>const uint32_t errorBits  = ( payload[7] >> 30 ) &  0x3;
         
         L1CaloRdoFexTob::TobType tobType(L1CaloRdoFexTob::TobType::Invalid);
         L1CaloRdoFexTob::TobSource tobSource(L1CaloRdoFexTob::TobSource::Ph1Topo);
         
         // Most fibres have up to six TOBs, but jFEX fibres have seven.
         size_t numTobs = 6;
         if (fibreType == TopoInputFibreTypes::J1 || fibreType == TopoInputFibreTypes::JF ||
             fibreType == TopoInputFibreTypes::J2 || fibreType == TopoInputFibreTypes::JFXE) {
           numTobs = 7;
         }
         
         for (size_t iTob = 0; iTob < numTobs; iTob++) {
            // TOBs from eFEX.
            if (fibreType == TopoInputFibreTypes::EM1 || fibreType == TopoInputFibreTypes::TAU1 ||
                fibreType == TopoInputFibreTypes::EM2 || fibreType == TopoInputFibreTypes::TAU2) {
                tobType = (fibreType <= TopoInputFibreTypes::EM2)  // Assumes enum order!
                        ? L1CaloRdoFexTob::TobType::EM
                        : L1CaloRdoFexTob::TobType::Tau;
               // Need mapping!
               this->decodeOneEfexTob( &payload[index+chanIndex+iTob], fexShelf, fexModule, fexFpga,
                                       fpgaErrors, numSlices, sliceNum, tobType, tobSource,
                                       etob, rodInfo );
            }
            // Fibres from jFEX.
            /*
            else if () {
            }
            */
         }
         chanIndex += 8;
      }
   }
   //??if ( m_verbosity > 0 )
   //??{
   //??   std::cout << "L1CaloBsDecoderRun3::decodeJfexData: n.towers added="
   //??             << tower.size() - numTowers << std::endl;
   //??}
}
#endif // OFFLINE
/*!
 * Decode word(s) for one eFEX TOB (or xTOB) and create one RDO.
 * **FIXME** Document other parameters when stable!
 * \param tob list of RDO to be filled
 * \param rodInfo iterator to ROD information for this block
 */
void
L1CaloBsDecoderRun3::decodeOneEfexTob( const uint32_t word[], const uint32_t shelfNumber,
                                       const uint32_t efexNumber, const uint32_t/* fpgaNumber*/,
                                       const uint32_t errorMask,
                                       const uint32_t numSlices, const uint32_t sliceNum,
                                       L1CaloRdoFexTob::TobType tobType,
                                       L1CaloRdoFexTob::TobSource tobSource,
                                       std::list<L1CaloRdoEfexTob>& tob,
                                       std::list<L1CaloRdoRodInfo>::const_iterator rodInfo )
{
   const uint32_t tobWord  = word[0];
   const uint32_t isolInfo = (tobWord      ) & 0xffc000;
   const uint32_t tobPhi   = (tobWord >> 24) &      0x7;
   const uint32_t tobEta   = (tobWord >> 27) &      0x7;
   const uint32_t tobFpga  = (tobWord >> 30) &      0x3;
   
   uint32_t etValue(0);
   if (tobSource == L1CaloRdoFexTob::TobSource::EfexXtob) {
      // XTOB: Et from second TOB word.
      etValue = word[1] & 0xffff;
   }
   else {  // EfexTob or Ph1Topo
      // TOB: Et from single TOB word.
      etValue = tobWord & 0xfff;
   }
   
   if ( sliceNum >= numSlices ) {
       ERS_ERROR("L1CaloBsDecoderRun3::decodeOneEfexTob: TOB slice " << sliceNum
                << " exceeds number of slices " << numSlices << " in processor trailer");
   }
   else if ( etValue ) {
      // Should already be zero suppressed in the readout
      // but no harm in making doubly sure.
      // Add the error bits from the eFEX control FPGA (low 6 bits)
      // and the error bits from the ROD output packet (low 7 bits)
      // to the isolation bits (which have the low 14 bits unused).
      // This probably needs more sophisticated treatment.
      // The internal eta within one FPGA is in the range 0-5
      // where 1-4 are the standard values and 0 & 5 are only
      // used at the extreme eta, ie |eta| > 2.4.
      // To get an eta within the module we use the range 0-17
      // where 0 & 17 are only used at the extremes.
      const uint32_t moduleEta = 1 + (tobEta - 1) + 4 * tobFpga;
      const uint32_t flagMask = isolInfo | errorMask;
      L1CaloRdoEfexTob newOne( shelfNumber, efexNumber, moduleEta, tobPhi,
                               numSlices, tobType, tobSource );
      newOne.setRodInfo( rodInfo );
      L1CaloRdoEfexTob& rdo = L1CaloBsDecoderUtil::findRdo( newOne, tob );
      rdo.setValue( etValue, sliceNum );
      rdo.setFlag( flagMask, sliceNum );
#ifdef OFFLINE_DECODER
      // store the raw words - used in offline to construct the EDM objects
       rdo.setWord0( word[0], sliceNum );
      if(tobSource == L1CaloRdoFexTob::TobSource::EfexXtob) rdo.setWord1(word[1], sliceNum);
#endif
      if ( m_verbosity > 0 )
      {
         std::cout << "L1CaloBsDecoderRun3::decodeOneEfexTob: tobType=" << tobType
                   << ", tobSource=" << tobSource << ", slice=" << sliceNum
                   << ", shelf=" << shelfNumber << ", module=" << efexNumber
                   << ", eta=" << moduleEta << ", phi=" << tobPhi
                   << std::hex << ", Et=0x" << etValue << ", flag=0x" << flagMask
                   << std::dec << ", numSlices=" << numSlices << std::endl;
      }
   }
}

/*!
 * Check the CRC in an input fibre block of words.
 * The CRC field should be the top 9 bits in the 7 word block.
 * This means we calculate the CRC over the remaining 215 bits.
 * \param data vector of 7 input fibre words
 * \return true if the CRC is valid
 */
bool
L1CaloBsDecoderRun3::checkFibreCRC( std::vector<uint32_t>& data ) const
{
   const size_t numWords = FexDefs::num32BitWordsPerFibre();
   if ( data.size() != numWords ) {
      return false;
   }
   GenericCrc crc;
   const size_t numPayloadBits = ( 32 * numWords ) - 9;
   unsigned int actualCRC = ( data[numWords-1] >> 23 ) & 0x1ff;
   data[0] &= 0xffffff00;   // Zero the K character
   unsigned int expectCRC = crc.crc9fibre( data, numPayloadBits );
   
   return (actualCRC == expectCRC);
}
