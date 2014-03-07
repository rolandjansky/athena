/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage
@author Nikos.Konstantinidis -at- cern.ch, Eric.Jansen -at- hef.ru.nl, Juergen.Thomas -at- cern.ch
@section IntroRecoJiveXML Introduction

This package contains the AlgToos that produce the XML fragments of Rconstruction related datatypes for the XML files read by atlantis.
The AlgTools, which inheriting from JiveXML::DataRetriever, are the following:

   - JiveXML::JetRecJetRetriever : retrieves all jet collections from StoreGate. Property:
      - StoreGateKey : ( default "Cone4TowerJets" ) the StoreGate key of the jet collection that is displayed by default in atlantis.

   - JiveXML::MissingETRetriever : retrieves all Etmis collections from StoreGate. Property:
      - StoreGateKey : ( default "MET_Final" ) the StoreGate key of the missing Et that is displayed by default in atlantis.


@htmlinclude used_packages.html

@include requirements

*/
