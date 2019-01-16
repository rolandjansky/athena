/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileByteStream/test/TileROD_Decoder_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Tests for TileROD_Decoder.
 */

#undef NDEBUG
#include "TileByteStream/TileROD_Decoder.h"
#include "TileByteStream/TileCellCont.h"
#include "TileEvent/TileDigitsCollection.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "PathResolver/PathResolver.h"
#include "TileIdentifier/TileHWID.h"
#include "StoreGate/StoreGateSvc.h"
#include "IdDictParser/IdDictParser.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include <cassert>
#include <iostream>
#include <fstream>


IdDictParser parser;


class TileCablingSvc
{
public:
  static
  void init_idhelpers()
  {
    ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "test");
    assert( detStore.retrieve().isSuccess() );

    auto tileid = std::make_unique<TileID>();
    auto hwid = std::make_unique<TileHWID>();
    auto tbid = std::make_unique<TileTBID>();

    tileid->set_do_neighbours (false);
    parser.register_external_entity("TileCalorimeter", "IdDictTileCalorimeter.xml");
    IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
    hwid->set_quiet (true);
    tbid->set_quiet (true);
    tileid->set_quiet (true);
    assert (hwid->initialize_from_dictionary (idd) == 0);
    assert (tbid->initialize_from_dictionary (idd) == 0);
    assert (tileid->initialize_from_dictionary (idd) == 0);
    TileCablingService* svc = TileCablingService::getInstance();
    svc->setTileHWID (hwid.get());
    svc->setTileTBID (tbid.get());
    svc->setTileID (tileid.get());

    assert( detStore->record(std::move(tileid), "TileID").isSuccess() );
    assert( detStore->record(std::move(hwid), "TileHWID").isSuccess() );
    assert( detStore->record(std::move(tbid), "TileTBID").isSuccess() );
  }
};


std::string findFile (const std::string& fnameBase)
{
  std::string f = PathResolver::find_file ("TileByteStream/" + fnameBase,
                                           "DATAPATH");
  if (f.empty()) {
    std::cout << "Can't find file TileByteStream/" << fnameBase
              << " in DATAPATH";
    std::abort();
  }

  return f;
}


class ROBBuffer
{
public:
  ROBBuffer (const std::string& fname);

  const uint32_t* data() const { return m_words.data(); }

private:
  std::vector<uint32_t> m_words;
};


ROBBuffer::ROBBuffer (const std::string& fname)
{
  std::ifstream in (fname);
  if (!in.is_open()) {
    std::cout << "Cannot open " << fname << "\n";
    std::abort();
  }

  std::string line;
  while (std::getline (in, line)) {
    std::string::size_type pos = 0;
    while (pos < line.size() && isspace(line[pos])) ++pos;
    while (pos < line.size() && !isspace(line[pos])) ++pos;
    while (pos < line.size() && isspace(line[pos])) ++pos;
    if (pos >= line.size()) continue;
    uint32_t word = strtoul (line.c_str()+pos, nullptr, 16);
    m_words.push_back (word);
  }

  assert (m_words.size() > 9);
}


class MyROBData
{
public:
  MyROBData (const std::string& fname);
  const TileROD_Decoder::ROBData& rob() const { return m_rob; }

private:
  ROBBuffer m_buf;
  TileROD_Decoder::ROBData m_rob;
};


MyROBData::MyROBData (const std::string& fname)
  : m_buf (fname),
    m_rob (m_buf.data())
{
}


void test1 (const TileROD_Decoder* decoder)
{
  std::cout << "test1 (fillCollection)\n";

  MyROBData data01 (findFile ("TileData-01.dump"));
  MyROBData data02 (findFile ("TileData-02.dump"));

  {
    TileDigitsCollection coll (256);
    decoder->fillCollection (&data01.rob(), coll);
    std::cout << "TileDigitsCollection: " << coll.size() << "\n";
    std::cout << static_cast<std::string> (coll);
    coll.printExtra (std::cout);
    std::cout << "\n";
  }
  {
    TileRawChannelContainer cont;
    TileRawChannelCollection coll (256);
    decoder->fillCollection (&data01.rob(), coll, &cont);
    std::cout << "TileRawChannelCollection: " << coll.size() << " "
              << cont.get_unit() << " "
              << cont.get_type() << " "
              << cont.get_bsflags() << "\n";
    std::cout << static_cast<std::string> (coll);
    std::cout << "\n";
  }
  {
    TileBeamElemCollection coll (0);
    decoder->fillCollection (&data02.rob(), coll);
    std::cout << "TileBeamElemCollection: " << coll.size() << "\n";
    std::cout << static_cast<std::string> (coll);
    std::cout << "\n";
  }
}


