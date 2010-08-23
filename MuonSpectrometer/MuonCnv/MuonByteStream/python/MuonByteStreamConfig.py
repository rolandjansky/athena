# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonByteStream.MuonByteStreamConf import *
from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags

print "WARNING: ",__name__,"is deprecated. Please use instead: import MuonRecExample.MuonReadBSConfig "

class TgcROD_DecoderConfig (TgcROD_Decoder):

    def __init__(self,name = Configurable.DefaultName ):
        super(TgcROD_DecoderConfig ,self).__init__(name)

        if muonByteStreamFlags.TgcDataType()=='m3':
            self.TgcSpecialROBNumber = 0x680019

    def setDefaults(cls,handle):
        if hasattr(handle,'TgcSpecialROBNumber'):
            if handle.TgcSpecialROBNumber != -1:
                print "TgcROD_Decoder uses a special ROB number = ",handle.TgcSpecialROBNumber


