/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

   \mainpage

This package contains a few tools to help writing unit tests. 

\section UnitTest_run The UnitTest_run cmt pattern

TestTools requirements file defines the cmt pattern UnitTest_run. UnitTest_run 
will compile, link and run a standalone C++ program when a "gmake check" 
command is issued. It will then run the share/post.sh script to compare the 
program output with a reference one, if available.
 The pattern takes one parameter <unit_test> which is used to identify a number of files:
 -# $PKGROOT/test/<unit_test>_test.cxx		C++ program to be run
 -# $PKGROOT/share/<unit_test>_test.ref		optional reference output
 -# $PKGROOT/run/<unit_test>_test.log		program output (stdout & stderr)
 -# $PKGROOT/$CMTCONFIG/<unit_test>_test.exe	executable

So for example 
<PRE>
  apply_pattern UnitTest_run unit_test=DataPool
will compile and link 
 ../test/DataPool_test.cxx 
into 
 ../$CMTCONFIG/DataPool_test.exe 
which will be run and produce 
 ../run/DataPool_test.log 
If you have created the reference output
 ../share/DataPool_test.ref
this will be compared to the log file at the end of the job.
</PRE>

Notice that the comparison tries to ignore a certain 
number of differences that are usually harmless (e.g. the execution time 
reported by ChronoStatSvc or the package versions and even in certain cases
pointer addresses). This is currently done in a very naive fashion (using 
diff -I option) but a more sophisticated "diff" script is in the plans

\section initGaudi The initGaudi functions
TestTools/initGaudi.h defines two functions in the namespace Athena_test 
to initialize Gaudi ApplicationMgr 
and be able to run using core Gaudi services. An optional string argument
<jobOptsFile> instructs initGaudi to read in the job options file
 $PKGROOT/share/<jobOptsFile>
to configure your job

\section scripts Scripts
 - share/runUnitTests.sh is a sh script that cmt broadcasts gmake check
 and filter its output. It is 
installed in the run area. It accepts one or more arguments that it passes
to cmt broadcast, for example
<PRE>
 ../run/%runUnitTests.sh -select=StoreGate
</PRE>
 - share/post.sh is a script used by the UnitTest_run pattern to 
analize a job output 

\section toys Toys 

The TestTools component library provides toy implementations of a number of
typical Gaudi classes, namely ToyConverter, ToyConversionSvc (and soon 
ToyAlgorithm). These are made available via the job opts file
 $TESTTOOLSROOT/share/ToyConversionOpts.txt



\section Examples Examples

The package AthenaTests/ControlTests contains several examples that use
the initGaudi function.
Control/StoreGate has a couple of non-Gaudi-based,
very simple unit tests (e.g. KeyConcept) as well as more sophisticated ones
that show 
 - how to link the test program with an extra library (e.g. Clear_Store) 
 - how to use the ToyConversionSvc provided by TestTools (ProxyProviderSvc)

\section links More info
The package can be browsed using LXR 
(http://atlassw1.phy.bnl.gov/lxr/source/atlas/AtlasTest/TestTools/)

To generate doxygen doc, run (from the cmt dir) gmake doxygen and point 
your browser to .../doc/Doxygen/html/index.html

\author Paolo Calafiura <Paolo.Calafiura@cern.ch>
*/
