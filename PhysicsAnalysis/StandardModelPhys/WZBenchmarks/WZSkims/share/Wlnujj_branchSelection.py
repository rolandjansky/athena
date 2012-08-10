# Set up a logger:
from AthenaCommon.Logging import logging
Wlnujj_msg = logging.getLogger( 'Wlnujj_branchSelection' )
if 'SkimOL' in dir():
    Wlnujj_msg.setLevel(SkimOL)
    
# name of the stream which should be modified
if 'streamName' in dir():
    Wlnujj_msg.info("Looking for output stream with name '%s' " % streamName)
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    try:
        SkimD3PDStream = MSMgr.GetStream(streamName)
    except:
        Wlnujj_msg.error("Output stream with name '%s' does not exist " % streamName)
            
# do all manipulations only if stream is defined
if 'SkimD3PDStream' in dir():

    #
    # additional fillers for W->lnujj stream
    #

    def _args (level, name, kwin, **kw):
        kw = kw.copy()
        kw['level'] = level
        for (k, v) in kwin.items():
            if k.startswith (name + '_'):
                kw[k[len(name)+1:]] = v
        return kw

    def WlnujjD3PD (alg = None,
                    doTrackJet = True,
                    **kw):

        # never create new alg - use existing one
        if alg:
            # extra blocks with TrackJets
            if doTrackJet:

                # trrack jets from QcdD3PDMaker do not work in rel 17.0.6.4.11 and later
                #include ("QcdD3PDMaker/TrackJet_jobOptions.py")
                # setting empty TrackSummaryTool to avoid error during initialization
                #ToolSvc.MyDetailedTrackSelectorTool.TrackSummaryTool = ""
        
                level = 2; # level = 3 is crashing for the moment
                Wlnujj_msg.info("TrackJets are added to D3PD")
                Wlnujj_msg.info("Level for TrackJets in D3PD is %d" % level)

                # this filtering doesn't work - commented out
                #from D2PDMaker.D2PDMakerConf import D2PDJetSelector
                #from AthenaCommon.AlgSequence import AlgSequence
                #preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
                #preseq += D2PDJetSelector( "SMWZ_HighPtJetTZ4Filter",
                #                           inputCollection      = 'AntiKt4TrackZJets',
                #                           outputCollection     = 'HighPtJetsTZ4',
                #                           ptMin                = 20.0*Units.GeV )

                #from JetD3PDMaker.JetD3PDObject import JetD3PDObject
                #alg += JetD3PDObject (**_args ( level, 'AntiKt4TrackJetsZ', kw,
                #                                sgkey = 'AntiKt4TrackZJets', prefix = 'jet_AntiKt4TrackZ_',
                #                                JetVertexFraction_FromUD=True,
                #                                JetVertexFraction_FillFullJVF=True,
                #                                allowMissing = True))

                include("TopInputsD3PDMaker/PrepareTrackJets.py")
                from JetD3PDMaker.JetD3PDObject import JetD3PDObject
                alg += JetD3PDObject (**_args ( level, 'AntiKt4TrackJetsZ', kw,
                                                sgkey = 'AntiKtZ4TrackJets', prefix = 'jet_AntiKt4TrackZ_',
                                                JetVertexFraction_FromUD=True,
                                                JetVertexFraction_FillFullJVF=True,
                                                allowMissing = True))

        return alg

    # adding TrackJets by default
    if not 'doTrackJet' in dir() or doTrackJet:
        TheAlg = WlnujjD3PD(SkimD3PDStream, doTrackJet=True)

    #
    # end of additional fillers block
    #

    #
    # removing some fillers in W->lnujj stream
    #

    # removal of top-level fillers
    #
    # removal of AntiKt6 LC jets:
    exclusion=['jet_AntiKt6LC','jet_antikt6LC','jet_AntiKt6Track']
    # removal of photons, taus, clusters, extra muon collections:
    exclusion+=['ph_','mu_muid_','mu_calo_','tau_','hr_','cl_','clpt10_','emclpt10_']
    # removal of tracks (if needed):
    #exclusion+=['trk_','trkpt4_']
    # trigger:
    exclusion+=['trig_roidescriptor_',
                'trig_L1_esum_','trig_L1_jet_',
                'trig_RoI_L2_b_','trig_RoI_L2_TileMu_',
                'trig_RoI_EF_b_','trig_RoI_EF_TileMu_',
                'trig_L2_idscan_','trig_L2_sitrack_','trig_L2_trk_','trig_L2_tile',
                'trig_L2_bjet_','trig_L2_emcl_','trig_L2_ph_','trig_L2_met_',
                'trig_EF_bjet_','trig_EF_emcl_','trig_EF_ph_','trig_EF_met_']

    # removal of private fillers from top-level fillers
    #
    exclist={'el_':['RawCluster','EMTrackFit'],'mu_staco_':['MuonTiming']}

    # keep only few trigger bits
    #
    # keep L1,L2,EF bits for single electron and single muon only
    triglist={'TriggerBits':['L1_E.*','L1_MU.*','L2_e.*','L2_mu.*','EF_e.*','EF_mu.*'],'MBTSTriggerBits':[]}
    #all bits: 'L1','L2','EF_e','EF_mu','EF_L1','EF_b','EF_g','EF_j','EF_tau','tau','EF_2','EF_3','EF.*_xe','EF.*_XE','EF_.*'

    Wlnujj_msg.debug("Modifying %s D3PD stream with %d filler tools" % (SkimD3PDStream.name(),len(SkimD3PDStream.Tools)))

    nrm=0
    nch=0
    nst=0
    ntr=0
    for tool in reversed(SkimD3PDStream.Tools):
        name=tool.name()
        tool2=vars(SkimD3PDStream.Stream)[name]
        deja=False
        
        for key in exclusion:
            if name.startswith(key):
                nrm+=1
                Wlnujj_msg.debug("removing %s",name)
                del SkimD3PDStream.Tools[name]
                del tool2
                deja=True
                break
        if deja: continue

        for key in exclist:
            if name.startswith(key):
                Wlnujj_msg.debug("changing %s",name)
                for block in reversed(tool.BlockFillers):
                    for exc in exclist[key]:
                        if exc in block.name():
                            nst+=1
                            Wlnujj_msg.debug("  removing sub-tool %s",block.name())
                            try:
                                del tool.BlockFillers[block.name()]
                                del tool2.BlockFillers[block.name()]
                            except:
                                Wlnujj_msg.debug("  removing of sub-tool %s failed - continue anyway",block.name())
                            deja=True
                if deja:
                    nch+=1
                else:
                    Wlnujj_msg.debug("  nothing changed")
                break

        for key in triglist:
            if name.startswith(key):
                ntr+=1
                Wlnujj_msg.debug("changing %s",name)
                for block in tool.BlockFillers:
                    Wlnujj_msg.debug("    changing %s",block.name())
                    Wlnujj_msg.debug("    old trigger bits: %s",block.Triggers)
                    block.Triggers[:]=triglist[key]
                    tool2.BlockFillers[block.name()].Triggers[:]=triglist[key]
                    Wlnujj_msg.debug("    keeping trigger bits: %s",block.Triggers)

    Wlnujj_msg.debug("%d tools remains, %d removed, %d sub-tools removed in %d tools, %d triggers changed" % (len(SkimD3PDStream.Tools),nrm,nst,nch,ntr))
    for tool in SkimD3PDStream.Tools:
        Wlnujj_msg.verbose("%s" % tool.name())
        for block in tool.BlockFillers:
            Wlnujj_msg.verbose("    %s" % block.name())

    #
    # end of removal block
    #

else:
    Wlnujj_msg.error("Output stream not defined - nothing to do")
