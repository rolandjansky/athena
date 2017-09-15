/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SpecialUtils_page SpecialUtils

@section SpecialUtils_introductionSpecialUtils Introduction
This package contains non-general tools, i.e., tools for Neutrino. General tools should be put in AnalysisTools/Utils.

@section SpecialUtils_packagecontentSpecialUtils Package Contents
SpecialUtils contains the following classes:

- NeutrinoUtils : tools for Neutrino. There 2 utilities here. One where the W mass constraint is used to find 
the longitudinal component of the neutrino momentum, for example in W --> e nu, assuming that the transverse 
missing energy is carried away by the neutrino. Up to 2 solutions can be obtained. The other is where one uses 
the collinear approximation to solve for the neutrinos in the decay X --> a + b + Missing Energy, for example Z 
--> tautau -> tauJet + tauJet + Missing Energy; The constraint is that a and b are collinear with the taus 
(either leptons or a TauJet. Thus, 'a' carries a fraction x_a of one tau's momentum and that tau's neutrinos 
carry (1-x_a) of that tau's momentum. Both of these 2 utilities return a container of Neutrino Objects. There is 
a class for the neutrino, Netrino.h, in the package PhyscisAnalysis/AnalysisCommon/ParticleEvent. 



*/
