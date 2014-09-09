# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# DetStatusLib.py
# python functions/classes to help management of detector status information
# Richard Hawkings, 5/2/07

def folderName(runLumi=True):
    if (runLumi):
        return '/GLOBAL/DETSTATUS/LBSUMM'
    else:
        return '/GLOBAL/DETSTATUS/TISUMM'

def colour(code):
    if (code==-1):
        return "BLACK"
    rcode=code % 4
    if (rcode==1):
        return "RED"
    elif (rcode==2):
        return "YELLOW"
    elif (rcode==3):
        return "GREEN"
    return "UNKNOWN"

def colourVal(sval):
    "Translate traffic light string (numerical literal or letter) to number"
    status=0
    try:
        uplight=sval.upper()
        if uplight in ["R","RED"]:
            status=1
        elif uplight in ["Y","YELLOW"]:
            status=2
        elif uplight in ["G","GREEN"]:
            status=3
        elif uplight in ["U","UNKNOWN"]:
            status=0
        elif uplight in ["B","BLACK"]:
            status=-1
        else:
            status=int(sval)
    except Exception,e:
        status=None
    return status


class DetStatusNames:
    def __init__(self):
        # current list of status flags
        # has to match definition in DetStatusSvc.cxx
        
        self.namedict={'PIXB':101,'PIX0':102,'PIXEA':104,'PIXEC':105,
                       'SCTB':111,'SCTEA':114,'SCTEC':115,
                       'TRTB':121,'TRTEA':124,'TRTEC':125,'TRTTR':126,
                       'IDGL':130,
                       'IDAL':140,
                       'IDBS':150,
                       'IDPF':160,'IDVX':161,
                       'IDBCM':170,
                       'EMBA':202,'EMBC':203,'EMECA':204,'EMECC':205,
                       'HECA':214,'HECC':215,'FCALA':224,'FCALC':225,
                       'TIGB':230,
                       'TILBA':232,'TILBC':233,'TIEBA':234,'TIEBC':235,
                       'MBTSA':244,'MBTSC':245,
                       'CALBA':251,'CALEA':254,'CALEC':255,
                       'MDTBA':302,'MDTBC':303,'MDTEA':304,'MDTEC':305,
                       'RPCBA':312,'RPCBC':313,
                       'TGCEA':324,'TGCEC':325,
                       'CSCEA':334,'CSCEC':335,
                       'LCD':350,'LCDA':353,'LCDC':354,
                       'ALFA':360,'ZDC':370,
                       'L1CAL':401,'L1MUB':402,'L1MUE':403,'L1CTP':404,
                       'TRCAL':411,
                       'TRDF':420,
                       'TRBJT':421,'TRBPH':422,'TRCOS':423,'TRELE':424,
                       'TRGAM':425,'TRJET':426,'TRMET':427,'TRMBI':428,
                       'TRMUO':429,'TRTAU':430,'TRIDT':431,
                       'LUMI':450,'LUMIONL':451,
                       'RUNCLT':460,
                       'RCOPS':461,
                       'ATLGL':480,'ATLSOL':481,'ATLTOR':482,
                       'EIDB':501,'EIDCR':502,'EIDE':503,
                       'PIDB':505,'PIDCR':506,'PIDE':507,
                       'EIDF':508,'EIDSOFT':509,
                       'MSTACO':510,'MMUIDCB':511,'MMUIDVX':512,
                       'MMUGIRL':513,'MMUBOY':514,'MMUIDSA':515,
                       'MMUTAG':516,'MMTIMO':517,'MCMUTAG':518,
                       'MCALLHR':519,
                       'JETB':521,'JETEA':524,'JETEC':525,
                       'JETFA':526,'JETFC':527,
                       'MET':530,'METCALO':531,'METMUON':532,
                       'BTGLIFE':541,'BTGSOFTE':544,'BTGSOFTM':545,
                       'TAUB':551,'TAUCR':552,'TAUE':553}
        
        # initialise reverse map from channel numbers to names
        self.numdict={}
        for (name,num) in self.namedict.items():
            self.numdict[num]=name
            
    def name(self,num):
        return self.numdict[num]

    def num(self,name):
        "Return the numeric channel identifier which exactly matches name"
        return self.namedict[name]

    def nums(self,name):
        "Return a list of all numeric channel identifiers which match name"
        result=[]
        for (iname,inum) in self.namedict.items():
            if (name==iname[:len(name)]): result+=[inum]
        return result

    def allNames(self):
        a=self.namedict.keys()
        a.sort()
        return a

    def allNums(self):
        a=self.namedict.values()
        a.sort()
        return a

class DetStatusReq:
    "Class to hold a list of detector status requirements"

    def __init__(self):
        "Initialise to empty set of requirements"
        # requirements are stored as dict[status_channelID]=minimum value
        self.req={}
        # access to name/number translation
        self.names=DetStatusNames()

    def setFromString(self,reqstring):
        """Set requirements from a space-separated string with flag req pairs
        e.g. 'SCTB 3 EMEC G' (numbers or letters for status)"""
        self.req={}
        namelist=self.names.allNames()
        tokens=str(reqstring).split()
        ix=0
        while (ix+1<len(tokens)):
            flagname=tokens[ix]
            val=colourVal(tokens[ix+1])
            if (val is not None):
                used=False
                # check if the name matches any flags
                for name in namelist:
                    if (flagname==name[0:len(flagname)]):
                        self.req[self.names.num(name)]=val
                        used=True
                if (not used):
                    print "Name %s does not match any status flag" % flagname
            else:
                print "Value %s does not define a status" % tokens[ix+1]
            ix+=2

    def getDict(self):
        "Return the dictionary holding the requirements (channel/value pairs)"
        return self.req

    def __str__(self):
        "Print representation of status requirements"
        result=""
        for (key,val) in self.req.items():
            result+="%s %i " % (self.names.name(key),val)
        return result
        
