# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# author: Andrea Di Simone

### based on Electron Skimmer by Karsten Koeneke


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
from math import sqrt
 
class CaloMuonOverlapFilter( PyAthena.AthFilterAlgorithm ):
    """ this removes overlap between calo muons and combined ones
    overlap is removed based on the associated id track """

    def __init__ ( self, name = "CaloMuonOverlapFilter", **kw ):
        
        kw['name'] = name
        super(CaloMuonOverlapFilter, self).__init__(**kw)
        
        self.inCaloColl=kw.get('inCaloMuColl', "CaloMuonCollection")
        self.outCaloColl=kw.get('outCaloMuColl', "UniqueCaloMuonCollection")
        self.combColls=kw.get('combCollections',[])
        self.storeGateSvc     = None
        
        return
    
    def initialize(self):
        
        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure
        
        return StatusCode.Success
    
    
    def execute(self):
        
        # Create a class-member list of all muons that passed filter.
        
        VIEW_ELEMENTS = 1
        goodMuons = PyAthena.Analysis.MuonContainer(VIEW_ELEMENTS)
        
        # retrieve input calo muons
        try:
            caloMuonCollection = self.storeGateSvc.retrieve("Analysis::MuonContainer" , self.inCaloColl )
        except LookupError:
            #if self.nProcessed <100:
            self.msg.warning( 'Collection %s not found' % self.inCaloColl ) 
            return StatusCode.Success


        #print "dumping original calo muons"
        #for m in caloMuonCollection:
        #    print m.eta(),m.phi(),m.pt(),m.track()

        # retrieve comb muon collections

        combcollections=[]

        for collName in self.combColls:

            try:
                combMuonCollection = self.storeGateSvc.retrieve("Analysis::MuonContainer" , collName )
            except LookupError:

                self.msg.warning( 'Collection %s not found' % collName ) 
                return StatusCode.Success

            combcollections.append(combMuonCollection)


        self.msg.debug( 'Retrieved %s collections for overlap removal ' % len(combcollections) ) 


        # now loop over input calo muons

        for calom in caloMuonCollection:

            thistrack=calom.track()

            if not self.hasSharedTrack(thistrack,combcollections):
                if not self.deltaROverlap(0.05,calom,combcollections):
                    goodMuons.push_back( calom )


        if self.storeGateSvc.record( goodMuons, self.outCaloColl ) != StatusCode.Success :
            self.msg.error( 'Could not record the goodMuons into StoreGate with the key = ' % self.outCaloColl )
            pass

        self.msg.debug( '%s calo muons surviving the overlap removal (were %s)'% (len(goodMuons),len(caloMuonCollection)))

        #print "dumping good muons"
        #for m in goodMuons:
        #    print m.eta(),m.phi(),m.pt(),m.track()

        return StatusCode.Success


    def hasSharedTrack(self,track,combcollections):

        for combc in combcollections:
            
                for combm in combc:

                    if track == combm.track():
                        return True
        return False
    
    def deltaROverlap(self,cut,muon,combcollections):
        for combc in combcollections:
            
                for combm in combc:

                    deltaphi=muon.phi()-combm.phi()
                    deltaeta=muon.eta()-combm.eta()
                    deltaR=sqrt(deltaeta*deltaeta+deltaphi*deltaphi)
                    if deltaR<cut:
                        return True
        return False
