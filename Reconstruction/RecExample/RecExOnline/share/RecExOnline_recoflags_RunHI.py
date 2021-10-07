## -------------------------------------------------------------------

## default settings
if (not 'doAllReco' in dir()):
  doAllReco = True

if (not 'doInDet' in dir()):
  doInDet = doAllReco

if (not 'doMuon' in dir()):
  doMuon = doAllReco

if (not 'doLArg' in dir()):
  doLArg = doAllReco

if (not 'doTile' in dir()):
  doTile = doAllReco

if (not 'doLucid' in dir()):
  doLucid = False

if (not 'doZdc' in dir()):
  doZdc = False

if (not 'doAlfa' in dir()):
  doAlfa = False

if (not 'doTrigger' in dir()):
  doTrigger = False

if (not 'doHist' in dir()):
  doHist = True

if (not 'doJiveXML' in dir()):
  doJiveXML = False

if (not 'doEgammaTau' in dir()):
  doEgammaTau = doAllReco

if (not 'doCommissioning' in dir()):
  doCommissioning = False

if (not 'doHIRec' in dir()):
  doHIRec = False

## ------------------------------------------------------------------- Reco flags

rec.doInDet.set_Value_and_Lock(doInDet)
rec.doMuon.set_Value_and_Lock(doMuon)                                      
rec.doLArg.set_Value_and_Lock(doLArg)                                      
rec.doTile.set_Value_and_Lock(doTile)                                      
rec.doLucid.set_Value_and_Lock(doLucid)
rec.doZdc.set_Value_and_Lock(doZdc)
rec.doAlfa.set_Value_and_Lock(doAlfa)
rec.doForwardDet.set_Value_and_Lock(doLucid or doZdc or doAlfa)
rec.doTrigger.set_Value_and_Lock(doTrigger)

from RecExConfig.RecAlgsFlags import recAlgs

rec.doHeavyIon.set_Value_and_Lock(doHIRec)

#rec.doHIP.set_Value_and_Lock(True)

## --------------------- Added by S. Sun to disable Muon Girl algorithm -----------------------------##

recAlgs.doMuGirl.set_Value_and_Lock(False)

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
muonCombinedRecFlags.doMuGirl.set_Value_and_Lock(False)

## --------------------------------------------------------------------------------------------------##

rec.doHist.set_Value_and_Lock(doHist)
rec.doJiveXML.set_Value_and_Lock(doJiveXML)                               
rec.doEgamma.set_Value_and_Lock(doEgammaTau)
rec.doTau.set_Value_and_Lock(doEgammaTau)

rec.Commissioning.set_Value_and_Lock(True) # set to True by default
