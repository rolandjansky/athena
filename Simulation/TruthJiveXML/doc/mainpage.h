/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TruthJiveXML
@author Nikos.Konstantinidis -at- cern.ch, Juergen.Thomas -at- cern.ch, Sebastian.Boeser -at- cern.ch

@section TruthJiveXMLIntro Introduction

This package contains the @c AthAlgTools that retrieve truth information ( @c McEventCollection and @c TrackRecordCollection ) from @c StoreGate and 
forward them to the formatting tool. Each tool implements the IDataRetriever interface, through which it is called from the @c AlgoJiveXML.

@section TruthJiveXMLTools Retrievers

- JiveXML::TruthTrackRetriever @copydoc JiveXML::TruthTrackRetriever

- JiveXML::TruthMuonTrackRetriever @copydoc JiveXML::TruthMuonTrackRetriever

- JiveXML::TruthCosmicsTrackRetriever @copydoc JiveXML::TruthCosmicsTrackRetriever

@section TruthJiveXMLEnv Environment
 - @ref used_TruthJiveXML
 - @ref requirements_TruthJiveXML

*/

/**
@page used_TruthJiveXML Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_TruthJiveXML Requirements
@include requirements
*/
