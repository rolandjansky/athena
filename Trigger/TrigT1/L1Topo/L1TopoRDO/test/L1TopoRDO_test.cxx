/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "L1TopoRDO/L1TopoRDO.h"
#include "L1TopoRDO/L1TopoRDOCollection.h"
#include "L1TopoRDO/Header.h"
#include "L1TopoRDO/Fibre.h"
#include "L1TopoRDO/Status.h"
#include "L1TopoRDO/L1TopoTOB.h"
#include "L1TopoRDO/Helpers.h"
#include "L1TopoRDO/Error.h"
#include "L1TopoRDO/BlockTypes.h"
#include "L1TopoRDO/ModuleID.h"
#include <iostream>
#include <cassert>
#include <cstdint>

// Check decoding of header word via example
void test1()
{
  std::cout << "** test1: L1Topo::Header decoding **\n";
  uint32_t word(0xc0460023); // a header block 
  L1Topo::Header h(word);
  std::cout << "L1Topo::Header created from word " << L1Topo::formatHex8(word) << std::endl;
  // check word was correctly copy-assigned internally
  assert (h.word() == word);
  // check word correctly decoded
  assert(L1Topo::blockType(h.word()) == L1Topo::BlockTypes::HEADER);
  assert(h.version() == 0);
  assert(h.active_fibres() == 0x46);
  assert(h.payload_crc() == 0);
  assert(h.fpga() == 1);
  assert(h.last_block() == 0);
  assert(h.bcn_sign() == 0);
  assert(h.bcn_offset() == 3);
}

// check encoding of header word via example
void test2()
{
  std::cout << "** test2: L1Topo::Header encoding **\n";
  uint32_t version(0x0);
  uint32_t active_fibres(0x42);
  uint32_t payload_crc(0x00);
  uint32_t fpga(1); 
  uint32_t last_block(1);
  uint32_t bcn_sign(1);
  uint32_t bcn_offset(1);
  L1Topo::Header h(version, active_fibres, payload_crc, fpga, last_block, bcn_sign, bcn_offset);
  uint32_t word(0xc0420039); // the header block that matches the above values 
  std::cout << "L1Topo::Header encoded word " << L1Topo::formatHex8(h.word()) << std::endl;
  std::cout << "L1Topo::Header     ref word " << L1Topo::formatHex8(word) << std::endl;
  assert (h.word() == word);
}

// check round trip decoding othen encoding of example header word
void test3()
{
  std::cout << "** test3: L1Topo::Header decode+encode **\n";
  uint32_t word(0xc0460023); // a header block 
  L1Topo::Header h1(word);
  L1Topo::Header h2(h1.version(), h1.active_fibres(), h1.payload_crc(), h1.fpga(), h1.last_block(), h1.bcn_sign(), h1.bcn_offset());
  std::cout << "L1Topo::Header  decoded from word " << L1Topo::formatHex8(word) << std::endl;
  std::cout << "L1Topo::Header re-encoded to word " << L1Topo::formatHex8(h2.word()) << std::endl;
  assert (h2.word() == word);
}

void test4()
{
  std::cout << "** test4: L1Topo::signedBCN all cases **\n";
  assert (L1Topo::signedBCN(0,0)==0);
  assert (L1Topo::signedBCN(1,0)==0);
  assert (L1Topo::signedBCN(0,1)==1);
  assert (L1Topo::signedBCN(1,1)==-1);
  assert (L1Topo::signedBCN(0,2)==2);
  assert (L1Topo::signedBCN(1,2)==-2);
}

// check decoding of L1TopoTOB by example
void test5()
{
  std::cout << "** test5: L1Topo::L1TopoTOB encoding **\n";
  uint32_t word(0x83001037); 
  L1Topo::L1TopoTOB c(word);
  std::cout << "L1Topo::L1TopoTOB created from word " << L1Topo::formatHex8(word) << std::endl;
  // check word was correctly copy-assigned internally
  assert (c.word() == word);
  // check word correctly decoded
  assert(L1Topo::blockType(c.word()) == L1Topo::BlockTypes::L1TOPO_TOB);
  assert(c.ctp_signal()==0x3);
  assert(c.overflow_bits()==0x10);
  assert(c.trigger_bits()==0x37);
}


