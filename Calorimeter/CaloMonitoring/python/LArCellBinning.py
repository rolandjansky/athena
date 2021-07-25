#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
phiSteps={}
M_PI=TMath.Pi()

lArCellBinningScheme.phiNbin["EMB"]= {"P":[64],"1":[64],"2":[256],"3":[256]}
phiSteps["EMB"]= {"P":[-M_PI,M_PI],"1":[-M_PI,M_PI],"2":[-M_PI,M_PI],"3":[-M_PI,M_PI]}
lArCellBinningScheme.phiNbin["EMEC"]= {"P":[64],"1":[64],"2":[256],"3":[256]}
phiSteps["EMEC"]= {"P":[-M_PI,M_PI],"1":[-M_PI,M_PI],"2":[-M_PI,M_PI],"3":[-M_PI,M_PI]}
lArCellBinningScheme.phiNbin["HEC"]= {"0":[64],"1":[64],"2":[64],"3":[64]}
phiSteps["HEC"]= {"0":[-M_PI,M_PI],"1":[-M_PI,M_PI],"2":[-M_PI,M_PI],"3":[-M_PI,M_PI]}
lArCellBinningScheme.phiNbin["FCAL"]= {"1":[2,1,1,1,1,1,1,2,6,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,3,1,1,2,1,2,1,2,4,3,1,1,2,1,1,3,1,2,2,1,2,3,1,1,1,1,1,1,2,6,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,3,1,1,2,1,2,1,2,4,3,1,1,2,1,1,3,1,2,2,1,2,1],
                                       "2":[1,1,1,1,1,1,1,1,1,8,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,8,1,3,1,1,1,1],
                                       "3":[10,1,1,4,2,1,2,2,1,1,2,10,1,1,4,2,1,2,2,1,1,2]}
