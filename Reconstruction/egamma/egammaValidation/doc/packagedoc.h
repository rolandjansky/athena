/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page egammaValidation_page egammaValidation Package

This package contains the jobOptions and root Macros ran by RTT tests.

@author Frederic Derue derue@lpnhe.in2p3.fr
@author many many others

The share directory contain a jobOption which runs the egamma sw, starting from RDO's : all other combined reconstruction algs are sitched off. 
The output is the standard combined ntuple.
A set of root macros is then ran on top of the ntuple, and some .hist files are produced. 
Another macro then superimposes these histograms to a reference file ( if any ) and creates .ps files.

The list of input files ( all single particles ) and tests ran can be found in the RTT configuration file :
test/egammaValidation_TestConfiguration.xml 
*/
