#!/usr/bin/env python

def get_status(rfile):

    import ROOT, PyCintex
    ROOT.gSystem.Load('libTrigMonitoringEventDict')
    ROOT.gSystem.Load('libPhysicsInfoDict')
    
    file = ROOT.TFile(rfile)
    
    tree = file.Get("event")
    tree.Print()
    file.Close()

###########################################################################
# Main function for command line execuation
#
if __name__ == "__main__":

    from optparse import OptionParser
    
    p = OptionParser(usage="usage: -r <ROOT file> -l <log file>", version="1.0")

    p.add_option( "-r",
                  type    = "string",
                  dest    = "root_file",
                  default = "",
                  help    = "path to ROOT file" )

    p.add_option( "-l",
                  type    = "string",
                  dest    = "log_file",
                  default = "",
                  help    = "path to log file" )

    (options, args) = p.parse_args()  

    if options.root_file == None or len(options.root_file) < 2:
        p.error("Missing input ROOT file")

    get_status(options.root_file)
