#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os
import re
import math
import subprocess as sp

def fcheck(f,v=1.e-4):
    return abs(float(f))>v

# dot -Tps -oout.ps -Gsize="7.5,10" out.dot

class Event:
    def __init__(self,idx):
        self.id=idx
        self.vtx_list={}
        self.particles={}
        self.print_momentum=True
        self.print_position=True

    def createParticle(self,idx,pdg,l):
        p=Particle(idx,pdg)
        if self.particles.has_key(p.id):
            p1=self.particles[p.id]
            if p.pdg!=p1.pdg :
                print 'ERROR particle ',idx,'already exists with different pdgcode'
            return p1

        # "O: 2    201974       13 +2.44e+02,-1.51e+01,+4.94e+03,+4.94e+03   1"
        m=re.search(" ([e0-9\+\-\.]*),(.*),(.*),([e0-9\+\-\.]*) ",l)
        if m:
            p.mom=tuple([float(f) if fcheck(f) else 0. for f in m.groups()[:3] ])
        self.particles[p.id]=p
        return p

    def createVertex(self,idx,l=''):
        vtx=Vertex(idx)
        vtx.evt=self
        #pos="(X,cT)=-8.06e-02,+1.08e+00,+1.29e+02,+0.00e+00"
        m=re.search("=(.*),(.*),(.*),(.*)$",l)
        if m:
            vtx.pos=tuple([float(f) if fcheck(f) else 0. for f in m.groups()[:3] ])
        self.vtx_list[vtxid]=vtx
        return vtx

    def hideAll(self):
        for i,p in self.particles.iteritems():
            p.hide=True
        for i,vtx in self.vtx_list.iteritems():
            vtx.hide=True

class Particle:
    def __init__(self,idx,pdg):
        if idx>1000000000:
            # probably overlap between idx and pdg, guessing TODO: improve with pattern matching
            #print 'guessing',idx
            sidx=str(idx)
            idx,pdg=int(sidx[:6]),int(sidx[6:])
        self.id=idx
        self.pdg=pdg
        self.mother=None
        self.vtx=None
        self.color=None
        self.hide=False
        self.mom=(0.,0.,0.)

    def Highlight(self,color='yellow'):
        self.color=color
        self.hide=False
        if self.vtx is not None:
            self.vtx.Highlight(color)

    def __repr__(self):
        mid=0
        if self.mother is not None:
            mid=self.mother.id
        return 'Particle(%d,%s,%s,%d)'%(self.id,self.pdg,self.mom,mid)

class Vertex:
    def __init__(self,idx):
        self.id=idx
        self.ins=[]
        self.out=[]
        self.pos=(0.,0.,0.)
        self.color=None
        self.hide=False
        self.evt=None

    def Highlight(self,color='yellow',recursive=True):
        print 'highlight',self.id
        self.color=color
        self.hide=False
        if recursive:
            for p in self.out:
                p.Highlight(color)

    def createInParticle(self,idx,pdg,pos=None):
        p=self.evt.createParticle(idx,pdg,pos)
        self.ins.append(p)
        p.vtx=self

    def createOutParticle(self,idx,pdg,pos=None):
        p=self.evt.createParticle(idx,pdg,pos)
        self.out.append(p)
        p.mother=self

    def __repr__(self):
        return 'Vertex(%d,%s)'%(self.id,self.pos)

evt=None
evtplot=0
evtnum=0

vtx=None
vtxbegin=False
vtxend=False

evt_list=[]

inpart=0
outpart=0

print "parsing TruthEvent"

iname="run7_minbiaslog"
if len(sys.argv)>=2 :
    iname=sys.argv[1]
f = file(iname)

# ----------------------------------------
# start parse particles
# ----------------------------------------
for l in f:
    if l.startswith("GenEvent"):
        xx=l.split()
        evtnum=int(xx[1].strip('#'))
        sigvtx=int(xx[5])
        print 'new GenEvent',evtnum,' signal',sigvtx
        print xx
        evt=Event(evtnum)
        evt_list.append(evt)
    if l.startswith("__________________________"):
        if evtnum>0 and vtxbegin==False:
            vtxbegin=True
            vtxend=False
        elif vtxbegin and not vtxend:
            vtxbegin=False
            vtxend=True
            evtnum=0
    if vtxbegin: # evtnum==evtplot
        #print l
        if l.startswith("GenVertex") or l.startswith("Vertex"):
            ll=l.split()
            vtxid=int(ll[1])
            vtx=evt.createVertex(vtxid,ll[4])
        elif l.startswith(" I:"):
            xx=l.replace("I:","I: ").split()
            inpart=int(xx[1])-1
            idx=int(xx[2])
            pdg=xx[3]
            p=vtx.createInParticle(idx,pdg,l)
        elif l.startswith(" O:"):
            xx=l.replace("O:","O: ").split()
            outpart=int(xx[1])-1
            idx=int(xx[2])
            pdg=xx[3]
            p=vtx.createOutParticle(idx,pdg,l)
        elif inpart>0:
            xx=l.split()
            idx=int(xx[0])
            pdg=xx[1]
            p=vtx.createInParticle(idx,pdg,l)
            inpart-=1
        elif outpart>0:
            xx=l.split()
            idx=int(xx[0])
            pdg=xx[1]
            p=vtx.createOutParticle(idx,pdg,l)
            outpart-=1

