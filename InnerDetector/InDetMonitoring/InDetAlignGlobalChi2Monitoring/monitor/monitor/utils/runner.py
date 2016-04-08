# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os

# -----------------------------------------------------------------------------
#  monitor runner
# -----------------------------------------------------------------------------

Iterations = 6
FirstIteration = 0
OutputPath = '/home/cescobar/work/alignment/results/calibstream/fdr2/fdr2_week/day1/'
Sample = 'Collision'
# Sample = 'Cosmics'


# -----------------------------------------------------------------------------

os.chdir("../")

for i in range(FirstIteration,Iterations):
    print "----------------------------------------------"
    print "running monitor..."
    p = os.popen("root -b",'w')
    p.write(".x monitor.cc(\"%s/Iter%d/%s/\",%d)\n" % (OutputPath,i,Sample,i))
    p.write(".q\n")
    p.close()
    print "----------------------------------------------"


# -----------------------------------------------------------------------------

print ''
print 'Plots from the monitor are there:'
print '--------------------------------------------------------'
for i in range(FirstIteration,Iterations):
    print '%s/Iter%d/%s/Iter%d_plots.ps.gz' % (OutputPath,i,Sample,i)
print ''

# -----------------------------------------------------------------------------

emails = [os.getlogin()]
print 'sending mail...'
os.system('mail %s -s "your the plots are ready, so back to work" < /dev/null' % (emails))


