#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import sys, argparse
from TrigConfMuctpi.XMLReader import MioctGeometryXMLReader


def readXML(filename):
    return MioctGeometryXMLReader(filename)

def validate(geometry):
    stats = {'miocts' : 0, 'sectors' : 0, 'rois' : 0, 'decodes' : 0, 'topocells' : 0}

    stats['miocts'] = len(geometry.getMIOCTs())

    for mioct in geometry.getMIOCTs():
        stats['sectors'] += len(mioct.Sectors)
        for sector in mioct.Sectors:
            stats['rois'] += len(sector.ROIs)
            #print "M %2s  S %-4s  #R %3i" % (mioct['id'], sector['name'], len(sector.ROIs) )

        stats['decodes'] += len(mioct.Decodes)
        for decode in mioct.Decodes:
            stats['topocells'] += len(decode.TopoCells)
    print("#MIOCTs    : %i" % stats['miocts'])
    print("#Sectors   : %i" % stats['sectors'])
    print("#ROIs      : %i" % stats['rois'])
    print("#Decodes   : %i" % stats['decodes'])
    print("#TopoCells : %i" % stats['topocells'])



    

def main(args):

    print("Using input %s" % args.infile)
    geometry = readXML( args.infile )

    validate(geometry)

if __name__=="__main__":

    parser = argparse.ArgumentParser( description=__doc__, 
                                      formatter_class = argparse.RawTextHelpFormatter)

    parser.add_argument('-i', dest='infile', default="TrigConfMuctpi/TestMioctGeometry2016.xml", type=str,
                        help='name of input combined muon geometry filei [TrigConfMuctpi/TestMioctGeometry2016.xml]')

    opts = parser.parse_args()

    sys.exit( main(opts) )
