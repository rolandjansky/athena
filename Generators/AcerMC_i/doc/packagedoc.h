/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page AcerMC_i_page AcerMC_i
@author  Borut.Kersevan@cern.ch, Giorgios.Stavropoulos@cern.ch, Ian.Hinchliffe@cern.ch,  Judith.Katzy@cern.ch

@section AcerMC_i_The interface between AcerMC and Athena
 
This package runs AcerMC  from within Athena. See the example
in AcerMC_i/share/jobOptions.AcerMCPythia.py  and 
  AcerMC_i/share/jobOptions.AcerMCHerwig.py  which show how to
read AcerMC events and hadronize them using Pythia or Herwig

Users must first run 
AcerMC in standalone mode and make a file of events. An athena job
then takes these events hadronizes them and passes them down the
Athena event chain. The events must be made with a version of AcerMC
 that supports the Les Houches
interface. There  are compatible versions in  /afs/cern.ch/atlas/offline/external/acermc
To hadronize AcerMC generated events with Herwig, you only need to run athena with the jobOptions
file jobOptions.AcerMCHerwig.py by typing in the prompt 
 athena jobOptions.AcerMCHerwig.py

To hadronize  AcerMC generated events with Pythia, you only need to run athena with the jobOptions
file jobOptions.AcerMCPythia.py by typing in the prompt  athena jobOptions.AcerMCPythia.py

Example jobOption files to run with Tauola and Photos are also provided, see 
jobOptions.AcerMCHerwig.AtlasTauolaPhotos.py and jobOptions.AcerMCPythia.AtlasTauolaPhotos.py



The  AcerMC generator home page is here
https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AcerMCForAtlas




*/
