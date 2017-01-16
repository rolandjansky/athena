# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ViewAlgsTest.Menu import *
import AthenaCommon.AtlasUnixStandardJob

from ViewAlgs.ViewAlgsConf import *
from ViewAlgsTest.ViewAlgsTestConf import *
from ViewAlgsTest.connectAlgorithmsIO import *
from L1Decoder.L1DecoderConf import FakeRoI, FakeCTP
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
from GaudiKernel.Configurable import ConfigurableAlgorithm



def makeChains():
    chains = ChainsList()

    chains + Chain("HLT_e3",    "L1_EM3", 1, [("job/activeRoIsAfterPrescaling", "EM3 x 1"), 
                                              ("L2CaloSeq/L2CaloDecision", "L2_3GeVCaloEtCut x 1"),
                                              ("EFTrackSeq/EFCaloDecision", "EF_3GeVCaloEtCut x 1")])
                                                  
    chains + Chain("HLT_e3merged", "L1_EM3", 1, [("job/activeRoIsAfterPrescaling", "EM3 x 1"), 
                                                 ("L2CaloSeq/L2CaloDecision", "L2_3GeVCaloEtCut x 1"), 
                                                 ("EFTrackSeq/CaloTrackingDecision", "EF_3GeVCaloEtCut x 1")]) 

    chains + Chain("HLT_2e3", "L1_2EM3", 1, [("job/activeRoIsAfterPrescaling", "EM3 x 2"), 
                                             ("L2CaloSeq/L2CaloDecision", "L2_3GeVCaloEtCut x 2"),
                                             ("EFTrackSeq/EFCaloDecision", "EF_3GeVCaloEtCut x 2")])

    chains + Chain("HLT_mu6", "L1_MU6", 1, [("job/activeRoIsAfterPrescaling", "MU6 x 1"), 
                                            ("L2MuonSeq/L2MuonDecision", "L2_6GeVMuonCut x 1")])


    chains + Chain("HLT_mu6e3_merged", "L1_EM7,L1_MU6", 1, [("job/activeRoIsAfterPrescaling", "EM3 x 1, MU6 x 1"), 
                                                            ("job/L2ComboDecision", "L2_3GeVCaloEtCut x 1, L2_6GeVMuonCut x 1")])


        # add here other chains and the corresponding sequences in the section below

    print "\n List of Chains: "
    for chain in chains:
        print " - Chain: %s  %s "%(chain.name, chain.requirements)
    print "\n"

    return chains


