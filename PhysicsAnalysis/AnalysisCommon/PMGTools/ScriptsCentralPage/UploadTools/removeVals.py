#!/usr/bin/env python 

import uploadTools

basepath="/afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/ScriptsCentralPage/"

def main():

    # parse options from command line
    from optparse import OptionParser
    parser = OptionParser(usage = "usage: %prog arguments", version="%prog")
    parser.add_option("-v", "--verbose",    action="store_true", dest="verbose",    help="Set verbose mode (default: %default)")
    parser.add_option("-y", "--alwaysyes",  action="store_true", dest="alwaysyes",  help="Always answer yes to checks (default: %default)")
    parser.add_option("-t", "--istest",     action="store",      dest="istest",     help="Is test (no actual upload to AMI) (default: %default)")
    parser.add_option("-e", "--explanation",action="store",      dest="explanation",help="Comment for explanation field (default: %default)")
    parser.add_option("-d", "--dataset",    action="store",      dest="dataset",    help="Dataset to remove value from (default: %default)")
    parser.add_option("-p", "--param",      action="store",      dest="param",      help="Parameter to remove (default: %default)")
    
    parser.set_defaults(verbose=False,alwaysyes=False,istest="TRUE",explanation=None,dataset=None,param=None)

    (opts, args) = parser.parse_args()

    if not opts.explanation:
        print "ERROR: No explanation found - Should be JIRA ticket or similar"
        return
    elif 'CENTRPAGE' not in opts.explanation:
        print "WARNING: Explanation found, but does not include CENTRPAGE - this should usually be a JIRA ticket"

    if not opts.dataset:
        print "ERROR: No dataset found"
        return

    if not opts.param:
        print "ERROR: No parameter found"
        return
    
    if opts.istest=="FALSE":
        opts.istest=False

    UT=uploadTools.UT(opts=opts)
    cols=UT.colors

    if not opts.istest:
        print cols.FAIL+"INFO: THIS IS NOT A TEST - UPLOADING FOR REALS!"+cols.ENDC
    else:
        print cols.OKGREEN+"INFO: THIS IS A TEST - Nothing will be uploaded!"+cols.ENDC


    UT.removeParamEntry(opts.dataset,UT.getScope(opts.dataset),opts.param,opts.explanation)
          

if __name__ == '__main__':
    main()

