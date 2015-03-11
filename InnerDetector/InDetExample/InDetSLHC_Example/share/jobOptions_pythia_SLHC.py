#--------------------------------------------------------------
# Pythia job options for SLHC
# input environment variables:
#   SEEDNUMBER = seed part of a seed number
#--------------------------------------------------------------

import os
SeedNumber = os.environ['SEEDNUMBER']

import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.AppMgr import ServiceMgr
from PartPropSvc.PartPropSvcConf import PartPropSvc
ServiceMgr += PartPropSvc()
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 47890" + SeedNumber + " 98978" + SeedNumber, "PYTHIA_INIT 82056" + SeedNumber + " 23434" + SeedNumber]

from Pythia_i.Pythia_iConf import Pythia
Pythia=Pythia()
topSequence += Pythia

selector = "min_bias"
#selector = "wz"
#Wtojj = "0"
#Wtoen = "1"
#Wtomn = "0"
#Ztojj = "0"
#Ztoee = "1"
#Ztomm = "0"
uetune = 1

#####--------- Min_bias
if selector == "min_bias":
  Pythia.Tune_Name="ATLAS_20080001"
  Pythia.PythiaCommand = [
      "pysubs msel 1",        #pre-programmed processes 
      "pyinit win 14000.",
##      #"pypars mstp 2  2",    #second order running alpha_s
##      #"pypars mstp 33 3",    #K-factors
##      #"pysubs ckin 13 -0.1.", #low limit of the highest rapidity
##      #"pysubs ckin 14  0.1.", #hight limit of the highest rapidity
##      #"pysubs ckin 15 -0.1.", #low limit of the smallest rapidity
##      #"pysubs ckin 16  0.1.", #hight limit of the smallest rapidity
      #"pypars mstp 81 1",     #multiple interaction on
      #"pypars mstp 82 4",     #impact parameter choice
      #"pydat1 mstj 11 3",     #(D=4), fragementation=hybrid scheme
      #"pydat1 mstj 22 2",     #(D=1), dist to deacay a part=parj(71) 
      #"pydat1 parj 54 -0.07", #related to fragmentation
      #"pydat1 parj 55 -0.006",#related to fragmentation
      #"pypars parp 82 1.8",   #PT0 multiple distribution tail
      #"pystat 1 3 4 5",
      #"pyinit output event_listing.txt",
      #"pypars mstp 128 0",
      "pypars parp 84 0.5"]   #gaussian distribution mstp(82)=4)
