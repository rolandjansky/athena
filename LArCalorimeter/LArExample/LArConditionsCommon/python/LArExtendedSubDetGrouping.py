# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

class LArExtendedSubDetGrouping:
    def __init__(self):
        self._EMBC=()
        self._EMBA=()
        self._EMBCPS=()
        self._EMBAPS=()
        self._EMECC=()
        self._EMECA=()
        self._EMECCPS=()
        self._EMECAPS=()
        self._HECC=()
        self._HECA=()
        self._FCALC=()
        self._FCALA=()
        for gain in range(0,3):
           self._EMBC+=(3+gain*12,)
           self._EMBA+=(9+gain*12,)

           self._EMBCPS+=(4+gain*12,)
           self._EMBAPS+=(10+gain*12,)

           self._EMECC+=(5+gain*12,) 
           self._EMECA+=(11+gain*12,)

           self._EMECCPS+=(6+gain*12,)
           self._EMECAPS+=(12+gain*12,)

           self._HECC+=(7+gain*12,)
           self._HECA+=(13+gain*12,)

           self._FCALC+=(8+gain*12,)
           self._FCALA+=(14+gain*12,)

        self._emptyC=()
        self._emptyA=()
        self._empty=self._emptyC + self._emptyA
        
        self._withCorr=True

        #Build dictionary:
        self._partitions=dict()
        self._partitions['EMBAPS'] = self._EMBAPS
        self._partitions['EMBCPS'] = self._EMBCPS
        self._partitions['EMECAPS'] = self._EMECAPS
        self._partitions['EMECCPS'] = self._EMECCPS
        self._partitions['EMBA'] = self._EMBA
        self._partitions['EMBC'] = self._EMBC
        self._partitions['EMECA']= self._EMECA
        self._partitions['EMECC'] = self._EMECC
        self._partitions['HECA'] = self._HECA
        self._partitions['HECC'] = self._HECC
        self._partitions['FCALA'] = self._FCALA
        self._partitions['FCALC'] = self._FCALC
        self._partitions['EMPTYA'] = self._emptyA
        self._partitions['EMPTYC'] = self._emptyC

        #Correction Channels:
        self._corr=dict()
        self._corr['EMBAPS'] = 1001
        self._corr['EMBCPS'] = 1002
        self._corr['EMECAPS'] = 1003
        self._corr['EMECCPS'] = 1004
        self._corr['EMBA']  = 1005
        self._corr['EMBC']  = 1006
        self._corr['EMECA'] = 1007
        self._corr['EMECC'] = 1008
        self._corr['HECA']  = 1009
        self._corr['HECC']  = 1010
        self._corr['FCALA'] = 1011
        self._corr['FCALC'] = 1012

        #Lookup-dict indexed by channel (partition is the payload)
        self._revLookup=dict()
        for (p, chs) in self._partitions.items():
            for c in chs:
                self._revLookup[c]=p


    def setWithCorr(self,val):
        self._withCorr=val

    def Print(self):
        print (self._EMBC)
        print (self._EMBA)


    def getChannelList(self,partitions,gains=[0]):
        chans=list()
        for g in gains:
            if g<0 or g>2:
                print ("ERROR: Unkown gain",g)
                return None

        extPart=list()
        for partition in partitions:
            p=partition.upper()
            if (p=="ECC"):
                extPart+=["EMECCPS","EMECC","HECC","FCALC"]
            elif (p=="ECA"):
                extPart+=["EMECAPS","EMECA","HECA","FCALA"]
            elif (p=="EC"):
                extPart+=["EMECAPS","EMECA","HECA","FCALA",
                          "EMECCPS","EMECC","HECC","FCALC"]
            elif (p=="EMB"):
                extPart+=["EMBA","EMBAPS","EMBC","EMBCPS"]
            else:
                extPart+=[p]
                
        for p in extPart:
            if p in self._partitions:
                for g in gains:
                    print (self._partitions[p])
                    chans+=[self._partitions[p][g]]
                if (self._withCorr):
                    for g in gains:
                        chans+=[self._corr[p]+g*12]
            else:
                print ("ERROR: Unkown partition '",partition,"'")

        return chans

    def makeRange(self,chans):
        chans.sort()
        retVal=""
        if (len(chans)==0):
            return retVal
        retVal=str(chans[0])
        c1=chans[0]
        series=False
        for c2 in chans[1:]:
            if c1 == c2:
                print ("Duplicated entry",c2)
                continue
            if c2-1 == c1 or c2-1 in self._empty:
                series=True
            else:
                if series:
                    retVal+=":"+str(c1)+","+str(c2)
                    series=False
                else:
                    retVal+=","+str(c2)
