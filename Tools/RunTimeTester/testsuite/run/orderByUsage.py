# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

lines = open('junk.log', 'r').readlines()
tList = []
for l in lines:
    try:
        usage = int(l.split()[3][:-1])
    except:
        continue
    print usage
    tList.append((usage, l))        

tList.sort()
lines = [l[1] for l in tList]
for l in lines:
    print l

    
    
