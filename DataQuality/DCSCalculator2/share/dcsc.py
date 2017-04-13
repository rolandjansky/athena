#! /usr/bin/env python

from DCSCalculator2.main import main

if __name__ == "__main__":
    import sys
    
    # Make a copy of argv and empty the system argv to prevent 
    # ROOT from intercepting arguments. This might be necessary is main() is 
    # called from elsewhere
    our_argv = sys.argv[:]
    #sys.argv[:] = []
    #sys.original_argv = our_argv
    
    main(our_argv)
