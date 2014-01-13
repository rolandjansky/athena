/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage
@author Nikos.Konstantinidis -at- cern.ch, Eric.Jansen -at- hef.ru.nl, Juergen.Thomas -at- cern.ch
@section IntroCaloJiveXML Introduction

This package contains the AlgToos that produce the XML fragments of Calorimeter related datatypes for the XML files read by atlantis.
The AlgTools, all inheriting from JiveXML::DataRetriever, are the following:

   - JiveXML::CaloClusterRetriever : retrieves the CaloClusters. Property:
      - StoreGateKey : the StoreGate location of the CaloClusterContainer ( default "LArClusterEM" )

   - JiveXML::CaloRetriever  : retrieves the cells for all the different ATLAS Calorimeters

   - JiveXML::CaloTBRetriever : largely obsolete now, it was used for CTB-2004 data, to access the information about the rotation of the calorimeters in the CTB setup.


@htmlinclude used_packages.html

@include requirements

*/
