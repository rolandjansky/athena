
print 'Loading default InDetAlignExample_Flags'
include.block("InDetAlignExample/InDetAlignExample_Flags.py")

# Default options 
class InDetAlignExampleFlags:

    # -------------------------------------
    # Setup
    # -------------------------------------
    Iteration = 0
  
    # Number of events to be processed
    EvtMax = 50
    SkipEvents = 0

    #challenge = 'ID_FDR_09'
    challenge = 'MC09'
    #challenge = 'CSC'
    #challenge = 'FDR1'
    #challenge = 'FDR2'
    
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
    #doBeamSpotReader = False

    # Si+TRT Aligned (if True, SiNominal and ReadAlignmentConstants flags will be ignored)

    # Error Scaling
    ErrorScalingFromFile = False
    
    # TRT Constraint (RA)
    doTRTConstraint = False

    # -------------------------------------
    # Input Files    
    # -------------------------------------
    Files = ["castor:/castor/cern.ch/grid/atlas/atlasgroupdisk/perf-idtracking/dq2/mc09_valid/ESD/e436_s561_r731/mc09_valid.107271.Multimuons_pt9.recon.ESD.e436_s561_r731_tid076491/ESD.076491._000155.pool.root.1"]
    #Files = ["/afs/cern.ch/atlas/maxidisk/d158/alignment/misal1_mc12.007270.MultiMuonsGeneration.digit.RDO.v12003102_tid003678._00001.pool.root.2"]
    #    Files = ["castor:/castor/cern.ch/grid/atlas/dq2/misal1_mc12/misal1_mc12.007270.MultiMuonsGeneration.digit.RDO.v12003102_tid003678/misal1_mc12.007270.MultiMuonsGeneration.digit.RDO.v12003102_tid003678._00001.pool.root.2"]   # CSC multimuons

    #    Files = ["rfio:/castor/cern.ch/user/h/hawkings/calibstream/fdr1/idcalib_5802_fdr1_0001.data"] # FDR 1 data - doesn't work with Rel 15 :(
    #    Files = ["rfio:/castor/cern.ch/user/b/bvendapi/FDR2_Pi/BS_files/CalibStream/CSC.007422.singlepart_singlepi_pt10_50_Calib_1000_1.data"]  # FDR 2 data - doesn't work with Rel 15 :(

    if challenge == 'ID_FDR_09':
        Files = ["/afs/cern.ch/user/t/tcorneli/public/InDetRecESD.root"]

    Cosmics         =  False
    BeamHalo        =  False
    BeamGas         =  False
    
    # -------------------------------------
    # Reconstruct data
    # -------------------------------------
    doPixel  = True
    doSCT    = True
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


