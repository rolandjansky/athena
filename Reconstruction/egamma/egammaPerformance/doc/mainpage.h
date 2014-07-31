/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage egammaPerformance Package

This package contains the jobOptions and root Macros ran by RTT tests.
It also contains the monitoring tools of electron and photon reconstruction identification 
run on Tier 0 for Data Quality


@author Claire Adam-Bourdarios claire.bourdarios@cern.ch
@author many many others

@section RTT jobOption and macros

@author Claire Adam-Bourdarios claire.bourdarios@cern.ch

The share directory contain a jobOption which runs the egamma sw, starting from RDO's : all other combined reconstruction algs are sitched off. 
The output is the standard combined ntuple.
A set of root macros is then ran on top of the ntuple, and some .hist files are produced. 
Another macro then superimposes these histograms to a reference file ( if any ) and creates .ps files.

The list of input files ( all single particles ) and tests ran can be found in the RTT configuration file :
test/egammaPerformance_TestConfiguration.xml 

@section egammaMonTool

@author Kamal Benslama  kamal.benslama@uregina.ca

The egammaPerformance code is to be executed during the T0 processing. It makes a set of monitoring histograms and saves them into a ROOT file for later examination. The package contains following classes: 
        - egammaMonTool - a tool for monitoring egamma reconstruction and identification
        - electronMonTool - a tool for monitoring electron reconstruction and identification
        - softEMonTool - a tool for monitoring track-based electron reconstruction and identification
        - forwardEgammaMonTool - a tool for monitoring forward electron reconstruction and identification
        - photonMonTool - a tool for monitoring photon reconstruction and identification
        - PhysicsMonTool - a tool for monitoring physics process such as,  J/Psi->ee, Upsilon->ee, W->enu, and Z->ee

See wiki page https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EgammaPerformance for more details.


*/
