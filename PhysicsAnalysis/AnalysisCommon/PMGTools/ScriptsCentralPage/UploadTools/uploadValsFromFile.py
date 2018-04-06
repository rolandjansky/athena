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



          
    currentfile=UT.getCurrentVals(UT.getDSListFromFile(opts.infile),UT.getFieldsInFile(opts.infile))
    UT.currentVals=UT.getDetailsFromFile(currentfile)
    #print UT.currentVals

    UT.newVals=uploadVals=UT.getDetailsFromFile(opts.infile)
    #print uploadVals

    for ds in uploadVals:
        if ds in UT.currentVals:
            print cols.OKGREEN+"INFO: Working on datset %s"%(ds)+cols.ENDC
            for param in uploadVals[ds]:
                if param in UT.currentVals[ds]:
                    if len(uploadVals[ds][param].strip()) or uploadVals[ds][param].strip()=="#UNKNOWN#":
                        newval=uploadVals[ds][param]
                        currentval=UT.currentVals[ds][param]
                        if param in ["genFiltEff","crossSection","kFactor"] and "unknown" not in newval.lower() and "unknown" not in currentval.lower():
                            newval=float(uploadVals[ds][param])
                            currentval=float(UT.currentVals[ds][param])
                        if newval!=currentval:
                            print cols.WARNING+"INFO: Upload %s = %s (current val = %s)"%(param,uploadVals[ds][param],UT.currentVals[ds][param])+cols.ENDC
                            UT.updateValue(ds,param,uploadVals[ds][param],opts.explanation,uploadVals[ds]["crossSectionRef"])
                        else:
                            print "INFO: Inputted %s value is identical to current DB value (%s) - ignoring"%(param,UT.currentVals[ds][param])
                    else:
                        print "INFO: %s value empty or unknown - ignoring"%(param)
                else:
                    print "WARNING: Parameter %s not found in current AMI vals dictionary for %s"%(param,ds)
        else:
            print "WARNING: Parameter dataset %s not found in current AMI val dictionary"%(ds)

    #UT.updateXS("test","mc15","1234","Just a test")

if __name__ == '__main__':
    main()

