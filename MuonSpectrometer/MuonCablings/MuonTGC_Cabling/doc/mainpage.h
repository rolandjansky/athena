/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
@mainpage MuonTGC_Cabling package

@author Susumu Oda <Susumu.Oda@cen.ch>
@author Hisaya Kurashige


@section MuonTGC_CablingAbout About this document
This document is created from MuonSpectrometer/MuonCablings/TGCcabling12/doc/readme. 
The file was originally written by Yousuke Kataoka and was updated by Hisaya Kurashige on August, 2007.
This informaion can be obsolete as of June, 2009. 
This document is being updated.


@section MuonTGC_CablingSvc MuonTGC_CablingSvc
MuonTGC_CablingSvc manages all cablings in TGC electronics system.

@subsection MuonTGC_CablingElectronicsSystem TGC electronics system
\code
ASD -- PP -- SLB -- HPB -- SL   (-- MuonCTPI) trigger part
               |		
                 -- SSW -- ROD  (-- ROS)      readout part
\endcode
Methods are 
- getChannel 
- getModule

@subsection MuonTGC_CablingIDs IDs
This service also provides transformation of 3 IDs.
- OfflineID : offline Muon Identifier
- OnlineID : private numbering scheme of offline IDs
- ReadoutID : IDs shown in RDO

Methods are
- getOfflineIDfromReadoutID
- getReadoutIDfromOfflineID
- getOfflineIDfromOnlineID
- getOnlineIDfromOfflineID
- getOnlineIDfromReadoutID
- getReadoutIDfromOnlineID


@section MuonTGC_CablingGetChannel getChannel method
This method provides connections of channels shown below.

@subsection MuonTGC_CablingChannels Channels
- ASDIn : Detector channel in Muon ID Identifier numbering scheme 
 - This is TGC chamber channel. 
- ASDOut : Detector channel in private online numbering scheme
 - This is chnanel in terms of "sector". 
- PPIn : Input channel for front-end module PatchPanel
 - This is front channel of TGC electronics. 
- PPOut : Output channel for front-end module PatchPanel
 - This is channel after Oring, adjacent scheme. 
- SLBIn : Input channel for coincidence matrix SlaveBoard (2-station coincidence for doublets and coincidence for triplets)
 - This is also used for hitmap channel in RDO. 
- SLBOut : Output channel for coincidence matrix SlaveBoard
 - This is also used for coincidence channel in RDO. 
- HPBIn : Input channel for coincidence matrix High Pt Board (3-station coincidence)
 - This is also used for coincidence channel in RDO. 

@subsection MuonTGC_CablingCahnnelConections Connections
- ASDIn <--> ASDOut : Nothing but numbering scheme transformation. 
- ASDOut<--> PPIn : Detector to electronics mapping. 
- PPIn  <--> PPOut : 1to1, 1to2, 2to1 for Oring or adjacent.  
- PPOut <--> SLBIn : Connection to second module SLB. 
- SLBIn <--> SLBOut : Double or triple for channel stagging. 
- SLBOut <--> HPBIn : Mapping between SLB chip and HPB block. 


@section MuonTGC_CablingGetModule getModule method
This method provide connections of modules shown below.

@subsection MuonTGC_CablingModules Modules
- PP  (PatchPanel) : BCID, Oring 
 - Many modules in a sector
- SLB (SlaveBoard) : Coincidence matrix for "LowPt"
 - Many modules in a sector. 6 types (WD, WT, SD, ST, WI, SI). 
- HPB (HighPtBoard) : Coincedence matrix for "HighPt"
 - Some modules in a sector. 2 types (Wire, Strip). 
- SL  (SectorLogic) : Pt decision
 - One in each sector. 
- SSW (StarSwitch) : Readout module in front-end
 - 10 modules in each readout sector (1/12 of wheel). 
- ROD (ReadOutDriver) : Readout module in back-end
 - Only one in each readout sector. 

@subsection MuonTGC_CablingModuleConnections Connections
- PP  <--> SLB : 1 to 2, consistent to PPOut <-> SLBIn.
- SLB <--> HPB : Some to 1, in the same signalType and sector. 
- HPB <--> SL  : Some to 1, in the same sector. 
- SLB <--> SSW : Some to 1, in the same readout sector (1/12 of wheel). 
- SSW <--> ROD : Some (10) to 1, in the same readout sector. 


@section MuonTGC_CablingGetOfflineIDfromReadoutID getOfflineIDfromReadoutID method
This method provide connections of IDs shown below.

@subsection MuonTGC_CablingGetOfflineIDfromReadoutIDIDs IDs
- Offline IDs : Offline Muon Identifier. ASDIn has OfflineID. 
 - See MuonSpectrometer/MuonIdHelpers/TgcIdHelper.h or ATL-MUON-2002-019. 
 - stationName : Tx(E/F) 
  - x : station number 1:M1 2:M2 3:M3 4:Inner
  - E/F: Endcap / Forward
 - stationEta : Chmaber numbering in eta (increases with R!!!)
  - plus sign : A-side  minus sign :C-side
  - Ex: M1 4: T8 3:T7 2:T6 1:T3 
  - Ex: M3 5: T9 4:T8 3:T7 2:T6 1:T5 
 - stationPhi : Chamber numbering in phi from x-axis 
  - Endcap          1..48
  - Forward         1..24
  - Endcap Inner    1..21
  - Endcap Forward  1..24
 - channel 
  - Wire  : 1 .. n increases with R	
  - Strip : 1 ..32 increases with Phi
- Online IDs : Private numbering scheme of offline IDs. ASDOut has OnlineID. 
 - subsystemNumber : 1 for Aside(z>0), -1 for Cside(z<0)
 - octantNumber : 0..7 increases with phi
 - moduleNumber
