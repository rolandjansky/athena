/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TruthJiveXML_page TruthJiveXML
@author Nikos.Konstantinidis -at- cern.ch, Juergen.Thomas -at- cern.ch, Sebastian.Boeser -at- cern.ch

@section TruthJiveXML_TruthJiveXMLIntro Introduction

This package contains the @c AthAlgTools that retrieve truth information ( @c McEventCollection and @c TrackRecordCollection ) from @c StoreGate and 
forward them to the formatting tool. Each tool implements the IDataRetriever interface, through which it is called from the @c AlgoJiveXML.

@section TruthJiveXML_TruthJiveXMLTools Retrievers

- JiveXML::TruthTrackRetriever @copydoc JiveXML::TruthTrackRetriever

- JiveXML::TruthMuonTrackRetriever @copydoc JiveXML::TruthMuonTrackRetriever

- JiveXML::TruthCosmicsTrackRetriever @copydoc JiveXML::TruthCosmicsTrackRetriever

@section TruthJiveXML_TruthJiveXMLEnv Environment

*/
