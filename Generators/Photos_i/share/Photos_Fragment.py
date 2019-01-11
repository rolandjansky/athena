## Photos config
from Photos_i.Photos_iConf import Photos
genSeq += Photos()

genSeq.Photos.PhotosCommand = [
    "photos pmode 1",
    "photos xphcut 0.01",
    "photos alpha -1.",
    "photos interf 1",
    "photos isec 1",
    "photos itre 0",
    "photos iexp 1",
    "photos iftop 0"]

evgenConfig.generators += [ "Photos" ]
