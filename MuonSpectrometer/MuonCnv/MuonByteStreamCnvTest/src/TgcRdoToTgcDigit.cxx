/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcRdoToTgcDigit.h"

TgcRdoToTgcDigit::TgcRdoToTgcDigit(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode TgcRdoToTgcDigit::initialize()
{
  ATH_CHECK( m_idHelperSvc.retrieve() );
  ATH_CHECK( m_tgcRdoDecoderTool.retrieve() );
  ATH_CHECK(m_tgcRdoKey.initialize());
  ATH_CHECK(m_tgcDigitKey.initialize());
  // get TGC cablingSvc
  ATH_CHECK(m_tgcCablingServerSvc.retrieve());
  ATH_CHECK(m_tgcCablingServerSvc->giveCabling(m_tgcCabling));
  m_is12foldTgc = m_tgcCablingServerSvc->isAtlas();
  return StatusCode::SUCCESS;
}

StatusCode TgcRdoToTgcDigit::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG( "in execute()"  );
  // retrieve the collection of RDO
  SG::ReadHandle<TgcRdoContainer> rdoRH(m_tgcRdoKey, ctx);
  if (!rdoRH.isValid()) {
    ATH_MSG_WARNING( "No TGC RDO container found!"  );
    return StatusCode::SUCCESS;
  }
  const TgcRdoContainer* rdoContainer = rdoRH.cptr();
  ATH_MSG_DEBUG( "Retrieved " << rdoContainer->size() << " TGC RDOs." );

  SG::WriteHandle<TgcDigitContainer> wh_tgcDigit(m_tgcDigitKey, ctx);
  ATH_CHECK(wh_tgcDigit.record(std::make_unique<TgcDigitContainer> (m_idHelperSvc->tgcIdHelper().module_hash_max())));
  ATH_MSG_DEBUG( "Decoding TGC RDO into TGC Digit"  );

  Identifier oldElementId;

  TgcRdoContainer::const_iterator tgcRDO = rdoContainer->begin();

  for (; tgcRDO!=rdoContainer->end();++tgcRDO) {
    if ( (*tgcRDO)->size() > 0 ) {
      ATH_CHECK(this->decodeTgc ( *tgcRDO, wh_tgcDigit.ptr(), oldElementId ));
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TgcRdoToTgcDigit::decodeTgc( const TgcRdo *rdoColl,
                                        TgcDigitContainer * tgcContainer,
                                        Identifier& oldElementId ) const
{
  TgcDigitCollection* collection = nullptr;

  const IdContext tgcContext = m_idHelperSvc->tgcIdHelper().module_context();

  ATH_MSG_DEBUG( "Number of RawData in this rdo "
                 << rdoColl->size()  );
  // for each Rdo, loop over RawData, converter RawData to digit
  // retrieve/create digit collection, and insert digit into collection

  std::map<std::vector<uint16_t>, uint16_t> stripMap;
  if(m_is12foldTgc) { // Only when 12-fold TGC cabling is used
    // TGC2 Endcap Strip OR channel treatement preparation start
    // Signals are ORed as follows:
    // |stationEta|=5, T9, E1 - slbId=16, bit0 of stripSlbBits in this code
    //                        /
    // |stationEta|=4, T8, E2 - slbId=17, bit1 of stripSlbBits in this code
    //                        /
    // |stationEta|=3, T7, E3 - slbId=18, bit2 of stripSlbBits in this code
    //                        /
    // |stationEta|=2, T6, E4 - slbId=19, bit3 of stripSlbBits in this code
    //                        /
    // |stationEta|=1, T4, E5 - slbId=20, bit4 of stripSlbBits in this code
    for (const TgcRawData * rawData : *rdoColl) {
      if(rawData->isCoincidence()) continue; // Require hits
      if(rawData->slbType()!=TgcRawData::SLB_TYPE_DOUBLET_STRIP) continue; // Require TGC2 or TGC3
      if(rawData->sswId()==7) continue; // Exclude Forward
      if(rawData->bitpos()<112 || rawData->bitpos()>199) continue; // Require C, D-input

      std::vector<uint16_t> stripId(5, 0);
      stripId.at(0) = rawData->subDetectorId();
      stripId.at(1) = rawData->rodId();
      stripId.at(2) = rawData->sswId();
      stripId.at(3) = rawData->bitpos();
      stripId.at(4) = rawData->bcTag();
      uint16_t stripSlbBits = 0x1 << (rawData->slbId()-16);
      std::map<std::vector<uint16_t>, uint16_t>::iterator itMap = stripMap.find(stripId); // Find correspond channel
      if(itMap==stripMap.end()) { // This is new one
        stripMap.insert(std::map<std::vector<uint16_t>, uint16_t>::value_type(stripId, stripSlbBits));
      } else { // This already exists
        itMap->second |= stripSlbBits;
      }
    }
    // Covert to original hit patterns
    for (auto& jtPair : stripMap) {
      if(jtPair.second<=31) {
        // x    : 5-bit variable
        // f(x) : OR function above, Digit->RDO conversion
        // g(x) : originalHitBits which satisfies f(g(f(x))) = f(x), RDO->Digit conversion
        static const uint16_t originalHitBits[32] = {
          //  0   1   2   3   4   5   6   7
          0,  1,  0,  3,  0,  0,  4,  7,
          //  8   9  10  11  12  13  14  15
          0,  0,  0,  0,  8,  9, 12, 15,
          // 16  17  18  19  20  21  22  23
          0,  0,  0,  0,  0,  0,  0,  0,
          // 24  25  26  27  28  29  30  31
          16, 17,  0, 19, 24, 25, 28, 31
        };
        jtPair.second = originalHitBits[jtPair.second];
      } else {
        jtPair.second = 0;
      }
    }
    // TGC2 Endcap Strip OR channel treatement preparation end
  }

  for (const TgcRawData * rawData : *rdoColl) {
      // check Hit or Coincidence
      if (rawData->isCoincidence()) continue;

      if(m_is12foldTgc) { // Only when 12-fold TGC cabling is used
        // TGC2 Endcap Strip OR channel treatement start
        if(rawData->slbType()==TgcRawData::SLB_TYPE_DOUBLET_STRIP && // Require TGC2 or TGC3
           rawData->sswId()!=7 && // Exclude Forward
           rawData->bitpos()>=112 && rawData->bitpos()<=199 // Require C, D-input
           ) {
          std::vector<uint16_t> stripId(5, 0);
          stripId.at(0) = rawData->subDetectorId();
          stripId.at(1) = rawData->rodId();
          stripId.at(2) = rawData->sswId();
          stripId.at(3) = rawData->bitpos();
          stripId.at(4) = rawData->bcTag();
          std::map<std::vector<uint16_t>, uint16_t>::iterator itMap = stripMap.find(stripId); // Find correspond hit
          if(itMap!=stripMap.end()) {
            uint16_t stripSlbBits = 0x1 << (rawData->slbId()-16);
            if(!(itMap->second & stripSlbBits)) continue; // This hit is additional.
          }
        }
        // TGC2 Endcap Strip OR channel treatement end
      }

      // repeat two times for ORed channel
      for (int iOr=0; iOr<2; ++iOr)
        {

          if(m_is12foldTgc) { // Only when 12-fold TGC cabling is used
            // TGC2 Endcap Strip OR channel is not converted.
            if(iOr && rawData->slbType()==TgcRawData::SLB_TYPE_DOUBLET_STRIP) continue;
          }

          bool orFlag = false;

          // check if this channel has ORed partner only when 2nd time
          if (iOr != 0)
            {
              const bool o_found = m_tgcCabling->isOredChannel(rawData->subDetectorId(),
                                                               rawData->rodId(),
                                                               rawData->sswId(),
                                                               rawData->slbId(),
                                                               rawData->bitpos());
              // set OR flag
              if (o_found)
                orFlag=true;
              else
                continue;
            }

          // get element ID
          Identifier elementId;
          const bool e_found = m_tgcCabling->getElementIDfromReadoutID
            (elementId,rawData->subDetectorId(),rawData->rodId(),
             rawData->sswId(),rawData->slbId(),rawData->bitpos(),orFlag);

          if (!e_found) {
            bool show_warning_level = true;

            /* One invalid channel in TGC sector A09:
               sub=103 rod=9 ssw=6 slb=20 bitpos=151 orFlag=0
               was always seen in 2008 data, at least run 79772 - 91800.
               bug #48828 */
            if(rawData->subDetectorId()==103 &&
               rawData->rodId()==9 &&
               rawData->sswId()==6 &&
               rawData->slbId()==20 &&
               rawData->bitpos()==151) {
              show_warning_level = m_show_warning_level_invalid_TGC_A09_SSW6_hit;
            }

            if(show_warning_level || msgLvl(MSG::DEBUG)) {
              ATH_MSG_WARNING( "ElementID not found for "
                               << " sub=" << rawData->subDetectorId()
                               << " rod=" << rawData->rodId()
                               << " ssw=" << rawData->sswId()
                               << " slb=" << rawData->slbId()
                               << " bitpos=" << rawData->bitpos()
                               << " orFlag=" << orFlag  );
            }
            continue;
          }

          // convert RawData to Digit
          std::unique_ptr<TgcDigit> newDigit (m_tgcRdoDecoderTool->getDigit(rawData,orFlag));

          // check if converted correctly
          if (!newDigit) continue;

          // check new element or not
          IdentifierHash coll_hash;
          if (m_idHelperSvc->tgcIdHelper().get_hash(elementId, coll_hash, &tgcContext)) {
            ATH_MSG_WARNING( "Unable to get TGC digit collection hash "
                             << "context begin_index = " << tgcContext.begin_index()
                             << " context end_index  = " << tgcContext.end_index()
                             << " the identifier is " );
            elementId.show();
          }

          if (elementId != oldElementId) {
            // get collection
            auto coll = tgcContainer->indexFindPtr(coll_hash);
            if (nullptr !=  coll) {
              TgcDigitCollection* aCollection ATLAS_THREAD_SAFE = const_cast<TgcDigitCollection*>( coll ); // FIXME
              collection = aCollection;
            }
            else
              {
                // create new collection
                ATH_MSG_DEBUG( "Created a new digit collection : " << coll_hash  );
                collection = new TgcDigitCollection(elementId, coll_hash);
                ATH_CHECK( tgcContainer->addCollection(collection,coll_hash) );
              }

            oldElementId = elementId;
          }

          if(!collection) {
            ATH_MSG_WARNING( "TgcRdoToTgcDigit::decodeTgc TgcDigitCollection* collection is null."  );
            return StatusCode::SUCCESS;
          }

          // check duplicate digits
          bool duplicate = false;
          for (const TgcDigit* digit : *collection) {
            if ((newDigit->identify() == digit->identify()) &&
                (newDigit->bcTag()    == digit->bcTag())) {
              duplicate = true;
              ATH_MSG_DEBUG( "Duplicate TGC Digit removed"   );
              break;
            }
          }
          if (!duplicate) {
            // add the digit to the collection
            collection->push_back(newDigit.release());
          }
        }
    }
  return StatusCode::SUCCESS;
}
