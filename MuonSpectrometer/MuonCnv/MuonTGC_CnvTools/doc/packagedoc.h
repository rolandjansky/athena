/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuonTGC_CnvTools_page MuonTGC_CnvTools Package

MuonTGC_CnvTools package contains classes for conversions among BS, RDO, PRD etc. of TGC. 

@author Toshi Sumida <Toshi.Sumida@cern.ch>
@author Susumu Oda <Susumu.Oda@cern.ch>
@author Zvi Tarem 
@author Hisaya Kurashige 
@author Edward Moyse 

@section MuonTGC_CnvTools_MuonTGC_CnvToolsIntro Introduction
MuonTGC_CnvTools package contains classes for conversions among BS, ROD (ROB fragment in BS), RDO, PRD and Digit of TGC. 

- Conversion from BS to RDO
 - TGC_RawDataProviderTool selects ROB fragment from BS
 - ITGC_RodDecoder is the interface to convert from ROB fragment to RDO. 
  - TGC_RodDecoderRawdata converts from ROB fragment written in rawdata format to RDO using TgcRODReadOut, TgcSlbDataHelper, TgcSlbData and TGC_BYTESTREAM_XXX in TgcByteStreamData.h. 
  - TGC_RodDecoderReadout converts from ROB fragment written in readout format to RDO using TGC_BYTESTREAM_XXX. 
- Conversion from RDO to BS 
 - TgcROD_Encoder converts from RDO to ROB fragment written in readout format using TgcByteStream. 
 - TgcRdoContByteStreamTool writes out ROB fragment written in readout format as BS.  
- Conversion from RDO to PRD 
 - TgcRdoToPrepDataTool converts from RDO to PRD. 
- Conversion from RDO to Digit 
 - TgcRDO_Decoder converts from RDO to Digit which can be compared with simulated Digit. 
- TGC_Hid2RESrcID is used in TGC_RawDataProviderTool and TgcRdoContByteStreamTool. 

In this file, the following things are explained:   
- Identifier
- RDO 
- PRD
- Data Format
- Requirements

Many information can be accessed from 
- https://twiki.cern.ch/twiki/bin/view/Main/TgcDocument 

Doxygen documentation of MuonTGC_CnvTools in dev nightly in html can be accessed at 
- http://atlas-computing.web.cern.ch/atlas-computing/links/nightlyDevDirectory/AtlasOffline/latest_doxygen/InstallArea/doc/MuonTGC_CnvTools/html/ 

@section MuonTGC_CnvTools_MuonTGC_CnvToolsID Identifier
TGC has four kinds of identifiers, Offline ID, Online ID, Readout ID and Trigger ID.  

@subsection MuonTGC_CnvToolsOfflineID Offline ID
Offline ID is an identifier for each wire/strip channel. 
Offline ID is a basic ID in Prep Raw Data (PRD) and is used by many tools (tracking/DQA and so on). 
TgcIdHelper provides an interface to the fields of Offline ID. 

@subsection MuonTGC_CnvToolsOnlineID Online ID
Online ID is an identifier for each wire/strip channel. 
Online ID consists of subsystemNumber, octantNumber, moduleNumber, layerNumber, rNumber, wireOfStrip and channelNumber.  
Online ID is a TGC private ID and is used for mechanical numbering (cable label etc.). 
There is 1 to 1 correspondence between Online ID and Offline ID. 
Specification
- subsystemNumber : +1 for A side (z>0) and -1 for C side (z<0)
- octantNumber : 0-7 increases with phi
- moduleNumber : 0-8 (TGC1, TGC2 and TGC3), 9-14 (TGC EIFI)
- layerNumber : 0-6 increases with |z| (there is no 1 for strip) 
- rNumber : 0-n increases with |eta|
- wireOfStrip : 0 for wire and 1 for strip
- channelNumber : 0-n increases with phi for strip and with |eta| for wire. 

