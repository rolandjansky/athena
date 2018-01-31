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

#---- Minimal joboptions -------

theApp.EvtMax=10                                         #how many events to run over. Set to -1 for all events.
jps.AthenaCommonFlags.FilesInput = ["%(inFile)s"]   #insert your list of input files here (do this before next lines)

#Now choose your read mode (POOL, xAOD, or TTree):

#POOL:
#import AthenaPoolCnvSvc.ReadAthenaPool                   #sets up reading of any POOL files (but POOL is slow)

#xAOD:
import AthenaRootComps.ReadAthenaxAODHybrid               #FAST xAOD reading!

#TTree:
#import AthenaRootComps.ReadAthenaRoot                    #read a flat TTree, very fast, but no EDM objects
#svcMgr.EventSelector.TupleName="MyTree"                  #You usually must specify the name of the tree (default: CollectionTree)

athAlgSeq += CfgMgr.%(klass)s()                           #adds an instance of your alg to the main alg sequence

#-------------------------------


#note that if you edit the input files after this point you need to pass the new files to the EventSelector:
#like this: svcMgr.EventSelector.InputCollections = ["new","list"] 





##--------------------------------------------------------------------
## This section shows up to set up a HistSvc output stream for outputing histograms and trees
## See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_output_trees_and_histogra for more details and examples

#if not hasattr(svcMgr, 'THistSvc'): svcMgr += CfgMgr.THistSvc() #only add the histogram service if not already present (will be the case in this jobo)
#svcMgr.THistSvc.Output += ["MYSTREAM DATAFILE='myfile.root' OPT='RECREATE'"] #add an output root file stream

##--------------------------------------------------------------------


##--------------------------------------------------------------------
## The lines below are an example of how to create an output xAOD
## See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_create_an_output_xAOD for more details and examples

#from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
#xaodStream = MSMgr.NewPoolRootStream( "StreamXAOD", "xAOD.out.root" )

##EXAMPLE OF BASIC ADDITION OF EVENT AND METADATA ITEMS
##AddItem and AddMetaDataItem methods accept either string or list of strings
#xaodStream.AddItem( ["xAOD::JetContainer#*","xAOD::JetAuxContainer#*"] ) #Keeps all JetContainers (and their aux stores)
#xaodStream.AddMetaDataItem( ["xAOD::TriggerMenuContainer#*","xAOD::TriggerMenuAuxContainer#*"] )
#ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool("TriggerMenuMetaDataTool") #MetaDataItems needs their corresponding MetaDataTool
#svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ] #Add the tool to the MetaDataSvc to ensure it is loaded

##EXAMPLE OF SLIMMING (keeping parts of the aux store)
#xaodStream.AddItem( ["xAOD::ElectronContainer#Electrons","xAOD::ElectronAuxContainer#ElectronsAux.pt.eta.phi"] ) #example of slimming: only keep pt,eta,phi auxdata of electrons

##EXAMPLE OF SKIMMING (keeping specific events)
#xaodStream.AddAcceptAlgs( "%(klass)s" ) #will only keep events where 'setFilterPassed(false)' has NOT been called in the given algorithm

##--------------------------------------------------------------------


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

