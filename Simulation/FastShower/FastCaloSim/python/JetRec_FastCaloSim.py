# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from JetRec.JetGetters import make_StandardJetGetter
from JetRec.JetRecFlags import jetFlags

def make_FastCaloSimJets(doTowerJet = True, doTopoJet = True, doTruthJet = True, doLocalCalib = False ,
                      disableOpt=False, usingOldname=False, doTruthPileup = False):

    #useOldJetNames(usingOldname) # maybe usefull for transition period / since 14 obsolete
    
    if doTowerJet:
        # Build CaloTower        

        # Cone Jet from CaloTower
        make_StandardJetGetter('Cone',0.7,'H1Tower',disable=disableOpt, doCalib = True, doAtlfastII = True)
        make_StandardJetGetter('Cone',0.4,'H1Tower',disable=disableOpt, doCalib = True, doAtlfastII = True)
                
        make_StandardJetGetter('Kt',0.6,'H1Tower',disable=disableOpt, doCalib = True, doAtlfastII = True)
        make_StandardJetGetter('Kt',0.4,'H1Tower',disable=disableOpt, doCalib = True, doAtlfastII = True)
        
                
    if doTopoJet:
        # Cone Jet from TopoCluster
        make_StandardJetGetter('Cone',0.7,'H1Topo',disable=disableOpt, doCalib = True, doAtlfastII = True)
        make_StandardJetGetter('Cone',0.4,'H1Topo',disable=disableOpt, doCalib = True, doAtlfastII = True) 
        # Kt Jet from TopoCluster
        make_StandardJetGetter('Kt',0.6,'H1Topo',disable=disableOpt, doCalib = True, doAtlfastII = True)
        make_StandardJetGetter('Kt',0.4,'H1Topo',disable=disableOpt, doCalib = True, doAtlfastII = True)
        

    from RecExConfig.RecFlags import rec
    if doTruthJet and rec.doTruth():
        # Cone Jet from Generator Particles
        make_StandardJetGetter('Cone',0.7,'Truth',disable=disableOpt) 
        make_StandardJetGetter('Cone',0.4,'Truth',disable=disableOpt) 
        
        # Kt Jet from Generator Particles
        make_StandardJetGetter('Kt',0.6,'Truth',disable=disableOpt)
        make_StandardJetGetter('Kt',0.4,'Truth',disable=disableOpt)

        if doTruthPileup:
            # Cone Jet from Generator Particles
            make_StandardJetGetter('Cone',0.7,'Truth',disable=disableOpt,doTruthPileup=True,inputSuff="Pileup") 
            make_StandardJetGetter('Cone',0.4,'Truth',disable=disableOpt,doTruthPileup=True,inputSuff="Pileup") 
            
            # Kt Jet from Generator Particles
            make_StandardJetGetter('Kt',0.6,'Truth',disable=disableOpt,doTruthPileup=True,inputSuff="Pileup") 
            make_StandardJetGetter('Kt',0.4,'Truth',disable=disableOpt,doTruthPileup=True,inputSuff="Pileup") 
    if doLocalCalib :
        jetFlags.recordLCjets.set_Value(False) # by default this is not wanted
        #recordLCjetsESD(False) 
        make_StandardJetGetter('Cone',0.7,'LCTopo',disable=disableOpt)
        make_StandardJetGetter('Cone',0.4,'LCTopo',disable=disableOpt)
        make_StandardJetGetter('Kt',0.6,'LCTopo',disable=disableOpt)
        make_StandardJetGetter('Kt',0.4,'LCTopo',disable=disableOpt)

            
            
