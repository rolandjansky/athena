# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DCSCalculator2.variable import CodeIOV
from DCSCalculator2.lib import DCSC_Subdetector, DCSC_Global_Variable
from DCSCalculator2.consts import BLACK, WHITE, GREY, RED, YELLOW, GREEN

IDBS = 150
    
class IDBS_Beampos(DCSC_Global_Variable):
    
    input_db = "COOLOFL_INDET/CONDBR2"
    timewise_folder = False
    fetch_args = dict(tag="IndetBeampos_cosmic_loose-RUN2")
    
    STATUSMAP = {
        59 : GREEN,
        83 : YELLOW,
        0  : GREY,
    }
    
    def make_good_iov(self, iov):
        """
        Logic to choose the colour code the calculator should write
        """
        state = self.STATUSMAP.get(iov.status, GREY)
        return CodeIOV(iov.since, iov.until, iov.channel, state)

class IDBS(DCSC_Subdetector):
    
    folder_base = "/Indet"
    
    mapping = {
        IDBS: [0],
    }
    
    variables = [
        IDBS_Beampos("Beampos", None),
    ]
