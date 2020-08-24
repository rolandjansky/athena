#!/usr/bin/env python
import os,sys

inFile="README.md"
if len(sys.argv) >1:
 inFile=sys.argv[1]

f = open(inFile)

regularLines=[]
tocLines=[]

afterBreak=False
afterTOC=False
inCodeBlock=False

for line in f.readlines():
  if '```' in line: inCodeBlock= not inCodeBlock
  if '<br/>' in line: 
    #regularLines+=[line]
    afterBreak=True
  if 'Table of Contents:' in line: 
    regularLines+=[line]
    afterTOC=True
  if not (afterTOC and not afterBreak): regularLines+=[line]

  if '#'==line[0] and afterBreak and not inCodeBlock:  
    header=line.split('#')[-1].strip()
    tag='#'+header.lower().replace(" ","-").replace(",","").replace("?","").replace("(","").replace(")","").replace("/","")
    nIndents=line.count('#')
    tocLine=""
    for i in range(nIndents-1):
      tocLine+="\t"
    tocLine+="- [%s](%s)\n"%(header,tag)
    tocLines+=[tocLine]

fOut = open (inFile+".tmp",'w')

for rl in regularLines:
  fOut.write(rl)
  if ('Table of Contents:') in rl:
    for tl in tocLines:
      fOut.write(tl)

os.system('mv %s %s'%(inFile+".tmp",inFile))
