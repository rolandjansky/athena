*************************************

Author : Yi Yang <yi.yang@cern.ch> 

*************************************

Codes needed : 
  (1) BTrigger.C   ( ***** No need to change anything here!!!!! *****)
  (2) MakeHistograms.py
  (3) MakePlots.py

Step : 

  (1) Get the standard Onia-ntuple (event-based) from Athena : Onia.c++ in "JpsiUpsilonAlgs"
 
  (2) Setting MakeHistograms.py
      * Basic setting : GRLs ( no need for MC, and probably no need for official onia-ntuple )

                       ( if want to apply GRL selection, need to get the package, please follow: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/GoodRunsListsTutorial#Stand_alone_compilation_and_use )
                        
                        If you don't have GRL package, you need to comment out few lines in BTrigger.C code (search GRLs).
      
      * Name of output file 
    
      * Read file from directory "Director = True" ( loop over all *root* files in the directory ) 
        or specify the input files "Director = False"
      
      * Specify the triggers to be the denominator for efficiency calculation 
        => denominators += [ "xxx" ]
      
      * Specify the triggers for numerators 
        => chains += [ "yyy" ] 
      
      * Setting the Selection cuts : SCT, Pixel, TRT, Combined Muon, pT, eta

  (3) Run MakeHistograms.py to create histograms
      => python MakeHistograms.py
        
  (4) Modify MakePlots.py
      * Load ATLAS style ? If True, need ATLAS style files in the working directory

      * Make plots or not ?

      * Automatically save the plots ? If True, give the path. 

      * Specify the signal type : 
        * J/psi   : Signal = "Jpsi"
        * Upsilon : Signal = "Upsi"
        * Bmumu   : Signal = "Bmumu"

      * Specify the input files : ( treat different for MC and Daata )
        * Don't forget give the "name" of input file
     
      * Specify the denominator 

      * Specify the chains want to run over

   (5) Run MakPlots.py
       => python MakPlots.py

       ***** The efficiencies will print out on the screen ******
        


