#!/usr/bin/env python
__doc__ = """JobTransform to run LAr CAF jobs"""


from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *


class LArCAFTrf( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('Walter Lampl','walter.lampl@cern.ch'),],
                              name = 'LArCAF_calib',
                              skeleton='LArCafJobs/skeleton.LArCAF.py',
                              help = __doc__ )
        # Add arguments:
        self.add( InputBSFileArg())
        self.add( NtupleFileArg(name="outputNTUP_SAMPLESMONFile",tree_names="events"))
        self.add( ConditionsTagArg(),default="CURRENT")
        self.add( GeometryVersionArg(),default='ATLAS-GEO-18-00-00')
        self.add( MaxEventsArg(), default='-1')
        self.add( SkipEventsArg(), default='0')
        self.enableMaskAllErrors('ALL')



    def runJob(self):        
        report=JobTransform.runJob(self)
        return report




# execute it if not imported
if __name__ == '__main__':
    trf = LArCAFTrf()
    sys.exit(trf.exeSysArgs().exitCode())
    
