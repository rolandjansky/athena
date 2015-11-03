# Flags for Heavy Ion ESD production
# ----------------------------------
include.block ('HIRecExample/heavyion_flagsESD.py')

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from HIRecExample.HIRecExampleFlags import jobproperties
from HIJetRec.HIJetRecFlags import jobproperties

rec.doDPD.set_Value_and_Lock(False)

if not jobproperties.HIRecExampleFlags.ppMode :

   if rec.doESD :
      print "Applying HI ESD flags mods"

      # AO (standard JetRec turned off)
      from JetRec.JetRecFlags import jetFlags
      jetFlags.Enabled =                  False
      #jetFlags.noStandardConfig =         True

      rec.doRestrictedESD =               False
      
      #turning off TopoClusters
      from CaloRec.CaloRecFlags import jobproperties
      #need this eventually, but for now it breaks egamma isolation
      #jobproperties.CaloRecFlags.doCaloTopoCluster = False
      #jobproperties.CaloRecFlags.doCaloEMTopoCluster = False
      jobproperties.CaloRecFlags.doCaloTopoTower = False
      
      if jobproperties.HIRecExampleFlags.doHIJetRec() and \
             jobproperties.HIRecExampleFlags.doHIegamma() :
         print 'AO: CaloRecFlags doCaloCluster=False'
         jobproperties.CaloRecFlags.doCaloCluster=False  # not working?
         jobproperties.CaloRecFlags.doEmCluster=False
         rec.doEgamma=False

      from egammaRec.egammaRecFlags import jobproperties
      # turning off TopoCaloSeeded
      jobproperties.egammaRecFlags.doTopoCaloSeeded = False
      # turning off not used softe
      #tmp 112014
      #jobproperties.egammaRecFlags.doEgammaTrackSeeded = False 

      rec.doTau = False
      #from JetRec.JetRecFlags import jobproperties
      #jobproperties.JetRecFlags.doBTagging = False
      #from tauRec.tauRecFlags import jobproperties
      #jobproperties.tauRecFlags.doTauRec = False 

      # AO (this turns off all MissingET and EFlow)
      rec.doJetMissingETTag = False
      recAlgs.doMissingET = False
      recAlgs.doMissingETSig = False
      recAlgs.doEFlow = False
      recAlgs.doEFlowJet = False   


      recAlgs.doMuGirl = False
      recAlgs.doMuidLowPt = False
      recAlgs.doCaloTrkMuId = False
      from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
      muonCombinedRecFlags.doMuGirlLowBeta = False


      ptCutVal = 0.5
      if jobproperties.HIRecExampleFlags.ptCutOn():
         ptCutVal = jobproperties.HIRecExampleFlags.ptCut()
         print "ptCutOn: Modify ptCutVal to ", ptCutVal," GeV"
         if ( rec.triggerStream() == "HardProbes" ) or ( globalflags.DataSource == "geant4" ):
            print "Modify ConfiguredNewTrackingCuts__minPT to ", ptCutVal," GeV"
            from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
            InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("HeavyIon")
            InDetNewTrackingCuts._ConfiguredNewTrackingCuts__minPT = ptCutVal * Units.GeV
            InDetNewTrackingCuts.printInfo()
       
############################################################################
