=====================================================
MuonDQAHistPlotSHIFTER.cc --- Ioannis.Nomidis@cern.ch
=====================================================
Root macro useful to shifter. It prints selected offline monitoring histograms from all subsystems+tracks+segments to a PDF.
In case of need to modify macro (add/remove histograms, ordering, visibility, errors), 
contact Nectarios.Benekos@cern.ch or Ioannis.Nomidis@cern.ch.

To execute, in Root command line enter:

       .x MuonDQAHistPlotSHIFTER.cc("myfilename.root")

Mind that another macro (MuonDQAHistPlot) is also available for the same purpose, 
which can print almost ALL histograms in an offline muon monitoring root file; useful to experts!
