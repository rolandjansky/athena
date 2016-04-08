# This jobO should not be included more than once:
include.block( "JetN2N/SlimSMQCDNTUP_prodJobOFragment.py" )
# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.SkimNTUP_ProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from AthenaCommon.SystemOfUnits import GeV

JetN2N=prodFlags.WriteSlimSMQCDNTUP

# Set up a logger:
from AthenaCommon.Logging import logging
SlimSMQCDStream_msg = logging.getLogger( 'SlimSMQCD_prodJobOFragment' )

# Check if the configuration makes sense:
if JetN2N.isVirtual:
    SlimSMQCDStream_msg.error( "NTUP stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "NTUP set to be a virtual stream" )
    pass

## Construct the stream and file names:
streamName = JetN2N.StreamName
if JetN2N.FileName=='':
    fileName   = buildFileName( JetN2N )
else:
    fileName   = JetN2N.FileName
SlimSMQCDStream_msg.info( "Configuring SlimSMQCDNTUP with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

## set input tree:
from AthenaCommon.JobProperties import jobproperties
ntupFlags=jobproperties.SkimNTUP_ProdFlags
tree_name=ntupFlags.TreeName()

#do skim
from JetN2N.SlimSMQCD import doSlimSMQCD
from JetN2N.JetN2NFlags import JetN2NFlags
doSlimSMQCD(tree_name,fileName,athenaCommonFlags.FilesInput(),JetN2NFlags.JetN2NYear())