# ----------------------------------------
# end parse particles
# ----------------------------------------
evt=evt_list[evtplot]
print 'number of vtx in evt=',evtplot,' found',len(evt.vtx_list)


# ----------------------------------------
# hide all but the highlighted part below
# ----------------------------------------
#evt.hideAll()


# ----------------------------------------
# highlight special vertex and daughters
# ----------------------------------------
#vtx=evt.vtx_list[-534]
#vtx.Highlight()
#vtx.Highlight(color='red',recursive=False)
#vtx=evt.vtx_list[-651]
#vtx.Highlight()
#vtx.Highlight(color='green',recursive=False)

# ----------------------------------------
# highlight verticies with zero position
# ----------------------------------------
for vtx in evt.vtx_list.values():
    if vtx.pos==(0.,0.,0.):
        vtx.Highlight()
        vtx.Highlight(color='red',recursive=False)


# ----------------------------------------
# start writing dot file
# ----------------------------------------
fname='out.dot'
if len(sys.argv)>2 :
    fname=sys.argv[2]
else:
    fname=os.path.splitext(iname)[0]+'.dot'
    
f=file(fname,'w')

f.write('digraph Event%d { \n'%evtplot)
# write particle blobs
for p in evt.particles.values():
    pid = 'p_%d'%p.id
    if not p.hide:
        cstr=''
        if p.color is not None:
            cstr=',style=filled,fillcolor=%s'%p.color
        label=',label="%s\\npdg:%s"'%(pid,p.pdg)
        if evt.print_momentum:
            mom='(%g,%g,%g)'%p.mom
            label=',label="%s\\npdg:%s\\nmom:%s"'%(pid,p.pdg,mom)
        f.write(pid+'[shape=box'+cstr+label+'] ;\n')

# write verticies
for vtx in evt.vtx_list.values():
    vtxid = 'vtx_%d'%-vtx.id
    if not vtx.hide:
        cstr=label=''
        if vtx.color is not None:
            cstr=',style=filled,fillcolor=%s'%vtx.color
        if evt.print_position:
            pos='(%g,%g,%g)'%vtx.pos
            label=',label="%s\\npos:%s"'%(vtxid,pos)
        f.write(vtxid+'[shape=ellipse'+cstr+label+']; \n')

    # add links to incoming particles
    for p in vtx.ins:
        pid = 'p_%d'%p.id
        if not p.hide:
            f.write( pid+' -> '+vtxid+';\n')
    # add links to outgoing particles
    for p in vtx.out:
        pid = 'p_%d'%p.id
        if not p.hide:
            f.write( vtxid+' -> '+pid+';\n')
        
# ----------------------------------------
# end dot file
# ----------------------------------------
f.write('}\n')
print 'writen to ',fname
f=None

# ----------------------------------------
# hints for creating ps/pdf file
# ----------------------------------------

pname=os.path.splitext(fname)[0]+'.ps'
    
cmd='dot -Tps2 %s -Gsize="75,100" -o%s'%(fname,pname)
err = sp.call(cmd,shell=True)
if err!=0:
    print 'failed to convert',fname,'to',pname
else :
    print 'created',pname

# for large graphs use size option (otherwise dot will complain)
# dot -Tps2 devvalr5_TruthEvent.event2.dot -Gsize="75,100" -oout.ps


cmd='mv %s %s'%(pname,pname+'.tmp')
sp.call(cmd,shell=True)

print 'fixing bounding box'
fin=open(pname+'.tmp')
fout=open(pname,'w')
for l in fin:
    m=re.search('(%%.*BoundingBox:) ([0-9]*) ([0-9]*) ([0-9]*) ([0-9]*)',l)
    if m is not None:
        key,x1,y1,x2,y2=m.groups()
        x1,y1,x2,y2=[int(s) for s in m.groups()[1:]]
        y2+=1
        fout.write('%s %d %d %d %d\n'%(key,x1,y1,x2,y2))
        continue
    m=re.search('<< /(PageSize) \[([0-9]*) ([0-9]*)\] >> setpagedevice',l)
    if m is not None:
        xx,yy=[int(s) for s in m.groups()[1:]]
        yy=int(yy)+1
        fout.write('<< /PageSize [%d %d] >> setpagedevice\n'%(xx,yy))
        continue
    m=re.search('\[ /(CropBox) \[([0-9]*) ([0-9]*) ([0-9]*) ([0-9]*)\] /PAGES pdfmark',l)
    if m is not None:
        x1,y1,x2,y2=[int(s) for s in m.groups()[1:]]
        y2+=1
        fout.write('[ /CropBox [%d %d %d %d] /PAGES pdfmark\n'%(x1,y1,x2,y2))
        continue
    m=re.search('^([0-9]*) ([0-9]*) ([0-9]*) ([0-9]*) boxprim clip newpath',l)
    if m is not None:
        x1,y1,x2,y2=[int(s) for s in m.groups()]
        y2+=1
        fout.write('%d %d %d %d boxprim clip newpath\n'%(x1,y1,x2,y2))
        continue

    fout.write(l)
fout=None
fin=None

print 'creating pdf file'
sp.call('ps2pdf %s'%pname,shell=True)

