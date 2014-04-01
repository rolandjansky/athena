print 'Loading default InDetAlignExample_CosmicFlags'
include.block("InDetAlignExample/InDetAlignExample_CosmicFlags.py")

# Default options 
class InDetAlignExampleFlags:

    # -------------------------------------
    # Setup
    # -------------------------------------
    Iteration = 0
  
    # Number of events to be processed
    EvtMax = 50
    SkipEvents = 0

    challenge  = 'M8plus'
    # challenge = 'CSC'
    # challenge = 'CosmicsRel14'
    
    # Geometry Description
    # - True :: Use Nominal Silicon Geometry (misalignment unknown)
    # - False :: Use Misaligned Silicon Geometry (misalignment known)
    SiNominal  = True
    TRTNominal = False
    Aligned = False
    
    # Read Alignment Constants (if True, SiNominal flag will be ignored)
    ReadAlignmentConstants = False
    AlignConstsPath        = os.environ['PWD']
    WritePrefixName        = "Iter0_"
    ReadPrefixName         = "Iter0_"

    # Beam spot
    doReadBeamSpot   = False
    doBeamSpotReader = False

    # TRT Constraint (RA)
    doTRTConstraint = False

    # Error Scaling
    ErrorScalingFromFile = True
    
    # -------------------------------------
    # Input Files    
    # -------------------------------------
    Files = ["/afs/cern.ch/atlas/maxidisk/d158/alignment/InDetESD_90413_3.root"]                                    # Test file 
    # Files = ["castor:/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/InDetESD_90413_3.root"]                                  # M8plus cosmics with BField
    # Files = ["castor:/castor/cern.ch/user/l/lytken/cosmic_13010/digitization/NoField/TRTBarrel/misaligned/dig.trtbarrel_ma100.root"]     # CSC Cosmics without BField
    # Files = ["castor:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-simcos/rel14/ATLAS-GEO-03-00-00/dig/dig.GEO-03-TRTBarrel.1.20.root"] # CosmicsR14 cosmics with BField
    

    Magnet          =  True
    # Magnet          = False

    Cosmics         =  True
    BeamHalo        =  False
    BeamGas         =  False
    
    # -------------------------------------
    # Reconstruct data
    # -------------------------------------
    onlyPixel  = False
    onlySCT    = False
    doTRT    = False


    # -------------------------------------
    # Output stream
    # -------------------------------------
    OutputPath = os.environ['PWD']+"/"
    
    # -------------------------------------
    # Cog Flags
    # -------------------------------------
    InputConstants     = 'Iter0_AlignmentConstants.root'
    OutputConstants    = 'Cog_AlignmentConstants.root'
    OutputDbConstants  = 'Cog_AlignmentConstants.db'
    OutputTxtConstants = 'Cog_AlignmentConstants.txt'
    OnlySCTCog         = True
        
    # -------------------------------------
    # Alignment Algorithms 
    # -------------------------------------
    
    # --- GlobalChi2 alignment
    doGlobalChi2Align       = True
    if doGlobalChi2Align:
        AlignLevel = 1
        SolveOption         = 0 # None=0, Solve=1, SolveFast=2, DirectSolve=3, DirectSolveFast=4, DirectSolveCluster=5

   
    # --- Local chi2 alignment
    doLocalChi2Align        = False
    
    # --- Robust alignment 
    doRobustAlign           = False
    if doRobustAlign:
        SolveOption             = 0
        SetTextFileWriteIndex   = 1
        SetTextFileReadEndIndex = 1
    
    # -------------------------------------
    # EOF
    # -------------------------------------