def makeHLTSequences():
    HLTSequences = {}

    # L1 emualtion sequences:
    HLTSequences["activeRoIsAfterPrescaling"] = HLTSequence("activeRoIsAfterPrescaling",  inc=[], outc=[], tasks=[
        HLTTask("data/caloRoIData.dat", "FakeRoI/caloFakeRoI.InputFilename"),
        HLTTask("data/muonRoIData.dat", "FakeRoI/muonFakeRoI.InputFilename"),
        HLTTask("data/menuData.dat", "FakeCTP/FakeCTP.InputFilename"),
        HLTTask("FakeCTP/FakeCTP.OutputDecisions",  "PrescaleDecision/HLTChainsPrescaling.InputChainDecisions"),
        HLTTask("PrescaleDecision/HLTChainsPrescaling.OutputChainDecisions", "DecisionAlg/activeRoIsAfterPrescaling.InputChainDecisions"),
        HLTTask("FakeRoI/caloFakeRoI.OutputDecisions", "DecisionAlg/activeRoIsAfterPrescaling.InputHypoDecisions"),
        HLTTask("FakeRoI/muonFakeRoI.OutputDecisions", "DecisionAlg/activeRoIsAfterPrescaling.InputHypoDecisions"),
        HLTTask("FakeRoI/caloFakeRoI.OutputDecisions", "CopyPassing/copyCaloRoIsPassingPrecaling.CollectionBeforeMenu"),
        HLTTask("FakeRoI/muonFakeRoI.OutputDecisions", "CopyPassing/copyMuonRoIsPassingPrecaling.CollectionBeforeMenu"),
        HLTTask("DecisionAlg/activeRoIsAfterPrescaling.OutputHypoDecisions", "CopyPassing/copyCaloRoIsPassingPrecaling.CollectionAfterMenu"),
        HLTTask("DecisionAlg/activeRoIsAfterPrescaling.OutputHypoDecisions", "CopyPassing/copyMuonRoIsPassingPrecaling.CollectionAfterMenu") ])

        #    now starting HLT sequences:
    HLTSequences["L2CaloDecision"] = HLTSequence("L2CaloDecision",
                                                 inc=[],
                                                 outc=  ["TestHypoAlg/L2CaloHypo.OutputDecisions"],
                                                 tasks= [ HLTTask("CopyPassing/copyCaloRoIsPassingPrecaling.Output", "TestFEXAlg/L2CaloReco.RoIsContainer"),
                                                          HLTTask("TestFEXAlg/L2CaloReco.OutputProxyContainer", "TestHypoAlg/L2CaloHypo.InputProxyContainer"),
                                                          HLTTask("TestHypoAlg/L2CaloHypo.OutputDecisions", "DumpDecisions/L2CaloRoIsDumper.OutputDecisions")] )


    
    HLTSequences["EFCaloDecision"] = HLTSequence("EFCaloDecision",
                                                 inc =["TestFEXAlg/EFCaloReco.RoIsContainer"],
                                                 outc=["TestHypoAlg/EFCaloHypo.OutputDecisions"],
                                                 tasks= [ HLTTask("TestFEXAlg/EFCaloReco.OutputProxyContainer", "TestHypoAlg/EFCaloHypo.InputProxyContainer" ),
                                                          HLTTask("TestHypoAlg/EFCaloHypo.OutputDecisions", "DumpDecisions/EFCaloRoIsDumper.OutputDecisions")])
    
    HLTSequences["L2MuonDecision"] = HLTSequence("L2MuonDecision",
                                                 inc=[],
                                                 outc=["TestHypoAlg/L2MuonHypo.OutputDecisions"],
                                                 tasks =[ HLTTask("CopyPassing/copyMuonRoIsPassingPrecaling.Output", "TestFEXAlg/L2MuonReco.RoIsContainer"),
                                                          HLTTask("TestFEXAlg/L2MuonReco.OutputProxyContainer", "TestHypoAlg/L2MuonHypo.InputProxyContainer")])#,
        #                                                          HLTTask("TestHypoAlg/L2MuonHypo.OutputDecisions", "DumpDecisions/L2MuonRoIsDumper.OutputDecisions"] )

    HLTSequences["EFMuonDecision"] = HLTSequence("EFMuonDecision",
                                                 inc=["TestFEXAlg/EFMuonReco.RoIsContainer"],
                                                 outc=["TestHypoAlg/EFMuonHypo.OutputDecisions"],
                                                 tasks= [HLTTask("TestFEXAlg/EFMuonReco.OutputProxyContainer", "TestHypoAlg/EFMuonHypo.InputProxyContainer" )])

    HLTSequences["L2ComboDecision"] = HLTSequence("L2ComboDecision",
                                                  inc=[],
                                                  outc=[
                                                        "TestHypoAlg/L2CaloHypo.OutputDecisions",
                                                        "TestHypoAlg/L2MuonHypo.OutputDecisions"],
                                                  tasks=[])
    
    HLTSequences["CaloTrackingDecision"] = HLTSequence("CaloTrackingDecision",
                                                     inc=["TestMergeAlg/MergeAllRoIs.InputCollections","TestSplitAlg/SplitCaloRoIs.ReferenceProxyContainer"],
                                                     outc=["TestHypoAlg/CaloMatchingHypo.OutputDecisions"],
                                                     tasks= [HLTTask("TestMergeAlg/MergeAllRoIs.MergedProxyContainer", "TestFEXAlg/MergedTracking.RoIsContainer" ),
                                                             HLTTask("TestFEXAlg/MergedTracking.OutputProxyContainer", "TestSplitAlg/SplitCaloRoIs.InputProxyContainer"),
                                                             HLTTask("TestSplitAlg/SplitCaloRoIs.SplitProxyContainer", "TestFEXAlg/CaloMatchingFex.RoIsContainer"),
                                                             HLTTask("TestFEXAlg/CaloMatchingFex.OutputProxyContainer", "TestHypoAlg/CaloMatchingHypo.InputProxyContainer" ),
                                                             HLTTask("TestHypoAlg/CaloMatchingHypo.OutputDecisions", "DumpDecisions/CaloTrackingDumper.OutputDecisions") ])

     
    print "\n List of HLTSequences: "
    for key, seq in HLTSequences.items():
        print seq
    print "\n"
    return HLTSequences