@subsection MuonTGC_CnvToolsReadoutID Readout ID
Readout ID is an identifier for each channel of readout electronics. 
Readout ID consists of subDetectorID, rodID, sswID, sbLoc and channelID. 
Readout ID is used in hit Raw Data Object (RDO) and simulation (TrigT1TGC package). 
This TGC Readout ID corresponds to Online ID of other muon technologies.
There is N to N correspondence between Online ID and Readout ID. 
Specification 
- subDetectorID : 0x67 for A side (z>0) and 0x68 for C side (z<0) 
- rodID : 1-12 for readout sectors 1-12
- sswID : 
 - 0, 1       for endcap triplet
 - 2          for forward triplet
 - 3, 4, 5, 6 for endcap doublet
 - 7          for forward doublet
 - 8          for inner station (One SSW covers 1/4 wheel)
 - 9          for SLB on SL 
- sbLoc : 0-n, unique identifier for each SLB in a SSW.
 - Details in https://twiki.cern.ch/twiki/pub/Main/TgcDocument/SLB_LocationID.xls
- channelID : 0-199, channel in an SLB (40-199 is hitmap)

@subsection MuonTGC_CnvToolsTriggerID Trigger ID 
Trigger ID is an identifier for individual trigger electronics component (Sector, LowPt (Tracklet), HighPt and SectorLogic). 
Trigger ID is used in trigger RDO and simulation (TrigT1TGC package). 

@section MuonTGC_CnvTools_MuonTGC_CnvToolsRDO RDO
RDO (Raw Data Object) is written by ReadoutID. 
It is well described Tables, 7, 9, 11 and 12 in http://cern.ch/Atlas-TGC/doc/ROBformat.pdf .

- TgcRawData is the class for individual RDOs.
- TgcRdo is the container class of TgcRawData's. 
- TgcRdoContiner is the container class of TgcRdo's.  

@subsection MuonTGC_CnvToolsRDOHit Hit RDO Variables
- bcTag : int, 0:Previous Bunch Crossing (BC), 1:Current BC, 2:Next BC, 3: Unknown BC
- subDetectorId : int, 103:A side, 104:C side 
- rodId : int, 1-12, address of ROD and it corresponds to the 1/12 sector number
- sswId : int, 0-9, address of SSW, 
 - 0, 1       for endcap triplet,
 - 2          for forward triplet,
 - 3, 4, 5, 6 for endcap doublet,
 - 7          for forward doublet,
 - 8          for inner station (One SSW covers 1/4 wheel),
 - 9          for SLB on SL.
- slbId : int, 0-25, sbLoc ID which is translated from ID on individual SLB chips. 
- l1Id : int, Level1 ID from readout electronics
- bcId : int, Bunch Crossing ID from readout electronics
- slbType : enum, 0:Doublet Wire, 1:Doublet Strip, 2:Triplet Wire, 3:Triplet Strip, 4:Inner Wire, 5:Inner Strip, 6:Unknown
- adj : bool, hit in adjacent region
- channel : int, bitposition of SLB, 40-75:A-input, 76-111:B-input, 112-155:C-input, 156-199:D-input 

@subsection MuonTGC_CnvToolsRDOTracklet Tracklet RDO Variables
- bcTag : int, 0-3, the same with Hit RDO
- subDetectorId : int, 103-104, the same with Hit RDO
- rodId : int, 1-12, the same with Hit RDO
- sswId : int, 0-9, the same with Hit RDO
- slbId : int, 0-25, the same with Hit RDO
- l1Id : int, the same with Hit RDO
- bcId : int, the same with Hit RDO
- slbType : enum, 0-6, the same with Hit RDO
- delta : int, -7 to +7 for wire and -3 to +3 for strip, saggita
- seg : int, 0 (Triplet Strip TRIGA, Inner Wire, Doublet) or 1 (Triplet Strip TRIGB, Inner Strip), 
- sub : int, 0-3, for identification of TRIG1, TRIG2 etc. 
- rphi : int, 0-31, position information with granularity of 1/2 for Doublet and 1/3 for Triplet.    

