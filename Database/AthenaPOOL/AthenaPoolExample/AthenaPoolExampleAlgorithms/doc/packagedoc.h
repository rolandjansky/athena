/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page AthenaPoolExampleAlgorithms_page 

The package Database/AthenaPOOL/AthenaPoolExample contains running examples of algorithms writing and
reading Data Objects using AthenaPool. This sub package, AthenaPoolExampleAlgorithms, defines the 
algorithms write and read data, tags and conditions via AthenaPOOL.

WriteData: The WriteData algorithm creates some ExampleHits in a ExampleHitContainer and records them
into StoreGate.

WriteTag: The WriteTag algorithm creates an AthenaAttributeList and records it in StoreGate so it can be
used for collections.

AthenaPoolExample_WriteJobOptions.py: These jobOptions create two streams to write two different data
files via AthenaPOOL.

	Tests:

	- Simple Writing of EventData and InFile MetaData.
	- Multiple Streams.
	- Tag Writing.


@verbatim

run > checkFile.py SimplePoolFile1.root
## opening file [SimplePoolFile1.root]...
## importing ROOT...
## importing ROOT... [DONE]
## opening file [OK]
File:SimplePoolFile1.root
Size:       34.836 kb
Nbr Events: 20

================================================================================
     Mem Size       Disk Size        Size/Evt      MissZip/Mem  items  (X) Container Name (X=Tree|Branch)
================================================================================
      72.382 kb        0.000 kb        0.000 kb        1.000       20  (T) DataHeader
--------------------------------------------------------------------------------
      46.367 kb        0.000 kb        0.000 kb        1.000       20  (B) EventInfo_p2_McEventInfo
      29.958 kb        0.000 kb        0.000 kb        1.000       20  (B) ExampleHitContainer_p1_MyHits
       9.207 kb        0.000 kb        0.000 kb        1.000        1  (B) EventStreamInfo_p1_Stream1
      50.704 kb        0.000 kb        0.000 kb        1.000        1  (T) MetaDataHdrDataHeader
================================================================================
     208.618 kb        0.000 kb        0.000 kb        0.000       20  TOTAL (POOL containers)
================================================================================
## Bye.
run > checkFile.py SimplePoolFile2.root
## opening file [SimplePoolFile2.root]...
## importing ROOT...
## importing ROOT... [DONE]
## opening file [OK]
File:SimplePoolFile2.root
Size:       29.271 kb
Nbr Events: 20

================================================================================
     Mem Size       Disk Size        Size/Evt      MissZip/Mem  items  (X) Container Name (X=Tree|Branch)
================================================================================
      67.502 kb        0.000 kb        0.000 kb        1.000       20  (T) DataHeader
--------------------------------------------------------------------------------
      46.367 kb        0.000 kb        0.000 kb        1.000       20  (B) EventInfo_p2_McEventInfo
       9.207 kb        0.000 kb        0.000 kb        1.000        1  (B) EventStreamInfo_p1_Stream2
      50.704 kb        0.000 kb        0.000 kb        1.000        1  (T) MetaDataHdrDataHeader
================================================================================
     173.780 kb        0.000 kb        0.000 kb        0.000       20  TOTAL (POOL containers)
================================================================================
## Bye.

@endverbatim

AthenaPoolExample_AppendJobOptions.py: These jobOptions appends events and tags to the second stream written by
AthenaPoolExample_WriteJobOptions.py.

        Tests:

        - Simple Appending of EventData and InFile MetaData.
        - Tag Writing in update (append) mode.

	Note:
	- Appending InFile MetaData does _not_ work in the current framework.

@verbatim

run > checkFile.py SimplePoolFile2.root
## opening file [SimplePoolFile2.root]...
## importing ROOT...
## importing ROOT... [DONE]
## opening file [OK]
File:SimplePoolFile2.root
Size:       43.342 kb
Nbr Events: 40

================================================================================
     Mem Size       Disk Size        Size/Evt      MissZip/Mem  items  (X) Container Name (X=Tree|Branch)
================================================================================
      84.822 kb        0.000 kb        0.000 kb        1.000       40  (T) DataHeader
--------------------------------------------------------------------------------
      52.327 kb        0.000 kb        0.000 kb        1.000       40  (B) EventInfo_p2_McEventInfo
       9.207 kb        0.000 kb        0.000 kb        1.000        1  (B) EventStreamInfo_p1_Stream2
       9.207 kb        0.000 kb        0.000 kb        1.000        1  (B) EventStreamInfo_p1_Stream1
      51.562 kb        0.000 kb        0.000 kb        1.000        2  (T) MetaDataHdrDataHeader
================================================================================
     207.125 kb        0.000 kb        0.000 kb        0.000       40  TOTAL (POOL containers)
================================================================================
## Bye.

@endverbatim

AthenaPoolExample_RWJobOptions.py: These jobOptions read the previous events (SimplePoolFile1.root from
AthenaPoolExample_WriteJobOptions.py) and processes the hits into tracks which are than written out.

        Tests:

        - Reading of EventData (without navigation) and InFile MetaData.
	- Writing EventData with navigational relations (ElementLinks, ElementLinkVector, Navigable) to
	  upstream EventData.


@verbatim

run > checkFile.py SimplePoolFile3.root
## opening file [SimplePoolFile3.root]...
## importing ROOT...
## importing ROOT... [DONE]
## opening file [OK]
File:SimplePoolFile3.root
Size:       39.700 kb
Nbr Events: 20

================================================================================
     Mem Size       Disk Size        Size/Evt      MissZip/Mem  items  (X) Container Name (X=Tree|Branch)
================================================================================
      79.462 kb        0.000 kb        0.000 kb        1.000       20  (T) DataHeader
--------------------------------------------------------------------------------
      46.367 kb        0.000 kb        0.000 kb        1.000       20  (B) EventInfo_p2_McEventInfo
      49.346 kb        0.000 kb        0.000 kb        1.000       20  (B) ExampleTrackContainer_p1_MyTracks
       9.207 kb        0.000 kb        0.000 kb        1.000        1  (B) EventStreamInfo_p1_Stream1
      50.704 kb        0.000 kb        0.000 kb        1.000        1  (T) MetaDataHdrDataHeader
================================================================================
     235.086 kb        0.000 kb        0.000 kb        0.000       20  TOTAL (POOL containers)
================================================================================
## Bye.

@endverbatim

AthenaPoolExample_ReadJobOptions.py: These jobOptions read the previous events (SimplePoolFile1-3.root) including
BackNavigation via global flag and scope.

        Tests:

        - Reading of EventData (with navigation) and InFile MetaData.
	- Skipping events.
	- BackNavigation to upstream data.

SimplePoolFile1.root: EventInfo, Hits
SimplePoolFile2.root: EventInfo
SimplePoolFile3.root: EventInfo, Hits (via Navigation), Tracks


@section AthenaPoolExampleAlgorithms_Documentation Additional Documentation

See the ATLAS User/Developer HowTo document for additional information.

The code can be browsed using LXR
(http://alxr.usatlas.bnl.gov/lxr/source/atlas/Database/AthenaPOOL/AthenaPoolExample/AthenaPoolExampleAlgorithms/)








@author Peter van Gemmeren <gemmeren@anl.gov>
*/
