# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Simple utility class to provide channel number - name mappings
class LumiChannelDefs:

    # Define here to give class scope
    names = dict()
    
    # Mapping of channel numbers to channel names
    names[0] = 'ATLAS Preferred'
    names[1] = 'LHC Lumi'
    names[5] = 'CMS Lumi'
    names[17] = 'ATLAS Preferred BGRP7'
    
    names[101] = 'Lucid Evt AND'
    names[102] = 'Lucid Evt OR'
    names[103] = 'Lucid Hit OR'
    names[104] = 'Lucid Hit AND'
    names[105] = 'Lucid Evt A'
    names[106] = 'Lucid Evt C'

    names[111] = 'LucidBis Evt AND'
    names[112] = 'LucidBis Evt OR'
    names[113] = 'LucidBis Hit OR'
    names[114] = 'LucidBis Hit AND'
    names[115] = 'LucidBis Evt A'
    names[116] = 'LucidBis Evt C'

    names[121] = 'LucidMod Evt AND'
    names[122] = 'LucidMod Evt OR'
    names[123] = 'LucidMod Hit OR'
    names[124] = 'LucidMod Hit AND'
    names[125] = 'LucidMod Evt A'
    names[126] = 'LucidMod Evt C'

    names[131] = 'LucidBis A1'
    names[132] = 'LucidBis A5'
    names[133] = 'LucidBis A9'
    names[134] = 'LucidBis A13'
    names[135] = 'LucidBis C1'
    names[136] = 'LucidBis C5'
    names[137] = 'LucidBis C9'
    names[138] = 'LucidBis C13'
    
    # Lucid Charge algorithms
    names[151] = 'Lucid Charge A'
    names[152] = 'Lucid Charge C'
    names[153] = 'LucidBis Charge A'
    names[154] = 'LucidBis Charge C'
    names[155] = 'LucidFib Charge A'
    names[156] = 'LucidFib Charge C'

    # BCM    
    names[201] = 'BCMH Evt OR'
    names[202] = 'BCMH Evt AND'
    names[205] = 'BCMH ORA'
    names[206] = 'BCMH ORC'

    names[211] = 'BCMV Evt OR'
    names[212] = 'BCMV Evt AND'
    names[215] = 'BCMV ORA'
    names[216] = 'BCMV ORC'

    names[221] = 'BCMT Evt OR'
    names[222] = 'BCMT Evt AND'
    names[225] = 'BCMT ORA'
    names[226] = 'BCMT ORC'

    names[231] = 'BCMH Early Evt OR'
    names[235] = 'BCMH Early Evt A'
    names[236] = 'BCMH Early Evt C'

    names[241] = 'BCMV Early Evt OR'
    names[245] = 'BCMV Early Evt A'
    names[246] = 'BCMV Early Evt C'

    # MBTS    
    names[301] = 'MBTS Evt OR'
    names[302] = 'MBTS Evt AND'
    names[303] = 'MBTS2'
    
    names[501] = 'FCAL_A' 
    names[502] = 'FCAL_C'

    
    # Sorted list of valid channel numbers
    numbers = sorted(names.keys())
    
    def name(self, chan):
        return self.names.get(chan, 'Unknown')