@subsection MuonTGC_CnvToolsRDOHipT HipT RDO Variables
- bcTag : int, 0-3, the same with Hit RDO
- subDetectorId : int, 103-104, the same with Hit RDO
- rodId : int, 1-12, the same with Hit RDO
- sswId : int, 0-9, the same with Hit RDO
- slbId : int, 0-25, the same with Hit RDO
- l1Id : int, the same with Hit RDO
- bcId : int, the same with Hit RDO
- strip : bool, true:Strip, false:Wire 
- forward : bool, true:Forward, false:Endcap
- sector : int, 0 and 1 for Forward and 0-3 for Endcap
- chip : int, 0-3, ID for chip on HipT board
- index : int, 0-1, ID for candidates
- hipt : bool, true:HipT, false:LowpT
- hitId : int, 1-6, ID for location with a granularity of 16 channels
        : int, 0-7, ID for EI/FI hit information read-out in SL
- sub : int, 0-1, Divide hitId into two parts 
- delta : int, -15 to +15 for Wire and -7 and +7 for Strip, saggita 
- tile : int, 8 bits for TileMuon trigger information

@subsection MuonTGC_CnvToolsRDOSL SL RDO Variables
- bcTag : int, 0-3, the same with Hit RDO
- subDetectorId : int, 103-104, the same with Hit RDO
- rodId : int, 1-12, the same with Hit RDO
- l1Id : int, the same with Hit RDO
- bcId : int, the same with Hit RDO
- cand3plus : bool, more than two candidates
- forward : bool, the same with HipT RDO
- sector : int, 0-3, the same with HipT RDO
- index : int, 0-1, ID for candidates
- muplus : int, 0:deltaR<0, 1:deltaR>=0 
- threshold : int, 1-6, Trigger menu label
- overlap : int
- roi : int, 0-147, ROI (Region of Interest) number
- veto : bool, 0-> success of Inner-Coincidence, 1->vetoed
 - https://twiki.cern.ch/twiki/pub/Main/TgcDocument/RoI_Numbering-080815.pdf

@section MuonTGC_CnvTools_MuonTGC_CnvToolsPRD PRD
Variables of Prep Raw Data (PRD) are as follows:
@subsection MuonTGC_CnvToolsPRDHit Hit PRD Variables 
Muon::TgcPrepData class is used for Hit PRD. 
- Identifier RDOId : The OfflineID of the channel
- IdentifierHash idDE  : The IdenifierHash of the collection used to store this object (i.e. of Muon::TgcPrepDataCollection)
- Trk::LocalPosition locpos : The local coordinates of the measurement [mm]
- std::vector<Identifier> rdoList : Vector of all the Identifiers (TGC hit has only one Identifier in this vector)
- Trk::ErrorMatrix locErrMat : The error of the measurement, the channel width divided by sqrt(12) [mm]
- MuonGM::TgcReadoutElement detEl : The pointer to the Detector Element (i.e. chamber) on which this measurement was made

@subsection MuonTGC_CnvToolsPRDTracklet Tracklet PRD Variables
Muon::TgcCoinData class is used for Tracklet PRD. 
- Identifier ChannelIdIn : OfflineID of the TGC2 Hit which creates this Tracklet
- Identifier ChannelIdOut : OfflineID of the TGC3 Hit which creates this Tracklet
- IdentifierHash collectionIdHash : he IdenifierHash of the collection used to store this object (i.e. of Muon::TgcCoinDataCollection)
- MuonGM::TgcReadoutElement detElIn : The pointer to the Detector Element of TGC2 on which this Tracklet was made
- MuonGM::TgcReadoutElement detElOut : The pointer to the Detector Element of TGC3 on which this Tracklet was made
- TgcCoinData::CoinDataType type : 0 (enum, 0:Tracklet, 1:HipT, 2:SL, 3:Unknown)
- bool isAside : true : Side A, false : Side C
- int phi : Trigger sector (1-24 for Forward, 1-48 for Endcap)
- bool isForward : true : Forward, false : Endcap
- bool isStrip : true : Tracklet Strip, false : Tracklet Wire
- int trackletId : Identifier of Tracklet
- Trk::LocalPosition posIn : The local coordinates on TGC2 [mm]
- Trk::LocalPosition posOut : The local coordinates on TGC3 [mm]
- double widthIn : The channel size of the TGC2 hit [mm]
- double widthOut : The channle size of the TGC3 hit [mm]
- int delta : The sagitta (unit is Trigger Channel)
- int sub : subMatrix (sub) of Tracklet RDO

