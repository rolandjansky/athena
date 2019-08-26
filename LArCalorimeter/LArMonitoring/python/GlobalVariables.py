
from ROOT import TMath

#Lumi block ranges
LB_Bins=3000
LB_Min=0
LB_Max=LB_Bins+LB_Min


#bunch crossing id ranges
BCID_Bins=3564
BCID_Min=0.5
BCID_Max=BCID_Min+BCID_Bins

#LArCollisionTime (A-C) ranges
colTime_Nbins=101
colTime_min=-50.5
colTime_max=50.5
avgColTime_Nbins=320
avgColTime_min=-40
avgColTime_max=40

#FEB ranges
FEB_Feedthrough={"EMBA":[0,31],"EMBC":[0,31],"EMECA":[0,24],"EMECC":[0,24],"HECA":[0,24],"HECC":[0,24],"FCalA":[0,24],"FCalC":[0,24]}
FEB_Slot={"EMBA":[1,14],"EMBC":[1,14],"EMECA":[1,15],"EMECC":[1,15],"HECA":[1,15],"HECC":[1,15],"FCalA":[1,15],"FCalC":[1,15]}

#Feb N channels
FEB_N_channels=128

#total number of FEB
N_FEB=1524
N_FEB_Parttions_Max=500

#ROD ranges
ROD_Crates={"EMBA":[1,4],"EMBC":[1,4],"EMECA":[1,3],"EMECC":[1,3],"HECA":[1],"HECC":[1],"FCalA":[1],"FCalC":[1]}
ROD_Slots={"EMBA":[1,14],"EMBC":[1,14],"EMECA":[1,13],"EMECC":[1,13],"HECA":[1,6],"HECC":[1,6],"FCalA":[1,4],"FCalC":[1,4]}

#ROS ranges
ROS_link={"EMBA":[0,31],"EMBC":[0,31],"EMECA":[0,24],"EMECC":[0,24],"HECA":[0,24],"HECC":[0,24],"FCalA":[0,24],"FCalC":[0,24]}

#number of cells
TotalNcell=182468

#Samples
Sample_N_Min=0
Sample_N_Max=32

#Cell time (in ns)
Cell_Time_Min=-200
Cell_Time_Max=200

#Energies ranges
Emin=-20000.0
Emax=20000.0

#Difference Energies
DEmin=-512
DEmax=512

#Qfactor range
Qmin=0
Qmax=6553

#Differences Q factor
DQmin=-3000
DQmax=1000

#Time ranges (ps)
Tmin=-80000
Tmax=80000

#Diff Time ranges (ps)
DTmin=-800
DTmax=800

#Online Vs offline computations Energies ranges
OnlineOffline=[0,4096,65536,524288,4194304]

#triggerWord
TTriggerTypeMax=256

#Cell threshold ADC counts
MaxCellThresholdADC=1900

#Corruption errors
CorruptionSource=["Parity","BCID Mismatch","Sample header","EVT ID","SCAC status","Sca out of range","Gain mismatch","Type mismatch","# of samples","Empty data block","Checksum /block size","Missing header","Bad gain"]

#Collision time range
CollTimeRange=[-50,50]

#LAr streams
Streams=["express_express","physics_CosmicCalo","physics_Zerobias","physics_Main",]

#Gains
Ginas=["Low Gain","Medium Gain","High Gain"]


#HV eta-phi bins (used by LArAffectedRegions, bin values copied from there). NOTE: EMEC bins are provided as a list of bin boundaries (variable bin size), all other partitions are in format [Nbins,x_low,x_high]
#barrel
HVeta_EMB = {"EMBAPS":[8, 0.,1.6],"EMBA":[7,0.,1.4]}
HVphi_EMB = {"EMBAPS":[32,-TMath.Pi(),TMath.Pi()],"EMBA":[128,-TMath.Pi(),TMath.Pi()]}
for end in ["PS",""]:
    bins=HVeta_EMB["EMBA"+end]
    HVeta_EMB["EMBC"+end]=[bins[0],-1*(bins[2]+0.01),-1*(bins[1]+0.01)]
    HVphi_EMB["EMBC"+end]=HVphi_EMB["EMBA"+end]
