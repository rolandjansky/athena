# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from types import (IntType,LongType,FloatType,BooleanType)
from array import array
from math import (pi,hypot)
from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena
from ROOT import (TTree,)
from AthenaCommon.SystemOfUnits import (GeV,cm)
from operator import (attrgetter,itemgetter,setitem,getitem)

class TriggerTree(PyAthena.Alg):
    def __init__(self, name="JetTurnOnTree", **kw):
        kw['name'] = name
        super(TriggerTree,self).__init__(**kw)
        return

    def clearDATA(self):
        for i in self.DATA:
            self.DATA[i][0]=0
        return

    def treewrap(self,variable=None,value=None):
        if not self.DATA.has_key(variable):
            if type(value) in [IntType,LongType]:
                CHANGE=True
                self.DATA[variable]=array('i',[0])
                self.BRANCHES[variable]=self.tree.Branch(variable,self.DATA[variable],"%s/I" % variable)
            elif type(value) in [FloatType]:
                CHANGE=True
                self.DATA[variable]=array('d',[0.0])
                self.BRANCHES[variable]=self.tree.Branch(variable,self.DATA[variable],"%s/D" % variable)
            elif type(value) in [BooleanType]:
                CHANGE=True
                self.DATA[variable]=array('i',[0])
                self.BRANCHES[variable]=self.tree.Branch(variable,self.DATA[variable],"%s/I" % variable)
        self.DATA[variable][0]=value
        return

    def initialize(self):
        self.msg.info('initializing [%s]...',self.name())
        self.sg = PyAthena.py_svc('StoreGateSvc')
        self.hsvc=PyAthena.py_svc('THistSvc')
        self.trigDec = PyAthena.py_tool('Trig::TrigDecisionTool/TrigDecisionTool')
        #
        # TTree
        #
        self.DATA={}
        self.BRANCHES={}
        self.hsvc['/turnontree/dijets']=TTree("dijets","dijets")
        self.tree=self.hsvc['/turnontree/dijets']
        #
        for i in ['event_number','run_number','lumi_block']:
            self.treewrap(i,0)
        for i in ['%s%d' % (i,j)
                  for i in ['px','py','pz','e','rapidity','pt']
                  for j in [0,1]]:
            self.treewrap(i,0.0)
        for i in ['roi_%s%d' % (i,j)
                  for i in ['eta','phi']
                  for j in [0,1]]:
            self.treewrap(i,0.0)
        return StatusCode.Success


    def deltaR(self,eta1,phi1,eta2,phi2):
        phi=abs(phi1-phi2)
        if phi>2*pi: phi=phi-2*pi
        return hypot(abs(eta1-eta2),phi)

    def execute(self):
        #
        # This is a cooked example and is not intended for actual use in an analysis
        #
        #
        # Require at least two jets
        #
        inputjets=self.sg.retrieve("JetCollection","Cone4H1TopoJets")
        if len(inputjets)<2: return StatusCode.Success
        #
        # Need leading two jets
        #
        jets=map(itemgetter(1),sorted([(j.pt(),j) for j in inputjets],reverse=True)) 
        #
        # Did we fire a jet trigger?
        #
        jettriggers=self.trigDec.getChainGroup("EF_[jJ][0-9].*")
        JETTRIGGERS=jettriggers.getListOfTriggers()
        for i in JETTRIGGERS:
            self.treewrap(i,0)
            self.treewrap('prescale_%s' % i,1.0)
        ALSOJET=map(None,
                    JETTRIGGERS,
                    map(self.trigDec.isPassed,JETTRIGGERS),
                    map(self.trigDec.getPrescale,JETTRIGGERS)
                    )
        # get trigger elements
        fc=jettriggers.features()
        roijets=[i.cptr() for i in fc.get('TrigRoiDescriptor')('initialRoI')]
        if len(roijets)<1: return StatusCode.Success # The next line doesn't work without trigger jets...
        matchedroijets=[min([(self.deltaR(i.eta(),i.phi(),j.eta0(),j.phi0()),j) for j in roijets])[1] for i in jets[:2]]
        #
        # Event information
        #
        EventInfo=self.sg.retrieve('EventInfo')
        evid=EventInfo.event_ID()
        #
        # Fill TTree
        #
        # note that we are creating branches on the fly and this is technically incorrect
        self.clearDATA()
        for i in ['event_number','run_number','lumi_block']:
            self.DATA[i][0]=getattr(evid,i)()
        for i,j,k in ALSOJET:
            self.DATA[i][0]=j
            self.DATA['prescale_%s' % i][0]=k
        for i in ['px','py','pz','e','pt']:
            for j in [0,1]:
                self.DATA['%s%d' % (i,j)][0] =getattr(jets[j],i)()/GeV
        for i in ['rapidity']:
            for j in [0,1]:
                self.DATA['%s%d' % (i,j)][0] =getattr(jets[j],i)()
        for i in ['phi','eta']:
            for j in [0,1]:
                self.DATA['roi_%s%d' % (i,j)][0] = getattr(matchedroijets[j],'%s0' % i)()
        self.tree.Fill()
        return StatusCode.Success

    def finalize(self):
        return StatusCode.Success