#            print ("c1=",c1,"c2=",c2,"sep=",sep)
            c1=c2
        if series: retVal+=":"+str(c1)
        return retVal



    def getChannelSelection(self,partitions,gains):
        return self.makeRange(self.getChannelList(partitions,gains))

    def getChannelSelectionAllGains(self,partitions):
        chans=list()
        for p in partitions:
            chans+=self.getChannelList([p],[0,1,2])
        return self.makeRange(chans)


    def getGain(self,c):
        if c<2:
            return (0,None)
        if c<15:
            return (0,c)
        if c<27:
            return (1,c-12)
        if c<39:
            return (2,c-24)
        if c<1001:
            return (0,None)
        if c<1013:
            return (0,c)
        if c<1025:
            return (1,c-12)
        if c<1037:
            return (2,c-24)
        return (0,None)


    def channelsPerPartition(self,chans,show=True):
        class counterElem:
            def __init__(self,l,n):
                self.size=l
                self.name=n
                self.counts=[0,0,0]
            def inc(self,g):
                self.counts[g]=1+self.counts[g]

            def show(self):
                print( "%7s: " % self.name,end="")
                print( "HIGH:%2i/%2i" % (self.counts[0],self.size),)
                if self.counts[0] != self.size:
                    print( "*  ",end="")
                else:
                    print( "   ",end="")
                print( "MED:%2i/%2i" % (self.counts[1],self.size),)
                if self.counts[1] != self.size:
                    print( "*  ",end="")
                else:
                    print( "   ",end="")
                print( "LOW:%2i/%2i" % (self.counts[2],self.size),)
                if self.counts[2] != self.size:
                    print( "*  ")
                else:
                    print( "   ")
            
        partCounter=dict()
        for (p, chs) in self._partitions.items():
            partCounter[p]=counterElem(len(chs),p)
        
        for c in chans:
            (gain,cs)=self.getGain(c)
            if cs is None:
                print( "ERROR: Unkown channel",c )
            else:    
                if (c<39):
                    #p=self._revLookup[cs]
                    p=self._revLookup[c]
                    partCounter[p].inc(gain)

        return partCounter


    #Create COOL-channel selection string for partitions/gain strings as use by by the
    #Automatic Processing of LAr Elec Calib runs

    
_lArExtendedSubDetGrouping = LArExtendedSubDetGrouping()
def channelSelectionFromString(Partition,PartitionType, PartitionTypeGeneric, GainList):
    print('LArExtendedSubDetGrouping')
    print('Partition: ',Partition)
    print('PGen: ',PartitionTypeGeneric)


       

    ## defined gain :
        
    ## HIGH
    if ( GainList[0]=="HIGH" ):
        gain= [0]
        
    ## MEDIUM    
    elif  ( GainList[0]=="MEDIUM" ) :
        gain= [1]

    ## LOW    
    elif  ( GainList[0]=="LOW" ) :
        gain= [2]
        
    else:
        gain=[0,1,2]

    ## defined partition

    ## EMB  A+C    
    if ( Partition=='EB-EMBA' and ( PartitionTypeGeneric!='EMBPS' ) ) :
        partition =['EMBA']
        print('EMBA partition')
    elif ( Partition=='EB-EMBC' and ( PartitionTypeGeneric!='EMBPS' ) ) :
        partition =['EMBC']
        print('EMBC partition')
    elif ( Partition=='EB-EMB' and ( PartitionTypeGeneric!='EMBPS' ) ) :
        partition =['EMBA','EMBC']
        print('EMB partition')
        
    ## EMBPS  A+C     
    if ( Partition=='EB-EMBA' and ( PartitionTypeGeneric=='EMBPS' ) ) :
        partition =['EMBAPS']
        print('EMBAPS partition')
    elif ( Partition=='EB-EMBC' and ( PartitionTypeGeneric=='EMBPS' ) ) :
        partition =['EMBCPS']
        print('EMBCPS partition')
    elif ( Partition=='EB-PS' and ( PartitionTypeGeneric=='EMBPS' ) ) :
        partition =['EMBAPS','EMBCPS']
        print('EMBAPS and EMBCPS partition')
        
    if ( Partition=='EB-EM' ) :
        partition =['EMBAPS','EMBCPS','EMBA','EMBC','EMECAPS','EMECA','EMECCPS','EMECC']
        print('EM partition')
        
    ## EMEC + PS A  44    
    if ( Partition=='EB-EMECA' ) :
        partition =['EMECAPS','EMECA']
    elif ( Partition=='EB-EMECC' ) :
        partition =['EMECCPS','EMECC']
    elif ( Partition=='EB-EMEC' ) :
        partition =['EMECAPS','EMECA','EMECCPS','EMECC']
        ## HEC A + C   8 (4+4)  
    elif ( PartitionType=='HEC'):
        partition =['HECA','HECC']
        
        ## FCAL A + C   2 (1+1)
    elif ( PartitionType=='FCAL') :   
        partition =['FCALA','FCALC']
        
    elif ( Partition=='HECFCALC') :   
        partition =['HECC','FCALC']

    selection = _lArExtendedSubDetGrouping.getChannelSelection(partition,gain)
    ChannelSelection='<channelSelection>'+selection+'</channelSelection>'
    print(ChannelSelection)
    print("CoolChannel Selection for ", partition, " and ",gain, " gain. ")
    return ChannelSelection
