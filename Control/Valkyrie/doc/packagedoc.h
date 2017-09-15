/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page Valkyrie_page Valkyrie

This package contains several classes that facilitate the use of valgrind tools
within the Atlas software. The following sections describe different use cases.
General information about using valgrind in Atlas can be found on the
<a href="https://twiki.cern.ch/twiki/bin/view/Atlas/UsingValgrind">UsingValgrind</a> TWiki.

@section Valkyrie_ValkyrieAlgProf Algorithm profiling using callgrind

Since code profilers like %callgrind collect a huge amount of data and since you are most likely only
interested in your particular algorithm it is not a good idea to simply run the profiler on an athena job.
Using ValgrindSvc it is possible to profile specific algorithms only.
Add the following to your job options

<pre>
  from Valkyrie.JobOptCfg import ValgrindSvc
  svcMgr += ValgrindSvc( OutputLevel = DEBUG,
                         ProfiledAlgs = ["HelloWorld"] )
</pre>

and run the athena job within valgrind:

<pre>
  valgrind --tool=callgrind --trace-children=yes --instr-atstart=no `which athena.py` HelloWorldOptions.py
</pre>

Add your other favorite valgrind options but make sure that the instrumentation at start is turned off,
since ValgrindSvc will take care of enabling it at the right time.
The algorithm names in <tt>ProfiledAlgs</tt> can contain regular expressions (Perl synatx) to match
several instances of one algorithm. For example,

<pre>
   ProfiledAlgs = ["TrigIDSCAN_.*"]
</pre>
will profile all algorithms matching this regular expression. If <tt>ProfiledAlgs</tt> is empty (the
default) the entire event loop, i.e. everything between BeginEvent/EndEvent, will be profiled.


@section Valkyrie_ValkyrieIntervalProf Profiling of arbitrary intervals
It is possible to profile arbitrary intervals in the lifetime of a Gaudi component.
For example, you may want to profile only the initialize method of your algorithms,
or anything that is executed between initialize and finalize, or anything in between the initialize of
two algorithms. You can use the <tt>ProfiledIntervals</tt> property for this. Examples:

\li <tt>ProfiledIntervals = ["MyAlg.initialize:MyAlg.finalize"]</tt><br>
Profile between initialize and finalize of MyAlg
\li <tt>ProfiledIntervals = ["MyAlg.initialize:YourAlg.initialize"]</tt><br>
Profile between initialize of MyAlg and initialize of YourAlg
\li <tt>ProfiledIntervals = ["MyAlg.initialize:xxx.yyy"]</tt><br>
Start profiling at initialize of MyAlg and never stop (becaues xxx.yyy does not exist)
\li <tt>ProfiledIntervals = ["MyAlg.initialize"]</tt><br>
Short for <tt>["MyAlg.initialize:MyAlg.initialize"]</tt>, i.e. profile initialize of MyAlg
\li <tt>ProfiledIntervals = ["MyAlg.initialize:YourAlg.initialize", "MyAlg.finalize"]</tt><br>
Any number of (non-overlapping) intervals can be specified

As above, the algorithm name can contain a regular expression:
\li <tt>ProfiledIntervals = ["TrigIDSCAN_.*.initialize"]</tt><br>
Profile initialize of all algorithms that match the expression. Be careful with nested initialize
calls since the profiling will stop once the first instance is done with initialize.

Finally, a special notation can be used to steer the profiling based on incidents (no regular
expressions allowed):
\li <tt>ProfiledIntervals = ["BeginRun.incident:EndRun.incident"]</tt><br>
Profile between the BeginRun and EndRun incident.

In all cases the intervals are closed intervals in the mathematical sense, i.e. the profile always
includes the start and end point.

@section Valkyrie_ValkyrieDump Properties to control profile dumps
The following properties can be set to control whether a new profile is being dumped after
each event and interval, respectively. The default values are:
<pre>
  DumpAfterEachEvent = False
  DumpAfterEachInterval = True
</pre>
Additonal profiles can be dumped on any "Incident" by setting:
<pre>
  DumpAfterIncident = ["BeginRun","EndRun","MyIncident"]
</pre>



@author
    Sebastien Binet http://consult.cern.ch/xwho/people/607900 <br>
    Frank Winklmeier http://consult.cern.ch/xwho/people/660298

    
@section Valkyrie_ValkyrieExtraPages Extra pages

*/
