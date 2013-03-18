# $Id$

###################################################################3
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [ "/tmp/morrisj/data12_8TeV.00215559.calibration_L1CaloCalib.daq.RAW._lb0000._SFO-1._0002.data" ]
#athenaCommonFlags.FilesInput = [ "/tmp/morrisj/tile/data11_calib.00177970.calibration_L1CaloEnergyScan.daq.RAW._lb0000._SFI-LVL1-1._0001.data" ]

###################################################################3
# Define the output file here.
#

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'l1calocalib.root'

from D3PDMakerConfig.D3PDProdFlags import prodFlags
prodFlags.WriteL1CALOD3PD = True
prodFlags.WriteL1CALOD3PD.FileName = tupleFileOutput
prodFlags.WriteL1CALOD3PD.lock()

###################################################################3
# Define other job options here.
#

athenaCommonFlags.EvtMax = -1

####################################################################
# Configure RecExCommon.
#

from RecExConfig.RecFlags import rec
rec.DPDMakerScripts.append( "L1CaloD3PDMaker/L1CaloD3PD_RAW_prodJobOFragment.py" )
rec.doCalo.set_Value_and_Lock( True )
rec.Commissioning.set_Value_and_Lock(True)

disableRecJobOpts = ('doAOD', 'doAODCaloCells', 'doAODall', 'doAlfa', 'doApplyAODFix', 'doCBNT', 'doCheckDictionary',
                    'doCheckJOT',  'doDetStatus', 'doDetailedAuditor', 'doDetailedPerfMon', 'doDumpMC',
                    'doDumpPoolInputContent', 'doDumpProperties', 'doDumpTDS', 'doDumpTES', 'doESD', 'doEdmMonitor',
                    'doEgamma', 'doFileMetaData', 'doFloatingPointException', 'doForwardDet', 'doHeavyIon', 'doHist',
                    'doJetMissingETTag', 'doJiveXML', 'doLowPt', 'doLucid', 'doMinimalRec', 'doMonitoring',
                    'doMuonCombined', 'doNameAuditor', 'doPerfMon', 'doPersint', 'doPyDump', 'doRestrictedESD', 'doSGAuditor',
                    'readAOD', 'readESD', 'readTAG','doWriteAOD','doTruth','doTagRawSummary','doWriteTAG')

for p in disableRecJobOpts:
   getattr(rec, p).set_Value_and_Lock(False)

#for i in dir(rec):
  #try:
    #print getattr(rec,i)
  #except ValueError:
    #pass

include ("RecExCommon/RecExCommon_topOptions.py")