###--------- Z/gamma + jet production
##Pythia.PythiaCommand = [
##--      "pysubs msel 13",       #(D=1), user controlled processes selection
##      #"pysubs msub 1 1",     #Z/gamma process
##      #"pypars mstp 43 1",    #Z only. D=1=Z/Gamma mixture
##--      "pysubs ckin 1 30.",   #low limit masse range: m=sqrt(s)
##--      "pydat3 mdme 174 1 0", #Z->dd
##--      "pydat3 mdme 175 1 0", #Z->uu
##--      "pydat3 mdme 176 1 0", #Z->ss
##--      "pydat3 mdme 177 1 0", #Z->cc
##--      "pydat3 mdme 178 1 0", #Z->bb
##--      "pydat3 mdme 179 1 0", #Z->tt
##--      "pydat3 mdme 182 1 1", #Z->ee
##--      "pydat3 mdme 183 1 0", #Z->nu_e nu_ve
##--      "pydat3 mdme 184 1 0", #Z->mu mu
##--      "pydat3 mdme 185 1 0", #Z->nu_mu nu_mu
##--      "pydat3 mdme 186 1 0", #Z->tau tau
##--      "pydat3 mdme 187 1 0", #Z->nu_tau nu_tau
##      #"pypars mstp 61 1",    #D=1=initial state shower (0=hard process only)
##      #"pypars mstp 71 1",    #D=1=final state shower (0=no FSR)
##      #"pypars mstp 81 1",    #D=1=multiple interaction (0=no mult inter)
##      #"pypars mstp 82 4",    #impact parameter choice
##      #"pydat1 mstj 11 3",    #(D=4), fragementation=hybrid scheme
##      #"pydat1 mstj 22 2",    #(D=1), cutt-off on decay lenght of a part
##      #"pydat1 parj 54 -0.07", #related to fragmentation
##      #"pydat1 parj 55 -0.006",#related to fragmentation
##      #"pypars parp 82 1.8",  #PT0 multiple distribution tail
##      #"pypars parp 84 0.5"  #gaussian distribution mstp(82)=4)
##      #"pydat3 mdcy 15 1 0",  #tau particle is not allowed to decay
##--      "pyinit pylisti 12",   #list the processs table
##--      "pyinit pylistf 1",
##--      "pystat 1 3 4 5",
##--      "pyinit dumpr 1 5",
##--      "pyinit output event_listing_zgamma.txt", 
##--      "pypars mstp 128 0"] 
####------MSSM Higgs (taken from an example: MSSM H to bbA to bbar mass of 115.)
##Pythia.PythiaCommand = [
##      "pydatr mrpy 1 2912",   #about ramdom numbers
##      "pyinit pylisti 12",    #list the processs table
##      "pyinit pylistf 1",     #listing of the event!!
##      "pyinit dumpr 1 1",     #
##      "pystat 1 3 4 5",
##      "pypars mstp 128 0",     #copy decay product to doc section
##      #"pysubs msub 186 1",     #gg->A0 QQbar process allowed
##      #"pysubs msub 187 1"      #qqbar->A0 QQbar process allowed
##      "pysubs msel 16",        # Select h0 Higgs production (press:102,123,124)
##      #"pyint2 kfpr 186 2 5",   #KF code in product of process 186
##      #"pyint2 kfpr 187 2 5",   #KF code in product of process 186
##      #"pysubs ckin 3  10",     #(D=2.) minimum Pt in a 2->2 process
##      #"pysubs ckin 51 10",     #(D=0.) minimum Pt in 2->3 process
##      #"pysubs ckin 52 -1",     #(D=-1.) maximum Pt in 2->3 process
##      #"pysubs ckin 53 10",     #(D=0) minimum Pt in 2->3 process
##      #"pysubs ckin 54 -1",     #(D=-1) maximum Pt in 2->3 process
##      "pymssm imss 1   2",     #SUGRA simulation
##      "pymssm rmss 1 300",     #common gaugino mass m1/2
##      "pymssm rmss 4  -1",     #sign of mu
##      "pymssm rmss 5   5",     #tan(beta)
##      "pymssm rmss 8 300",     #common scalar mass m0
##      "pymssm rmss 16  0",     #common trulinear coupling A
##      "pymssm imss 4   1",     #higgs sector mass determined by exact formulas
##      #"pymssm rmss 19 500",     #pseudo-scalar Higgs mass (M_A)
##      # Higgs h0 Decays
##      "pydat3 mdme 210 1 0",   # h->dd(bar)
##      "pydat3 mdme 211 1 0",   # h->uu
##      "pydat3 mdme 212 1 0",   # h->ss
##      "pydat3 mdme 213 1 0",   # h->cc
##      "pydat3 mdme 214 1 0",   # h->bb
##      "pydat3 mdme 215 1 0",   # h->tt
##      "pydat3 mdme 216 1 0",   # h->b'b'bar
##      "pydat3 mdme 217 1 0",   # h->t't'bar
##      "pydat3 mdme 218 1 0",   # h->ee
##      "pydat3 mdme 219 1 0",   # h->mumu
##      "pydat3 mdme 220 1 0",   # h->tautau
##      "pydat3 mdme 221 1 0",   # h->tau'+tau'-
##      "pydat3 mdme 222 1 0",   # h->gg
##      "pydat3 mdme 223 1 0",   # h->gammagamma
##      "pydat3 mdme 224 1 0",   # h->gammaZ
##      "pydat3 mdme 225 1 1",   # h->ZZ
##      "pydat3 mdme 226 1 0",   # h->WW
##      "pydat3 mdme 227 1 0",   # h->chi_10 chi_10
##      # Z Decays
##      "pydat3 mdme 174 1 1", #Z->dd
##      "pydat3 mdme 175 1 1", #Z->uu
##      "pydat3 mdme 176 1 1", #Z->ss
##      "pydat3 mdme 177 1 1", #Z->cc
##      "pydat3 mdme 178 1 1", #Z->bb
##      "pydat3 mdme 179 1 1", #Z->tt
##      "pydat3 mdme 182 1 0", #Z->ee
##      "pydat3 mdme 183 1 0", #Z->nu_e nu_ve
##      "pydat3 mdme 184 1 1", #Z->mu mu
##      "pydat3 mdme 185 1 0", #Z->nu_mu nu_mu
##      "pydat3 mdme 186 1 0", #Z->tau tau
##      "pydat3 mdme 187 1 0", #Z->nu_tau nu_tau
##      #"pydat3 mdme 188 1 0", #Z->tau'tau'
##      #"pydat3 mdme 189 1 0", #Z->nu_tau' nu_tau'
##      # Other stuff
##      "pyinit pylisti 12",   #list the processs table
##      "pyinit pylistf 1",
##      "pystat 1 3 4 5",
##      "pypars parp 82 2.2",  #PT0 multiple distribution tail
##      "pyinit output event_listing.txt", 
##      "pyinit dumpr 1 1"];
 
