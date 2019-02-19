include("Sherpa_i/2.2.5_NNPDF30NNLO.py")

evgenConfig.description = "Sherpa 2.2.x example JO, Z+0,1-jet production."
evgenConfig.keywords = [ "2lepton" ]
evgenConfig.contact  = [ "atlas-generators-sherpa@cern.ch", "frank.siegert@cern.ch"]

genSeq.Sherpa_i.RunCard="""
(processes){
  Process 93 93 -> 11 -11 93{1}
  Order (*,2)
  CKKW sqr(20/E_CMS)
  End process;
}(processes)

(selector){
  Mass 11 -11 40 E_CMS
}(selector)
"""

genSeq.Sherpa_i.Parameters += []
genSeq.Sherpa_i.OpenLoopsLibs = []
genSeq.Sherpa_i.ExtraFiles = []
genSeq.Sherpa_i.NCores = 1

genSeq.Sherpa_i.CleanupGeneratedFiles = 1
