This directory contains a script for making time dependent plots for the CBNT output of the monitoring package. 

Running the strippedDown.py job option in the share directory makes a file called SCT_CBNT.root. For each event this contains:

1) The event number
2) Arrays containing the eta, phi, barrelec, side and layer of wafers
3) An array containing the hits in wafers

Therefore to plot histograms, you can for each event loop over the array contents and extract the position of each element in the array and the number of hits.

To use the script, start ROOT then type the following:

.L SCTTimeHists.C++
SCTTimeHists t;
t.Loop()

This will produce a list of histograms ordered by wafer hash that you can plot. Those that feature hits will have the title changed so that you can tell where the module is. The next stage should be to add an automatic checker for problems.