void test2 (const TileROD_Decoder* decoder)
{
  std::cout << "test2 (fillCollection_TileMuRcv)\n";

  MyROBData data03 (findFile ("TileData-03.dump"));
  {
    TileDigitsCollection coll (256);
    decoder->fillCollection_TileMuRcv_Digi (&data03.rob(), coll);
    std::cout << "TileDigitsCollection_TileMuRcv: " << coll.size() << "\n";
    std::cout << static_cast<std::string> (coll);
    coll.printExtra (std::cout);
    std::cout << "\n";
  }
  {
    TileRawChannelCollection coll (256);
    decoder->fillCollection_TileMuRcv_RawChannel (&data03.rob(), coll);
    std::cout << "TileRawChannelCollection: " << coll.size() << "\n";
    std::cout << static_cast<std::string> (coll);
    std::cout << "\n";
  }
  {
    TileMuonReceiverContainer cont;
    decoder->fillContainer_TileMuRcv_Decision (&data03.rob(), cont);
    std::cout << "TileMuonReceiverContainer: " << cont.size() << "\n";
    std::cout << static_cast<std::string> (cont);
    std::cout << "\n";
  }
}


void test3 (TileROD_Decoder* decoder_nc)
{
  std::cout << "test3 (fillCollectionHLT)\n";

  TileCablingService* cabling = TileCablingService::getInstance();
  int maxChannels = cabling->getMaxChannels();
  std::vector<int> r2map (maxChannels, -1);
  for (int i=1; i < 4; i++) {
    decoder_nc->loadRw2Cell (i, r2map);
  }

  for (int j=0; j < 48; j++) {
    r2map[j] = j;
  }
  decoder_nc->loadRw2Cell (0, r2map);

  for (int j=0; j < maxChannels; j++) {
    r2map[j] = j%2;
  }
  for (int i=0; i < 4; i++) {
    decoder_nc->loadRw2Pmt  (i, r2map);
  }

  const TileROD_Decoder* decoder = decoder_nc;

  MyROBData data01 (findFile ("TileData-01.dump"));
  {
    TileCellCollection coll (256, SG::OWN_ELEMENTS);
    std::vector<DummyDetDescrElement> ddes;
    ddes.reserve (48);
    for (int j=0; j < 48; j++) {
      ddes.emplace_back (j, j, j, nullptr);
      coll.push_back (std::make_unique<TileCell>(&ddes[j], 0));
    }

    TileROD_Decoder::D0CellsHLT d0cells;
    uint32_t stat = decoder->fillCollectionHLT (&data01.rob(), coll, d0cells);
    assert (stat == 0);
    std::cout << "TileCellCollection: " << coll.size() << "\n";
    std::cout << static_cast<std::string> (coll);
    std::cout << "\n";

    std::cout << "d0cells: " << "\n";
    for (int i=0; i < 64; i++) {
      std::cout << i << " "
                << d0cells.m_D0Existpos[i] << " "
                << d0cells.m_D0Existneg[i] << " "
                << d0cells.m_D0Maskpos[i] << " "
                << d0cells.m_D0Maskneg[i] << " "
                << d0cells.m_D0chanpos[i].channel() << " "
                << d0cells.m_D0chanpos[i].adc() << " "
                << d0cells.m_D0chanpos[i].amplitude() << " "
                << d0cells.m_D0chanpos[i].time() << " "
                << d0cells.m_D0chanpos[i].quality() << " "
                << d0cells.m_D0channeg[i].channel() << " "
                << d0cells.m_D0channeg[i].adc() << " "
                << d0cells.m_D0channeg[i].amplitude() << " "
                << d0cells.m_D0channeg[i].time() << " "
                << d0cells.m_D0channeg[i].quality()
                << "\n";
    }
    std::cout << "\n";

    decoder->mergeD0cellsHLT (d0cells, coll);
    std::cout << "merged TileCellCollection: " << coll.size() << "\n";
    std::cout << static_cast<std::string> (coll);
    std::cout << "\n";
  }
}


void test4 (const TileROD_Decoder* decoder)
{
  std::cout << "test4 (fillCollectionL2)\n";

  MyROBData data04 (findFile ("TileData-04.dump"));
  {
    TileL2Container cont;
    for (int i=0; i < 256; i++) {
      int collId = decoder->hashFunc()->identifier(i);
      cont.push_back (std::make_unique<TileL2> (collId));
    }
    decoder->fillCollectionL2 (&data04.rob(), cont);
    std::cout << "TileL2Container: " << cont.size() << "\n";
    std::cout << static_cast<std::string> (cont);
    std::cout << "\n";
  }
}


void test5 (const TileROD_Decoder* decoder)
{
  std::cout << "test5 (fillTileLaserObj)\n";

  MyROBData data05 (findFile ("TileData-05.dump"));
  {
    TileLaserObject laser;
    decoder->fillTileLaserObj (&data05.rob(), laser);
    std::cout << "TileLaserObj: \n";
    std::cout << static_cast<std::string> (laser);
    std::cout << "\n";
  }
}


int main()
{
  std::cout << "TileROD_Decoder_test\n";

  ISvcLocator* svcloc;
  if (!Athena_test::initGaudi("TileROD_Decoder_test.txt", svcloc)) {
    return 1;
  }
  TileCablingSvc::init_idhelpers();

  ToolHandle<TileROD_Decoder> decoder ("TileROD_Decoder");
  assert( decoder.retrieve().isSuccess() );

  test1 (decoder.get());
  test2 (decoder.get());
  test3 (decoder.get());
  test4 (decoder.get());
  test5 (decoder.get());
  return 0;
}

