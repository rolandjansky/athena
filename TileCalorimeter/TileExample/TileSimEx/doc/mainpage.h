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

  - SimFlags.SimLayout

      Three Options: tb_Tile2000_2003_2B2EB for two barrel modules and two extended barrel modules, tb_Tile2000_2003_3B for three barrel modules or tb_Tile2000_2003_5B for five barrel modules. 
  - SimFlags.Eta

       Use only for eta-projective scans. This is the default option with eta set to 0.35. 
  - SimFlags.Phi

       To rotate the table to top/bottom module (default is middle). 
  - SimFlags.Y

       Y is the vertical distance (in mm) from the center of the middle module (above > 0, below < 0). 
  - SimFlags.Theta

       Set to +/- 90 for beam entering the side of the module (with positive theta beam entering the positive eta side). 
  - SimFlags.Z

       for +/-90 degrees scan Z is the distance from the center of ATLAS (in mm). The sensitive part of the detector start at Z=2300 and ends at Z = 2300 + 3*100 + 3*130 + 3*150 + 2*190 = 3820.

       Center of the 2nd tile is at Z = 2300 + 100 + 100/2 = 2450;

       center of the 5th tile is at Z = 2300 + 3*100 + 130 + 130/2 = 2795;

       center of the 10th tile is at Z= 2300 + 3*100 + 3*130 + 3*150 + 190/2 = 3535 

  - SimFlags.ParticlePDG

       Set the particle type according to its PDG code (e.g 11 - electrons, 211 - positive pions). 
  - SimFlags.Energy

       Set the beam energy (in MeV). 

  - TileUshape
      
      Set up U-shape to be used

jobOptions_TileTB_Dig.py

  - PoolHitsInput

    Input file name with hits (by default: HITS.pool.root)

  - PoolRDOOutput

    Output file name (by default: DIGITS.pool.root)

  - D3PDOutput

    Output D3PD file name (by default: tile_d3pd.root)

  - DetDescrVersion

    Detector description version

  - DetGeo

    By default 'atlas'

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

  - TileTB

    Set up digitization for sampling fraction calculation

  - doHitNtuple

    Create TileCal h32 ntuple with all G4 hits

  - doRawChannelNtuple


jobOptions_TileTB_Dig.py

  - jobproperties.Global.DetDescrVersion

      Set the same geometry tag as was used during the simulation (e.g GEO-06-00-00).
  - doCBNT

      Set doCBNT to True if you would like to store MC truth information in the ntuple. Hits information will be also saved, but the h1000 ntuple will not be created in this case.


@section SamplingFraction How to extract the sampling fraction value

 - Running simulations in Athena

 -  Digitization (Ushape), ntuple making in Athena

 -  Calculating sampling fraction in Root


@subsection SamplingFractionSimulation Simulation:

   jobOptions_TileTB_Sim.py:

   - test beam setup with 5 barrels, electrons at 100 GeV, eta = 0.35

   - FTFP_BERT Physics List

   - 10000 events

   - TileUshape=1 
    
       U-shape table to be used (change to the required value)

   - Output file with HITS: 
 
       tile.hits.pool.root

  - Example:

      athena.py -c 'TileUshape=1;' jobOptions_TileTB_Sim.py

@subsection SamplingFractionDigitization Digitization & Ntuple creation

jobOptions_Tile_Dig.py

  - Input file (HITS file from simulation step):
  
     tile.hits.pool.root

  - TileTB = True:
    
    Set up digitization to produce ntuples needed for sampling fraction calculation

  - Output files (both ntuples needed):

      tile_d3pd.root with tree called "truth" - truth information about particles

      tile_ntup.root with tree called "h32" - hit information

  - Example:

      athena.py -c 'TileTB=True;' jobOptions_Tile_Dig.py

@subsection SamplingFractionCalculation Sampling fraction calculation

  - sf_eta035.C

    - Input files (produced in digitization step):

        tile_ntup.root

        tile_d3pd.root

    - one should check the file names file_hit and file_truth in ROOT macro

  - Example ( 1/SF value on the last line of the print-out):

      root -l sf_eta035.C


*/
