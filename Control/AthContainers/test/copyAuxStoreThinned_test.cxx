/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthContainers/test/copyAuxStoreThinned_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Regression tests for copyAuxStoreThinned
 */


#undef NDEBUG

#include "AthContainers/tools/copyAuxStoreThinned.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/PackedContainer.h"
#include "AthenaKernel/ThinningDecisionBase.h"
#include "CxxUtils/FloatCompressor.h"
#include <vector>
#include <iostream>
#include <cassert>


class AuxStoreTest
  : public SG::AuxStoreInternal
{
public:
  AuxStoreTest() {}
  void suppress (SG::auxid_t auxid)
  {
    m_suppressed.insert (auxid);
    m_selected = getAuxIDs();
    for (SG::auxid_t supid : m_suppressed)
      m_selected.erase (supid);
  }

  virtual SG::auxid_set_t getSelectedAuxIDs() const
  {
    if (m_suppressed.empty()) return getAuxIDs();
    return m_selected;
  }

private:
  SG::auxid_set_t m_suppressed;
  SG::auxid_set_t m_selected;
};


void compare (const SG::PackedParameters& a,
              const SG::PackedParameters& b)
{
  assert (a.nbits() == b.nbits());
  assert (a.isFloat() == b.isFloat());
  assert (a.isSigned() == b.isSigned());
  assert (a.rounding() == b.rounding());
  assert (a.nmantissa() == b.nmantissa());
  assert (a.scale() == b.scale());
}


void compare (const SG::AuxStoreInternal& a,
              const SG::AuxStoreInternal& b,
              bool thinned = false,
              SG::auxid_t suppressed = SG::null_auxid)
{
  if (thinned) {
    assert (a.size()/2 == b.size());
  }
  else {
    assert (a.size() == b.size());
  }

  const SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

  for (SG::auxid_t id : a.getAuxIDs()) {
    if (id == suppressed) {
      assert (b.getAuxIDs().count(id) == 0);
      continue;
    }
    const char* aptr = reinterpret_cast<const char*>(a.getData(id));
    const char* bptr = reinterpret_cast<const char*>(b.getData(id));
    assert (aptr != 0 && bptr != 0);

    size_t sz = reg.getEltSize(id);
    for (size_t ia=0, ib=0; ia < a.size(); ++ia) {
      if (!thinned || ia%2 == 0) {
        assert (memcmp (aptr + ia*sz , bptr + ib*sz, sz) == 0);
        ++ib;
      }
    }

    assert (a.getIOType(id) == b.getIOType(id));
    if (a.getIOType(id) == &typeid(SG::PackedContainer<int>)) {
      const SG::PackedContainer<int>* ap
        = reinterpret_cast<const SG::PackedContainer<int>*> (a.getIOData(id));
      const SG::PackedContainer<int>* bp
        = reinterpret_cast<const SG::PackedContainer<int>*> (b.getIOData(id));
      compare (ap->parms(), bp->parms());
    }
    else if (a.getIOType(id) == &typeid(SG::PackedContainer<float>)) {
      const SG::PackedContainer<float>* ap
        = reinterpret_cast<const SG::PackedContainer<float>*> (a.getIOData(id));
      const SG::PackedContainer<float>* bp
        = reinterpret_cast<const SG::PackedContainer<float>*> (b.getIOData(id));
      compare (ap->parms(), bp->parms());
    }
  }
}


void test1()
{
  std::cout << "test1\n";
  SG::ThinningDecisionBase dec;
  SG::ThinningInfo info;
  info.m_decision = &dec;

  AuxStoreTest src;
  SG::AuxStoreInternal dst;

  copyAuxStoreThinned (src, dst, nullptr);
  compare (src, dst);

  copyAuxStoreThinned (src, dst, &info);
  compare (src, dst);

  SG::auxid_t ityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  SG::auxid_t ftyp = SG::AuxTypeRegistry::instance().getAuxID<float> ("aFloat");
  SG::auxid_t pityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("pInt");
  SG::auxid_t pftyp = SG::AuxTypeRegistry::instance().getAuxID<float> ("pFloat");

  int* iptr = reinterpret_cast<int*> (src.getData (ityp, 10, 10));
  float* fptr = reinterpret_cast<float*> (src.getData (ftyp, 10, 10));
  int* piptr = reinterpret_cast<int*> (src.getData (pityp, 10, 10));
  float* pfptr = reinterpret_cast<float*> (src.getData (pftyp, 10, 10));

  src.setOption (pityp, SG::AuxDataOption ("nbits", 10));
  src.setOption (pityp, SG::AuxDataOption ("signed", false));
  src.setOption (pftyp, SG::AuxDataOption ("nbits", 12));
  src.setOption (pftyp, SG::AuxDataOption ("signed", true));
  src.setOption (pftyp, SG::AuxDataOption ("rounding", true));
  src.setOption (pftyp, SG::AuxDataOption ("nmantissa", 8));
  src.setOption (pftyp, SG::AuxDataOption ("scale", 2.0));

  for (int i=0; i < 10; i++) {
    iptr[i] = i;
    fptr[i] = 10*i + 0.5;
    piptr[i] = i + 13;
    pfptr[i] = 10*i + 0.5 + 13;
  }

  dec.resize (10);

  for (int i=0; i < 10; ++i) {
    if (i%2 != 0) {
      dec.thin (i);
    }
  }

  dec.buildIndexMap();

  copyAuxStoreThinned (src, dst, &info);
  compare (src, dst, true);

  SG::AuxStoreInternal dst2;
  info.m_vetoed.insert (ftyp);
  copyAuxStoreThinned (src, dst2, &info);
  compare (src, dst2, true, ftyp);

  info.m_vetoed.clear();
  SG::AuxStoreInternal dst3;
  src.suppress (ftyp);
  copyAuxStoreThinned (src, dst2, &info);
  compare (src, dst2, true, ftyp);

}

