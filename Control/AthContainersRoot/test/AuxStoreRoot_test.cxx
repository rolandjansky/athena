/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainersRoot/test/AuxStoreRoot_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2016
 * @brief Regression test for AuxStoreRoot.
 */


#undef NDEBUG
#include "AthContainersRoot/AuxStoreRoot.h"
#include "AthContainersRoot/AthContainersRootTestDict.h"
#include "AthContainers/AuxElement.h"
#include "TestTools/expect_exception.h"
#include "TTree.h"
#include "TBranch.h"
#include <iostream>
#include <cassert>


class TestBranches
  : public SG::IAuxBranches
{
public:
  TestBranches (bool standalone);
  virtual const std::string& getName() const override;
  virtual std::vector<auxpair_t> auxBranches() const override;
  virtual TBranch* findAuxBranch (SG::auxid_t auxid) override;
  virtual StatusCode readAuxBranch (TBranch& br, void* p, long long entry) override;
  virtual StatusCode readAuxBranch (TBranch& br, void* p,
                                    const std::type_info& ti, long long entry) override;


  std::string m_name;
  TTree m_tree;
  TBranch* m_br_foo;
  TBranch* m_br_bar;
  TBranch* m_br_baz;

  SG::auxid_t m_foo_id;
  SG::auxid_t m_bar_id;
  SG::auxid_t m_baz_id;
  SG::auxid_t m_fee_id;
};


TestBranches::TestBranches (bool standalone)
  : m_name("TestBranches"),
    m_tree (m_name.c_str(), m_name.c_str())
{
  if (standalone) {
    m_br_foo =
      m_tree.Branch ("foo", "AthContainersRootTest::Foo", nullptr);
    m_br_bar =
      m_tree.Branch ("bar", "AthContainersRootTest::Bar", nullptr);
    m_br_baz =
      m_tree.Branch ("baz", static_cast<void*>(nullptr), "baz/I");
  }
  else {
    m_br_foo =
      m_tree.Branch ("foo", "std::vector<AthContainersRootTest::Foo>", nullptr);
    m_br_bar =
      m_tree.Branch ("bar", "std::vector<AthContainersRootTest::Bar>", nullptr);
    m_br_baz =
      m_tree.Branch ("baz", "SG::PackedContainer<int>", nullptr);
  }
    
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  m_foo_id = r.getAuxID<AthContainersRootTest::Foo> ("foo");
  m_bar_id = r.getAuxID<AthContainersRootTest::Bar> ("bar");
  m_baz_id = r.getAuxID<int> ("baz");
  m_fee_id = r.getAuxID<float> ("fee");
}


const std::string& TestBranches::getName() const
{
  return m_name;
}


std::vector<SG::IAuxBranches::auxpair_t> TestBranches::auxBranches() const
{
  std::vector<SG::IAuxBranches::auxpair_t> v;
  v.emplace_back ("foo", m_br_foo);
  v.emplace_back ("bar", m_br_bar);
  v.emplace_back ("baz", m_br_baz);
  return v;
}


TBranch* TestBranches::findAuxBranch (SG::auxid_t auxid)
{
  if (auxid == m_foo_id)
    return m_br_foo;
  if (auxid == m_bar_id)
    return m_br_bar;
  if (auxid == m_baz_id)
    return m_br_baz;
  return nullptr;
}


StatusCode
TestBranches::readAuxBranch (TBranch& /*br*/, void* /*p*/, long long /*entry*/)
{
  std::abort();
}


StatusCode
TestBranches::readAuxBranch (TBranch& /*br*/, void* /*p*/,
                             const std::type_info& /*ti*/,
                             long long /*entry*/)
{
  std::abort();
}


class AuxStoreRootTest
  : public SG::AuxStoreRoot
{
public:
  AuxStoreRootTest (SG::IAuxBranches& container,
                    long long entry,
                    bool standalone);

  using SG::AuxStoreRoot::setEntry;

  virtual SG::auxid_t resolveAuxID (SG::IAuxBranches& container,
                                    const std::type_info* ti,
                                    const std::string& name,
                                    const std::string& elem_type_name,
                                    const std::string& branch_type_name) override;

  virtual bool doReadData (SG::IAuxBranches& container,
                           SG::auxid_t auxid,
                           TBranch& branch,
                           TClass* cl,
                           void* vector,
                           long long entry) override;

  struct DRDLog
  {
    SG::IAuxBranches& m_container;
    SG::auxid_t m_auxid;
    TBranch& m_branch;
    TClass* m_cl;
    void* m_vector;
    long long m_entry;

    DRDLog (SG::IAuxBranches& container,
            SG::auxid_t auxid,
            TBranch& branch,
            TClass* cl,
            void* vector,
            long long entry)
      : m_container (container),
        m_auxid (auxid),
        m_branch (branch),
        m_cl (cl),
        m_vector (vector),
        m_entry (entry)
    {
    }
  };

  std::vector<DRDLog> m_log;
};


AuxStoreRootTest::AuxStoreRootTest (SG::IAuxBranches& container,
                                    long long entry,
                                    bool standalone)
  : SG::AuxStoreRoot (container, entry, standalone)
{
  fillAuxIDs();
}


