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
    
    names[101] = 'Onl Lucid Evt AND'
    names[102] = 'Onl Lucid Evt OR'
    names[103] = 'Onl Lucid Hit OR'
    names[104] = 'Onl Lucid Hit AND'
    names[105] = 'Onl Lucid Evt A'
    names[106] = 'Onl Lucid Evt C'

    names[111] = 'Onl LucidFib Evt AND'
    names[112] = 'Onl LucidFib Evt OR'
    names[113] = 'Onl LucidFib Hit OR'
    names[114] = 'Onl LucidFib Hit AND'
    names[115] = 'Onl LucidFib Evt A'
    names[116] = 'Onl LucidFib Evt C'
    
    # Offline versions of online numbers
    names[151] = 'Lucid Evt AND'
    names[152] = 'Lucid Evt OR'
    names[153] = 'Lucid Hit OR'
    names[154] = 'Lucid Hit AND'
    names[155] = 'Lucid Evt A'
    names[156] = 'Lucid Evt C'
    
    names[161] = 'LucidFib Evt AND'
    names[162] = 'LucidFib Evt OR'
    names[163] = 'LucidFib Hit OR'
    names[164] = 'LucidFib Hit AND'
    names[165] = 'LucidFib Evt A'
    names[166] = 'LucidFib Evt C'
    
    names[201] = 'Onl BCMH Evt OR'
    names[202] = 'Onl BCMH Evt AND'
    names[203] = 'Onl BCMH XORA'
    names[204] = 'Onl BCMH XORC'
    names[205] = 'Onl BCMH ORA'
    names[206] = 'Onl BCMH ORC'
    names[207] = 'Onl BCMH AND25'

    names[211] = 'Onl BCMV Evt OR'
    names[212] = 'Onl BCMV Evt AND'
    names[213] = 'Onl BCMV XORA'
    names[214] = 'Onl BCMV XORC'
    names[216] = 'Onl BCMV ORC'
    names[217] = 'Onl BCMV AND25'

    names[221] = 'Onl BCMT Evt OR'
    names[222] = 'Onl BCMT Evt AND'
    names[226] = 'Onl BCMT ORC'
    
    names[251] = 'BCMH Evt OR'
    names[252] = 'BCMH Evt AND'
    names[253] = 'BCMH XORC'
    names[254] = 'BCMH XORA'
    names[255] = 'BCMH ORA'
    names[256] = 'BCMH ORC'
    names[257] = 'BCMH AND25'
    
    names[261] = 'BCMV Evt OR'
    names[262] = 'BCMV Evt AND'
    names[263] = 'BCMV XORC'
    names[264] = 'BCMV XORA'
    names[266] = 'BCMV ORC'
    names[267] = 'BCMV AND25'
    
    names[271] = 'BCMT Evt OR'
    names[272] = 'BCMT Evt AND'
    names[276] = 'BCMT ORC'
    
    names[301] = 'Onl MBTS Evt OR'
    names[302] = 'Onl MBTS Evt AND'
    names[303] = 'Onl MBTS Hit OR'
    
    names[351] = 'MBTS Evt OR'
    names[352] = 'MBTS Evt AND'
    names[353] = 'MBTS Hit OR'
    
    names[401] = 'Onl ZDC Evt AND'
    names[402] = 'Onl ZDC Evt ORA'
    names[403] = 'Onl ZDC Evt ORC'

    names[411] = 'Onl ZDCL Evt AND' 
    names[412] = 'Onl ZDCL Evt OR' 
    names[413] = 'Onl ZDCL Evt ORA' 
    names[414] = 'Onl ZDCL Evt ORC' 
    
    names[451] = 'ZDC Evt AND'
    names[452] = 'ZDC Evt ORA'
    names[453] = 'ZDC Evt ORC'

    names[461] = 'ZDCL Evt AND' 
    names[462] = 'ZDCL Evt OR' 
    names[463] = 'ZDCL Evt ORA' 
    names[464] = 'ZDCL Evt ORC'

    names[501] = 'FCAL_A' 
    names[502] = 'FCAL_C'

    names[521] = 'RPC'
    
    names[902] = 'Vtx Count'

    names[1001] = 'Onl BCM11H Evt OR'
    names[1002] = 'Onl BCM11H Evt AND'
    names[1004] = 'Onl BCM11H Evt XORC'
    
    names[1011] = 'Onl BCM11V Evt OR'
    names[1012] = 'Onl BCM11V Evt AND'
    names[1014] = 'Onl BCM11V Evt XORC'
    
    names[1051] = 'BCM11H Evt OR'
    names[1052] = 'BCM11H Evt AND'
    names[1054] = 'BCM11H Evt XORC'
    
    names[1061] = 'BCM11V Evt OR'
    names[1062] = 'BCM11V Evt AND'
    names[1064] = 'BCM11V Evt XORC'
    
    # Sorted list of valid channel numbers
    numbers = sorted(names.keys())
    
    def name(self, chan):
        return self.names.get(chan, 'Unknown')
