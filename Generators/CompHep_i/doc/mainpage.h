/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

 <i>CompHep_i is an interface between CompHep generator and Athena.

This package runs Comphep  from within Athena. See the example
in Comphep_i/share/jobOptions.comphep.py  which shows how to
read Comphep events and hadronize them using Pythia. 

Users must first run 
Comphep in standalone mode and make a file of events. An athena job
then takes these events hadronizes them and passes them down the
Athena event chain. The events must be made with a version of Comphep
that is compatible, recent versions that support the Les Houches
interface should be acceptable. CompHep version 4.4.3 is currently
interfaced.

Modify the file inparmCompHep.dat that
you will find in your run area. You specify the name of your event
file here. No Athena recompilation is needed is needed to process the events.

Comphep information can be found here

http://theory.sinp.msu.ru/parser/parser.php?file=/phpcms/comphep

@author Ian Hinchliffe (I_Hinchliffe@lbl.gov) and Georgios Stavropoulos (George.Stavropoulos@cern.ch)

put into doxygen by E. Lobodzinska (Jan. 2008)

</i>

@mainpage

@htmlinclude used_packages.html

@include requirements

*/
