/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage
@author John Baines
@author Tomasz Bold

@section MyPackageOverview  Introducion
This package is defining utilities to be used within trigger community to measure times of execution.
It contains basically 2 ingredients: <br/>

1) TrigTimer - which is defining objects used to measure time intervals <br/>
  Users will get object of this type and can do on it start() stop() operation
  in the algorithms code. The period spent between start() and stop() 
  is given by return value of elapsed(). Actually the elapsed() is called by external tools 
  like TrigTimeHistTool and TrigTimeTreeTool to record them in different forms.
  The TrigTimer also by itself accumulates measured intervals and keeps statistical properties of 
  this measurements. (average, RMS, number of measurements).
  The timers can be created in dummy state then call to start() stop() is faster.
  <br/>

2) ITrigTimerSvc - which is to be used to acquire the TrigTimers <br/>
  At the moment direct use of this service is not recommended when timers are needed directly in the HLT algorithms.
  For that purpose HLT::Algo implements method addTimer().<br/>
  The package does not proved storage of timing information besides pretty printout in the log.
  For that look at: TrigTimeMonitor package. The implementation of that interface, TrigTimerSvc besides implementing 
  this interface is adding some configuration possibility to enable/disable certain timers.



For the whole picture look at: <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/TrigTimerSvc"> TWIKI page explaining usage of all timing related utilities. </a>


@htmlinclude used_packages.html


*/