void test2()
{
  std::cout << std::endl;
  std::cout << "Testing lossy float compression..." << std::endl;
  std::cout << std::endl;

  // Prepare the necessary bits and pieces
  SG::ThinningDecisionBase dec;
  SG::ThinningInfo info;
  info.m_decision = &dec;
  dec.resize (5);
  dec.buildIndexMap();
  AuxStoreTest src;

  // We want two types: float and std::vector<float>
  SG::auxid_t ftyp = SG::AuxTypeRegistry::instance().getAuxID<float> ("aFloat");
  SG::auxid_t fvtyp = SG::AuxTypeRegistry::instance().getAuxID<std::vector<float>> ("aVecFloat");

  // Get a handle on the underlying data
  float* fptr = reinterpret_cast<float*> (src.getData (ftyp, 5, 5));
  std::vector<float>* fvptr = reinterpret_cast<std::vector<float>*> (src.getData (fvtyp, 5, 5));

  // Fill some random valus
  for (int i=0; i < 5; i++) {
    fptr[i] = i + 0.512345;
    fvptr[i].push_back(i + 0.512345);
    fvptr[i].push_back(i*2 + 0.123456);
  }


  // Compress the values w/ nmantissa bits
  const unsigned int nmantissa = 7;
  info.m_compression[nmantissa].insert(ftyp);
  info.m_compression[nmantissa].insert(fvtyp);
  SG::AuxStoreInternal dst1;
  copyAuxStoreThinned (src, dst1, &info);

  // Retrieve the compressed values
  float* rfptr = reinterpret_cast<float*> (dst1.getData (ftyp, 5, 5));
  std::vector<float>* rfvptr = reinterpret_cast<std::vector<float>*> (dst1.getData (fvtyp, 5, 5));

  // Instentiate a new compressor and check against the result
  const CxxUtils::FloatCompressor fc( nmantissa );
  CxxUtils::FloatCompressor::floatint_t uni;
  uint32_t result{0};

  // Helper to print int in binary w/ a nicer format
  auto intToBinStr = [] (uint32_t val)
  {
    std::string s = std::bitset<32>(val).to_string();
    return (s.substr(0,1) + " " + s.substr(1,8) + " " + s.substr(9,32));
  };

  // Check the compresion results
  for (int i = 0; i < 5; i++) {
    std::cout << "Iteration [" << i << "]" << std::endl;
    std::cout << std::endl;

    // Testing float

    // Print the original value
    uni.fvalue = fptr[i];
    std::cout << "Original value    " << std::dec << std::setw( 15 ) << uni.fvalue
              << " (binary : " << intToBinStr(uni.ivalue) << ")" << std::endl;
    // Print the compressed value
    uni.fvalue = rfptr[i];
    std::cout << "Compressed value  " << std::dec << std::setw( 15 ) << uni.fvalue
              << " (binary : " << intToBinStr(uni.ivalue) << ")" << std::endl;
    result = uni.ivalue;
    // Check against by-hand compression
    uni.fvalue = fc.reduceFloatPrecision( fptr[i] );
    assert( result == uni.ivalue );

    // Testing std::vector<float>

    std::vector<float>* invec = fvptr + i;
    std::vector<float>* outvec = rfvptr + i;
    assert(invec != nullptr && outvec != nullptr);
    assert(invec->size() == 2 && outvec->size() == 2);

    for(int j = 0; j < 2; j++) {
      // Print the original value
      uni.fvalue = invec->at(j);
      std::cout << "Original value at [" << j << "]    " << std::dec << std::setw( 8 ) << uni.fvalue
                << " (binary : " << intToBinStr(uni.ivalue) << ")" << std::endl;
      // Print the compressed value
      uni.fvalue = outvec->at(j);
      std::cout << "Compressed value at [" << j << "]  " << std::dec << std::setw( 8 ) << uni.fvalue
                << " (binary : " << intToBinStr(uni.ivalue) << ")" << std::endl;
      result = uni.ivalue;
      // Check against by-hand compression
      uni.fvalue = fc.reduceFloatPrecision( invec->at(j) );
      assert( result == uni.ivalue );
    }

    std::cout << std::endl;
  }

}

int main()
{
  test1();
  test2();
  return 0;
}