#####-----SM Higgs
##Pythia.PythiaCommand = [
##      "pydat2 pmas 25 1 185.", # Higgs (h0) mass
      #"pysubs msel 16",        # Select h0 Higgs production
##      "pysubs msel 0",        # user selected processes
      #"pysubs msub 102 1",     #gg->h0 (gluon-gluon fusion)
##      "pysubs msub 123 1",     #qq->h0 (ZZ fusion)
##      "pysubs msub 124 1",     #qq->h0 (WW fusion)
      # Higgs Decays
##      "pydat3 mdme 210 1 0",   # h->dd(bar)
##      "pydat3 mdme 211 1 0",   # h->uu
##      "pydat3 mdme 212 1 0",   # h->ss
##      "pydat3 mdme 213 1 0",   # h->cc
##      "pydat3 mdme 214 1 0",   # h->bb
##      "pydat3 mdme 215 1 0",   # h->tt
##      "pydat3 mdme 218 1 0",   # h->ee
##      "pydat3 mdme 219 1 0",   # h->mumu
##      "pydat3 mdme 220 1 0",   # h->tautau
##      "pydat3 mdme 222 1 0",   # h->gg
##      "pydat3 mdme 223 1 0",   # h->gammagamma
##      "pydat3 mdme 224 1 0",   # h->gammaZ
##      "pydat3 mdme 225 1 1",   # h->ZZ
##      "pydat3 mdme 226 1 0",   # h->WW
      # Z Decays
##      "pydat3 mdme 174 1 0", #Z->dd
##      "pydat3 mdme 175 1 0", #Z->uu
##      "pydat3 mdme 176 1 0", #Z->ss
##      "pydat3 mdme 177 1 0", #Z->cc
      #"pydat3 mdme 178 1 4", #Z->bb
##      "pydat3 mdme 178 1 0", #Z->bb
##      "pydat3 mdme 179 1 0", #Z->tt
      #"pydat3 mdme 182 1 0", #Z->ee
##      "pydat3 mdme 182 1 1", #Z->ee
##      "pydat3 mdme 183 1 0", #Z->nu_e nu_ve
##      "pydat3 mdme 184 1 0", #Z->mu mu
##      "pydat3 mdme 185 1 0", #Z->nu_mu nu_mu
##      "pydat3 mdme 186 1 0", #Z->tau tau
##      "pydat3 mdme 187 1 0", #Z->nu_tau nu_tau
      #"pydat3 mdme 188 1 0", #Z->tau'tau'
      #"pydat3 mdme 189 1 0", #Z->nu_tau' nu_tau'
      # Other stuff