// check encoding of L1TopoTOB by example
void test6()
{
  std::cout << "** test6: L1Topo::L1TopoTOB decoding **\n";
  uint32_t ctp_signal(0x4);
  uint32_t overflow_bits(0x80);
  uint32_t trigger_bits(0xff);
  L1Topo::L1TopoTOB c(ctp_signal, overflow_bits, trigger_bits);
  uint32_t word(0x840080ff);
  assert (c.word() == word);
}
// check L1TopoTOB error bits
void test6a()
{
  std::cout << "** test6: L1Topo::L1TopoTOB decoding **\n";
  uint32_t ctp_signal(0x4);
  uint32_t overflow_bits(0x80);
  uint32_t trigger_bits(0xff);

  std::cout << std::hex << std::showbase << "test params: ctp_signal " << ctp_signal << " overflow_bits " << overflow_bits << " trigger_bits " << trigger_bits << std::dec << std::noshowbase << std::endl;
  // try different patterns and check they are encoded and decoded correctly
  for (uint32_t pattern=0; pattern<=0x1f; ++pattern){
    std::cout << "  test pattern: CRC error_bits " << std::bitset<5>(pattern) << std::noboolalpha << std::endl;
    L1Topo::L1TopoTOB c(ctp_signal, overflow_bits, trigger_bits, pattern);
    assert (L1Topo::blockType(c.word()) == L1Topo::BlockTypes::L1TOPO_TOB);
    std::cout << "test TOB from pattern: " << c << std::endl;
    assert (c.crc_EM()     == ((pattern>>0) & 0x1));
    assert (c.crc_Tau()    == ((pattern>>1) & 0x1));
    assert (c.crc_Muon()   == ((pattern>>2) & 0x1));
    assert (c.crc_Jet()    == ((pattern>>3) & 0x1));
    assert (c.crc_Energy() == ((pattern>>4) & 0x1));
    assert (c.error_bits() == pattern);
    // merge pattern with word
    uint32_t word( 0x840080ff | (pattern << 16));
    std::cout << std::hex << std::showbase <<  " test word: " << word << std::dec << std::noshowbase << std::endl;
    assert (c.word() == word);
    L1Topo::L1TopoTOB d(word);
    std::cout << "test TOB from word: " << d << std::endl;
    assert (L1Topo::blockType(d.word()) == L1Topo::BlockTypes::L1TOPO_TOB);
    assert (d.ctp_signal()==ctp_signal);
    assert (d.overflow_bits()==overflow_bits);
    assert (d.trigger_bits()==trigger_bits);
    assert (c.error_bits() == d.error_bits() );
    assert (c.crc_EM()     == d.crc_EM()     );
    assert (c.crc_Tau()    == d.crc_Tau()    );
    assert (c.crc_Muon()   == d.crc_Muon()   );
    assert (c.crc_Jet()    == d.crc_Jet()    );
    assert (c.crc_Energy() == d.crc_Energy() );
  }

}


// check round trip decoding othen encoding of example L1TopoTOB word
void test7()
{
  std::cout << "** test7: L1Topo::L1TopoTOB encode+decode **\n";
  uint32_t word(0x82000112); 
  L1Topo::L1TopoTOB c1(word);
  L1Topo::L1TopoTOB c2(c1.ctp_signal(),c1.overflow_bits(),c1.trigger_bits());
  std::cout << "L1Topo::L1TopoTOB encoded from word " << L1Topo::formatHex8(c1.word()) << std::endl;
  std::cout << "L1Topo::L1TopoTOB decoded   to word " << L1Topo::formatHex8(c2.word()) << std::endl;
  assert (c2.word()==c1.word());
}

void test8()
{
  std::cout << "** test8: L1Topo::L1TopoTOB index, clock and fpga decoding **\n";
  L1Topo::L1TopoTOB c1(0x80000000); 
  assert(c1.index()==0);
  assert(c1.clock()==0);
  assert(c1.fpga()==0);
  L1Topo::L1TopoTOB c2(0x81000000); 
  assert(c2.index()==1);
  assert(c2.clock()==0);
  assert(c2.fpga()==0);
  L1Topo::L1TopoTOB c3(0x88000000); 
  assert(c3.index()==0);
  assert(c3.clock()==0);
  assert(c3.fpga()==1);
  L1Topo::L1TopoTOB c4(0x8b000000); 
  assert(c4.index()==1);
  assert(c4.clock()==1);
  assert(c4.fpga()==1);
}

