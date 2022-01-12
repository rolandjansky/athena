/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "Lvl1ResultAccessTool.h"

#include "TrigT1Result/RoIBResult.h"
#include "CTPfragment/CTPdataformatVersion.h"

#include "TrigConfData/L1Menu.h"
#include "TrigConfHLTUtils/HLTUtils.h"
#include "TrigSteeringEvent/Lvl1Result.h"


HLT::Lvl1ResultAccessTool::Lvl1ResultAccessTool(const std::string& name, const std::string& type,
                                                const IInterface* parent) :
   base_class(name, type, parent)
{
}

StatusCode
HLT::Lvl1ResultAccessTool::initialize() {

   ATH_CHECK( m_l1PrescaleSetInputKey.initialize( ) );
   ATH_CHECK( m_l1ResultKey.initialize( ) );

   return StatusCode::SUCCESS;
}

// =============================================================
// Prepare LVL1 items, i.e. name -> hashID, put everything into
// a vector where the position corresponds to the LVL1 item
// position in the CTP result!
// =============================================================
std::vector< std::unique_ptr<LVL1CTP::Lvl1Item> >
HLT::Lvl1ResultAccessTool::makeLvl1ItemConfig(const EventContext& context) const
{
   // vector holding all configured LVL1 items, default initializes to empty unique_ptr
   constexpr size_t numberOfCTPItems = 512; // this is fixed
   std::vector<std::unique_ptr<LVL1CTP::Lvl1Item>> lvl1ItemConfig(numberOfCTPItems);

   const TrigConf::L1Menu *l1menu = nullptr;
   if( detStore()->retrieve(l1menu).isFailure() ) {
      ATH_MSG_ERROR("No L1Menu found");
   } else {
      SG::ReadCondHandle<TrigConf::L1PrescalesSet> l1psRH(m_l1PrescaleSetInputKey, context);
      if( !l1psRH.isValid() ) {
         ATH_MSG_ERROR("No valid L1PrescalesSet handle");
      } else {
         const TrigConf::L1PrescalesSet* l1PrescaleSet{*l1psRH};
         if(l1PrescaleSet == nullptr) {
            ATH_MSG_ERROR( "No L1PrescalesSet available");
         } else {
            for(auto & item : *l1menu) {
               double prescale = l1PrescaleSet->getPrescaleFromCut(l1PrescaleSet->prescale(item.name()).cut);
               lvl1ItemConfig[item.ctpId()] = std::make_unique<LVL1CTP::Lvl1Item>( item.name(),
                                                                                   TrigConf::HLTUtils::string2hash(item.name()),
                                                                                   0, 0, 0, prescale);
            }
         }
      }
   }
   return lvl1ItemConfig;
}


// check for calo calibration bits
// get the calibration triggers, 253, 254 and 255 ie, bits 
// 29, 30 and 31 from word 7				  
bool
HLT::Lvl1ResultAccessTool::isCalibrationEvent(const ROIB::RoIBResult& result) const {

   ///return false; // in the RUN 2 menu there is currently no such restriction and all kinds of items are on these ctp IDs
   int ctpVersion = result.cTPResult().header().formatVersion() & 0xf ;
   CTPdataformatVersion v(ctpVersion);
   std::vector<ROIB::CTPRoI> ctpRoIVec(result.cTPResult().TAV());
   //  if ( ctpRoIVec.size() >= 8 && (ctpRoIVec[7].roIWord() &  0xE0000000) )
   if ( ctpRoIVec.size()==v.getTAVwords() && ( ctpRoIVec[ctpRoIVec.size()-1].roIWord() & 0xE0000000 ) )
      return true;
   else
      return false;
}

namespace {
   bool getBit(const std::vector<ROIB::CTPRoI>& words, unsigned position) {
      unsigned w  = position/32;  
      unsigned b  = position%32;  
      if ( words.size() > w) {
         uint32_t roIWord = words[w].roIWord();
         return (roIWord >> b) & 0x1;
      } 
      return false;
   }
}

