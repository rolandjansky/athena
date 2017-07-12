# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import math
import array
import struct

from PyCool import cool

import cppyy
cppyy.gbl.cool.IDatabase # force the load of the dictionary (to stay on the safe side)
from cppyy import gbl, getAllClasses
def blob_read(self, size = -1):
    if size < 0:
        endpos = self.size()
    else:
        endpos = self.pos + size
    beginpos = self.pos
    self.pos = endpos
    buf = self.startingAddress()
    buf.SetSize(self.size())
    return buf[beginpos:endpos]

# add the new functions
getattr(gbl,"coral::Blob").read = blob_read

def bConvert(b, nbyte=1):
    # routine to store an unsigned int (1, 2, 4 or 8 byte) in a blob
    packopt=dict([[1,'B'],[2,'H'],[4,'f'],[8,'d']])
    if nbyte in packopt:
        # print 'bConvert - b:[', b[0:nbyte], '] nbyte:', nbyte, ' fmt:', packopt[nbyte], type(b)
        ival=struct.unpack(packopt[nbyte], b[0:nbyte])
    else:
        print 'bConvert: Unrecognized pack option'
        sys.exit()

    return ival[0]

# Optional arguemnt to nval to specify number of values to read
def bConvertList(b, nbyte=1, nval=1):
    # routine to store an unsigned int (1, 2, 4 or 8 byte) in a blob
    packopt=dict([[1,'B'],[2,'H'],[4,'f'],[8,'d']])
    if nbyte in packopt:
        # print 'bConvert - b:[', b[0:nbyte], '] nbyte:', nbyte, ' fmt:', packopt[nbyte], type(b)
        fmt = '%d%s' % (nval, packopt[nbyte])
        ival=struct.unpack(fmt, b[0:nval*nbyte])
    else:
        print 'bConvertList: Unrecognized pack option'
        sys.exit()

    return list(ival)

# Unpack bunch group bgrp.  By default, bgrp=1 is the physics bunch group. 
def unpackBunchGroup(blob, bgrp=1):
    physBG = []
    if blob.size() == 0: return
    
    blobCopy = blob.read()
    mask = (1 << int(bgrp))
    
    ivallist = bConvertList(blobCopy, 1, 3564)
    for i in xrange(3564):
        if ivallist[i] & mask:
            physBG.append(i)

#     blobCounter = 0
#     for i in xrange(3564):
#         try:
#             b = blobCopy[blobCounter:blobCounter+1]
#             blobCounter += 1
#             ival = struct.unpack('B', b)
#             #s = struct.unpack('B', b)
#             #ival = bConvert(s)
#         except Exception, e:
#             print e
#             ival = 0
#         if (ival>>1) & 1 == 1:
#             physBG.append(i)

    return physBG
            
# Unpack bunch group bgrp.  By default, bgrp=1 is the physics bunch group. 
def unpackBunchGroupList(blob, bgrp=[1]):
    physBG = dict()
    mask = dict()
    
    if blob.size() == 0: return
    
    blobCopy = blob.read()

    for id in bgrp:
        mask[id] = (1 << int(id))
        physBG[id] = []
        
    ivallist = bConvertList(blobCopy, 1, 3564)
    for i in xrange(3564):
        for id in bgrp:
            if ivallist[i] & mask[id]:
                physBG[id].append(i)

    return physBG

# Generic routine to unpack BCID mask
def unpackBCIDMask(blob,nb1,nb2,nlumi):

    bloblength = blob.size()

    if bloblength == 3564:
        return unpackRun2BCIDMask(blob,nb1,nb2,nlumi)
    else:
        return unpackRun1BCIDMask(blob,nb1,nb2,nlumi)

# routine to unpack the BCID mask stored in COOL
# This is the run2 version
def unpackRun2BCIDMask(blob,nb1,nb2,nlumi):
    beam1=[]
    beam2=[]
    coll=[]
    blobCopy = blob.read()
    rawData = bConvertList(blobCopy, 1, 3564)

    for i in range(3564):
        val = rawData[i]
        if val & 0x01:
            beam1.append(i)
        if val & 0x02: 
            beam2.append(i)
        if (val & 0x03) == 0x03:
            coll.append(i)

    print 'unpackRun2BCIDMask found:'
    print ' Beam1:', beam1
    print ' Beam2:', beam2
    print ' Coll: ', coll

    return beam1,beam2,coll

# routine to unpack the BCID mask stored in COOL
# This is the run1 version
def unpackRun1BCIDMask(blob,nb1,nb2,nlumi):
    beam1=[]
    beam2=[]
    coll=[]
    blobCopy = blob.read()
    beam1 = bConvertList(blobCopy, 2, nb1)
    beam2 = bConvertList(blobCopy[2*nb1:], 2, nb2)
    coll = bConvertList(blobCopy[2*(nb1+nb2):], 2, nlumi)
    #unpackfmt = '%dH' % nb1
    #list(struct.unpack(unpackfmt, blobCopy[0:(2*nb1)]))
    #unpackfmt = '%dH' % nb2
    #beam2 = list(struct.unpack(unpackfmt, blobCopy[(2*nb1):2*(nb1+nb2)]))
    #unpackfmt = '%dH' % nlumi
    #coll = list(struct.unpack(unpackfmt, blobCopy[2*(nb1+nb2):2*(nb1+nb2+nlumi)]))
                 
#    blobCounter = 0
#     for i in xrange(nb1):
#         b = blobCopy[blobCounter:blobCounter+2]
#         blobCounter += 2
#         val=struct.unpack('H', b)
#         beam1.append(val)
        