def makeAthenaSequences(job):
 #make gaudi sequences
    from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
    AthenaSequences = {}
    
    runGaudiSequence = False
    if runGaudiSequence:
        job += AthSequencer("L2Step")
        #job.L2Step.setFilterPassed(L2CaloSeq._cppHandle.filterPassed() | L2MuonSeq._cppHandle.filterPassed())
        L2CaloSeq = AthSequencer("L2CaloSeq")
        job.L2Step +=L2CaloSeq
        L2MuonSeq = AthSequencer("L2MuonSeq")
        job.L2Step +=L2MuonSeq

        job += AthSequencer("EFStep")
        EFCaloSeq = AthSequencer("EFCaloSeq")
        job.EFStep +=EFCaloSeq
        EFTrackSeq = AthSequencer("EFTrackSeq")
        job.EFStep += EFTrackSeq
    
    else:
        L2MuonSeq = job
        L2CaloSeq = job
        L2MuonSeq = job
        EFTrackSeq = job
        EFCaloSeq = job
        EFTrackSeq = job

    AthenaSequences["job"]=job
    AthenaSequences["L2CaloSeq"]=L2CaloSeq
    AthenaSequences["L2MuonSeq"]=L2MuonSeq
        
    AthenaSequences["EFCaloSeq"]=EFCaloSeq
    AthenaSequences["EFTrackSeq"]=EFTrackSeq


    
    print "\n List of AthenaSequences: "
    for key, seq in AthenaSequences.items():
        print key
    print "\n"

    return AthenaSequences



def makeMenu(job, chains, AthenaSequences, HLTSequences, outputlevel):

    # first  run the fake L1
    print "Make L1 sequence"
    L1sequences={HLTSequences["activeRoIsAfterPrescaling"]}
    for L1seq in L1sequences:
        assert L1seq, "Missing L1 sequence "
        # create/get decision algorithm
        decAlg = genDecisionAlg(L1seq.name)
        for task in L1seq.tasks:    
            Alg_in, Alg_out = task.connect()           
            if not hasattr(job,Alg_in.name()):
                job += Alg_in            

            if Alg_out:
                if not hasattr(job,Alg_out.name()):
                    job += Alg_out

                
    print "Now make HLT chains"
    for chain in chains:
        print "- Chain: %s  %s "%(chain.name, chain.requirements)
        psAlg = getattr(job, "HLTChainsPrescaling")
        psAlg.Prescales.append( "%s %f" % (chain.name, chain.ps) )
        prevDecAlg= None

        for (seqAlg, req) in chain.requirements:
            seq, alg = seqAlg.split('/')        

             # get HLTsequences
            HLTseq = HLTSequences.get(alg)
            assert HLTseq, "Missing HLT sequence "+str(alg)            

            # get AthenaSequences
            sequence = AthenaSequences.get(seq)
            assert sequence, "Missing Athena sequence "+str(seq)

            
            # create/get decision algorithm
            decAlg = genDecisionAlg(alg)
            decAlg.Required += [chain.name + " = " + req]
            decAlg.OutputLevel=outputlevel

            # skip if already done
            if hasattr(sequence,alg):
                prevDecAlg = decAlg
                continue

            if "AfterPrescaling" in seqAlg:
                prevDecAlg = decAlg
                continue

            sequence += decAlg
            print "    making sequence %s -> %s %s "%(seqAlg, seq,alg)            

            
            # make connections between tasks            
            for task in HLTseq.tasks: 
                print "      making task: ",task
                Alg_in, Alg_out =task.connect()
                
                if not hasattr(sequence,Alg_out.name()):
                    print "    algo %s added to sequence %s"%(Alg_out.name(),seq)
                    sequence += Alg_out
                Alg_out.OutputLevel=outputlevel

                if not hasattr(sequence,Alg_in.name()):
                    print "     algo %s added to sequence %s"%(Alg_in.name(),seq)
                    sequence += Alg_in

                Alg_in.OutputLevel=outputlevel
                
            # connect to decisions ****
            
            # seqin connected to previous decision
            HLTseq.connectInput(prevDecAlg)
          
            # seqout connected to this decision
            HLTseq.connectOutput(decAlg)

            # connect Decision to last Decision 
            if prevDecAlg is not None :
                connectAlgorithmsIO(consumer=(decAlg, "InputChainDecisions"), producer=(prevDecAlg, "OutputChainDecisions"))
           
            print "Done sequence %s"%seq
            prevDecAlg = decAlg

    print "Menu completed"



