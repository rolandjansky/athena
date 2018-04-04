#!/usr/bin/env python 


import uploadTools
basepath="/afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/ScriptsCentralPage/"

def main():

    # parse options from command line
    from optparse import OptionParser
    parser = OptionParser(usage = "usage: %prog arguments", version="%prog")
    parser.add_option("-v", "--verbose",    action="store_true", dest="verbose",    help="Set verbose mode (default: %default)")
    parser.add_option("-y", "--alwaysyes",  action="store_true", dest="alwaysyes",  help="Always answer yes to checks (default: %default)")
    parser.add_option("-i", "--infile",     action="store",      dest="infile",     help="Input file (default: %default)")
    parser.add_option("-x", "--xsreffile",  action="store",      dest="xsreffile",  help="Cross section reference file (default: %default)")
    parser.add_option("-t", "--istest",     action="store",      dest="istest",     help="Is test (no actual upload to AMI) (default: %default)")
    parser.add_option("-d", "--delim",      action="store",      dest="delim",      help="Delimiter for input file (default: %default)")
    parser.add_option("-e", "--explanation",action="store",      dest="explanation",help="Comment for explanation field (default: %default)")
    parser.add_option("-D", "--DS",         action="store",      dest="DS",         help="DS to modify (default: %default)")
    parser.add_option("-p", "--param",      action="store",      dest="param",      help="Parameter to modify (default: %default)")
    parser.add_option("-V", "--value",      action="store",      dest="value",      help="Value to upload (default: %default)")
    
    parser.set_defaults(verbose=False,alwaysyes=False,infile=None,xsreffile=basepath+"InputInformation/CrossSectionReference",istest="TRUE",delim=';',explanation=None)

    (opts, args) = parser.parse_args()

    if not opts.explanation:
        print "ERROR: No explanation found - Should be JIRA ticket or similar"
        return
    elif 'CENTRPAGE' not in opts.explanation:
        print "WARNING: Explanation found, but does not include CENTRPAGE - this should usually be a JIRA ticket"

    if opts.istest=="FALSE":
        opts.istest=False

    UT=uploadTools.UT(istest=opts.istest,verbose=opts.verbose,alwaysyes=opts.alwaysyes,xsRefFile=opts.xsreffile if hasattr(opts,"xsreffile") else None,delim=opts.delim if hasattr(opts,"delim") else None)

    cols=UT.colors

    if not opts.istest:
        print cols.FAIL+"INFO: THIS IS NOT A TEST - UPLOADING FOR REALS!"+cols.ENDC
    else:
       print cols.OKGREEN+"INFO: THIS IS A TEST - Nothing will be uploaded!"+cols.ENDC
       


    UT.updateValue(opts.DS,opts.param,opts.value,opts.explanation)

if __name__ == '__main__':
    main()
