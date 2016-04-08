#!/usr/bin/env python
#TODO : prune standalone alg and objects
#       remove number of read write
# remove monitoring, L1, CTP
# arrow show be single side !
# @file:    SGout2dot.py
# @purpose: read a SGAudSvc produced file and make dot file.
# @author:  Ilija Vukotic <ivukotic@cern.ch>
# @date:    May 2007
#
# @example:
#
# checkFile SGAudSvc.out

__version__ = "$Revision: 1.2 $"
__author__  = "Ilija Vukotic <ivukotic@cern.ch>"

import sys
import os
import shutil
import glob

# some configuration booleans
debug=True
debugRemove=False
drawNReadWrite=False # if draw on each line the number of read and write


#Algorithms to be masked. Might need to be tuned
lRemove=[]
lRemove+= ["MboyMuonChamberT0"]
lRemove+= ['Stream','StreamAOD','noKey',"basic_"]
lRemove+= ["Trig","HLT","EF","L1","Mon","CTP","RoIB","GlobalManager"]
#lRemove += ['LArRawChannelCollection','TileRawChannelCollection','DetStatusAlg','DetectorStatus','_Data','_Link']
#lRemove+= ['EventSelector','Input']
#lRemove+= ['iPatStatistics']
def chLabel(inpVal):
	result=inpVal
	result=result.replace('>','_GT_')
	result=result.replace('<','_LT_')
	result=result.replace('::','_')
	result=result.replace('.','_')
	result=result.replace('+','_')	
	return result



def isNumb(inpVal):
	result = False
	try:                                
		stripped = str(int(inpVal))
		result=True
	except:
		result=False
	return result

def toRemove(inpVal):
	result=False
	if isNumb(inpVal)==True:
		if debugRemove:
			print "remove ", inpVal, "is a number"
		result=True
	for thing in lRemove:
		if inpVal.count(thing)>0:
			if debugRemove:
				print "remove ", inpVal ,"since it contains", thing
			result=True
	return result

nArgs= len (sys.argv)
outFN="dot.dot"
inFN=""
if nArgs==1:
	print "Usage: SGout2dot.py input_filename [output dot file]"
	sys.exit()
if nArgs>1:
	print "Input file",sys.argv[1]
	inFN=sys.argv[1]
if nArgs==3:
	print "Output file",sys.argv[2]
	outFN=sys.argv[2]
try:
	fi = open (inFN)
except:
	print "No such file!"
	sys.exit()
	
fo = open (outFN, 'w' )

lines = fi.readlines()
fi.close() 

# get number of algs
line=lines.pop(0)
words=line.split(' ')
nAlgs=int(words[1])
print '%(a)d algorithms found'%{'a':nAlgs}

#read all algs
#note that algs to be removed cannot be masked before reading the alg <-> obj relation
algs=[]
nGoodAlgs=0
for i in range(nAlgs):
	line=lines.pop(0).strip('\n')
	line=line.replace('/','_')
	algs.append(line)
	if toRemove(line):
		print "alg %s to be removed" % line
	else:
		nGoodAlgs+=1

print " number of good algs ",nGoodAlgs," bad algs ",nAlgs-nGoodAlgs
if debug:
	print algs

# get number of objs
line=lines.pop(0)
words=line.split(' ')
nObjs=int(words[1])
print '%(a)d objects found'%{'a':nObjs}

# get all object names, and replace possible annoying characters
#note that objs to be removed cannot be masked before reading the alg <-> obj relation
objs=[]
nGoodObjs=0
for i in range(nObjs):
	line=lines.pop(0).strip('\n')
	line=line.replace('/','_')
	line=line.replace('<','LT')
	line=line.replace('>','GT')
	line=line.replace('+','_')
	objs.append(line)
	if toRemove(line):
		print "obj %s to be removed" % line
	else:
		nGoodObjs+=1

print " number of good objs ",nGoodObjs," bad objs ",nObjs-nGoodObjs	
if debug:
	print objs

fo.write("// %s algorithms and %s objects \n" % (nGoodAlgs,nGoodObjs))
	

#write out dot file header
fo.write("digraph SGAudSvc{\nrankdir=LR;\n") # begining and graph stuff
fo.write("ratio=0.7;\nnodesep=0.05;\n")
fo.write("subgraph c_0{\nnode[style=filled,color=red];\n") # alg subgraph
fo.write("label=\"Algs READING\";\ncolor=red;\n")

