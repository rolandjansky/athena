/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage InDetJiveXML
@author Nikos.Konstantinidis -at- cern.ch, Eric.Jansen -at- cern.ch, Juergen.Thomas -at- cern.ch, Sebastian.Boeser -at- cern.ch

@section InDetJiveXMLIntro Introduction

This package contains the @c AthAlgTools that retrieve inner detector data (Pixel- , SCT- and TRT-hits, clusters, etc. ) from @c StoreGate and 
forward them to the formatting tool. Each tool implements the IDataRetriever interface, through which it is called from the @c AlgoJiveXML.

@section InDetJiveXMLTools Retrievers

- JiveXML::SiSpacePointRetriever @copydoc JiveXML::SiSpacePointRetriever

- JiveXML::SiClusterRetriever @copydoc JiveXML::SiClusterRetriever

- JiveXML::TRTRetriever @copydoc JiveXML::TRTRetriever

- JiveXML::PixelRDORetriever @copydoc JiveXML::PixelRDORetriever

- JiveXML::SCTRDORetriever @copydoc JiveXML::SCTRDORetriever

@section InDetJiveXMLEnv Environment
 - @ref used_InDetJiveXML
 - @ref requirements_InDetJiveXML

*/

/**
@page used_InDetJiveXML Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_InDetJiveXML Requirements
@include requirements
*/
