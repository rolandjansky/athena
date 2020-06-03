/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  ***************************************************************************
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *

#include "TrigT1CaloCalibTools/L1CaloFcal23Cells2RxMappingTool.h"

namespace LVL1{

  L1CaloFcal23Cells2RxMappingTool::L1CaloFcal23Cells2RxMappingTool( const std::string& name ) :
    asg::AsgTool( name ),
    m_mapOfflineCell2RxId(nullptr),
    m_mapOnlineCell2RxId(nullptr),
    m_mapOfflineCellNotConnectedToCalibLines(nullptr),
    m_mapOnlineCellNotConnectedToCalibLines(nullptr)
{
  
    m_mapOfflineCell2RxId = new std::map<unsigned int,unsigned int>;   
    m_mapOnlineCell2RxId = new std::map<unsigned int,unsigned int>;
    
    m_mapOfflineCellNotConnectedToCalibLines = new std::map<unsigned int,unsigned int>;    
    m_mapOnlineCellNotConnectedToCalibLines = new std::map<unsigned int,unsigned int>;
  }
  
  L1CaloFcal23Cells2RxMappingTool::~L1CaloFcal23Cells2RxMappingTool()
  {
    delete m_mapOfflineCell2RxId;
    delete m_mapOnlineCell2RxId;
    delete m_mapOfflineCellNotConnectedToCalibLines;
    delete m_mapOnlineCellNotConnectedToCalibLines;
  }

  unsigned int L1CaloFcal23Cells2RxMappingTool::offlineCell2RxId(const unsigned int& cellId) const{
    std::map<unsigned int,unsigned int>::iterator itr = m_mapOfflineCell2RxId->find(cellId);
    if(itr != m_mapOfflineCell2RxId->end()){
      return itr->second;
    }
    return 0;
  }

  unsigned int L1CaloFcal23Cells2RxMappingTool::onlineCell2RxId(const unsigned int& cellId) const{
    std::map<unsigned int,unsigned int>::iterator itr = m_mapOnlineCell2RxId->find(cellId);
    if(itr != m_mapOnlineCell2RxId->end()){
      return itr->second;
    }
    return 0;
  }

  StatusCode L1CaloFcal23Cells2RxMappingTool::finalize(){
    return StatusCode::SUCCESS;
  }

