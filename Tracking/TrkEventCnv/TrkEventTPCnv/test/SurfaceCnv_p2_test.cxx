/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkEventTPCnv/test/SurfaceCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p2.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "TestTools/FLOATassert.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/PluginService.h"
#include "TestTools/leakcheck.h"
#include "TestTools/initGaudi.h"
#include <cassert>
#include <iostream>


class TestCnvTool
  : public AthAlgTool, public Trk::IEventCnvSuperTool
{
public:
  TestCnvTool (const std::string& t, const std::string& n, const IInterface* p);
  ~TestCnvTool();

  virtual Trk::ITrkEventCnvTool* getCnvTool(const Identifier& /*id*/) override
  { std::abort(); }
  virtual void recreateRIO_OnTrack( Trk::RIO_OnTrack */*RoT*/ ) override
  { std::abort(); }
  virtual void prepareRIO_OnTrack( Trk::RIO_OnTrack* /*Rot*/) override
  { std::abort(); }
  virtual bool canHandleInDet() const override
  { std::abort(); }
  virtual bool canHandleMuonSpectrometer() const override
  { std::abort(); }
  virtual bool noGeometry() const override
  { std::abort(); }
  virtual int maxNumberOfErrors() const override
  { std::abort(); }
  
  virtual const Trk::Surface* getSurface(const Identifier& id) override;
  void addSurface (std::unique_ptr<Trk::Surface> surf);

  typedef std::map<Identifier, std::unique_ptr<Trk::Surface> > map_t;
  map_t m_map;
};


TestCnvTool::TestCnvTool (const std::string& t,
                          const std::string& n,
                          const IInterface* p)
  : AthAlgTool (t, n, p)
{
  declareInterface<Trk::IEventCnvSuperTool>(this);
}


TestCnvTool::~TestCnvTool()
{
  for (map_t::value_type& p : m_map)
    delete p.second->associatedDetectorElement();
}


const Trk::Surface* TestCnvTool::getSurface (const Identifier& id)
{
  map_t::iterator it = m_map.find (id);
  if (it != m_map.end())
    return it->second.get();
  return nullptr;
}


void TestCnvTool::addSurface (std::unique_ptr<Trk::Surface> surf)
{
  Identifier id = surf->associatedDetectorElementIdentifier();
  m_map[id] = std::move (surf);
}


class TestElement
  : public Trk::TrkDetElementBase
{
public:
  TestElement (unsigned int val);
  virtual Identifier identify() const override;
    
  virtual IdentifierHash identifyHash() const override
  { std::abort(); }
  virtual const Amg::Transform3D & transform() const override
  { std::abort(); }
  virtual const Amg::Transform3D & transform(const Identifier& /*id*/) const override
  { std::abort(); }
  virtual const Trk::Surface& surface () const override
  { std::abort(); }
  virtual const Trk::Surface& surface (const Identifier& /*id*/) const override
  { std::abort(); }
  virtual const Trk::SurfaceBounds & bounds() const  override
  { std::abort(); }
  virtual const Trk::SurfaceBounds & bounds(const Identifier& /*id*/) const override
  { std::abort(); }
  virtual const Amg::Vector3D & center() const override
  { std::abort(); }
  virtual const Amg::Vector3D & center(const Identifier& /*id*/) const override
  { std::abort(); }
  virtual const Amg::Vector3D & normal() const override
  { std::abort(); }
  virtual const Amg::Vector3D & normal(const Identifier& /*id*/) const override
  { std::abort(); }

  Identifier m_id;
};


TestElement::TestElement (unsigned int val)
  : Trk::TrkDetElementBase(nullptr), m_id (val)
{
}


Identifier TestElement::identify() const
{
  return m_id;
}


