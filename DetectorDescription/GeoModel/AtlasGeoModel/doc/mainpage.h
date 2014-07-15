/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage AtlasGeoModel Package

@authors Grant Gorfine, Vakho Tsulaia, Joe Boudreau

@section AtlasGeoModelIntro Introduction


This package contains scripts to initialize the GeoModel Detector Description.
For the migration to Configurables a new python module has been created. All clients
of AtlasGeoModel package are requested to switch to the scripts in this python module.
Old python scripts in the share directory have been declared obsolete. 

@section AtlasGeoModelUsage General Usage Notes.

To initialize the geometry simply put in your joboptions:

@verbatim
 from AthenaCommon.GlobalFlags import globalflags
 globalflags.DetDescrVersion = "ATLAS-CSC-02-00-00"
 from AtlasGeoModel import SetGeometryVersion
 from AtlasGeoModel import GeoModelInit
@endverbatim

There are other possibilities for the values of DetDescrVersion.
See SetGeometryVersion.py in the python module of this package in order to see full list of values that the 
DetDescrVersion flag can have.

You can turn off detectors using DetFlags. For example

@verbatim
from AthenaCommon.DetFlags import DetFlags
# NB. Flags are off by default.
#DetFlags.detdescr.all_setOn()
DetFlags.detdescr.ID_setOn()
DetFlags.detdescr.Muon_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
@endverbatim


The scripts also look at GlobalFlags
to determine if it is CTB or DC1 geometries which require special treatment.
Normally these are already set if you are using ReExCommon or RecExTB.
As long as these flags are set correctly the correct geometry should be
loaded. If they are not already set you need to do:

For CTB
@verbatim
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_ctbh8()
@endverbatim

For DC1
@verbatim
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_geant3()
@endverbatim

@section AtlasGeoModelJobOptions Job Options

 - GeoModelInit.py:		Initializes GeoModel. Loads GeoModelSvc.

 - SetGeometryVersion.py:       Sets the appropriate Geometry tags based
                                on the DetDescrVersion flag. In the
                                future this will not be needed for
                                reconstruction as the geometry version will
                                be determined from the POOL file or CondDB.

The following job options are used by the above. Users do not need to call
them directly. Users can turn off detectors using DetFlags.
                                
 - GeoModelInitStandard.py:        Standard initialization.
 - GeoModelInitDC1.py:             Initialization for DC1.
 - GeoModelInitCTB.py:             Initialization fro CTB.
 - GeoModelInitH6.py:              Initialization of H6 TestBeam configurations
    
 - InDetGM.py:    	   	   Inner Detector Det Descr. 
 - MuonGM.py: 		    	   Muon Det Descr
 - LArGM.py:			   LAr Det Descr
 - TileGM.py:			   Tile Det Descr
 - MiscGM.py:			   Other Det Descr. Contains beam pipe.

 - InDetGMCTB.py:		   Inner Detector CTB Descr. 
 - TileGMCTB.py:		   Tile CTB Descr


*/