void test9()
{
  std::cout << "** test9: L1Topo::getCTPbits **\n";
  // contrive a L1TOPO ROI RDO with some test data
  std::vector<uint32_t> data1({
        0x80000100, 
	0x81000201,
	0x82000330, 
	0x83000402, 
	0x88000510, 
	0x890006f6, 
	0x8a0007b0, 
	0x8b000800
	});
  L1TopoRDO rdo1;
  std::vector<uint32_t> data2({
        0x800010d1, 
	0x810020ab,
	0x82003002, 
	0x83004028, 
	0x88005000, 
	0x89006083, 
	0x8a007045, 
	0x8b008079
	});
  L1TopoRDO rdo2;
  rdo1.setDataWords(data1);
  rdo1.setSourceID(0x00910080); // module 0
  rdo2.setDataWords(data2);
  rdo2.setSourceID(0x00910090); // module 1
  std::cout << rdo1 << rdo2;
  // construct reference value - complicated by 128 bit length
  std::pair< std::bitset<128>, std::bitset<128> > test, ref;
  uint64_t ref1=0x55148b0000090a00;
  uint64_t ref2=0x6a8720224cc55109;
  ref.first  = std::bitset<128>(ref1) | (std::bitset<128>(ref2) << 64);
  uint64_t ref3=0x0094003b0024000b;
  uint64_t ref4=0x94003b0024000b00;
  ref.second = std::bitset<128>(ref3) | (std::bitset<128>(ref4) << 64);
  // compute bitset using helper function and compare to expected result
  L1TopoRDOCollection colRDOs;
  colRDOs.push_back(new L1TopoRDO(rdo1));
  colRDOs.push_back(new L1TopoRDO(rdo2));
  test = L1Topo::getDecisionAndOverflowBits(colRDOs);
  std::cout << "Trigger decision bits:\n  ref " << ref.first  << "\n test " << test.first  << std::endl;
  std::cout << "Trigger overflow bits:\n  ref " << ref.second << "\n test " << test.second << std::endl;
  assert(ref.first==test.first);   // trigger
  assert(ref.second==test.second); // overflow
  assert(ref==test); // both together
}

void test10()
{
  std::cout << "** test10: L1Topo::triggerBitIndex **\n";
  unsigned long ul(0);
  uint32_t u32(0);
  std::cout << "size of unsigned long " << sizeof(ul) << " size of uint32_t " << sizeof(u32) << std::endl;
  std::vector<uint32_t> modules({0x00910080,0x00910090});
  std::vector<uint32_t> rois({0x80000000, 0x81000000, 0x82000000, 0x83000000, 0x84000000, 0x85000000, 0x86000000, 0x87000000});
  std::cout << "module, ROIword, (index clock fpga), index, multiple of 8?" << std::endl;
  for (uint32_t m: modules){
    for (uint32_t r: rois){
      L1Topo::L1TopoTOB c(r);
      uint32_t index = L1Topo::triggerBitIndex(m,c);
      uint32_t module = (m >> 4) & 0x1;
      std::cout << module << " " << std::hex << std::showbase << r << std::dec << " ( "  
		<< c.index() << " "<< c.clock() << " " << c.fpga() << " ) -> " 
		<< index << " " << std::boolalpha << bool(index%8==0) << std::endl;
    }
  }
}

void test11()
{
    std::cout << "** test11: L1Topo::Error **\n";
    L1Topo::Error e1(L1Topo::Error::ROB_ERROR);
    L1Topo::Error e2 = L1Topo::Error::ROD_ERROR;
    std::vector<L1Topo::Error> v1 = {L1Topo::Error::ROB_ERROR, L1Topo::Error::SLINK_STATUS_ERROR};
    std::cout << static_cast<int>(e1) << " " << e1 << std::endl;
    std::cout << static_cast<int>(e2) << " " << e2 << std::endl;
    std::cout << v1 << std::endl;
    L1Topo::Error e3( static_cast<L1Topo::Error>(99));
    std::cout << static_cast<int>(e3) << " " << e3 << std::endl;

    L1TopoRDO r;
    r.setError(L1Topo::Error::ROB_ERROR);
    r.setError(L1Topo::Error::SLINK_STATUS_ERROR);
    assert(r.checkError(L1Topo::Error::ROB_ERROR));
    assert(! r.checkError(L1Topo::Error::ROD_ERROR));
    auto v2 = r.getErrors();
    std::cout << v2 << std::endl;
    assert(v2.size()==2);
}

