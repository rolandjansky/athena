# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.cmt_newalg
# @purpose streamline and ease the creation of a skeleton joboption
# @author Will Buttinger
# @date March 2015

#Note - this code could use a serious rewrite, I just hacked it together to get something working

from __future__ import with_statement

__version__ = "$Revision: 801598 $"
__author__ = "Will Buttinger"
__doc__ = "streamline and ease the creation of new skeleton joboption for analysis"

### imports -------------------------------------------------------------------
import os
import textwrap
import commands
import PyUtils.acmdlib as acmdlib
import fileinput

class Templates:
    jobo_template = """\
#Skeleton joboption for a simple analysis job

#---- Minimal job options -----

athAlgSeq += CfgMgr.%(klass)s()                               #adds an instance of your alg to the main alg sequence

jps.AthenaCommonFlags.HistOutputs = ["MYSTREAM:myfile.root"]  #register output files like this. MYSTREAM is used in the code


#---- Options you could specify on command line -----
#jps.AthenaCommonFlags.EvtMax=-1                          #set on command-line with: --evtMax=-1
#jps.AthenaCommonFlags.SkipEvents=0                       #set on command-line with: --skipEvents=0
#jps.AthenaCommonFlags.FilesInput = ["%(inFile)s"]        #set on command-line with: --filesInput=...


include("AthAnalysisBaseComps/SuppressLogging.py")              #Optional include to suppress as much athena output as possible. Keep at bottom of joboptions so that it doesn't suppress the logging of the things you have configured above

"""



### functions -----------------------------------------------------------------
@acmdlib.command(
    name='cmt.new-jobo'
    )
@acmdlib.argument(
    'joboName',
    help="name of the joboption"
    )
@acmdlib.argument(
    'alg',
    default='MyAlg',
    help='name of the algorithm to add to sequence'
    )
@acmdlib.argument(
    '--inFile',
    default='my.pool.root',
    help='name of an input file to add to EventSelector'
    )
def main(args):
    """create a new algorithm inside the current package. Call from within the package directory

    ex:
     $ acmd cmt new-jobo myJobo MyAlg
    """

    full_jobo_name = args.joboName
    full_alg_name = args.alg

    print textwrap.dedent("""\
    ::: create jobo [%(full_jobo_name)s] for alg [%(full_alg_name)s]""" %locals())

    #following code borrowed from gen_klass
    jobo = getattr(Templates, 'jobo_template')

    d = dict( klass=args.alg,
              inFile=args.inFile,
              )
    fname = args.joboName+'.py'
    #first check doesn't exist 
    if os.path.isfile(fname):
       print ":::  ERROR %s already exists" % fname
       return -1
    o_hdr = open(fname, 'w')
    o_hdr.writelines(jobo%d)
    o_hdr.flush()
    o_hdr.close()

