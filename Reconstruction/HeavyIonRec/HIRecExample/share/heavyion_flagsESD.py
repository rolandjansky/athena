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

      # temp cutLevel setting in HI config
      from InDetRecExample.InDetJobProperties import InDetFlags
      InDetFlags.cutLevel.set_Value_and_Lock(2)

      # AO (this turns off all MissingET and EFlow)
      rec.doJetMissingETTag =             False

      # AO (standard JetRec turned off)
      from JetRec.JetRecFlags import jetFlags
      jetFlags.Enabled =                  False
      jetFlags.noStandardConfig =         True

      rec.doRestrictedESD =               False

      from CaloRec.CaloRecFlags import jobproperties
      jobproperties.CaloRecFlags.doEmCluster = True
      jobproperties.CaloRecFlags.doCaloCluster = True
      
      #turning off TopoClusters
      jobproperties.CaloRecFlags.doCaloTopoCluster = False
      jobproperties.CaloRecFlags.doCaloEMTopoCluster = False
      jobproperties.CaloRecFlags.doCaloTopoTower = False
      
      from egammaRec.egammaRecFlags import jobproperties
      # turning off TopoCaloSeeded
      jobproperties.egammaRecFlags.doTopoCaloSeeded = False
      # turning off not used softe
      jobproperties.egammaRecFlags.doEgammaTrackSeeded = False 

      from JetRec.JetRecFlags import jobproperties
      jobproperties.JetRecFlags.doBTagging = False
      from tauRec.tauRecFlags import jobproperties
      jobproperties.tauRecFlags.doTauRec = False 

      recAlgs.doMuidLowPt = False
      recAlgs.doEFlow = False
      recAlgs.doEFlowJet = False   

      recAlgs.doMuGirl = False
      recAlgs.doCaloTrkMuId = False

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
