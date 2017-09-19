include("MC15JobOptions/Sherpa_NNPDF30NNLO_Common.py")

evgenConfig.description = "Sherpa 2.2.x example JO, Z+0,1-jet production."
evgenConfig.keywords = [ "Example", "DrellYan" ]
evgenConfig.contact  = [ "atlas-generators-sherpa@cern.ch", "frank.siegert@cern.ch"]

sherpaRunCard="""
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
