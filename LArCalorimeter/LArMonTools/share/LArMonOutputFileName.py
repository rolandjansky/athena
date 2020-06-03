
rootHisto="RootHistos"
if 'OutName' in dir():
        rootHisto='RootHistos_%(stuff)s' % {"stuff" : OutName}


if not online:
    if not runAccumulator:
        if not 'RootHistOutputFileName' in dir():
            if Type == 'Delay':
                RootHistOutputFileName='%(dir)s/%(rootHisto)s_%(No)d_Wave_avg_%(partition)s.root' % {"dir" : OutputDir, "rootHisto" : rootHisto, "No" : RunNumber, "partition" : Partition}
            elif Type == 'Pedestal':
                RootHistOutputFileName='%(dir)s/%(rootHisto)s_%(No)d_%(Type)s_%(partition)s.root' % {"dir" : OutputDir, "rootHisto" : rootHisto, "No" : RunNumber, "Type" : Type, "partition" : Partition}
            elif Type == 'Cosmic':
                RootHistOutputFileName='%(dir)s/%(rootHisto)s_%(No)d_%(Type)s.root' % {"dir" : OutputDir, "rootHisto" : rootHisto, "No" : RunNumber, "Type" : Type}
            else:
                RootHistOutputFileName='%(dir)s/%(rootHisto)s_%(No)d_%(Type)s_avg_%(partition)s.root' % {"dir" : OutputDir, "rootHisto" : rootHisto, "No" : RunNumber, "Type" : Type, "partition" : Partition}
        if Type == "Pedestal":
            if not 'RootHistReferenceFileName' in dir():
                RootHistReferenceFileName='%(dir)s/RootHistos_%(No)d_%(Type)s_%(partition)s.root' % {"dir" : OutputDir, "No" : RefRunNumber, "Type" : Type, "partition" : Partition}
        if Type == "Cosmic":
            if not 'RootHistReferenceFileName' in dir():
                RootHistReferenceFileName='%(dir)s/RootHistos_%(No)d_%(Type)s_%(partition)s.root' % {"dir" : OutputDir, "No" : RefRunNumber, "Type" : Type, "partition" : Partition}
        if Type == 'Delay':
            if not 'DelayOutputFile' in dir():
                DelayOutputFile = '%(dir)s/RootHistos_%(No)d_%(Type)s_avg_%(partition)s.root' % {"dir" : OutputNtupleDir, "No" : RunNumber, "Type" : Type, "partition" : Partition}
    else:
        if Type == 'Delay':
            if not 'RootHistOutputFileName' in dir():
                RootHistOutputFileName = '%(dir)s/%(rootHisto)s_%(No)d_%Wave_trans_%(partition)s.root' % {"dir" : OutputDir, "rootHisto" : rootHisto, "No" : RunNumber, "Type" : Type, "partition" : Partition}
            if not 'DelayOutputFile' in dir():
                DelayOutputFile = '%(dir)s/RootHistos_%(No)d_%(Type)s_trans_%(partition)s.root' % {"dir" : OutputNtupleDir, "No" : RunNumber, "Type" : Type, "partition" : Partition}
        elif Type == 'Pedestal':
            if not 'RootHistOutputFileName' in dir():
                RootHistOutputFileName='%(dir)s/%(rootHisto)s_%(No)d_%(Type)s_%(partition)s.root' % {"dir" : OutputDir, "rootHisto" : rootHisto, "No" : RunNumber, "Type" : Type, "partition" : Partition}
            if not 'RootHistReferenceFileName' in dir():
                RootHistReferenceFileName='%(dir)s/RootHistos_%(No)d_%(Type)s_%(partition)s.root' % {"dir" : OutputDir, "No" : RefRunNumber, "Type" : Type, "partition" : Partition}
        elif Type == 'Cosmic':
            if not 'RootHistOutputFileName' in dir():
                RootHistOutputFileName='%(dir)s/%(rootHisto)s_%(No)d_%(Type)s.root' % {"dir" : OutputDir, "rootHisto" : rootHisto, "No" : RunNumber, "Type" : Type}
            if not 'RootHistReferenceFileName' in dir():
                RootHistReferenceFileName='%(dir)s/RootHistos_%(No)d_%(Type)s.root' % {"dir" : OutputDir, "No" : RefRunNumber, "Type" : Type}
        else:
            if not 'RootHistOutputFileName' in dir():
                RootHistOutputFileName='%(dir)s/%(rootHisto)s_%(No)d_%(Type)s_trans_%(partition)s.root' % {"dir" : OutputDir, "rootHisto" : rootHisto, "No" : RunNumber, "Type" : Type, "partition" : Partition}