\code
   <---- phi ----      
    7 6 4 3 1 0        11 10  9
     8   5   2         14 13 12
    [M1,M2, M3]         [EI/FI] 
\endcode
 - layerNumber 
  - 0..6 increases with |Z| (strip triplet 0 or 2)
  - 7,8 for EI/FI 
 - rNumber : 0..n increases with eta in a sector
  - Ex: M1 1: T8 2:T7 3:T6 4:T3 
  - Ex: M3 0: T9 1:T8 2:T7 3:T6 4:T5 
 - wireOfStrip : 0 for wire, 1 for strip
 - channelNumber : 0..n, increases with ASD channel number in a sector
  - Wire: inreases with eta 
  - Strip: increases with phi (A-side Forward chamber and C-side Backward Chamber)
  - Strip: decreases with phi (A-side Backward chamber and C-side Forward Chamber) 
- Readout IDs : IDs shown in RDO
 - subDetectorID : 0x67 for Aside(z>0), 0x68 for Cside(z<0)
 - rodID : 1 .. 12 for readout sector 0..11 
 - sswID           
   - In a readout sector (1/12 wheel) 
    - 0,1         for endcap triplet
    - 2           for forward triplet
    - 3,4,5,6     for endcap doublet
    - 7           for forward doublet
   - In the whole wheel         
    - 8           for Inner station (each covers 1/4 wheel)
    - 9           for SLB on SL (each covers 1/12 wheel)
 - sbLoc : 0..n, unique identifer for each SLB in a SSW 
  - See details in 'PSPack' sheet in http://atlas.web.cern.ch/Atlas/project/TGC/www/design/tgc.xls
 - channelID : 0..199 channel of SLB (40..199 is hitmap, 0..39 is used for coincidence info)
- Other IDs 
 - slbID : 0..n   
  - First wire, then strip. For each, larger phi sector is back in a sector, id increases with eta (used in TrigT1TGC not appeared in BS and RDO). 
 - slbAddress : 0..31  
  - slb address which is set by DIP SW on SB can be obtained from Readout ID by using getSLBAddressfromReadoutID method. 
 - rxID : 0..22 
  - Input connector ID of SSW can be used instead of sbLoc by using getSLBIDfromRxID. 
 - sector 
  - Endcap : 0 .. 47
  - Forward : 0 .. 23
  - Inner : 0 .. 23
  - Increases with phi 
  - #0 sector is in Octant #0
\code
mapping between sector and station Phi
 Endcap
  sector   0   1   2   3   4   ... 
  Phi     47  48   1   2   3   ...
 Forward 
  sector   0   1   2   3   4   ... 
  Phi     24   1   2   3   4   ...
 Endcap Inner
  sector   0  1  2  3  4  5  6  7  8  9 10 11
  Phi     21  1  2  3  4  5  6  7  8  -  9 10
  sector  12 13 14 15 16 17 18 19 20 21 22 23
  Phi     11 12 13 14 15  - 16 17 18  - 19 20
 Forward Inner
  sector   0   1   2   3   4   ... 
  Phi     24   1   2   3   4   ...
\endcode
 - sectorInReadout : 0 .. 11 increases with phi ( = rodID -1)
  - Each readoutSector covers 4 endcap sectors and 2 forward sectors. 
   - ex) readoutSector #0 covers
    - Endcap sector : 0 .. 3
    - Forward sector :  0 .. 1
  - Inner sectors in each 1/4 wheel is covered by one readout sector. 
   - ex) readoutSector #1 covers
    - Endcap Inner sector : 0 .. 5 
    - Forward Inner sector : 0 .. 5 
- Related properties  
 - isBackward : Indicates Backward chamber if true
 - F/B assignment in A-side is as follows (F <--> B is reversed in C-side) :
\code
Endcap
 sector      0    1    2    3    4    5
             F    B    F    B    F    B
Forward
 sector      0         1         2
             B         B         B
Endcap Inner (except for octant#5)
 sector      0         1         2
             F         F         B
Endcap Inner (octant#5)
 sector      0         1         2
             F         B         -
Forward Inner 
 sector      0         1         2
             B         B         B
\endcode

@section MuonTGC_CablingFiles Necessary files 
Under share directory, there are necessary files. 
These files describes connection in a 1/12 sector. 
All 1/12 sectors are the same.  
- MuonTGC_Cabling_ASD2PP.db : Cconnection table between ASDs and PP inputs 
- MuonTGC_Cabling_PP.db : PP inputs and outputs (i.e. SLBs)
- MuonTGC_Cabling_PP2SL.db : PP outputs(SLB) and SLs (trigger path)
- MuonTGC_Cabling_SLB2ROD.db : SLBs and RODs (readout path)  

While correct SBLOCs for EIFI in MuonTGC_Cabling_SLB2ROD.db are
\code
 EWI 2
  0 0  8  6   8  1
  1 0  9  7   8  3
 ESI 2
  0 0  8  6   8  1
  1 0  9  7   8  3
 FWI 2
  0 0  0  0   8  0
  1 0  1  1   8  2
 FSI 2
  0 0  0  0   8  0
  1 0  1  1   8  2
\endcode
the following SBLOCs are used. 
\code
 EWI 2
  0 0  1  6   8  1
  1 0  3  7   8  3
 ESI 2
  0 0  1  6   8  1
  1 0  3  7   8  3
 FWI 2
  0 0  0  0   8  0
  1 0  2  1   8  2
 FSI 2
  0 0  0  0   8  0
  1 0  2  1   8  2
\endcode
The modification to use the correct SBLOCs are difficult. 
The correct SBLOCs of readout formatted data of Byte Stream are converted to the used SBLOC at the Byte Stream <--> RDO conversions. 
https://savannah.cern.ch/bugs/?57051

@include requirements
  
*/