SG::auxid_t
AuxStoreRootTest::resolveAuxID (SG::IAuxBranches& /*container*/,
                                const std::type_info* ti,
                                const std::string& name,
                                const std::string& elem_type_name,
                                const std::string& branch_type_name)
{
  if (name == "foo") {
    SG::AuxElement::Accessor<AthContainersRootTest::Foo> a ("foo");
    assert (elem_type_name == "AthContainersRootTest::Foo");
    if (standalone()) {
      assert (branch_type_name == "AthContainersRootTest::Foo");
    }
    else {
      assert (branch_type_name == "vector<AthContainersRootTest::Foo>");
    }
    assert  (*ti == typeid (AthContainersRootTest::Foo));
    return a.auxid();
  }
  else if (name == "bar") {
    SG::AuxElement::Accessor<AthContainersRootTest::Bar> a ("bar");
    assert (elem_type_name == "AthContainersRootTest::Bar");
    if (standalone()) {
      assert (branch_type_name == "AthContainersRootTest::Bar");
    }
    else {
      assert (branch_type_name == "vector<AthContainersRootTest::Bar>");
    }
    assert  (*ti == typeid (AthContainersRootTest::Bar));
    return a.auxid();
  }
  else if (name == "baz") {
    SG::AuxElement::Accessor<int> a ("baz");
    if (standalone()) {
      assert (elem_type_name == "Int_t");
      assert (branch_type_name == "Int_t");
    }
    else {
      assert (elem_type_name == "int");
      assert (branch_type_name == "SG::PackedContainer<int>");
    }
    assert  (*ti == typeid (int));
    return a.auxid();
  }
  return SG::null_auxid;
}


bool AuxStoreRootTest::doReadData (SG::IAuxBranches& container,
                                   SG::auxid_t auxid,
                                   TBranch& branch,
                                   TClass* cl,
                                   void* vector,
                                   long long entry)
{
  m_log.emplace_back (container, auxid, branch,
                      cl, vector, entry);

  if ((entry&1) == 0)
    return true;
  return false;
}


void test1()
{
  std::cout << "test1\n";

  TestBranches branches (false);
  AuxStoreRootTest store (branches, 122, false);
  std::set<SG::auxid_t> aset { branches.m_foo_id, branches.m_bar_id, branches.m_baz_id };
  assert (std::set<SG::auxid_t> (store.getAuxIDs().begin(),
                                 store.getAuxIDs().end()) == aset);
  store.m_log.clear();

  const void* d = store.getData (branches.m_foo_id);
  assert (d != nullptr);
  assert (store.m_log.size() == 1);
  assert (&store.m_log[0].m_container == &branches);
  assert (store.m_log[0].m_auxid == branches.m_foo_id);
  assert (&store.m_log[0].m_branch == branches.m_br_foo);
  assert (std::string(store.m_log[0].m_cl->GetName()) == "vector<AthContainersRootTest::Foo>");
  assert (store.m_log[0].m_entry == 122);
  auto foo_vec = reinterpret_cast<std::vector<AthContainersRootTest::Foo>* > (store.m_log[0].m_vector);
  assert (foo_vec->size() == 1);
  assert (foo_vec->data() == d);

  d = store.getData (branches.m_foo_id);
  assert (foo_vec->data() == d);
  assert (store.m_log.size() == 1);

  d = store.getIOData (branches.m_foo_id);
  assert (foo_vec == d);
  assert (store.m_log.size() == 1);

  d = store.getDecoration (branches.m_foo_id, 1, 1);
  assert (foo_vec->data() == d);
  assert (store.m_log.size() == 1);

  d = store.getDecoration (branches.m_fee_id, 1, 1);
  assert (d != nullptr);
  assert (store.m_log.size() == 1);

  EXPECT_EXCEPTION (SG::ExcStoreLocked,
                    store.getDecoration (branches.m_bar_id, 1, 1));

  d = store.getData (branches.m_baz_id);
  assert (store.m_log.size() == 2);
  assert (&store.m_log[1].m_container == &branches);
  assert (store.m_log[1].m_auxid == branches.m_baz_id);
  assert (&store.m_log[1].m_branch == branches.m_br_baz);
  assert (std::string(store.m_log[1].m_cl->GetName()) == "SG::PackedContainer<int>");
  assert (store.m_log[1].m_entry == 122);
  auto baz_vec = reinterpret_cast<SG::PackedContainer<int>* > (store.m_log[1].m_vector);
  assert (baz_vec->size() == 1);
  assert (baz_vec->data() == d);
  assert (baz_vec->parms().nbits() == 32);

  store.setEntry (123);
  d = store.getData (branches.m_bar_id);
  assert (d == nullptr);
  assert (store.m_log.size() == 3);
  assert (&store.m_log[2].m_container == &branches);
  assert (store.m_log[2].m_auxid == branches.m_bar_id);
  assert (&store.m_log[2].m_branch == branches.m_br_bar);
  assert (std::string(store.m_log[2].m_cl->GetName()) == "vector<AthContainersRootTest::Bar>");
  assert (store.m_log[2].m_entry == 123);
}


// Standalone case
void test2()
{
  std::cout << "test2\n";

  TestBranches branches (true);
  AuxStoreRootTest store (branches, 122, true);
  std::set<SG::auxid_t> aset { branches.m_foo_id, branches.m_bar_id, branches.m_baz_id };
  assert (std::set<SG::auxid_t> (store.getAuxIDs().begin(),
                                 store.getAuxIDs().end()) == aset);
  store.m_log.clear();

  const void* d = store.getData (branches.m_foo_id);
  assert (d != nullptr);
  assert (store.m_log.size() == 1);
  assert (&store.m_log[0].m_container == &branches);
  assert (store.m_log[0].m_auxid == branches.m_foo_id);
  assert (&store.m_log[0].m_branch == branches.m_br_foo);
  assert (std::string(store.m_log[0].m_cl->GetName()) == "AthContainersRootTest::Foo");
  assert (store.m_log[0].m_entry == 122);
  assert (store.m_log[0].m_vector == d);
}


int main()
{
  test1();
  test2();
  return 0;
}