void test12()
{
  std::cout << "** test12: L1Topo::Fibre encode+decode **\n";
  std::vector<uint32_t> status = { 0, 1, 0, 0, 0  }; 
  std::vector<uint32_t>  count = { 0, 1, 0, 6, 13 };
  uint32_t word(0xd0440668); // word that matches the above values 0x1101 0000 0100 0100 0000 0110 0110 1000 = 0xd0440668
  L1Topo::Fibre f1(status,count);
  std::cout << "Fibre with status and sizes " << f1 << std::endl;  
  std::cout << "Fibre word encoded from these " << L1Topo::formatHex8(f1.word()) << std::endl;
  std::cout << "Compare to word " << L1Topo::formatHex8(word) << std::endl;
  assert (f1.word()==word);
  L1Topo::Fibre f2(word);
  L1Topo::Fibre f3(f2.status(),f2.count());
  std::cout << "L1Topo::Fibre  decoded from word " << L1Topo::formatHex8(f2.word()) << std::endl;
  std::cout << "L1Topo::Fibre re-encoded to word " << L1Topo::formatHex8(f3.word()) << std::endl;
  assert (f3.word() == f2.word());
}

void test13()
{
  std::cout << "** test13: L1Topo::Status encode+decode **\n";
  assert (L1Topo::Status(1,1).word()==uint32_t(0xec000000));
  assert (L1Topo::Status(1,0).word()==uint32_t(0xe8000000));
  assert (L1Topo::Status(0,1).word()==uint32_t(0xe4000000));
  assert (L1Topo::Status(0,0).word()==uint32_t(0xe0000000));
  L1Topo::Status s1(0xe8000000);
  L1Topo::Status s2(s1.overflow(),s1.crc());
  std::cout << "L1Topo::Status  decoded from word " << L1Topo::formatHex8(s1.word()) << std::endl;
  std::cout << "L1Topo::Status re-encoded to word " << L1Topo::formatHex8(s2.word()) << std::endl;
  assert (s1.word() == s2.word());

}


void test14()
{
  std::cout << "** test14: L1Topo::ModuleID encode+decode **\n";
  // Test 4 actual use cases
  L1Topo::ModuleID a(0x0000);
  std::cout << a << std::endl;
  assert (a.module()==0 && a.link()==0 && a.isDAQ());

  L1Topo::ModuleID b(0x0081);
  std::cout << b << std::endl;
  assert (b.module()==0 && b.link()==1 && b.isROI());

  L1Topo::ModuleID c(static_cast<uint16_t>(0x00910010));
  std::cout << c << std::endl;
  assert (c.module()==1 && c.link()==0 && c.isDAQ());

  L1Topo::ModuleID d(0x00910091); // expect build warning due to implicit truncation to 16 bits
  std::cout << d << std::endl;
  assert (d.module()==1 && d.link()==1 && d.isROI());

  L1Topo::ModuleID e(0x0092);
  std::cout << e << std::endl;
  assert (e.module()==1 && e.link()==2 && e.isROI());

  L1Topo::ModuleID f(0,0,0); // link, module, roiDaq
  std::cout << f << std::endl;
  assert(f.id()==a.id());

  L1Topo::ModuleID g(1,0,1);
  std::cout << g << std::endl;
  assert(g.id()==b.id());

  L1Topo::ModuleID h(0,1,0);
  std::cout << h << std::endl;
  assert(h.id()==c.id());

  L1Topo::ModuleID i(1,1,1); 
  std::cout << i << std::endl;
  assert(i.id()==d.id());

  L1Topo::ModuleID j(2,1,1); // link, module, roiDaq
  std::cout << j << std::endl;
  assert(j.id()==e.id());

  // check isDAQ and isROI are opposites
  assert(f.isDAQ() != f.isROI());
  assert(h.isDAQ() != h.isROI());

  // check use in L1TopoRDO also works
  L1TopoRDO r1,r2;
  r1.setSourceID(0x00910081);
  r2.setSourceID(0x00910010);
  assert(r1.isROIModule());
  assert(r2.isDAQModule());
}

int main()
{ 
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test6a();
  test7();
  test8();
  test9();
  test10();
  test11();
  test12();
  test13();
  test14();
  return 0;
}