  StatusCode L1CaloFcal23Cells2RxMappingTool::initialize(){


    //Trigger Tower : 0x4180000 eta = 3.4125, phi = 0.1963
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914358272] = 4995; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914489344] = 4995; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914620416] = 4995; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914751488] = 4995; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914882560] = 4995; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915013632] = 4995; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915144704] = 4995; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915275776] = 4995; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915406848] = 4994; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915537920] = 4994; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915668992] = 4994; // taken from run 178588
    (*m_mapOfflineCell2RxId)[915800064] = 4994; // taken from run 178586
    (*m_mapOfflineCell2RxId)[915931136] = 4994; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916062208] = 4994; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916193280] = 4994; // taken from run 178578
    (*m_mapOfflineCell2RxId)[916324352] = 4994; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993308416] = 4995; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993308160] = 4995; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993307904] = 4995; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993307648] = 4995; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993307392] = 4995; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993307136] = 4995; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993306880] = 4995; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993306624] = 4995; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993306368] = 4994; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993306112] = 4994; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993305856] = 4994; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993305600] = 4994; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993305344] = 4994; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993305088] = 4994; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993304832] = 4994; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993304576] = 4994; // taken from run 178577

    //Trigger Tower : 0x4180100 eta = 3.4125, phi = 0.5890
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914366464] = 4993; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914497536] = 4993; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914628608] = 4993; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914759680] = 4993; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914890752] = 4993; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915021824] = 4993; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915152896] = 4993; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915283968] = 4993; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915415040] = 4992; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915546112] = 4992; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915677184] = 4992; // taken from run 178586
    (*m_mapOfflineCell2RxId)[915808256] = 4992; // taken from run 178588
    (*m_mapOfflineCell2RxId)[915939328] = 4992; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916070400] = 4992; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916201472] = 4992; // taken from run 178577
    (*m_mapOfflineCell2RxId)[916332544] = 4992; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993300224] = 4993; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993299968] = 4993; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993299712] = 4993; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993299456] = 4993; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993299200] = 4993; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993298944] = 4993; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993298688] = 4993; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993298432] = 4993; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993298176] = 4992; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993297920] = 4992; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993297664] = 4992; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993297408] = 4992; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993297152] = 4992; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993296896] = 4992; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993296640] = 4992; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993296384] = 4992; // taken from run 178578

    //Trigger Tower : 0x4180200 eta = 3.4125, phi = 0.9817
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914374656] = 5003; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914505728] = 5003; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914636800] = 5003; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914767872] = 5003; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914898944] = 5003; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915030016] = 5003; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915161088] = 5003; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915292160] = 5003; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915423232] = 5002; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915554304] = 5002; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915685376] = 5002; // taken from run 178588
    (*m_mapOfflineCell2RxId)[915816448] = 5002; // taken from run 178586
    (*m_mapOfflineCell2RxId)[915947520] = 5002; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916078592] = 5002; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916209664] = 5002; // taken from run 178578
    (*m_mapOfflineCell2RxId)[916340736] = 5002; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993324800] = 5003; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993324544] = 5003; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993324288] = 5003; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993324032] = 5003; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993323776] = 5003; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993323520] = 5003; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993323264] = 5003; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993323008] = 5003; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993322752] = 5002; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993322496] = 5002; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993322240] = 5002; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993321984] = 5002; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993321728] = 5002; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993321472] = 5002; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993321216] = 5002; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993320960] = 5002; // taken from run 178577

    //Trigger Tower : 0x4180300 eta = 3.4125, phi = 1.3744
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914382848] = 5001; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914513920] = 5001; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914644992] = 5001; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914776064] = 5001; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914907136] = 5001; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915038208] = 5001; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915169280] = 5001; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915300352] = 5001; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915431424] = 5000; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915562496] = 5000; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915693568] = 5000; // taken from run 178586
    (*m_mapOfflineCell2RxId)[915824640] = 5000; // taken from run 178588
    (*m_mapOfflineCell2RxId)[915955712] = 5000; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916086784] = 5000; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916217856] = 5000; // taken from run 178577
    (*m_mapOfflineCell2RxId)[916348928] = 5000; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993316608] = 5001; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993316352] = 5001; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993316096] = 5001; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993315840] = 5001; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993315584] = 5001; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993315328] = 5001; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993315072] = 5001; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993314816] = 5001; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993314560] = 5000; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993314304] = 5000; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993314048] = 5000; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993313792] = 5000; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993313536] = 5000; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993313280] = 5000; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993313024] = 5000; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993312768] = 5000; // taken from run 178578

    //Trigger Tower : 0x4180400 eta = 3.4125, phi = 1.7671
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914391040] = 5011; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914522112] = 5011; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914653184] = 5011; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914784256] = 5011; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914915328] = 5011; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915046400] = 5011; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915177472] = 5011; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915308544] = 5011; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915439616] = 5010; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915570688] = 5010; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915701760] = 5010; // taken from run 178588
    (*m_mapOfflineCell2RxId)[915832832] = 5010; // taken from run 178586
    (*m_mapOfflineCell2RxId)[915963904] = 5010; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916094976] = 5010; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916226048] = 5010; // taken from run 178578
    (*m_mapOfflineCell2RxId)[916357120] = 5010; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993341184] = 5011; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993340928] = 5011; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993340672] = 5011; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993340416] = 5011; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993340160] = 5011; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993339904] = 5011; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993339648] = 5011; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993339392] = 5011; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993339136] = 5010; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993338880] = 5010; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993338624] = 5010; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993338368] = 5010; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993338112] = 5010; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993337856] = 5010; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993337600] = 5010; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993337344] = 5010; // taken from run 178577

    //Trigger Tower : 0x4180500 eta = 3.4125, phi = 2.1598
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914399232] = 5009; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914530304] = 5009; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914661376] = 5009; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914792448] = 5009; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914923520] = 5009; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915054592] = 5009; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915185664] = 5009; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915316736] = 5009; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915447808] = 5008; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915578880] = 5008; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915709952] = 5008; // taken from run 178586
    (*m_mapOfflineCell2RxId)[915841024] = 5008; // taken from run 178588
    (*m_mapOfflineCell2RxId)[915972096] = 5008; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916103168] = 5008; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916234240] = 5008; // taken from run 178577
    (*m_mapOfflineCell2RxId)[916365312] = 5008; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993332992] = 5009; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993332736] = 5009; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993332480] = 5009; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993332224] = 5009; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993331968] = 5009; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993331712] = 5009; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993331456] = 5009; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993331200] = 5009; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993330944] = 5008; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993330688] = 5008; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993330432] = 5008; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993330176] = 5008; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993329920] = 5008; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993329664] = 5008; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993329408] = 5008; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993329152] = 5008; // taken from run 178578

    //Trigger Tower : 0x4180600 eta = 3.4125, phi = 2.5525
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914407424] = 5019; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914538496] = 5019; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914669568] = 5019; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914800640] = 5019; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914931712] = 5019; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915062784] = 5019; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915193856] = 5019; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915324928] = 5019; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915456000] = 5018; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915587072] = 5018; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915718144] = 5018; // taken from run 178588
    (*m_mapOfflineCell2RxId)[915849216] = 5018; // taken from run 178586
    (*m_mapOfflineCell2RxId)[915980288] = 5018; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916111360] = 5018; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916242432] = 5018; // taken from run 178578
    (*m_mapOfflineCell2RxId)[916373504] = 5018; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993357568] = 5019; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993357312] = 5019; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993357056] = 5019; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993356800] = 5019; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993356544] = 5019; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993356288] = 5019; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993356032] = 5019; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993355776] = 5019; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993355520] = 5018; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993355264] = 5018; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993355008] = 5018; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993354752] = 5018; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993354496] = 5018; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993354240] = 5018; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993353984] = 5018; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993353728] = 5018; // taken from run 178577

    //Trigger Tower : 0x4180700 eta = 3.4125, phi = 2.9452
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914415616] = 5017; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914546688] = 5017; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914677760] = 5017; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914808832] = 5017; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914939904] = 5017; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915070976] = 5017; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915202048] = 5017; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915333120] = 5017; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915464192] = 5016; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915595264] = 5016; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915726336] = 5016; // taken from run 178586
    (*m_mapOfflineCell2RxId)[915857408] = 5016; // taken from run 178588
    (*m_mapOfflineCell2RxId)[915988480] = 5016; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916119552] = 5016; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916250624] = 5016; // taken from run 178577
    (*m_mapOfflineCell2RxId)[916381696] = 5016; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993349376] = 5017; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993349120] = 5017; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993348864] = 5017; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993348608] = 5017; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993348352] = 5017; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993348096] = 5017; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993347840] = 5017; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993347584] = 5017; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993347328] = 5016; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993347072] = 5016; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993346816] = 5016; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993346560] = 5016; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993346304] = 5016; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993346048] = 5016; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993345792] = 5016; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993345536] = 5016; // taken from run 178578

    //Trigger Tower : 0x4180800 eta = 3.4125, phi = 3.3379
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914423808] = 5027; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914554880] = 5027; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914685952] = 5027; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914817024] = 5027; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914948096] = 5027; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915079168] = 5027; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915210240] = 5027; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915341312] = 5027; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915472384] = 5026; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915603456] = 5026; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915734528] = 5026; // taken from run 178588
    (*m_mapOfflineCell2RxId)[915865600] = 5026; // taken from run 178586
    (*m_mapOfflineCell2RxId)[915996672] = 5026; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916127744] = 5026; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916258816] = 5026; // taken from run 178578
    (*m_mapOfflineCell2RxId)[916389888] = 5026; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993373952] = 5027; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993373696] = 5027; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993373440] = 5027; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993373184] = 5027; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993372928] = 5027; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993372672] = 5027; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993372416] = 5027; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993372160] = 5027; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993371904] = 5026; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993371648] = 5026; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993371392] = 5026; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993371136] = 5026; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993370880] = 5026; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993370624] = 5026; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993370368] = 5026; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993370112] = 5026; // taken from run 178577

    //Trigger Tower : 0x4180900 eta = 3.4125, phi = 3.7306
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914432000] = 5025; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914563072] = 5025; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914694144] = 5025; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914825216] = 5025; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914956288] = 5025; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915087360] = 5025; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915218432] = 5025; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915349504] = 5025; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915480576] = 5024; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915611648] = 5024; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915742720] = 5024; // taken from run 178586
    (*m_mapOfflineCell2RxId)[915873792] = 5024; // taken from run 178588
    (*m_mapOfflineCell2RxId)[916004864] = 5024; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916135936] = 5024; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916267008] = 5024; // taken from run 178577
    (*m_mapOfflineCell2RxId)[916398080] = 5024; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993365760] = 5025; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993365504] = 5025; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993365248] = 5025; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993364992] = 5025; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993364736] = 5025; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993364480] = 5025; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993364224] = 5025; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993363968] = 5025; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993363712] = 5024; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993363456] = 5024; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993363200] = 5024; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993362944] = 5024; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993362688] = 5024; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993362432] = 5024; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993362176] = 5024; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993361920] = 5024; // taken from run 178578

    //Trigger Tower : 0x4180a00 eta = 3.4125, phi = 4.1233
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914440192] = 5035; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914571264] = 5035; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914702336] = 5035; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914833408] = 5035; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914964480] = 5035; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915095552] = 5035; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915226624] = 5035; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915357696] = 5035; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915488768] = 5034; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915619840] = 5034; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915750912] = 5034; // taken from run 178588
    (*m_mapOfflineCell2RxId)[915881984] = 5034; // taken from run 178586
    (*m_mapOfflineCell2RxId)[916013056] = 5034; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916144128] = 5034; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916275200] = 5034; // taken from run 178578
    (*m_mapOfflineCell2RxId)[916406272] = 5034; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993390336] = 5035; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993390080] = 5035; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993389824] = 5035; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993389568] = 5035; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993389312] = 5035; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993389056] = 5035; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993388800] = 5035; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993388544] = 5035; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993388288] = 5034; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993388032] = 5034; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993387776] = 5034; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993387520] = 5034; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993387264] = 5034; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993387008] = 5034; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993386752] = 5034; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993386496] = 5034; // taken from run 178577

    //Trigger Tower : 0x4180b00 eta = 3.4125, phi = 4.5160
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914448384] = 5033; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914579456] = 5033; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914710528] = 5033; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914841600] = 5033; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914972672] = 5033; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915103744] = 5033; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915234816] = 5033; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915365888] = 5033; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915496960] = 5032; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915628032] = 5032; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915759104] = 5032; // taken from run 178586
    (*m_mapOfflineCell2RxId)[915890176] = 5032; // taken from run 178588
    (*m_mapOfflineCell2RxId)[916021248] = 5032; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916152320] = 5032; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916283392] = 5032; // taken from run 178577
    (*m_mapOfflineCell2RxId)[916414464] = 5032; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993382144] = 5033; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993381888] = 5033; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993381632] = 5033; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993381376] = 5033; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993381120] = 5033; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993380864] = 5033; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993380608] = 5033; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993380352] = 5033; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993380096] = 5032; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993379840] = 5032; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993379584] = 5032; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993379328] = 5032; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993379072] = 5032; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993378816] = 5032; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993378560] = 5032; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993378304] = 5032; // taken from run 178578

    //Trigger Tower : 0x4180c00 eta = 3.4125, phi = 4.9087
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914456576] = 5043; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914587648] = 5043; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914718720] = 5043; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914849792] = 5043; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914980864] = 5043; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915111936] = 5043; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915243008] = 5043; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915374080] = 5043; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915505152] = 5042; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915636224] = 5042; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915767296] = 5042; // taken from run 178588
    (*m_mapOfflineCell2RxId)[915898368] = 5042; // taken from run 178586
    (*m_mapOfflineCell2RxId)[916029440] = 5042; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916160512] = 5042; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916291584] = 5042; // taken from run 178578
    (*m_mapOfflineCell2RxId)[916422656] = 5042; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993406720] = 5043; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993406464] = 5043; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993406208] = 5043; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993405952] = 5043; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993405696] = 5043; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993405440] = 5043; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993405184] = 5043; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993404928] = 5043; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993404672] = 5042; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993404416] = 5042; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993404160] = 5042; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993403904] = 5042; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993403648] = 5042; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993403392] = 5042; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993403136] = 5042; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993402880] = 5042; // taken from run 178577

    //Trigger Tower : 0x4180d00 eta = 3.4125, phi = 5.3014
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914464768] = 5041; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914595840] = 5041; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914726912] = 5041; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914857984] = 5041; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914989056] = 5041; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915120128] = 5041; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915251200] = 5041; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915382272] = 5041; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915513344] = 5040; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915644416] = 5040; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915775488] = 5040; // taken from run 178586
    (*m_mapOfflineCell2RxId)[915906560] = 5040; // taken from run 178588
    (*m_mapOfflineCell2RxId)[916037632] = 5040; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916168704] = 5040; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916299776] = 5040; // taken from run 178577
    (*m_mapOfflineCell2RxId)[916430848] = 5040; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993398528] = 5041; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993398272] = 5041; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993398016] = 5041; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993397760] = 5041; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993397504] = 5041; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993397248] = 5041; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993396992] = 5041; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993396736] = 5041; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993396480] = 5040; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993396224] = 5040; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993395968] = 5040; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993395712] = 5040; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993395456] = 5040; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993395200] = 5040; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993394944] = 5040; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993394688] = 5040; // taken from run 178578

    //Trigger Tower : 0x4180e00 eta = 3.4125, phi = 5.6941
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914472960] = 5051; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914604032] = 5051; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914735104] = 5051; // taken from run 178741
    (*m_mapOfflineCell2RxId)[914866176] = 5051; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914997248] = 5051; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915128320] = 5051; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915259392] = 5051; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915390464] = 5051; // taken from run 178735
    (*m_mapOfflineCell2RxId)[915521536] = 5050; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915652608] = 5050; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915783680] = 5050; // taken from run 178588
    (*m_mapOfflineCell2RxId)[915914752] = 5050; // taken from run 178586
    (*m_mapOfflineCell2RxId)[916045824] = 5050; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916176896] = 5050; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916307968] = 5050; // taken from run 178578
    (*m_mapOfflineCell2RxId)[916439040] = 5050; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993423104] = 5051; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993422848] = 5051; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993422592] = 5051; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993422336] = 5051; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993422080] = 5051; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993421824] = 5051; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993421568] = 5051; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993421312] = 5051; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993421056] = 5050; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993420800] = 5050; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993420544] = 5050; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993420288] = 5050; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993420032] = 5050; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993419776] = 5050; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993419520] = 5050; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993419264] = 5050; // taken from run 178577

    //Trigger Tower : 0x4180f00 eta = 3.4125, phi = 6.0868
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[914481152] = 5049; // taken from run 178742
    (*m_mapOfflineCell2RxId)[914612224] = 5049; // taken from run 178743
    (*m_mapOfflineCell2RxId)[914743296] = 5049; // taken from run 178739
    (*m_mapOfflineCell2RxId)[914874368] = 5049; // taken from run 178741
    (*m_mapOfflineCell2RxId)[915005440] = 5049; // taken from run 178737
    (*m_mapOfflineCell2RxId)[915136512] = 5049; // taken from run 178738
    (*m_mapOfflineCell2RxId)[915398656] = 5049; // taken from run 178736
    (*m_mapOfflineCell2RxId)[915529728] = 5048; // taken from run 178716
    (*m_mapOfflineCell2RxId)[915660800] = 5048; // taken from run 178717
    (*m_mapOfflineCell2RxId)[915791872] = 5048; // taken from run 178586
    (*m_mapOfflineCell2RxId)[915922944] = 5048; // taken from run 178588
    (*m_mapOfflineCell2RxId)[916054016] = 5048; // taken from run 178582
    (*m_mapOfflineCell2RxId)[916185088] = 5048; // taken from run 178584
    (*m_mapOfflineCell2RxId)[916316160] = 5048; // taken from run 178577
    (*m_mapOfflineCell2RxId)[916447232] = 5048; // taken from run 178578
    
    (*m_mapOfflineCell2RxId)[915267584] = 5049; // Not connected to Calib Lines - this is a guess
    
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993414912] = 5049; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993414656] = 5049; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993414400] = 5049; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993414144] = 5049; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993413888] = 5049; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993413632] = 5049; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993413120] = 5049; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993412864] = 5048; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993412608] = 5048; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993412352] = 5048; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993412096] = 5048; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993411840] = 5048; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993411584] = 5048; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993411328] = 5048; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993411072] = 5048; // taken from run 178578
    
    (*m_mapOnlineCell2RxId)[993413376] = 5049; // Not connected to Calib Lines - this is a guess
    
    (*m_mapOfflineCellNotConnectedToCalibLines)[915267584] = 0;
    (*m_mapOnlineCellNotConnectedToCalibLines)[993413376] = 0;

    //Trigger Tower : 0x4180001 eta = 4.6875, phi = 0.1963
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923795456] = 5067; // taken from run 178734
    (*m_mapOfflineCell2RxId)[923926528] = 5067; // taken from run 178733
    (*m_mapOfflineCell2RxId)[924057600] = 5067; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924188672] = 5067; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924319744] = 5066; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924450816] = 5066; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924581888] = 5066; // taken from run 178578
    (*m_mapOfflineCell2RxId)[924712960] = 5066; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993441536] = 5067; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993441280] = 5067; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993441024] = 5067; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993440768] = 5067; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993440512] = 5066; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993440256] = 5066; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993440000] = 5066; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993439744] = 5066; // taken from run 178577

    //Trigger Tower : 0x4180101 eta = 4.6875, phi = 0.5890
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923803648] = 5065; // taken from run 178734
    (*m_mapOfflineCell2RxId)[923934720] = 5065; // taken from run 178733
    (*m_mapOfflineCell2RxId)[924065792] = 5065; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924196864] = 5065; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924327936] = 5064; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924459008] = 5064; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924590080] = 5064; // taken from run 178578
    (*m_mapOfflineCell2RxId)[924721152] = 5064; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993437440] = 5065; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993437184] = 5065; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993436928] = 5065; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993436672] = 5065; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993436416] = 5064; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993436160] = 5064; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993435904] = 5064; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993435648] = 5064; // taken from run 178577

    //Trigger Tower : 0x4180201 eta = 4.6875, phi = 0.9817
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923811840] = 5059; // taken from run 178733
    (*m_mapOfflineCell2RxId)[923942912] = 5059; // taken from run 178734
    (*m_mapOfflineCell2RxId)[924073984] = 5059; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924205056] = 5059; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924336128] = 5058; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924467200] = 5058; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924598272] = 5058; // taken from run 178577
    (*m_mapOfflineCell2RxId)[924729344] = 5058; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993433344] = 5059; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993433088] = 5059; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993432832] = 5059; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993432576] = 5059; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993432320] = 5058; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993432064] = 5058; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993431808] = 5058; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993431552] = 5058; // taken from run 178578

    //Trigger Tower : 0x4180301 eta = 4.6875, phi = 1.3744
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923820032] = 5057; // taken from run 178733
    (*m_mapOfflineCell2RxId)[923951104] = 5057; // taken from run 178734
    (*m_mapOfflineCell2RxId)[924082176] = 5057; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924213248] = 5057; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924344320] = 5056; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924475392] = 5056; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924606464] = 5056; // taken from run 178577
    (*m_mapOfflineCell2RxId)[924737536] = 5056; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993429248] = 5057; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993428992] = 5057; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993428736] = 5057; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993428480] = 5057; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993428224] = 5056; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993427968] = 5056; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993427712] = 5056; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993427456] = 5056; // taken from run 178578

    //Trigger Tower : 0x4180401 eta = 4.6875, phi = 1.7671
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923828224] = 5083; // taken from run 178734
    (*m_mapOfflineCell2RxId)[923959296] = 5083; // taken from run 178733
    (*m_mapOfflineCell2RxId)[924090368] = 5083; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924221440] = 5083; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924352512] = 5082; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924483584] = 5082; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924614656] = 5082; // taken from run 178578
    (*m_mapOfflineCell2RxId)[924745728] = 5082; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993457920] = 5083; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993457664] = 5083; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993457408] = 5083; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993457152] = 5083; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993456896] = 5082; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993456640] = 5082; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993456384] = 5082; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993456128] = 5082; // taken from run 178577

    //Trigger Tower : 0x4180501 eta = 4.6875, phi = 2.1598
    // This Trigger Tower has n cells = 7
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923836416] = 5081; // taken from run 178734
    (*m_mapOfflineCell2RxId)[923967488] = 5081; // taken from run 178733
    (*m_mapOfflineCell2RxId)[924098560] = 5081; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924229632] = 5081; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924360704] = 5080; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924491776] = 5080; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924622848] = 5080; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993453824] = 5081; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993453568] = 5081; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993453312] = 5081; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993453056] = 5081; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993452800] = 5080; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993452544] = 5080; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993452288] = 5080; // taken from run 178578

    //Trigger Tower : 0x4180601 eta = 4.6875, phi = 2.5525
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923844608] = 5075; // taken from run 178733
    (*m_mapOfflineCell2RxId)[923975680] = 5075; // taken from run 178734
    (*m_mapOfflineCell2RxId)[924106752] = 5075; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924237824] = 5075; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924368896] = 5074; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924499968] = 5074; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924631040] = 5074; // taken from run 178577

    (*m_mapOfflineCell2RxId)[924762112] = 5074; // Not connected to Calib Lines - this is a guess
    
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993449728] = 5075; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993449472] = 5075; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993449216] = 5075; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993448960] = 5075; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993448704] = 5074; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993448448] = 5074; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993448192] = 5074; // taken from run 178577

    (*m_mapOnlineCell2RxId)[993447936] = 5074; // Not connected to Calib Lines - this is a guess
    
    (*m_mapOfflineCellNotConnectedToCalibLines)[924762112] = 0;
    (*m_mapOnlineCellNotConnectedToCalibLines)[993447936] = 0;

    //Trigger Tower : 0x4180701 eta = 4.6875, phi = 2.9452
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923852800] = 5073; // taken from run 178733
    (*m_mapOfflineCell2RxId)[923983872] = 5073; // taken from run 178734
    (*m_mapOfflineCell2RxId)[924114944] = 5073; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924246016] = 5073; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924377088] = 5072; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924508160] = 5072; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924639232] = 5072; // taken from run 178577

    (*m_mapOfflineCell2RxId)[924770304] = 5072; // Not connected to Calib Lines - this is a guess
     
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993445632] = 5073; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993445376] = 5073; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993445120] = 5073; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993444864] = 5073; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993444608] = 5072; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993444352] = 5072; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993444096] = 5072; // taken from run 178577

    (*m_mapOnlineCell2RxId)[993443840] = 5072; // Not connected to Calib Lines - this is a guess
    
    (*m_mapOfflineCellNotConnectedToCalibLines)[924770304] = 0;
    (*m_mapOnlineCellNotConnectedToCalibLines)[993443840] = 0;

    //Trigger Tower : 0x4180801 eta = 4.6875, phi = 3.3379
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923860992] = 5099; // taken from run 178734
    (*m_mapOfflineCell2RxId)[923992064] = 5099; // taken from run 178733
    (*m_mapOfflineCell2RxId)[924123136] = 5099; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924254208] = 5099; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924385280] = 5098; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924516352] = 5098; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924647424] = 5098; // taken from run 178578
    (*m_mapOfflineCell2RxId)[924778496] = 5098; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993474304] = 5099; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993474048] = 5099; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993473792] = 5099; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993473536] = 5099; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993473280] = 5098; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993473024] = 5098; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993472768] = 5098; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993472512] = 5098; // taken from run 178577

    //Trigger Tower : 0x4180901 eta = 4.6875, phi = 3.7306
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923869184] = 5097; // taken from run 178734
    (*m_mapOfflineCell2RxId)[924000256] = 5097; // taken from run 178733
    (*m_mapOfflineCell2RxId)[924131328] = 5097; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924262400] = 5097; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924393472] = 5096; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924524544] = 5096; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924655616] = 5096; // taken from run 178578
    (*m_mapOfflineCell2RxId)[924786688] = 5096; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993470208] = 5097; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993469952] = 5097; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993469696] = 5097; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993469440] = 5097; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993469184] = 5096; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993468928] = 5096; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993468672] = 5096; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993468416] = 5096; // taken from run 178577

    //Trigger Tower : 0x4180a01 eta = 4.6875, phi = 4.1233
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923877376] = 5091; // taken from run 178733
    (*m_mapOfflineCell2RxId)[924008448] = 5091; // taken from run 178734
    (*m_mapOfflineCell2RxId)[924139520] = 5091; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924270592] = 5091; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924401664] = 5090; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924532736] = 5090; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924663808] = 5090; // taken from run 178577
    (*m_mapOfflineCell2RxId)[924794880] = 5090; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993466112] = 5091; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993465856] = 5091; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993465600] = 5091; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993465344] = 5091; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993465088] = 5090; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993464832] = 5090; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993464576] = 5090; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993464320] = 5090; // taken from run 178578

    //Trigger Tower : 0x4180b01 eta = 4.6875, phi = 4.5160
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923885568] = 5089; // taken from run 178733
    (*m_mapOfflineCell2RxId)[924016640] = 5089; // taken from run 178734
    (*m_mapOfflineCell2RxId)[924147712] = 5089; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924278784] = 5089; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924409856] = 5088; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924540928] = 5088; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924672000] = 5088; // taken from run 178577
    (*m_mapOfflineCell2RxId)[924803072] = 5088; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993462016] = 5089; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993461760] = 5089; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993461504] = 5089; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993461248] = 5089; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993460992] = 5088; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993460736] = 5088; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993460480] = 5088; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993460224] = 5088; // taken from run 178578

    //Trigger Tower : 0x4180c01 eta = 4.6875, phi = 4.9087
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923893760] = 5115; // taken from run 178734
    (*m_mapOfflineCell2RxId)[924024832] = 5115; // taken from run 178733
    (*m_mapOfflineCell2RxId)[924155904] = 5115; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924286976] = 5115; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924418048] = 5114; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924549120] = 5114; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924680192] = 5114; // taken from run 178578
    (*m_mapOfflineCell2RxId)[924811264] = 5114; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993490688] = 5115; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993490432] = 5115; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993490176] = 5115; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993489920] = 5115; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993489664] = 5114; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993489408] = 5114; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993489152] = 5114; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993488896] = 5114; // taken from run 178577

    //Trigger Tower : 0x4180d01 eta = 4.6875, phi = 5.3014
    // This Trigger Tower has n cells = 7
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923901952] = 5113; // taken from run 178734
    (*m_mapOfflineCell2RxId)[924033024] = 5113; // taken from run 178733
    (*m_mapOfflineCell2RxId)[924164096] = 5113; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924295168] = 5113; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924426240] = 5112; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924557312] = 5112; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924688384] = 5112; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993486592] = 5113; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993486336] = 5113; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993486080] = 5113; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993485824] = 5113; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993485568] = 5112; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993485312] = 5112; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993485056] = 5112; // taken from run 178578

    //Trigger Tower : 0x4180e01 eta = 4.6875, phi = 5.6941
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923910144] = 5107; // taken from run 178733
    (*m_mapOfflineCell2RxId)[924041216] = 5107; // taken from run 178734
    (*m_mapOfflineCell2RxId)[924172288] = 5107; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924303360] = 5107; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924434432] = 5106; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924565504] = 5106; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924696576] = 5106; // taken from run 178577
    (*m_mapOfflineCell2RxId)[924827648] = 5106; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993482496] = 5107; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993482240] = 5107; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993481984] = 5107; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993481728] = 5107; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993481472] = 5106; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993481216] = 5106; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993480960] = 5106; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993480704] = 5106; // taken from run 178578

    //Trigger Tower : 0x4180f01 eta = 4.6875, phi = 6.0868
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[923918336] = 5105; // taken from run 178733
    (*m_mapOfflineCell2RxId)[924049408] = 5105; // taken from run 178734
    (*m_mapOfflineCell2RxId)[924180480] = 5105; // taken from run 178731
    (*m_mapOfflineCell2RxId)[924311552] = 5105; // taken from run 178732
    (*m_mapOfflineCell2RxId)[924442624] = 5104; // taken from run 178582
    (*m_mapOfflineCell2RxId)[924573696] = 5104; // taken from run 178584
    (*m_mapOfflineCell2RxId)[924704768] = 5104; // taken from run 178577
    (*m_mapOfflineCell2RxId)[924835840] = 5104; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993478400] = 5105; // taken from run 178733
    (*m_mapOnlineCell2RxId)[993478144] = 5105; // taken from run 178734
    (*m_mapOnlineCell2RxId)[993477888] = 5105; // taken from run 178731
    (*m_mapOnlineCell2RxId)[993477632] = 5105; // taken from run 178732
    (*m_mapOnlineCell2RxId)[993477376] = 5104; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993477120] = 5104; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993476864] = 5104; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993476608] = 5104; // taken from run 178578

    //Trigger Tower : 0x4180002 eta = 3.8375, phi = 0.1963
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922746880] = 5071; // taken from run 178743
    (*m_mapOfflineCell2RxId)[922877952] = 5071; // taken from run 178742
    (*m_mapOfflineCell2RxId)[923009024] = 5071; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923140096] = 5071; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923271168] = 5070; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923402240] = 5070; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923533312] = 5070; // taken from run 178720
    (*m_mapOfflineCell2RxId)[923664384] = 5070; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993443584] = 5071; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993443328] = 5071; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993443072] = 5071; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993442816] = 5071; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993442560] = 5070; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993442304] = 5070; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993442048] = 5070; // taken from run 178720
    (*m_mapOnlineCell2RxId)[993441792] = 5070; // taken from run 178718

    //Trigger Tower : 0x4180102 eta = 3.8375, phi = 0.5890
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922755072] = 5069; // taken from run 178743
    (*m_mapOfflineCell2RxId)[922886144] = 5069; // taken from run 178742
    (*m_mapOfflineCell2RxId)[923017216] = 5069; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923148288] = 5069; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923279360] = 5068; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923410432] = 5068; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923541504] = 5068; // taken from run 178720
    (*m_mapOfflineCell2RxId)[923672576] = 5068; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993439488] = 5069; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993439232] = 5069; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993438976] = 5069; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993438720] = 5069; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993438464] = 5068; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993438208] = 5068; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993437952] = 5068; // taken from run 178720
    (*m_mapOnlineCell2RxId)[993437696] = 5068; // taken from run 178718

    //Trigger Tower : 0x4180202 eta = 3.8375, phi = 0.9817
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922763264] = 5063; // taken from run 178742
    (*m_mapOfflineCell2RxId)[922894336] = 5063; // taken from run 178743
    (*m_mapOfflineCell2RxId)[923025408] = 5063; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923156480] = 5063; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923287552] = 5062; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923418624] = 5062; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923549696] = 5062; // taken from run 178718
    (*m_mapOfflineCell2RxId)[923680768] = 5062; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993435392] = 5063; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993435136] = 5063; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993434880] = 5063; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993434624] = 5063; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993434368] = 5062; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993434112] = 5062; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993433856] = 5062; // taken from run 178718
    (*m_mapOnlineCell2RxId)[993433600] = 5062; // taken from run 178720

    //Trigger Tower : 0x4180302 eta = 3.8375, phi = 1.3744
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922771456] = 5061; // taken from run 178742
    (*m_mapOfflineCell2RxId)[922902528] = 5061; // taken from run 178743
    (*m_mapOfflineCell2RxId)[923033600] = 5061; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923164672] = 5061; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923295744] = 5060; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923426816] = 5060; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923557888] = 5060; // taken from run 178718
    (*m_mapOfflineCell2RxId)[923688960] = 5060; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993431296] = 5061; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993431040] = 5061; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993430784] = 5061; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993430528] = 5061; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993430272] = 5060; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993430016] = 5060; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993429760] = 5060; // taken from run 178718
    (*m_mapOnlineCell2RxId)[993429504] = 5060; // taken from run 178720

    //Trigger Tower : 0x4180402 eta = 3.8375, phi = 1.7671
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922779648] = 5087; // taken from run 178743
    (*m_mapOfflineCell2RxId)[922910720] = 5087; // taken from run 178742
    (*m_mapOfflineCell2RxId)[923041792] = 5087; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923172864] = 5087; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923303936] = 5086; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923435008] = 5086; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923566080] = 5086; // taken from run 178720
    (*m_mapOfflineCell2RxId)[923697152] = 5086; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993459968] = 5087; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993459712] = 5087; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993459456] = 5087; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993459200] = 5087; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993458944] = 5086; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993458688] = 5086; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993458432] = 5086; // taken from run 178720
    (*m_mapOnlineCell2RxId)[993458176] = 5086; // taken from run 178718

    //Trigger Tower : 0x4180502 eta = 3.8375, phi = 2.1598
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922787840] = 5085; // taken from run 178743
    (*m_mapOfflineCell2RxId)[922918912] = 5085; // taken from run 178742
    (*m_mapOfflineCell2RxId)[923049984] = 5085; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923181056] = 5085; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923312128] = 5084; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923443200] = 5084; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923574272] = 5084; // taken from run 178720
    (*m_mapOfflineCell2RxId)[923705344] = 5084; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993455872] = 5085; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993455616] = 5085; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993455360] = 5085; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993455104] = 5085; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993454848] = 5084; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993454592] = 5084; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993454336] = 5084; // taken from run 178720
    (*m_mapOnlineCell2RxId)[993454080] = 5084; // taken from run 178718

    //Trigger Tower : 0x4180602 eta = 3.8375, phi = 2.5525
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922796032] = 5079; // taken from run 178742
    (*m_mapOfflineCell2RxId)[922927104] = 5079; // taken from run 178743
    (*m_mapOfflineCell2RxId)[923058176] = 5079; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923189248] = 5079; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923320320] = 5078; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923451392] = 5078; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923582464] = 5078; // taken from run 178718
    (*m_mapOfflineCell2RxId)[923713536] = 5078; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993451776] = 5079; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993451520] = 5079; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993451264] = 5079; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993451008] = 5079; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993450752] = 5078; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993450496] = 5078; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993450240] = 5078; // taken from run 178718
    (*m_mapOnlineCell2RxId)[993449984] = 5078; // taken from run 178720

    //Trigger Tower : 0x4180702 eta = 3.8375, phi = 2.9452
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922804224] = 5077; // taken from run 178742
    (*m_mapOfflineCell2RxId)[922935296] = 5077; // taken from run 178743
    (*m_mapOfflineCell2RxId)[923066368] = 5077; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923197440] = 5077; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923328512] = 5076; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923459584] = 5076; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923590656] = 5076; // taken from run 178718
    (*m_mapOfflineCell2RxId)[923721728] = 5076; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993447680] = 5077; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993447424] = 5077; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993447168] = 5077; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993446912] = 5077; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993446656] = 5076; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993446400] = 5076; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993446144] = 5076; // taken from run 178718
    (*m_mapOnlineCell2RxId)[993445888] = 5076; // taken from run 178720

    //Trigger Tower : 0x4180802 eta = 3.8375, phi = 3.3379
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922812416] = 5103; // taken from run 178743
    (*m_mapOfflineCell2RxId)[923074560] = 5103; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923205632] = 5103; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923336704] = 5102; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923467776] = 5102; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923598848] = 5102; // taken from run 178720
    (*m_mapOfflineCell2RxId)[923729920] = 5102; // taken from run 178718

    (*m_mapOfflineCell2RxId)[922943488] = 5103; // Not connected to Calib Lines - this is a guess
    
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993476352] = 5103; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993475840] = 5103; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993475584] = 5103; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993475328] = 5102; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993475072] = 5102; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993474816] = 5102; // taken from run 178720
    (*m_mapOnlineCell2RxId)[993474560] = 5102; // taken from run 178718

    (*m_mapOnlineCell2RxId)[993476096] = 5103; // Not connected to Calib Lines - this is a guess
    
    (*m_mapOfflineCellNotConnectedToCalibLines)[922943488] = 0;
    (*m_mapOnlineCellNotConnectedToCalibLines)[993476096] = 0;

    //Trigger Tower : 0x4180902 eta = 3.8375, phi = 3.7306
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922820608] = 5101; // taken from run 178743
    (*m_mapOfflineCell2RxId)[922951680] = 5101; // taken from run 178742
    (*m_mapOfflineCell2RxId)[923082752] = 5101; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923213824] = 5101; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923344896] = 5100; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923475968] = 5100; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923607040] = 5100; // taken from run 178720
    (*m_mapOfflineCell2RxId)[923738112] = 5100; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993472256] = 5101; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993472000] = 5101; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993471744] = 5101; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993471488] = 5101; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993471232] = 5100; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993470976] = 5100; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993470720] = 5100; // taken from run 178720
    (*m_mapOnlineCell2RxId)[993470464] = 5100; // taken from run 178718

    //Trigger Tower : 0x4180a02 eta = 3.8375, phi = 4.1233
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922828800] = 5095; // taken from run 178742
    (*m_mapOfflineCell2RxId)[922959872] = 5095; // taken from run 178743
    (*m_mapOfflineCell2RxId)[923090944] = 5095; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923222016] = 5095; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923353088] = 5094; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923484160] = 5094; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923615232] = 5094; // taken from run 178718
    (*m_mapOfflineCell2RxId)[923746304] = 5094; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993468160] = 5095; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993467904] = 5095; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993467648] = 5095; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993467392] = 5095; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993467136] = 5094; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993466880] = 5094; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993466624] = 5094; // taken from run 178718
    (*m_mapOnlineCell2RxId)[993466368] = 5094; // taken from run 178720

    //Trigger Tower : 0x4180b02 eta = 3.8375, phi = 4.5160
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922836992] = 5093; // taken from run 178742
    (*m_mapOfflineCell2RxId)[922968064] = 5093; // taken from run 178743
    (*m_mapOfflineCell2RxId)[923099136] = 5093; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923230208] = 5093; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923361280] = 5092; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923492352] = 5092; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923623424] = 5092; // taken from run 178718
    (*m_mapOfflineCell2RxId)[923754496] = 5092; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993464064] = 5093; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993463808] = 5093; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993463552] = 5093; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993463296] = 5093; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993463040] = 5092; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993462784] = 5092; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993462528] = 5092; // taken from run 178718
    (*m_mapOnlineCell2RxId)[993462272] = 5092; // taken from run 178720

    //Trigger Tower : 0x4180c02 eta = 3.8375, phi = 4.9087
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922845184] = 5119; // taken from run 178743
    (*m_mapOfflineCell2RxId)[922976256] = 5119; // taken from run 178742
    (*m_mapOfflineCell2RxId)[923107328] = 5119; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923238400] = 5119; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923369472] = 5118; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923500544] = 5118; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923631616] = 5118; // taken from run 178720
    (*m_mapOfflineCell2RxId)[923762688] = 5118; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993492736] = 5119; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993492480] = 5119; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993492224] = 5119; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993491968] = 5119; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993491712] = 5118; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993491456] = 5118; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993491200] = 5118; // taken from run 178720
    (*m_mapOnlineCell2RxId)[993490944] = 5118; // taken from run 178718

    //Trigger Tower : 0x4180d02 eta = 3.8375, phi = 5.3014
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922853376] = 5117; // taken from run 178743
    (*m_mapOfflineCell2RxId)[922984448] = 5117; // taken from run 178742
    (*m_mapOfflineCell2RxId)[923115520] = 5117; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923246592] = 5117; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923377664] = 5116; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923508736] = 5116; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923639808] = 5116; // taken from run 178720
    (*m_mapOfflineCell2RxId)[923770880] = 5116; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993488640] = 5117; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993488384] = 5117; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993488128] = 5117; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993487872] = 5117; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993487616] = 5116; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993487360] = 5116; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993487104] = 5116; // taken from run 178720
    (*m_mapOnlineCell2RxId)[993486848] = 5116; // taken from run 178718

    //Trigger Tower : 0x4180e02 eta = 3.8375, phi = 5.6941
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922861568] = 5111; // taken from run 178742
    (*m_mapOfflineCell2RxId)[922992640] = 5111; // taken from run 178743
    (*m_mapOfflineCell2RxId)[923123712] = 5111; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923254784] = 5111; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923385856] = 5110; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923516928] = 5110; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923648000] = 5110; // taken from run 178718
    (*m_mapOfflineCell2RxId)[923779072] = 5110; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993484544] = 5111; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993484288] = 5111; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993484032] = 5111; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993483776] = 5111; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993483520] = 5110; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993483264] = 5110; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993483008] = 5110; // taken from run 178718
    (*m_mapOnlineCell2RxId)[993482752] = 5110; // taken from run 178720

    //Trigger Tower : 0x4180f02 eta = 3.8375, phi = 6.0868
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[922869760] = 5109; // taken from run 178742
    (*m_mapOfflineCell2RxId)[923000832] = 5109; // taken from run 178743
    (*m_mapOfflineCell2RxId)[923131904] = 5109; // taken from run 178739
    (*m_mapOfflineCell2RxId)[923262976] = 5109; // taken from run 178741
    (*m_mapOfflineCell2RxId)[923394048] = 5108; // taken from run 178721
    (*m_mapOfflineCell2RxId)[923525120] = 5108; // taken from run 178722
    (*m_mapOfflineCell2RxId)[923656192] = 5108; // taken from run 178718
    (*m_mapOfflineCell2RxId)[923787264] = 5108; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993480448] = 5109; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993480192] = 5109; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993479936] = 5109; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993479680] = 5109; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993479424] = 5108; // taken from run 178721
    (*m_mapOnlineCell2RxId)[993479168] = 5108; // taken from run 178722
    (*m_mapOnlineCell2RxId)[993478912] = 5108; // taken from run 178718
    (*m_mapOnlineCell2RxId)[993478656] = 5108; // taken from run 178720

    //Trigger Tower : 0x4180003 eta = 4.2625, phi = 0.1963
    // This Trigger Tower has n cells = 14
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916455424] = 4999; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916586496] = 4999; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916717568] = 4999; // taken from run 178741
    (*m_mapOfflineCell2RxId)[916848640] = 4999; // taken from run 178739
    (*m_mapOfflineCell2RxId)[916979712] = 4999; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917110784] = 4999; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917241856] = 4999; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917372928] = 4999; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917504000] = 4998; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917635072] = 4998; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917766144] = 4998; // taken from run 178588
    (*m_mapOfflineCell2RxId)[917897216] = 4998; // taken from run 178586
    (*m_mapOfflineCell2RxId)[918028288] = 4998; // taken from run 178584
    (*m_mapOfflineCell2RxId)[918159360] = 4998; // taken from run 178582
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993312512] = 4999; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993312256] = 4999; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993312000] = 4999; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993311744] = 4999; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993311488] = 4999; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993311232] = 4999; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993310976] = 4999; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993310720] = 4999; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993310464] = 4998; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993310208] = 4998; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993309952] = 4998; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993309696] = 4998; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993309440] = 4998; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993309184] = 4998; // taken from run 178582

    //Trigger Tower : 0x4180103 eta = 4.2625, phi = 0.5890
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916463616] = 4997; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916594688] = 4997; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916725760] = 4997; // taken from run 178739
    (*m_mapOfflineCell2RxId)[916856832] = 4997; // taken from run 178741
    (*m_mapOfflineCell2RxId)[916987904] = 4997; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917118976] = 4997; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917250048] = 4997; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917381120] = 4997; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917512192] = 4996; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917643264] = 4996; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917774336] = 4996; // taken from run 178586
    (*m_mapOfflineCell2RxId)[917905408] = 4996; // taken from run 178588
    (*m_mapOfflineCell2RxId)[918036480] = 4996; // taken from run 178582
    (*m_mapOfflineCell2RxId)[918167552] = 4996; // taken from run 178584
    (*m_mapOfflineCell2RxId)[918298624] = 4996; // taken from run 178577
    (*m_mapOfflineCell2RxId)[918429696] = 4996; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993304320] = 4997; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993304064] = 4997; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993303808] = 4997; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993303552] = 4997; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993303296] = 4997; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993303040] = 4997; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993302784] = 4997; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993302528] = 4997; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993302272] = 4996; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993302016] = 4996; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993301760] = 4996; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993301504] = 4996; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993301248] = 4996; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993300992] = 4996; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993300736] = 4996; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993300480] = 4996; // taken from run 178578

    //Trigger Tower : 0x4180203 eta = 4.2625, phi = 0.9817
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916471808] = 5007; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916602880] = 5007; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916733952] = 5007; // taken from run 178741
    (*m_mapOfflineCell2RxId)[916865024] = 5007; // taken from run 178739
    (*m_mapOfflineCell2RxId)[916996096] = 5007; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917127168] = 5007; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917258240] = 5007; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917389312] = 5007; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917520384] = 5006; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917651456] = 5006; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917782528] = 5006; // taken from run 178588
    (*m_mapOfflineCell2RxId)[917913600] = 5006; // taken from run 178586
    (*m_mapOfflineCell2RxId)[918044672] = 5006; // taken from run 178584
    (*m_mapOfflineCell2RxId)[918175744] = 5006; // taken from run 178582
    (*m_mapOfflineCell2RxId)[918306816] = 5006; // taken from run 178578
    (*m_mapOfflineCell2RxId)[918437888] = 5006; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993328896] = 5007; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993328640] = 5007; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993328384] = 5007; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993328128] = 5007; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993327872] = 5007; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993327616] = 5007; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993327360] = 5007; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993327104] = 5007; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993326848] = 5006; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993326592] = 5006; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993326336] = 5006; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993326080] = 5006; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993325824] = 5006; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993325568] = 5006; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993325312] = 5006; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993325056] = 5006; // taken from run 178577

    //Trigger Tower : 0x4180303 eta = 4.2625, phi = 1.3744
    // This Trigger Tower has n cells = 15
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916480000] = 5005; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916611072] = 5005; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916742144] = 5005; // taken from run 178739
    (*m_mapOfflineCell2RxId)[916873216] = 5005; // taken from run 178741
    (*m_mapOfflineCell2RxId)[917004288] = 5005; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917135360] = 5005; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917266432] = 5005; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917397504] = 5005; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917528576] = 5004; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917659648] = 5004; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917790720] = 5004; // taken from run 178586
    (*m_mapOfflineCell2RxId)[917921792] = 5004; // taken from run 178588
    (*m_mapOfflineCell2RxId)[918052864] = 5004; // taken from run 178582
    (*m_mapOfflineCell2RxId)[918183936] = 5004; // taken from run 178584
    (*m_mapOfflineCell2RxId)[918315008] = 5004; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993320704] = 5005; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993320448] = 5005; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993320192] = 5005; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993319936] = 5005; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993319680] = 5005; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993319424] = 5005; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993319168] = 5005; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993318912] = 5005; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993318656] = 5004; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993318400] = 5004; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993318144] = 5004; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993317888] = 5004; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993317632] = 5004; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993317376] = 5004; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993317120] = 5004; // taken from run 178577

    //Trigger Tower : 0x4180403 eta = 4.2625, phi = 1.7671
    // This Trigger Tower has n cells = 15
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916488192] = 5015; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916619264] = 5015; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916750336] = 5015; // taken from run 178741
    (*m_mapOfflineCell2RxId)[916881408] = 5015; // taken from run 178739
    (*m_mapOfflineCell2RxId)[917012480] = 5015; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917143552] = 5015; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917274624] = 5015; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917405696] = 5015; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917536768] = 5014; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917667840] = 5014; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917798912] = 5014; // taken from run 178588
    (*m_mapOfflineCell2RxId)[917929984] = 5014; // taken from run 178586
    (*m_mapOfflineCell2RxId)[918061056] = 5014; // taken from run 178584
    (*m_mapOfflineCell2RxId)[918192128] = 5014; // taken from run 178582
    (*m_mapOfflineCell2RxId)[918323200] = 5014; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993345280] = 5015; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993345024] = 5015; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993344768] = 5015; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993344512] = 5015; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993344256] = 5015; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993344000] = 5015; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993343744] = 5015; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993343488] = 5015; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993343232] = 5014; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993342976] = 5014; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993342720] = 5014; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993342464] = 5014; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993342208] = 5014; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993341952] = 5014; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993341696] = 5014; // taken from run 178578

    //Trigger Tower : 0x4180503 eta = 4.2625, phi = 2.1598
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916496384] = 5013; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916627456] = 5013; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916758528] = 5013; // taken from run 178739
    (*m_mapOfflineCell2RxId)[916889600] = 5013; // taken from run 178741
    (*m_mapOfflineCell2RxId)[917020672] = 5013; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917151744] = 5013; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917282816] = 5013; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917413888] = 5013; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917544960] = 5012; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917676032] = 5012; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917807104] = 5012; // taken from run 178586
    (*m_mapOfflineCell2RxId)[917938176] = 5012; // taken from run 178588
    (*m_mapOfflineCell2RxId)[918069248] = 5012; // taken from run 178582
    (*m_mapOfflineCell2RxId)[918200320] = 5012; // taken from run 178584
    (*m_mapOfflineCell2RxId)[918331392] = 5012; // taken from run 178577
    (*m_mapOfflineCell2RxId)[918462464] = 5012; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993337088] = 5013; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993336832] = 5013; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993336576] = 5013; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993336320] = 5013; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993336064] = 5013; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993335808] = 5013; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993335552] = 5013; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993335296] = 5013; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993335040] = 5012; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993334784] = 5012; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993334528] = 5012; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993334272] = 5012; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993334016] = 5012; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993333760] = 5012; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993333504] = 5012; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993333248] = 5012; // taken from run 178578

    //Trigger Tower : 0x4180603 eta = 4.2625, phi = 2.5525
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916504576] = 5023; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916635648] = 5023; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916766720] = 5023; // taken from run 178741
    (*m_mapOfflineCell2RxId)[916897792] = 5023; // taken from run 178739
    (*m_mapOfflineCell2RxId)[917028864] = 5023; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917159936] = 5023; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917291008] = 5023; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917422080] = 5023; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917553152] = 5022; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917684224] = 5022; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917815296] = 5022; // taken from run 178588
    (*m_mapOfflineCell2RxId)[917946368] = 5022; // taken from run 178586
    (*m_mapOfflineCell2RxId)[918077440] = 5022; // taken from run 178584
    (*m_mapOfflineCell2RxId)[918208512] = 5022; // taken from run 178582
    (*m_mapOfflineCell2RxId)[918339584] = 5022; // taken from run 178578
    (*m_mapOfflineCell2RxId)[918470656] = 5022; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993361664] = 5023; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993361408] = 5023; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993361152] = 5023; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993360896] = 5023; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993360640] = 5023; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993360384] = 5023; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993360128] = 5023; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993359872] = 5023; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993359616] = 5022; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993359360] = 5022; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993359104] = 5022; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993358848] = 5022; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993358592] = 5022; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993358336] = 5022; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993358080] = 5022; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993357824] = 5022; // taken from run 178577

    //Trigger Tower : 0x4180703 eta = 4.2625, phi = 2.9452
    // This Trigger Tower has n cells = 14
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916512768] = 5021; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916643840] = 5021; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916774912] = 5021; // taken from run 178739
    (*m_mapOfflineCell2RxId)[916905984] = 5021; // taken from run 178741
    (*m_mapOfflineCell2RxId)[917037056] = 5021; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917168128] = 5021; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917299200] = 5021; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917430272] = 5021; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917561344] = 5020; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917692416] = 5020; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917823488] = 5020; // taken from run 178586
    (*m_mapOfflineCell2RxId)[917954560] = 5020; // taken from run 178588
    (*m_mapOfflineCell2RxId)[918085632] = 5020; // taken from run 178582
    (*m_mapOfflineCell2RxId)[918216704] = 5020; // taken from run 178584
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993353472] = 5021; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993353216] = 5021; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993352960] = 5021; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993352704] = 5021; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993352448] = 5021; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993352192] = 5021; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993351936] = 5021; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993351680] = 5021; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993351424] = 5020; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993351168] = 5020; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993350912] = 5020; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993350656] = 5020; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993350400] = 5020; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993350144] = 5020; // taken from run 178584

    //Trigger Tower : 0x4180803 eta = 4.2625, phi = 3.3379
    // This Trigger Tower has n cells = 14
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916520960] = 5031; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916652032] = 5031; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916783104] = 5031; // taken from run 178741
    (*m_mapOfflineCell2RxId)[916914176] = 5031; // taken from run 178739
    (*m_mapOfflineCell2RxId)[917045248] = 5031; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917176320] = 5031; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917307392] = 5031; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917438464] = 5031; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917569536] = 5030; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917700608] = 5030; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917831680] = 5030; // taken from run 178588
    (*m_mapOfflineCell2RxId)[917962752] = 5030; // taken from run 178586
    (*m_mapOfflineCell2RxId)[918093824] = 5030; // taken from run 178584
    (*m_mapOfflineCell2RxId)[918224896] = 5030; // taken from run 178582
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993378048] = 5031; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993377792] = 5031; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993377536] = 5031; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993377280] = 5031; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993377024] = 5031; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993376768] = 5031; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993376512] = 5031; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993376256] = 5031; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993376000] = 5030; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993375744] = 5030; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993375488] = 5030; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993375232] = 5030; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993374976] = 5030; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993374720] = 5030; // taken from run 178582

    //Trigger Tower : 0x4180903 eta = 4.2625, phi = 3.7306
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916529152] = 5029; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916660224] = 5029; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916791296] = 5029; // taken from run 178739
    (*m_mapOfflineCell2RxId)[916922368] = 5029; // taken from run 178741
    (*m_mapOfflineCell2RxId)[917053440] = 5029; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917184512] = 5029; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917315584] = 5029; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917446656] = 5029; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917577728] = 5028; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917708800] = 5028; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917839872] = 5028; // taken from run 178586
    (*m_mapOfflineCell2RxId)[917970944] = 5028; // taken from run 178588
    (*m_mapOfflineCell2RxId)[918102016] = 5028; // taken from run 178582
    (*m_mapOfflineCell2RxId)[918233088] = 5028; // taken from run 178584
    (*m_mapOfflineCell2RxId)[918364160] = 5028; // taken from run 178577
    (*m_mapOfflineCell2RxId)[918495232] = 5028; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993369856] = 5029; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993369600] = 5029; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993369344] = 5029; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993369088] = 5029; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993368832] = 5029; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993368576] = 5029; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993368320] = 5029; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993368064] = 5029; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993367808] = 5028; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993367552] = 5028; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993367296] = 5028; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993367040] = 5028; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993366784] = 5028; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993366528] = 5028; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993366272] = 5028; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993366016] = 5028; // taken from run 178578

    //Trigger Tower : 0x4180a03 eta = 4.2625, phi = 4.1233
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916537344] = 5039; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916668416] = 5039; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916799488] = 5039; // taken from run 178741
    (*m_mapOfflineCell2RxId)[916930560] = 5039; // taken from run 178739
    (*m_mapOfflineCell2RxId)[917061632] = 5039; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917192704] = 5039; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917323776] = 5039; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917454848] = 5039; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917585920] = 5038; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917716992] = 5038; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917848064] = 5038; // taken from run 178588
    (*m_mapOfflineCell2RxId)[917979136] = 5038; // taken from run 178586
    (*m_mapOfflineCell2RxId)[918110208] = 5038; // taken from run 178584
    (*m_mapOfflineCell2RxId)[918241280] = 5038; // taken from run 178582
    (*m_mapOfflineCell2RxId)[918372352] = 5038; // taken from run 178578
    (*m_mapOfflineCell2RxId)[918503424] = 5038; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993394432] = 5039; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993394176] = 5039; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993393920] = 5039; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993393664] = 5039; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993393408] = 5039; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993393152] = 5039; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993392896] = 5039; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993392640] = 5039; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993392384] = 5038; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993392128] = 5038; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993391872] = 5038; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993391616] = 5038; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993391360] = 5038; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993391104] = 5038; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993390848] = 5038; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993390592] = 5038; // taken from run 178577

    //Trigger Tower : 0x4180b03 eta = 4.2625, phi = 4.5160
    // This Trigger Tower has n cells = 15
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916545536] = 5037; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916676608] = 5037; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916807680] = 5037; // taken from run 178739
    (*m_mapOfflineCell2RxId)[916938752] = 5037; // taken from run 178741
    (*m_mapOfflineCell2RxId)[917069824] = 5037; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917200896] = 5037; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917331968] = 5037; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917463040] = 5037; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917594112] = 5036; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917725184] = 5036; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917856256] = 5036; // taken from run 178586
    (*m_mapOfflineCell2RxId)[917987328] = 5036; // taken from run 178588
    (*m_mapOfflineCell2RxId)[918118400] = 5036; // taken from run 178582
    (*m_mapOfflineCell2RxId)[918249472] = 5036; // taken from run 178584
    (*m_mapOfflineCell2RxId)[918380544] = 5036; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993386240] = 5037; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993385984] = 5037; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993385728] = 5037; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993385472] = 5037; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993385216] = 5037; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993384960] = 5037; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993384704] = 5037; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993384448] = 5037; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993384192] = 5036; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993383936] = 5036; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993383680] = 5036; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993383424] = 5036; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993383168] = 5036; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993382912] = 5036; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993382656] = 5036; // taken from run 178577

    //Trigger Tower : 0x4180c03 eta = 4.2625, phi = 4.9087
    // This Trigger Tower has n cells = 15
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916553728] = 5047; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916684800] = 5047; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916815872] = 5047; // taken from run 178741
    (*m_mapOfflineCell2RxId)[916946944] = 5047; // taken from run 178739
    (*m_mapOfflineCell2RxId)[917078016] = 5047; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917209088] = 5047; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917340160] = 5047; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917471232] = 5047; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917602304] = 5046; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917733376] = 5046; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917864448] = 5046; // taken from run 178588
    (*m_mapOfflineCell2RxId)[917995520] = 5046; // taken from run 178586
    (*m_mapOfflineCell2RxId)[918126592] = 5046; // taken from run 178584
    (*m_mapOfflineCell2RxId)[918257664] = 5046; // taken from run 178582
    (*m_mapOfflineCell2RxId)[918388736] = 5046; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993410816] = 5047; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993410560] = 5047; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993410304] = 5047; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993410048] = 5047; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993409792] = 5047; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993409536] = 5047; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993409280] = 5047; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993409024] = 5047; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993408768] = 5046; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993408512] = 5046; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993408256] = 5046; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993408000] = 5046; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993407744] = 5046; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993407488] = 5046; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993407232] = 5046; // taken from run 178578

    //Trigger Tower : 0x4180d03 eta = 4.2625, phi = 5.3014
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916561920] = 5045; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916692992] = 5045; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916824064] = 5045; // taken from run 178739
    (*m_mapOfflineCell2RxId)[916955136] = 5045; // taken from run 178741
    (*m_mapOfflineCell2RxId)[917086208] = 5045; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917217280] = 5045; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917348352] = 5045; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917610496] = 5044; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917741568] = 5044; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917872640] = 5044; // taken from run 178586
    (*m_mapOfflineCell2RxId)[918003712] = 5044; // taken from run 178588
    (*m_mapOfflineCell2RxId)[918134784] = 5044; // taken from run 178582
    (*m_mapOfflineCell2RxId)[918265856] = 5044; // taken from run 178584
    (*m_mapOfflineCell2RxId)[918396928] = 5044; // taken from run 178577
    (*m_mapOfflineCell2RxId)[918528000] = 5044; // taken from run 178578

    (*m_mapOfflineCell2RxId)[917479424] = 5045; // Not connected to Calib Lines - this is a guess
    
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993402624] = 5045; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993402368] = 5045; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993402112] = 5045; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993401856] = 5045; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993401600] = 5045; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993401344] = 5045; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993401088] = 5045; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993400576] = 5044; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993400320] = 5044; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993400064] = 5044; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993399808] = 5044; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993399552] = 5044; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993399296] = 5044; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993399040] = 5044; // taken from run 178577
    (*m_mapOnlineCell2RxId)[993398784] = 5044; // taken from run 178578

    (*m_mapOnlineCell2RxId)[993400832] = 5045; // Not connected to Calib Lines - this is a guess
    
    (*m_mapOfflineCellNotConnectedToCalibLines)[917479424] = 0;
    (*m_mapOnlineCellNotConnectedToCalibLines)[993400832] = 0;

    //Trigger Tower : 0x4180e03 eta = 4.2625, phi = 5.6941
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916570112] = 5055; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916701184] = 5055; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916832256] = 5055; // taken from run 178741
    (*m_mapOfflineCell2RxId)[916963328] = 5055; // taken from run 178739
    (*m_mapOfflineCell2RxId)[917094400] = 5055; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917225472] = 5055; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917356544] = 5055; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917487616] = 5055; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917618688] = 5054; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917749760] = 5054; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917880832] = 5054; // taken from run 178588
    (*m_mapOfflineCell2RxId)[918011904] = 5054; // taken from run 178586
    (*m_mapOfflineCell2RxId)[918142976] = 5054; // taken from run 178584
    (*m_mapOfflineCell2RxId)[918274048] = 5054; // taken from run 178582
    (*m_mapOfflineCell2RxId)[918405120] = 5054; // taken from run 178578
    (*m_mapOfflineCell2RxId)[918536192] = 5054; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993427200] = 5055; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993426944] = 5055; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993426688] = 5055; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993426432] = 5055; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993426176] = 5055; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993425920] = 5055; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993425664] = 5055; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993425408] = 5055; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993425152] = 5054; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993424896] = 5054; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993424640] = 5054; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993424384] = 5054; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993424128] = 5054; // taken from run 178584
    (*m_mapOnlineCell2RxId)[993423872] = 5054; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993423616] = 5054; // taken from run 178578
    (*m_mapOnlineCell2RxId)[993423360] = 5054; // taken from run 178577

    //Trigger Tower : 0x4180f03 eta = 4.2625, phi = 6.0868
    // This Trigger Tower has n cells = 14
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[916578304] = 5053; // taken from run 178742
    (*m_mapOfflineCell2RxId)[916709376] = 5053; // taken from run 178743
    (*m_mapOfflineCell2RxId)[916840448] = 5053; // taken from run 178739
    (*m_mapOfflineCell2RxId)[916971520] = 5053; // taken from run 178741
    (*m_mapOfflineCell2RxId)[917102592] = 5053; // taken from run 178737
    (*m_mapOfflineCell2RxId)[917233664] = 5053; // taken from run 178738
    (*m_mapOfflineCell2RxId)[917364736] = 5053; // taken from run 178735
    (*m_mapOfflineCell2RxId)[917495808] = 5053; // taken from run 178736
    (*m_mapOfflineCell2RxId)[917626880] = 5052; // taken from run 178716
    (*m_mapOfflineCell2RxId)[917757952] = 5052; // taken from run 178717
    (*m_mapOfflineCell2RxId)[917889024] = 5052; // taken from run 178586
    (*m_mapOfflineCell2RxId)[918020096] = 5052; // taken from run 178588
    (*m_mapOfflineCell2RxId)[918151168] = 5052; // taken from run 178582
    (*m_mapOfflineCell2RxId)[918282240] = 5052; // taken from run 178584
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[993419008] = 5053; // taken from run 178742
    (*m_mapOnlineCell2RxId)[993418752] = 5053; // taken from run 178743
    (*m_mapOnlineCell2RxId)[993418496] = 5053; // taken from run 178739
    (*m_mapOnlineCell2RxId)[993418240] = 5053; // taken from run 178741
    (*m_mapOnlineCell2RxId)[993417984] = 5053; // taken from run 178737
    (*m_mapOnlineCell2RxId)[993417728] = 5053; // taken from run 178738
    (*m_mapOnlineCell2RxId)[993417472] = 5053; // taken from run 178735
    (*m_mapOnlineCell2RxId)[993417216] = 5053; // taken from run 178736
    (*m_mapOnlineCell2RxId)[993416960] = 5052; // taken from run 178716
    (*m_mapOnlineCell2RxId)[993416704] = 5052; // taken from run 178717
    (*m_mapOnlineCell2RxId)[993416448] = 5052; // taken from run 178586
    (*m_mapOnlineCell2RxId)[993416192] = 5052; // taken from run 178588
    (*m_mapOnlineCell2RxId)[993415936] = 5052; // taken from run 178582
    (*m_mapOnlineCell2RxId)[993415680] = 5052; // taken from run 178584

    //Trigger Tower : 0x5180000 eta = -4.6875, phi = 0.1963
    // This Trigger Tower has n cells = 14
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[882900992] = 6045; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883032064] = 6045; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883163136] = 6045; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883294208] = 6045; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883425280] = 6045; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883556352] = 6045; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883687424] = 6045; // taken from run 178735
    (*m_mapOfflineCell2RxId)[883818496] = 6045; // taken from run 178736
    (*m_mapOfflineCell2RxId)[883949568] = 6044; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884080640] = 6044; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884211712] = 6044; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884342784] = 6044; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884473856] = 6044; // taken from run 178582
    (*m_mapOfflineCell2RxId)[884604928] = 6044; // taken from run 178584
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976576256] = 6045; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976576000] = 6045; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976575744] = 6045; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976575488] = 6045; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976575232] = 6045; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976574976] = 6045; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976574720] = 6045; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976574464] = 6045; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976574208] = 6044; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976573952] = 6044; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976573696] = 6044; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976573440] = 6044; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976573184] = 6044; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976572928] = 6044; // taken from run 178584

    //Trigger Tower : 0x5180100 eta = -4.6875, phi = 0.5890
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[882909184] = 6047; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883040256] = 6047; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883171328] = 6047; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883302400] = 6047; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883433472] = 6047; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883564544] = 6047; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883695616] = 6047; // taken from run 178736
    (*m_mapOfflineCell2RxId)[883826688] = 6047; // taken from run 178735
    (*m_mapOfflineCell2RxId)[883957760] = 6046; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884088832] = 6046; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884219904] = 6046; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884350976] = 6046; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884482048] = 6046; // taken from run 178584
    (*m_mapOfflineCell2RxId)[884613120] = 6046; // taken from run 178582
    (*m_mapOfflineCell2RxId)[884744192] = 6046; // taken from run 178578
    (*m_mapOfflineCell2RxId)[884875264] = 6046; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976584448] = 6047; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976584192] = 6047; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976583936] = 6047; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976583680] = 6047; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976583424] = 6047; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976583168] = 6047; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976582912] = 6047; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976582656] = 6047; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976582400] = 6046; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976582144] = 6046; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976581888] = 6046; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976581632] = 6046; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976581376] = 6046; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976581120] = 6046; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976580864] = 6046; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976580608] = 6046; // taken from run 178577

    //Trigger Tower : 0x5180200 eta = -4.6875, phi = 0.9817
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[882917376] = 6037; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883048448] = 6037; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883179520] = 6037; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883310592] = 6037; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883441664] = 6037; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883572736] = 6037; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883703808] = 6037; // taken from run 178735
    (*m_mapOfflineCell2RxId)[883834880] = 6037; // taken from run 178736
    (*m_mapOfflineCell2RxId)[883965952] = 6036; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884097024] = 6036; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884228096] = 6036; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884359168] = 6036; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884490240] = 6036; // taken from run 178582
    (*m_mapOfflineCell2RxId)[884621312] = 6036; // taken from run 178584
    (*m_mapOfflineCell2RxId)[884752384] = 6036; // taken from run 178577
    (*m_mapOfflineCell2RxId)[884883456] = 6036; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976559872] = 6037; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976559616] = 6037; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976559360] = 6037; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976559104] = 6037; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976558848] = 6037; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976558592] = 6037; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976558336] = 6037; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976558080] = 6037; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976557824] = 6036; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976557568] = 6036; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976557312] = 6036; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976557056] = 6036; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976556800] = 6036; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976556544] = 6036; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976556288] = 6036; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976556032] = 6036; // taken from run 178578

    //Trigger Tower : 0x5180300 eta = -4.6875, phi = 1.3744
    // This Trigger Tower has n cells = 15
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[882925568] = 6039; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883056640] = 6039; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883187712] = 6039; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883318784] = 6039; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883449856] = 6039; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883580928] = 6039; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883712000] = 6039; // taken from run 178736
    (*m_mapOfflineCell2RxId)[883843072] = 6039; // taken from run 178735
    (*m_mapOfflineCell2RxId)[883974144] = 6038; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884105216] = 6038; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884236288] = 6038; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884367360] = 6038; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884498432] = 6038; // taken from run 178584
    (*m_mapOfflineCell2RxId)[884629504] = 6038; // taken from run 178582
    (*m_mapOfflineCell2RxId)[884760576] = 6038; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976568064] = 6039; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976567808] = 6039; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976567552] = 6039; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976567296] = 6039; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976567040] = 6039; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976566784] = 6039; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976566528] = 6039; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976566272] = 6039; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976566016] = 6038; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976565760] = 6038; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976565504] = 6038; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976565248] = 6038; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976564992] = 6038; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976564736] = 6038; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976564480] = 6038; // taken from run 178578

    //Trigger Tower : 0x5180400 eta = -4.6875, phi = 1.7671
    // This Trigger Tower has n cells = 15
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[882933760] = 6029; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883064832] = 6029; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883195904] = 6029; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883326976] = 6029; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883458048] = 6029; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883589120] = 6029; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883720192] = 6029; // taken from run 178735
    (*m_mapOfflineCell2RxId)[883851264] = 6029; // taken from run 178736
    (*m_mapOfflineCell2RxId)[883982336] = 6028; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884113408] = 6028; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884244480] = 6028; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884375552] = 6028; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884506624] = 6028; // taken from run 178582
    (*m_mapOfflineCell2RxId)[884637696] = 6028; // taken from run 178584
    (*m_mapOfflineCell2RxId)[884768768] = 6028; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976543488] = 6029; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976543232] = 6029; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976542976] = 6029; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976542720] = 6029; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976542464] = 6029; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976542208] = 6029; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976541952] = 6029; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976541696] = 6029; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976541440] = 6028; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976541184] = 6028; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976540928] = 6028; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976540672] = 6028; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976540416] = 6028; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976540160] = 6028; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976539904] = 6028; // taken from run 178577

    //Trigger Tower : 0x5180500 eta = -4.6875, phi = 2.1598
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[882941952] = 6031; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883073024] = 6031; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883204096] = 6031; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883335168] = 6031; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883466240] = 6031; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883597312] = 6031; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883728384] = 6031; // taken from run 178736
    (*m_mapOfflineCell2RxId)[883859456] = 6031; // taken from run 178735
    (*m_mapOfflineCell2RxId)[883990528] = 6030; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884121600] = 6030; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884252672] = 6030; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884383744] = 6030; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884514816] = 6030; // taken from run 178584
    (*m_mapOfflineCell2RxId)[884645888] = 6030; // taken from run 178582
    (*m_mapOfflineCell2RxId)[884776960] = 6030; // taken from run 178578
    (*m_mapOfflineCell2RxId)[884908032] = 6030; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976551680] = 6031; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976551424] = 6031; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976551168] = 6031; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976550912] = 6031; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976550656] = 6031; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976550400] = 6031; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976550144] = 6031; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976549888] = 6031; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976549632] = 6030; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976549376] = 6030; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976549120] = 6030; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976548864] = 6030; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976548608] = 6030; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976548352] = 6030; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976548096] = 6030; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976547840] = 6030; // taken from run 178577

    //Trigger Tower : 0x5180600 eta = -4.6875, phi = 2.5525
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[882950144] = 6021; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883081216] = 6021; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883212288] = 6021; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883343360] = 6021; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883474432] = 6021; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883605504] = 6021; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883736576] = 6021; // taken from run 178735
    (*m_mapOfflineCell2RxId)[883867648] = 6021; // taken from run 178736
    (*m_mapOfflineCell2RxId)[883998720] = 6020; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884129792] = 6020; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884260864] = 6020; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884391936] = 6020; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884523008] = 6020; // taken from run 178582
    (*m_mapOfflineCell2RxId)[884654080] = 6020; // taken from run 178584
    (*m_mapOfflineCell2RxId)[884785152] = 6020; // taken from run 178577
    (*m_mapOfflineCell2RxId)[884916224] = 6020; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976527104] = 6021; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976526848] = 6021; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976526592] = 6021; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976526336] = 6021; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976526080] = 6021; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976525824] = 6021; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976525568] = 6021; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976525312] = 6021; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976525056] = 6020; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976524800] = 6020; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976524544] = 6020; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976524288] = 6020; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976524032] = 6020; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976523776] = 6020; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976523520] = 6020; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976523264] = 6020; // taken from run 178578

    //Trigger Tower : 0x5180700 eta = -4.6875, phi = 2.9452
    // This Trigger Tower has n cells = 14
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[882958336] = 6023; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883089408] = 6023; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883220480] = 6023; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883351552] = 6023; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883482624] = 6023; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883613696] = 6023; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883744768] = 6023; // taken from run 178736
    (*m_mapOfflineCell2RxId)[883875840] = 6023; // taken from run 178735
    (*m_mapOfflineCell2RxId)[884006912] = 6022; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884137984] = 6022; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884269056] = 6022; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884400128] = 6022; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884531200] = 6022; // taken from run 178584
    (*m_mapOfflineCell2RxId)[884662272] = 6022; // taken from run 178582
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976535296] = 6023; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976535040] = 6023; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976534784] = 6023; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976534528] = 6023; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976534272] = 6023; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976534016] = 6023; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976533760] = 6023; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976533504] = 6023; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976533248] = 6022; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976532992] = 6022; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976532736] = 6022; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976532480] = 6022; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976532224] = 6022; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976531968] = 6022; // taken from run 178582

    //Trigger Tower : 0x5180800 eta = -4.6875, phi = 3.3379
    // This Trigger Tower has n cells = 14
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[882966528] = 6077; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883097600] = 6077; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883228672] = 6077; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883359744] = 6077; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883490816] = 6077; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883621888] = 6077; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883752960] = 6077; // taken from run 178735
    (*m_mapOfflineCell2RxId)[883884032] = 6077; // taken from run 178736
    (*m_mapOfflineCell2RxId)[884015104] = 6076; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884146176] = 6076; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884277248] = 6076; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884408320] = 6076; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884539392] = 6076; // taken from run 178582
    (*m_mapOfflineCell2RxId)[884670464] = 6076; // taken from run 178584
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976641792] = 6077; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976641536] = 6077; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976641280] = 6077; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976641024] = 6077; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976640768] = 6077; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976640512] = 6077; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976640256] = 6077; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976640000] = 6077; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976639744] = 6076; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976639488] = 6076; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976639232] = 6076; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976638976] = 6076; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976638720] = 6076; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976638464] = 6076; // taken from run 178584

    //Trigger Tower : 0x5180900 eta = -4.6875, phi = 3.7306
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[882974720] = 6079; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883105792] = 6079; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883236864] = 6079; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883367936] = 6079; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883499008] = 6079; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883630080] = 6079; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883761152] = 6079; // taken from run 178736
    (*m_mapOfflineCell2RxId)[883892224] = 6079; // taken from run 178735
    (*m_mapOfflineCell2RxId)[884023296] = 6078; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884154368] = 6078; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884285440] = 6078; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884416512] = 6078; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884547584] = 6078; // taken from run 178584
    (*m_mapOfflineCell2RxId)[884678656] = 6078; // taken from run 178582
    (*m_mapOfflineCell2RxId)[884809728] = 6078; // taken from run 178578
    (*m_mapOfflineCell2RxId)[884940800] = 6078; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976649984] = 6079; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976649728] = 6079; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976649472] = 6079; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976649216] = 6079; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976648960] = 6079; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976648704] = 6079; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976648448] = 6079; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976648192] = 6079; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976647936] = 6078; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976647680] = 6078; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976647424] = 6078; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976647168] = 6078; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976646912] = 6078; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976646656] = 6078; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976646400] = 6078; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976646144] = 6078; // taken from run 178577

    //Trigger Tower : 0x5180a00 eta = -4.6875, phi = 4.1233
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[882982912] = 6069; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883113984] = 6069; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883245056] = 6069; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883376128] = 6069; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883507200] = 6069; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883638272] = 6069; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883769344] = 6069; // taken from run 178735
    (*m_mapOfflineCell2RxId)[883900416] = 6069; // taken from run 178736
    (*m_mapOfflineCell2RxId)[884031488] = 6068; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884162560] = 6068; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884293632] = 6068; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884424704] = 6068; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884555776] = 6068; // taken from run 178582
    (*m_mapOfflineCell2RxId)[884686848] = 6068; // taken from run 178584
    (*m_mapOfflineCell2RxId)[884817920] = 6068; // taken from run 178577
    (*m_mapOfflineCell2RxId)[884948992] = 6068; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976625408] = 6069; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976625152] = 6069; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976624896] = 6069; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976624640] = 6069; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976624384] = 6069; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976624128] = 6069; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976623872] = 6069; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976623616] = 6069; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976623360] = 6068; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976623104] = 6068; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976622848] = 6068; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976622592] = 6068; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976622336] = 6068; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976622080] = 6068; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976621824] = 6068; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976621568] = 6068; // taken from run 178578

    //Trigger Tower : 0x5180b00 eta = -4.6875, phi = 4.5160
    // This Trigger Tower has n cells = 15
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[882991104] = 6071; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883122176] = 6071; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883253248] = 6071; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883384320] = 6071; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883515392] = 6071; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883646464] = 6071; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883777536] = 6071; // taken from run 178736
    (*m_mapOfflineCell2RxId)[883908608] = 6071; // taken from run 178735
    (*m_mapOfflineCell2RxId)[884039680] = 6070; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884170752] = 6070; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884301824] = 6070; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884432896] = 6070; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884563968] = 6070; // taken from run 178584
    (*m_mapOfflineCell2RxId)[884695040] = 6070; // taken from run 178582
    (*m_mapOfflineCell2RxId)[884826112] = 6070; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976633600] = 6071; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976633344] = 6071; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976633088] = 6071; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976632832] = 6071; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976632576] = 6071; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976632320] = 6071; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976632064] = 6071; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976631808] = 6071; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976631552] = 6070; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976631296] = 6070; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976631040] = 6070; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976630784] = 6070; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976630528] = 6070; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976630272] = 6070; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976630016] = 6070; // taken from run 178578

    //Trigger Tower : 0x5180c00 eta = -4.6875, phi = 4.9087
    // This Trigger Tower has n cells = 15
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[882999296] = 6061; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883130368] = 6061; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883261440] = 6061; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883392512] = 6061; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883523584] = 6061; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883654656] = 6061; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883785728] = 6061; // taken from run 178735
    (*m_mapOfflineCell2RxId)[883916800] = 6061; // taken from run 178736
    (*m_mapOfflineCell2RxId)[884047872] = 6060; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884178944] = 6060; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884310016] = 6060; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884441088] = 6060; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884572160] = 6060; // taken from run 178582
    (*m_mapOfflineCell2RxId)[884703232] = 6060; // taken from run 178584
    (*m_mapOfflineCell2RxId)[884834304] = 6060; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976609024] = 6061; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976608768] = 6061; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976608512] = 6061; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976608256] = 6061; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976608000] = 6061; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976607744] = 6061; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976607488] = 6061; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976607232] = 6061; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976606976] = 6060; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976606720] = 6060; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976606464] = 6060; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976606208] = 6060; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976605952] = 6060; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976605696] = 6060; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976605440] = 6060; // taken from run 178577

    //Trigger Tower : 0x5180d00 eta = -4.6875, phi = 5.3014
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[883007488] = 6063; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883138560] = 6063; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883269632] = 6063; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883400704] = 6063; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883531776] = 6063; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883662848] = 6063; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883793920] = 6063; // taken from run 178736
    (*m_mapOfflineCell2RxId)[883924992] = 6063; // taken from run 178735
    (*m_mapOfflineCell2RxId)[884056064] = 6062; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884187136] = 6062; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884318208] = 6062; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884449280] = 6062; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884580352] = 6062; // taken from run 178584
    (*m_mapOfflineCell2RxId)[884711424] = 6062; // taken from run 178582
    (*m_mapOfflineCell2RxId)[884842496] = 6062; // taken from run 178578
    (*m_mapOfflineCell2RxId)[884973568] = 6062; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976617216] = 6063; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976616960] = 6063; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976616704] = 6063; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976616448] = 6063; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976616192] = 6063; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976615936] = 6063; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976615680] = 6063; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976615424] = 6063; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976615168] = 6062; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976614912] = 6062; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976614656] = 6062; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976614400] = 6062; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976614144] = 6062; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976613888] = 6062; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976613632] = 6062; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976613376] = 6062; // taken from run 178577

    //Trigger Tower : 0x5180e00 eta = -4.6875, phi = 5.6941
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[883015680] = 6053; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883146752] = 6053; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883277824] = 6053; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883408896] = 6053; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883539968] = 6053; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883671040] = 6053; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883802112] = 6053; // taken from run 178735
    (*m_mapOfflineCell2RxId)[883933184] = 6053; // taken from run 178736
    (*m_mapOfflineCell2RxId)[884064256] = 6052; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884195328] = 6052; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884326400] = 6052; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884457472] = 6052; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884588544] = 6052; // taken from run 178582
    (*m_mapOfflineCell2RxId)[884719616] = 6052; // taken from run 178584
    (*m_mapOfflineCell2RxId)[884850688] = 6052; // taken from run 178577
    (*m_mapOfflineCell2RxId)[884981760] = 6052; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976592640] = 6053; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976592384] = 6053; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976592128] = 6053; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976591872] = 6053; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976591616] = 6053; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976591360] = 6053; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976591104] = 6053; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976590848] = 6053; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976590592] = 6052; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976590336] = 6052; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976590080] = 6052; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976589824] = 6052; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976589568] = 6052; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976589312] = 6052; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976589056] = 6052; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976588800] = 6052; // taken from run 178578

    //Trigger Tower : 0x5180f00 eta = -4.6875, phi = 6.0868
    // This Trigger Tower has n cells = 14
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[883023872] = 6055; // taken from run 178743
    (*m_mapOfflineCell2RxId)[883154944] = 6055; // taken from run 178742
    (*m_mapOfflineCell2RxId)[883286016] = 6055; // taken from run 178741
    (*m_mapOfflineCell2RxId)[883417088] = 6055; // taken from run 178739
    (*m_mapOfflineCell2RxId)[883548160] = 6055; // taken from run 178738
    (*m_mapOfflineCell2RxId)[883679232] = 6055; // taken from run 178737
    (*m_mapOfflineCell2RxId)[883810304] = 6055; // taken from run 178736
    (*m_mapOfflineCell2RxId)[883941376] = 6055; // taken from run 178735
    (*m_mapOfflineCell2RxId)[884072448] = 6054; // taken from run 178717
    (*m_mapOfflineCell2RxId)[884203520] = 6054; // taken from run 178716
    (*m_mapOfflineCell2RxId)[884334592] = 6054; // taken from run 178588
    (*m_mapOfflineCell2RxId)[884465664] = 6054; // taken from run 178586
    (*m_mapOfflineCell2RxId)[884596736] = 6054; // taken from run 178584
    (*m_mapOfflineCell2RxId)[884727808] = 6054; // taken from run 178582
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976600832] = 6055; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976600576] = 6055; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976600320] = 6055; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976600064] = 6055; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976599808] = 6055; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976599552] = 6055; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976599296] = 6055; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976599040] = 6055; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976598784] = 6054; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976598528] = 6054; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976598272] = 6054; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976598016] = 6054; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976597760] = 6054; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976597504] = 6054; // taken from run 178582

    //Trigger Tower : 0x5180001 eta = -3.4125, phi = 0.1963
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889192448] = 6111; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889323520] = 6111; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889454592] = 6111; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889585664] = 6111; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889716736] = 6110; // taken from run 178722
    (*m_mapOfflineCell2RxId)[889847808] = 6110; // taken from run 178721
    (*m_mapOfflineCell2RxId)[889978880] = 6110; // taken from run 178720
    (*m_mapOfflineCell2RxId)[890109952] = 6110; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976682752] = 6111; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976682496] = 6111; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976682240] = 6111; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976681984] = 6111; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976681728] = 6110; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976681472] = 6110; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976681216] = 6110; // taken from run 178720
    (*m_mapOnlineCell2RxId)[976680960] = 6110; // taken from run 178718

    //Trigger Tower : 0x5180101 eta = -3.4125, phi = 0.5890
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889200640] = 6109; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889331712] = 6109; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889462784] = 6109; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889593856] = 6109; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889724928] = 6108; // taken from run 178722
    (*m_mapOfflineCell2RxId)[889856000] = 6108; // taken from run 178721
    (*m_mapOfflineCell2RxId)[889987072] = 6108; // taken from run 178720
    (*m_mapOfflineCell2RxId)[890118144] = 6108; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976678656] = 6109; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976678400] = 6109; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976678144] = 6109; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976677888] = 6109; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976677632] = 6108; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976677376] = 6108; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976677120] = 6108; // taken from run 178720
    (*m_mapOnlineCell2RxId)[976676864] = 6108; // taken from run 178718

    //Trigger Tower : 0x5180201 eta = -3.4125, phi = 0.9817
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889208832] = 6103; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889339904] = 6103; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889470976] = 6103; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889602048] = 6103; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889733120] = 6102; // taken from run 178721
    (*m_mapOfflineCell2RxId)[889864192] = 6102; // taken from run 178722
    (*m_mapOfflineCell2RxId)[889995264] = 6102; // taken from run 178718
    (*m_mapOfflineCell2RxId)[890126336] = 6102; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976674560] = 6103; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976674304] = 6103; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976674048] = 6103; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976673792] = 6103; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976673536] = 6102; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976673280] = 6102; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976673024] = 6102; // taken from run 178718
    (*m_mapOnlineCell2RxId)[976672768] = 6102; // taken from run 178720

    //Trigger Tower : 0x5180301 eta = -3.4125, phi = 1.3744
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889217024] = 6101; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889348096] = 6101; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889479168] = 6101; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889610240] = 6101; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889741312] = 6100; // taken from run 178721
    (*m_mapOfflineCell2RxId)[889872384] = 6100; // taken from run 178722
    (*m_mapOfflineCell2RxId)[890003456] = 6100; // taken from run 178718
    (*m_mapOfflineCell2RxId)[890134528] = 6100; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976670464] = 6101; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976670208] = 6101; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976669952] = 6101; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976669696] = 6101; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976669440] = 6100; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976669184] = 6100; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976668928] = 6100; // taken from run 178718
    (*m_mapOnlineCell2RxId)[976668672] = 6100; // taken from run 178720

    //Trigger Tower : 0x5180401 eta = -3.4125, phi = 1.7671
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889225216] = 6095; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889356288] = 6095; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889487360] = 6095; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889618432] = 6095; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889749504] = 6094; // taken from run 178722
    (*m_mapOfflineCell2RxId)[889880576] = 6094; // taken from run 178721
    (*m_mapOfflineCell2RxId)[890011648] = 6094; // taken from run 178720
    (*m_mapOfflineCell2RxId)[890142720] = 6094; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976666368] = 6095; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976666112] = 6095; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976665856] = 6095; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976665600] = 6095; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976665344] = 6094; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976665088] = 6094; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976664832] = 6094; // taken from run 178720
    (*m_mapOnlineCell2RxId)[976664576] = 6094; // taken from run 178718

    //Trigger Tower : 0x5180501 eta = -3.4125, phi = 2.1598
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889233408] = 6093; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889364480] = 6093; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889495552] = 6093; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889626624] = 6093; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889757696] = 6092; // taken from run 178722
    (*m_mapOfflineCell2RxId)[889888768] = 6092; // taken from run 178721
    (*m_mapOfflineCell2RxId)[890019840] = 6092; // taken from run 178720
    (*m_mapOfflineCell2RxId)[890150912] = 6092; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976662272] = 6093; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976662016] = 6093; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976661760] = 6093; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976661504] = 6093; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976661248] = 6092; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976660992] = 6092; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976660736] = 6092; // taken from run 178720
    (*m_mapOnlineCell2RxId)[976660480] = 6092; // taken from run 178718

    //Trigger Tower : 0x5180601 eta = -3.4125, phi = 2.5525
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889241600] = 6087; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889372672] = 6087; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889503744] = 6087; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889634816] = 6087; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889765888] = 6086; // taken from run 178721
    (*m_mapOfflineCell2RxId)[889896960] = 6086; // taken from run 178722
    (*m_mapOfflineCell2RxId)[890028032] = 6086; // taken from run 178718
    (*m_mapOfflineCell2RxId)[890159104] = 6086; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976658176] = 6087; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976657920] = 6087; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976657664] = 6087; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976657408] = 6087; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976657152] = 6086; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976656896] = 6086; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976656640] = 6086; // taken from run 178718
    (*m_mapOnlineCell2RxId)[976656384] = 6086; // taken from run 178720

    //Trigger Tower : 0x5180701 eta = -3.4125, phi = 2.9452
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889249792] = 6085; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889380864] = 6085; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889511936] = 6085; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889643008] = 6085; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889774080] = 6084; // taken from run 178721
    (*m_mapOfflineCell2RxId)[889905152] = 6084; // taken from run 178722
    (*m_mapOfflineCell2RxId)[890036224] = 6084; // taken from run 178718
    (*m_mapOfflineCell2RxId)[890167296] = 6084; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976654080] = 6085; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976653824] = 6085; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976653568] = 6085; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976653312] = 6085; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976653056] = 6084; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976652800] = 6084; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976652544] = 6084; // taken from run 178718
    (*m_mapOnlineCell2RxId)[976652288] = 6084; // taken from run 178720

    //Trigger Tower : 0x5180801 eta = -3.4125, phi = 3.3379
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889257984] = 6143; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889389056] = 6143; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889520128] = 6143; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889651200] = 6143; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889782272] = 6142; // taken from run 178722
    (*m_mapOfflineCell2RxId)[889913344] = 6142; // taken from run 178721
    (*m_mapOfflineCell2RxId)[890044416] = 6142; // taken from run 178720
    (*m_mapOfflineCell2RxId)[890175488] = 6142; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976715520] = 6143; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976715264] = 6143; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976715008] = 6143; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976714752] = 6143; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976714496] = 6142; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976714240] = 6142; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976713984] = 6142; // taken from run 178720
    (*m_mapOnlineCell2RxId)[976713728] = 6142; // taken from run 178718

    //Trigger Tower : 0x5180901 eta = -3.4125, phi = 3.7306
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889266176] = 6141; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889397248] = 6141; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889528320] = 6141; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889659392] = 6141; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889790464] = 6140; // taken from run 178722
    (*m_mapOfflineCell2RxId)[889921536] = 6140; // taken from run 178721
    (*m_mapOfflineCell2RxId)[890052608] = 6140; // taken from run 178720
    (*m_mapOfflineCell2RxId)[890183680] = 6140; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976711424] = 6141; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976711168] = 6141; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976710912] = 6141; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976710656] = 6141; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976710400] = 6140; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976710144] = 6140; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976709888] = 6140; // taken from run 178720
    (*m_mapOnlineCell2RxId)[976709632] = 6140; // taken from run 178718

    //Trigger Tower : 0x5180a01 eta = -3.4125, phi = 4.1233
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889274368] = 6135; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889405440] = 6135; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889536512] = 6135; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889667584] = 6135; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889798656] = 6134; // taken from run 178721
    (*m_mapOfflineCell2RxId)[889929728] = 6134; // taken from run 178722
    (*m_mapOfflineCell2RxId)[890060800] = 6134; // taken from run 178718
    (*m_mapOfflineCell2RxId)[890191872] = 6134; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976707328] = 6135; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976707072] = 6135; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976706816] = 6135; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976706560] = 6135; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976706304] = 6134; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976706048] = 6134; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976705792] = 6134; // taken from run 178718
    (*m_mapOnlineCell2RxId)[976705536] = 6134; // taken from run 178720

    //Trigger Tower : 0x5180b01 eta = -3.4125, phi = 4.5160
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889282560] = 6133; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889413632] = 6133; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889544704] = 6133; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889675776] = 6133; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889806848] = 6132; // taken from run 178721
    (*m_mapOfflineCell2RxId)[889937920] = 6132; // taken from run 178722
    (*m_mapOfflineCell2RxId)[890068992] = 6132; // taken from run 178718
    (*m_mapOfflineCell2RxId)[890200064] = 6132; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976703232] = 6133; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976702976] = 6133; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976702720] = 6133; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976702464] = 6133; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976702208] = 6132; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976701952] = 6132; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976701696] = 6132; // taken from run 178718
    (*m_mapOnlineCell2RxId)[976701440] = 6132; // taken from run 178720

    //Trigger Tower : 0x5180c01 eta = -3.4125, phi = 4.9087
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889290752] = 6127; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889421824] = 6127; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889552896] = 6127; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889683968] = 6127; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889815040] = 6126; // taken from run 178722
    (*m_mapOfflineCell2RxId)[889946112] = 6126; // taken from run 178721
    (*m_mapOfflineCell2RxId)[890077184] = 6126; // taken from run 178720
    (*m_mapOfflineCell2RxId)[890208256] = 6126; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976699136] = 6127; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976698880] = 6127; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976698624] = 6127; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976698368] = 6127; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976698112] = 6126; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976697856] = 6126; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976697600] = 6126; // taken from run 178720
    (*m_mapOnlineCell2RxId)[976697344] = 6126; // taken from run 178718

    //Trigger Tower : 0x5180d01 eta = -3.4125, phi = 5.3014
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889298944] = 6125; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889430016] = 6125; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889561088] = 6125; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889692160] = 6125; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889823232] = 6124; // taken from run 178722
    (*m_mapOfflineCell2RxId)[889954304] = 6124; // taken from run 178721
    (*m_mapOfflineCell2RxId)[890085376] = 6124; // taken from run 178720
    (*m_mapOfflineCell2RxId)[890216448] = 6124; // taken from run 178718
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976695040] = 6125; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976694784] = 6125; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976694528] = 6125; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976694272] = 6125; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976694016] = 6124; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976693760] = 6124; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976693504] = 6124; // taken from run 178720
    (*m_mapOnlineCell2RxId)[976693248] = 6124; // taken from run 178718

    //Trigger Tower : 0x5180e01 eta = -3.4125, phi = 5.6941
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889307136] = 6119; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889438208] = 6119; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889569280] = 6119; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889700352] = 6119; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889831424] = 6118; // taken from run 178721
    (*m_mapOfflineCell2RxId)[889962496] = 6118; // taken from run 178722
    (*m_mapOfflineCell2RxId)[890093568] = 6118; // taken from run 178718
    (*m_mapOfflineCell2RxId)[890224640] = 6118; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976690944] = 6119; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976690688] = 6119; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976690432] = 6119; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976690176] = 6119; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976689920] = 6118; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976689664] = 6118; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976689408] = 6118; // taken from run 178718
    (*m_mapOnlineCell2RxId)[976689152] = 6118; // taken from run 178720

    //Trigger Tower : 0x5180f01 eta = -3.4125, phi = 6.0868
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[889315328] = 6117; // taken from run 178742
    (*m_mapOfflineCell2RxId)[889446400] = 6117; // taken from run 178743
    (*m_mapOfflineCell2RxId)[889577472] = 6117; // taken from run 178739
    (*m_mapOfflineCell2RxId)[889708544] = 6117; // taken from run 178741
    (*m_mapOfflineCell2RxId)[889839616] = 6116; // taken from run 178721
    (*m_mapOfflineCell2RxId)[889970688] = 6116; // taken from run 178722
    (*m_mapOfflineCell2RxId)[890101760] = 6116; // taken from run 178718
    (*m_mapOfflineCell2RxId)[890232832] = 6116; // taken from run 178720
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976686848] = 6117; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976686592] = 6117; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976686336] = 6117; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976686080] = 6117; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976685824] = 6116; // taken from run 178721
    (*m_mapOnlineCell2RxId)[976685568] = 6116; // taken from run 178722
    (*m_mapOnlineCell2RxId)[976685312] = 6116; // taken from run 178718
    (*m_mapOnlineCell2RxId)[976685056] = 6116; // taken from run 178720

    //Trigger Tower : 0x5180002 eta = -4.2625, phi = 0.1963
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890241024] = 6107; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890372096] = 6107; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890503168] = 6107; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890634240] = 6107; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890765312] = 6106; // taken from run 178584
    (*m_mapOfflineCell2RxId)[890896384] = 6106; // taken from run 178582
    (*m_mapOfflineCell2RxId)[891027456] = 6106; // taken from run 178578
    (*m_mapOfflineCell2RxId)[891158528] = 6106; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976680704] = 6107; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976680448] = 6107; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976680192] = 6107; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976679936] = 6107; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976679680] = 6106; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976679424] = 6106; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976679168] = 6106; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976678912] = 6106; // taken from run 178577

    //Trigger Tower : 0x5180102 eta = -4.2625, phi = 0.5890
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890249216] = 6105; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890380288] = 6105; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890511360] = 6105; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890642432] = 6105; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890773504] = 6104; // taken from run 178584
    (*m_mapOfflineCell2RxId)[890904576] = 6104; // taken from run 178582
    (*m_mapOfflineCell2RxId)[891035648] = 6104; // taken from run 178578
    (*m_mapOfflineCell2RxId)[891166720] = 6104; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976676608] = 6105; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976676352] = 6105; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976676096] = 6105; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976675840] = 6105; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976675584] = 6104; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976675328] = 6104; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976675072] = 6104; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976674816] = 6104; // taken from run 178577

    //Trigger Tower : 0x5180202 eta = -4.2625, phi = 0.9817
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890257408] = 6099; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890388480] = 6099; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890519552] = 6099; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890650624] = 6099; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890781696] = 6098; // taken from run 178582
    (*m_mapOfflineCell2RxId)[890912768] = 6098; // taken from run 178584
    (*m_mapOfflineCell2RxId)[891043840] = 6098; // taken from run 178577
    (*m_mapOfflineCell2RxId)[891174912] = 6098; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976672512] = 6099; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976672256] = 6099; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976672000] = 6099; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976671744] = 6099; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976671488] = 6098; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976671232] = 6098; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976670976] = 6098; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976670720] = 6098; // taken from run 178578

    //Trigger Tower : 0x5180302 eta = -4.2625, phi = 1.3744
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890265600] = 6097; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890396672] = 6097; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890527744] = 6097; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890658816] = 6097; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890789888] = 6096; // taken from run 178582
    (*m_mapOfflineCell2RxId)[890920960] = 6096; // taken from run 178584
    (*m_mapOfflineCell2RxId)[891052032] = 6096; // taken from run 178577
    (*m_mapOfflineCell2RxId)[891183104] = 6096; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976668416] = 6097; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976668160] = 6097; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976667904] = 6097; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976667648] = 6097; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976667392] = 6096; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976667136] = 6096; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976666880] = 6096; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976666624] = 6096; // taken from run 178578

    //Trigger Tower : 0x5180402 eta = -4.2625, phi = 1.7671
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890273792] = 6091; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890404864] = 6091; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890535936] = 6091; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890667008] = 6091; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890798080] = 6090; // taken from run 178584
    (*m_mapOfflineCell2RxId)[890929152] = 6090; // taken from run 178582
    (*m_mapOfflineCell2RxId)[891060224] = 6090; // taken from run 178578
    (*m_mapOfflineCell2RxId)[891191296] = 6090; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976664320] = 6091; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976664064] = 6091; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976663808] = 6091; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976663552] = 6091; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976663296] = 6090; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976663040] = 6090; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976662784] = 6090; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976662528] = 6090; // taken from run 178577

    //Trigger Tower : 0x5180502 eta = -4.2625, phi = 2.1598
    // This Trigger Tower has n cells = 7
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890281984] = 6089; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890413056] = 6089; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890544128] = 6089; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890675200] = 6089; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890806272] = 6088; // taken from run 178584
    (*m_mapOfflineCell2RxId)[890937344] = 6088; // taken from run 178582
    (*m_mapOfflineCell2RxId)[891068416] = 6088; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976660224] = 6089; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976659968] = 6089; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976659712] = 6089; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976659456] = 6089; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976659200] = 6088; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976658944] = 6088; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976658688] = 6088; // taken from run 178578

    //Trigger Tower : 0x5180602 eta = -4.2625, phi = 2.5525
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890290176] = 6083; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890421248] = 6083; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890552320] = 6083; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890683392] = 6083; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890814464] = 6082; // taken from run 178582
    (*m_mapOfflineCell2RxId)[890945536] = 6082; // taken from run 178584
    (*m_mapOfflineCell2RxId)[891076608] = 6082; // taken from run 178577
    (*m_mapOfflineCell2RxId)[891207680] = 6082; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976656128] = 6083; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976655872] = 6083; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976655616] = 6083; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976655360] = 6083; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976655104] = 6082; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976654848] = 6082; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976654592] = 6082; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976654336] = 6082; // taken from run 178578

    //Trigger Tower : 0x5180702 eta = -4.2625, phi = 2.9452
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890298368] = 6081; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890429440] = 6081; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890560512] = 6081; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890691584] = 6081; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890822656] = 6080; // taken from run 178582
    (*m_mapOfflineCell2RxId)[890953728] = 6080; // taken from run 178584
    (*m_mapOfflineCell2RxId)[891084800] = 6080; // taken from run 178577
    (*m_mapOfflineCell2RxId)[891215872] = 6080; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976652032] = 6081; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976651776] = 6081; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976651520] = 6081; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976651264] = 6081; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976651008] = 6080; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976650752] = 6080; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976650496] = 6080; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976650240] = 6080; // taken from run 178578

    //Trigger Tower : 0x5180802 eta = -4.2625, phi = 3.3379
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890306560] = 6139; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890437632] = 6139; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890568704] = 6139; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890699776] = 6139; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890830848] = 6138; // taken from run 178584
    (*m_mapOfflineCell2RxId)[890961920] = 6138; // taken from run 178582
    (*m_mapOfflineCell2RxId)[891092992] = 6138; // taken from run 178578
    (*m_mapOfflineCell2RxId)[891224064] = 6138; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976713472] = 6139; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976713216] = 6139; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976712960] = 6139; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976712704] = 6139; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976712448] = 6138; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976712192] = 6138; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976711936] = 6138; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976711680] = 6138; // taken from run 178577

    //Trigger Tower : 0x5180902 eta = -4.2625, phi = 3.7306
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890314752] = 6137; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890445824] = 6137; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890576896] = 6137; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890707968] = 6137; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890839040] = 6136; // taken from run 178584
    (*m_mapOfflineCell2RxId)[890970112] = 6136; // taken from run 178582
    (*m_mapOfflineCell2RxId)[891101184] = 6136; // taken from run 178578
    (*m_mapOfflineCell2RxId)[891232256] = 6136; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976709376] = 6137; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976709120] = 6137; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976708864] = 6137; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976708608] = 6137; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976708352] = 6136; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976708096] = 6136; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976707840] = 6136; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976707584] = 6136; // taken from run 178577

    //Trigger Tower : 0x5180a02 eta = -4.2625, phi = 4.1233
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890322944] = 6131; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890454016] = 6131; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890585088] = 6131; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890716160] = 6131; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890847232] = 6130; // taken from run 178582
    (*m_mapOfflineCell2RxId)[890978304] = 6130; // taken from run 178584
    (*m_mapOfflineCell2RxId)[891109376] = 6130; // taken from run 178577
    (*m_mapOfflineCell2RxId)[891240448] = 6130; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976705280] = 6131; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976705024] = 6131; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976704768] = 6131; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976704512] = 6131; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976704256] = 6130; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976704000] = 6130; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976703744] = 6130; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976703488] = 6130; // taken from run 178578

    //Trigger Tower : 0x5180b02 eta = -4.2625, phi = 4.5160
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890331136] = 6129; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890462208] = 6129; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890593280] = 6129; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890724352] = 6129; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890855424] = 6128; // taken from run 178582
    (*m_mapOfflineCell2RxId)[890986496] = 6128; // taken from run 178584
    (*m_mapOfflineCell2RxId)[891117568] = 6128; // taken from run 178577
    (*m_mapOfflineCell2RxId)[891248640] = 6128; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976701184] = 6129; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976700928] = 6129; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976700672] = 6129; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976700416] = 6129; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976700160] = 6128; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976699904] = 6128; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976699648] = 6128; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976699392] = 6128; // taken from run 178578

    //Trigger Tower : 0x5180c02 eta = -4.2625, phi = 4.9087
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890339328] = 6123; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890470400] = 6123; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890601472] = 6123; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890732544] = 6123; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890863616] = 6122; // taken from run 178584
    (*m_mapOfflineCell2RxId)[890994688] = 6122; // taken from run 178582
    (*m_mapOfflineCell2RxId)[891125760] = 6122; // taken from run 178578
    (*m_mapOfflineCell2RxId)[891256832] = 6122; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976697088] = 6123; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976696832] = 6123; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976696576] = 6123; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976696320] = 6123; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976696064] = 6122; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976695808] = 6122; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976695552] = 6122; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976695296] = 6122; // taken from run 178577

    //Trigger Tower : 0x5180d02 eta = -4.2625, phi = 5.3014
    // This Trigger Tower has n cells = 7
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890347520] = 6121; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890478592] = 6121; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890609664] = 6121; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890740736] = 6121; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890871808] = 6120; // taken from run 178584
    (*m_mapOfflineCell2RxId)[891133952] = 6120; // taken from run 178578

    (*m_mapOfflineCell2RxId)[891002880] = 6120; // Not connected to Calib Lines - this is a guess
      
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976692992] = 6121; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976692736] = 6121; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976692480] = 6121; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976692224] = 6121; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976691968] = 6120; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976691456] = 6120; // taken from run 178578

    (*m_mapOnlineCell2RxId)[976691712] = 6120; // Not connected to Calib Lines - this is a guess
    
    (*m_mapOfflineCellNotConnectedToCalibLines)[891002880] = 0;
    (*m_mapOnlineCellNotConnectedToCalibLines)[976691712] = 0;

    //Trigger Tower : 0x5180e02 eta = -4.2625, phi = 5.6941
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890355712] = 6115; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890486784] = 6115; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890617856] = 6115; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890748928] = 6115; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890880000] = 6114; // taken from run 178582
    (*m_mapOfflineCell2RxId)[891142144] = 6114; // taken from run 178577
    (*m_mapOfflineCell2RxId)[891273216] = 6114; // taken from run 178578

    (*m_mapOfflineCell2RxId)[891011072] = 6114; // Not connected to Calib Lines - this is a guess
    
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976688896] = 6115; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976688640] = 6115; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976688384] = 6115; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976688128] = 6115; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976687872] = 6114; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976687360] = 6114; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976687104] = 6114; // taken from run 178578

    (*m_mapOnlineCell2RxId)[976687616] = 6114; // Not connected to Calib Lines - this is a guess 
    
    (*m_mapOfflineCellNotConnectedToCalibLines)[891011072] = 0;
    (*m_mapOnlineCellNotConnectedToCalibLines)[976687616] = 0;

    //Trigger Tower : 0x5180f02 eta = -4.2625, phi = 6.0868
    // This Trigger Tower has n cells = 8
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[890363904] = 6113; // taken from run 178733
    (*m_mapOfflineCell2RxId)[890494976] = 6113; // taken from run 178734
    (*m_mapOfflineCell2RxId)[890626048] = 6113; // taken from run 178731
    (*m_mapOfflineCell2RxId)[890757120] = 6113; // taken from run 178732
    (*m_mapOfflineCell2RxId)[890888192] = 6112; // taken from run 178582
    (*m_mapOfflineCell2RxId)[891019264] = 6112; // taken from run 178584
    (*m_mapOfflineCell2RxId)[891150336] = 6112; // taken from run 178577
    (*m_mapOfflineCell2RxId)[891281408] = 6112; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976684800] = 6113; // taken from run 178733
    (*m_mapOnlineCell2RxId)[976684544] = 6113; // taken from run 178734
    (*m_mapOnlineCell2RxId)[976684288] = 6113; // taken from run 178731
    (*m_mapOnlineCell2RxId)[976684032] = 6113; // taken from run 178732
    (*m_mapOnlineCell2RxId)[976683776] = 6112; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976683520] = 6112; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976683264] = 6112; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976683008] = 6112; // taken from run 178578

    //Trigger Tower : 0x5180003 eta = -3.8375, phi = 0.1963
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880803840] = 6041; // taken from run 178742
    (*m_mapOfflineCell2RxId)[880934912] = 6041; // taken from run 178743
    (*m_mapOfflineCell2RxId)[881065984] = 6041; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881197056] = 6041; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881328128] = 6041; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881459200] = 6041; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881590272] = 6041; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881721344] = 6041; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881852416] = 6040; // taken from run 178716
    (*m_mapOfflineCell2RxId)[881983488] = 6040; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882114560] = 6040; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882245632] = 6040; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882376704] = 6040; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882507776] = 6040; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882638848] = 6040; // taken from run 178577
    (*m_mapOfflineCell2RxId)[882769920] = 6040; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976572160] = 6041; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976571904] = 6041; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976571648] = 6041; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976571392] = 6041; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976571136] = 6041; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976570880] = 6041; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976570624] = 6041; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976570368] = 6041; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976570112] = 6040; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976569856] = 6040; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976569600] = 6040; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976569344] = 6040; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976569088] = 6040; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976568832] = 6040; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976568576] = 6040; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976568320] = 6040; // taken from run 178578

    //Trigger Tower : 0x5180103 eta = -3.8375, phi = 0.5890
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880812032] = 6043; // taken from run 178743
    (*m_mapOfflineCell2RxId)[880943104] = 6043; // taken from run 178742
    (*m_mapOfflineCell2RxId)[881074176] = 6043; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881205248] = 6043; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881336320] = 6043; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881467392] = 6043; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881598464] = 6043; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881729536] = 6043; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881860608] = 6042; // taken from run 178717
    (*m_mapOfflineCell2RxId)[881991680] = 6042; // taken from run 178716
    (*m_mapOfflineCell2RxId)[882122752] = 6042; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882253824] = 6042; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882384896] = 6042; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882515968] = 6042; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882647040] = 6042; // taken from run 178578
    (*m_mapOfflineCell2RxId)[882778112] = 6042; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976580352] = 6043; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976580096] = 6043; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976579840] = 6043; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976579584] = 6043; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976579328] = 6043; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976579072] = 6043; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976578816] = 6043; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976578560] = 6043; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976578304] = 6042; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976578048] = 6042; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976577792] = 6042; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976577536] = 6042; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976577280] = 6042; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976577024] = 6042; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976576768] = 6042; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976576512] = 6042; // taken from run 178577

    //Trigger Tower : 0x5180203 eta = -3.8375, phi = 0.9817
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880820224] = 6033; // taken from run 178742
    (*m_mapOfflineCell2RxId)[880951296] = 6033; // taken from run 178743
    (*m_mapOfflineCell2RxId)[881082368] = 6033; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881213440] = 6033; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881344512] = 6033; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881475584] = 6033; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881606656] = 6033; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881737728] = 6033; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881868800] = 6032; // taken from run 178716
    (*m_mapOfflineCell2RxId)[881999872] = 6032; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882130944] = 6032; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882262016] = 6032; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882393088] = 6032; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882524160] = 6032; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882655232] = 6032; // taken from run 178577
    (*m_mapOfflineCell2RxId)[882786304] = 6032; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976555776] = 6033; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976555520] = 6033; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976555264] = 6033; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976555008] = 6033; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976554752] = 6033; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976554496] = 6033; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976554240] = 6033; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976553984] = 6033; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976553728] = 6032; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976553472] = 6032; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976553216] = 6032; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976552960] = 6032; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976552704] = 6032; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976552448] = 6032; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976552192] = 6032; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976551936] = 6032; // taken from run 178578

    //Trigger Tower : 0x5180303 eta = -3.8375, phi = 1.3744
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880828416] = 6035; // taken from run 178743
    (*m_mapOfflineCell2RxId)[880959488] = 6035; // taken from run 178742
    (*m_mapOfflineCell2RxId)[881090560] = 6035; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881221632] = 6035; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881352704] = 6035; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881483776] = 6035; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881614848] = 6035; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881745920] = 6035; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881876992] = 6034; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882008064] = 6034; // taken from run 178716
    (*m_mapOfflineCell2RxId)[882139136] = 6034; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882270208] = 6034; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882401280] = 6034; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882532352] = 6034; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882663424] = 6034; // taken from run 178578

    (*m_mapOfflineCell2RxId)[882794496] = 6034; // Not connected to Calib Lines - this is a guess
    
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976563968] = 6035; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976563712] = 6035; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976563456] = 6035; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976563200] = 6035; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976562944] = 6035; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976562688] = 6035; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976562432] = 6035; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976562176] = 6035; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976561920] = 6034; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976561664] = 6034; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976561408] = 6034; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976561152] = 6034; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976560896] = 6034; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976560640] = 6034; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976560384] = 6034; // taken from run 178578

    (*m_mapOnlineCell2RxId)[976560128] = 6034; // Not connected to Calib Lines - this is a guess
    
    (*m_mapOfflineCellNotConnectedToCalibLines)[882794496] = 0;
    (*m_mapOnlineCellNotConnectedToCalibLines)[976560128] = 0;

    //Trigger Tower : 0x5180403 eta = -3.8375, phi = 1.7671
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880836608] = 6025; // taken from run 178742
    (*m_mapOfflineCell2RxId)[880967680] = 6025; // taken from run 178743
    (*m_mapOfflineCell2RxId)[881098752] = 6025; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881229824] = 6025; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881360896] = 6025; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881491968] = 6025; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881623040] = 6025; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881754112] = 6025; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881885184] = 6024; // taken from run 178716
    (*m_mapOfflineCell2RxId)[882016256] = 6024; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882147328] = 6024; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882278400] = 6024; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882409472] = 6024; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882540544] = 6024; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882671616] = 6024; // taken from run 178577
    (*m_mapOfflineCell2RxId)[882802688] = 6024; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976539392] = 6025; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976539136] = 6025; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976538880] = 6025; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976538624] = 6025; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976538368] = 6025; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976538112] = 6025; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976537856] = 6025; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976537600] = 6025; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976537344] = 6024; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976537088] = 6024; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976536832] = 6024; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976536576] = 6024; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976536320] = 6024; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976536064] = 6024; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976535808] = 6024; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976535552] = 6024; // taken from run 178578

    //Trigger Tower : 0x5180503 eta = -3.8375, phi = 2.1598
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880844800] = 6027; // taken from run 178743
    (*m_mapOfflineCell2RxId)[880975872] = 6027; // taken from run 178742
    (*m_mapOfflineCell2RxId)[881106944] = 6027; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881238016] = 6027; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881369088] = 6027; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881500160] = 6027; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881631232] = 6027; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881762304] = 6027; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881893376] = 6026; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882024448] = 6026; // taken from run 178716
    (*m_mapOfflineCell2RxId)[882155520] = 6026; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882286592] = 6026; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882417664] = 6026; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882548736] = 6026; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882679808] = 6026; // taken from run 178578
    (*m_mapOfflineCell2RxId)[882810880] = 6026; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976547584] = 6027; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976547328] = 6027; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976547072] = 6027; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976546816] = 6027; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976546560] = 6027; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976546304] = 6027; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976546048] = 6027; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976545792] = 6027; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976545536] = 6026; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976545280] = 6026; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976545024] = 6026; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976544768] = 6026; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976544512] = 6026; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976544256] = 6026; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976544000] = 6026; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976543744] = 6026; // taken from run 178577

    //Trigger Tower : 0x5180603 eta = -3.8375, phi = 2.5525
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880852992] = 6017; // taken from run 178742
    (*m_mapOfflineCell2RxId)[880984064] = 6017; // taken from run 178743
    (*m_mapOfflineCell2RxId)[881115136] = 6017; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881246208] = 6017; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881377280] = 6017; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881508352] = 6017; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881639424] = 6017; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881770496] = 6017; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881901568] = 6016; // taken from run 178716
    (*m_mapOfflineCell2RxId)[882032640] = 6016; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882163712] = 6016; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882294784] = 6016; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882425856] = 6016; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882556928] = 6016; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882688000] = 6016; // taken from run 178577
    (*m_mapOfflineCell2RxId)[882819072] = 6016; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976523008] = 6017; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976522752] = 6017; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976522496] = 6017; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976522240] = 6017; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976521984] = 6017; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976521728] = 6017; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976521472] = 6017; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976521216] = 6017; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976520960] = 6016; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976520704] = 6016; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976520448] = 6016; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976520192] = 6016; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976519936] = 6016; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976519680] = 6016; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976519424] = 6016; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976519168] = 6016; // taken from run 178578

    //Trigger Tower : 0x5180703 eta = -3.8375, phi = 2.9452
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880861184] = 6019; // taken from run 178743
    (*m_mapOfflineCell2RxId)[880992256] = 6019; // taken from run 178742
    (*m_mapOfflineCell2RxId)[881123328] = 6019; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881254400] = 6019; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881385472] = 6019; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881516544] = 6019; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881647616] = 6019; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881778688] = 6019; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881909760] = 6018; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882040832] = 6018; // taken from run 178716
    (*m_mapOfflineCell2RxId)[882171904] = 6018; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882302976] = 6018; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882434048] = 6018; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882565120] = 6018; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882696192] = 6018; // taken from run 178578
    (*m_mapOfflineCell2RxId)[882827264] = 6018; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976531200] = 6019; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976530944] = 6019; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976530688] = 6019; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976530432] = 6019; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976530176] = 6019; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976529920] = 6019; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976529664] = 6019; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976529408] = 6019; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976529152] = 6018; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976528896] = 6018; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976528640] = 6018; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976528384] = 6018; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976528128] = 6018; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976527872] = 6018; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976527616] = 6018; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976527360] = 6018; // taken from run 178577

    //Trigger Tower : 0x5180803 eta = -3.8375, phi = 3.3379
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880869376] = 6073; // taken from run 178742
    (*m_mapOfflineCell2RxId)[881000448] = 6073; // taken from run 178743
    (*m_mapOfflineCell2RxId)[881131520] = 6073; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881262592] = 6073; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881393664] = 6073; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881524736] = 6073; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881655808] = 6073; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881786880] = 6073; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881917952] = 6072; // taken from run 178716
    (*m_mapOfflineCell2RxId)[882049024] = 6072; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882180096] = 6072; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882311168] = 6072; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882442240] = 6072; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882573312] = 6072; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882704384] = 6072; // taken from run 178577
    (*m_mapOfflineCell2RxId)[882835456] = 6072; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976637696] = 6073; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976637440] = 6073; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976637184] = 6073; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976636928] = 6073; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976636672] = 6073; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976636416] = 6073; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976636160] = 6073; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976635904] = 6073; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976635648] = 6072; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976635392] = 6072; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976635136] = 6072; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976634880] = 6072; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976634624] = 6072; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976634368] = 6072; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976634112] = 6072; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976633856] = 6072; // taken from run 178578

    //Trigger Tower : 0x5180903 eta = -3.8375, phi = 3.7306
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880877568] = 6075; // taken from run 178743
    (*m_mapOfflineCell2RxId)[881008640] = 6075; // taken from run 178742
    (*m_mapOfflineCell2RxId)[881139712] = 6075; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881270784] = 6075; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881401856] = 6075; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881532928] = 6075; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881664000] = 6075; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881795072] = 6075; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881926144] = 6074; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882057216] = 6074; // taken from run 178716
    (*m_mapOfflineCell2RxId)[882188288] = 6074; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882319360] = 6074; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882450432] = 6074; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882581504] = 6074; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882712576] = 6074; // taken from run 178578
    (*m_mapOfflineCell2RxId)[882843648] = 6074; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976645888] = 6075; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976645632] = 6075; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976645376] = 6075; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976645120] = 6075; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976644864] = 6075; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976644608] = 6075; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976644352] = 6075; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976644096] = 6075; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976643840] = 6074; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976643584] = 6074; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976643328] = 6074; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976643072] = 6074; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976642816] = 6074; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976642560] = 6074; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976642304] = 6074; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976642048] = 6074; // taken from run 178577

    //Trigger Tower : 0x5180a03 eta = -3.8375, phi = 4.1233
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880885760] = 6065; // taken from run 178742
    (*m_mapOfflineCell2RxId)[881016832] = 6065; // taken from run 178743
    (*m_mapOfflineCell2RxId)[881147904] = 6065; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881278976] = 6065; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881410048] = 6065; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881541120] = 6065; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881672192] = 6065; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881803264] = 6065; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881934336] = 6064; // taken from run 178716
    (*m_mapOfflineCell2RxId)[882065408] = 6064; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882196480] = 6064; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882327552] = 6064; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882458624] = 6064; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882589696] = 6064; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882720768] = 6064; // taken from run 178577
    (*m_mapOfflineCell2RxId)[882851840] = 6064; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976621312] = 6065; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976621056] = 6065; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976620800] = 6065; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976620544] = 6065; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976620288] = 6065; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976620032] = 6065; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976619776] = 6065; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976619520] = 6065; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976619264] = 6064; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976619008] = 6064; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976618752] = 6064; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976618496] = 6064; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976618240] = 6064; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976617984] = 6064; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976617728] = 6064; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976617472] = 6064; // taken from run 178578

    //Trigger Tower : 0x5180b03 eta = -3.8375, phi = 4.5160
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880893952] = 6067; // taken from run 178743
    (*m_mapOfflineCell2RxId)[881025024] = 6067; // taken from run 178742
    (*m_mapOfflineCell2RxId)[881156096] = 6067; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881287168] = 6067; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881418240] = 6067; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881549312] = 6067; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881680384] = 6067; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881811456] = 6067; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881942528] = 6066; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882073600] = 6066; // taken from run 178716
    (*m_mapOfflineCell2RxId)[882204672] = 6066; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882335744] = 6066; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882466816] = 6066; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882597888] = 6066; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882728960] = 6066; // taken from run 178578
    (*m_mapOfflineCell2RxId)[882860032] = 6066; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976629504] = 6067; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976629248] = 6067; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976628992] = 6067; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976628736] = 6067; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976628480] = 6067; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976628224] = 6067; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976627968] = 6067; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976627712] = 6067; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976627456] = 6066; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976627200] = 6066; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976626944] = 6066; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976626688] = 6066; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976626432] = 6066; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976626176] = 6066; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976625920] = 6066; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976625664] = 6066; // taken from run 178577

    //Trigger Tower : 0x5180c03 eta = -3.8375, phi = 4.9087
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880902144] = 6057; // taken from run 178742
    (*m_mapOfflineCell2RxId)[881033216] = 6057; // taken from run 178743
    (*m_mapOfflineCell2RxId)[881164288] = 6057; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881295360] = 6057; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881426432] = 6057; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881557504] = 6057; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881688576] = 6057; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881819648] = 6057; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881950720] = 6056; // taken from run 178716
    (*m_mapOfflineCell2RxId)[882081792] = 6056; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882212864] = 6056; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882343936] = 6056; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882475008] = 6056; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882606080] = 6056; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882737152] = 6056; // taken from run 178577
    (*m_mapOfflineCell2RxId)[882868224] = 6056; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976604928] = 6057; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976604672] = 6057; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976604416] = 6057; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976604160] = 6057; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976603904] = 6057; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976603648] = 6057; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976603392] = 6057; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976603136] = 6057; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976602880] = 6056; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976602624] = 6056; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976602368] = 6056; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976602112] = 6056; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976601856] = 6056; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976601600] = 6056; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976601344] = 6056; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976601088] = 6056; // taken from run 178578

    //Trigger Tower : 0x5180d03 eta = -3.8375, phi = 5.3014
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880910336] = 6059; // taken from run 178743
    (*m_mapOfflineCell2RxId)[881041408] = 6059; // taken from run 178742
    (*m_mapOfflineCell2RxId)[881172480] = 6059; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881303552] = 6059; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881434624] = 6059; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881565696] = 6059; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881696768] = 6059; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881827840] = 6059; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881958912] = 6058; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882089984] = 6058; // taken from run 178716
    (*m_mapOfflineCell2RxId)[882221056] = 6058; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882352128] = 6058; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882483200] = 6058; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882614272] = 6058; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882745344] = 6058; // taken from run 178578
    (*m_mapOfflineCell2RxId)[882876416] = 6058; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976613120] = 6059; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976612864] = 6059; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976612608] = 6059; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976612352] = 6059; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976612096] = 6059; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976611840] = 6059; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976611584] = 6059; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976611328] = 6059; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976611072] = 6058; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976610816] = 6058; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976610560] = 6058; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976610304] = 6058; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976610048] = 6058; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976609792] = 6058; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976609536] = 6058; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976609280] = 6058; // taken from run 178577

    //Trigger Tower : 0x5180e03 eta = -3.8375, phi = 5.6941
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880918528] = 6049; // taken from run 178742
    (*m_mapOfflineCell2RxId)[881049600] = 6049; // taken from run 178743
    (*m_mapOfflineCell2RxId)[881180672] = 6049; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881311744] = 6049; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881442816] = 6049; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881573888] = 6049; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881704960] = 6049; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881836032] = 6049; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881967104] = 6048; // taken from run 178716
    (*m_mapOfflineCell2RxId)[882098176] = 6048; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882229248] = 6048; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882360320] = 6048; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882491392] = 6048; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882622464] = 6048; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882753536] = 6048; // taken from run 178577
    (*m_mapOfflineCell2RxId)[882884608] = 6048; // taken from run 178578
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976588544] = 6049; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976588288] = 6049; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976588032] = 6049; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976587776] = 6049; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976587520] = 6049; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976587264] = 6049; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976587008] = 6049; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976586752] = 6049; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976586496] = 6048; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976586240] = 6048; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976585984] = 6048; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976585728] = 6048; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976585472] = 6048; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976585216] = 6048; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976584960] = 6048; // taken from run 178577
    (*m_mapOnlineCell2RxId)[976584704] = 6048; // taken from run 178578

    //Trigger Tower : 0x5180f03 eta = -3.8375, phi = 6.0868
    // This Trigger Tower has n cells = 16
    // Offline Cell ID -> Rx
    (*m_mapOfflineCell2RxId)[880926720] = 6051; // taken from run 178743
    (*m_mapOfflineCell2RxId)[881057792] = 6051; // taken from run 178742
    (*m_mapOfflineCell2RxId)[881188864] = 6051; // taken from run 178741
    (*m_mapOfflineCell2RxId)[881319936] = 6051; // taken from run 178739
    (*m_mapOfflineCell2RxId)[881451008] = 6051; // taken from run 178738
    (*m_mapOfflineCell2RxId)[881582080] = 6051; // taken from run 178737
    (*m_mapOfflineCell2RxId)[881713152] = 6051; // taken from run 178736
    (*m_mapOfflineCell2RxId)[881844224] = 6051; // taken from run 178735
    (*m_mapOfflineCell2RxId)[881975296] = 6050; // taken from run 178717
    (*m_mapOfflineCell2RxId)[882106368] = 6050; // taken from run 178716
    (*m_mapOfflineCell2RxId)[882237440] = 6050; // taken from run 178588
    (*m_mapOfflineCell2RxId)[882368512] = 6050; // taken from run 178586
    (*m_mapOfflineCell2RxId)[882499584] = 6050; // taken from run 178584
    (*m_mapOfflineCell2RxId)[882630656] = 6050; // taken from run 178582
    (*m_mapOfflineCell2RxId)[882761728] = 6050; // taken from run 178578
    (*m_mapOfflineCell2RxId)[882892800] = 6050; // taken from run 178577
    // Online Cell ID - > Rx
    (*m_mapOnlineCell2RxId)[976596736] = 6051; // taken from run 178743
    (*m_mapOnlineCell2RxId)[976596480] = 6051; // taken from run 178742
    (*m_mapOnlineCell2RxId)[976596224] = 6051; // taken from run 178741
    (*m_mapOnlineCell2RxId)[976595968] = 6051; // taken from run 178739
    (*m_mapOnlineCell2RxId)[976595712] = 6051; // taken from run 178738
    (*m_mapOnlineCell2RxId)[976595456] = 6051; // taken from run 178737
    (*m_mapOnlineCell2RxId)[976595200] = 6051; // taken from run 178736
    (*m_mapOnlineCell2RxId)[976594944] = 6051; // taken from run 178735
    (*m_mapOnlineCell2RxId)[976594688] = 6050; // taken from run 178717
    (*m_mapOnlineCell2RxId)[976594432] = 6050; // taken from run 178716
    (*m_mapOnlineCell2RxId)[976594176] = 6050; // taken from run 178588
    (*m_mapOnlineCell2RxId)[976593920] = 6050; // taken from run 178586
    (*m_mapOnlineCell2RxId)[976593664] = 6050; // taken from run 178584
    (*m_mapOnlineCell2RxId)[976593408] = 6050; // taken from run 178582
    (*m_mapOnlineCell2RxId)[976593152] = 6050; // taken from run 178578
    (*m_mapOnlineCell2RxId)[976592896] = 6050; // taken from run 178577
   
    return StatusCode::SUCCESS;
  }

} // end of namespace




