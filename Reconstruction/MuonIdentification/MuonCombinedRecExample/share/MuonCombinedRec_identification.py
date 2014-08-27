#
include.block ("MuonCombinedRecExample/MuonCombinedRec_identification.py")

# 
# InDet + Calo muon identification
#

if rec.doMuonCombined() and muonCombinedRecFlags.doxAOD() and \
      ( rec.readESD() or
        (InDetFlags.Enabled() and not InDetFlags.disableInDetReco() and \
           DetFlags.haveRIO.ID_on() and  DetFlags.haveRIO.Muon_on()) ):
  from MuonCombinedRecExample.MuonCombinedAlgs import MuonCombinedReconstruction
  muonRec = MuonCombinedReconstruction()


