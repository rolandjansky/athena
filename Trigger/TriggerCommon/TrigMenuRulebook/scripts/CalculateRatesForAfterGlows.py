# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



def main():                                                                                                                                                          
        
    from optparse import OptionParser                                                                                                                                
    p = OptionParser(usage = "usage: python CaclulateRateForAfterGlows.py", version = "1.0")                                                                                       

    p.add_option('--MAXOUTPUT',       type  = "int",         dest = "maxoutput",          help = "Max sustainble output rate in Hz ")                                       
    p.add_option('--FILLED',         type  = "int",         dest = "filled",          help = "# filled bunches ")                                       
    p.add_option('--UNPAIRED_ISO',    type  = "int",        dest = "unpaired_iso",    help = "# unpaired_iso bunches " )
    p.add_option('--BGRP9',           type  = "int",        dest = "bgrp9",           help = "# bunches in BGRP9 " )
    
    (options, args) = p.parse_args()
    
    #lumiPeb events are 20% of the normal event size, hence 2kHz full-event <-> 10kHz lumipeb
    maxout= options.maxoutput if options.maxoutput else 10e3

    #max Random Rate/BCID (otherwise, FFVT kicks in)
    maxin=5e3

    nbunches= options.filled +options.unpaired_iso + options.bgrp9
    
    rate_bcid = maxout/nbunches

    time= 250e3/rate_bcid
    
    m, s = divmod(time, 60)
    h, m = divmod(m, 60)

    print "Run lumipeb_vdm_L1RD0_BGRP9 with rate :        ", options.bgrp9*rate_bcid   
    print "Run lumipeb_vdm_L1RD0_FILLED with rate :       ", options.filled*rate_bcid      
    print "Run lumipeb_vdm_L1RD0_UNPAIRED_ISO with rate : ", options.bgrp9*rate_bcid  
    print "Rate per BCID: %d, hence need to run for at least %d:%02d hours to collect 250K events/BCID " %(rate_bcid,h,m)


if __name__ == '__main__':
    main()
    

