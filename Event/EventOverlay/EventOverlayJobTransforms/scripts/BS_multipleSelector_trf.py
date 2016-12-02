#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Filter BS data based on trigger bit and lbn_map and write to multiple output streams"""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *
from PyJobTransformsCore.TransformConfig import *
import PyJobTransformsCore.basic_trfarg as trfarg

from RecJobTransforms.RecConfig import recConfig

class OutputBSFilterFileArg0(StringArg):
    """The output file prefix0"""
    def __init__(self,help='default',name='OutputBSFilterFile0'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True
    
class OutputBSFilterFileArg1(StringArg):
    """The output file prefix1"""
    def __init__(self,help='default',name='OutputBSFilterFile1'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True
    
class OutputBSFilterFileArg2(StringArg):
    """The output file prefix2"""
    def __init__(self,help='default',name='OutputBSFilterFile2'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True
    
class OutputBSFilterFileArg3(StringArg):
    """The output file prefix3"""
    def __init__(self,help='default',name='OutputBSFilterFile3'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True
    
class OutputBSFilterFileArg4(StringArg):
    """The output file prefix4"""
    def __init__(self,help='default',name='OutputBSFilterFile4'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True

#####################################
#from "outputclasses.py" made by:
#cd Event/EventOverlay/EventOverlayJobTransforms/scripts/
#./makeoutputclasses.sh

class OutputBSFilterFileArg5(StringArg):
        """The output file prefix5"""
        def __init__(self,help='default',name='OutputBSFilterFile5'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg6(StringArg):
        """The output file prefix6"""
        def __init__(self,help='default',name='OutputBSFilterFile6'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg7(StringArg):
        """The output file prefix7"""
        def __init__(self,help='default',name='OutputBSFilterFile7'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg8(StringArg):
        """The output file prefix8"""
        def __init__(self,help='default',name='OutputBSFilterFile8'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg9(StringArg):
        """The output file prefix9"""
        def __init__(self,help='default',name='OutputBSFilterFile9'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg10(StringArg):
        """The output file prefix10"""
        def __init__(self,help='default',name='OutputBSFilterFile10'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg11(StringArg):
        """The output file prefix11"""
        def __init__(self,help='default',name='OutputBSFilterFile11'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg12(StringArg):
        """The output file prefix12"""
        def __init__(self,help='default',name='OutputBSFilterFile12'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg13(StringArg):
        """The output file prefix13"""
        def __init__(self,help='default',name='OutputBSFilterFile13'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg14(StringArg):
        """The output file prefix14"""
        def __init__(self,help='default',name='OutputBSFilterFile14'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg15(StringArg):
        """The output file prefix15"""
        def __init__(self,help='default',name='OutputBSFilterFile15'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg16(StringArg):
        """The output file prefix16"""
        def __init__(self,help='default',name='OutputBSFilterFile16'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg17(StringArg):
        """The output file prefix17"""
        def __init__(self,help='default',name='OutputBSFilterFile17'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg18(StringArg):
        """The output file prefix18"""
        def __init__(self,help='default',name='OutputBSFilterFile18'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg19(StringArg):
        """The output file prefix19"""
        def __init__(self,help='default',name='OutputBSFilterFile19'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg20(StringArg):
        """The output file prefix20"""
        def __init__(self,help='default',name='OutputBSFilterFile20'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg21(StringArg):
        """The output file prefix21"""
        def __init__(self,help='default',name='OutputBSFilterFile21'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg22(StringArg):
        """The output file prefix22"""
        def __init__(self,help='default',name='OutputBSFilterFile22'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg23(StringArg):
        """The output file prefix23"""
        def __init__(self,help='default',name='OutputBSFilterFile23'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg24(StringArg):
        """The output file prefix24"""
        def __init__(self,help='default',name='OutputBSFilterFile24'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg25(StringArg):
        """The output file prefix25"""
        def __init__(self,help='default',name='OutputBSFilterFile25'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg26(StringArg):
        """The output file prefix26"""
        def __init__(self,help='default',name='OutputBSFilterFile26'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg27(StringArg):
        """The output file prefix27"""
        def __init__(self,help='default',name='OutputBSFilterFile27'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg28(StringArg):
        """The output file prefix28"""
        def __init__(self,help='default',name='OutputBSFilterFile28'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg29(StringArg):
        """The output file prefix29"""
        def __init__(self,help='default',name='OutputBSFilterFile29'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg30(StringArg):
        """The output file prefix30"""
        def __init__(self,help='default',name='OutputBSFilterFile30'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg31(StringArg):
        """The output file prefix31"""
        def __init__(self,help='default',name='OutputBSFilterFile31'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg32(StringArg):
        """The output file prefix32"""
        def __init__(self,help='default',name='OutputBSFilterFile32'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg33(StringArg):
        """The output file prefix33"""
        def __init__(self,help='default',name='OutputBSFilterFile33'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg34(StringArg):
        """The output file prefix34"""
        def __init__(self,help='default',name='OutputBSFilterFile34'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg35(StringArg):
        """The output file prefix35"""
        def __init__(self,help='default',name='OutputBSFilterFile35'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg36(StringArg):
        """The output file prefix36"""
        def __init__(self,help='default',name='OutputBSFilterFile36'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg37(StringArg):
        """The output file prefix37"""
        def __init__(self,help='default',name='OutputBSFilterFile37'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg38(StringArg):
        """The output file prefix38"""
        def __init__(self,help='default',name='OutputBSFilterFile38'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg39(StringArg):
        """The output file prefix39"""
        def __init__(self,help='default',name='OutputBSFilterFile39'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg40(StringArg):
        """The output file prefix40"""
        def __init__(self,help='default',name='OutputBSFilterFile40'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg41(StringArg):
        """The output file prefix41"""
        def __init__(self,help='default',name='OutputBSFilterFile41'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg42(StringArg):
        """The output file prefix42"""
        def __init__(self,help='default',name='OutputBSFilterFile42'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg43(StringArg):
        """The output file prefix43"""
        def __init__(self,help='default',name='OutputBSFilterFile43'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg44(StringArg):
        """The output file prefix44"""
        def __init__(self,help='default',name='OutputBSFilterFile44'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg45(StringArg):
        """The output file prefix45"""
        def __init__(self,help='default',name='OutputBSFilterFile45'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg46(StringArg):
        """The output file prefix46"""
        def __init__(self,help='default',name='OutputBSFilterFile46'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg47(StringArg):
        """The output file prefix47"""
        def __init__(self,help='default',name='OutputBSFilterFile47'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg48(StringArg):
        """The output file prefix48"""
        def __init__(self,help='default',name='OutputBSFilterFile48'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

class OutputBSFilterFileArg49(StringArg):
        """The output file prefix49"""
        def __init__(self,help='default',name='OutputBSFilterFile49'):
            StringArg.__init__(self,help,name)
        
        def isFullArgument(self):
            return True

##########

class EventIdFileArg(StringArg):
    """The output text file for EventIdModifierSvc lines"""
    def __init__(self,help='default',name='EventIdFile'):
        StringArg.__init__(self,help,name)        
    def isFullArgument(self):
        return True

class TrigFileArg(StringArg):
    """The input text file for trigger info lines"""
    def __init__(self,help='default',name='TrigFile'):
        StringArg.__init__(self,help,name)        
    def isFullArgument(self):
        return True

class LbnMapFileArg(StringArg):
    """The lbn_map_file for the selector to read in"""
    def __init__(self,help='default',name='LbnMapFile'):
        StringArg.__init__(self,help,name)        
    def isFullArgument(self):
        return True

class UniqArg(StringArg):
    """A unique ID for the output files from this job"""
    def __init__(self,help='default',name='uniq'):
        StringArg.__init__(self,help,name)        
    def isFullArgument(self):
        return True
    
class TriggerBitArg(IntegerArg):
    """The trigger bit to select"""
    def __init__(self,help='default',name='TriggerBit'):
        IntegerArg.__init__(self,help,name)        
    def isFullArgument(self):
        return True

class NoutputsArg(IntegerArg):
    """The number of outputstreams"""
    def __init__(self,help='default',name='Noutputs'):
        IntegerArg.__init__(self,help,name)        
    def isFullArgument(self):
        return True

class MaxEventsVecArg(StringArg):
    """A vector of maxevent args for each stream"""
    def __init__(self,help='default',name='maxeventsvec'):
        StringArg.__init__(self,help,name)        
    def isFullArgument(self):
        return True

class SkipEventsVecArg(StringArg):
    """A vector of skipevent args for each stream"""
    def __init__(self,help='default',name='skipeventsvec'):
        StringArg.__init__(self,help,name)        
    def isFullArgument(self):
        return True

class NoutputsVecArg(StringArg):
    """A vector of noutputs args for each stream"""
    def __init__(self,help='default',name='noutputsvec'):
        StringArg.__init__(self,help,name)        
    def isFullArgument(self):
        return True

class StreamVecArg(StringArg):
    """A vector of stream indices for each stream"""
    def __init__(self,help='default',name='streamvec'):
        StringArg.__init__(self,help,name)        
    def isFullArgument(self):
        return True

class PostIncludeArg(JobOptionsArg):
    """Joboptions file with user settings, to run after the job itself"""
    def __init__(self,help='default',package='',name='default'):
        # split comma separated string into list
        if type(package) == str: package = package.split(',')
        # always add 'EventOverlayJobTransforms' package (which contain common postIncludeConfig files)
        commonPack = 'EventOverlayJobTransforms'
        if commonPack not in package: package.append(commonPack)
        JobOptionsArg.__init__(self,help=help,package=package,name=name)
        self.__config = None
    def isFullArgument(self):
        return True

    
class BSFilterJobTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('Andrew Haas', 'ahaas@cern.ch') ] ,
                              skeleton='EventOverlayJobTransforms/skeleton.BS_multipleSelector.py' ,
                              help = __doc__,
                              config = recConfig )

        #add arguments
        self.add( InputBSFileArg() )

        self.add( OutputBSFilterFileArg0(),default="" )
        self.add( OutputBSFilterFileArg1(),default="" )
        self.add( OutputBSFilterFileArg2(),default="" )
        self.add( OutputBSFilterFileArg3(),default="" )
        self.add( OutputBSFilterFileArg4(),default="" )
        self.add( OutputBSFilterFileArg5(),default="" )
        self.add( OutputBSFilterFileArg6(),default="" )
        self.add( OutputBSFilterFileArg7(),default="" )
        self.add( OutputBSFilterFileArg8(),default="" )
        self.add( OutputBSFilterFileArg9(),default="" )
        self.add( OutputBSFilterFileArg10(),default="" )
        self.add( OutputBSFilterFileArg11(),default="" )
        self.add( OutputBSFilterFileArg12(),default="" )
        self.add( OutputBSFilterFileArg13(),default="" )
        self.add( OutputBSFilterFileArg14(),default="" )
        self.add( OutputBSFilterFileArg15(),default="" )
        self.add( OutputBSFilterFileArg16(),default="" )
        self.add( OutputBSFilterFileArg17(),default="" )
        self.add( OutputBSFilterFileArg18(),default="" )
        self.add( OutputBSFilterFileArg19(),default="" )
        self.add( OutputBSFilterFileArg20(),default="" )
        self.add( OutputBSFilterFileArg21(),default="" )
        self.add( OutputBSFilterFileArg22(),default="" )
        self.add( OutputBSFilterFileArg23(),default="" )
        self.add( OutputBSFilterFileArg24(),default="" )
        self.add( OutputBSFilterFileArg25(),default="" )
        self.add( OutputBSFilterFileArg26(),default="" )
        self.add( OutputBSFilterFileArg27(),default="" )
        self.add( OutputBSFilterFileArg28(),default="" )
        self.add( OutputBSFilterFileArg29(),default="" )
        self.add( OutputBSFilterFileArg30(),default="" )
        self.add( OutputBSFilterFileArg31(),default="" )
        self.add( OutputBSFilterFileArg32(),default="" )
        self.add( OutputBSFilterFileArg33(),default="" )
        self.add( OutputBSFilterFileArg34(),default="" )
        self.add( OutputBSFilterFileArg35(),default="" )
        self.add( OutputBSFilterFileArg36(),default="" )
        self.add( OutputBSFilterFileArg37(),default="" )
        self.add( OutputBSFilterFileArg38(),default="" )
        self.add( OutputBSFilterFileArg39(),default="" )
        self.add( OutputBSFilterFileArg40(),default="" )
        self.add( OutputBSFilterFileArg41(),default="" )
        self.add( OutputBSFilterFileArg42(),default="" )
        self.add( OutputBSFilterFileArg43(),default="" )
        self.add( OutputBSFilterFileArg44(),default="" )
        self.add( OutputBSFilterFileArg45(),default="" )
        self.add( OutputBSFilterFileArg46(),default="" )
        self.add( OutputBSFilterFileArg47(),default="" )
        self.add( OutputBSFilterFileArg48(),default="" )
        self.add( OutputBSFilterFileArg49(),default="" )

        self.add( EventIdFileArg(), default="" )
        self.add( TrigFileArg(), default="" )
        self.add( LbnMapFileArg(), default="lbn_anal_map.txt" )
        self.add( UniqArg(), default="" )
        self.add( TriggerBitArg(), default=5 )
        self.add( NoutputsArg(), default=50 )
        self.add( MaxEventsArg() )
        self.add( SkipEventsArg(), default=0  )
        self.add( PostIncludeArg(), default='NONE' )
        self.add( MaxEventsVecArg(), default='' )
        self.add( SkipEventsVecArg(), default='' )
        self.add( NoutputsVecArg(), default='' )
        self.add( StreamVecArg(), default='' )
        
        #add other features
        self.add( SQLiteSupport() )


# execute it if not imported
if __name__ == '__main__':
    trf = BSFilterJobTransform()
    sys.exit(trf.exeSysArgs().exitCode())

