/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage InDetPhysValMonitoring Package
@author shaun.roe@cern.ch

@section IDPVMIntro Introduction

This package was originally intended to be a straight translation of the previous 
InDetPerformanceRTT, for the period when the input data format was updated to xAOD.
The older package, however, was a monolithic class which proved difficult to maintain
and upgrade. Thus the algorithms were refactored and became the current package.

@section IDPVMPhilosophy Philosophy
The functions have been divided into components as follows:
1. 'Decorating' Algorithm: 
This should calculate the required values and add them as 'decorations' on the xAOD
container, each value (or set of values) is keyed on a string name. This Algorithm is
independent of other tools in the package but must be run prior to calling them in 
order to ensure that the quantities exist before being plotted.

2.Plotting tool:
The plotting tool exists in the monitoring framework and contains the main control for
retrieving the data structure, looping over the tracks (truth/reconstructed) while 
performing a selection and then calling the filling methods on the plots.

3. Plots:
The plots are intended to be as 'dumb' as possible; ideally they should simply retrieve
the pre-existing named quantity and insert this into the histogram. All the plots in this
package should inherit from InDetPlotBase, which in turn inherits from PlotBase in 
TrkValHistUtils. This base class implements a container/leaf concept which mimics that of
Root, so a 'plot' may in fact simply be a container of other plots and delegate filling to them.
One example of this is the InDetRttPlots in this package.

The InDetPlotBase class adds additional infrastructure: access to Athena messaging (ATH_MSG), 
the facility to define plot limits and titles in an independent text file, easier histogram 
booking interface.

4. Histogram definition service:
The HistogramDefinitionService class enables plot characteristics to be read from plain text or xml
files, examples of which are in the package, in 'share'. In turn this allows jobs to be run with different plot
definitions without recompilation each time.

@section IDPVMQuickStart Quick Start
From your Athena directory:
mkdir build source run
cd source
asetup here, dev, rel_5, cmake
svnco InDetPhysValMonitoring
cd ../build
cmake ../source
make -j4
cd ..
ln -s $TestArea/InnerDetector/InDetValidation/InDetPhysValMonitoring/share .
cd run
source ../build/x86_64-slc6-gcc49-opt/setup.sh
athena $TestArea/InnerDetector/InDetValidation/InDetPhysValMonitoring/run/PhysVal_jobOptions.py
**/
