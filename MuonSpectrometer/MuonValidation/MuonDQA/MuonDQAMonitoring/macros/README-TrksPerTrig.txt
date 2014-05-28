===============================================================================
MuonDQATrksPerTrig.cc ------------- Ioannis.Nomidis@cern.ch (AUTh-Thessaloniki)
===============================================================================
Root macro to see number of reconstructed tracks for various trigger streams
...............................................................................

*) Edit streams.txt entering keywords of trigger streams and end with new line.

*) Run findfiles.sh script, which searches for the relative root HIST files in CASTOR 
   and makes a list of them in 'streamsfiles.txt'. 
   Script takes the run number as input like this:
   ./findfiles.sh 0090272

*) In root command line, enter:
   .x MuonDQATrksPerTrig.cc("90272")
