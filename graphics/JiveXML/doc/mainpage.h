/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage JiveXML Package
@author Nikos.Konstantinidis@cern.ch, Eric.Jansen@hef.ru.nl, Juergen.Thomas@cern.ch
@section IntroJiveXML Introduction


This is the core package containing the code for producing the XML event files for 
the <a href="http://cern.ch/atlantis/">Atlantis</a> event display. User documentation
for JiveXML can be found <a href="http://www.hep.ucl.ac.uk/atlas/atlantis/?q=jivexml">here</a>.

The main component of JiveXML is the AlgTool EventData2XML. This is ultimately responsible for 
producing the XML files for atlantis. For each datatype you want to put in the XML file, EventData2XML
initializes and calls an AlgTool. Each such AlgTool conforms with the abstract interface JiveXML::IDataRetriever
and inherits from the JiveXML::DataRetriever base class AlgTool, and all it does is access the corresponding 
data from StoreGate, extract the relevant info and produce an XML fragment which is passed back to
EventData2XML, which at the end writes out the complete XML file, one per event.

Being an AlgTool, EventData2XML can be called conditionally at each event (so as to produce XML files 
only for interesting or pathological events). However it is often simpler (and sometimes desirable) to
produce an XML file for each event processed in athena. For this there is the athena Algorithm JiveXML::AlgoJiveXML,
which does nothing but initialize and then execute the EventData2XML AlgTool.

@section propsJiveXML JiveXML properties

   - FileNamePrefix : Prefix of output XML files ( default JiveXML )
   - AtlasRelease : The offline release version, appears in the XML files 
   - DataTypes : array of strings, each containing the name of a DataRetriever (corresponding to a given datatype)
   - WriteToFile : in online more, in addition to sending the XML data through the network from athena to atlantis, you can also write them to files ( default True )
   - OnlineMode : when True it starts a server on a separate thread which sends XML data to Atlantis through the network (default false) 
   - ServerPort : the network port number to use in OnlineMode; normally, never needs to be changed ( default 48965 )
   - WriteGeometry : when True it produces two XML files AGeometry.xml and AMuonGeometry.xml, which contain all the geometry info that Atlantis needs (info obtained directly from GeoModel)
   - IgnoreEventInfo : when True the event/run number are ignored and instead the XML file names have the form FileNamePrefix_0_X, where X is incremented every time EventData2XML is called. Potentially useful if you want to call EventData2XML more than once for the same event (e.g. for different Trigger RoIs?) ( default False )


@htmlinclude used_packages.html


@include requirements

*/
