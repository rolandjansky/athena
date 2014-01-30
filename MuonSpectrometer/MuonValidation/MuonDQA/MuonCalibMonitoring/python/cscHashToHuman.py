#!/afs/cern.ch/sw/lcg/external/Python/2.5/slc4_ia32_gcc34/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#############################################################################################
############################################################################################
#
# cscHashToHuman.py
#
# Author: Caleb Lampen <lampen@physics.arizona.edu>
#
# This script will calculate a human readable channel ID from a CSC hash ID. 
# Note that this isn't official, as it doesn't use the athena framework to unpack the id.
# It is correct as of 2008-12-18. No changes are expected until the first ATLAS upgrade though,
# so it should be okay.
#############################################################################################

from math import *

def getSector(phi,tech):
  return(2*phi + 50 - tech)


def hashToHuman(hash):
  nhash = hash #nhash =newhash
  
  #tech 
  techCut = 30720
  if(nhash >= techCut):
    tech = 51 #CSL
  else:
    tech = 50 #CSS

  nhash = nhash % techCut

  #Precision
  precCut = 24576
  if(nhash >= precCut):
    prec = "Y"
    range = techCut - precCut +1
  else:
    prec = "X"
    range = precCut
    
  nhash = nhash % precCut

  #Eta
  etaCut = range/2
  if(nhash >= etaCut):
    eta = 1
  else:
    eta = -1

  nhash = nhash % etaCut
  range = range/2
  
  #phi
  phiDiv = range/8
  phi = nhash/phiDiv +1

  nhash = nhash % phiDiv
  range = range/8

  #chamLay
  chamCut = range/2
  if(nhash >= chamCut):
    chamLay = 2
  else:
    chamLay =1

  nhash = nhash % chamCut
  range = range /2
  
  #wireLay
  wireDiv = range/4
  wireLay = nhash/wireDiv +1

  nhash = nhash % wireDiv
  range = range/4

  #strip
  strip = nhash +1

  sector = getSector(phi,tech)

  if(chamLay != 2):
    print '\n****Warning! Chamber layer = ' + str(chamLay) + '. 2 expected!****\n' 
  print 'chamLay: ' + str(chamLay) + ', eta: ' +  str(eta) + \
      ', sector ' + str(sector) + ', wireLayer ' + str(wireLay) + \
      ', direction: ' + prec + ', strip: ' + str(strip)
  return

#Start the program proper

while(1):
  print '\n'
  hash = input('Enter a hash ID (negative to quit): ')
  if(hash < 0):
    break
  hashToHuman(hash)




  
