##=============================================================================
## Name:        CommDPD_topOptions.py
##
## Authors:     David Cote & Karsten Koeneke (DESY)
## Created:     February 2009
##
## Description: job options for all Commissioning ESD->DPD.
##
## Usage: athena PrimaryDPDMaker/CommDPD_topOptions.py 2>&1 | tee log.txt
##        Edit this file to disable DPDs or change the global setup
##
##=============================================================================

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolESDInput=['/afs/cern.ch/user/c/cote/gencomm/NightlyTestInput/data09_cos.00121244.physics_IDCosmic.daq.RAW._lb0000._SFO-5._0017.data']
if not vars().has_key('EvtMax'): EvtMax = 10
athenaCommonFlags.EvtMax=EvtMax

#auto-configuration
from RecExConfig.RecFlags  import rec
rec.AutoConfiguration.append('everything')


#Commissioning DPD
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
primDPD.WritePixelCommStream   = True
primDPD.WriteSCTCommStream     = True
primDPD.WriteIDCommStream      = True
primDPD.WriteIDProjCommStream  = True
primDPD.WriteCaloCommStream    = True
primDPD.WriteTileCommStream    = True
primDPD.WriteEMClustCommStream = True
primDPD.WriteEGamTauCommStream = True
primDPD.WriteRPCCommStream     = True
primDPD.WriteTGCCommStream     = True
primDPD.WriteRandomCommStream  = True
primDPD.WriteMUONCommStream    = True

primDPD.UseMCTruth=False

include("PrimaryDPDMaker/esdtodpd.py")

