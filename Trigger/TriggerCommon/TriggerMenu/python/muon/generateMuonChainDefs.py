# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Muon chains
###########################################################################
__author__  = 'M.Woudstra, Y.Nakahama, K.Nagano'
__doc__="Definition of muon chains as used v4- menu" 

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.muon.generateMuonChainDefs.py' )
log.info("Importing %s",__name__)

from TriggerMenu.muon.MuonDef import L2EFChain_mu as L2EFChain_mu
import traceback

try:
    from TriggerMenu.muon.MuonDefIdTest import L2EFChain_mu as L2EFChain_mu_IdTest
except:
    log.error('generateMuonChainDefs: Problems when importing MuonDefIdTest.')
    log.info(traceback.print_exc())


from TriggerMenu.menu.MenuUtils import splitChainDict,mergeChainDefs

###########################################################################
###########################################################################

##Create a list with the threshold of all the muons in the chain
##it's needed to correctly configure the hypo for the FS muon chains
def GetAllMuonThresholds(chainDict):
    
    muons = []
    for chainpart in chainDict['chainParts']:
      if 'mu' in chainpart['trigType']:        
          for x in range(0,int(chainpart['multiplicity'])):

              if chainpart['threshold']!='0':
                  thr= '%sGeV' %(chainpart['threshold'] )
              else:
                  thr= 'passthrough'

              muons.append(thr)

    return muons

def generateChainDefs(chainDict, thisIsBphysChain=False):
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    asymDiMuonChain = False
    if (len(listOfChainDicts) > 1): asymDiMuonChain = True
    else: asymDiMuonChain = False

    asymMultiMuonChain = False
    if(len(listOfChainDicts) > 1 and "noL1" not in chainDict["chainName"] and sum(map(int, [subChainDict["chainParts"]["multiplicity"] for subChainDict in listOfChainDicts])) > 1):
        asymMultiMuonChain = True

    modifyNscanInputTE = False
    modifyCalotagInputTE = False

    for subChainDict in listOfChainDicts:
        if "IdTest" in subChainDict["chainParts"]["addInfo"]:
            Muon = L2EFChain_mu_IdTest(subChainDict, asymDiMuonChain)
        else:
            #If FS muon, needs total counts of muons in chain
            if 'noL1' in subChainDict["chainParts"]["extra"]:
                AllMuons=GetAllMuonThresholds(chainDict)
            else:
                AllMuons=[]

            Muon = L2EFChain_mu(subChainDict, asymDiMuonChain, asymMultiMuonChain, AllMuons, thisIsBphysChain)

        listOfChainDefs += [Muon.generateHLTChainDef()]
        
        #only needed for nscan
        if "nscan" in  subChainDict["chainParts"]['FSinfo']:
            modifyNscanInputTE = True
        if "calotag" in subChainDict["chainParts"]['FSinfo']:
            modifyCalotagInputTE = True


    if len(listOfChainDefs)>1:
        if ('mergingStrategy' in chainDict.keys()):        
            theChainDef = mergeChainDefs(listOfChainDefs,chainDict["mergingStrategy"],chainDict["mergingOffset"])
        else:
            theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]

    # needed for nscan to replace the placeholder TE with the L2TE of the other chain
    if (modifyNscanInputTE == True):
        theChainDef = _modifyTEinChainDef(theChainDef,chainDict)
    if (modifyCalotagInputTE == True):
        theChainDef = _modifyTEinChainDef(theChainDef, chainDict)
        
    #if chainDict["topo"]:
    #    theChainDef = _addTopoInfo(theChainDef,chainDict)
        
    if (len(listOfChainDicts) > 1):
        nomucomb_asymmetric = True
        for ii in range(len(listOfChainDicts)):
            if listOfChainDicts[ii]['chainParts']['reccalibInfo']!='nomucomb':nomucomb_asymmetric=False
        if nomucomb_asymmetric == True:
            theChainDef = _AsymmChainConfig(theChainDef,chainDict);

    return theChainDef



def _modifyTEinChainDef(theChainDef,chainDict):
    maxL2SignatureIndex = -1
    for signatureIndex,signature in enumerate(theChainDef.signatureList):
        if signature['listOfTriggerElements'][0][0:2] == "L2":
            maxL2SignatureIndex = max(maxL2SignatureIndex,signatureIndex)            
    inputTEsL2 = theChainDef.signatureList[maxL2SignatureIndex+2]['listOfTriggerElements']    
    for index,item in enumerate(theChainDef.sequenceList):
        if (item['input'] == ['placeHolderTE']):
            theChainDef.sequenceList[index]['input']=inputTEsL2[0]


    return theChainDef