#endcap
emecbinsA=[1.375,1.50,1.6,1.8,2.0,2.1,2.3,2.5,2.8,3.2]
emecbinsC=[-1*(bi+0.01) for bi in reversed(emecbinsA)]
phibinsPS=64
phibins=256
HVeta_EMEC = {"EMECAPS":emecbinsA,"EMECA":emecbinsA,"EMECCPS":emecbinsC,"EMECC":emecbinsC} 
HVphi_EMEC={}
HVphi_EMEC["EMECAPS"]=[-TMath.Pi()+ x*2*TMath.Pi()/phibinsPS for x in xrange(phibinsPS+1)]
HVphi_EMEC["EMECA"]=[-TMath.Pi()+ x*2*TMath.Pi()/phibins for x in xrange(phibins+1)]
HVphi_EMEC["EMECCPS"]=HVphi_EMEC["EMECAPS"]
HVphi_EMEC["EMECC"]=HVphi_EMEC["EMECA"]
#hec-fcal
HVeta_HECFcal = {"HECA":[1,1.5, 3.2],"FCalA":[1, 2.9, 4.9]} #same for all layers
HVphi_HECFcal = {"HECA":[32,-TMath.Pi(),TMath.Pi()],"FCalA":[16, -3.2, 3.2]} #same for all layers
for par in ["HEC","FCal"]:
    bins=HVeta_HECFcal[par+"A"]
    HVeta_HECFcal[par+"C"]=[bins[0],-1*(bins[2]+0.01),-1*(bins[1]+0.01)]
    HVphi_HECFcal[par+"C"]=HVphi_HECFcal[par+"A"]


#Creation of the dictionnary
Cell_Variables={}
NBins={}
Partitions=["EMB","EMEC","HEC","FCal"]
Sides=["A","C"]
Variables=["etaRange","phiRange","etaNbin", "phiNbin","etasize","etaCellSize","etaMin"]
Layers=["0","1","2","3"]
for Variable in Variables :
    Cell_Variables[Variable]={}
    for Partition in Partitions :
        Cell_Variables[Variable][Partition]={}
        for Side in Sides :
            Cell_Variables[Variable][Partition][Side]={}
            for Layer in Layers:
                Cell_Variables[Variable][Partition][Side][Layer]=list()

#Number of Bins in phi
Cell_Variables["phiNbin"]["EMB"]["A"]= {"0":64,"1":256,"2":256,"3":256}
Cell_Variables["phiNbin"]["EMB"]["C"]= Cell_Variables["phiNbin"]["EMB"]["A"]

Cell_Variables["phiNbin"]["EMEC"]["A"]= {"0":64,"1":64,"2":256,"3":256}
Cell_Variables["phiNbin"]["EMEC"]["C"]= Cell_Variables["phiNbin"]["EMEC"]["A"]

Cell_Variables["phiNbin"]["HEC"]["A"]= {"0":64,"1":64,"2":64,"3":64}
Cell_Variables["phiNbin"]["HEC"]["C"]= Cell_Variables["phiNbin"]["HEC"]["A"]


#Number of bins in eta in each range of cells (defining only for A side since C side is symmetric)
#To be read as follow :
#The EMB Layer 1 have 3 types of cell sizes 
#The first bin start at eta=0.0, and have a size of 0.025/4
#The bins [2;446], have a size of 0.025/8
#The bins [447;450, have a size of 0.025

