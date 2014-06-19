## Photos++ QED config

from Photospp_i.Photospp_iConf import Photospp_i
topAlg += Photospp_i("Photospp")
topAlg.Photospp.InfraRedCutOff = 1E-7
evgenConfig.generators += ["Photospp"]
