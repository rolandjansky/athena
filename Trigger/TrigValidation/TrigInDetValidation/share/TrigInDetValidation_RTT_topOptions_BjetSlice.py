
#####################################################################################################
#
# top level jobOptions to run b-jet chains in the RTT or standalone
# sets some global variables that adjust the execution of TrigInDetValidation_RTT_Common.py
#
# Jiri.Masik@manchester.ac.uk
#
#####################################################################################################

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#set athenaCommonFlags.FilesInput to be able to use this job options standalone without RTT
#secondSet of files can be activated by the if statement below 

if athenaCommonFlags.FilesInput()==[]:
  athenaCommonFlags.FilesInput=[
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_bjet5200/RDO.213802._000001.pool.root.1"
    ]
  secondSet=True 
  if secondSet: 
    athenaCommonFlags.FilesInput=[
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000016.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000019.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000028.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000031.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000035.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000046.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000054.pool.root.1 ",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000057.pool.root.1 ",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000070.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000071.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000072.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000079.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000090.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000096.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000103.pool.root.1"
      ]

include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")

rMC = False
if 'runMergedChain' in dir() and runMergedChain==True:
  rMC = True
rID=False
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True
rFTK=False
if 'doFTK' in dir() and doFTK==True:
  from TriggerJobOpts.TriggerFlags import TriggerFlags
  TriggerFlags.doFTK=True
  rFTK=True

(idtrigChainlist, tidaAnalysischains) = bjetChains(rMC,rID,rFTK)


def resetSigs():
  #TriggerFlags.doMuon=False         #dependency of L1Topo
  TriggerFlags.doHypo=False
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.BjetSlice.setAll();
  TriggerFlags.BjetSlice.signatures = idtrigChainlist

include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")

# print "*********************************"
# print "*********************************"
# print " ***         WARNING          ***"
# print "    Temporarily removing MDT"
# print "     (Savannah bug #104846) "
# print "*********************************"
# print "*********************************"
# disable the muons due to MDT error
RegSelSvcDefault.enableMDT=False

print "*********************************"
print " ***         WARNING          ***"
print "      NO LONGER removing MDT"
print "      (Savannah bug #104846) "
print "*********************************"


