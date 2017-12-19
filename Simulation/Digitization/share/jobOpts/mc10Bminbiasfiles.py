minbiaslist=["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105000.pythia_minbias_inelastic.merge.HITS.e723_s932_s952/HITS.277248._000041.pool.root.1",
             "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105000.pythia_minbias_inelastic.merge.HITS.e723_s932_s952/HITS.277248._000042.pool.root.1",
             "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105000.pythia_minbias_inelastic.merge.HITS.e723_s932_s952/HITS.277248._000043.pool.root.1",
             "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105000.pythia_minbias_inelastic.merge.HITS.e723_s932_s952/HITS.277248._000044.pool.root.1",
             "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105000.pythia_minbias_inelastic.merge.HITS.e723_s932_s952/HITS.277248._000045.pool.root.1",
             "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105000.pythia_minbias_inelastic.merge.HITS.e723_s932_s952/HITS.277248._000046.pool.root.1",
             "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105000.pythia_minbias_inelastic.merge.HITS.e723_s932_s952/HITS.277248._000047.pool.root.1",
             "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105000.pythia_minbias_inelastic.merge.HITS.e723_s932_s952/HITS.277248._000048.pool.root.1"]

def makeMinBiasBkgInputCol(nEvts, initialList):
    uberlist=[]
    refreshrate=1/150.0
    from Digitization.DigitizationFlags import digitizationFlags
    if(digitizationFlags.doXingByXingPileUp()):
        refreshrate=1.0
    bob = 65.0*digitizationFlags.numberOfCollisions.get_Value()
    #print bob
    bob += bob*refreshrate*nEvts
    #print bob
    bob/=float(2000*len(initialList))
    #print bob
    bill = 1+ int(bob)
    #print bill
    for i in range(0,bill):
        #print len(uberlist)
        uberlist+=initialList
    #print len(uberlist)
    return uberlist

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.LowPtMinBiasInputCols= makeMinBiasBkgInputCol(25*len(athenaCommonFlags.PoolHitsInput.get_Value()), minbiaslist)
print "digitizationFlags.LowPtMinBiasInputCols size = ", len(digitizationFlags.LowPtMinBiasInputCols.get_Value())
