/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TrigByteStreamTools
@author Anna.Sfyrla@cern.ch Brian.Petersen@cern.ch Tomasz.Bold@cern.ch

@section IntroTrigByteStreamTools Introduction

This packages contains several small python scripts and python modules
to ease the use of bytestream files

 - pickEvents.py       : save selected events from a bytestream file
 - PickErrorEvents.py  : save events in different streams based on 
                         the algorithm errors recorded in data file
 - addL1.py            : Recalculate RoIB fragments based on the
                         DAQ fragments if original RoIB fragments
			 are missing. 
			 Can be used as an event-modifier in athenaMT
 - replaceMUCTPI.py    : Recalculate MuCTPi RoIB fragment based on the
                         DAQ fragment.
			 Can be used as an event-modifier in athenaMT
 - truncateEvents.py   : Truncates certain ROD fragments in a BS file
 - PickEventsCastor.py : save selected list of events from specific 
                         run and stream, from CASTOR
 - mergeBSfiles.py     : Merges all events from a list of directories

*/
