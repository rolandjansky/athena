#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#


from ROOT import TMath

class LArCellBinningScheme(object):
      __slots__ = ('PartitionLayers','Sides','PartLayerNames','PartitionNames','etaRange','phiRange','phiNbin','etaNbin','etaMin','etaCellSize','energyscale','timescale','logEnergyBins','cosmicEneBinning','defaultEnergyBins','larCellSummary')

lArCellBinningScheme = LArCellBinningScheme()


#Creation of the dictionnary

LayersEM=["P","1","2","3"]
LayersHEC=["0","1","2","3"]
LayersFCAL=["1","2","3"]
lArCellBinningScheme.PartitionLayers={"EMB":LayersEM,"EMEC":LayersEM,"HEC":LayersHEC,"FCAL":LayersFCAL}
lArCellBinningScheme.Sides=["A","C"]

lArCellBinningScheme.PartLayerNames=[]
lArCellBinningScheme.PartitionNames=[]

for Partition in lArCellBinningScheme.PartitionLayers :
      for Side in lArCellBinningScheme.Sides :
            lArCellBinningScheme.PartitionNames.append(Partition+Side)
            pass
      for Layer in lArCellBinningScheme.PartitionLayers[Partition]:
            for Side in lArCellBinningScheme.Sides :
                  lArCellBinningScheme.PartLayerNames.append(Partition+Layer+Side)



#Number of Bins in phi
lArCellBinningScheme.phiNbin={}
lArCellBinningScheme.phiNbin["EMB"]= {"P":64,"1":256,"2":256,"3":256}
lArCellBinningScheme.phiNbin["EMEC"]= {"P":64,"1":64,"2":256,"3":256}
lArCellBinningScheme.phiNbin["HEC"]= {"0":64,"1":64,"2":64,"3":64}
lArCellBinningScheme.phiNbin["FCAL"]= {"1":16,"2":16,"3":16}


#Number of bins in eta in each range of cells (defining only for A side since C side is symmetric)
#To be read as follow :
#The EMB Layer 1 have 3 types of cell sizes
#The first bin start at eta=0.0, and have a size of 0.025/4
#The bins [2;446], have a size of 0.025/8
#The bins [447;450, have a size of 0.025

lArCellBinningScheme.etaNbin = {}
lArCellBinningScheme.etaMin = {}
lArCellBinningScheme.etaCellSize = {}

lArCellBinningScheme.etaNbin["EMB"]=    {"P":[0,61],"1":[0,1,446,450],"2":[0,56,57],"3":[0,27]}
lArCellBinningScheme.etaMin["EMB"]=    {"P":0.0,"1":0.0,"2":0.0,"3":0.0}
lArCellBinningScheme.etaCellSize["EMB"]={"P":[0.025],"1":[0.025/4,0.025/8,0.025],"2":[0.025,0.05],"3":[0.05]}

lArCellBinningScheme.etaNbin["EMEC"]=    {"P":[0,12],"1":[0,1,3,99,147,211,215,223],"2":[0,1,43,51],"3":[0,20]}
lArCellBinningScheme.etaMin["EMEC"]=    {"P":1.5,"1":1.375,"2":1.375,"3":1.5}
lArCellBinningScheme.etaCellSize["EMEC"]={"P":[0.025],"1":[0.05,0.025,0.025/8,0.025/6,0.025/4,0.025,0.1],"2":[0.05,0.025,0.1],"3":[0.05]}

lArCellBinningScheme.etaNbin["HEC"]=    {"0":[0,10,14],"1":[0,10,13],"2":[0,9,12],"3":[0,8,12]}
lArCellBinningScheme.etaMin["HEC"]=    {"0":1.5,"1":1.5,"2":1.6,"3":1.7}
lArCellBinningScheme.etaCellSize["HEC"]={"0":[0.1,0.2],"1":[0.1,0.2],"2":[0.1,0.2],"3":[0.1,0.2]}

lArCellBinningScheme.etaNbin["FCAL"]=    {"1":[0,63],"2":[0,32],"3":[0,16]}
lArCellBinningScheme.etaMin["FCAL"]=    {"1":-0.5,"2":-0.5,"3":-0.5}
lArCellBinningScheme.etaCellSize["FCAL"] ={"1":[1],"2":[1],"3":[1]}