phiSteps["FCAL"]= {"1":[-M_PI,(-250.0/256)*M_PI,(-248.0/256)*M_PI,(-243.0/256)*M_PI,(-240.0/256)*M_PI,(-234.0/256)*M_PI,
                        (-230.0/256)*M_PI,(-225.0/256)*M_PI,(-219.0/256)*M_PI,(-195.0/256)*M_PI,(-190.0/256)*M_PI,(-186.0/256)*M_PI,
                        (-183.0/256)*M_PI,(-178.0/256)*M_PI,(-175.0/256)*M_PI,(-173.0/256)*M_PI,(-167.0/256)*M_PI,(-161.0/256)*M_PI,
                        (-159.0/256)*M_PI,(-156.0/256)*M_PI,(-151.0/256)*M_PI,(-148.0/256)*M_PI,(-146.0/256)*M_PI,(-141.0/256)*M_PI,
                        (-139.0/256)*M_PI,(-135.0/256)*M_PI,(-132.0/256)*M_PI,(-120.0/256)*M_PI,(-117.0/256)*M_PI,(-114.0/256)*M_PI,
                        (-110.0/256)*M_PI,(-105.0/256)*M_PI,(-97.0/256)*M_PI,(-94.0/256)*M_PI,(-86.0/256)*M_PI,(-78.0/256)*M_PI,
                        (-63.0/256)*M_PI,(-60.0/256)*M_PI,(-56.0/256)*M_PI,(-46.0/256)*M_PI,(-44.0/256)*M_PI,(-41.0/256)*M_PI,
                        (-32.0/256)*M_PI,(-28.0/256)*M_PI,(-22.0/256)*M_PI,(-10.0/256)*M_PI,(-7.0/256)*M_PI,(-3.0/256)*M_PI,
                        (6.0/256)*M_PI,(8.0/256)*M_PI,(13.0/256)*M_PI,(16.0/256)*M_PI,(22.0/256)*M_PI,(26.0/256)*M_PI,(31.0/256)*M_PI,
                        (37.0/256)*M_PI,(61.0/256)*M_PI,(66.0/256)*M_PI,(70.0/256)*M_PI,(73.0/256)*M_PI,(78.0/256)*M_PI,(81.0/256)*M_PI,
                        (83.0/256)*M_PI,(89.0/256)*M_PI,(95.0/256)*M_PI,(97.0/256)*M_PI,(100.0/256)*M_PI,(105.0/256)*M_PI,
                        (108.0/256)*M_PI,(110.0/256)*M_PI,(115.0/256)*M_PI,(117.0/256)*M_PI,(121.0/256)*M_PI,(124.0/256)*M_PI,
                        (136.0/256)*M_PI,(139.0/256)*M_PI,(142.0/256)*M_PI,(146.0/256)*M_PI,(151.0/256)*M_PI,(159.0/256)*M_PI,
                        (162.0/256)*M_PI,(170.0/256)*M_PI,(178.0/256)*M_PI,(193.0/256)*M_PI,(196.0/256)*M_PI,(200.0/256)*M_PI,
                        (210.0/256)*M_PI,(212.0/256)*M_PI,(215.0/256)*M_PI,(224.0/256)*M_PI,(228.0/256)*M_PI,(234.0/256)*M_PI,
                        (246.0/256)*M_PI,(249.0/256)*M_PI,(253.0/256)*M_PI,M_PI],
                   "2":[-M_PI,(-62.0/64)*M_PI,(-59.0/64)*M_PI,(-55.0/64)*M_PI,(-52.0/64)*M_PI,(-50.0/64)*M_PI,(-48.0/64)*M_PI,
                        (-45.0/64)*M_PI,(-41.0/64)*M_PI,(-38.0/64)*M_PI,(-22.0/64)*M_PI,(-18.0/64)*M_PI,(-12.0/64)*M_PI,
                        (-8.0/64)*M_PI,(-5.0/64)*M_PI,(-1.0/64)*M_PI,0,(2.0/64)*M_PI,(5.0/64)*M_PI,(9.0/64)*M_PI,(12.0/64)*M_PI,
                        (14.0/64)*M_PI,(16.0/64)*M_PI,(19.0/64)*M_PI,(23.0/64)*M_PI,(26.0/64)*M_PI,(42.0/64)*M_PI,(46.0/64)*M_PI,
                        (52.0/64)*M_PI,(56.0/64)*M_PI,(59.0/64)*M_PI,(63.0/64)*M_PI,M_PI],
                   "3":[-M_PI,(-40.0/64)*M_PI,(-39.0/64)*M_PI,(-35.0/64)*M_PI,(-29.0/64)*M_PI,(-24.0/64)*M_PI,(-22.0/64)*M_PI,
                        (-16.0/64)*M_PI,(-11.0/64)*M_PI,(-9.0/64)*M_PI,(-6.0/64)*M_PI,0,(24.0/64)*M_PI,(25.0/64)*M_PI,
                        (29.0/64)*M_PI,(35.0/64)*M_PI,(40.0/64)*M_PI,(42.0/64)*M_PI,(48.0/64)*M_PI,(53.0/64)*M_PI,(55.0/64)*M_PI,
                        (58.0/64)*M_PI,M_PI]}


#Number of bins in eta in each range of cells (defining only for A side since C side is symmetric)
#To be read as follow :
#The EMB Layer 1 have 3 types of cell sizes
#The first bin start at eta=0.0, and have a size of 0.025/4
#The bins [2;446], have a size of 0.025/8
#The bins [447;450, have a size of 0.025

lArCellBinningScheme.etaNbin = {}
lArCellBinningScheme.etaMin = {}
lArCellBinningScheme.etaCellSize = {}

lArCellBinningScheme.etaNbin["EMB"]=    {"P":[0,61],"1":[0,1,446,450],"2":[0,56,59],"3":[0, 27, 28]}
lArCellBinningScheme.etaMin["EMB"]=    {"P":0.0,"1":0.0,"2":0.0,"3":0.0}
lArCellBinningScheme.etaCellSize["EMB"]={"P":[0.025],"1":[0.025/4,0.025/8,0.025],"2":[0.025,0.05],"3":[0.05, 0.125]}

