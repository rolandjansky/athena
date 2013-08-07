# This jobO should not be included more than once:
include.block( "ExoticsN2N/EXMJNTUP_prodJobOFragment.py" )
# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.SkimNTUP_ProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from AthenaCommon.SystemOfUnits import GeV

exmjntup=prodFlags.WriteEXMJNTUP

# Set up a logger:
from AthenaCommon.Logging import logging
EXMJStream_msg = logging.getLogger( 'EXMJNTUP_prodJobOFragment' )

# Check if the configuration makes sense:
if exmjntup.isVirtual:
    EXMJStream_msg.error( "NTUP stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "NTUP set to be a virtual stream" )
    pass

## Construct the stream and file names:
streamName = exmjntup.StreamName
if exmjntup.FileName=='':
    fileName   = buildFileName( exmjntup )
else:
    fileName   = exmjntup.FileName
EXMJStream_msg.info( "Configuring NTUP_EXMJ with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

## set command
from ExoticsN2N.ExoticsN2NFlags import ExoticsN2NFlags
import subprocess
def boolToStr(flag):
    if flag:return "1"
    else:return "0"

cmd=['ExoticsMultiJetSkim.exe',
     '-n',prodFlags.TreeName(),
     '-o',fileName,
     '-t',ExoticsN2NFlags.EXMJTriggers(),
     '-m',boolToStr(ExoticsN2NFlags.EXMJDoMC()),
     '-s',boolToStr(ExoticsN2NFlags.EXMJDoMCSlim()),
     '-b',boolToStr(ExoticsN2NFlags.EXMJDoBH()),
     '-f',boolToStr(ExoticsN2NFlags.EXMJDoMCSkim()),
     '-e',str(athenaCommonFlags.EvtMax()),
     '-S',str(athenaCommonFlags.SkipEvents()),
     ]

inputs=athenaCommonFlags.FilesInput()
if isinstance(inputs,str):
    inputs=[inputs]
cmd+=inputs
print 'cmd =',cmd
proc=subprocess.Popen( args=cmd, bufsize = 1, shell = False,stdout = subprocess.PIPE, stderr = subprocess.STDOUT )

while proc.poll() is None:
    line = proc.stdout.readline()
    if line:
        print line.rstrip()

rc=proc.returncode
print 'rc =',rc

if not rc==0:
    raise RuntimeError("ExoticsMultiJetSkim.exe returned with value {0:d} instead of 0. Stopping!".format(rc))

