/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page xAODEventFormatCnv_page xAODEventFormatCnv package

   @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

   $Revision: 578438 $
   $Date: 2014-01-15 10:34:19 +0100 (Wed, 15 Jan 2014) $

@section xAODEventFormatCnv_xAODEventFormatCnvOverview Overview

   This package holds the code that creates, and in general handles the
   xAOD::EventFormat metadata in Athena.

   While some functionality is accessible to the general user, the code
   is expected to be used mostly by RecExCommon.

@section xAODEventFormatCnv_xAODEventFormatCnvClasses Classes

   The package provides a simple interface, xAODMaker::IEventFormatSvc.
   It can be used to interact with the service implemented in the
   package.

   The package also holds a single implementation for the interface,
   xAODMaker::EventFormatSvc. It is meant to be instantiated in jobs
   handling xAOD files with something like:

   <code>
   theApp.CreateSvc += [ "xAODMaker::EventFormatSvc" ]<br/>
   xaodStream.AddMetaDataItem( "xAOD::EventFormat_v1#EventFormat" )
   </code>


*/
