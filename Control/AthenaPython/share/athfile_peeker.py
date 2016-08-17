## @file: AthenaPython/athfile_peeker.py
## @purpose: simple joboptions to inspect a POOL file's content
## @date May 2009
## @author Sebastien Binet <binet@cern.ch>

__version__ = "$Revision$"
__author__  = "Sebastien Binet <binet@cern.ch>"

## imports
import os

## usual Athena POOL drudegery
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
FNAME = vars().get('FNAME', 'esd.pool')
if isinstance(FNAME, str):
    FNAMES = [FNAME]
elif isinstance(FNAME, list):
    FNAMES = FNAME[:]
svcMgr.EventSelector.InputCollections = FNAMES

## configuring top-sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from AthenaPython.FilePeekerLib import FilePeeker
import AthenaCommon.Constants as Lvl
job += FilePeeker('peeker', OutputLevel=Lvl.INFO)
job.peeker.outfname = 'peeker.%08i.pkl' % os.getpid()
job.peeker.infname  = FNAMES[0]

## metadata + taginfo configuration
import IOVDbSvc.IOVDb

## evt-max
theApp.EvtMax = vars().get('EVTMAX', 1)

svcMgr.MessageSvc.OutputLevel = Lvl.ERROR
