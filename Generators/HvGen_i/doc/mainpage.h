/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

 <i> An interface between HiddenValley Generator and Athena

This package runs HV generator from within Athena. \\See the example
in HvGen_i/share/jobOptions.HvGenPythia.py  which show how to
read HV events and hadronize them using Pythia.

Users must first run HV Generator in standalone mode and make a file of events. An athena job
then takes these events hadronizes them and passes them down the
Athena event chain. The events must be made with a version of HV generator
that is compatible.
Compatible versions can be found in:
/afs/cern.ch/user/g/giagu/public/HvGen and /afs/cern.ch/atlas/offline/external/hvgen

To hadronize HV generated events with Pythia, you only need to run athena with the jobOptions
file jobOptions.HvGenPythia.py by typing in the prompt 
 athena jobOptions.HvGenPythia.py

More infomation about Hidden Valley Models and HV generator here: 

/afs/cern.ch/user/g/giagu/public/HvGen/doc 

/afs/cern.ch/atlas/offline/external/hvgen/doc and in: 

http://www.phys.washington.edu/users/strasslr/hv/hv.htm 

@author Stefano Giagu (Stefano.Giagu@cern.ch) </i>

@mainpage

@htmlinclude used_packages.html

@include requirements

*/
