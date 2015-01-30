#!/usr/bin/env python
__doc__ = """JobTransform to run LAr CAF jobs"""


#from PyJobTransformsCore.trf import *
import sys
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addDetectorArguments
import PyJobTransforms.trfArgClasses as trfArgClasses
#from PyJobTransformsCore.full_trfarg import *
#from PyJobTransformsCore.trfutil import *



#	                                   perfMonFile = 'ntuple_RAWtoESD.pmon.gz'))


#class LArCAFTrf( JobTransform ):
#    def __init__(self):
#        JobTransform.__init__(self,
#                              authors = [ Author('Walter Lampl','walter.lampl@cern.ch'),],
#                              name = 'LArCAF_calib',
#                              skeleton='LArCafJobs/skeleton.LArCAF.py',
#                              help = __doc__ )
#        # Add arguments:
#        self.add( InputBSFileArg())
#        self.add( NtupleFileArg(name="outputNTUP_SAMPLESMONFile",tree_names="events"))
#        self.add( ConditionsTagArg(),default="CURRENT")
#        self.add( GeometryVersionArg(),default='ATLAS-R2-2015-02-00-00')
#        self.add( MaxEventsArg(), default='-1')
#        self.add( SkipEventsArg(), default='0')
#        self.enableMaskAllErrors('ALL')



#    def runJob(self):        
#        report=JobTransform.runJob(self)
#        return report




# execute it if not imported
if __name__ == '__main__':

    executorSet = set()
    executorSet.add(athenaExecutor(name = 'LArCAFcalib', skeletonFile = 'LArCafJobs/skeleton.LArCAF.py',
                                   substep = 'r2e', inData = ['BS',], outData = ['NTUP_SAMPLESMON']))
    
    trf = transform(executor = executorSet)  
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    trf.parser.add_argument('--inputBSFile', nargs='+',
                            type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input'),
                            help='Input bytestream file', group='Reco Files')
    
    trf.parser.add_argument('--outputNTUP_SAMPLESMONFile', nargs='+',
                            type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, io='output', treeNames="events"),
                            help='Output LAr Samples Mon file', group='Ntuple Files')

    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    
