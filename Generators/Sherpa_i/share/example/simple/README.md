# Simple Sherpa job option example

Event generation jobs in ATLAS are steered using jobOption files and run using the `Gen_tf.py` transform as described in [PmgMcSoftware#Production_transforms_and_job_op](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PmgMcSoftware#Production_transforms_and_job_op).

A simple example JO for running Sherpa on-the-fly within Athena is contained in this directory and will be explained below:

```
include("Sherpa_i/Base_Fragment.py")
include("Sherpa_i/NNPDF30NNLO.py")

evgenConfig.description = "Sherpa 2.2.x example JO, Z+0,1-jet production."
evgenConfig.keywords = [ "2lepton" ]
evgenConfig.contact  = [ "atlas-generators-sherpa@cern.ch", "frank.siegert@cern.ch"]
evgenConfig.nEventsPerJob = 10000

genSeq.Sherpa_i.RunCard="""
(run){
  ME_SIGNAL_GENERATOR=Amegic
}(run)

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
```

Features from top to bottom:
* Common include files containing all the default settings, which can be found in the [Sherpa_i interface package of Athena](https://gitlab.cern.ch/atlas/athena/tree/21.6/Generators/Sherpa_i/share/common)
* Evgen metadata
* A Sherpa run card containing the process-specific settings, e.g. hard scattering setup, ME level cuts, model settings. For details on how to write this part of the JO file, please refer to the [Sherpa manual](https://sherpa-team.gitlab.io/)
* All Sherpa parameters can also be set/overwritten later within the JO file by adding to the `genSeq.Sherpa_i.Parameters` array. Every Sherpa setting of the type parameter=value entered here will be treated in the same way as on the Sherpa command line (spaces are not allowed). Example:```
genSeq.Sherpa_i.Parameters += [ "MI_HANDLER=None", "FRAGMENTATION=Off" ]```
* The Athena interface will automatically set some parameters to Atlas defaults, regardless of whether you have set them in the run card snippet or not (cf. the included files from [Sherpa_i](https://gitlab.cern.ch/atlas/athena/tree/21.6/Generators/Sherpa_i/share/common) for a list of them). These can only be overwritten through the `genSeq.Sherpa_i.Parameters` setting (not in the run card snippet).
* Note that the base fragments will automatically add event weights for hundreds of on-the-fly systematic variations. If you do not want those, you can switch them off by adding ```
genSeq.Sherpa_i.Parameters += [ "SCALE_VARIATIONS=None", "PDF_VARIATIONS=None" ] ```
