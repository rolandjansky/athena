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
  map<IdentifierHash, set<Identifier> >::const_iterator m_itr = m_noise_map.find(mdtCollection->collectionHash());
  if(m_itr == m_noise_map.end()) return false;
  else noisyTubes = m_itr->second;

  if( noisyTubes.find(mdtCollection->identify()) != noisyTubes.end() ) return true;
  else return false;
}

set<Identifier> MDTNoisyTubes::getNoiseList(IdentifierHash idHash){

  set<Identifier> noisyTubes;
  map<IdentifierHash, set<Identifier> >::const_iterator m_itr = m_noise_map.find(idHash);
  if(m_itr == m_noise_map.end()) return noisyTubes;
  
  return m_itr->second;

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

  /**********BIL6C13**********/
  idHash = 5; 
  tubes.insert( (Identifier) 1612083264); // hist bin #213
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BIL4C01**********/
  idHash = 12; 
  tubes.insert( (Identifier) 1612738656); // hist bin #256
  tubes.insert( (Identifier) 1612739680); // hist bin #288
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BIL2C03**********/
  idHash = 25; 
  tubes.insert( (Identifier) 1613853536); // hist bin #280
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BIL2C07**********/
  idHash = 27; 
  tubes.insert( (Identifier) 1613984416); // hist bin #274
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BIL2C09**********/
  idHash = 28; 
  tubes.insert( (Identifier) 1614049824); // hist bin #270
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BIL1A13**********/
  idHash = 41; 
  tubes.insert( (Identifier) 1615754112); // hist bin #239
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BIL2A13**********/
  idHash = 47; 
  tubes.insert( (Identifier) 1616277984); // hist bin #268
  tubes.insert( (Identifier) 1616278016); // hist bin #269
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BIL6A05**********/
  idHash = 68; 
  tubes.insert( (Identifier) 1618112736); // hist bin #260
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BIS1A12**********/
  idHash = 141; 
  tubes.insert( (Identifier) 1632436864); // hist bin #21
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BIS4A14**********/
  idHash = 166; 
  tubes.insert( (Identifier) 1634074944); // hist bin #11
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML4C03**********/
  idHash = 217; 
  tubes.insert( (Identifier) 1646329856); // hist bin #1
  tubes.insert( (Identifier) 1646333952); // hist bin #41
  tubes.insert( (Identifier) 1646338048); // hist bin #81
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML4C09**********/
  idHash = 220; 
  tubes.insert( (Identifier) 1646535904); // hist bin #120
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML4C15**********/
  idHash = 223; 
  tubes.insert( (Identifier) 1646732096); // hist bin #107
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML3C05**********/
  idHash = 226; 
  tubes.insert( (Identifier) 1646919776); // hist bin #4
  tubes.insert( (Identifier) 1646920160); // hist bin #16
  tubes.insert( (Identifier) 1646923968); // hist bin #63
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML3C09**********/
  idHash = 228; 
  tubes.insert( (Identifier) 1647051008); // hist bin #9
  tubes.insert( (Identifier) 1647076320); // hist bin #312
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML3C11**********/
  idHash = 229; 
  tubes.insert( (Identifier) 1647116768); // hist bin #16
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML2C09**********/
  idHash = 236; 
  tubes.insert( (Identifier) 1647575264); // hist bin #8
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML1C01**********/
  idHash = 240; 
  tubes.insert( (Identifier) 1647842272); // hist bin #64
  tubes.insert( (Identifier) 1647858656); // hist bin #160
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML1C07**********/
  idHash = 243; 
  tubes.insert( (Identifier) 1648041984); // hist bin #65
  tubes.insert( (Identifier) 1648058368); // hist bin #161
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML1A15**********/
  idHash = 255; 
  tubes.insert( (Identifier) 1649344768); // hist bin #9
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML2A01**********/
  idHash = 256; 
  tubes.insert( (Identifier) 1649415872); // hist bin #111
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML2A05**********/
  idHash = 258; 
  tubes.insert( (Identifier) 1649557984); // hist bin #184
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML5A03**********/
  idHash = 281; 
  tubes.insert( (Identifier) 1651053792); // hist bin #80
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML5A15**********/
  idHash = 287; 
  tubes.insert( (Identifier) 1651458048); // hist bin #121
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML6A03**********/
  idHash = 289; 
  tubes.insert( (Identifier) 1651580928); // hist bin #97
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BML6A11**********/
  idHash = 293; 
  tubes.insert( (Identifier) 1651852224); // hist bin #175
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS6C06**********/
  idHash = 298; 
  tubes.insert( (Identifier) 1662140416); // hist bin #145
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS6C10**********/
  idHash = 300; 
  tubes.insert( (Identifier) 1662279680); // hist bin #241
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS5C02**********/
  idHash = 302; 
  tubes.insert( (Identifier) 1662522336); // hist bin #64
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS5C04**********/
  idHash = 303; 
  tubes.insert( (Identifier) 1662583776); // hist bin #32
  tubes.insert( (Identifier) 1662591872); // hist bin #93
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS5C06**********/
  idHash = 304; 
  tubes.insert( (Identifier) 1662649088); // hist bin #25
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS4C02**********/
  idHash = 308; 
  tubes.insert( (Identifier) 1663057920); // hist bin #145
  tubes.insert( (Identifier) 1663063520); // hist bin #240
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS4C04**********/
  idHash = 309; 
  tubes.insert( (Identifier) 1663123456); // hist bin #145
  tubes.insert( (Identifier) 1663123488); // hist bin #146
  tubes.insert( (Identifier) 1663132320); // hist bin #262
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS4C16**********/
  idHash = 313; 
  tubes.insert( (Identifier) 1663500576); // hist bin #10
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS2C02**********/
  idHash = 320; 
  tubes.insert( (Identifier) 1664106496); // hist bin #145
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS2C04**********/
  idHash = 321; 
  tubes.insert( (Identifier) 1664163840); // hist bin #97
  tubes.insert( (Identifier) 1664165056); // hist bin #135
  tubes.insert( (Identifier) 1664172032); // hist bin #145
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS2C08**********/
  idHash = 323; 
  tubes.insert( (Identifier) 1664286976); // hist bin #9
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS2C16**********/
  idHash = 325; 
  tubes.insert( (Identifier) 1664557056); // hist bin #97
  tubes.insert( (Identifier) 1664565280); // hist bin #146
  tubes.insert( (Identifier) 1664569344); // hist bin #193
  tubes.insert( (Identifier) 1664573440); // hist bin #241
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS1C02**********/
  idHash = 326; 
  tubes.insert( (Identifier) 1664616096); // hist bin #54
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS1C04**********/
  idHash = 327; 
  tubes.insert( (Identifier) 1664702176); // hist bin #280
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS1C06**********/
  idHash = 328; 
  tubes.insert( (Identifier) 1664747168); // hist bin #54
  tubes.insert( (Identifier) 1664747200); // hist bin #55
  tubes.insert( (Identifier) 1664747232); // hist bin #56
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS1A02**********/
  idHash = 332; 
  tubes.insert( (Identifier) 1665671808); // hist bin #133
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS1A04**********/
  idHash = 333; 
  tubes.insert( (Identifier) 1665737632); // hist bin #142
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS1A08**********/
  idHash = 335; 
  tubes.insert( (Identifier) 1665860096); // hist bin #17
  tubes.insert( (Identifier) 1665861024); // hist bin #46
  tubes.insert( (Identifier) 1665861248); // hist bin #53
  tubes.insert( (Identifier) 1665861280); // hist bin #54
  tubes.insert( (Identifier) 1665864288); // hist bin #76
  tubes.insert( (Identifier) 1665868384); // hist bin #132
  tubes.insert( (Identifier) 1665869184); // hist bin #157
  tubes.insert( (Identifier) 1665876384); // hist bin #182
  tubes.insert( (Identifier) 1665880544); // hist bin #240
  tubes.insert( (Identifier) 1665881824); // hist bin #280
  tubes.insert( (Identifier) 1665884672); // hist bin #297
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS1A10**********/
  idHash = 336; 
  tubes.insert( (Identifier) 1665941504); // hist bin #169
  tubes.insert( (Identifier) 1665942016); // hist bin #185
  tubes.insert( (Identifier) 1665942240); // hist bin #192
  tubes.insert( (Identifier) 1665942272); // hist bin #193
  tubes.insert( (Identifier) 1665942304); // hist bin #194
  tubes.insert( (Identifier) 1665942400); // hist bin #197
  tubes.insert( (Identifier) 1665942528); // hist bin #201
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS2A02**********/
  idHash = 338; 
  tubes.insert( (Identifier) 1666212128); // hist bin #250
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS2A08**********/
  idHash = 341; 
  tubes.insert( (Identifier) 1666392064); // hist bin #97
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS2A10**********/
  idHash = 342; 
  tubes.insert( (Identifier) 1666471200); // hist bin #234
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS2A16**********/
  idHash = 343; 
  tubes.insert( (Identifier) 1666667712); // hist bin #231
  tubes.insert( (Identifier) 1666671264); // hist bin #262
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS3A04**********/
  idHash = 345; 
  tubes.insert( (Identifier) 1666802976); // hist bin #282
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS3A16**********/
  idHash = 349; 
  tubes.insert( (Identifier) 1667175904); // hist bin #96
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS4A10**********/
  idHash = 354; 
  tubes.insert( (Identifier) 1667514368); // hist bin #145
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS4A16**********/
  idHash = 355; 
  tubes.insert( (Identifier) 1667702784); // hist bin #97
  tubes.insert( (Identifier) 1667711712); // hist bin #168
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS5A02**********/
  idHash = 356; 
  tubes.insert( (Identifier) 1667760928); // hist bin #26
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS5A04**********/
  idHash = 357; 
  tubes.insert( (Identifier) 1667843040); // hist bin #128
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS5A08**********/
  idHash = 359; 
  tubes.insert( (Identifier) 1667957728); // hist bin #32
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS5A10**********/
  idHash = 360; 
  tubes.insert( (Identifier) 1668031136); // hist bin #86
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS6A02**********/
  idHash = 362; 
  tubes.insert( (Identifier) 1668300800); // hist bin #145
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS6A06**********/
  idHash = 364; 
  tubes.insert( (Identifier) 1668416704); // hist bin #39
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS6A08**********/
  idHash = 365; 
  tubes.insert( (Identifier) 1668489216); // hist bin #97
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMS6A16**********/
  idHash = 367; 
  tubes.insert( (Identifier) 1668759552); // hist bin #145
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOL3A07**********/
  idHash = 435; 
  tubes.insert( (Identifier) 1683685376); // hist bin #1
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOL3A15**********/
  idHash = 439; 
  tubes.insert( (Identifier) 1683965600); // hist bin #222
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOL4A11**********/
  idHash = 445; 
  tubes.insert( (Identifier) 1684367296); // hist bin #423
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS6C02**********/
  idHash = 464; 
  tubes.insert( (Identifier) 1695548352); // hist bin #31
  tubes.insert( (Identifier) 1695548384); // hist bin #32
  tubes.insert( (Identifier) 1695548416); // hist bin #33
  tubes.insert( (Identifier) 1695548512); // hist bin #36
  tubes.insert( (Identifier) 1695548544); // hist bin #37
  tubes.insert( (Identifier) 1695552448); // hist bin #95
  tubes.insert( (Identifier) 1695552832); // hist bin #107
  tubes.insert( (Identifier) 1695556800); // hist bin #167
  tubes.insert( (Identifier) 1695565280); // hist bin #240
  tubes.insert( (Identifier) 1695565312); // hist bin #241
  tubes.insert( (Identifier) 1695571968); // hist bin #321
  tubes.insert( (Identifier) 1695573504); // hist bin #369
  tubes.insert( (Identifier) 1695573536); // hist bin #370
  tubes.insert( (Identifier) 1695573760); // hist bin #377
  tubes.insert( (Identifier) 1695573888); // hist bin #381
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS6C04**********/
  idHash = 465; 
  tubes.insert( (Identifier) 1695612992); // hist bin #3
  tubes.insert( (Identifier) 1695618368); // hist bin #107
  tubes.insert( (Identifier) 1695622112); // hist bin #160
  tubes.insert( (Identifier) 1695634912); // hist bin #304
  tubes.insert( (Identifier) 1695637920); // hist bin #334
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS6C06**********/
  idHash = 466; 
  tubes.insert( (Identifier) 1695679840); // hist bin #44
  tubes.insert( (Identifier) 1695680032); // hist bin #50
  tubes.insert( (Identifier) 1695683424); // hist bin #92
  tubes.insert( (Identifier) 1695695264); // hist bin #206
  tubes.insert( (Identifier) 1695703040); // hist bin #321
  tubes.insert( (Identifier) 1695704480); // hist bin #366
  tubes.insert( (Identifier) 1695704512); // hist bin #367
  tubes.insert( (Identifier) 1695704992); // hist bin #382
  tubes.insert( (Identifier) 1695705056); // hist bin #384
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS6C08**********/
  idHash = 467; 
  tubes.insert( (Identifier) 1695753632); // hist bin #174
  tubes.insert( (Identifier) 1695754208); // hist bin #192
  tubes.insert( (Identifier) 1695768704); // hist bin #325
  tubes.insert( (Identifier) 1695770208); // hist bin #372
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS6C10**********/
  idHash = 468; 
  tubes.insert( (Identifier) 1695835776); // hist bin #373
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS6C16**********/
  idHash = 469; 
  tubes.insert( (Identifier) 1696006528); // hist bin #13
  tubes.insert( (Identifier) 1696022528); // hist bin #193
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS5C08**********/
  idHash = 473; 
  tubes.insert( (Identifier) 1696270272); // hist bin #63
  tubes.insert( (Identifier) 1696270304); // hist bin #64
  tubes.insert( (Identifier) 1696270496); // hist bin #70
  tubes.insert( (Identifier) 1696272384); // hist bin #73
  tubes.insert( (Identifier) 1696272416); // hist bin #74
  tubes.insert( (Identifier) 1696276480); // hist bin #145
  tubes.insert( (Identifier) 1696276512); // hist bin #146
  tubes.insert( (Identifier) 1696277472); // hist bin #176
  tubes.insert( (Identifier) 1696293952); // hist bin #395
  tubes.insert( (Identifier) 1696293984); // hist bin #396
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS5C10**********/
  idHash = 474; 
  tubes.insert( (Identifier) 1696334592); // hist bin #25
  tubes.insert( (Identifier) 1696342784); // hist bin #169
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS4C04**********/
  idHash = 477; 
  tubes.insert( (Identifier) 1696686240); // hist bin #366
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS4C10**********/
  idHash = 480; 
  tubes.insert( (Identifier) 1696858112); // hist bin #1
  tubes.insert( (Identifier) 1696858656); // hist bin #18
  tubes.insert( (Identifier) 1696874656); // hist bin #222
  tubes.insert( (Identifier) 1696880832); // hist bin #359
  tubes.insert( (Identifier) 1696880864); // hist bin #360
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS4C16**********/
  idHash = 481; 
  tubes.insert( (Identifier) 1697081568); // hist bin #432
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS3C10**********/
  idHash = 486; 
  tubes.insert( (Identifier) 1697391072); // hist bin #160
  tubes.insert( (Identifier) 1697404320); // hist bin #334
  tubes.insert( (Identifier) 1697405152); // hist bin #360
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS3C16**********/
  idHash = 487; 
  tubes.insert( (Identifier) 1697585376); // hist bin #144
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS2C02**********/
  idHash = 488; 
  tubes.insert( (Identifier) 1697667264); // hist bin #359
  tubes.insert( (Identifier) 1697667296); // hist bin #360
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS2C08**********/
  idHash = 491; 
  tubes.insert( (Identifier) 1697842880); // hist bin #55
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS2C10**********/
  idHash = 492; 
  tubes.insert( (Identifier) 1697915456); // hist bin #163
  tubes.insert( (Identifier) 1697915648); // hist bin #169
  tubes.insert( (Identifier) 1697915680); // hist bin #170
  tubes.insert( (Identifier) 1697915712); // hist bin #171
  tubes.insert( (Identifier) 1697915744); // hist bin #172
  tubes.insert( (Identifier) 1697915808); // hist bin #174
  tubes.insert( (Identifier) 1697915840); // hist bin #175
  tubes.insert( (Identifier) 1697915872); // hist bin #176
  tubes.insert( (Identifier) 1697915904); // hist bin #177
  tubes.insert( (Identifier) 1697915936); // hist bin #178
  tubes.insert( (Identifier) 1697915968); // hist bin #179
  tubes.insert( (Identifier) 1697916032); // hist bin #181
  tubes.insert( (Identifier) 1697916128); // hist bin #184
  tubes.insert( (Identifier) 1697916416); // hist bin #193
  tubes.insert( (Identifier) 1697916800); // hist bin #205
  tubes.insert( (Identifier) 1697923296); // hist bin #224
  tubes.insert( (Identifier) 1697923744); // hist bin #238
  tubes.insert( (Identifier) 1697923840); // hist bin #241
  tubes.insert( (Identifier) 1697929440); // hist bin #360
  tubes.insert( (Identifier) 1697931264); // hist bin #361
  tubes.insert( (Identifier) 1697933024); // hist bin #416
  tubes.insert( (Identifier) 1697933536); // hist bin #432
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS2C16**********/
  idHash = 493; 
  tubes.insert( (Identifier) 1698112448); // hist bin #175
  tubes.insert( (Identifier) 1698124480); // hist bin #311
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS1C06**********/
  idHash = 496; 
  tubes.insert( (Identifier) 1698299904); // hist bin #1
  tubes.insert( (Identifier) 1698318112); // hist bin #274
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS2A02**********/
  idHash = 506; 
  tubes.insert( (Identifier) 1699768032); // hist bin #416
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS2A16**********/
  idHash = 511; 
  tubes.insert( (Identifier) 1700216832); // hist bin #217
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS3A02**********/
  idHash = 512; 
  tubes.insert( (Identifier) 1700274176); // hist bin #145
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS3A16**********/
  idHash = 517; 
  tubes.insert( (Identifier) 1700725792); // hist bin #34
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS4A04**********/
  idHash = 519; 
  tubes.insert( (Identifier) 1700864000); // hist bin #145
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS4A10**********/
  idHash = 522; 
  tubes.insert( (Identifier) 1701075168); // hist bin #360
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS4A16**********/
  idHash = 523; 
  tubes.insert( (Identifier) 1701249024); // hist bin #1
  tubes.insert( (Identifier) 1701258048); // hist bin #171
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS5A02**********/
  idHash = 524; 
  tubes.insert( (Identifier) 1701337312); // hist bin #360
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS5A04**********/
  idHash = 525; 
  tubes.insert( (Identifier) 1701380352); // hist bin #9
  tubes.insert( (Identifier) 1701397504); // hist bin #249
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS5A10**********/
  idHash = 528; 
  tubes.insert( (Identifier) 1701585376); // hist bin #160
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS5A16**********/
  idHash = 529; 
  tubes.insert( (Identifier) 1701773312); // hist bin #1
  tubes.insert( (Identifier) 1701773472); // hist bin #6
  tubes.insert( (Identifier) 1701773984); // hist bin #22
  tubes.insert( (Identifier) 1701774080); // hist bin #25
  tubes.insert( (Identifier) 1701774240); // hist bin #30
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS6A04**********/
  idHash = 531; 
  tubes.insert( (Identifier) 1701930432); // hist bin #367
  tubes.insert( (Identifier) 1701930464); // hist bin #368
  tubes.insert( (Identifier) 1701930624); // hist bin #373
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOS6A06**********/
  idHash = 532; 
  tubes.insert( (Identifier) 1701974048); // hist bin #66
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BEE2C14**********/
  idHash = 542; 
  tubes.insert( (Identifier) 1714816288); // hist bin #42
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BEE1C12**********/
  idHash = 549; 
  tubes.insert( (Identifier) 1715279136); // hist bin #90
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BEE1A06**********/
  idHash = 554; 
  tubes.insert( (Identifier) 1716126976); // hist bin #41
  tubes.insert( (Identifier) 1716127008); // hist bin #42
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BEE1A08**********/
  idHash = 555; 
  tubes.insert( (Identifier) 1716191456); // hist bin #8
  tubes.insert( (Identifier) 1716191968); // hist bin #24
  tubes.insert( (Identifier) 1716192160); // hist bin #30
  tubes.insert( (Identifier) 1716192544); // hist bin #42
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BEE1A10**********/
  idHash = 556; 
  tubes.insert( (Identifier) 1716258048); // hist bin #41
  tubes.insert( (Identifier) 1716258080); // hist bin #42
  tubes.insert( (Identifier) 1716262144); // hist bin #89
  tubes.insert( (Identifier) 1716262176); // hist bin #90
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BEE2A06**********/
  idHash = 562; 
  tubes.insert( (Identifier) 1716651296); // hist bin #42
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BEE2A10**********/
  idHash = 564; 
  tubes.insert( (Identifier) 1716782336); // hist bin #41
  tubes.insert( (Identifier) 1716782368); // hist bin #42
  tubes.insert( (Identifier) 1716790528); // hist bin #137
  tubes.insert( (Identifier) 1716790560); // hist bin #138
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BEE2A16**********/
  idHash = 567; 
  tubes.insert( (Identifier) 1716978336); // hist bin #22
  tubes.insert( (Identifier) 1716978368); // hist bin #23
  tubes.insert( (Identifier) 1716978560); // hist bin #29
  tubes.insert( (Identifier) 1716978624); // hist bin #31
  tubes.insert( (Identifier) 1716978656); // hist bin #32
  tubes.insert( (Identifier) 1716978816); // hist bin #37
  tubes.insert( (Identifier) 1716978912); // hist bin #40
  tubes.insert( (Identifier) 1716978944); // hist bin #41
  tubes.insert( (Identifier) 1716978976); // hist bin #42
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BIR3C11**********/
  idHash = 574; 
  tubes.insert( (Identifier) 1731015648); // hist bin #140
  tubes.insert( (Identifier) 1731015680); // hist bin #141
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BIR1C11**********/
  idHash = 578; 
  tubes.insert( (Identifier) 1732051072); // hist bin #11
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMF2C14**********/
  idHash = 595; 
  tubes.insert( (Identifier) 1748370560); // hist bin #37
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMF1C12**********/
  idHash = 596; 
  tubes.insert( (Identifier) 1748828160); // hist bin #1
  tubes.insert( (Identifier) 1748830112); // hist bin #62
  tubes.insert( (Identifier) 1748830144); // hist bin #63
  tubes.insert( (Identifier) 1748830336); // hist bin #69
  tubes.insert( (Identifier) 1748836352); // hist bin #145
  tubes.insert( (Identifier) 1748836384); // hist bin #146
  tubes.insert( (Identifier) 1748853024); // hist bin #370
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMF1C14**********/
  idHash = 597; 
  tubes.insert( (Identifier) 1748894080); // hist bin #13
  tubes.insert( (Identifier) 1748903616); // hist bin #199
  tubes.insert( (Identifier) 1748903712); // hist bin #202
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMF1A12**********/
  idHash = 598; 
  tubes.insert( (Identifier) 1749902560); // hist bin #400
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BMF1A14**********/
  idHash = 599; 
  tubes.insert( (Identifier) 1749943520); // hist bin #40
  tubes.insert( (Identifier) 1749952256); // hist bin #201
  tubes.insert( (Identifier) 1749964128); // hist bin #332
  tubes.insert( (Identifier) 1749964224); // hist bin #335
  tubes.insert( (Identifier) 1749968800); // hist bin #422
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOF7C12**********/
  idHash = 604; 
  tubes.insert( (Identifier) 1764057472); // hist bin #213
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOF7C14**********/
  idHash = 605; 
  tubes.insert( (Identifier) 1764106336); // hist bin #84
  tubes.insert( (Identifier) 1764106368); // hist bin #85
  tubes.insert( (Identifier) 1764106400); // hist bin #86
  tubes.insert( (Identifier) 1764106464); // hist bin #88
  tubes.insert( (Identifier) 1764106656); // hist bin #94
  tubes.insert( (Identifier) 1764107168); // hist bin #110
  tubes.insert( (Identifier) 1764114656); // hist bin #128
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOF1C12**********/
  idHash = 610; 
  tubes.insert( (Identifier) 1765610208); // hist bin #96
  tubes.insert( (Identifier) 1765629952); // hist bin #361
  tubes.insert( (Identifier) 1765631904); // hist bin #422
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOF1A12**********/
  idHash = 612; 
  tubes.insert( (Identifier) 1766678880); // hist bin #372
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOF5A14**********/
  idHash = 617; 
  tubes.insert( (Identifier) 1767768800); // hist bin #24
  tubes.insert( (Identifier) 1767777088); // hist bin #123
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOG8C12**********/
  idHash = 620; 
  tubes.insert( (Identifier) 1780810496); // hist bin #25
  tubes.insert( (Identifier) 1780814496); // hist bin #62
  tubes.insert( (Identifier) 1780814592); // hist bin #65
  tubes.insert( (Identifier) 1780818656); // hist bin #104
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOG8C14**********/
  idHash = 621; 
  tubes.insert( (Identifier) 1780892768); // hist bin #156
  tubes.insert( (Identifier) 1780896864); // hist bin #196
  tubes.insert( (Identifier) 1780896896); // hist bin #197
  tubes.insert( (Identifier) 1780896928); // hist bin #198
  tubes.insert( (Identifier) 1780896992); // hist bin #200
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOG6C14**********/
  idHash = 623; 
  tubes.insert( (Identifier) 1781408224); // hist bin #96
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOG4C14**********/
  idHash = 625; 
  tubes.insert( (Identifier) 1781932448); // hist bin #94
  tubes.insert( (Identifier) 1781932480); // hist bin #95
  tubes.insert( (Identifier) 1781948544); // hist bin #205
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOG2C12**********/
  idHash = 626; 
  tubes.insert( (Identifier) 1782407168); // hist bin #201
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOG2C14**********/
  idHash = 627; 
  tubes.insert( (Identifier) 1782448800); // hist bin #22
  tubes.insert( (Identifier) 1782449344); // hist bin #39
  tubes.insert( (Identifier) 1782452992); // hist bin #65
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********BOG6A12**********/
  idHash = 634; 
  tubes.insert( (Identifier) 1784504832); // hist bin #217
  tubes.insert( (Identifier) 1784504896); // hist bin #219
  tubes.insert( (Identifier) 1784504992); // hist bin #222
  tubes.insert( (Identifier) 1784505312); // hist bin #232
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EIL2A15**********/
  idHash = 687; 
  tubes.insert( (Identifier) 1800864832); // hist bin #35
  tubes.insert( (Identifier) 1800864864); // hist bin #36
  tubes.insert( (Identifier) 1800877120); // hist bin #143
  tubes.insert( (Identifier) 1800877152); // hist bin #144
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EIL4A07**********/
  idHash = 699; 
  tubes.insert( (Identifier) 1801679264); // hist bin #392
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EML2A13**********/
  idHash = 822; 
  tubes.insert( (Identifier) 1851150624); // hist bin #266
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EML2A15**********/
  idHash = 823; 
  tubes.insert( (Identifier) 1851220256); // hist bin #330
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EMS2A08**********/
  idHash = 899; 
  tubes.insert( (Identifier) 1867727104); // hist bin #201
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EMS2A16**********/
  idHash = 903; 
  tubes.insert( (Identifier) 1867978272); // hist bin #114
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EMS3A02**********/
  idHash = 904; 
  tubes.insert( (Identifier) 1868048192); // hist bin #187
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EMS3A08**********/
  idHash = 907; 
  tubes.insert( (Identifier) 1868253152); // hist bin #256
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EMS5A10**********/
  idHash = 924; 
  tubes.insert( (Identifier) 1869349472); // hist bin #20
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EOL1A03**********/
  idHash = 977; 
  tubes.insert( (Identifier) 1883842240); // hist bin #167
  tubes.insert( (Identifier) 1883857024); // hist bin #285
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EOL1A09**********/
  idHash = 980; 
  tubes.insert( (Identifier) 1884054048); // hist bin #298
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EOL2A05**********/
  idHash = 986; 
  tubes.insert( (Identifier) 1884442656); // hist bin #226
  tubes.insert( (Identifier) 1884446976); // hist bin #289
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EOL2A13**********/
  idHash = 990; 
  tubes.insert( (Identifier) 1884684704); // hist bin #14
  tubes.insert( (Identifier) 1884684736); // hist bin #15
  tubes.insert( (Identifier) 1884684960); // hist bin #22
  tubes.insert( (Identifier) 1884705280); // hist bin #241
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EOL5A09**********/
  idHash = 1012; 
  tubes.insert( (Identifier) 1886127584); // hist bin #48
  tubes.insert( (Identifier) 1886135712); // hist bin #142
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EOL5A13**********/
  idHash = 1014; 
  tubes.insert( (Identifier) 1886258656); // hist bin #48
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EOL6A09**********/
  idHash = 1020; 
  tubes.insert( (Identifier) 1886675200); // hist bin #249
  tubes.insert( (Identifier) 1886676448); // hist bin #288
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EOS6C06**********/
  idHash = 1026; 
  tubes.insert( (Identifier) 1897021472); // hist bin #146
  tubes.insert( (Identifier) 1897030016); // hist bin #253
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EOS6C10**********/
  idHash = 1028; 
  tubes.insert( (Identifier) 1897145056); // hist bin #120
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EOS1A04**********/
  idHash = 1073; 
  tubes.insert( (Identifier) 1900614656); // hist bin #89
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EOS1A12**********/
  idHash = 1077; 
  tubes.insert( (Identifier) 1900877536); // hist bin #112
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

  /**********EOS4A04**********/
  idHash = 1097; 
  tubes.insert( (Identifier) 1902199168); // hist bin #157
  tubes.insert( (Identifier) 1902199264); // hist bin #160
  tubes.insert( (Identifier) 1902207296); // hist bin #251
  tubes.insert( (Identifier) 1902207456); // hist bin #256
  m_noise_map.insert( make_pair(idHash, tubes) );
  tubes.clear();

}


  
