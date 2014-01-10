# This is the configuration file to run ATLAS Digitization
# Use the following switches for:
#   jobproperties.AthenaCommonFlags.EvtMax = <NEvents>         Number of events to digitize (set to -1 for all)
#   jobproperties.AthenaCommonFlags.SkipEvents = <NSkip>       Number of events from input file collection to skip
#   jobproperties.AthenaCommonFlags.PoolHitsInput=<FileName>   Input collections. The list of files with the hits to digitize
#   jobproperties.AthenaCommonFlags.PoolRDOOutput=<FileName>   Output file name

#--------------------------------------------------------------------
# Perfmon configuration
#--------------------------------------------------------------------
#from PerfMonComps.PerfMonFlags import jobproperties
#jobproperties.PerfMonFlags.doMonitoring = True
#jobproperties.PerfMonFlags.doHephaestusMon = True
#jobproperties.PerfMonFlags.doPerEvtLeakCheck = True

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=5
jobproperties.AthenaCommonFlags.PoolHitsInput=[ "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462/HITS.025431._02899.pool.root.1",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462/HITS.025431._05408.pool.root.1",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462/HITS.025431._07403.pool.root.1",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462/HITS.025431._08657.pool.root.1",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462/HITS.025431._09029.pool.root.1",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462/HITS.025431._10204.pool.root.1",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462/HITS.025431._11291.pool.root.1",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462/HITS.025431._15535.pool.root.1",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462/HITS.025431._18371.pool.root.1",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462/HITS.025431._19075.pool.root.1"
                                                ]

jobproperties.AthenaCommonFlags.PoolRDOOutput="Valgrind.PileUpTools.RDO.pool.root"

#--------------------------------------------------------------
# Setup Output File For Histograms
#--------------------------------------------------------------
## from GaudiSvc.GaudiSvcConf import THistSvc
## ServiceMgr+=THistSvc()
## ServiceMgr.THistSvc.Output = ["SCT_DigiRTT DATAFILE='PileUpToolsHists.root' opt='RECREATE'"];

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties
jobproperties.Digitization.doXingByXingPileUp=True
jobproperties.Digitization.doMinimumBias=True
jobproperties.Digitization.doCavern=True
jobproperties.Digitization.doBeamGas=True
jobproperties.Digitization.doBeamHalo=True

jobproperties.Digitization.bunchSpacing=25
jobproperties.Digitization.numberOfCavern=2
jobproperties.Digitization.numberOfCollisions=2.30
jobproperties.Digitization.initialBunchCrossing=-32 #default
jobproperties.Digitization.finalBunchCrossing=32 #default
jobproperties.Digitization.numberOfBeamHalo=0.05
jobproperties.Digitization.numberOfBeamGas=0.0003

#include("Digitization/minbiasfiles.py")

minbiaslist=[
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00030.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00067.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00069.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00070.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00076.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00090.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00129.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00138.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00140.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00166.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00176.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00194.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00200.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00214.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00246.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00259.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00261.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00276.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00304.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00309.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00312.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00339.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00406.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00423.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00448.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00460.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00492.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00506.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00558.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00624.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00628.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00632.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00669.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00683.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00805.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00807.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00813.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00855.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00867.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00900.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00927.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00934.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00938.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00953.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00977.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00978.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01030.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01038.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01105.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01128.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01172.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01179.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01190.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01193.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01213.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01219.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01231.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01237.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01252.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01260.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01301.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01355.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01372.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01383.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01401.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01435.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01452.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01456.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01476.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01486.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01502.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01511.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01522.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01534.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01544.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01547.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01556.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01567.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01582.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01612.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01657.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01658.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01660.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01705.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01729.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01738.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01744.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01785.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01787.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01797.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01811.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01887.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01906.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01908.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01944.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01949.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01956.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01966.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._01996.pool.root",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._02000.pool.root"
    ]

def makeMinBiasBkgInputCol(nEvts, initialList):
    uberlist=[]
    refreshrate=1/150.0
    from Digitization.DigitizationFlags import jobproperties
    if(jobproperties.Digitization.doXingByXingPileUp()):
        refreshrate=1.0
    from AthenaCommon.AthenaCommonFlags import jobproperties
    bob = 65.0*jobproperties.Digitization.numberOfCollisions.get_Value()
    #print bob
    bob += bob*refreshrate*nEvts
    #print bob
    bob/=25*len(initialList)
    #print bob
    bill = 1+ int(bob)
    #print bill
    for i in range(0,bill):
        #print len(uberlist)
        uberlist+=initialList
        #print len(uberlist)
    return uberlist

jobproperties.Digitization.minBiasInputCols= makeMinBiasBkgInputCol(25*len(jobproperties.AthenaCommonFlags.PoolHitsInput.get_Value()), minbiaslist)
print "jobproperties.Digitization.minBiasInputCols size = ", len(jobproperties.Digitization.minBiasInputCols.get_Value())

#include("Digitization/cavernfiles.py")