def _addTopoInfo(theChainDef,chainDict,doAtL2AndEF=True):

    #maxL2SignatureIndex = -1
    # for signatureIndex,signature in enumerate(theChainDef.signatureList):
    #     if signature['listOfTriggerElements'][0][0:2] == "L2":
    #         maxL2SignatureIndex = max(maxL2SignatureIndex,signatureIndex)
    
    # inputTEsL2 = theChainDef.signatureList[maxL2SignatureIndex]['listOfTriggerElements']    
    # inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']

    # L2ChainName = "L2_" + chainDict['chainName']
    # EFChainName = "EF_" + chainDict['chainName']
    # HLTChainName = "HLT_" + chainDict['chainName']
    
    # if "Jpsimumu" in chainDict["topo"]:
    #     # to be adapted!!!!!!!!!!!!!!!!!
    #     # from TrigEgammaHypo.TrigL2DielectronMassHypoConfig import TrigL2DielectronMassFex_Jpsiee, TrigL2DielectronMassHypo_Jpsiee
    #     # from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Jpsi, TrigEFDielectronMassHypo_Jpsi

    #     # L2Fex = TrigL2DielectronMassFex_Jpsiee()
    #     # L2Hypo = TrigL2DielectronMassHypo_Jpsiee()        

    #     # EFFex = TrigEFDielectronMassFex_Jpsi()
    #     # EFHypo = TrigEFDielectronMassHypo_Jpsi()


    #     # theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2ChainName)
    #     # theChainDef.addSignatureL2([L2ChainName])

    #     # theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
    #     # theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])
        
    return theChainDef


def _AsymmChainConfig(theChainDef,chainDict):
    maxL2SignatureIndex = -1
    for signatureIndex,signature in enumerate(theChainDef.signatureList):
        print "DEBUG1 : check0 : ", signature;
        if signature['listOfTriggerElements'][0][0:2] == "L2":
            maxL2SignatureIndex = max(maxL2SignatureIndex,signatureIndex)
    
    inputTEsL2 = theChainDef.signatureList[maxL2SignatureIndex]['listOfTriggerElements'] 
    inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']

    idmultiArr = []
    listOfChainDicts = splitChainDict(chainDict)
    totalmulti = 0
    for subChainDict in listOfChainDicts:
        if subChainDict['chainParts']['reccalibInfo'] == "nomucomb":
            totalmulti += int(subChainDict['chainParts']['multiplicity'])
            idmultiArr += ["%03i_AA_%03i" % ( int(subChainDict['chainParts']['threshold']) , int(subChainDict['chainParts']['multiplicity']) )]

    idmultiArr.sort()
    totalmulti_lowest = 0
    totalmulti_2ndlow = 0
    for ii in range(len(idmultiArr)):
        multi = idmultiArr[ii].split("_AA_")[1]
        totalmulti_lowest += int(multi)
        if ii > 0: totalmulti_2ndlow += int(multi)

    idmultiArr2 = []
    for ii in range(len(idmultiArr)):
        thre = idmultiArr[ii].split("_AA_")[0]
        multi = idmultiArr[ii].split("_AA_")[1]
        if ii == 0: idmultiArr2 += [ "%ipt%i" % (totalmulti_lowest,int(thre)) ]
        if ii == 1: idmultiArr2 += [ "%ipt%i" % (totalmulti_2ndlow,int(thre)) ]
        if ii == 2: idmultiArr2 += [ "%ipt%i" % (int(multi),int(thre)) ]
        if ii > 2: print "ERROR : not supported"

    idmulti = "_".join(idmultiArr2)

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonIDTrackMultiHypoConfig
    theTrigMuonIDTrackMultiHypoConfig_FTF = TrigMuonIDTrackMultiHypoConfig( idmulti, "FTF" )
    theTrigMuonIDTrackMultiHypoConfig_Muon = TrigMuonIDTrackMultiHypoConfig( idmulti, "Muon" )

    L2TEname = "L2_idtrkmulti_" + idmulti + "_nomucomb_" + chainDict['L1item']

    theChainDef.addSequence([theTrigMuonIDTrackMultiHypoConfig_FTF], inputTEsL2, L2TEname)
    theChainDef.addSignatureL2([L2TEname])

    EFTEname = "EF_mutrkmulti_" + idmulti + "_nomucomb_" + chainDict['L1item']
    theChainDef.addSequence([theTrigMuonIDTrackMultiHypoConfig_Muon],inputTEsEF, EFTEname);
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFTEname])       


    return theChainDef
