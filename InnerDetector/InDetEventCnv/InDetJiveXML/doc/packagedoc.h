/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page InDetJiveXML_page InDetJiveXML
@author Nikos.Konstantinidis -at- cern.ch, Eric.Jansen -at- cern.ch, Juergen.Thomas -at- cern.ch, Sebastian.Boeser -at- cern.ch

@section InDetJiveXML_InDetJiveXMLIntro Introduction

This package contains the @c AthAlgTools that retrieve inner detector data (Pixel- , SCT- and TRT-hits, clusters, etc. ) from @c StoreGate and 
forward them to the formatting tool. Each tool implements the IDataRetriever interface, through which it is called from the @c AlgoJiveXML.

@section InDetJiveXML_InDetJiveXMLTools Retrievers

- JiveXML::SiSpacePointRetriever @copydoc JiveXML::SiSpacePointRetriever

- JiveXML::SiClusterRetriever @copydoc JiveXML::SiClusterRetriever

- JiveXML::TRTRetriever @copydoc JiveXML::TRTRetriever

- JiveXML::PixelRDORetriever @copydoc JiveXML::PixelRDORetriever

- JiveXML::SCTRDORetriever @copydoc JiveXML::SCTRDORetriever

@section InDetJiveXML_InDetJiveXMLEnv Environment

*/
