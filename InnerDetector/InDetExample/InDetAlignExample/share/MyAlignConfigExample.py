#!/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.7.6/sw/lcg/releases/LCG_81e/Python/2.7.9.p1/x86_64-slc6-gcc49-opt/bin/python
# =====================================================================
# Main script to run the NewInDetIterator
# run with ./RunIterator.py
# =====================================================================
import os
import sys

os.system("get_files -jo InDetAlignExample/alignConfig.py >/dev/null")
import alignConfig

#----------------------------------------------------------------------------------------------------
class myAlignConfig( alignConfig.alignConfigBase ):
    def __init__(self):
        
        #===============================
        # Number of Iterations
        #===============================
        self.FirstIteration    = 0
        self.Iterations        = 2
        
        #===============================
        # Decide alignment strategies
        #===============================
        strategies = [ self.use_dynamicDb,
                       self.do_L11,
                       #self.do_L16,
                       #self.do_L2,
                       #self.do_L27,
                       #self.do_L3,
                       
                       #self.do_beamspot_constraint,
                       #self.do_EoverP_constraint,
                       #self.do_d0_constraint,
                       #self.do_z0_constraint,
                       
                       self.do_truncation,
                       ]
        
        # Inherit baseline.
        alignConfig.alignConfigBase.__init__( self, self.FirstIteration, self.Iterations, strategies )
        
        #=======================================================
        #
        # The below are force overriding even after optParsing
        #
        #======================================================
        
        # dry run
        self.is_dryrun         = False
        
        # Batch queue: suggestions are 8nh, 1nd, atlasb1
        self.QUEUE             = '8nh'
        
        # Prefix to identify your jobs
        #self.preName           = "prefix"
        
        #suffix to the folder name
        #self.folderSuffix      = "suffix"
        
        # OutputLevel (DEBUG, INFO, VERBOSE)
        self.OutputLevel       = 'INFO'
        
        # Data Type
        self.DataType          = "data16_13TeV"
        
        self.setup_extra_options()
        
        self.print_setup()
        
        
    def setup_extra_options(self):
        # for more options, see
        # https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetExample/InDetAlignExample/trunk/share/jobOption_ConditionsOverrider.py#L15
        
        # These options will be written to the jobOptions
        # have a look into NewInDetAlignAlgSetup.py and NewInDetAlignGeometrySetup.py
        # for all supported options
        
        self.extraOptions["doMonitoring"]                = True
        
        self.extraOptions["trackCollection"]             = "ExtendedTracks"
        self.extraOptions["doBSTrackSelection"]          = True
        self.extraOptions["PtCutForBSConstraint"]        = 3.0
        self.extraOptions["MinPtForConstrainedProvider"] = 3.0


#----------------------------------------------------------------------------------------------------
# class myAlignConfig end.
#----------------------------------------------------------------------------------------------------

config = myAlignConfig()

# check that inputList and eventtype have the same number of elements
assert len(config.inputList) == len(config.eventType), " Internal error in " + __file__

DataToRun = config.generate_dataset()

os.system("get_files -jo InDetAlignExample/NewInDetIterator_SmartBatch.py >/dev/null")
os.system("get_files -jo InDetAlignExample/IteratorManager.py >/dev/null")
execfile("NewInDetIterator_SmartBatch.py")
