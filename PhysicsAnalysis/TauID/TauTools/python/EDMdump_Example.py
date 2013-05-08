# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file TauTools/python/EDMdump_Example.py
## @purpose Dumping the EDM contents into a flat root tuple 
## @author Dugan O'Neil 

import inspect
from array import array
import AthenaCommon.SystemOfUnits as Units
from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena
from ROOT import TFile,TH1F,TH2F,TTree,TClonesArray

class EDMdump_Example(PyAthena.Alg):
    """My first python algorithm"""
    def __init__(self, name="EDMdump_Example"):
	        ## init the base class
        super(EDMdump_Example,self).__init__()
        self.initialized=False 
# initialize histo service
        self.hsvc = None
        return
##############################################
    def initialize(self):
        ## note that we are using the python logging service
        ## and that the PyAthena.Alg base class has already initialized
        ## it for us
        self.msg.info('initializing [%s]...',self.name())
        self.msg.debug('retrieve StoreGateSvc pointer...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if self.sg is None:
            self.msg.error('Problem retrieving StoreGateSvc pointer !')
            return StatusCode.Failure
        else:
            self.msg.info('retrieved [%s]', self.sg.name())
        self.sgKeyList = ["ElectronAODCollection","Cone4H1TopoJets","StacoMuonCollection"]
        self.classList = ["PyAthena.Analysis.Electron","PyAthena.Jet","PyAthena.Analysis.Muon"]
        typeList = ["int","unsigned int","float","long"]
        self.classMethodDict = {}
        self.varDict = {}
        self.numDict = {}
        self.maxObjs = 30
        # loop over classes, methodList for each. 
        # Use instantiation+"try" to figure out the return type. 
        for i in range(len(self.classList)):
            key = self.classList[i].rsplit(".",1)[1]
            obj_class = eval(self.classList[i])
            methodList = []
            for meth_name in inspect.getmembers(obj_class):
                try:
                    v=getattr(obj_class(),meth_name[0])()
                    methType=str(type(v)).split("'")[1].split("'")[0]
                    if methType in typeList and not meth_name[0].startswith("__"):
                        methodList.append(meth_name[0])
                        # everything is a float in our tree, up to maxObjects/event
                        # fill a dictionary of arrays, one for each variable in the final tree/map
                        self.varDict[key+"_"+meth_name[0]] = array('f',self.maxObjs*[0.])
                except:
                    pass
            # Now fill the dictionary of classes/methodLists.
            # Note that the key is a string like "TauJet" 
            self.classMethodDict[key] = methodList[:]
            # I also want the number of each type of object in each event
            self.numDict["number_"+key] =  array('i',[0])
        # Initialize ROOT stuff
        # If you run this example with TauEDMdump.py you will create a separate tree for these objects
        # which I guess you should friend while analyzing. It is possible to instead add these variables to the
        # same tree by changing the tree name below from EDMtree to TauEDMtree, removing the line which
        # creates the tree and removing the "fill" in the execute method (else the tree would be filled twice!)
        self.hsvc = PyAthena.py_svc('THistSvc/THistSvc')
        if not self.hsvc:
            self.msg.error('could not retrieve THistSvc/THistSvc')
            return StatusCode.Failure
        from ROOT import TTree
        self.hsvc['/rec/EDMtree'] = TTree('EDMtree','EDM Tree') # remove this line if you want to use a single tree 
        self.tr = self.hsvc['/rec/EDMtree']
        # set length of variable length arrays by the number of objects in the event
        for myclass in self.classList:
            className = myclass.rsplit(".",1)[1]
            key = "number_"+className
            self.tr.Branch(key,self.numDict[key],key+"/I")
        # make branches for each variable. Name by class_method.
        # make variable length arrays in ntuple....be careful that the right number is used for each object type
        for k,v in self.varDict.iteritems():
                objectKey=k.split("_")[0]
                self.tr.Branch(k, self.varDict[k], k+"[number_"+objectKey+"]/F")
        # Set the initialized state flag and return success
        self.initialized=True
        return StatusCode.Success
######################################################    
    def execute(self):
        self.msg.info('running execute...')
        for sgKey in self.sgKeyList:
            objs = self.sg[sgKey]
            # I assume all objects inside this container have the same type
            # Figure out what the type is (eg. Electron, TauJet, etc.)
            # objects in the analysis namespace have ROOT.Analysis::Electron, while
            # others have just ROOT.Jet, must handle both kinds in general
            if objs.size() != 0:
                if str(type(objs[0])).find("::") is -1:
                    objkey = str(type(objs[0])).rsplit(".",1)[1].split("'")[0]
                else:
                    objkey = str(type(objs[0])).split("::")[1].split("'")[0]
                self.numDict["number_"+objkey][0] = objs.size() 
        # loop over object list in this event, filling variables
            objCounter = 0
            for obj in enumerate(objs):
                if objCounter >= self.maxObjs:
                    break
                # get key for this object type. objkey looks like "TauJet", 
                # so have to parse the return of "type"
                if self.classMethodDict.has_key(objkey):
                    for method in self.classMethodDict[objkey]:
                        value = getattr(obj[1],method)()
                        key=objkey+"_"+method
                        self.varDict[key][objCounter] = float(value)
                objCounter += 1
        # fill ROOT tree        
        self.tr.Fill()
        return StatusCode.Success
###############################################
    def finalize(self):
        self.msg.info('finalizing...')
        return StatusCode.Success
