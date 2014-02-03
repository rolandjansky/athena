#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os
import sys
import glob
print "Summarize CPU"
sIn = "*_log"
sOut = "cpuSummary.log"
sInArray = glob.glob(sIn)
fIn = open(sInArray[0])
fOut = open(sOut,"w")
print fIn
# pickle.dump(fIn,fOut)
fOut.write(str(fIn)+'\n')
lines = fIn.readlines()
fIn.close()

### convert lines to seconds ###
def getSeconds(line):
  myString = ''
  myString = line.split('=')
#     myString
#     myString.split('=')
  myString2 = myString[2].split('(')
#     val = float(myString[6])
  val = float(myString2[0])
  if myString[2].find('[s]') >= 0:
    val *= 1
  elif myString[2].find('[min]') >= 0:
    val *= 60
  elif myString[2].find('[ms]') >= 0:
    val *= 0.001     
  elif myString[2].find('[us]') >= 0:
    val *= 1e-6 
  else : print "ERROR: time units not found" 
  return val

### create groups ###
mutagimo = 0
mugirl   = 0
innerdet = 0
moore    = 0
muid     = 0
staco    = 0
muonboy  = 0
mutag    = 0
athena   = 0
  
### look for keywords ###  
for i in range(len(lines)): 
  if lines[i].find('iPatRec:execute') >= 0 or lines[i].find('InDetPriVxFinder:execute') >= 0 or lines[i].find('InDetTRT_SeededAmbiguitySolver:execute') >= 0 or lines[i].find('InDetAmbiguitySolver:execute') >= 0 or lines[i].find('InDetExtensionProcessor:execute') >= 0 or lines[i].find('InDetSiSpTrackFinder:execute') >= 0 or  lines[i].find('InDetTRT_TrackSegmentsFinder:execute') >= 0 or lines[i].find('InDetTRT_StandaloneTrackFinder:execute') >= 0 or lines[i].find('InDetTRT_SeededTrackFinder:execute') >= 0:
#   Trk_VKalVrtFitter?
    val = getSeconds(lines[i])
    innerdet += val
    print lines[i].strip()
    fOut.write(str(lines[i].strip())+'\n')
  elif lines[i].find('MuTagMasterIMO:execute') >= 0:
    val = getSeconds(lines[i])
    mutagimo += val 
    print lines[i].strip()
    fOut.write(str(lines[i].strip())+'\n')
  elif lines[i].find('MuGirl:execute') >= 0:
    val = getSeconds(lines[i])
    mugirl += val 
    print lines[i].strip()
    fOut.write(str(lines[i].strip())+'\n')
  elif lines[i].find('MuonCombiTrackBuilder:execute') >= 0 or lines[i].find('MooSegmentMaker:execute') >= 0:
    val = getSeconds(lines[i])
    moore += val     
    print lines[i].strip()    
    fOut.write(str(lines[i].strip())+'\n')
  elif lines[i].find('MuidVertexAssociator:execute') >= 0 or lines[i].find('MuidCombined:execute') >= 0 or lines[i].find('MuidStandalone:execute') >= 0:
    val = getSeconds(lines[i])
    muid += val     
    print lines[i].strip()      
    fOut.write(str(lines[i].strip())+'\n')
  elif lines[i].find('StacoMaster:execute') >= 0 or lines[i].find('StacoDressingTP2Ana:execute') >= 0:
    val = getSeconds(lines[i])
    staco += val     
    print lines[i].strip()    
    fOut.write(str(lines[i].strip())+'\n')
  elif lines[i].find('MboyRec:execute') >= 0:
    val = getSeconds(lines[i])
    muonboy += val     
    print lines[i].strip()        
    fOut.write(str(lines[i].strip())+'\n')
  elif lines[i].find('MuTagMaster:execute') >= 0:
    val = getSeconds(lines[i])
    mutag += val 
    print lines[i].strip()    
    fOut.write(str(lines[i].strip())+'\n')
  elif lines[i].find('AthMasterSeq:execute') >= 0:
    val = getSeconds(lines[i])
    athena += val 
    print lines[i].strip()    
    fOut.write(str(lines[i].strip())+'\n')

### print summary ###    
print '*** Summary of average time per event ***'    
print 'Muid     '+str(muid)+' s' 
print 'Moore    '+str(moore)+' s' 
print 'MuGirl   '+str(mugirl)+' s'    
print 'MuTagIMO '+str(mutagimo)+' s'    
print '-------------------------'
print 'Staco    '+str(staco)+' s'   
print 'Muonboy  '+str(muonboy)+' s'
print 'MuTag    '+str(mutag)+' s'
print '-------------------------'
print 'InnerDet '+str(innerdet)+' s'
print 'Athena   '+str(athena)+' s'
print 'Muon reconstruction uses '+str(round(100*(muid+moore+mugirl+mutagimo+staco+muonboy+mutag)/athena,2))+'% of total Athena time' 
fOut.write('*** Summary of average time per event ***\n'
  +'Muid     '+str(muid)+' s\n' 
  +'Moore    '+str(moore)+' s\n' 
  +'MuGirl   '+str(mugirl)+' s\n'    
  +'MuTagIMO '+str(mutagimo)+' s\n'    
  +'-------------------------\n'
  +'Staco    '+str(staco)+' s\n'   
  +'Muonboy  '+str(muonboy)+' s\n'
  +'MuTag    '+str(mutag)+' s\n'
  +'-------------------------\n'
  +'InnerDet '+str(innerdet)+' s\n'
  +'Athena   '+str(athena)+' s\n'
  +'Muon reconstruction uses '+str(round(100*(muid+moore+mugirl+mutagimo+staco+muonboy+mutag)/athena,2))+'% of total Athena time\n' )
fOut.close()
# MuTagMasterIMO:execute                               INFO Time User   : Tot=  173  [s] Ave/Min/Max=0.863(+-0.376)/0.192/ 3.07  [s] #=200
