#!/bin/env python
# Author: Michael Miller (miller@uchicago.edu)
import os, sys


def main():

    if len(sys.argv)!=3:
        print 'Please pass a destination directory and a run number.'
        return True

    outdir = str(sys.argv[1])
    runnumber = int(sys.argv[2])
    
    parts = ['LBA', 'LBC', 'EBA', 'EBC']

    indir='/castor/cern.ch/grid/atlas/atlasgroupdisk/det-tile/2009/'
    
    for partition in parts:
        for module in range(1,65):
            getname = os.popen('nsls %s%s%02d' % (indir, partition, module))
            for line in getname.readlines():
                if str(runnumber) in line:
                    file = line.strip()
            if file:
                os.system('rfcp %s%s%02d/%s %s &' % (indir, partition, module, file, outdir))
              
                      

if __name__ == '__main__':
    main()
