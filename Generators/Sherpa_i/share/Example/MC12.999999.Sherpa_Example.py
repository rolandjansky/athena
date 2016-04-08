evgenConfig.description = "Sherpa example JO, Z+0,1-jet production."
evgenConfig.keywords = [ "Example", "DrellYan" ]
evgenConfig.contact  = ["frank.siegert@cern.ch"]

sherpaRunCard="""
(processes){
  Process 93 93 -> 11 -11 93{1}
  Order_EW 2
  CKKW sqr(20/E_CMS)
  End process;
}(processes)

(selector){
  Mass 11 -11 40 E_CMS
}(selector)
"""

include("MC12JobOptions/Sherpa_CT10_Common.py")