lArCellBinningScheme.etaNbin["EMEC"]=    {"P":[0,12],"1":[0,1,3,99,147,211,215,223],"2":[0,1,43,51],"3":[0,20]}
lArCellBinningScheme.etaMin["EMEC"]=    {"P":1.5,"1":1.375,"2":1.375,"3":1.5}
lArCellBinningScheme.etaCellSize["EMEC"]={"P":[0.025],"1":[0.05,0.025,0.025/8,0.025/6,0.025/4,0.025,0.1],"2":[0.05,0.025,0.1],"3":[0.05]}

lArCellBinningScheme.etaNbin["HEC"]=    {"0":[0,10,14],"1":[0,10,13],"2":[0,9,12],"3":[0,8,12]}
lArCellBinningScheme.etaMin["HEC"]=    {"0":1.5,"1":1.5,"2":1.6,"3":1.7}
lArCellBinningScheme.etaCellSize["HEC"]={"0":[0.1,0.2],"1":[0.1,0.2],"2":[0.1,0.2],"3":[0.1,0.2]}

lArCellBinningScheme.etaNbin["FCAL"]=    {"1":[0, 3, 9, 13, 15],"2":[0, 1, 2, 3, 5, 7, 8, 9, 12, 13, 17, 19, 20, 21, 22, 23],"3":[0,2,4,5,6,8]}
lArCellBinningScheme.etaMin["FCAL"]=    {"1":3.08,"2":3.192,"3":3.27}
lArCellBinningScheme.etaCellSize["FCAL"] ={"1":[0.04, 0.1, 0.2, 0.11],"2":[0.007, 0.007, 0.014, 0.02, 0.04, 0.03, 0.02, 0.043, 0.05, 0.066, 0.132, 0.265, 0.135, 0.15, 0.16],"3":[0.08, 0.135, 0.21, 0.42, 0.21]}





###################
#Filling Phi ranges
lArCellBinningScheme.phiRange={}
for Part in lArCellBinningScheme.PartitionLayers:
      for Layer in lArCellBinningScheme.PartitionLayers[Part]:
#            if Part=="FCAL": TMath.Pi(
 #                 lArCellBinningScheme.phiRange[Part+Layer+"A"]=[-0.5+x for x in range(lArCellBinningScheme.phiNbin[Part][Layer]+1)]
  #          else: #all other partitions
            lArCellBinningScheme.phiRange[Part+Layer+"A"]=[]
            for istep in range(1,len(phiSteps[Part][Layer])):
                  lArCellBinningScheme.phiRange[Part+Layer+"A"].extend([phiSteps[Part][Layer][istep-1]+ x*(phiSteps[Part][Layer][istep]-phiSteps[Part][Layer][istep-1])/lArCellBinningScheme.phiNbin[Part][Layer][istep-1] for x in range(lArCellBinningScheme.phiNbin[Part][Layer][istep-1])])
                  pass
            lArCellBinningScheme.phiRange[Part+Layer+"A"].append(phiSteps[Part][Layer][-1])
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
#            if Part=="FCAL":
 #                 lArCellBinningScheme.etaRange[Part+Lay+"C"] = lArCellBinningScheme.etaRange[Part+Lay+"A"]
  #          else: #all other partitions
            lArCellBinningScheme.etaRange[Part+Lay+"C"] =list(map(lambda x: x*-1,lArCellBinningScheme.etaRange[Part+Lay+"A"]))[::-1]
   #               pass

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

lArCellBinningScheme.larCellSummary={"xbins":[7, 1., 8.],
                                     "xlabels":["Total Events", "RNDM Trigger", "Calo Trigger", "MinBias Trigger", "MET Trigger", "Misc Trigger", "Events Sele\
cted for Noise Plots"]}



if __name__ =="__main__":

    print (lArCellBinningScheme.PartLayerNames)
    print (lArCellBinningScheme.etaRange)
    for k in lArCellBinningScheme.etaRange: 
          print (k)