cavernlist=[
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00040.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00063.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00120.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00146.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00171.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00201.pool.root.2",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00206.pool.root.2",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00235.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00285.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00366.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00382.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00430.pool.root.3",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00480.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00483.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00571.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00627.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00648.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00683.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00697.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00700.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00813.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00840.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00856.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00952.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00961.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01043.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01065.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01105.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01123.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01150.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01223.pool.root.2",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01279.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01289.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01330.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01341.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01618.pool.root.2",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01642.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01660.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01707.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01719.pool.root.2",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01814.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01867.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01877.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01913.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01973.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._01999.pool.root.3",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02041.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02119.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02129.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02152.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02194.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02313.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02319.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02387.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02409.pool.root.2",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02410.pool.root.2",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02417.pool.root.2",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02479.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02487.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02491.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02495.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02505.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02632.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02640.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02683.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02695.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02714.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02760.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02775.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02800.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02804.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02818.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02827.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02843.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._02941.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03021.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03037.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03043.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03059.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03078.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03120.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03183.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03211.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03259.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03294.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03301.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03329.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03338.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03381.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03421.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03435.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03456.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03567.pool.root.1",
    "/atlasdisk1/wpark/MonteCarlo/sim_dig/filtered.mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._03582.pool.root.1"
]

def makeCavernBkgInputCol(nEvts, initialList):
    uberlist=[]
    refreshrate=1/150.0
    from Digitization.DigitizationFlags import jobproperties
    if(jobproperties.Digitization.doXingByXingPileUp()):
        refreshrate=1.0
    from AthenaCommon.AthenaCommonFlags import jobproperties
    bob = 65.0*float(jobproperties.Digitization.numberOfCavern.get_Value())
    #print bob
    bob += bob*refreshrate*nEvts
    #print bob
    bob/=25*len(initialList)
    #print bob
    bill = 1+ int(bob)
    #print bill
    for i in range(0,bill):
        #print len(uberlist)
        uberlist+=initialList
        #print len(uberlist)
    return uberlist
    
jobproperties.Digitization.cavernInputCols=makeCavernBkgInputCol(25*len(jobproperties.AthenaCommonFlags.PoolHitsInput.get_Value()), cavernlist)
print "jobproperties.Digitization.cavernInputCols size = ", len(jobproperties.Digitization.cavernInputCols.get_Value())



jobproperties.Digitization.beamGasInputCols = ["/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00007.pool.root.1",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00015.pool.root.3",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00047.pool.root.2",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00056.pool.root.1",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00101.pool.root.1",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00123.pool.root.2",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00161.pool.root.1",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00166.pool.root.1",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00208.pool.root.1",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00235.pool.root.1",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00256.pool.root.1",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00263.pool.root.1",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00317.pool.root.1",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00328.pool.root.2",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00335.pool.root.2",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00358.pool.root.1",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00374.pool.root.1",
                                               "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470_tid024853/HITS.024853._00382.pool.root.1"]

jobproperties.Digitization.beamHaloInputCols = ["/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00017.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00023.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00032.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00088.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00135.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00151.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00152.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00167.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00183.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00201.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00211.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00235.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00265.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00291.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00294.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00319.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00354.pool.root",
                                                "/atlasdisk1/wpark/MonteCarlo/sim_dig/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473_tid025027/HITS.025027._00400.pool.root"]

#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-02-01-00'

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
#DetFlags.BCM_setOn()
#DetFlags.pixel_setOn()
#DetFlags.SCT_setOn()
#DetFlags.Calo_setOn()
#DetFlags.Muon_setOn()
DetFlags.CSC_setOn()
#DetFlags.MDT_setOff()
#DetFlags.RPC_setOff()
#DetFlags.TGC_setOff()
DetFlags.Truth_setOn()
include( "Digitization/Digitization.py" )

#use a wide name field to "see" the tools in use
svcMgr.MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"
MessageSvc.defaultLimit = 9999999
svcMgr.MessageSvc.enableSuppression =False

#remove some of POOL annoying warnings
svcMgr.PoolSvc.AttemptCatalogPatch = True

## from SCT_DigitizationRTT.SCT_DigitizationRTTConf import SCT_DigitizationRTT
## job += SCT_DigitizationRTT("SCT_DigitizationRTT")
## SCT_DigiRTT = job.SCT_DigitizationRTT
## SCT_DigiRTT.SelectEvent         = False
## SCT_DigiRTT.PrintGeomInfo       = False
## SCT_DigiRTT.PrintEvents         = False
## SCT_DigiRTT.RawDataHists        = True
## SCT_DigiRTT.SimDataHists        = False
## SCT_DigiRTT.G4HitsHists         = False
## SCT_DigiRTT.CresHists           = False
## SCT_DigiRTT.MinSctBrlClusMult   = 100
## SCT_DigiRTT.MaxSctBrlClusMult   = 0
## SCT_DigiRTT.OutputLevel         = ERROR

theApp.EvtMax = 10
