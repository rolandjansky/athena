## Photos++ QED config

from Photospp_i.Photospp_iConf import Photospp_i
genSeq += Photospp_i("Photospp")
genSeq.Photospp.InfraRedCutOff = 1E-7
evgenConfig.generators += ["Photospp"]
