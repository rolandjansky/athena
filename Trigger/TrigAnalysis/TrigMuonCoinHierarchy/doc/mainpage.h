/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
@mainpage TrigMuonCoinHierarchy Package

The TrigMuonCoinHierarchy package contains classes to provide the hierarchical information of TGC coincidences and hits using pointers of hit PRD (TgcPrepData) and coincidence PRD (TgcCoinData). 

@author Susumu Oda <Susumu.Oda@cern.ch>

@section TgcCoinHierarchy TgcCoinHierarchy
The TgcCoinHierarchy class holds pointers of related TgcCoinData (coincidence PRD) and TgcPrepData (hit PRD). 
There are three levels of coincidences, Sector Logic (SL), High pT (HiPt) and Tracklet. 
HiPt and Tracklet are sub-divided into HiPt Wire (in the eta direction) and HiPt Strip (in the phi direction), and Tracklet Wire and Tracklet Strip, respectively. 
Tracklet is created by coincidence of hits in TGC2 and TGC3. 
HiPt is created by coincidence of hits in TGC1 and Tracklet. 
SL is created by HiPt Wire and/or HiPt Strip. 
In some special settings, for example cosmic running, SL is created by Tracklet only. 
The TgcCoinHierarchy instance has at least one coincidence and each coincidence is unique. 
However, hits in each station can be multiple.    
The relations among coincidences are checked by TrackletId. 
The relations between hits and coincidences are checked using the TGC cabling service.  
These checks are performed by TgcCoinHierarchyFindTool. 
The instances of this class are held and by TgcCoinHierarchySvc. 
TgcCoinHierarchySvc provides the pointers of the instances to the client classes. 
TgcCoinHierarchy defines enum's which are used in this package. 
TgcCoinHierarchy also provides The conversion methods for the enum's. 

\code
         Sector Logic 
        /  |     |   \ 
 HiPt Wire |     | HiPt Strip 
     /  \  |     |   /   \ 
    / Tracklet  Tracklet  \ 
   /    Wire     Strip     \ 
  /    /    \     /   \     \ 
 TGC1 TGC2 TGC3 TGC3  TGC2  TGC1 
 Wire Wire Wire Strip Strip Strip 
 Hits Hits Hits Hits  Hits  Hits 
\endcode

@section TgcCoinHierarchySvc TgcCoinHierarchySvc
The TgcCoinHierarchySvc provides pointers of the TgcCoinHierarchy class based on the followings: 
- Hit
 - Identifier 
 - Muon::TgcPrepData*
 - Muon::MuonCluster*
 - Trk::PrepRawData*
 - Trk::RIO_OnTrack* 
- Hits associated with track
 - Trk::Track* 
- Coincidence
 - Muon::TgcCoinData*
- Trigger Sector (48 trigger sectors in Endcap and 24 trigger sectors in Forward per side)
 - bool isAside, bool isForward, int phi
- ROI number
 - bool isAside, bool isForward, int phi, int roi 
  - In this case, returns only TgcCoinHierarchy's which have Sector Logic.  
- Event 
  - In this case, returns all the pointers of TgcCoinHierarchy's in the event. 

@section TgcCoinHierarchyClassifyTool TgcCoinHierarchyClassifyTool 
The TgcCoinHierarchyClassifyTool class classifies TgcCoinData and TgcPrepData into each trigger sector using the TgcCoinHierarchyTriggerSector class. 
While the trigger decision is performed for each trigger sector, the TgcCoinData and TgcPrepData are recorded in those container with the granuality of sector (there are 12 sectors per side). 
The TgcCoinHierarchyClassifyTool also provides ID conversion functionalities. 

@section TgcCoinHierarchyTriggerSector TgcCoinHierarchyTriggerSector
The TgcCoinHierarchyTriggerSector class holds multiple coincidence PRD's and multiple hit PRD's in one trigger sector. 
This class is created by TgcCoinHierarchyClassifyTool and is used by TgcCoinHierarchyFindTool to create TgcCoinHierarchy. 

@section TgcCoinHierarchyFindTool TgcCoinHierarchyFindTool 
The TgcCoinHierarchyFindTool class checks relations among coincidences and hits in one sector and creates TgcCoinHierarchy instances. 
The relations among coincidences are checked by TrackletId.  
The relations between hits and coincidences are checked using the TGC cabling service. 
The check of the relations between hits and coincidences will be improved very soon (as of July 16, 2009). 
Currently, all the hits in a trigger sector are associated to all the TgcCoinHierarchy's in the same trigger sector.  

@section TgcCoinHierarchyTestAlg TgcCoinHierarchyTestAlg
The TgcCoinHierarchyTestAlg is used for debugging and validation. 
Currently, it uses the following as starting points: 
- Event 
- Identifier 
- Trk::PrepRawData* 
- Muon::TgcCoinData* 
- bool isAside, bool isForward, int phi 
- Trk::Track* 

In share/TgcCoinHierarchyTestAlg_topOptions.py, there are flags to enable/disable the above starting points.  

@section Presentation Presentation 
A presentation on the TrigMuonCoinHierarchy package and on the results obtained with the package was shown at the Muon Offline Software Meeting on 9 December, 2009. 
- http://indicobeta.cern.ch/conferenceDisplay.py?confId=76230
 - Validation of decoding of TGC hits and coincidences   

@include requirements

*/