# get the relations betwen alg and obj
# TODO number write or read not kept at this point
objsWrittenByAlg={}  #A->many O (A write O may read it as well)
objsReadByAlg={}  #many O->A (A read O may do not read it )
algsWritingObj={}  #many A->O (A write O may read it as well).
algsReadingObj={} # O->many A (A does not write O)
for alg in algs:
	line=lines.pop(0)
	words=line.strip('\n').split('\t')
	for obj in objs:
		rw=words.pop(0).split(':')
		reads=int(rw[0])
		writes=int(rw[1])
		if reads==0 and writes==0:
			continue
		if toRemove(obj) or toRemove(alg):
			continue
		#print alg,obj,reads, writes

		if writes==0 and reads>0:
			if not obj in algsReadingObj.keys():
				algsReadingObj[obj]=[]
			algsReadingObj[obj]+=[(alg,reads,writes)]
			if not alg in objsReadByAlg.keys():
				objsReadByAlg[alg]=[]
			objsReadByAlg[alg]+=[obj]
		elif writes>0:
			if not obj in algsWritingObj.keys():
				algsWritingObj[obj]=[]
			algsWritingObj[obj]+=[alg]

			if not alg in objsWrittenByAlg.keys():
				objsWrittenByAlg[alg]=[]
			objsWrittenByAlg[alg]+=[(obj,reads,writes)]


#write out all algorithms, but only the ones that have at least one connection (and are not masked)
for alg in algs:
	if toRemove(alg)==False:
		if alg in objsWrittenByAlg or alg in objsReadByAlg:
			towrite='a_%(a)s [label=\"%(a)s\"];\n'%{'a':chLabel(alg),'b':alg}
			fo.write(towrite)
		else:
			print "alg %s masked because not connected" % alg
			
fo.write("}\n\nsubgraph c_1{\nnode[style=filled,shape=box];\ncolor=green;label=\"Objects\";\n")
#write out all objecs
for obj in objs:
	if toRemove(obj)==False:
		if obj in algsWritingObj or obj in algsReadingObj:
			towrite='o_%(a)s [label=\"%(b)s\"];\n'%{'a':chLabel(obj),'b':obj}
			fo.write(towrite)
		else:
			print "obj %s masked because not connected" % obj			
fo.write("}\n\n")


if debug:
	print " "
	print "objsWrittenByAlg:", objsWrittenByAlg
	print " "
	print "objsReadByAlg:", objsReadByAlg
	print " "
	print "algsWritingObj:", algsWritingObj
	print " "
	print "algsReadingObj:", algsReadingObj
	print " "
	
#write the connection a to obj
for alg in objsWrittenByAlg.keys():
	for objrec in objsWrittenByAlg[alg]:
		obj=objrec[0]
		reads=objrec[1]
		writes=objrec[2]
		towrite='a_%(ALG)s -> o_%(OBJ)s [ '%{'ALG':chLabel(alg),'OBJ':chLabel(obj)}
		if drawNReadWrite:
			if reads==0:
				towrite+='label=\"%s\" ' % writes
			else:
				# if read in addition to write, write both number of read and write plus double arrow
				# this happen when an alg writes an object and read it back to modify it
				towrite+='label=\"w:%(WRITES)s r:%(READS)s\" arrowtail=\"normal\" '%{'WRITES':writes,'READS':reads}				
		else:
			if reads>0: # double arrow
				towrite+='arrowtail=\"normal\" '				
		towrite+='];\n'
		fo.write(towrite)

#write the connection obj to alg
# by definition only read no write
for obj in algsReadingObj.keys():
	for algrec in algsReadingObj[obj]:
		alg=algrec[0]
		reads=algrec[1]
		writes=algrec[2]

		towrite='o_%(OBJ)s -> a_%(ALG)s [ '%{'ALG':chLabel(alg),'OBJ':chLabel(obj)}
		if drawNReadWrite:
			towrite+='label=\"%s\" ' % reads
		towrite+='];\n'
		fo.write(towrite)

## # get the relations betwen alg and obj, and write them out
## for alg in algs:
## 	alg=chLabel(alg)
## 	line=lines.pop(0)
## 	words=line.strip('\n').split('\t')
## 	for obj in objs:
## 		rw=words.pop(0).split(':')
## 		reads=rw[0]
## 		writes=rw[1]
## 		#print alg,obj,reads, writes
## 		if toRemove(obj)==False and toRemove(alg)==False:
## 				if int(writes)>0 and int(reads)>0:
## 					towrite='a_%(ALG)s -> o_%(OBJ)s [label=\"w:%(WRITES)s r:%(READS)s\" '%{'ALG':alg,'OBJ':obj,'WRITES':writes,'READS':reads}
## 					towrite+='arrowtail=\"normal\"];\n'
## 					fo.write(towrite)
## 				else:
## 					if int(writes)>0:
## 						towrite='a_%(ALG)s -> o_%(OBJ)s [label=\"%(WRITES)s\"];\n'%{'ALG':alg,'OBJ':obj,'WRITES':writes}
## 						fo.write(towrite)
## 					if int(reads)>0:
## 						towrite='o_%(OBJ)s -> a_%(ALG)s [label=\"%(READS)s\"];\n'%{'ALG':alg,'OBJ':obj,'READS':reads}
## 						fo.write(towrite)

fo.write("}")
fo.close()
