/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage

The Cabling Services ( CaloTriggerTowerService of the present package, LArCablingService and  TileCablingService ) 
are meant to provide the mapping between cells and Trigger Towers, between offline and online identifiers and other mappings.<br>


<center>  <H2> What is provided by the CaloTriggerTowerService </H2></center>

Online TT channel to TT offline conversion, example:

<pre>
try {
   HWIdentifier ttChannel = ...;
   Identifier ttId = cnvToIdentifier(ttChannel) ;
} 
catch(LArID_Exception& except) {
   msglog << MSG::ERROR << "LArID_Exception " << (std::string) except << endreq ;
}
</pre>

and TT offline id to TT online conversion, example:

<pre>
try {
   Identifier ttOffId = ...;
   HWIdentifier ttOnlId = createTTChannelID(ttOffId)
} 
catch(LArID_Exception& except) {
   msglog << MSG::ERROR << "LArID_Exception " << (std::string) except << endreq ;
}
</pre>


Cell offline to Trigger Tower offline correspondance, example:

<pre>
//
//... to which TT does this cell belong
//
Identifier cellId = ...;
Identifier ttId = m_cablingService->whichTTID(cellId);
</pre>

Trigger Tower offline to cell offline correspondance, example:

<pre>
Identifier ttId = ...;
//
//... fill a vector with offline identifier of cells belonging to offline tower ttId (offline id with layer info-- so called 'extended')
//
std::vector < Identifier > channelIdVec = m_cablingService->createCellIDvecLayer(ttId);
//
//... fill a vector with offline identifier of cells belonging to offline tower ttId (offline id without layer info)
//
std::vector < Identifier > channelIdVec = m_cablingService->createCellIDvecTT(ttId);
</pre>


Channel online to Trigger Tower online correspondance <br>
NOTA BENE: the mapping is actually stored in terms of offline<->offline correspondance,
therefore the offline<->offline conversion is faster than the online<->online conversion below. <br>
Example:<br>

<pre>
//
//... to which TT does this (cell's) channel belong
//
HWIdentifier channelId = ...;
HWIdentifier ttOnl = m_cablingService->whichTTChannelID(channelId);
</pre>

Trigger Tower online to cell channel online correspondance, example:

<pre>
//
//... fill a vector with online identifier of cells belonging to tower ttChannel (online id with layer info)
//
HWIdentifier ttChannel=...;
std::vector < HWIdentifier > channelIdVec = m_cablingService->createChannelIDvec(ttChannel,0);
</pre>




@htmlinclude used_packages.html

@include requirements

*/
