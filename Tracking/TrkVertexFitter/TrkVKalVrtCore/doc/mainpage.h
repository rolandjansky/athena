/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
@mainpage TrkVKalVrtCore Package

Vertex fitting engine and some service routines of VKalVrt. 

@author Vadim.Kostyukhin@cern.ch

@section TrkVKalVrtCoreIntro Introduction

This package contains vertex fitting engine and some service routines
of VKalVrt. Package is independent on ATLAS software and on any external library.
External magnetic field and propagator may be plugged in. 
Neutral and charged tracks may be mixed together. 

@section TrkVKalVrtCoreOverview Class Overview


   Package does vertex fitting with constraints. Tracks are transported to 
   vertex position and linearized there. 
  
 Some description of algorithms may be found in http://kostyuk.home.cern.ch/kostyuk/vertex/ and
 references inside.
 Software related issues may be viewed in
 https://twiki.cern.ch/twiki/bin/view/Atlas/VKalVrt .
  
@ref used_TrkVKalVrtCore

@ref requirements_TrkVKalVrtCore

*/

/**
@page used_TrkVKalVrtCore Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_TrkVKalVrtCore Requirements
@include requirements
*/