@subsection MuonTGC_CnvToolsPRDHipT HipT PRD Variables
Muon::TgcCoinData class is used for Hipt PRD. 
- Identifier ChannelIdIn : OfflineID of the TGC1 Hit which creates this HipT
- Identifier ChannelIdOut : OfflineID of the TGC3 Hit which creates this HipT
- IdentifierHash collectionIdHash : The IdenifierHash of the collection used to store this object (i.e. of Muon::TgcCoinDataCollection)
- MuonGM::TgcReadoutElement detElIn : The pointer to the Detector Element of TGC1 on which this HipT was made
- MuonGM::TgcReadoutElement detElOut : The pointer to the Detector Element of TGC3 on which this HipT was made
- TgcCoinData::CoinDataType type : 1 (enum, 0:Tracklet, 1:HipT, 2:SL, 3:Unknown)
- bool isAside : true : Side A, false : Side C
- int phi : Trigger sector (1-24 for Forward, 1-48 for Endcap)
- bool isForward : true : Forward, false : Endcap
- bool isStrip : true : HipT Strip, false : HipT Wire
- int trackletId : Identifier of Tracklet which creates this HipT
- Trk::LocalPosition posIn : The local coordinates on TGC1 [mm]
- Trk::LocalPosition posOut : The local coordinates on TGC3 [mm]
- double widthIn : The channel size of ROI at TGC1 [mm]
- double widthOut : The channle size of ROI at TGC3 [mm]
- int delta : The sagitta (unit is Trigger Channel)
- int sub : hsub (sub) of HipT RDO

@subsection MuonTGC_CnvToolsPRDSL SL PRD Variables
Muon::TgcCoinData class is used for SL PRD. 
- Identifier channelIdOut : OfflineID of the TGC3 Wire Hit which creates this SL
- IdentifierHash collectionIdHash : The IdenifierHash of the collection used to store this object (i.e. of Muon::TgcCoinDataCollection)
- MuonGM::TgcReadoutElement* detElOut : The pointer to the Detector Element of TGC3 on which this SL was made
- TgcCoinData::CoinDataType type : 2 (enum, 0:Tracklet, 1:HipT, 2:SL, 3:Unknown)
- bool isAside : true : Side A, false : Side C
- int phi : Trigger sector (1-24 for Forward, 1-48 for Endcap)
- bool isForward : true : Forward, false : Endcap
- int trackletId : Identifier of Tracklet Wire which creates this SL
- int trackletIdStrip : Identifier of Tracklet Strip which creates this SL
- Trk::LocalPosition posOut : The local coordinates on TGC3 [mm]
- Trk::ErrorMatrix errMat : The diagonal matrix of ROI widths at TGC3 [mm]. (0, 0) component is the width in the r direction and (1, 1) components is the width in the phi direction
- int roi : The ROI number (0-63 for Forward, 0-147 for Endcap)
- int pt : The pT threshold (1-6)
- bool isPositiveDeltaR : The sign of delta of HipT Wire (isMuplus)

@section MuonTGC_CnvTools_MuonTGC_CnvToolsDataFormat Data Format
The formats in Byte Stream is written in http://cern.ch/Atlas-TGC/doc/ROBformat.pdf and http://atlas-proj-tgc.web.cern.ch/atlas-proj-tgc/doc/FElnkprotocol.pdf .
- TGC Rawdata format is the data sent from Star Switch (SSW). 
- TGC Readout format is written in Readout ID by Readout Driver (ROD). 
- TGC Chamber format is written in Online ID by ROD. 

Rawdata format data is decoded by TGC_RodDecoderRawdata and readout format data is decoded by TGC_RodDecoderReadout. 
Switching of the format to be decoded is currently (February 2009) done in 
MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonReadBSConfig.py
based on muonRecFlags.doTGC_rawdataROD flag defined in MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonRecFlags.py.

TGC has 24 (12 sectors x 2 sides) Readout Buffers (ROBs). 



*/