###################
#Filling Phi ranges
lArCellBinningScheme.phiRange={}
for Part in lArCellBinningScheme.PartitionLayers:
      for Layer in lArCellBinningScheme.PartitionLayers[Part]:
            if Part=="FCAL": 
                  lArCellBinningScheme.phiRange[Part+Layer+"A"]=[-0.5+x for x in range(lArCellBinningScheme.phiNbin[Part][Layer]+1)]
            else: #all other partitions
                  lArCellBinningScheme.phiRange[Part+Layer+"A"]=[-TMath.Pi()+ x*2*TMath.Pi()/lArCellBinningScheme.phiNbin[Part][Layer] for x in range(lArCellBinningScheme.phiNbin[Part][Layer]+1)]
                  pass
            lArCellBinningScheme.phiRange[Part+Layer+"C"]=lArCellBinningScheme.phiRange[Part+Layer+"A"]
                  
            pass #layer loop  
      pass #partition loop
            
###################
#Filling Eta range
lArCellBinningScheme.etaRange = {}
for Part in lArCellBinningScheme.PartitionLayers:
      for Lay in lArCellBinningScheme.PartitionLayers[Part]:
            Ranges=lArCellBinningScheme.etaNbin[Part][Lay]
            Sizes=lArCellBinningScheme.etaCellSize[Part][Lay]
            if not len(Ranges)-len(Sizes)==1 :
                  print("Bad list of ranges and size please check")
                  continue
                  #exit(1)
            etamin=lArCellBinningScheme.etaMin[Part][Lay]
            currange=[etamin]
            for k in range(len(Ranges)-1) :
                  currange+=[round(currange[-1] + x * Sizes[k],5) for x in range(1,Ranges[k+1]-Ranges[k]+1)]
                  pass
            lArCellBinningScheme.etaRange[Part+Lay+"A"]=currange
            #The C side is just the symmeteric of the A side
            if Part=="FCAL":
                  lArCellBinningScheme.etaRange[Part+Lay+"C"] = lArCellBinningScheme.etaRange[Part+Lay+"A"]
            else: #all other partitions
                  lArCellBinningScheme.etaRange[Part+Lay+"C"] =list(map(lambda x: x*-1,lArCellBinningScheme.etaRange[Part+Lay+"A"]))[::-1]
                  pass

#energy and time
lArCellBinningScheme.timescale = [-200,-195,-190,-185,-180,-175,-170,-165,-160,-155,-150,-145,-140,-135,-130,-125,-120,-115,-110,-105,-100,-95,-90,-85,-80,-75,-70,-65,-60,-55,-50,-45,-40,-35,-30,-25,-20,-15,-10,-8,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,8,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200]
lArCellBinningScheme.energyscale = [250,500,1000,2500,5000,10000,25000,50000,100000,250000,500000,1000000]

lArCellBinningScheme.logEnergyBins=[None]*55
lArCellBinningScheme.logEnergyBins[0]=10
step = pow(10,0.1)
for i in range(1,len(lArCellBinningScheme.logEnergyBins)):
      lArCellBinningScheme.logEnergyBins[i] = step * lArCellBinningScheme.logEnergyBins[i-1]
      pass

xbins=201
lArCellBinningScheme.cosmicEneBinning={}
for side in lArCellBinningScheme.Sides:
      for Lay in lArCellBinningScheme.PartitionLayers["EMB"]:
            lArCellBinningScheme.cosmicEneBinning["EMB"+Lay+side]=[xbins,-318.5,1088.5]
            pass
      for Lay in lArCellBinningScheme.PartitionLayers["EMEC"]:
            lArCellBinningScheme.cosmicEneBinning["EMEC"+Lay+side]=[xbins,-591.5,2021.5]
            pass
      for Lay in lArCellBinningScheme.PartitionLayers["HEC"]:
            lArCellBinningScheme.cosmicEneBinning["HEC"+Lay+side]=[xbins,-2047.5,6997.5]
            pass
      for Lay in lArCellBinningScheme.PartitionLayers["FCAL"]:
            lArCellBinningScheme.cosmicEneBinning["FCAL"+Lay+side]=[xbins,-1865.5,6375.5]
            pass
      pass

lArCellBinningScheme.defaultEnergyBins=[401,-1912.5,28162.5]

lArCellBinningScheme.larCellSummary={"xbins":[7, -0.5, 6.5],
                                     "xlabels":["Total Events", "RNDM Trigger", "Calo Trigger", "MinBias Trigger", "MET Trigger", "Misc Trigger", "Events Sele\
cted for Noise Plots"]}



if __name__ =="__main__":

    print (lArCellBinningScheme.PartLayerNames)
    print (lArCellBinningScheme.etaRange)
    for k in lArCellBinningScheme.etaRange: 
          print (k)
