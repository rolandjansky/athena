/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage

@section count EventCount

12 June 2007 J. Cranshaw

The EventCount algorithm gives a basic description of the contents of 
a POOL file by rapidly iterating through the file and interrogating the 
DataHeaders.

The simplest invocation is 
@verbatim
athena -c "In=['<filename>']" AthenaPoolUtilities/EventCount.py
@endverbatim

Sample output (produced during finalize):

EventCount           INFO ---------- INPUT FILE SUMMARY ----------<br>
EventCount           INFO Input contained: 2 events<br>
EventCount           INFO  -- Event Range ( 1 .. 2 )<br>
EventCount           INFO Input contained: 1 runs<br>
EventCount           INFO  -- 1<br>
EventCount           INFO Input contained the following Event Types<br>
EventCount           INFO  -- Detector<br>
EventCount           INFO  -- Physics<br>
EventCount           INFO  -- Simulation<br>
EventCount           INFO Input contained references to the following File GUID's<br>
EventCount           INFO  -> EventSelector<br>
EventCount           INFO       - 12E5C634-DA84-D911-9440-0002B3D87B0F<br>
EventCount           INFO       - 4C81302D-6D13-DA11-90F7-0002B3D8C0EF<br>
EventCount           INFO       - 680D723B-E184-D911-8C6F-00D0B7B86232<br>
EventCount           INFO  -> StreamRDO<br>
EventCount           INFO       - 8474BB36-B213-DA11-87E4-000E0C4D3C64<br>
EventCount           INFO  -> StreamESD<br>
EventCount           INFO       - B0056D54-B718-DA11-84A0-000E0C4DEAEA<br>
EventCount           INFO Input contained the following CLIDs and Keys<br>
EventCount           INFO  -> 2101 EventInfo (2) <br>
EventCount           INFO       - !McEventInfo<br>
EventCount           INFO  -> 133273 Unknown (2) <br>
EventCount           INFO       - GEN_AOD<br>
EventCount           INFO  -> 9327453 Unknown (2)<br> 
EventCount           INFO       - !VxPrimaryCandidate<br>
EventCount           INFO  -> 222376821 DataHeader (2) <br>
EventCount           INFO       - !StreamAOD<br>
EventCount           INFO  -> 1073853171 Unknown (4)<br> 
EventCount           INFO       - !MuidMuonCollection<br>
EventCount           INFO       - !StacoMuonCollection<br>
EventCount           INFO  -> 1094973728 Unknown (2) <br>
EventCount           INFO       - !PhotonCollection<br>
EventCount           INFO  -> 1118613496 Unknown (12) <br>
EventCount           INFO       - !Cone4TowerParticleJets<br>
EventCount           INFO       - !Cone4TruthParticleJets<br>
EventCount           INFO       - !ConeTowerParticleJets<br>
EventCount           INFO       - !ConeTruthParticleJets<br>
EventCount           INFO       - !KtTowerParticleJets<br>
EventCount           INFO       - !KtTruthParticleJets<br>
EventCount           INFO ----------------------------------------<br>

Necessarily, any metadata information in the DataHeader is 
dependent on the output process properly setting it. 

@section RequireUniqueEvent

Issues a WARNING if a run/event number repeats during an athena job.

*/