def MenuMaker(job,outputlevel):
   
    chains = makeChains()
    HLTSequences = makeHLTSequences()
    AthenaSequences = makeAthenaSequences(job)
    makeMenu(job, chains, AthenaSequences, HLTSequences,outputlevel)
    
    

class HLTSequence:
    def __init__(self, name, inc=[], outc=[], tasks=[]):
        self.name = name
        self.input = inc
        self.output = outc
        self.tasks = tasks

    def __str__(self):
        return "HLTSequence " + self.name + ": \n  " + ", ".join( str(x) for x in self.input) + " \n  -> [ \n  " + ",\n  ".join( str(x) for x in self.tasks) + "\n     ] \n  -> " + ", ".join( str(x) for x in self.output)

    def connectInput(self, prevDecAlg):
        if self.input:
            for inc in self.input:
                seqin, seqin_con = inc.split('.')
                seqclass_in, seqalg_in =  seqin.split('/')
                if prevDecAlg is not None:
                    Alg_in_seq = eval(seqclass_in)(seqalg_in)
                    assert Alg_in_seq, "Missing in algo "+str(Alg_in_seq.name())
                     #                    print "connectAlgorithmsIO(consumer=("+Alg_in_seq.name()+", "+seqin_con +"), producer=("+prevDecAlg.name()+", OutputHypoDecisions))"
                    connectAlgorithmsIO(consumer=(Alg_in_seq, seqin_con), producer=(prevDecAlg, "OutputHypoDecisions"))

    def connectOutput(self,decAlg):
        if self.output:
            for outc in self.output:
                seqout, seqout_con = outc.split('.')
                seqclass_out, seqalg_out =  seqout.split('/')
                if decAlg is not None:
                    Alg_out_seq = eval(seqclass_out)(seqalg_out)
                    assert Alg_out_seq, "Missing out algo "+str(Alg_out_seq.name())
                    #                    print "connectHypoToMenu(consumer="+decAlg.name()+", producer=("+Alg_out_seq.name()+", "+seqout_con+"))"
                    connectHypoToMenu(consumer=decAlg, producer=(Alg_out_seq, seqout_con))


class HLTAlgo:
    def __init__(self, name, inc, outc):
        self.name = name
        self.input = inc
        self.output = outc

    def __str__(self):
        return self.name + "(" + self.input + "->" + self.output + ")"
    

class HLTTask:
    def __init__(self, outc, inc):
        self.output = outc
        self.input = inc
        self.inputAlg, self.inputLine = self.input.split('.')
        self.outputAlg, self.outputLine = self.output.split('.')

    def __str__(self):
        return  "(" + self.output + "->" + self.input + ")"

    def connect(self):
        print "      making task: ",self
        fexin, fexin_con   = self.input.split('.')
        fexout, fexout_con = self.output.split('.')
        print fexin,fexin_con,fexout,fexout_con                

        fexclass_out, fexalg_out =  fexout.split('/')
        Alg_out=None
        if str(fexclass_out) != "data":
            Alg_out = eval(fexclass_out)(fexalg_out)                
            assert Alg_out, "Missing out algo "+str(fexalg_out)

        fexclass_in, fexalg_in =  fexin.split('/')
        #Alg = __import__(fexclass_in)
        Alg_in = eval(fexclass_in)(fexalg_in)
        assert Alg_in, "Missing in algo "+str(fexalg_in)
        if Alg_out is not None:
            connectAlgorithmsIO(consumer=(Alg_in, fexin_con), producer=(Alg_out, fexout_con) )            
        else:
            (fexout1, fexout2) = self.output.split('/')
            filename = fexout2  
            setattr(Alg_in, fexin_con,filename)

        return (Alg_in, Alg_out)
    
