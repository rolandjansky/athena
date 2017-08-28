#!/usr/bin/env python

# @file:    checkTriggerConfig.py
# @purpose: print trigger configuration summary info of a pool file
# @author:  Joerg Stelzer 
# @date:    January 2016
#
# @example:
# @code
# checkTriggerConfig.py aod.pool.root
# @endcode
#

statistics = {}


def _elemFilter(elem):
    if elem.key().startswith('/TRIGGER/'):
        return elem
    return None



def dumpMeta(tt, name):

    print "----------------------------------------------------------------------"

    # Get meta data branch
    br  = tt.GetBranch( name )
    if br == None:
        print "Could not find branch for ",name
        return

    try:
        if br.GetEntry( 0 ) <= 0:
            print ":: Could not get meta data for first event for branch", anem
            pass

        #print "Dumping class", br.GetObjClassName(), " for key ", name
        #  access meta data
        metaData = getattr(tt, br.GetName())
        payloadContainer = metaData.payloadContainer()
        print "Folder %s [size %i]" % (metaData.folderName(),payloadContainer.size())
        statistics[metaData.folderName()] = payloadContainer.size()
        for c, attrListColl in enumerate(payloadContainer):
            #attrListColl.dump()
            for i in range(attrListColl.size()):
                chanNum = attrListColl.chanNum(i)
                iov = attrListColl.iovRange( chanNum )
                print "%i) %s" % (i,iov)
                attrList = attrListColl.attributeList( chanNum )
                for k in attrList.keys():
                    print "       %s : %s" % (k,attrList[k])

    except Exception, ex:
        print "## Caught exception [%s] !!" % str(ex.__class__)
        print "## What:",ex
        import sys
        print sys.exc_info()[0]
        print sys.exc_info()[1]
        sc = 1



def dumpAll(tt, doDetailedDump):

    folders = ['_TRIGGER_HLT_HltConfigKeys', '_TRIGGER_LVL1_Lvl1ConfigKey', '_TRIGGER_LVL1_BunchGroupKey', '_TRIGGER_HLT_PrescaleKey']
    if doDetailedDump:
        folders += [ '_TRIGGER_LVL1_Thresholds', '_TRIGGER_LVL1_Prescales', '_TRIGGER_LVL1_Menu', '_TRIGGER_LVL1_ItemDef', '_TRIGGER_HLT_Prescales', '_TRIGGER_HLT_Menu' ]

    for f in folders:
        dumpMeta(tt, f)





if __name__ == '__main__':

    import sys
    from optparse import OptionParser
    parser = OptionParser(usage="usage: %prog options file [file2 [...]]")
    parser.add_option( "-d", "--detail", action = "store_true", default = False,
                       dest = "printdetail", help = "prints detailed chain definition" )
    (options, args) = parser.parse_args()

    fileNames = [arg for arg in args if arg[0] != "-" ]
    if len(fileNames)==0:
        print "No file specified"
        parser.print_help()
        from sys import exit
        exit(1)


    import cppyy
    import PyCool
    import ROOT

    ROOT.IOVTime.__str__ = lambda x: "%i/%i" % (x.run(),x.event())
    ROOT.IOVTime.__lt__ = lambda s,o: s.run()<o.run() or s.run()==o.run() and s.event()<o.event()
    ROOT.IOVTime.__eq__ = lambda s,o: s.run()==o.run() and s.event()==o.event()
    ROOT.IOVTime.__le__ = lambda s,o: s<o or s==o
    ROOT.IOVRange.__str__ = lambda x: "[%s - %s]" % (x.start(),x.stop())
    ROOT.IOVRange.isInRange = lambda x,e: x.start()<=e and e<x.stop()

    from PyUtils.Helpers import ROOT6Setup
    ROOT6Setup()

    import AthenaROOTAccess.transientTree


    fileName = fileNames[0] # only one file for now
    print "... opening file %s" % fileName
    f = ROOT.TFile.Open(fileName)
    assert f.IsOpen()
    tt = AthenaROOTAccess.transientTree.makeTree( f, persTreeName = 'MetaData', dhTreeName = 'MetaDataHdr',
                                                  dhBranchName = 'DataHeader', dhfilt = _elemFilter )

    print "\n\n\n"

    dumpAll( tt, doDetailedDump = options.printdetail )
