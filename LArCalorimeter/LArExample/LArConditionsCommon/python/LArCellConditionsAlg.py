# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

##=================================================================================
## Name:        LArCellConditionsAlg.py
##
## Description: An athena-like algorithm to interactivly convert LAr Identifiers
##              and print the bad channel status
##               
##==================================================================================

__author__="Walter Lampl <walter.lampl@cern.ch>"
__doc__ = " An athena-like algorithm to interactivly convert LAr Identifiers and print the bad channel status"

#import AthenaCommon.SystemOfUnits as Units
from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena

import cppyy 

from ROOT import HWIdentifier, Identifier, Identifier32, IdentifierHash, LArBadChannel
from ROOT import CaloDetDescrManager

from ctypes import c_uint

from string import *
#from cmath import pi
from time import clock

import os

class LArCellConditionsAlg(PyAthena.Alg):
    """
    This class is an Athena algorithm although it doesn't quite behave like one.
    It's purpose is to interactivly convert LAr Online/Offline identifiers and
    prints the bad channel status and optionally some calibration constants for
    the given channel identifier.
    """
    
    def __init__(self, name="LArCellConditionsAlg", **kw):
        ## init the base class
        kw['name'] = name
        super(LArCellConditionsAlg,self).__init__(**kw)

        self.includeConditions=kw.get('printConditions',False)
        self.includeLocation=kw.get('printLocation',False)
        self.includeDSPTh=kw.get('printDSPTh',False)
        self.nLinesPrinted=0
        self.nEvts=0
        
        return
    

    def initialize(self):
        ## note that we are using the python logging service
        ## and that the PyAthena.Alg base class has already initialized
        ## it for us

        # Get DetectorStore...
        from StoreGateBindings.Bindings import StoreGate
        self._detStore = StoreGate.pointer("DetectorStore")
        if self._detStore is None:
            self.msg.error("Failed to get DetectorStore")
            return StatusCode.Failure
        # Get LArOnlineID helper class
        self.onlineID=self._detStore.retrieve("LArOnlineID","LArOnlineID")
        if self.onlineID is None:
            self.msg.error("Failed to get LArOnlineID")
            return StatusCode.Failure

        # Get CaloCell_ID
        self.offlineID=self._detStore.retrieve("CaloCell_ID","CaloCell_ID")
        if self.offlineID is None:
            self.msg.error("Failed to get CaloCell_ID")
            return StatusCode.Failure


        # -----------------------------------------------------------
        # Initialize LArCabling service
        self.larCablingSvc=PyAthena.py_tool("LArCablingService")
        if self.larCablingSvc is None:
            self.msg.error('Problem retrieving LArCablingService pointer !')
            return StatusCode.Failure
        else:
            self.msg.info('retrieved [%s]', self.larCablingSvc.name())


        self.badChannelTool=PyAthena.py_tool("LArBadChanTool",iface="ILArBadChanTool")
        if self.badChannelTool is None:
            self.msg.error('Problem retrieving LArBadChanTool pointer !')
            return StatusCode.Failure
        else:
            self.msg.info('retrieved [%s]', self.badChannelTool.name())


        self.class_larBCBitPacking=cppyy.makeClass("LArBadChanBitPacking")
        self.bc_packing=self.class_larBCBitPacking()

        self.noisepattern=0L
        for n in ("lowNoiseHG","highNoiseHG","unstableNoiseHG","lowNoiseMG","highNoiseMG","unstableNoiseMG","lowNoiseLG","highNoiseLG","unstableNoiseLG","sporadicBurstNoise"):
            stat=self.bc_packing.enumName(n)
            if stat[0]:
                self.noisepattern |= 1<<stat[1]

        self.deadpattern=0L
        for n in ("deadReadout","deadPhys","almostDead"):
            stat=self.bc_packing.enumName(n)
            if stat[0]:
                self.deadpattern |= 1<<stat[1]

        self.noid=Identifier()

        if self.includeLocation:
            try:
                self.caloDDM = CaloDetDescrManager.instance() ;
            except:
                print "Failed to retrieve CaloDDM"
                return StatusCode.Failure


        self.larPedestal=None
        self.larMphysOverMcal=None
        self.larRamp=None
        self.larDAC2uA=None
        self.laruA2MeV=None
        self.larhvScaleCorr=None
        self.larDSPThr=None

        return StatusCode.Success

        
    def execute(self):
        self.msg.info('running execute...')

        #for some obscure reason, we need run dump before we can retrieve the flat objects using their abstract interface
        garbagedump = open(os.devnull, 'w')
        self._detStore.dump(garbagedump)
        garbagedump.close()

        if self.includeConditions:
            try:
                self.larPedestal=self._detStore.retrieve("ILArPedestal","Pedestal")
            except Exception,e:
                print "WARNING: Failed to retrieve Pedestal from DetStore"
                print e
                self.larPedestal=None
                
            try:
                self.larMphysOverMcal=self._detStore.retrieve("ILArMphysOverMcal","LArMphysOverMcal")
            except Exception,e:
                print "WARNING: Failed to retrieve MphysOverMcal from DetStore"
                print e
                self.larMphysOverMcal=None

            try:
                self.larRamp=self._detStore.retrieve("ILArRamp","LArRamp")
            except Exception,e:
                print "WARNING: Failed to retrieve LArRamp from DetStore"
                print e
                self.larRamp=None
                
            try:
                self.larDAC2uA=self._detStore.retrieve("ILArDAC2uA","LArDAC2uA")
            except Exception,e:
                print "WARNING: Failed to retrieve LArDAC2uA from DetStore"
                print e
                self.larDAC2uA=None

            try:
                self.laruA2MeV=self._detStore.retrieve("ILAruA2MeV","LAruA2MeV")
            except Exception,e:
                print "WARNING: Failed to retrieve LAruA2MeV from DetStore"
                print e
                self.laruA2MeV=None

            try:
                self.larhvScaleCorr=self._detStore.retrieve("ILArHVScaleCorr","LArHVScaleCorr")
            except Exception,e:
                print "WARNING: Failed to retrieve LArHVScaleCorr from DetStore"
                print e
                self.larhvScaleCorr=None
                
        if self.includeDSPTh:
            try:
                self.larDSPThr=self._detStore.retrieve("LArDSPThresholdsComplete","LArDSPThresholds")
            except Exception,e:
                print "WARNING: Failed to retrieve LArDPSThresholds from DetStore"
                print e
                self.larDSPThr=None   
            

        if self.nEvts==0:
            self.nEvts+=1
            #Process one 'dummy' event to make sure all DB connections get closed
            #print "Dummy event..."
            return StatusCode.Success

        self.onlineID.set_do_checks(True)
        self.offlineID.set_do_checks(True)
        
        while (1):
            id=None
            chid=None
            rep_in=self.readInput() #"Enter Id >").upper().strip()
            #print "User Input..."
            #rep_in="EMBA 0 0 60 2"
            rep=rep_in.upper()
            
            #Quit
            if len(rep)==0 or rep=="QUIT" or rep=="EXIT" or rep==".Q": break

                               
            if rep.startswith("SEARCH"):
                inp=rep_in[6:].strip();
                starttime=clock()
                self.search(inp)
                print "seach time %.1f sec" % (clock()-starttime)
                continue

            #Help
            if rep=="HELP":
                self.printHelp()
                continue



            try:
                t_int=int(rep,10)
                t=Identifier(c_uint(t_int))
                if self.onlineID.is_lar(t):
                    print t," IsLAr (online)"
                if self.offlineID.is_lar(t):
                    print t," isLAr (offline)" 
            except:
                pass



            

            if rep.startswith("ON"):
                s=rep.find(" ")
                if s==-1:s=0
                chid=self.getOnlineIDFromString(rep[s:])
                if chid is not None and self.onlineID.is_lar(chid):
                    id=self.larCablingSvc.cnvToIdentifier(chid)
                    if id is None: id=self.noid
                    self.printChannelInfo(id,chid)
                else:
                    print "ERROR: Could not interpret input."
                    print "Interpretation as online ID gave:",self._onlErrStr
                continue

            if rep.startswith("OF"):
                s=rep.find(" ")
                if s==-1:s=0
                id=self.getOfflineIDFromString(rep[s:])
                if id is not None and self.offlineID.is_lar(id):
                    chid=self.larCablingSvc.createSignalChannelID(id)
                    self.printChannelInfo(id,chid)
                else:
                    print "ERROR: Could not interpret input."
                    print "Interpretation as offline ID gave:",self._oflErrStr
                continue

            #Try to interpet input as identifiers
            chid=self.getOnlineIDFromString(rep)
            if chid is not None and self.onlineID.is_lar(chid):
                id=self.larCablingSvc.cnvToIdentifier(chid)
            else: #try interpret at offline ID
                id=self.getOfflineIDFromString(rep)
                if id is not None and self.offlineID.is_lar(id):
                     chid=self.larCablingSvc.createSignalChannelID(id)
                     
            if chid is None or id is None:
                print "ERROR: Could not interpret input."
                print "Interpretation as online ID gave:",self._onlErrStr
                print "Interpretation as offline ID gave:",self._oflErrStr
                continue

            self.printChannelInfo(id,chid)
            #break
        return StatusCode.Success

    def printChannelInfo(self,id,chid):
        bc=self.badChannelTool.status(chid)
        print self.IdentifiersToString(chid,id) + " " + self.bc_packing.stringStatus(bc)
        
        if id!=self.noid: #Don't try to show anything more for disconnected channels
            if self.includeLocation:
                try:
                    #idHash=self.offlineID.calo_cell_hash(id)
                    theDDE=self.caloDDM.get_element(id)
                    print "raw Eta= %.3f, Phi=%.3f r=%.3f" % (theDDE.eta_raw(),theDDE.phi_raw(),theDDE.r_raw())
                    print "raw x=%.3f, y=%.3f, z=%.3f" % (theDDE.x(),theDDE.y(),theDDE.z_raw())
                except Exception,e:
                    print e
               
            if self.includeConditions:
                if self.larDAC2uA is not None:
                    print "DAC2uA: %.3f" % self.larDAC2uA.DAC2UA(chid),
                else:
                    print "DAC2uA: None",
                        
                if self.laruA2MeV is not None:
                    print " uA2MeV: %.3f" % self.laruA2MeV.UA2MEV(chid),
                else:
                    print " uA2MeV: None",

                if self.larhvScaleCorr is not None:
                    print " HVScaleCorr: %.3f" % self.larhvScaleCorr.HVScaleCorr(chid)
                else:
                    print " HVScaleCorr: None"
                          
                for gain,gainname in ((0,"HG"),(1,"MG"),(2,"LG")):
                    print gainname,
                    if self.larPedestal is not None:
                        ped=self.larPedestal.pedestal(chid,gain)
                        pedRMS=self.larPedestal.pedestalRMS(chid,gain)
                    else:
                        ped=-9999
                        pedRMS=-9999

                    print "Ped: %.3f " % ped,
                    print "PedRMS: %.3f" % pedRMS,

                    if self.larRamp is not None:
                        ramp=self.larRamp.ADC2DAC(chid,gain)
                        if len(ramp)>1:
                            print " Ramp: %.3f" % ramp[1],
                        else:
                            print " Ramp: Empty",
                    else:
                        print " Ramp: None",

                    if self.larMphysOverMcal is not None:
                        mpmc=self.larMphysOverMcal.MphysOverMcal(chid,gain)
                        print " MphysOverMcal: %.5f" % mpmc,
                    else:
                        print " MphysOverMcal: None",
                    print ""
            if self.includeDSPTh:
                if self.larDSPThr is not None:
                    tQThr=self.larDSPThr.tQThr(chid)
                    samThr=self.larDSPThr.samplesThr(chid)
                    trgSumTrh=self.larDSPThr.trigSumThr(chid)
                    print "DSP Thresholds: tQ:%f, samples:%f, trigSum:%f" % (tQThr,samThr,trgSumTrh)
                else:
                    print "DSP Thresholds: None"



    def finalize(self):
        self.msg.info('finalizing...')
        return StatusCode.Success


    def output(self,out,file=None):
        if file is not None: file.write(out+"\n")
        print out
        self.nLinesPrinted=self.nLinesPrinted+1
        if self.nLinesPrinted%40 is 0:
            c=raw_input("Press 'q' to quit, 'a' for all ...")
            if c.upper().startswith("Q"):
                return True
            if c.upper().startswith("A"):
                self.nLinesPrinted=0xFFFFFFFF
        return False


    def readInput(self):
        self.nLinesPrinted=0
        try:
            rep=raw_input("Enter Id >")
        except:
            return ""

        return rep.strip()

    def IdentifiersToString(self,chid,id):
        onlName="None"
        oflName="None"
        if chid is not None:
            onlName=str(chid.get_identifier32().get_compact())+" " #+str(chid.get_compact())+" "
            #onlName=self.onlineID.channel_name(chid)
            bec=self.onlineID.barrel_ec(chid)
            side=self.onlineID.pos_neg(chid)
            ft=self.onlineID.feedthrough(chid)
            slot=self.onlineID.slot(chid)
            chan=self.onlineID.channel(chid)
            ftname=self.onlineID.feedthrough_name(chid) 
            
            if bec==0:
                onlName+="[BARREL/"
            else:
                onlName+="[ENDCAP/"
                
            if side==0:
                onlName+="C/"
            else:
                onlName+="A/"

            onlName+="FT "+str(ft)+"("+ftname+")/Slot "+str(slot)+"/Chan "+str(chan)

            try:
                calibLines=self.larCablingSvc.calibSlotLine(chid);
                if (len(calibLines)):
                    onlName+="/CL"
                    for calib_chid in calibLines:
                        onlName+=" "+str(self.onlineID.channel(calib_chid))
            except:
                pass

            onlName+=']'
            
        if id is not None:

            oflName=str(id.get_identifier32().get_compact()) + " "# +str(id.get_compact())+" "

            if (id==self.noid):#0xFFFFFFFF):
                oflName += "[disconnected]"
            else:
                subcalo=self.offlineID.sub_calo(id)
                side=self.offlineID.pos_neg(id)
                layer=self.offlineID.sampling(id)
                region=self.offlineID.region(id)
                eta=self.offlineID.eta(id)
                phi=self.offlineID.phi(id)

                if subcalo is self.offlineID.LAREM:
                    if abs(side)==1:
                        oflName+="[EMB"
                    elif abs(side)==2:
                        oflName+="[EMECOW"
                    elif abs(side)==3:
                        oflName+="[EMECIW"
                    else:
                        oflName+="[UNKNOWN"
                elif subcalo is self.offlineID.LARHEC:
                    oflName+="[HEC"
                elif subcalo is self.offlineID.LARFCAL:
                    oflName+="[FCAL"
                else:
                    oflName+="[UNKNOWN"

                oflName+="/"
                if side>0:
                    oflName+="A/"
                else:
                    oflName+="C/"

                oflName+="Lay "+str(layer)+"/Reg "+str(region)+"/Eta "+str(eta)+"/Phi "+str(phi) +"]"
        return onlName+" = "+oflName
        
        
    def getOfflineIDFromString(self,input):
        self._oflErrStr=""
        ###print "offline id input=[%s]" % input
        upInput=input.upper().strip()
        if upInput.startswith('0X'):
            #hex-input
            try:
                id_int=int(upInput,16)
            except:
                self._oflErrStr="Can't interpret hexadecimal identifier, got " + upInput
                return None
            id=Identifier(id_int << 32)
            if not self.offlineID.is_lar(id):
                self._oflErrStr="Not a LAR identifier: " + upInput
                return None

        elif upInput.isdigit():
            try:
                id_int=int(upInput,10)
            except:
                self._oflErrStr="Can't interpret decimal identifier, got "+upInput
                return None
            id=Identifier(id_int << 32)

            if not self.offlineID.is_lar(id):
                self._oflErrStr="Not a LAR identifier: " + upInput
                return None

        else: #given as expanded ID
            tbl=maketrans(",:;/\#","      "); 
            fields=[]
            for f in upInput.translate(tbl).split():
                if len(f):
                    fields+=[f]
            if len(fields)<5:
                self._oflErrStr="Not enough fields to build offline id"
                return None
            
            #Format: Subdet/Side/Layer/Region/Eta/Phi
            if fields[0].startswith("EMB"):
                subcalo=self.offlineID.LAREM
                bepn=1
            elif fields[0].startswith("EMECOW"):
                subcalo=self.offlineID.LAREM
                bepn=2
            elif fields[0].startswith("EMECIW"):
                subcalo=self.offlineID.LAREM
                bepn=3
            elif fields[0].startswith("HE"):
                subcalo=self.offlineID.LARHEC
                bepn=2
            elif fields[0].startswith("F"):
                subcalo=self.offlineID.LARFCAL
                bepn=2
            else:
                self._oflErrStr="Can't interpret subcalo field, got "+fields[0]
                return None

            if len(fields)==6:
                sidefield=fields[1]
            elif len(fields)==5:
                sidefield=fields[0]
            else:
                self._oflErrStr="Can't interpret input: Expected 5 or 6 sub-fields, found "+str(len(fields))
                return None

            if sidefield.endswith("A") or sidefield.endswith("P") or sidefield.endswith("POS") or sidefield is '1':
                side=1
            elif sidefield.endswith("C") or sidefield.endswith("N") or sidefield.endswith("NEG") or sidefield is '0':
                side=-1
            else:
                self._oflErrStr="Can't interpret field for detector side, got "+ sidefield
                return None

            bepn=bepn*side


            if fields[-4].isdigit():
                layer=int(fields[-4])
            else:
                ls=fields[-4]
                if (ls=="PS" or ls.startswith("PRES")): layer=0
                elif (ls=="FRONT" or ls.startswith("STRIP")): layer=1
                elif ls.startswith("MID"): layer=2
                elif ls=="BACK": layer=3
                else:
                    self._oflErrStr="Can't interpet textual value for layer, got " + ls
                    return None  
            try:
                region=int(fields[-3])
                eta=int(fields[-2])
                phi=int(fields[-1])
            except:
                self._oflErrStr="Not-numerical input for region, eta or phi"
                return None

            #print "Got",subcalo,bepn,layer,region,eta,phi
            #self.offlineID.set_do_checks(True)
            try: #Build Region ID
                regid=self.offlineID.region_id(subcalo,bepn,layer,region)
            except:
                self._oflErrStr="Failed to build offline region identifier. Layer/region values inconsistent?"
                return None   
                
            etamin=self.offlineID.eta_min(regid)
            etamax=self.offlineID.eta_max(regid)
            
            if (eta<etamin or eta>etamax):
                self._oflErrStr="Eta must be between %i and %i for this region, got %i" % (etamin,etamax,eta)
                return None

            phimin=self.offlineID.phi_min(regid)
            phimax=self.offlineID.phi_max(regid)
            if (phi<phimin or phi>phimax):
                self._oflErrStr="Phi must be between %i and %i for this region, got %i" % (phimin,phimax,phi)
                return None

            id=None
            try:
                id=self.offlineID.cell_id(regid,eta,phi)
                #id=self.offlineID.cell_id(subcalo,bepn,layer,region,eta,phi)
            except:
                self._oflErrStr="Failed to build offline identifier. One or more values out-of-range?"
                id=None
                
            #self.offlineID.set_do_checks(False)
        return id
            
            
    def getOnlineIDFromString(self,input):
        #print "onfline id input=[%s]" % input
        self._onlErrStr=""
        upInput=input.upper().strip()
        if upInput.startswith('0X'):
            #hex-input
            try:
                id_int=int(upInput,16)
            except:
                self._onlErrStr="Can't interpret hexadecimal online identifier, got "+input
                return None

            if (id_int < 0x38000000 or id_int > 0x3bc68000):
                self._onlErrStr="Outside of numerical range of online identifiers"
                return None


            id=HWIdentifier(id_int<<32)
            if not self.onlineID.is_lar(id):
                self._onlErrStr="Not a LAR identifier:" + input
                return None
            
        elif upInput.isdigit():
            try:
                id_int=int(upInput,10)
            except:
                self._onlErrStr="Can't interpret decimal online identifier, got "+input
                return None
            
            if (id_int < 0x38000000 or id_int > 0x3bc68000):
                self._onlErrStr="Outside of numerical range of online identifiers"
                return None

            id=HWIdentifier(id_int<<32)
            if not self.onlineID.is_lar(id):
                self._onlErrStr="Not a LAR identifier:" + input
                return None

        else: #given as expanded ID
            tbl=maketrans(",:;/\#","      "); 
            fields=[]
            for f in upInput.translate(tbl).split():
                if len(f):
                    fields+=[f]

            #fields is now a 'clean array' of input
            #Format: B/EC Side FT Slot Chan
            if len(fields)==4:
                sideField=fields[0]
            elif len(fields)==5: # and len(fields[1])==1:
                sideField=fields[1]
            else:
                self._onlErrStr="Expected 4 or 5 fields, found "+str(len(fields))
                return None  
            
            if "BARREL".find(fields[0][0:6])==0:
                bec=0
            elif "ENDCAP".find(fields[0][0:6])==0:
                bec=1
            elif "EC".find(fields[0][0:2])==0:
                bec=1
            else:
                self._onlErrStr="Can't interpret field for Barrel/Endcap, got "+fields[0]
                return None 
            
            if sideField.endswith("A") or sideField=="1" or sideField.startswith("P"):
                side=1
            elif sideField.endswith("C") or sideField=="0" or sideField.startswith("N"):
                side=0
            else:
                self._onlErrStr="Can't interpret field for detector side, got " + sideField
                return None 
            try:
                chan=int(fields[-1])
                slot=int(fields[-2])
                ft=int(fields[-3])
            except:
                self._onlErrStr="Not-numerical input for FT slot or channel"
                return None

            if bec==0:
                if ft<0 or ft>31:
                    self._onlErrStr="Barrel feedthrough number must be between 0 and 31, got "+str(slot)
                    return None
            else:
                if ft<0 or ft>24:
                    self._onlErrStr="Endcap feedthrough number must be between 0 and 24, got "+str(slot)
                    return None

            if slot<1 or slot>15:
                self._onlErrStr="Slot number must be between 1 and 15, got "+str(slot)
                return None

            if chan<0 or chan>127:
                self._onlErrStr="Channel number must be between 0 and 127, got "+str(chan)
                return None
            
            
            id=None  
            #self.onlineID.set_do_checks(True)
            try:
                id=self.onlineID.channel_Id(bec,side,ft,slot,chan)
            except:
                self._onlErrStr="Failed to build online identifier. One or more values out-of-range?"
                id=None
            #self.onlineID.set_do_checks(False)
        return id


    def search(self,input):

        subcalo=None
        eta=None
        phi=None
        bec=None
        side=None
        ft=None
        slot=None
        bctypes=0L
        outname=None
        outfile=None
        layer=None
        cl=None
        tbl=maketrans(",:;#=","     ");
        tokens=[]
        for t in input.translate(tbl).split():
            if len(t):
                if (len(tokens)>0) and not tokens[-1].isalpha() and not t[0].isalpha():
                    tokens[-1]+=t
                else:
                    tokens+=[t]
                    
        def interpretToken(input):
            pos=input[1:].find("+-")
            if pos==-1:
                pos=input[1:].find("-")
                if pos==-1:
                    uplow=None
                    pos=len(input)-1
                else:
                    uplow=True
            else:
                uplow=False


            pos=pos+1
            try:
                e1=float(input[:pos])
            except:
                print "Expected numerical value, got",input[:pos] 
                return None

            if uplow is not None:
                try:
                    e2=float(input[pos+1:])
                except:
                    print "Expected numerical value after '+-', got",input[pos+1:] 
                    return None

            
                if uplow:
                    ret=[e1,e2]
                else:
                    ret=[e1-e2,e1+e2]
            else:
                ret=[e1-0.025,e1+0.025]
            ret.sort()
            return ret

        i=0
        while (i<len(tokens)):
            if tokens[i].upper()=="OUT" and i+1<len(tokens):
                outname=tokens[i+1]
                i=i+1
            elif tokens[i].upper()=="ETA" and i+1<len(tokens):
                eta=interpretToken(tokens[i+1])
                i=i+1
            elif tokens[i].upper()=="LAYER" and i+1<len(tokens):
                layer=interpretToken(tokens[i+1])
                i=i+1
            elif tokens[i].upper()=="PHI" and i+1<len(tokens):
                phi=interpretToken(tokens[i+1])
                i=i+1
            elif tokens[i].upper()=="FT" and i+1<len(tokens):
                ft=interpretToken(tokens[i+1])
                i=i+1
            elif tokens[i].upper()=="SLOT" and i+1<len(tokens):
                 slot=interpretToken(tokens[i+1])
                 i=i+1
            elif tokens[i].upper()=="BAD":
                bctypes=0xFFFFFFFF
            elif tokens[i].upper()=="NOISE" or tokens[i].upper()=="NOISY":
                bctypes |= self.noisepattern
            elif tokens[i].upper()=="DEAD":
                bctypes |= self.deadpattern
            elif tokens[i].upper().startswith("BAR"):
                bec=0
            elif tokens[i].upper().startswith("END"):
                bec=1
            elif tokens[i].upper()=="EM":
                subcalo=self.offlineID.LAREM
            elif tokens[i].upper()=="HEC":
                subcalo=self.offlineID.LARHEC
            elif tokens[i].upper()=="FCAL":
                subcalo=self.offlineID.LARFCAL
            elif tokens[i].upper()=="A" or tokens[i]=="P":
                side=1
            elif tokens[i].upper()=="C" or tokens[i]=="N":
                side=0
            elif tokens[i].upper()=="CL" and tokens[i+1].isdigit():
                cl=int(tokens[i+1])
                i=i+1
            else:
                stat=self.bc_packing.enumName(tokens[i])
                if stat[0]:
                    bctypes |= 1<<stat[1]
                else:
                    print "Unknown Keyword",tokens[i]    
            i=i+1

        print "Searching for cells with",
        if eta is not None: print " %.3f <= eta <= %.3f" % (eta[0],eta[1]),
        if phi is not None: print " %.3f <= phi <= %.3f" % (phi[0],phi[1]),
        if layer is not None: print " %.0f <= layer <= %.0f" % (layer[0],layer[1]),

        if bec==0: print " Barrel",
        if bec==1: print " Endcap",

        if side==1: print " A Side",
        if side==0: print " C Side",

        if ft is not None: print " %.0f <= FT <= %.0f" % (ft[0],ft[1]),
        if slot is not None: print " %.0f <= Slot <= %.0f" % (slot[0],slot[1]),
        
        if bctypes!=0:
            print self.bc_packing.stringStatus(LArBadChannel(bctypes)),

        if phi is None and eta is None and bctypes==0 and ft is None and side is None and bec is None and slot is None and layer and subcalo is None:
            print "No search criteria set!"
            return


        if cl is not None: print " CL:",cl,

        print ""

        if not self.includeLocation and (eta is not None or phi is not None):
            print "ERROR: GeoModel not activated, run with -g option"
            return
            
        if outname is not None:
            try:
                outfile=open(outname,"w")
            except IOError,e:
                print "ERROR failed to open file",outname
                print e
                outfile=None
                outname=None
                
          
        #for idH in range(self.offlineID.calo_cell_hash_max()): //This one includes also tile cells
        for idH in range(182468):
            idHash=IdentifierHash(idH)
            chid=self.larCablingSvc.createSignalChannelIDFromHash(idHash)
            
            #print "Loop hash=%i , on: %x , off: %x" % (idH, chid.get_identifier32().get_compact(), self.offlineID.cell_id(idHash).get_identifier32().get_compact())

            #Check Bad-Channel Status
            if bctypes!=0:
                bcstat=self.badChannelTool.status(chid)
                bcw=bcstat.packedData()
                if bcw & bctypes == 0: continue
                    
            #Check Online Id
            if bec is not None and bec!=self.onlineID.barrel_ec(chid): continue
            if side is not None and side!=self.onlineID.pos_neg(chid): continue
            if ft is not None and (self.onlineID.feedthrough(chid)<ft[0] or self.onlineID.feedthrough(chid)>ft[1]): continue
            if slot is not None and (self.onlineID.slot(chid)<slot[0] or self.onlineID.slot(chid)>slot[1]): continue

            if cl is not None:
                try:
                    calibLines=self.larCablingSvc.calibSlotLine(chid);
                    keep=False
                    for foundCLs in calibLines:
                        if self.onlineID.channel(foundCLs) == cl:
                            keep=True
                            break
                        
                    if not keep: continue
                                        
                except Exception,e:
                    print "Failed to get calib line:",e
                    continue

            #Check geometrical location
            ep=""
            if eta is not None or phi is not None:
                theDDE=self.caloDDM.get_element(idHash)
                e=theDDE.eta()
                p=theDDE.phi()
                if eta is not None and (e<eta[0] or e>eta[1]): continue
                if phi is not None and (p<phi[0] or p>phi[1]): continue
                ep=" eta=%.3f phi=%.3f" % (e,p)
                
            id=self.offlineID.cell_id(idHash)
            if subcalo is not None and subcalo!=self.offlineID.sub_calo(id): continue
            if layer is not None and (self.offlineID.sampling(id)<layer[0] or self.offlineID.sampling(id)>layer[1]): continue
            if bctypes==0: bcstat=self.badChannelTool.status(chid)

            br=self.output(self.IdentifiersToString(chid,id) + " " +ep+self.bc_packing.stringStatus(bcstat),outfile)
            if br: break
            
        if outfile is not None: outfile.close()
        return
  
    def printHelp(self):
        print "First mode of operation:"
        print "Enter a channel identifier (online or offline). Allowed forms are:"
        print "  Decimal or hexadecimal compact identifier, eg. 'online 0x39000000'"
        print "  Expanded identifer like"
        print "     'online Bar C 1 1 1' (Barrel C FT 1, Slot 1 channel 1)"
        print "     'offline FCAL A 1 0 1 1' (FCAL A, Layer 1, Region 0, Eta 1 Phi 1)"
        print "    Offline ids must start with one of the following subdetectors names:"
        print "     EMB, EMECIW, EMECOW, HEC, FCAL"
        print "    Online ids must start with either 'BAR' or 'END'"
        print "  If the prefix 'online' or 'offline' is omitted both are tried."
        print "\nSecond mode of operation:"
        print "Search <keyword> <keyword> .."
        print "  Keywords are: 'barrel', 'endcap', A', 'C', or any bad-channel type"
        print "  Keywords with arguments are: 'eta', 'phi', 'layer', 'FT', 'Slot' and 'CL'"  
        print "\nType 'quit' to terminate'"


