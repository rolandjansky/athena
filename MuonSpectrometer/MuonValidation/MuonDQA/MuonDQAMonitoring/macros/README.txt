////////////////////////////////////////////////////////////////////////////////////////////
I. Nomidis, N. Benekos -  Feb. 2008 
////////////////////////////////////////////////////////////////////////////////////////////

### For MuonDQAHistPlot #######
#
# For help, type ./runDqaMacro in command line
#
### For MuonDQAShiftHistPlot #######
###################################################################################################
...................................(MuonDQAShiftHistPlot.cc)............................

To execute this ROOT macro ), enter in ROOT command line:

  .x MuonDQAShiftHistPlot.cc("fileName","runNumber",Sectors,
				 doRawDataMon,doTrackMon,doSegmMon,    
                                 doMBoyTrack,doMooreTrack,
				 doMBoySegm,doMoMuSegm)                           
NOTES:
 -> Do not omit quotes("").                                                                     
 -> Sector is a vector<TString>. Initialize it like this:
    (in ROOT command line):     vector<TString> Sectors
    and enter all the numbers of the sectors you want to get the MDT-RPC correlation histos 
    like this:
    (in ROOT command line):	Sectors.push_back("5")
    (in ROOT command line):	Sectors.puch_back("6")
    In this case histos of sectors 5+6 will be extracted. If you don't want such histos, just
    set the flag to NULL.
 -> doSomething are boolean flags, use true or false to enable/disable or just omit them and    
    get default values (true).                                                                  
 -> doMBoyTrack and doMooreTrack flags enable display of results for each reconstruction algo.  
 -> doMBoySegm and doMoMuSegm flags do the same for the 2 segment reco algorithms.              
 -> If data for one of those algorithms do not exist, user should better disable its display.   
###################################################################################################

