/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TileCalibAlgs_page TileCalibAlgs

This package provides algorithms for calculating calibration constants from calibration runs or from the data extracted from database

@author Ximo Poveda Torres <Joaquin.Poveda@cern.ch>
@author Karl Gellerstedt <Karl.Gellerstedt@cern.ch>
@author Nils Gollub <Nils.Gollub@cern.ch>
@author Luca Fiorini <Luca.Fiorini@cern.ch>
@author Andrei Artamonov <andrei.artamonov@cern.ch>

@section TileCalibAlgs_Package_Structure Package Structure

Different Algorithm Tools are provided for calculating calibration
constants related to the Charge
Injection System (TileCISCalibTool, TileMonoCISCalibTool).
All these tools derive from a common TileCondTool
that provides basic infrastructure shared by all the specialized
tools.

A subset of events needed as input for the different computing tools
can be preselected with the TileCalibEventSelectionAlg.

The Noise Calibration constants are produced by TileDigiNoiseCalibAlg and  
TileRawChNoiseCalibAlg. The first one extracts the noise constants from 
Digits (high frequency noise, low frequency noise, pedestals, ...) and the 
latter extract the noise constants from the various RawChannels containers
(average, mean, RMS and sigma of the amplitude distribution).

Algorithm TileOFC2DBAlg is used to calculate Optimal Filtering Constants 
and store them in sqlite file for further merging to COOL database.
It uses TileCondToolOfc which gets A/C matrix and pulse shapes from
database and calculated OFCs. The are two modes in TileOFC2DBAlg.
The first one: calculate OFCs for the only, default, channel 
for different phases
(usually in the window +/- 100 ns, 1 ns step). This kind of OFCs
are used later in on-line (DSP) reconstruction and, optionally,
in TileRawChannelBuilderOpt2Filter. The second mode: calculate OFCs
for every channel for the "best phase", extracted from database for
particular channel (not yet fully debugged at the moment).
The choice of the mode is made in joboptions. Example of joboptions
to use this algorithm is in jobOptions_OFC2DBTest.py.

Algorithm TileTriggerDefaultCalibTool is used for finding bad trigger towers. 
This algorithm runs on special calibration runs taken with L1Calo. The trigger
tower energy from L1Calo is used to determine if a given PMT or tower in Tile 
does not have the correct energy. 

@section TileCalibAlgs_Package_Configuration Package Configuration

The package configuration is done through old-style job options, need
to move to configurables.


@todo 
Most of the algorithms need to be updated to the latest best
knowledge of calibration constant calculation. While doing this more
detailed documentation will be provided. For the current status of the
package a detailed documentation would not make much sense.



 

*/