Cell_Variables["etaNbin"]["EMB"]["A"]=    {"0":[0,61],"1":[0,1,446,450],"2":[0,56,57],"3":[0,27]}
Cell_Variables["etaMin" ]["EMB"]["A"]=    {"0":0.0,"1":0.0,"2":0.0,"3":0.0}
Cell_Variables["etaCellSize"]["EMB"]["A"]={"0":[0.025],"1":[0.025/4,0.025/8,0.025],"2":[0.025,0.05],"3":[0.05]}

Cell_Variables["etaNbin"]["EMEC"]["A"]=    {"0":[0,12],"1":[0,1,3,99,147,211,215,223],"2":[0,1,43,51],"3":[0,20]}
Cell_Variables["etaMin" ]["EMEC"]["A"]=    {"0":1.5,"1":1.375,"2":1.375,"3":1.5}
Cell_Variables["etaCellSize"]["EMEC"]["A"]={"0":[0.025],"1":[0.05,0.025,0.025/8,0.025/6,0.025/4,0.025,0.1],"2":[0.05,0.025,0.1],"3":[0.05]}

Cell_Variables["etaNbin"]["HEC"]["A"]=    {"0":[0,10,14],"1":[0,10,13],"2":[0,9,12],"3":[0,8,12]}
Cell_Variables["etaMin" ]["HEC"]["A"]=    {"0":1.5,"1":1.5,"2":1.6,"3":1.7}
Cell_Variables["etaCellSize"]["HEC"]["A"]={"0":[0.1,0.2],"1":[0.1,0.2],"2":[0.1,0.2],"3":[0.1,0.2]}


###################
#Filling Phi ranges
for Layer in Layers :
    for Part in Partitions:
        if Part=="FCal": #and Layer=="3": we'll leave fcal empty for now
            continue

        Cell_Variables["phiRange"][Part]["A"][Layer]=[-TMath.Pi()+ x*2*TMath.Pi()/Cell_Variables["phiNbin"][Part]["A"][Layer] for x in xrange(Cell_Variables["phiNbin"][Part]["A"][Layer]+1)]
        Cell_Variables["phiRange"][Part]["C"]=Cell_Variables["phiRange"][Part]["A"]

###################
#Filling Eta range

for Part in Partitions:
    for Lay in Layers:
        
        Ranges=Cell_Variables["etaNbin"][Part]["A"][Lay]
        Sizes=Cell_Variables["etaCellSize"][Part]["A"][Lay]
        if not len(Ranges)-len(Sizes)==1 :
            print("Bad list of ranges and size please check")
            exit(1)
        etamin=Cell_Variables["etaMin"][Part]["A"][Lay]
        currange=[etamin]
        for k in xrange(len(Ranges)-1) :
            currange+=[round(currange[-1] + x * Sizes[k],5) for x in xrange(1,Ranges[k+1]-Ranges[k]+1)]

        Cell_Variables["etaRange"][Part]["A"][Lay]=currange
        #The C side is just the symmeteric of the A side
        Cell_Variables["etaRange"][Part]["C"][Lay] =map(lambda x: x*-1,Cell_Variables["etaRange"][Part]["A"][Lay])[::-1]

print len(currange)


if __name__ =="__main__":

    print("########## N bins in Phi")
    print(Cell_Variables["phiNbin"])
    print("########## Phi range")
    print(Cell_Variables["phiRange"])

    print("########## N bins in eta")
    print(Cell_Variables["etaNbin"])
    print("########## Eta range")
    print(Cell_Variables["etaRange"])

    nbinA=len(Cell_Variables["etaRange"]["EMB"]["A"]["1"])
    nbinC=len(Cell_Variables["etaRange"]["EMB"]["C"]["1"])
    print nbinA,nbinC
    for bin in  xrange(nbinA):
        print bin
        print Cell_Variables["etaRange"]["EMB"]["A"]["1"][bin], Cell_Variables["etaRange"]["EMB"]["C"]["1"][bin],Cell_Variables["etaRange"]["EMB"]["A"]["1"][bin]+Cell_Variables["etaRange"]["EMB"]["C"]["1"][nbinA-bin-1]
    

