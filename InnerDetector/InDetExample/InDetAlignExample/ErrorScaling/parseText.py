# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from glob import glob
from decimal import Decimal
import numpy
import pylab
import os
import sys



if len(sys.argv) == 2:
    outFolder = sys.argv[1] + '/'
else:
    outFolder = ''
print outFolder

outStr = 'test'

files = glob(outFolder + 'results_iterC*.txt')

pixPhiB = []
pixEtaB = []
pixPhiE = []
pixEtaE = []
sctBarrel = []
sctEndcap = []
trtBarrel = []
trtEndcap = []

pixPhiB_pull = []
pixEtaB_pull = []
pixPhiE_pull = []
pixEtaE_pull = []
sctBarrel_pull = []
sctEndcap_pull = []
trtBarrel_pull = []
trtEndcap_pull = []

for ff in files:
    fff = open(ff,'r')
    lineVec = fff.readlines()
    lineVec_C = lineVec[5:14]
    lineVec_pull = lineVec[18:26]
    for i in lineVec_C:
        i.strip()
        if 'PixPhi Barrel' in i:
            pixPhiB.append(i.split()[3])
        if 'PixEta Barrel' in i:
            pixEtaB.append(i.split()[3])
        if 'PixPhi Endcap' in i:
            pixPhiE.append(i.split()[3])
        if 'PixEta Endcap' in i:
            pixEtaE.append(i.split()[3])
        if 'SCT Barrel' in i:
            sctBarrel.append(i.split()[3])
        if 'SCT Endcap' in i:
            sctEndcap.append(i.split()[3])
        if 'TRT Barrel' in i:
            trtBarrel.append(i.split()[3])
        if 'TRT Endcap' in i:
            trtEndcap.append(i.split()[3])
    for i in lineVec_pull:
        i.strip()
        print i
        if 'PixPhi Barrel' in i:
            pixPhiB_pull.append(i.split()[4])
        if 'PixEta Barrel' in i:
            pixEtaB_pull.append(i.split()[4])
        if 'PixPhi Endcap' in i:
            pixPhiE_pull.append(i.split()[4])
        if 'PixEta Endcap' in i:
            pixEtaE_pull.append(i.split()[4])
        if 'SCT Barrel' in i:
            sctBarrel_pull.append(i.split()[4])
        if 'SCT Endcap' in i:
            sctEndcap_pull.append(i.split()[4])
        if 'TRT Barrel' in i:
            trtBarrel_pull.append(i.split()[4])
        if 'TRT Endcap' in i:
            trtEndcap_pull.append(i.split()[4])

    fff.close()

print 'PixPhi Barrel C evolution'
for i in pixPhiB:
    print i

print 'PixEta Barrel pull evolution'
for i in pixEtaB_pull:
    print i

print len(pixPhiB),len(pixEtaB),len(pixPhiE),len(pixEtaE),len(sctBarrel),len(sctEndcap),len(trtBarrel),len(trtEndcap)

textFile = open(outStr + 'Output.txt','w')

for i in range(0,len(sctEndcap)):
    if i == 0:
        fileStr = 'i' + ' ' + 'pixPhiB' + ' ' + 'pixEtaB' + ' ' + 'pixPhiE' + ' ' + 'pixEtaE' + ' ' + 'sctBarrel' + ' ' + 'sctEndcap' + ' ' + 'trtBarrel' + ' ' + 'trtEndcap' + '\n'
        textFile.write(fileStr)
    fileStr = str(i) + ' ' + pixPhiB[i] + ' ' + pixEtaB[i] + ' ' + pixPhiE[i] + ' ' + pixEtaE[i] + ' ' + sctBarrel[i] + ' ' + sctEndcap[i] + ' ' + trtBarrel[i] + ' ' + trtEndcap[i] + '\n'
    textFile.write(fileStr)

textFile.write('\n')

for i in range(0,len(sctEndcap)):
    #if i == 0:
    #    fileStr = 'i' + ' ' + 'pixPhiB' + ' ' + 'pixEtaB' + ' ' + 'pixPhiE' + ' ' + 'pixEtaE' + ' ' + 'sctBarrel' + ' ' + 'sctEndcap' + ' ' + 'trtBarrel' + ' ' + 'trtEndcap' + '\n'
    #    textFile.write(fileStr)
    fileStr = str(i) + ' ' + pixPhiB_pull[i] + ' ' + pixEtaB_pull[i] + ' ' + pixPhiE_pull[i] + ' ' + pixEtaE_pull[i] + ' ' + sctBarrel_pull[i] + ' ' + sctEndcap_pull[i] + ' ' + trtBarrel_pull[i] + ' ' + trtEndcap_pull[i] + '\n'
    textFile.write(fileStr)



textFile.close()



"""
pixPhiBarrC = []
for ff in files:
    f = open(ff, 'r')

    strTest = ''
    while(strTest != 'PixPhi'):
        #print 'Reading text file.'
	line =  f.readline()
        lineList = line.split()
        if(len(lineList)==0):
            continue
        strTest=lineList[0]

    pixPhiBarrC.append(lineList[3])

    #for s in lineList:
        #print s

iterateList = []
pixPhiBarrCVec = []
for i,s in enumerate(pixPhiBarrC):
    print s
"""

#    pixPhiBarrCVec.append(float(s))
#    iterateList.append(float(i))

#pylab.plot(iterateList,pixPhiBarrCVec)
#pylab.show()

#pylab.savefig('plotIterate')
