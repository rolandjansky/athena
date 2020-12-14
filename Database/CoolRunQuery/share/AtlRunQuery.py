#!/bin/env python
#
# ----------------------------------------------------------------
# Script : AtlRunQuery.py
# Project: AtlRunQuery
# Purpose: Main
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Nov 13, 2008
# ----------------------------------------------------------------
#

import sys, getopt, os

def main():
    arguments = sys.argv

    if len(arguments) <= 1:
        print('No query argument given')
        return 1

    fileindex = None

    if arguments[1] == 'fileindex':
        installpath = '/'.join(os.path.dirname(__file__).split('/')[:-1])
        fileindex = arguments[2]
        datapath = '/data/arq_%s/arq_%s' % (fileindex[:6],fileindex)
        #queryfile = '%s/%s/query.txt' % (installpath,datapath)
        queryfile = '%s/query.txt' % datapath
        fh = open(queryfile,"r")
        origQuery = fh.readline().strip()
        fh.close()
        arguments[1:] = origQuery.split()
    else:
        datapath='data'
        origQuery = ' '.join(arguments[1:])
    
    ignoreParser = origQuery.startswith('--')

    from CoolRunQuery.AtlRunQueryOptions import AtlRunQueryOptions
    if ignoreParser:

        (options, args) = AtlRunQueryOptions().parse()
        atlqueryarg = origQuery

    else:

        from CoolRunQuery.AtlRunQueryParser import ArgumentParser
        ap = ArgumentParser()

        # special treatment of some extra tools: pileup calc, det mask decoding, help(not implemented)
        firstarg = arguments[1].lower()
        if firstarg in ['pileup','detmask','help']:
            tabletemplate = '<table class="othertable" style="width:60%%"><tr><th>Query result:</th></tr><tr><td>\n%s</td></tr></table><p></p><br><p></p>'
            if firstarg == 'pileup':
                from CoolRunQuery.AtlRunQueryUtils import Pileup
                body = tabletemplate % (Pileup(arguments[2:]))
            elif firstarg == 'detmask':
                from CoolRunQuery.utils.AtlRunQueryLookup import DecodeDetectorMask
                body = tabletemplate % ( DecodeDetectorMask( arguments[2], isRun2=True ))
            else:
                body = ''

            # return a default page
            from CoolRunQuery.html.AtlRunQueryPageMaker import PageMaker
            from CoolRunQuery.AtlRunQueryQueryConfig import QC
            QC.datapath = datapath
            PageMaker.makePage(body, origQuery, removeExamples=True)
            return 0

        atlqueryarg = arguments[0].rsplit('/',1)[-1] + " " + ap.ParseArgument( ' '.join(arguments[1:]) )

        if '--verbose' in atlqueryarg:
            print(atlqueryarg)

        (options, args) = AtlRunQueryOptions().parse(atlqueryarg)


    from CoolRunQuery.AtlRunQueryLib import AtlRunQuery
    print("Data path %s" % datapath)
    AtlRunQuery(options, html="AUTO", origQuery=origQuery, datapath=datapath, parsedstring=atlqueryarg)  # html can be "YES", "NO", "AUTO"
    return 0

if __name__ == '__main__':
    #import cProfile
    #cProfile.run('main()','prof')
    import sys
    sys.exit(main())
