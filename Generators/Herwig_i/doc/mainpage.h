/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

 <i>

The herwig interface documentation is in  pdf format, have a look at

https://svnweb.cern.ch/trac/atlasoff/browser/Generators/Herwig_i/trunk/doc/Herwig.pdf

This package also contains the interface to run the Dpemc generator, an extension to Herwig. Have a look at Herwig_i/share/jobOptions.dpemc.py for an example. The Dpemc variables that can be changed include  typepr, typint, gapspr, prospr, cdffac, nstru. For more details see http://boonekam.home.cern.ch/boonekam/dpemc.htm 

This package also contains an interface between Jimmy and Athena. It puts the events into the transient store in HepMC format. Jimmy is developed as a plug-and-play add-on of Herwig. Ther user of the ATLAS Jimmy interface should read the documentation of the Herwig_i interface to understand how to set the herwig parameters, by replacing the word Herwig with the word Jimmy in Herwig_i's documentation. The Jimmy_i interface provides in addition the possibility to set the following, Jimmy specific, variables: msflag, jmbug, ptjim, and jmueo which are documented on the external Jimmy package documentation page in http://hepforge.cedar.ac.uk/jimmy . There is an example jobOptions file, see Generators/Herwig_i/share/jobOptions.jimmy.py

</i>
@author 

Judith Katzy (Jan 2008)
updated E.M.Lobodzinska (Jun 2009)
        
Vojtech Juranek for Dpemc

Georgios Stavropoulos for Jimmy        

@mainpage

@htmlinclude used_packages.html

@include requirements


*/
