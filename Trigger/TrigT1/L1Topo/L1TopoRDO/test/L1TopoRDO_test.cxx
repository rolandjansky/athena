/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "L1TopoRDO/L1TopoRDO.h"
#include "L1TopoRDO/L1TopoRDOCollection.h"
#include "L1TopoRDO/Header.h"
#include "L1TopoRDO/L1TopoTOB.h"
#include "L1TopoRDO/Helpers.h"
#include <iostream>
#include <cassert>
#include <cstdint>

// Check decoding of header word via example
void test1()
{
  std::cout << "** test1: L1Topo::Header decoding **\n";
  uint32_t word(0xc0460023); // a header block 
  L1Topo::Header h(word);
  std::cout << "L1Topo::Header created from word" << L1Topo::formatHex8(word) << std::endl;
  // check word was correctly copy-assigned internally
  assert (h.word() == word);
  // check word correctly decoded
  assert(L1Topo::blockType(h.word()) == L1Topo::BlockTypes::HEADER);
  assert(h.version() == 0);
  assert(h.active_fibres() == 0x46);
  assert(h.payload_crc() == 0);
  assert(h.fpga() == 1);
  assert(h.last_block() == 0);
  assert(h.bcn_sign() == 1);
  assert(h.bcn_offset() == 1);
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
  uint32_t bcn_sign(0);
  uint32_t bcn_offset(2);
  L1Topo::Header h(version, active_fibres, payload_crc, fpga, last_block, bcn_sign, bcn_offset);
  uint32_t word(0xc0420034); // the header block that matches the above values 
  std::cout << "L1Topo::Header encoded word" << L1Topo::formatHex8(h.word()) << std::endl;
  std::cout << "L1Topo::Header     ref word" << L1Topo::formatHex8(word) << std::endl;
  assert (h.word() == word);
}

// check round trip decoding othen encoding of example header word
void test3()
{
  std::cout << "** test3: L1Topo::Header decode+encode **\n";
  uint32_t word(0xc0460023); // a header block 
  L1Topo::Header h1(word);
  L1Topo::Header h2(h1.version(), h1.active_fibres(), h1.payload_crc(), h1.fpga(), h1.last_block(), h1.bcn_sign(), h1.bcn_offset());
  std::cout << "L1Topo::Header  decoded from word" << L1Topo::formatHex8(word) << std::endl;
  std::cout << "L1Topo::Header re-encoded to word" << L1Topo::formatHex8(h2.word()) << std::endl;
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
  std::cout << "L1Topo::L1TopoTOB created from word" << L1Topo::formatHex8(word) << std::endl;
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
  uint32_t ctp_signal(0x84);
  uint32_t overflow_bits(0x80);
  uint32_t trigger_bits(0xff);
  L1Topo::L1TopoTOB c(ctp_signal, overflow_bits, trigger_bits);
  uint32_t word(0x840080ff);
  assert (c.word() == word);
}

// check round trip decoding othen encoding of example L1TopoTOB word
void test7()
{
  std::cout << "** test7: L1Topo::L1TopoTOB encode + decode **\n";
  uint32_t word(0x82000112); 
  L1Topo::L1TopoTOB c1(word);
  L1Topo::L1TopoTOB c2(c1.ctp_signal(),c1.overflow_bits(),c1.trigger_bits());
  std::cout << "L1Topo::L1TopoTOB encoded from word" << L1Topo::formatHex8(c1.word()) << std::endl;
  std::cout << "L1Topo::L1TopoTOB decoded   to word" << L1Topo::formatHex8(c2.word()) << std::endl;
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
  L1Topo::L1TopoTOB c3(0x84000000); 
  assert(c3.index()==0);
  assert(c3.clock()==0);
  assert(c3.fpga()==1);
  L1Topo::L1TopoTOB c4(0x87000000); 
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
	0x84000510, 
	0x850006f6, 
	0x860007b0, 
	0x87000800
	});
  L1TopoRDO rdo1;
  std::vector<uint32_t> data2({
        0x800010d1, 
	0x810020ab,
	0x82003002, 
	0x83004028, 
	0x84005000, 
	0x85006083, 
	0x86007045, 
	0x87008079
	});
  L1TopoRDO rdo2;
  rdo1.setDataWords(data1);
  rdo1.setSourceID(0x00910080); // module 0
  rdo2.setDataWords(data2);
  rdo2.setSourceID(0x00910090); // module 1
  std::cout << rdo1 << rdo2;
  // construct reference value - complicated by 128 bit length
  std::pair< std::bitset<128>, std::bitset<128> > test, ref;
  uint64_t ref1=0x00b0f61002300100;
  uint64_t ref2=0x794583002802abd1;
  ref.first  = std::bitset<128>(ref1) | (std::bitset<128>(ref2) << 64);
  uint64_t ref3=0x0807060504030201;
  uint64_t ref4=0x8070605040302010;
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
      std::cout << module << " " << "0x" << std::hex << std::showbase << r << std::dec << " ( "  
		<< c.index() << " "<< c.clock() << " " << c.fpga() << " ) -> " 
		<< index << " " << std::boolalpha << bool(index%8==0) << std::endl;
    }
  }
}

int main()
{ 
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  test9();
  test10();
  return 0;
}
