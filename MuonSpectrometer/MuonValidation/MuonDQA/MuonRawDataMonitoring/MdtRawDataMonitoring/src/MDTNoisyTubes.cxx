/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
//Tool to mask Noisy Tubes
//Nov. 2009
//Author Justin Griffiths <griffith@cern.ch>
///////////////////////////////////////////////////////////////////////////

#include "MdtRawDataMonitoring/MDTNoisyTubes.h"

using std::map;
using std::string;
using std::set;


MDTNoisyTubes::~MDTNoisyTubes(){}

bool MDTNoisyTubes::isNoisy(const Muon::MdtPrepData* mdtCollection){

  set<Identifier> noisyTubes;
  map<IdentifierHash, set<Identifier> >::const_iterator itr = m_noise_map.find(mdtCollection->collectionHash());
  if(itr == m_noise_map.end()) return false;
  else noisyTubes = itr->second;

  if( noisyTubes.find(mdtCollection->identify()) != noisyTubes.end() ) return true;
  else return false;
}

set<Identifier> MDTNoisyTubes::getNoiseList(IdentifierHash idHash){

  set<Identifier> noisyTubes;
  map<IdentifierHash, set<Identifier> >::const_iterator itr = m_noise_map.find(idHash);
  if(itr == m_noise_map.end()) return noisyTubes;
  
  return itr->second;

}

MDTNoisyTubes::MDTNoisyTubes(bool doMask){
  //This Initializer Automatically Generated w/
  //MdtRawDataValAlg::mdtchamberId() 
  //To Regenerate w/ new list simply uncomment the relevant portion of the code
  //In the above function and place in your run directory a file named "tubes.txt"
  //Which should be of the form:
  //[chambername] [int],[int],[int]
  //Where the ints are tube numbers as generated w/:
  //int mdttube= m_mdtIdHelper->tube(*i) + (mdtlayer-1) * m_mdtIdHelper->tubeMax(*i);
  //ChamberTubeNumberCorrection(mdttube, hardware_name, m_mdtIdHelper->tube(*i), mdtlayer-1)
  //Output will be in your run directory called out.cxx

  if(!doMask) return;//Option to turn off tube masking

  IdentifierHash idHash;
  std::set<Identifier> tubes;

  //This noisy tube list was made using:
  //data15_13TeV.00267639.physics_Main.merge.HIST.f603_h51/data15_13TeV.00267639.physics_Main.merge.HIST.f603_h51._0001.1
  
  unsigned long long id = 0;
  
  //  BIL6A05 	 //
  idHash = 68; 
 id = 0x6072046000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #36
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BIL6A09 	 //
  idHash = 70; 
 id = 0x6074046000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #36
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BIS6C06 	 //
  idHash = 90; 
 id = 0x6112704000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #213
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BML2C09 	 //
  idHash = 236; 
 id = 0x623456e000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #280
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BML1C01 	 //
  idHash = 240; 
 id = 0x623853e000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #160
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BMS1C02 	 //
  idHash = 326; 
 id = 0x6338218000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #125
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BMS1C06 	 //
  idHash = 328; 
 id = 0x633a06a000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #54
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BMS1C16 	 //
  idHash = 331; 
 id = 0x633f06e000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #56
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BMS1A08 	 //
  idHash = 335; 
 id = 0x634b020000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #17
 id = 0x634b068000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #53
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BMS1A10 	 //
  idHash = 336; 
 id = 0x634c42e000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #192
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BMS1A16 	 //
  idHash = 337; 
 id = 0x634f200000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #113
 id = 0x634f62e000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #304
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOL6A13 	 //
  idHash = 463; 
 id = 0x6476600000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #281
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOL7A13 	 //
  idHash = 465; 
 id = 0x647e118000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #85
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOS6C02 	 //
  idHash = 466; 
 id = 0x651045e000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #240
 id = 0x6510660000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #369
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOS6C04 	 //
  idHash = 467; 
 id = 0x651155c000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #303
 id = 0x6511618000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #333
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOS6C08 	 //
  idHash = 469; 
 id = 0x651325a000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #174
 id = 0x6513608000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #325
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOS5C08 	 //
  idHash = 475; 
 id = 0x651b200000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #145
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOS2C10 	 //
  idHash = 494; 
 id = 0x6534624000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #379
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOS2A16 	 //
  idHash = 513; 
 id = 0x6557400000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #217
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOS4A04 	 //
  idHash = 521; 
 id = 0x6561200000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #145
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOS4A10 	 //
  idHash = 524; 
 id = 0x656458c000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #359
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOS4A16 	 //
  idHash = 525; 
 id = 0x6567000000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #1
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOS6A02 	 //
  idHash = 532; 
 id = 0x657024a000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #166
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOS6A04 	 //
  idHash = 533; 
 id = 0x657165c000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #367
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BIR2A15 	 //
  idHash = 585; 
 id = 0x675751e000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #166
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BMF1C12 	 //
  idHash = 598; 
 id = 0x683d07a000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #62
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BMF1A12 	 //
  idHash = 600; 
 id = 0x684d000000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #1
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BMF1A14 	 //
  idHash = 601; 
 id = 0x684e270000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #201
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOF1A12 	 //
  idHash = 614; 
 id = 0x694d616000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #372
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BOF5A14 	 //
  idHash = 619; 
 id = 0x695e02c000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #23
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  EIL1A05 	 //
  idHash = 676; 
 id = 0x6b4a708000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #257
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  EIL2A15 	 //
  idHash = 689; 
 id = 0x6b57046000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #36
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  EML5C11 	 //
  idHash = 775; 
 id = 0x6e1d43a000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #222
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  EML1C11 	 //
  idHash = 807; 
 id = 0x6e3d264000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #163
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  EML1A13 	 //
  idHash = 816; 
 id = 0x6e4e53c000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #255
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  EML2A13 	 //
  idHash = 824; 
 id = 0x6e56512000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #266
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  EML2A15 	 //
  idHash = 825; 
 id = 0x6e57612000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #330
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  EMS3C10 	 //
  idHash = 870; 
 id = 0x6f2c17a000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #126
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  EMS2A08 	 //
  idHash = 901; 
 id = 0x6f53410000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #201
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  EMS2A14 	 //
  idHash = 904; 
 id = 0x6f56578000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #317
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  EMS3A02 	 //
  idHash = 906; 
 id = 0x6f58274000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #187
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  //  BME1C14 	 //
  idHash = 1174; 
 id = 0x7e3e374000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #293
 id = 0x7e3e474000000000;
  tubes.insert(static_cast<Identifier>(id)); // hist bin #293
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

}


  
