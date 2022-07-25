#include "PixelByteStreamErrorDetectorElementStatusTool.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "PixelReadoutGeometry/PixelFEUtils.h"
#include "PixelConditionsData/PixelByteStreamErrors.h"

StatusCode PixelByteStreamErrorDetectorElementStatusTool::initialize()
{
   ATH_CHECK( PixelDetectorElementStatusToolBase::initialize() );

   bool useByteStream = (m_useByteStreamFEI4 || m_useByteStreamFEI3 || m_useByteStreamRD53);
   ATH_CHECK(m_BSErrContReadKey.initialize(useByteStream && !m_BSErrContReadKey.empty()));

   static_assert( static_cast<unsigned int>(InDetDD::PixelReadoutTechnology::FEI4) < 32u) ;
   static_assert( static_cast<unsigned int>(InDetDD::PixelReadoutTechnology::FEI3) < 32u) ;
   static_assert( static_cast<unsigned int>(InDetDD::PixelReadoutTechnology::RD53) < 32u) ;
   m_readoutTechnologyMask=  Pixel::makeReadoutTechnologyBit(InDetDD::PixelReadoutTechnology::FEI4, 1 && m_useByteStreamFEI4 )
      | Pixel::makeReadoutTechnologyBit(InDetDD::PixelReadoutTechnology::FEI3, 1 && m_useByteStreamFEI3 )
      | Pixel::makeReadoutTechnologyBit(InDetDD::PixelReadoutTechnology::RD53, 1 && m_useByteStreamRD53 );

   return StatusCode::SUCCESS;
}

namespace {
   inline constexpr bool isBSError(uint64_t error, uint64_t max_error, uint64_t mask) {
      return error<max_error && (error & mask);
   }

}

std::unique_ptr<InDet::SiDetectorElementStatus>
PixelByteStreamErrorDetectorElementStatusTool::getDetectorElementStatus(const EventContext& ctx,
                                                                        SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const  {
   std::unique_ptr<InDet::SiDetectorElementStatus> element_status( createDetectorElementStatus(ctx, whandle));
   std::vector<bool> &status=element_status->getElementStatus();
   if (status.empty()) {
      status.resize(m_pixelID->wafer_hash_max(),true );
   }
   std::vector<InDet::ChipFlags_t> &chip_status=element_status->getElementChipStatus();
   if (chip_status.empty()) {
      chip_status.resize(status.size(),0xffff);
   }

   if (whandle) {
     ATH_MSG_ERROR("PixelByteStreamErrorDetectorStatusTool is not for use with conditions objects");
     whandle->addDependency (IDetectorElementStatusTool::getInvalidRange());
   }

   if (m_readoutTechnologyMask) {
      SG::ReadHandle<IDCInDetBSErrContainer> idcErrCont(m_BSErrContReadKey, ctx);
      if (!idcErrCont.isValid()) {
         ATH_MSG_ERROR("Did not get bytestream error container with key " << m_BSErrContReadKey);
      }
      else {

         const IDCInDetBSErrContainer_Cache *idcCachePtr = idcErrCont->cache();

         constexpr uint64_t error_mask = PixelByteStreamErrors::makeError(PixelByteStreamErrors::TruncatedROB)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MaskedROB)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::Preamble)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::TimeOut)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::LVL1ID)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::BCID)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::Trailer)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCLVL1IDEoECheck)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCBCIDEoECheck)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCLVL1IDCheck)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCEoEOverflow)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCHitOverflow);

         constexpr uint64_t chip_error_mask = PixelByteStreamErrors::makeError(PixelByteStreamErrors::Preamble)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::TimeOut)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::LVL1ID)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::BCID)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::Trailer)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCLVL1IDEoECheck)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCBCIDEoECheck)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCLVL1IDCheck)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCEoEOverflow)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCHitOverflow);

         // The index array is defined in PixelRawDataProviderTool::SizeOfIDCInDetBSErrContainer()
         // Here, 52736 is a separator beween error flags and isActive flags.
         const bool active_only = m_activeOnly;
         unsigned int element_offset_i = (active_only ? 52736 : 0);

         unsigned int element_i=0;
         unsigned int maxHash = m_pixelID->wafer_hash_max();

         constexpr uint64_t missingErrorInfo{std::numeric_limits<uint64_t>::max()-3000000000};

         for (const InDetDD::SiDetectorElement *element : element_status->getDetectorElements()) {
            const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());
            InDetDD::PixelReadoutTechnology readout_technology = p_design->getReadoutTechnology();
            unsigned int readout_technology_flags = m_readoutTechnologyMask & Pixel::makeReadoutTechnologyBit(readout_technology);

            // set to false if has one of the considered errors and the readout technology is considered.
            status.at(element_i) = status.at(element_i) &&  not (   readout_technology_flags
                                                                &&  ( !active_only
                                                                     ?  isBSError(static_cast<uint64_t>(idcCachePtr->retrieve(element_i)), missingErrorInfo, error_mask)
                                                                      : idcCachePtr->retrieve(element_i+element_offset_i)!=1 ));

            // compute the status for the individual front-ends
            if (status[element_i]) {
               unsigned int number_of_chips = readout_technology == InDetDD::PixelReadoutTechnology::FEI3  ? 2*p_design->numberOfCircuits() : p_design->numberOfCircuits();
               InDet::ChipFlags_t  chip_mask = status[element_i] ? (1ul<<number_of_chips)-1ul : 0;
               assert( chip_mask != 0 );
               InDet::ChipFlags_t bs_status_flags = active_only ? chip_mask : 0;
               if (!active_only) {
                  for (unsigned int chip_i =0; chip_i < number_of_chips; ++chip_i) {
                     // get bytestream error for chip
                     unsigned int indexFE = (1+chip_i)*maxHash + element_i;    // (FE_channel+1)*2048 + moduleHash
                     InDet::ChipFlags_t chip_flag =not (    readout_technology_flags
                                                        && (isBSError(static_cast<uint64_t>(idcCachePtr->retrieve(indexFE)), missingErrorInfo, chip_error_mask)));
                     bs_status_flags |= (chip_flag << chip_i);
                  }
               }
               chip_status[element_i] &=  (bs_status_flags & chip_mask);
            }
            else {
               chip_status[element_i] = 0;
            }
            ++element_i;
         }
      }
   }

   return element_status;
}
