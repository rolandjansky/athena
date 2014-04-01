#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#import matplotlib as mpl
#mpl.use('pdf')
#import pylab as pl

#iterations = "A00 A01 A02".split()
iterations = []
iterations.extend(['C%02d' % (x) for x in range(10)])

tables = []

for i in iterations:
    fh = open('results_iter%s.txt' % i)

    table = {}

    for line in fh:
        
        if ('Barrel' in line) or ('Endcap' in line):
            s = (line.strip()).split()
            if (len(s) == 4):
                key = ' '.join(s[0:2])
                table[key] = ' '.join(s[2:4])
#                print table[key]
            if (len(s) == 10):
                key = ' '.join(s[0:2])
                table[key] += ' ' + ( ' '.join( s[4:8]))
#                print table[key]
            
#    for k in table:
#        print ( (k, table[k]))

    fh.close()

    tables.append(table)


for k in tables[0].keys():

    print("\\begin{table}")
    print("\\centering")

    print("\\begin{tabular}[ht]{|l|rrrrrr|}")
    print("%s & & & & & &\\\\" % k)
    print("Iter & A & C & Pull & PullErr & RMS & RMSErr \\\\")
    

    lA = []
    lC = []
    lPull = []
    lPullErr = []
    lRMS = []
    lRMSErr = []
    
    for (table, i) in zip(tables, iterations):
        (A, C, Pull, PullErr, RMS, RMSErr) = map(float, table[k].split())
        lA.append(A)
        lC.append(C)
        lPull.append(Pull)
        lPullErr.append(PullErr)
        lRMS.append(RMS)
        lRMSErr.append(RMSErr)
        print("%s & %0.4f & %0.4f & %0.4f & %0.4f & %0.4f & %0.4f \\\\" % (i, A, C, Pull, PullErr, RMS, RMSErr))

            
    print("\\end{tabular}")


    print("\\end{table}")