void compare (const Trk::Surface& s1,
              const Trk::Surface& s2)
{
  assert (s1.name() == s2.name());
  assert (s1.isFree() == s2.isFree());
  if (s1.isFree()) {
    for (int i=0; i < 4; i++)
      for (int j=0; j < 4; j++)
        assert (Athena_test::isEqual (s1.transform()(i,j), s2.transform()(i,j)));
  }
  else {
    assert (s1.associatedDetectorElementIdentifier() ==
            s2.associatedDetectorElementIdentifier());
  }
}


template <class CNV>
void testit (const Trk::Surface& trans1)
{
  MsgStream log (0, "test");
  CNV cnv;
  cnv.setCnvToolName ("TestCnvTool");
  Trk::Surface_p2 pers;
  const auto & p=dynamic_cast<const typename CNV::Trans_t*>(&trans1);
  cnv.transToPers (p, &pers, log);
  std::unique_ptr<Trk::Surface> trans2 (cnv.createTransient (&pers, log));
  compare (trans1, *trans2);
}


template <class SURF>
struct make_surf
{
  std::unique_ptr<SURF> operator() (const TestElement& elt)
  {
    return CxxUtils::make_unique<SURF> (elt, elt.identify());
  }
};

template <>
struct make_surf<Trk::DiscSurface>
{
  std::unique_ptr<Trk::DiscSurface> operator() (const TestElement& elt)
  {
    return CxxUtils::make_unique<Trk::DiscSurface> (elt);
  }
};


template <class SURF>
void add_det_surface (unsigned int val)
{
  auto detel = CxxUtils::make_unique<TestElement> (val);
  auto surf = make_surf<SURF>() (*detel.release());
  
  ToolHandle<Trk::IEventCnvSuperTool> h ("TestCnvTool");
  TestCnvTool* tool = dynamic_cast<TestCnvTool*> (&*h);
  if (tool) tool->addSurface (std::move (surf));
}


void populate_surfaces()
{
  add_det_surface<Trk::DiscSurface> (1);
  add_det_surface<Trk::PlaneSurface> (2);
  add_det_surface<Trk::StraightLineSurface> (3);
  add_det_surface<Trk::SaggedLineSurface> (4);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  ToolHandle<Trk::IEventCnvSuperTool> h ("TestCnvTool");

  testit<DiscSurfaceCnv_p2> (*h->getSurface (Identifier (1)));
  testit<PlaneSurfaceCnv_p2> (*h->getSurface (Identifier (2)));
  testit<StraightLineSurfaceCnv_p2> (*h->getSurface (Identifier (3)));
  testit<SaggedLineSurfaceCnv_p2> (*h->getSurface (Identifier (4)));

  Trk::DiscSurface disc (CxxUtils::make_unique<Amg::Transform3D>(Amg::getRotateX3D (0.5)));
  testit<DiscSurfaceCnv_p2> (disc);

  Trk::ConeSurface cone (CxxUtils::make_unique<Amg::Transform3D>(Amg::getRotateY3D (0.5)));
  testit<ConeSurfaceCnv_p2> (cone);

  Trk::CylinderSurface cyl (CxxUtils::make_unique<Amg::Transform3D>(Amg::getRotateZ3D (0.5)));
  testit<CylinderSurfaceCnv_p2> (cyl);

  Trk::PerigeeSurface per (CxxUtils::make_unique<Amg::Transform3D>(Amg::getRotateX3D (0.7)));
  testit<PerigeeSurfaceCnv_p2> (per);

  Trk::PlaneSurface plane (CxxUtils::make_unique<Amg::Transform3D>(Amg::getRotateX3D (0.7)));
  testit<PlaneSurfaceCnv_p2> (plane);

  Trk::StraightLineSurface sl (CxxUtils::make_unique<Amg::Transform3D>(Amg::getRotateY3D (0.7)));
  testit<StraightLineSurfaceCnv_p2> (sl);
}


int main()
{
  // This isn't really needed, except that without it, CommonMessaging
  // will leak memory, causing a Leakcheck failure.
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  populate_surfaces();
  test1();
  return 0;
}


DECLARE_COMPONENT (TestCnvTool)

