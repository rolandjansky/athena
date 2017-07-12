/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TileSimEx Package
 
This package contains examples of jobOptions for TileCal standalone simulation and digitization. Also an example of Tile digitization for the full ATLAS set-up is shown.

After the TB simulation job POOL file with hits is created, after digitization job (TB or ATLAS setup) two files are created: POOL file with digits and ROOT file with h32 hit ntuple and with usual h1000 TileCal testbeam ntuple. 

@author Alexander Solodkov <Sanya.Solodkov@cern.ch>

@tableofcontents

@section TileSimExJobOptions JobOptions parameters

jobOptions_TileTB_Sim.py:

  - EvtMax

      Number of events to generate
  - FileSuffix

    Suffix for output files, default is empty suffix and name of output file is tiletb.HITS.pool.root
  - Geo

      Three Options: '2B2EB' for two barrel modules and two extended barrel modules, '3B' for three barrel modules or '5B' for five barrel modules. 
  - Eta

       Use only for eta-projective scans. This is the default option with eta set to 0.35. 
  - Theta

       Angle between beam axis and symmetry axis of the module (eta=0.0 direction). Can be used instead of Eta. 
       Positive value of Theta corresponds to negative eta direction.

       Set to +/- 90 for beam entering the side of the module (with positive theta beam entering the positive eta side). 
  - Z

       For small Theta angles - coordiate at the front face of the calorimeter where beam eters the module (in mm)

       For +/-90 degrees scan Z is the distance from the center of ATLAS (in mm). The sensitive part of the detector start at Z=2300 and ends at Z = 2300 + 3*100 + 3*130 + 3*150 + 2*190 = 3820.

       Center of the 2nd tile is at Z = 2300 + 100 + 100/2 = 2450;

       center of the 5th tile is at Z = 2300 + 3*100 + 130 + 130/2 = 2795;

       center of the 10th tile is at Z= 2300 + 3*100 + 3*130 + 3*150 + 190/2 = 3535 
  - Y

       Y is the vertical distance (in mm) from the center of the middle module (above > 0, below < 0). 
  - Phi

       To rotate the table to top/bottom module (default is middle). 

  - PID

       Set the particle type according to its PDG code (e.g 11 - electrons, 211 - positive pions). 
  - E

       Set the beam energy (in MeV). 
  - Ybeam

       Set the beam coordinates in Y, by default [-20,20] - uniform distribution from -20 to 20 mm
  - Zbeam

       Set the beam coordinates in Z, by default [-15,15] - uniform distribution from -15 to 15 mm

  - TileUshape
      
      Set up U-shape to be used

jobOptions_Tile_Dig.py

  - TileTB

    If True, digitization of testbeam configuration,  otherwise digitization of full ATLAS

  - FileSuffix

    Suffix for input and all output files, default is empty suffix

  - PoolHitsInput

    Input file name with hits (by default: HITS.pool.root or tiletb.HITS.pool.root if TileTB=True)

  - PoolRDOOutput

    Output file name (by default: DIGITS.pool.root or tiletb.DIGITS.pool.root if TileTB=True)

  - D3PDOutput

    Output D3PD file name (by default: tile.d3pd.root or tiletb.d3pd.root if TileTB=True)

  - AANTOutput

    Output file name for h1000/h2000 ntuple (by default: tile.aant.root or tiletb.aant.root if TileTB=True)

  - NTUPOutput

    Output file name for h32 ntuple with hits (by default: tile.ntup.root or tiletb.ntup.root if TileTB=True)

  - DetGeo

    By default 'atlas' or 'ctbh8' if TileTB=True

  - DetDescrVersion

    Detector description version

  - ConddbTag

    Global conditions tag

  - doD3PD

    Produce D3PD

  - doD3PDHitInfo

    Save hit information into D3PD

  - doD3PDHit

    Save hits into D3PD

  - doD3PDCellInfo

    Save cell info into D3PD

  - doD3PDCell

    Save cells into D3PD

  - doD3PDDigit

    Save digits into D3PD

  - doD3PDRawChannel

    Save raw channels into D3PD

  - doD3PDMBTS

    Save MBTS into D3PD

  - doTileNtuple

    Create TileCal h2000 ntuple with raw channels

  - doHitNtuple

    Create TileCal h32 ntuple with all G4 hits

  - doRawChannelNtuple


@section SamplingFraction How to extract the sampling fraction value

 -  Running simulation in Athena

 -  Digitization, ntuple making in Athena

 -  Calculating sampling fraction in ROOT


@subsection SamplingFractionSimulation Simulation:

   jobOptions_TileTB_Sim.py:

   - test beam setup with 5 barrels, 100 GeV electrons at eta = 0.35

   - FTFP_BERT Physics List (used by default)

   - 10000 or 20000 events

   - TileUshape=1 
    
       U-shape table to be used (change to the required value)

   - Output file with HITS: 
 
       tiletb.HITS.pool.root

  - Example:

      athena.py -c 'EvtMax=10000; Eta=0.35; TileUshape=1;' TileSimEx/jobOptions_TileTB_Sim.py

@subsection SamplingFractionDigitization Digitization & Ntuple creation

jobOptions_Tile_Dig.py

  - Input file (HITS file from simulation step):
  
     tiletb.HITS.pool.root

  - TileTB = True:
    
    Set up digitization to produce ntuples needed for sampling fraction calculation

  - Output files (both files needed):

      tiletb.d3pd.root with tree called "truth" - truth information about particles

      tiletb.ntup.root with tree called "h32" - hit information

  - Example:

      athena.py -c 'TileTB=True;' TileSimEx/jobOptions_Tile_Dig.py

@subsection SamplingFractionCalculation Sampling fraction calculation

  - tile_sf.C

    - Input files (produced in digitization step):

        tiletb.ntup.root

        tiletb.d3pd.root

    - one should check the file names file_hit and file_truth in ROOT macro

  - Example ( 1/SF value on the last line of the print-out):

      root -l tile_sf.C("")


*/