##      "pyinit pylisti 12",   #list the processs table
##      "pyinit pylistf 1",
##      "pystat 1 3 4 5",
##      "pypars parp 82 2.2",  #PT0 multiple distribution tail
##      "pyinit output event_listing_higgs_gf.txt", 
##      "pyinit dumpr 1 5"]

###-----ttbar production
elif selector == "ttbar":
  Pythia.PythiaCommand = [
        "pysubs msel 6",        # ttbar production
##      #"pysubs msub 81 1",     #qqbar->ttbar
##      #"pysubs msub 82 1",     #gg->ttbar
##      #"pypars parp 51 7",  #PDF: D=7=CTEQ5L
##      # W decays
        "pydat3 mdme 190 1 0",#W->qq
        "pydat3 mdme 191 1 0",#W->qq
        "pydat3 mdme 192 1 0",#W->qq
        "pydat3 mdme 194 1 0",#W->qq
        "pydat3 mdme 195 1 0",#W->qq
        "pydat3 mdme 196 1 0",#W->qq
        "pydat3 mdme 198 1 0",#W->qq
        "pydat3 mdme 199 1 0",#W->qq
        "pydat3 mdme 200 1 0",#W->qq
        "pydat3 mdme 206 1 1",#W->ev
        "pydat3 mdme 207 1 0",#W->muv
        "pydat3 mdme 208 1 0",#W->tauv   
        "pypars parp 82 2.2",  #PT0 multiple distribution tail
        # Other stuff
        "pyinit pylisti 12",   #list the processs table
        "pyinit pylistf 1",
        "pystat 1 3 4 5",
        "pyinit output event_listing_ttbar.txt", 
        "pyinit dumpr 1 1"]
###-----bb(bar) production
elif selector == "bbbar":
  Pythia.PythiaCommand = [
        "pysubs msel 5",        # =5=bb(bar) production
##      #"pysubs msub 81 1",     #qqbar->bb(bar)
##      #"pysubs msub 82 1",     #gg->bb(bar)
##      #"pypars parp 51 7",  #PDF: D=7=CTEQ5L
        "pypars parp 82 2.2",  #PT0 multiple distribution tail
##      # Other stuff
        "pyinit pylisti 12",   #list the processs table
        "pyinit pylistf 1",
        "pyinit output event_listing_bbar.txt", 
        "pyinit dumpr 1 1"]
#####-----ZZ->eeqq
elif selector == "zzeeqq":
  Pythia.PythiaCommand = [
        "pysubs msel 0",        # user selected processes
        "pysubs msub 22 1",     #ffbar->(z/gamma*)(z/gamma*)
        # Z Decays
        "pydat3 mdme 174 1 1", #Z->dd
        "pydat3 mdme 175 1 1", #Z->uu
        "pydat3 mdme 176 1 1", #Z->ss
        "pydat3 mdme 177 1 1", #Z->cc
        "pydat3 mdme 178 1 1", #Z->bb
        "pydat3 mdme 179 1 1", #Z->tt
        "pydat3 mdme 182 1 0", #Z->ee
        "pydat3 mdme 183 1 0", #Z->nu_e nu_ve
        "pydat3 mdme 184 1 0", #Z->mu mu
        "pydat3 mdme 185 1 0", #Z->nu_mu nu_mu
        "pydat3 mdme 186 1 0", #Z->tau tau
        "pydat3 mdme 187 1 0", #Z->nu_tau nu_tau
        #"pydat3 mdme 188 1 0", #Z->tau'tau'
        #"pydat3 mdme 189 1 0", #Z->nu_tau' nu_tau'
        # Other stuff
        "pyinit pylisti 12",   #list the processs table
        "pyinit pylistf 1",
        "pypars parp 82 2.2",  #PT0 multiple distribution tail
        "pystat 1 3 4 5",
        "pyinit output event_listing_zz.txt", 
        "pyinit dumpr 1 1"]

