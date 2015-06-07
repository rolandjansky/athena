# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# this is to call macros for Efficiencies and Distributions
##

import sys,os
import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--file', type=str,help='input file')
parser.add_argument('--folder', type=str,help='folder')
parser.add_argument('--fileformat', type=str,help='filetype')
parser.add_argument('--triggers', nargs='*',type=str,help='triggers')
args = parser.parse_args()
root_file=args.file
triggerlist=args.triggers
folder=args.folder
fileformat=args.fileformat # not used
print triggerlist
print root_file

stringcommand='plotExtras.py -b '+root_file +' '+folder+' '+fileformat
os.system('python '+stringcommand)

for triggername in triggerlist:
    stringcommand='plotEfficiencies.py -b '+root_file+' '+triggername+' '+folder+' '+fileformat
    os.system('python '+stringcommand)

    stringcommand='plotDistributions.py -b '+root_file+' '+triggername+' '+folder+' '+fileformat
    os.system('python '+stringcommand)

# merge pdfs
os.system('gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -dAutoRotatePages=false -sOutputFile=output_singlepage.pdf *.ps')
