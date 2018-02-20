#!/bin/usr/python

import os
import sys

for subregion in xrange(1):
    for region in xrange(64):
        line = ' > run_'+str(subregion)+'_'+str(region)+'.sh'
        cmd = 'cd '+os.getcwd()
        os.system('echo '+cmd+line)
        #cmd = 'python ../bin/find_X11_relations.py sectors_raw_8L_4M_reg%s_sub%s.patt.bz2 4'% (region, subregion)
        cmd = '"setupATLAS; localSetupROOT;'
        #cmd += 'python ../bin/find_extrapolation_relations.py sectors_raw_8L_4M_reg%s_sub%s.patt.bz2 4"'% (region, subregion)
        cmd = 'python ../make_conn/find_extrapolation_relations_v2.py sectors_raw_8L_reg%s.patt.bz2 1'% (region)
        print cmd
        #os.system( cmd )
        line = ' >> run_'+str(subregion)+'_'+str(region)+'.sh'
        os.system('echo '+cmd+line)
        os.system('chmod 755 '+'run_'+str(subregion)+'_'+str(region)+'.sh')
        os.system('sleep 2')
        print subregion,region
        os.system('bsub -q 1nh '+'run_'+str(subregion)+'_'+str(region)+'.sh')
