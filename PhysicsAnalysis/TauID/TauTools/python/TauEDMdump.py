# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file TauTools/python/TauEDMdump.py
## @purpose Dumping the tau EDM contents into a flat root tuple and store map in storegate
## @author Dugan O'Neil with lots of help from Sebastien Binet and Doug Schouten

import inspect
from array import array
import AthenaCommon.SystemOfUnits as Units
from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena
from ROOT import TFile,TH1F,TH2F,TTree,TClonesArray
from TauAssocDump import *

class TauEDMDump(PyAthena.Alg):
    """My first python algorithm"""
    def __init__(self, name="TauEDMDump",**kw):
	        ## init the base class
        kw['name'] = name
        super(TauEDMDump,self).__init__(**kw)
        self.doTree = kw.get('doTree',True)
        self.doStore = kw.get('doStore',False)
        self.dumpLevel = kw.get('dumpLevel',0)
        self.blackList = kw.get('blackList','test')
        self.initialized=False 

# initialize histo service
        self.hsvc = None
        return
##############################################
    def _makeEmptyDetails(self,tauCounter,detailType):
        for detailMethod in self.classMethodDict[detailType]:
            self.varDict["Tau_"+detailMethod][tauCounter] = 0.0
        return
##############################################
    def initialize(self):
        # Use dumpLevel specified in JO to set what will be dumped
        if self.dumpLevel == 0:
            self.doDetails = False
            self.ignoreBlackList = False
            self.doAssoc = False
            self.doExtraDetails = False
        elif self.dumpLevel == 1:
            self.doDetails = True
            self.ignoreBlackList = False
            self.doAssoc = False
            self.doExtraDetails = False
        elif self.dumpLevel == 2:
            self.doDetails = True
            self.ignoreBlackList = False
            self.doAssoc = True
            self.doExtraDetails = False
        else:
            self.doDetails = True
            self.ignoreBlackList = True
            self.doAssoc = True
            self.doExtraDetails = True

        # if ignoreBlackList is true, set blackList to blank
        if self.ignoreBlackList:
            self.blackList[:] = []
        ## note that we are using the python logging service
        ## PyAthena.Alg base class has already initialized it for us
        self.msg.info('initializing [%s]...',self.name())
        self.msg.debug('retrieve StoreGateSvc pointer...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if self.sg is None:
            self.msg.error('Problem retrieving StoreGateSvc pointer !')
            return StatusCode.Success
        else:
            self.msg.info('retrieved [%s]', self.sg.name())
        # list classes to dump
        self.classList = ["PyAthena.Analysis.TauJet","PyAthena.Analysis.TauPID"]
        if self.doDetails:
            self.classList.append("PyAthena.Analysis.TauCommonDetails")
        if self.doExtraDetails:
            self.classList.append("PyAthena.Analysis.TauCommonExtraDetails")
        # list return types to dump
        typeList = ["int","unsigned int","float","long"]
        self.varKey = "Tau" # prefix for all dumped variables
        # Methods which should be excluded from the dump - MAYBE
        self.badMethodList = ["tanTh","iPt","rapidity"]
        #
        self.classMethodDict = {}
        self.varDict = {}
        self.maxTaus = 30
        # loop over classes, methodList for each. 
        # Use instantiation+"try" to figure out the return type.
        # everything is a float in our tree, up to maxTaus taus/event
        # fill a dictionary of arrays, one for each variable in the final tree/map
        for i in range(len(self.classList)):
            objkey = self.classList[i].split(".")[2] # class name
            tau_class = eval(self.classList[i])
            methodList = []
            inst = tau_class()
            for meth_name in inspect.getmembers(tau_class):
                if (meth_name[0] not in self.badMethodList):
                    try:
                        v=getattr(inst,meth_name[0])()
                        methType=str(type(v)).split("'")[1].split("'")[0]
                        if methType in typeList and not meth_name[0].startswith("__"):
                            methodList.append(meth_name[0])
                            self.varDict[self.varKey+"_"+meth_name[0]] = array('f',self.maxTaus*[0.])
                    except:
                        pass
            # Now fill the dictionary of classes/methodLists.
            # Note that the objkey is a string like "TauJet" or "TauCommonDetails"
            self.classMethodDict[objkey] = methodList[:]
        # OK, now add the "extra" validation info. associated tracks, etc.
        if self.doAssoc:
            self.varDict.update(initTrkDump(self.maxTaus))
            self.varDict.update(initSeedCaloTrkDump(self.maxTaus))
            self.varDict.update(initSeedCaloWideTrkDump(self.maxTaus))
            self.varDict.update(initOtherTrkDump(self.maxTaus))
            self.varDict.update(initSecVtx(self.maxTaus))
            self.varDict.update(initPiZero(self.maxTaus))
            self.varDict.update(initSumPi0Vec(self.maxTaus))
            self.varDict.update(initTauPID(self.maxTaus))
            self.varDict.update(initIsTau(self.maxTaus))
        # add authorship           
        self.varDict[self.varKey+"_hasAuthor_tauRec"] = array('f',self.maxTaus*[0.])
        self.varDict[self.varKey+"_hasAuthor_1P3P"] = array('f',self.maxTaus*[0.])
        # Initialize ROOT stuff
        if self.doTree:
            self.hsvc = PyAthena.py_svc('THistSvc/THistSvc')
            if not self.hsvc:
                self.msg.error('could not retrieve THistSvc/THistSvc')
                return StatusCode.Success
            from ROOT import TTree
            self.hsvc['/rec/TauEDMtree'] = TTree('TauEDMtree','Tau EDM Tree')
            self.tr = self.hsvc['/rec/TauEDMtree']
        # set length of variable length arrays by the number of taus/tracks in the event/tau
            self.Tau_N = array('i',[0])
            self.tr.Branch("Tau_N",self.Tau_N,"Tau_N/I")
        # prepare the C++ vector which will be sent to storegate
        # currently, a vector of strings is not supported by storegate, 
        # use a dummy map instead. Also prepare map for execute filling.
        if self.doStore:
            # prepare the C++ map which will be sent to storegate from execute
            import AthenaPython.Bindings as B
            self.sgMap = PyAthena.std.map('std::string', 'std::vector<float>')()
            B._clid_typename_aliases['map<string,vector<float> >'] = 'std::map<std::string,std::vector<float> >'
            # prepare map of names to be sent from initialize 
            self.sgMapNames = PyAthena.std.map('std::string', 'int')()
            B._clid_typename_aliases['map<string,int>'] = 'std::map<std::string,int>'
            self.sg['TauEDMDumpNames'] = self.sgMapNames
        # make branches for each variable using a simple dictionary loop
        # However, exclude variables on the "blackList"
        for k,v in self.varDict.iteritems():
            badTag = False
            if k in self.blackList:
                badTag = True
            if self.doTree and not badTag:
                self.tr.Branch(k, self.varDict[k], k+"[Tau_N]"+"/F")
            if self.doStore and not badTag:
                self.sgMapNames[k] = 2
        # Set the initialized state flag and return success
        self.initialized=True
        return StatusCode.Success
######################################################    
    def execute(self):
        self.msg.verbose ('===================TauEDMdump==============')
        self.msg.verbose('running execute...')
        try:
            taus = self.sg.retrieve("Analysis::TauJetContainer","TauRecContainer")
        except:
            self.msg.warning('Problem retrieving TauJetContainer pointer !')
            return StatusCode.Success
        #
        if self.doTree:
            self.Tau_N[0] = taus.size()
        # loop over tau list in this event, filling variables
        tauCounter = 0
        for tau in enumerate(taus):
            if tauCounter >= self.maxTaus:
                break
            # get key for this object type. objkey looks like "TauJet", 
            # then loop over chosen methods, filling the tree dictionary
            objkey = str(type(tau[1])).split("::")[1].split("'")[0]
            if self.classMethodDict.has_key(objkey):
                for method in self.classMethodDict[objkey]:
                    value = getattr(tau[1],method)()
                    self.varDict[self.varKey+"_"+method][tauCounter] = float(value)
            # now get the PID for each tau
            pid = tau[1].tauID()
            objkey = "TauPID"
            if self.classMethodDict.has_key(objkey):
                for method in self.classMethodDict[objkey]:
                    value = getattr(pid,method)()
                    self.varDict[self.varKey+"_"+method][tauCounter] = float(value)
            dumpTauPID(tauCounter,pid,self.varDict)
            dumpIsTau(tauCounter,pid,self.varDict)                    
            # Check the authorship
            self.varDict[self.varKey+"_hasAuthor_tauRec"][tauCounter] =  float(tau[1].hasAuthor(PyAthena.TauJetParameters.tauRec))        
            self.varDict[self.varKey+"_hasAuthor_1P3P"][tauCounter] =  float(tau[1].hasAuthor(PyAthena.TauJetParameters.tau1P3P))        
            # also dump some variables from the associated tracks
            if self.doAssoc:
                dumpTrkLink(tauCounter, tau, self.varDict)                  
                dumpSeedCaloTrkLink(tauCounter, tau, self.varDict)
            ###########
            # Now look at the Details classes
            # loop over Details classes. Getting pointers from TauJets.
            # Warning: pointer to extraDetails will exist but be garbage
            # in AODs...will work fine in ESDs. So, I put that whole section 
            # inside a try statement so that running dumpLevel=3+ on 
            # AOD will not crash. Not particularly elegant. 
            if self.doDetails:
                hasDetails = False
                hasExtraDetails = False
                for iDetail in range(tau[1].nDetails()):
                    if (tau[1].detailName(iDetail).find("Extra") is -1) or self.doExtraDetails:
                        try: # protect against dumping extraDetails on AOD
                            detail = tau[1].detail(iDetail) # get the Detail
                            detailType = str(type(detail)).split('::')[-1].split("'")[0]
                            for detailMethod in self.classMethodDict[detailType]:
                                value = getattr(detail,detailMethod)()
                                self.varDict[self.varKey+"_"+detailMethod][tauCounter] = float(value)
                                if detailType == "TauCommonDetails":
                                    hasDetails = True
                                    if self.doAssoc:
                                        dumpSeedCaloWideTrks(tauCounter, detail, self.varDict)  
                                        dumpOtherTrks(tauCounter, detail, self.varDict)          
                                        dumpSecVtx(tauCounter,detail,self.varDict)
                                        dumpSumPi0Vec(tauCounter, detail,self.varDict)
                                        dumpPiZero(tauCounter, detail, self.varDict)
                                elif detailType == "TauCommonExtraDetails":
                                    hasExtraDetails = True
                        except:
                            pass

            # if this tau is missing one of its details, make an empty one            
                if not hasDetails:

 
                    self._makeEmptyDetails(tauCounter,"TauCommonDetails")
                if not hasExtraDetails and self.doExtraDetails:
                    self._makeEmptyDetails(tauCounter,"TauCommonExtraDetails")            
            tauCounter += 1 # end loop over taus
        # fill ROOT tree        
        if self.doTree:
            self.tr.Fill()
        # now fill map and store in Storegate
        if self.doStore:
            for k,v in self.varDict.iteritems():
                self.sgMap[k].size()
                self.sgMap[k].clear()
                for i in range(tauCounter):
                    self.sgMap[k].push_back(v[i])
            self.sg['TauEDMDump'] = self.sgMap
        #
        return StatusCode.Success

###############################################
    def finalize(self):
        self.msg.info('finalizing...')
        if self.doStore:
            self.sgMap.clear()
        return StatusCode.Success
