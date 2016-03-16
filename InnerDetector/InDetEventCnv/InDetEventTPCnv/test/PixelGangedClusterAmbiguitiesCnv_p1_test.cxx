/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file InDetEventTPCnv/test/PixelGangedClusterAmbiguitiesCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "InDetEventTPCnv/InDetPrepRawData/PixelGangedClusterAmbiguitiesCnv_p1.h"
#include "InDetEventTPCnv/PixelClusterContainerCnv_tlp1.h"
#include "StoreGate/StoreGateSvc.h"
#include "TestTools/initGaudi.h"
#include "SGTools/TestStore.h"
#include "AthenaKernel/IThinningSvc.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


class TestThinningSvc
  : public SGTest::TestStore, virtual public IThinningSvc
{
public:
  virtual StatusCode sysInitialize()   override { std::abort(); }
  virtual StatusCode sysStart()        override { std::abort(); }
  virtual StatusCode sysStop()         override { std::abort(); }
  virtual StatusCode sysFinalize()     override { std::abort(); }
  virtual StatusCode sysReinitialize() override { std::abort(); }
  virtual StatusCode sysRestart()      override { std::abort(); }
  virtual StatusCode configure()       override { std::abort(); }
  virtual StatusCode initialize()      override { std::abort(); }
  virtual StatusCode start()           override { std::abort(); }
  virtual StatusCode stop()            override { std::abort(); }
  virtual StatusCode finalize()        override { std::abort(); }
  virtual StatusCode terminate()       override { std::abort(); }
  virtual StatusCode reinitialize()    override { std::abort(); }
  virtual StatusCode restart()         override { std::abort(); }
  virtual Gaudi::StateMachine::State FSMState() const override { std::abort(); }
  virtual Gaudi::StateMachine::State targetFSMState() const override { std::abort(); }
  virtual void setServiceManager (ISvcManager*) override { std::abort(); }

  virtual bool thinningOccurred() const override { std::abort(); }
  virtual StatusCode register_slimmer (Athena::ISlimmingHdlr */*handler*/) override { std::abort(); }
  virtual Athena::IThinningHdlr* handler( SG::DataProxy* /*proxy*/ ) override { std::abort(); }

  virtual StatusCode
  filter_impl( Athena::IThinningHdlr* /*handler*/,
               SG::DataProxy* /*proxy*/,
               const Filter_t& /*filter*/,
               const IThinningSvc::Operator::Type /*op*/ = Operator::And ) override
  { std::abort(); }
  virtual StatusCode commit() override { std::abort(); }
  virtual StatusCode rollback() override { std::abort(); }
  virtual bool is_thinned_impl(const SG::DataProxy* /*p*/) const override { std::abort(); }

  std::size_t index_impl( const SG::DataProxy* /*objProxy*/, 
                          std::size_t idx ) const 
  {
    map_t::const_iterator i = m_map.find (idx);
    if (i != m_map.end())
      return i->second;
    return idx;
  }

  void remap (size_t from, size_t to)
  {
    m_map[from] = to;
  }


  typedef std::map<size_t, size_t> map_t;
  map_t m_map;
};


void compare (const InDet::PixelGangedClusterAmbiguities& p1,
              const InDet::PixelGangedClusterAmbiguities& p2)
{
  assert (p1 == p2);
}


void testit (const InDet::PixelGangedClusterAmbiguities& trans1)
{
  MsgStream log (0, "test");
  PixelGangedClusterAmbiguitiesCnv_p1 cnv;
  InDet::PixelGangedClusterAmbiguities_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  InDet::PixelGangedClusterAmbiguities trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 (const InDet::PixelClusterContainer& cont)
{
  std::cout << "test1\n";

  const InDet::PixelClusterCollection& coll = **cont.indexFind(3);

  InDet::PixelGangedClusterAmbiguities trans1;
  trans1.emplace (coll[1], coll[2]);
  trans1.emplace (coll[1], coll[5]);
  trans1.emplace (coll[3], coll[7]);
  trans1.emplace (coll[7], coll[3]);
  testit (trans1);
}


void test2 (const InDet::PixelClusterContainer& cont)
{
  std::cout << "test2\n";

  const InDet::PixelClusterCollection& coll2 = **cont.indexFind(2);
  const InDet::PixelClusterCollection& coll3 = **cont.indexFind(3);

  InDet::PixelGangedClusterAmbiguities trans1;
  trans1.emplace (coll2[1], coll2[3]);
  trans1.emplace (coll3[2], coll3[4]);
  trans1.emplace (coll2[5], coll3[5]);
  trans1.emplace (coll2[6], coll3[6]);

  TestThinningSvc thinning;
  thinning.remap (2, IThinningSvc::RemovedIdx);
  IThinningSvc::instance (&thinning, true);

  MsgStream log (0, "test");
  PixelGangedClusterAmbiguitiesCnv_p1 cnv;
  InDet::PixelGangedClusterAmbiguities_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  InDet::PixelGangedClusterAmbiguities trans2;
  cnv.persToTrans (&pers, &trans2, log);

  for (const auto& p : trans2)
    std::cout << p.first->getHashAndIndex().objIndex() << " " << p.second->getHashAndIndex().objIndex() << "\n";
}


const InDet::PixelClusterContainer& makeclusts (StoreGateSvc* sg)
{
  auto cont = CxxUtils::make_unique<InDet::PixelClusterContainer>(5);

  for (int hash=2; hash <= 3; hash++) {
    auto coll = CxxUtils::make_unique<InDet::PixelClusterCollection>(IdentifierHash(hash));

    for (int i=0; i < 10; i++) {
      int offs = i*10 + hash*100;
      Amg::Vector2D locpos (offs+1.5, offs+2.5);
      std::vector<Identifier> rdoList { Identifier(offs+54436),
                                        Identifier(offs+132456),
                                        Identifier(offs+6456) };
      std::vector<int> totList { offs+6, offs+5, offs+4 };
      std::vector<float> chargeList { static_cast<float>(offs+4.5),
                                      static_cast<float>(offs+6.5),
                                      static_cast<float>(offs+1.5),
                                      static_cast<float>(offs+3.5) };
      InDet::SiWidth width (Amg::Vector2D (offs+1, offs+2),
                            Amg::Vector2D (offs+3.5, offs+4.5));

      Amg::MatrixX cov(2,2);
      for (int i=0; i < 2; i++)
        for (int j=0; j < 2; j++)
          cov(i,j) = 100*(i+1)*(j+1) + offs;

      auto cl = CxxUtils::make_unique<InDet::PixelCluster>
        (Identifier (offs+1234),
         locpos,
         rdoList,
         13,  // lvl1
         totList,
         chargeList,
         width,
         nullptr,
         new Amg::MatrixX(cov),
         offs+10.5,
         offs+11.5,
         i&1,
         (offs+2.5)/500,
         (offs+3.5)/500);
      cl->setFake(i&2);
      cl->setGangedPixel(i&4);
      cl->setAmbiguous(i&8);
      cl->setHashAndIndex (hash, i);
      coll->push_back (std::move (cl));
    }
    cont->addCollection (coll.release(), hash, true);
  }

  const InDet::PixelClusterContainer& ret = *cont;
  assert (sg->record (std::move (cont), "clusters").isSuccess());
  return ret;
}


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("InDetEventTPCnv_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }  

  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StoreGateSvc* sg = 0;
  assert ( svcLoc->service("StoreGateSvc", sg).isSuccess() );

  const InDet::PixelClusterContainer& cont = makeclusts (sg);

  test1 (cont);
  test2 (cont);
  return 0;
}