#     for i in xrange(nb2):
#         b = blobCopy[blobCounter:blobCounter+2]
#         blobCounter += 2
#         val=struct.unpack('H', b)
#         beam2.append(val)

#     for i in xrange(nlumi):
#         b = blobCopy[blobCounter:blobCounter+2]
#         blobCounter += 2
#         val=struct.unpack('H', b)
#         coll.append(val)

    return beam1,beam2,coll

# routine to unpack values (raw lumi or currents) stored as blob in COOL
# blob - COOL blob with per-BCID values
# mask - BCID mask appropriate for quantity being unpacked (i.e.: beam1, collisions, ...)
# normValue - Normalization value from same COOL folder as BLOB (i.e.: B1BunchAverage)
#
# Note, the normValue is only used in certain storage modes.  If you want to renormalize, do this yourself.
# Specifying a different value for the normValue will likely cause unpredictable results.

def unpackBCIDValues(blob, mask=[], normValue=1):

    bss, bcidVec, lvec = unpackBunches(blob, mask)
    
    if bss>0:
      if not (len(bcidVec)==len(lvec)):
        print 'unpackBCIDValues - length mismatch: len(bcidVec)=', len(bcidVec), 'len(lvec)=', len(lvec)
        sys.exit()
        
      bLumi=[]
      for i in range(len(bcidVec)):
        if bss<4:
          bLumi.append(lvec[i]*normValue/pow(100,bss))
        else:
          bLumi.append(lvec[i])

      #for i in range(len(bcidVec)):
      #    print 'BCID:', bcidVec[i], 'Raw:', bLumi[i]
          
      return bcidVec,bLumi

    else:
      return [],[]
    
def unpackBunches(blob,mask=[]):
    # routine to unpack Intensity/Luminosity info stored in COOL
    # the mask given as input has to match the quantity to be
    # unpacked (beam1, beam2, beamsand for B1, B2 intensities and
    # luminosities, respectively)

    if blob.size() == 0:
        return 0,[],[]
    
    blobCopy = blob.read()
    blobCounter = 0
    try:
        b = blobCopy[blobCounter:blobCounter+1]
        blobCounter += 1
        flag=bConvert(b)
        bss=(flag%100)/10
        smod=flag%10
        # print 'Storage mode for',str, 'is', smod, 'with bss=', bss
            
        if smod==2:
            b = blobCopy[blobCounter:blobCounter+2]
            blobCounter += 2
            vlen=bConvert(b, 2)
            #print 'Bunch vector has length ',vlen
            bcidVec=[]
            fmt = '%dH' % vlen
            bcidVec = bConvertList(blobCopy[blobCounter:], 2, vlen)
            blobCounter += 2*vlen
            # for i in range(vlen):
            #     valb = blobCopy[blobCounter:blobCounter+2]
            #     blobCounter += 2
            #     val=struct.unpack('H', valb)
            #     bcidVec.append(val)
            
        elif smod==0:
            # Make sure this is a list, and sorted (can pass set for example)
            bcidVec=list(mask)
            bcidVec.sort()
            vlen=len(mask)
        elif smod==3:
            print 'storage mode 3 not implemented in unpackBunches'
            sys.exit()
        elif smod==1:
            bcidVec=[i for i in range(3564)]
            vlen=3564
        else:
            print 'Unknown storage mode ',smod
            sys.exit()
        valueVec=[]

        valueVec = bConvertList(blobCopy[blobCounter:], bss, vlen)
#         for i in range(vlen):
#             valb = blobCopy[blobCounter:blobCounter+bss]
#             blobCounter += bss
#             val=bConvert(valb,bss)
#             valueVec.append(val)

        return bss,bcidVec,valueVec

    except RuntimeError, e:
        print e
        return 0,[],[]
                                  
# Unpack live fraction into vector keyed by bcid-1
# Takes payload of /TRIGGER/LUMI/PerBcidDeadtime folder
def unpackLiveFraction(trigPayload, priority = 'high'):

    liveVec = array.array('f', 3564*[0.])
    
    if priority == 'high':
        blob = trigPayload['HighPriority']
    elif priority == 'low':
        blob = trigPayload['LowPriority']
    else:
        print 'unpackLiveFraction - unknown priority requested %s', str(priority)
        return liveVec
    
    bloblength = blob.size()

    # Due to a bug, the blob was sometimes written at 3654 rather than desired 3564
    # More bugs, use anything long enough 
    if bloblength < 3*3564: #!= 3*3654 and bloblength != 3*3564:
        # Corrupt, don't trust anything
        print 'unpackLiveFraction found blob length %d!' % bloblength
        return liveVec

    blobCopy = blob.read()
    # blobCounter = 0

    # No counts, no work to do
    turnCounter = trigPayload['TurnCounter']
    if not turnCounter > 0:
        return liveVec

    # Even if longer blob is present, only care about this range
    
    byte = bConvertList(blobCopy, 1, 3*3564)
    
    for i in xrange(3564):

        busyCounter = byte[3*i] | (byte[3*i+1] << 8) | (byte[3*i+2] << 16)
        
        # byte0 = struct.unpack('B', blobCopy[blobCounter:blobCounter+1])
        # blobCounter += 1
        # byte1 = struct.unpack('B', blobCopy[blobCounter:blobCounter+1])
        # blobCounter += 1
        # byte2 = struct.unpack('B', blobCopy[blobCounter:blobCounter+1])
        # blobCounter += 1
        # busyCounter = byte0 | (byte1 << 8) | (byte2 << 16)
        
        liveFrac = 1 - float(busyCounter) / turnCounter

        liveVec[i] = liveFrac

        # print 'BCID: %d Busy: %d Turn: %d Live: %f' % (i+1, busyCounter, turnCounter, liveFrac)

    return liveVec

