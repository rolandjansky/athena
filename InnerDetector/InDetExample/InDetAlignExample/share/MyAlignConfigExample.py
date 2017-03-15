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
                self.Iterations        = 4
                
                #===============================
                # Decide alignment strategies
                #===============================
                strategiess = [ self.use_dynamicDb,
                                
                                self.do_L11,
                                #self.do_L16,
                                #self.do_L2,
                                #self.do_L27,
                                #self.do_L3,
                                
                                #self.do_beamspot_constraint,
                                #self.do_EoverP_constraint,
                                #self.do_d0_constraint,
                                #self.do_z0_constraint,
                                
                                #self.do_truncation,
                                ]
                
                # Inherit baseline.
                alignConfig.alignConfigBase.__init__( self, self.FirstIteration, self.Iterations, strategiess )
                
                #===============================
                # Basic running options
                #===============================
                
                # If this is true, only scripts are created, but jobs are not submit to LSF.
                self.is_dryrun         = False
                
                # Batch queue: suggestions are 8nh, 1nd, etc.
                self.QUEUE             = '8nh'
                
                # Prefix to identify your jobs. In this case, the iteration directory name becomes testL11_Iter[0-4]_suffix
                self.preName           = 'testL11'
                
                #suffix to the folder name
                self.folderSuffix      = "suffix"
                
                # OutputLevel (DEBUG, INFO, VERBOSE)
                self.OutputLevel       = 'INFO'
                
                # Global Condition Tag - this should be reflected to generate_dataset() properly later
                #self.GlobalTag         = 'CONDBR2-BLKPA-2016-12'
                
                # Geomodel - this should be reflected to generate_dataset() properly later
                #self.DescrVersion      = 'ATLAS-R2-2015-04-00-00'
                
                # Data Type
                self.DataType          = "data16_13TeV"
                
                self.setup_extra_options()
                self.print_setup()
                
                
        def setup_extra_options(self):
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


# check that inputList and eventype have the same number of elements
if (len(config.inputList) != len(config.eventType)):
    print " <RunIterator> ERROR "
    exit()


from InDetAlignExample.NewInDet_SetupData import setupData
DataToRun = config.generate_dataset()

os.system("get_files -jo InDetAlignExample/NewInDetIterator_SmartBatch.py >/dev/null")
os.system("get_files -jo InDetAlignExample/IteratorManager.py >/dev/null")
execfile("NewInDetIterator_SmartBatch.py")