// ==============================
//       create LVL1 items
// ==============================
std::vector< const LVL1CTP::Lvl1Item* >
HLT::Lvl1ResultAccessTool::createL1Items( const std::vector< std::unique_ptr<LVL1CTP::Lvl1Item> >& lvl1ItemConfig,
                                          const ROIB::RoIBResult& result,
                                          LVL1CTP::Lvl1Result const** lvl1ResultOut /*= nullptr*/) const
{
   std::vector< const LVL1CTP::Lvl1Item* > items;
   std::vector<ROIB::CTPRoI> bitsBP = result.cTPResult().TBP();
   std::vector<ROIB::CTPRoI> bitsAP = result.cTPResult().TAP();
   std::vector<ROIB::CTPRoI> bitsAV = result.cTPResult().TAV();

   bool calib_flag = isCalibrationEvent(result);
   ATH_MSG_DEBUG("Calibration event? " << calib_flag);
   
   // loop over all configured items if no calibration, else only 3 last // Needs fixing
   int ctpVersion = result.cTPResult().header().formatVersion() & 0xf ;
   CTPdataformatVersion v(ctpVersion);
   
   unsigned first_item = calib_flag ? v.getMaxTrigItems()-3 : 0; // last three items are calib items, only those should be activated

   for ( unsigned i = first_item; i < lvl1ItemConfig.size(); i++ ) {
      if ( !lvl1ItemConfig[ i ] ) continue; // empty slot

      LVL1CTP::Lvl1Item* item =  lvl1ItemConfig[ i ].get();
      *item = LVL1CTP::Lvl1Item( item->name(), item->hashId(), 
                  getBit(bitsBP, i), 
                  getBit(bitsAP, i), 
                  getBit(bitsAV, i), 
                  item->prescaleFactor() );  
      items.push_back(item);
      ATH_MSG_DEBUG("Set bits on "<< item->name() <<" PS="<< item->prescaleFactor() <<"  BP=" <<getBit(bitsBP, i)<<" AP="<<getBit(bitsAP, i)<<" AV="<<getBit(bitsAV, i));
   }

  // Fill TBP, TAP in case we're creating the Lvl1Result
   if (lvl1ResultOut) {
      auto lvl1Result = std::make_unique<LVL1CTP::Lvl1Result>(true);

      // 1.) TAV
      const std::vector<ROIB::CTPRoI> ctpRoIVecAV = result.cTPResult().TAV();
      for (unsigned int iWord = 0; iWord < ctpRoIVecAV.size(); ++iWord) {
         uint32_t roIWord = ctpRoIVecAV[iWord].roIWord();
         lvl1Result->itemsAfterVeto().push_back(roIWord);
         ATH_MSG_DEBUG("TAV word #" << iWord << " is 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << roIWord << std::dec);
      }

      // 1.) TBP
      const std::vector<ROIB::CTPRoI> ctpRoIVecBP = result.cTPResult().TBP();
      for (unsigned int iWord=0; iWord < ctpRoIVecBP.size(); ++iWord) {
         uint32_t roIWord = ctpRoIVecBP[iWord].roIWord();
         lvl1Result->itemsBeforePrescale().push_back(roIWord);
         ATH_MSG_DEBUG( "TBP word #" << iWord << " is 0x" << std::hex
                        << std::setw( 8 ) << std::setfill( '0' ) << roIWord << std::dec);
      }

      // 2.) TAP
      const std::vector<ROIB::CTPRoI> ctpRoIVecAP = result.cTPResult().TAP();
      for (unsigned int iWord=0; iWord < ctpRoIVecAP.size(); ++iWord) {
         uint32_t roIWord = ctpRoIVecAP[iWord].roIWord();
         lvl1Result->itemsAfterPrescale().push_back(roIWord);

         ATH_MSG_DEBUG("TAP word #" << iWord << " is 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << roIWord << std::dec);
         
      }
      // make sure TBP, TAP, TAV all have 8 entries !
      while (lvl1Result->itemsBeforePrescale().size() < 8) lvl1Result->itemsBeforePrescale().push_back(0);
      while (lvl1Result->itemsAfterPrescale().size() < 8) lvl1Result->itemsAfterPrescale().push_back(0);
      while (lvl1Result->itemsAfterVeto().size() < 8) lvl1Result->itemsAfterVeto().push_back(0);
      *lvl1ResultOut = lvl1Result.get();
      ATH_CHECK(SG::makeHandle(m_l1ResultKey).record( std::move(lvl1Result)), {});
   } // if (lvl1ResultOut)

   return items;
}

std::bitset<3>
HLT::Lvl1ResultAccessTool::lvl1EMTauJetOverflow(const ROIB::RoIBResult& result)
{   
   const int MAXEM = 5;         // see ATR-12285
   const int MAXTAU = 5;
   const int MAXJET = 4;
   
   std::map<std::pair<int,int>, int> em, tau, jet;  // (crate,module) : count

   // Count number of RoIs per crate/module
   for (const ROIB::EMTauResult& res : result.eMTauResult()) {
      for (const ROIB::EMTauRoI& roi : res.roIVec()) {
         int c = m_cpDecoder.crate(roi.roIWord());
         int m = m_cpDecoder.module(roi.roIWord());
         int t = m_cpDecoder.roiType(roi.roIWord());

         if (t==LVL1::TrigT1CaloDefs::EMRoIWordType) ++em[{c,m}];
         else if (t==LVL1::TrigT1CaloDefs::TauRoIWordType) ++tau[{c,m}];
      }
   }

   for (const ROIB::JetEnergyResult& res : result.jetEnergyResult()) {
      for (const ROIB::JetEnergyRoI& roi : res.roIVec()) {
         int c = m_jepDecoder.crate(roi.roIWord());
         int m = m_jepDecoder.module(roi.roIWord());
         int t = m_jepDecoder.roiType(roi.roIWord());

         if (t==LVL1::TrigT1CaloDefs::JetRoIWordType) ++jet[{c,m}];
      }
   }

   // Check if there was an overflow
   std::bitset<3> overflow;
   overflow[0] = std::count_if(em.begin(), em.end(), [](const decltype(em)::value_type& i){return i.second>MAXEM;});
   overflow[1] = std::count_if(tau.begin(), tau.end(), [](const decltype(tau)::value_type& i){return i.second>MAXTAU;});
   overflow[2] = std::count_if(jet.begin(), jet.end(), [](const decltype(jet)::value_type& i){return i.second>MAXJET;});

   if (msgLvl(MSG::DEBUG)) {
      msg() << "EM RoI multiplicities by crate,module: ";
      for (const auto& i : em) msg() << "(" << i.first.first << "," << i.first.second << "):" << i.second << " ";

      msg() << endmsg << "Tau RoI multiplicities by crate,module: ";
      for (const auto& i : tau) msg() << "(" << i.first.first << "," << i.first.second << "):" << i.second << " ";

      msg() << endmsg << "Jet RoI multiplicities by crate,module: ";
      for (const auto& i : jet) msg() << "(" << i.first.first << "," << i.first.second << "):" << i.second << " ";
      msg() << endmsg;
   }
   
   return overflow;
}
