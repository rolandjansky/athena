/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@mainpage RecExConfig Package

@section IntroductionRecExConfig Introduction

This package holds a number of simple python modules which are used to configure Reconstruction. 

@section OverviewRecExConfig Class Overview
 The RecExConfig package containes the following python modules:
   - python::RecFlags holds reconstruction specific flags (see <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/RecExCommonFlags#RecFlags">the following twiki</a> ). 
   - python::RecAlgsFlags holds reconstruction algorithm switches (see <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/RecExCommonFlags#RecAlgsFlags">the following twiki</a> ).
   - python::Configured is the base class for python "Configured" module for configuration of reconstruction algorithms (see <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/RecConfigured">the following twiki</a> ).
   - python::ObjKeyStore is a python module holding the list of object and keys in input files, outpfiles, detector and transient event store
(see <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/RecConfigured#ObjKeyStore">the following twiki</a> ).
   - python::testObjKeyStore is a simple python module to exercise ObjKeyStore

*/
