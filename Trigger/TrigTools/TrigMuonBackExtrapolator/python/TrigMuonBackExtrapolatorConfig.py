# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigMuonBackExtrapolator.TrigMuonBackExtrapolatorConf import TrigMuonBackExtrapolator

class MuonBackExtrapolatorForAlignedDet (TrigMuonBackExtrapolator):

    def __init__( self, name="AlignedBackExtrapolator" ):
        super( MuonBackExtrapolatorForAlignedDet, self ).__init__( name )

        self.Aligned = True
        self.DataSet = False


class MuonBackExtrapolatorForMisalignedDet (TrigMuonBackExtrapolator):

    def __init__( self, name="MisalignedBackExtrapolator" ):
        super( MuonBackExtrapolatorForMisalignedDet, self ).__init__( name )

        self.Aligned = False
        self.DataSet = False

class MuonBackExtrapolatorForData (TrigMuonBackExtrapolator):

    def __init__( self, name="DataBackExtrapolator" ):
        super( MuonBackExtrapolatorForData, self ).__init__( name )

        self.Aligned = False
        self.DataSet = True
