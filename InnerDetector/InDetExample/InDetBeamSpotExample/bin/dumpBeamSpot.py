#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Dump beam spot in a COOL SQLite file (using AtlCoolTool).

__author__  = 'Juerg Beringer'
__version__ = '$Id: dumpBeamSpot.py 217369 2009-10-05 18:43:35Z atlidbs $'
__usage__   = '%prog [options] beamspotfile.db|connectionstring'

from CoolConvUtilities import AtlCoolTool

from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-d', '--dbname', dest='dbname', default='BEAMSPOT', help='database instance name (default: BEAMSPOT)')
parser.add_option('-c', '--connstring', dest='connstring', action='store_true', help='use beamspotfile as connectionstring (e.g. COOLOFL_INDET/OFLP200)')
parser.add_option('-f', '--folder', dest='folder', default='/Indet/Beampos', help='COOL folder (default: /Indet/Beampos)')
parser.add_option('-t', '--tag', dest='tag', default='nominal', help='folder tag')
parser.add_option('-i', '--info', dest='folderinfo', action='store_true', help='show folder and tag information')
(options,args) = parser.parse_args()
if len(args) != 1:
    parser.error('wrong number of command line arguments')
folder = options.folder

if options.connstring:
    dbconn = args[0]
else:
    dbconn = "sqlite://;schema="+args[0]+";dbname="+options.dbname
print "\nUsing database connection:",dbconn

tool = AtlCoolTool.AtlCoolTool(dbconn)
tool.usetag(options.tag)   # Must always set tag

if options.folderinfo:
    print
    print tool.listinfo(folder)
    print
    print tool.lstags(folder)
    print
    exit()

print "\nUsing folder:",folder
print tool.more(folder)
print
