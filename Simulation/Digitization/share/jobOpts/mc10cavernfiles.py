cavernlist=["root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000011.pool.root.1",
            "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000111.pool.root.1",
            "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000144.pool.root.1",
            "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000150.pool.root.1",
            "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000151.pool.root.1",
            "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000161.pool.root.1",
            "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000189.pool.root.1",
            "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000244.pool.root.1",
            "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000316.pool.root.1",
            "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000393.pool.root.1",
            "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000415.pool.root.1"]
def makeCavernBkgInputCol(nEvts, initialList):
    uberlist=[]
    refreshrate=1/150.0
    from Digitization.DigitizationFlags import digitizationFlags
    if(digitizationFlags.doXingByXingPileUp()):
        refreshrate=1.0
    bob = 65.0*float(digitizationFlags.numberOfCavern.get_Value()) 
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
digitizationFlags.cavernInputCols=makeCavernBkgInputCol(25*len(athenaCommonFlags.PoolHitsInput.get_Value()), cavernlist)
print "digitizationFlags.cavernInputCols size = ", len(digitizationFlags.cavernInputCols.get_Value())
