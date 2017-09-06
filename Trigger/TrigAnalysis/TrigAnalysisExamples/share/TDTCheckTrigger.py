#!/usr/bin/env python
# @file:    TDTCheckTrigger.py
# @purpose: read a POOL file and dump a trigger summary.
#           re-written from the original by Till Eifert <Till.Eifert@cern.ch>
# @author:  Michael Begel <Michael.Begel@cern.ch>
# @date:    May 2009
#
# @example:
# @code
# TDTCheckTrigger aod.pool.root
# TDTCheckTrigger /castor/cern.ch/user/j/johndoe/aod.pool.root
# TDTCheckTrigger somedir/*/*.pool
# @endcode
#

__version__ = "$Revision: 1.1 $"
__author__  = "Michael Begel <Michael.Begel@cern.ch>"
##################################
import os,time,sys
import ROOT
from optparse import OptionParser
##################################
##################################
if __name__ == "__main__":
    parser = OptionParser(usage="usage: %prog [options] [-f] my.file.pool")

    p = parser.add_option
    p( "-f",
       "--file",
       dest = "fileName",
       help = "The path to the POOL file to analyze" )
    
    (options, args) = parser.parse_args()

    fileNames = []
   
    if len(args) > 0:
        fileNames = [ arg for arg in args if arg[0] != "-" ]
        pass

    if options.fileName == None and len(fileNames) == 0:
        str(parser.print_help() or "")
        sys.exit(1)

    if options.fileName != None:
        fileName = os.path.expandvars(os.path.expanduser(options.fileName))
        fileNames.append(fileName)

    fileNames = set( fileNames )
    sc = 0

    #fileNames=['/afs/cern.ch/user/t/tbold/public/TDTtest/menu1/AOD.pool.root']
    
    ##################################
    # Instantiate TrigDecisionTool
    print '...building transient trees...'
    from TrigDecisionTool.BuildTransientTrees import BuildTransientTrees
    (transientTree, transientMetaDataTree) = BuildTransientTrees(fileNames)
    tdt = ROOT.Trig.TrigDecisionToolARA(transientTree, transientMetaDataTree)
    import AthenaCommon.Constants as Constants
    tdt.setOutputLevel(Constants.FATAL)
    bits=['EF_passedRaw','EF_passThrough','EF_prescaled','EF_resurrected',
          'L2_passedRaw','L2_passThrough','L2_prescaled','L2_resurrected',
          'L1_isPassedAfterPrescale','L1_isPassedBeforePrescale','L1_isPassedAfterVeto']
    BITS=[(i,getattr(ROOT.TrigDefs,i)) for i in bits]
    ##################################
    # loop over all events
    DATA={}
    for evt in xrange(transientTree.GetEntries()):
        transientTree.GetEntry(evt)
        for trig in list(tdt.getListOfTriggers()):
            if trig not in DATA:
                DATA[trig]=DATA.fromkeys(bits,0)
            b=tdt.isPassedBits(trig)
            for i,j in BITS:
                DATA[trig][i]+=b & j
    for trig in sorted(DATA.keys()):
        line='%30s' % trig
        line+='| %(L1_isPassedAfterVeto)8d %(L1_isPassedBeforePrescale)8d %(L1_isPassedAfterPrescale)8d' % (DATA[trig])
        if not trig.startswith('L1'):
            line+='| %(L2_passedRaw)7d (%(L2_passThrough)7d)' % (DATA[trig])
        if trig.startswith('EF'):
            line+='| %(EF_passedRaw)7d (%(EF_passThrough)7d)' % (DATA[trig])
        print line
    ##################################
    ##################################
os._exit(0)
