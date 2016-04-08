/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage AlpGen_i

@author Ian Hinchliffe, Georgios Stavropoulos, Jon Butterworth and Judith Katzy
      
@section  An interface between AlpGen and Athena

This package runs AlpGen from within Athena.

LIke for the other Les Houche type generators, users must first run AlpGen in standalone mode and make a file of events. An athena job then takes these events 
hadronizes them and passes them down the Athena event chain. See the examples in 
AlpGen_i/share/jobOptions.AlpgenPythia.py
AlpGen_i/share/jobOptions.AlpgenHerwig.py

which show how to read the AlpGen events, add parton showers and hadronize them using Pythia or Herwig. Note that in ATLAS we use the "new" (i.e. PYTHIA ^.4) shower and this does not work with the AlpGen parton shower matching, so HERWIG should be used in general.

The events must be made with a version of AlpGen that is compatible with LesHouches interface, AlpGen version 2.13 and 2.12 are. It is recommended that you use the latest compatible version of standalone Alpgen.

Within the GeneratorModuls, m_ExternalProcess=4 is used to signify AlpGen to both the HERWIG and Pythia interfaces.

@section AlpGen and Pythia
To hadronize AlpGen generated events with Pythia, you need to link the file with the input parameters 
and the one with the unweighted events produced by Alpgen to the files inparmAlpGen.dat and alpgen.unw_events 
respectively. Then you only need to run athena with the jobOptions file jobOptions.AlpgenPythia.py by typing 
in the prompt 

athena jobOptions.AlpgenPythia.py


@section Alpgen and Herwig

To hadronize Alpgen generated events with Herwig, you need to
link the file with the input parameters and the one with the
unweighted events produced by Alpgen to the files inparmAlpGen.dat
and alpgen.unw_events respectively. Then you only need to run athena
with the jobOptions file jobOptions.AlpgenHerwig.py by typing in the
prompt 

  athena jobOptions.AlpgenHerwig.py




The program flow in Herwig is a little complex, especially at the
end of a run. The interface Herwig.cxx (in Herwig_i) calls HWHGUP (a
Herwig routine) which calls UPEVNT (modified by ATLAS to make sure
it calls the right routine in usealpgen.f, which is in
AlpGen_i). When the end of a file is found, UPEVNT calls ALSFIN (in
atoher_65.f in Herwig_i), which then calls HWUGUP (Herwig 
routine). ATLAS uses a modified version of HWUGUP which instead of
abruptly stopping the program (which is bad for Athena!) sets IERROR
and GENEV such that events generation is terminated more
gracefully. This still leads to some confusing apparent error messages
from Athena, but the Herwig and Alpgen output is at least
consistent.

To specify Alpgen running with Herwig\ you need to set the line

 job.Herwig.HerwigCommand = ["iproc alpgen"]

in your jobOptions file.  It is also possible to specify Higgs decay information 
for the events by adding a second argument to the  iproc parameter, 
which will be interpreted as the ID value (last two digits of IPROC) controlling 
the Higgs decay.  e.g.

 job.Herwig.HerwigCommand = ["iproc alpgen 12"]

would force any Higgs particles present in your input events to decay to two photons. See the Herwig manual for more information.



last update Feb.28, 2008
@htmlinclude used_packages.html

@include requirements


*/