class StatusObj:
    "Transient representation of one detector status"
    def __init__(self,start,stop,code,deadfrac,thrust,nconfig=-1,nworking=-1,comment=''):
        self.start=start
        self.stop=stop
        self.code=code
        self.deadfrac=deadfrac
        self.thrust=thrust
        self.nconfig=nconfig
        self.nworking=nworking
        self.comment=comment

    def updateStart(self,newStart):
        self.start=newStart

    def updateStop(self,newStop):
        self.stop=newStop

class StatusList:
    "Transient representation of detector status list (code,deadfrac,thrust)"
    def __init__(self):
        "Initialise to empty sequence"
        # sequence stores the detector status in time order
        self._seq=[]

    def merge(self,mobj,override=False):
        "Merge the given StatusObj into the list, ANDing (default) or override"
        if (mobj.start>=mobj.stop): return
        # loop over the list, looking for places affected by the new obj
        ix=0
        oldstop=0
        while (ix<len(self._seq)):
            # first check for a gap between current and previous object
            # where a new object has to be inserted
            if (mobj.start<self._seq[ix].start and mobj.stop>oldstop):
                # calculate extent of object to insert
                nstart=max(oldstop,mobj.start)
                nstop=min(self._seq[ix].start,mobj.stop)
                # only insert if gap is of non-zero length
                if (nstop>nstart):
                    # insert the new object at this position
                    self._seq.insert(ix,StatusObj(nstart,nstop,mobj.code,mobj.deadfrac,mobj.thrust,mobj.nconfig,mobj.nworking,mobj.comment))
                    ix+=1
            # now check if object intersects current one
            if (mobj.start<self._seq[ix].stop and mobj.stop>self._seq[ix].start):
                # use mobj if override set or
                # (new status is <= existing and new is not undefined) or
                # old is undefined
                if (override or
                    (mobj.code<=self._seq[ix].code and mobj.code!=0) or
                    (self._seq[ix].code==0)):
                    # record existing status for splitting
                    ecode=self._seq[ix].code
                    edeadfrac=self._seq[ix].deadfrac
                    ethrust=self._seq[ix].thrust
                    enconfig=self._seq[ix].nconfig
                    enworking=self._seq[ix].nworking
                    ecomment=self._seq[ix].comment
                    estart=self._seq[ix].start
                    estop=self._seq[ix].stop
                    # position where status will be updated
                    iy=ix
                    if (mobj.start>estart):
                        # need to split at front
                        self._seq.insert(ix,StatusObj(estart,mobj.start,ecode,edeadfrac,ethrust,enconfig,enworking,ecomment))
                        ix+=1
                        iy+=1
                    if (mobj.stop<estop):
                        # need to split at end
                        self._seq.insert(ix+1,StatusObj(mobj.stop,estop,ecode,edeadfrac,ethrust,enconfig,enworking,ecomment))
                        ix+=1
                    # now update entry at iy with new status
                    self._seq[iy]=StatusObj(max(mobj.start,estart),min(mobj.stop,estop),mobj.code,mobj.deadfrac,mobj.thrust,mobj.nconfig,mobj.nworking,mobj.comment)
            # move on to next object
            oldstop=self._seq[ix].stop
            ix+=1
        # check if merged object extends over end of current list
        # may not have processed the whole list in the loop
        if (len(self._seq)>0): oldstop=self._seq[-1].stop
        if (mobj.stop>oldstop):
            nstart=max(oldstop,mobj.start)
            self._seq+=[StatusObj(nstart,mobj.stop,mobj.code,mobj.deadfrac,mobj.thrust,mobj.nconfig,mobj.nworking,mobj.comment)]

    def compress(self):
        "Compress StatusList removing redundant entries with IoVs which can be combined"
        ix=1
        while (ix<len(self._seq)):
            # check if payload of this entry is same as previous
            # and start/stop times match without any gap
            if (self._seq[ix].start==self._seq[ix-1].stop and 
                self._seq[ix].code==self._seq[ix-1].code and
                self._seq[ix].deadfrac==self._seq[ix-1].deadfrac and
                self._seq[ix].thrust==self._seq[ix-1].thrust and
                self._seq[ix].nconfig==self._seq[ix-1].nconfig and
                self._seq[ix].nworking==self._seq[ix-1].nworking and
                self._seq[ix].comment==self._seq[ix-1].comment):
                self._seq[ix].updateStart(self._seq[ix-1].start)
                self._seq[ix-1:]=self._seq[ix:]
            else:
                ix+=1
        return len(self._seq)

    def size(self):
        "Return size of list"
        return len(self._seq)

    def list(self):
        "Return the list itself"
        return self._seq

    def __str__(self):
        "Print representation of StatusList"
        rep=''
        for i in self._seq:
            rep+='[%i %i] : %i %6.3f %6.3f %s\n' % (i.start,i.stop,i.code,i.deadfrac,i.thrust,i.comment)
        return rep
