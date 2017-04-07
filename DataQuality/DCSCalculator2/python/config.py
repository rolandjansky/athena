# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from optparse import OptionParser

opts, args = None, None

def parse_options(argv):
    if argv:
        from DCSCalculator2.subdetectors import SYS_NAMES
    else:
        SYS_NAMES = "-"
    
    # Parse options
    optp = OptionParser(
        usage="Usage: %prog [--help] [options]",
        prog="" if not argv else argv[0],
        description="ATLAS DCS summariser"
    )
    
    O = optp.add_option
    
    O('-r', '--run', type=int, help="Run number to process")
    
    O('-R', '--range', help="Run range: e.g. 150000-151000")
    
    O('-L', '--lbs', help="Specify a luminosity block range")
    
    O('-s', '--system', dest="systems", action="append", metavar="system", 
      help="Valid systems: %s. Specify once for each system. (Default: all)" % SYS_NAMES)
                    
    O('-d', '--input-database', default="CONDBR2",
      help="Database instance to use for input (Default: CONDBR2)")

    O('-o', '--output-database', default="None",
      help="(Default: None. Other valid input: any COOL connection string, "
           "or sqlite filename)")
    
    O('-v', '--verbose', action='count', default=0,
      help="Increase verbosity (specify multiple times for more)")
                                    
    O('-e', '--shell-on-exception', action="store_true",
      help="Drop to ipython shell on exception")
      
    O('-X', '--dont-ignore-system-exceptions', action="store_true",
      help="Instead of supressing system exceptions, raise them (useful with -e)")
    
    O('-T', '--timewise', action="store_true",
      help="Compute result in time rather than LBs")
                    
    O('-U', '--mark-unfilled-grey', action="store_true",
      help="If there are any unfilled channels, mark that LB grey.")
                    
    O('-t', '--tally-system-states', action="store_true",
      help="Tally system states (only makes sense for discrete "
                         "states)")
                    
    #O('-P', '--parallel', default=0, type=int, metavar="N",
      #help="Number of systems to run in parallel (-1 for all)")
      
    O('--check-input-time', action="store_true",
      help="Print statistics on when input IoVs became available")

    O('--email-on-failure', action="store_true",
      help="Send an email to atlas-dqm-core if a system fails")

    global opts, args                
    opts, args = optp.parse_args(argv)
    
    return optp, opts, args

# Set some default options
parse_options([])
