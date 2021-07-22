#! -*- python -*-
evgenConfig.description = "Single particle gun for Sampling Fraction event generation"
evgenConfig.keywords = ["singleParticle",]
evgenConfig.generators = ["ParticleGun"]
evgenConfig.contact = ["michael.duehrssen@cern.ch"]

import ParticleGun as PG
import ROOT, math, random

class MyParticleSampler(PG.ParticleSampler):
    """
    Projective showers starting at entrance of calorimeter, flat in eta, constant energy
    """

    def __init__(self,pid=11,momentum=50000.,eta1=0.,eta2=1.4,bec=0,radius=1500.,z=3740.5):
        self.pid = pid
        self.momentum = momentum
        self.eta1 = eta1
        self.eta2 = eta2
        pdg_table = ROOT.TDatabasePDG.Instance()
        self.mass = pdg_table.GetParticle(self.pid()).Mass()
        self.bec=bec
        self.radius=radius
        self.z=z

    def shoot(self):
       rtn=[]
       eta = random.uniform(self.eta1, self.eta2) 
       phi = random.uniform(0,random.TWOPI)
       momentum=self.momentum
       px=momentum*math.cos(phi)/math.cosh(eta)
       py=momentum*math.sin(phi)/math.cosh(eta)
       pz=momentum*math.sinh(eta)/math.cosh(eta)
       e=math.sqrt(momentum*momentum+self.mass*self.mass)
       v4 = ROOT.TLorentzVector(px,py,pz,e)
       if self.bec==0:
           radius= self.radius
           x=radius*math.cos(phi)
           y=radius*math.sin(phi)
           z=radius*math.sinh(eta)
       else:
           z=self.z
           radius=z/math.sinh(eta)
           x=radius*math.cos(phi)
           y=radius*math.sin(phi)
       t=math.sqrt(x*x+y*y+z*z)
       vp = ROOT.TLorentzVector(x,y,z,t)
       p = PG.SampledParticle(pid=self.pid(),mom=v4,pos=vp)
       #print "E,eta,phi,mass ",e,eta,phi,self.mass,"  position ",x,y,z," pid=",p.pid
       rtn.append(p)
       return rtn

##MC15 style with Generate_tf.py
#args=jofile.split('.py')[0]

##MC16 style with Gen_tf.py 
FIRST_DIR = (os.environ['JOBOPTSEARCHPATH']).split(":")[0]
jofiles = [f for f in os.listdir(FIRST_DIR) if (f.startswith('mc') and f.endswith('.py'))]

print "================ SETTTINGS ================="
print ("jofiles     = ", jofiles)

### parse options from MC job-options filename
args = jofiles[0].split('.py')[0]
print ("args     = ", args)

myMomentum = float(args.split('_Mom')[1].split('_')[0])
print ("Momentum = ", myMomentum,"MeV")

myPDGID = int(float(args.split('_pid')[1].split('_')[0].replace('m','-')))
print ("pid      = ", myPDGID)

myLowEta  = 0.01*float(args.split('eta_')[1].split('_')[0].replace('m','-'))
print ("etalow   = ", myLowEta)

myHighEta = 0.01*float(args.split('eta_')[1].split('_')[1].replace('m','-'))
print ("etahigh   = ", myHighEta)

if "_Radius" in args:
  myRadius = 0.001*float(args.split('_Radius')[1].split('_')[0]) #Argument needs to by in mum, since a "." in the filename is not allowed
else: 
  myRadius = 1500.
print ("radius   = ", myRadius,"mm")

if "_Z" in args:
  myZ = 0.001*float(args.split('_Z')[1].split('_')[0]) #Argument needs to by in mum, since a "." in the filename is not allowed
else:  
  myZ = 3740.5
print ("Z        = ", myZ,"mm")
  
if "bec" in args:
  bec=1
else:
  bec=0
print ("bec      = ", bec)
print "============================================"

genSeq += PG.ParticleGun()
genSeq.ParticleGun.sampler = MyParticleSampler(momentum=myMomentum,eta1=myLowEta,eta2=myHighEta,pid=myPDGID,bec=bec,radius=myRadius,z=myZ)

