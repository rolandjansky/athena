//usr/bin/env LD_PRELOAD="$LD_PRELOAD libGaudiKernel.so libAthenaServices.so libHepMC_base.so" root -b $0 $@; exit $?

// File: athena.C
// Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

// Note: this is written for PyROOT distributed with SEAL 1.4.2.
// Usage: athena.C <file1>.py

{

// get input file
   int argc = gApplication->Argc();
   if ( argc != 5 && argc != 6 )
   {
      printf( "usage: athena.C [-b] <file1>.py\n" );
      return;
   }

   bool runBatch = argc == 6 && (strcmp(gApplication->Argv()[4],"-b")==0);
   char* optf = gApplication->Argv()[argc-1];

// enter PyROOT
   gSystem->Load( "libPyROOT" );

// import basic python modules (used throughout)
   TPython::Exec( "import sys, os" );

// logging for Athena; not explicitly used
   TPython::Exec( "from AthenaCommon.Logging import *" );

// default settings
   TPython::Exec( "doDebug = ( '', 0 ); runBatch = 1; showincludes = 0" );

// implementation of the include function
   TPython::Exec( "from AthenaCommon.Include import Include" );
   TPython::Exec( "include = Include( showincludes )" );

// Gaudi bindings + compatibility
   TPython::Exec( "from gaudimodule import *" );
   TPython::Exec( "from AthenaCommon.Configurable import *" );
   TPython::Exec( "from AthenaCommon.OldStyleConfig import *" );

// some constants to ease end-user coding
   TPython::Exec( "from AthenaCommon.Constants import *" );

// Bootstrap for normal Athena; this is different for HLT, pileup etc.
   TPython::Exec( "theApp = iService( \"ApplicationMgr\" )" );
   TPython::Exec( "include( \"AthenaCommon/Bootstrap.py\" )"  );

// instantiate the application manager and setup message service
   TPython::Exec( "from AthenaCommon.AppMgr import theApp, ToolSvc, ServiceMgr" );

// pythonization of services
   TPython::Exec( "from AthenaCommon.ServicesPythonize import *" );

// prepare standard options and prevent them from being doubly included
   TPython::Exec( "include( \"AthenaCommon/Atlas.UnixStandardJob.py\" )" );
   TPython::Exec( "include.block( \"AthenaCommon/Atlas.UnixStandardJob.py\" )" );

// some options file
   TString cmd = TString( "include( \'" ) + optf + TString( "\' )" );
   TPython::Exec( cmd.Data() );

   if ( runBatch )
   {
   // execute run and exit
      TPython::Exec( "theApp.run()" );
      TPython::Exec( "theApp._appmgr.finalize(); theApp._appmgr.terminate()" );
      gSystem->Exit( 0 );
   }

// still here? give access to application manager and drop into interactive mode
   (void*)TPython::Eval( "theApp" );        // tickle class creation
   AthAppMgr* theApp = (void*)TPython::Eval( "theApp" );

}
