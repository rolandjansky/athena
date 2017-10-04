# Flags for Heavy Ion ESD production
# ----------------------------------
include.block ('HIRecExample/heavyion_flagsESD.py')

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags
from HIRecExample.HIRecExampleFlags import jobproperties
from HIJetRec.HIJetRecFlags import jobproperties

rec.doDPD.set_Value_and_Lock(False)

if not jobproperties.HIRecExampleFlags.ppMode :

   if rec.doESD :
      print "Applying HI ESD flags mods"

      # AO general
      rec.Commissioning.set_Value_and_Lock(True)
      rec.doRestrictedESD.set_Value_and_Lock(False)
      
      # AO (standard JetRec turned off)
      from JetRec.JetRecFlags import jetFlags
      jetFlags.Enabled = False
      #jetFlags.noStandardConfig = True

      # AO InDet 2015
      InDetFlags.cutLevel = 3
      InDetFlags.doSGDeletion = False
      InDetFlags.perigeeExpression.set_Value_and_Lock('Vertex')

      from CaloRec.CaloRecFlags import jobproperties
      #need this eventually, but for now it breaks egamma isolation
      #needed for now in rel. 21 for egamma reco
      #jobproperties.CaloRecFlags.doCaloTopoCluster = False
      jobproperties.CaloRecFlags.doCaloEMTopoCluster = False
      jobproperties.CaloRecFlags.doCaloTopoTower = False
      # 2015 data, no pileup
      from CaloTools.CaloNoiseFlags import jobproperties
      jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(0)
      from CaloRec.CaloCellFlags import jobproperties
      jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False)
      # MC, no pileup
      if ( globalflags.DataSource == "geant4" ):
         from CaloRec.CaloCellFlags import jobproperties
         jobproperties.CaloCellFlags.doLArCellEmMisCalib = False
      
      if jobproperties.HIRecExampleFlags.doHIJetRec() and \
             jobproperties.HIRecExampleFlags.doHIegamma() :
         # 2015 fix: cell subtraction
         from egammaRec.egammaRecFlags import jobproperties
         jobproperties.egammaRecFlags.cellContainerName.set_On()
         jobproperties.egammaRecFlags.cellContainerName.set_Value_and_Lock("SubtractedCells")

         print 'AO: CaloRecFlags doCaloCluster=False'
         jobproperties.CaloRecFlags.doCaloCluster=False  # not working?
         jobproperties.CaloRecFlags.doEmCluster=False
         #turning off TopoClusters
         #jobproperties.CaloRecFlags.doCaloTopoCluster = False
         #jobproperties.CaloRecFlags.doCaloEMTopoCluster = False
      
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
