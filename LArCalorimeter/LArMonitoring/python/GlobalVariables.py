from ROOT import TMath

class LArDQGlobals(object):
      __slots__ = ('HVeta_EMB','HVphi_EMB','HVeta_EMEC','HVphi_EMEC','HVeta_HECFcal','HVphi_HECFcal',
                   'LB_Bins','LB_Min','LB_Max','BCID_Bins','BCID_Min','BCID_Max',
                   'colTime_Bins','colTime_Min','colTime_Max','avgColTime_Bins','avgColTime_Min','avgColTime_Max',
                   'FEB_Feedthrough','FEB_Slot','FEB_N_channels','N_FEB_Parttions_Max','N_FEB')

lArDQGlobals = LArDQGlobals()

#HV eta-phi bins (used by LArAffectedRegions, bin values copied from there). NOTE: EMEC bins are provided as a list of bin boundaries (variable bin size), all other partitions are in format [Nbins,x_low,x_high]
#barrel
lArDQGlobals.HVeta_EMB = {"EMBAPS":[8, 0.,1.6],"EMBA":[7,0.,1.4]}
lArDQGlobals.HVphi_EMB = {"EMBAPS":[32,-TMath.Pi(),TMath.Pi()],"EMBA":[128,-TMath.Pi(),TMath.Pi()]}
for end in ["PS",""]:
    bins=lArDQGlobals.HVeta_EMB["EMBA"+end]
    lArDQGlobals.HVeta_EMB["EMBC"+end]=[bins[0],-1*(bins[2]+0.01),-1*(bins[1]+0.01)]
    lArDQGlobals.HVphi_EMB["EMBC"+end]=lArDQGlobals.HVphi_EMB["EMBA"+end]
#endcap
emecbinsA=[1.375,1.50,1.6,1.8,2.0,2.1,2.3,2.5,2.8,3.2]
emecbinsC=[-1*(bi+0.01) for bi in reversed(emecbinsA)]
phibinsPS=64
phibins=256
lArDQGlobals.HVeta_EMEC = {"EMECAPS":emecbinsA,"EMECA":emecbinsA,"EMECCPS":emecbinsC,"EMECC":emecbinsC} 
lArDQGlobals.HVphi_EMEC={}
lArDQGlobals.HVphi_EMEC["EMECAPS"]=[-TMath.Pi()+ x*2*TMath.Pi()/phibinsPS for x in xrange(phibinsPS+1)]
lArDQGlobals.HVphi_EMEC["EMECA"]=[-TMath.Pi()+ x*2*TMath.Pi()/phibins for x in xrange(phibins+1)]
lArDQGlobals.HVphi_EMEC["EMECCPS"]=lArDQGlobals.HVphi_EMEC["EMECAPS"]
lArDQGlobals.HVphi_EMEC["EMECC"]=lArDQGlobals.HVphi_EMEC["EMECA"]
#hec-fcal
lArDQGlobals.HVeta_HECFcal = {"HECA":[1,1.5, 3.2],"FCalA":[1, 2.9, 4.9]} #same for all layers
lArDQGlobals.HVphi_HECFcal = {"HECA":[32,-TMath.Pi(),TMath.Pi()],"FCalA":[16, -3.2, 3.2]} #same for all layers
for par in ["HEC","FCal"]:
    bins=lArDQGlobals.HVeta_HECFcal[par+"A"]
    lArDQGlobals.HVeta_HECFcal[par+"C"]=[bins[0],-1*(bins[2]+0.01),-1*(bins[1]+0.01)]
    lArDQGlobals.HVphi_HECFcal[par+"C"]=lArDQGlobals.HVphi_HECFcal[par+"A"]
#Lumi block ranges
lArDQGlobals.LB_Bins=3000
lArDQGlobals.LB_Min=0
lArDQGlobals.LB_Max=lArDQGlobals.LB_Bins+lArDQGlobals.LB_Min
#bunch crossing id ranges
lArDQGlobals.BCID_Bins=3564
lArDQGlobals.BCID_Min=0.5
lArDQGlobals.BCID_Max=lArDQGlobals.BCID_Min+lArDQGlobals.BCID_Bins
#LArCollisionTime (A-C) ranges
lArDQGlobals.colTime_Bins=101
lArDQGlobals.colTime_Min=-50.5
lArDQGlobals.colTime_Max=50.5
lArDQGlobals.avgColTime_Bins=320
lArDQGlobals.avgColTime_Min=-40
lArDQGlobals.avgColTime_Max=40
#FEB ranges
lArDQGlobals.FEB_Feedthrough={"EMBA":[0,31],"EMBC":[0,31],"EMECA":[0,24],"EMECC":[0,24],"HECA":[0,24],"HECC":[0,24],"FCalA":[0,24],"FCalC":[0,24]}
lArDQGlobals.FEB_Slot={"EMBA":[1,14],"EMBC":[1,14],"EMECA":[1,15],"EMECC":[1,15],"HECA":[1,15],"HECC":[1,15],"FCalA":[1,15],"FCalC":[1,15]}
#Feb N channels
lArDQGlobals.FEB_N_channels=128

#total number of FEB
lArDQGlobals.N_FEB=1524
lArDQGlobals.N_FEB_Parttions_Max=500