###---- WZ->enu ee
elif selector == "wz":

  Pythia.PythiaCommand = [
    # Set process type and W' mass
    "pysubs msel 0",
    
    "pysubs msub 73 1",            # WZ -> WZ (longitudinal)
    "pypars mstp 46 5",            #
    "pypars parp 45 700",          # Resonnance Mass
    
    #####################
    # Z -> ee or mu mu (apago los jets)
    # the last parameter turns on(1) or turns off(0) the desired channel
    # but he will take in to account for the width ! ... watch out !

    # Z -> to quarks
    "pydat3 mdme 174 1 "+Ztojj,
    "pydat3 mdme 175 1 "+Ztojj,
    "pydat3 mdme 176 1 "+Ztojj,
    "pydat3 mdme 177 1 "+Ztojj,
    "pydat3 mdme 178 1 "+Ztojj,
    "pydat3 mdme 179 1 "+Ztojj,
    
    # Z -> ll
    "pydat3 mdme 182 1 "+Ztoee,
    "pydat3 mdme 183 1 0",      # to nu_e
    "pydat3 mdme 184 1 "+Ztomm,
    "pydat3 mdme 185 1 0",     # to nu_mu
    
    # Close Z -> tau channels
    "pydat3 mdme 186 1 0",
    "pydat3 mdme 187 1 0",
    "pydat3 mdme 188 1 0",
    "pydat3 mdme 189 1 0",
    
    ######################
    # W -> leptons chanels  -------------
    "pydat3 mdme 206 1 "+Wtoen,
    "pydat3 mdme 207 1 "+Wtomn,

    # Close W -> tau channels
    "pydat3 mdme 208 1 0", # no tau
    "pydat3 mdme 209 1 0",
    
    # W -> quarks channels -------------------
    "pydat3 mdme 190 1 "+Wtojj,
    "pydat3 mdme 191 1 "+Wtojj,
    "pydat3 mdme 192 1 "+Wtojj,
    # "pydat3 mdme 193 1 "+Wtojj,  # to t' : keep default value
    "pydat3 mdme 194 1 "+Wtojj,
    "pydat3 mdme 195 1 "+Wtojj,
    "pydat3 mdme 196 1 "+Wtojj,
    #"pydat3 mdme 197 1 "+Wtojj, # to t' : keep default value
    "pydat3 mdme 198 1 "+Wtojj,
    "pydat3 mdme 199 1 "+Wtojj,
    "pydat3 mdme 200 1 "+Wtojj,
    #    "pydat3 mdme 201 1 "+Wtojj,    # to t' : keep default value
    ##     "pydat3 mdme 202 1 "+Wtojj,  # to t' : keep default value
    ##     "pydat3 mdme 203 1 "+Wtojj,  # to t' : keep default value
    ##     "pydat3 mdme 204 1 "+Wtojj,  # to t' : keep default value
    ##     "pydat3 mdme 205 1 "+Wtojj, # to t' : keep default value

    ######################
    "pysubs ckin 1 300",   # minimun for the invariant mass
                           # of WZ
    
    "pyinit pylistf 1",
    # list 1st 10 events
    "pyinit dumpr 1 10",

    # print the decay info
    "pyinit pylisti 12"

  ]

#UE tune (from Generators/DC3_joboptions/share/DC3_Pythia64UE_tune.py
if uetune == 1:
  Pythia.PythiaCommand += [
                   "pypars mstp 70 2",
                   "pypars mstp 72 0",
                   "pypars mstp 81 21",
                   "pypars mstp 82 4",
                   "pypars mstp 84 1",
                   "pypars mstp 85 1",
                   "pypars mstp 86 2",
                   "pypars mstp 87 4",
                   "pypars mstp 88 0",
                   "pypars mstp 89 1",
                   "pypars mstp 90 1",
                   "pypars mstp 95 1",
                   "pypars parp 78 0.2",
                   "pypars parp 80 0.01",
                   "pypars parp 82 1.9",
                   "pypars parp 83 0.3",
                   "pypars parp 84 0.5",
                   "pypars parp 89 1800",
                   "pypars parp 90 0.22",
                   "pydat1 parj 81 0.14"]

