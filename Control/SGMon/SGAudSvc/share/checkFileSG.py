#!/usr/bin/env python

# @file:    checkFileSG.py
# @purpose: answers queries on a SGAudSvc produced file
# @author:  Ilija Vukotic <ivukotic@cern.ch>
# @date:    October 2007
#
# @example:
#
# checkFileSG SGAudSvc.out 

__version__ = "$Revision: 1.1 $"
__author__  = "Ilija Vukotic <ivukotic@cern.ch>"

import sys
import os
import shutil
import glob

lRemove = ['_Data','_Link','noKey','HLTAuto','CBNT_']

def toSkip(inpName):
	for comp in lRemove:
		if inpName.count(comp)>0:
			return True
	return False

def toRemove(inpVal):
	result=False
	if isNumb(inpVal)==True:
		print "remove ", inpVal, "is a number"
		result=True
	for thing in lRemove:
		if inpVal.count(thing)>0:
			print "remove ", inpVal ,"since it contains", thing
			result=True
	return result

def findMatches(iAlg, iObj, F, Fa, Fo):
	n=0
	print "---- algos -----"
	for alg in iAlg: #----------------> find matching algos
		if alg.count(F)>0 and not toSkip(alg):
			Fa.append(alg)
			print  alg
			n+=1
	print '---- objects ----'
	for obj in iObj: #---------------> find matching objects
		if obj.count(F)>0 and not toSkip(obj):
			Fobj.append(obj)
			print  obj
			n+=1
	print '-----------------'
	return  n

nArgs= len (sys.argv)
inFN=''
if nArgs==1:
	print "Usage: checkFileSG.py input_filename "
	sys.exit()
if nArgs>1:
	print "Input file: ",sys.argv[1]
	inFN=sys.argv[1]
try:
	fi = open (inFN)
except:
	print "No such file!"
	sys.exit()
	

lines = fi.readlines()
fi.close() 

#===================================================================
line=lines.pop(0)
words=line.split(' ')
nAlgs=int(words[1])
print '%(a)d algorithms found'%{'a':nAlgs}

algs=[]
for i in range(nAlgs):   # -------------------> reading alg names
	line=lines.pop(0).strip('\n')
	line=line.replace('/','_')
	algs.append(line)
	
#print algs

line=lines.pop(0)
words=line.split(' ')
nObjs=int(words[1])
print '%(a)d objects found'%{'a':nObjs}

objs=[]
for i in range(nObjs):   # ----------------> reading object names
	line=lines.pop(0).strip('\n')
	line=line.replace('/','_')
	line=line.replace('<','LT')
	line=line.replace('>','GT')
	objs.append(line)
#==================================================================	


Falg=[]; Fobj=[]
LF=raw_input('Please enter algorithm or SG object name: ')


while (findMatches(algs, objs, LF, Falg, Fobj)!=1):   #----------> getting single input
	LF=raw_input('Please enter algorithm or SG object name. To exit type \'x\' : ')
	if LF=='x':
		sys.exit(0)
	Falg=[];Fobj=[]



print 'Calculating calls for:',LF,' '
if len(Falg)>0:
	#print 'algorithms', Falg
	ind=algs.index(Falg[0])+1
	for i in range(ind):
		line=lines.pop(0)
		#print line
	words=line.strip('\n').split('\t')
	for obj in objs:
		rw=words.pop(0).split(':')
		#print rw
		reads=int(rw[0])
		writes=int(rw[1])
		if (reads+writes)>0:
			print 'Object:',obj, "has been accessed",reads,"times and written",writes,"times."
		
else:
	#print 'objects',Fobj
	ind=objs.index(Fobj[0])
	for i in range(len(algs)):
		line=lines.pop(0)
		words=line.strip('\n').split('\t')
		rw=words[ind].split(':')
		reads=int(rw[0])
		writes=int(rw[1])
		if (reads+writes)>0:
			print 'Algorithm',algs[i],'accessed object',reads,'times and wrote it',writes,'times'
	
sys.exit(0)
