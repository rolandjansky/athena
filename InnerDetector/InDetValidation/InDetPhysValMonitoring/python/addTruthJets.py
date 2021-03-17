# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


def addTruthJetsIfNotExising(truth_jets_name):
    '''
    Add algorithm to create the truth jets collection unless the
    collection exists already, or a truth jet finder is already running
    '''
    from RecExConfig.AutoConfiguration import IsInInputFile

    # the jet collection name does not exist in the input file
    # add a jet finder algorithm in front of the monitoring if the algorithm
    # does not yet exist.
    if not IsInInputFile('xAOD::JetContainer', truth_jets_name):
        try:
            from AthenaCommon.Logging import logging
            log = logging.getLogger('InDetPhysValMonitoring/addTruthJets.py')

            from PyUtils.MetaReaderPeeker import convert_itemList, metadata
            eventdata_itemsDic = convert_itemList(layout='dict')
            log.info('DEBUG addTruthJetsIfNotExising {} not in {} [file_type={}]'.format(
                truth_jets_name, eventdata_itemsDic, metadata['file_type']))

            if truth_jets_name in eventdata_itemsDic:
                return
        except Exception:
            pass

        # Access the algorithm sequence:
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        # extract the jet finder type and main parameter
        import re
        extract_alg = re.search('^([^0-9]+)([0-9]+)TruthJets', truth_jets_name)
        if extract_alg is not None:
            alg_type = extract_alg.group(1)
            alg_param_str = extract_alg.group(2)
        else:
            alg_type = 'AntiKt'
            alg_param_str = 4

        jet_finder_alg_name = "jetalg"+alg_type+alg_param_str+'TruthJets'

        # add the jet finder unless it exists already in the alg sequence
        from InDetPhysValDecoration import findAlg, findMonMan
        alg_pos = findAlg([jet_finder_alg_name])
        if alg_pos is None:
            from JetRec.JetRecStandard import jtm
            mon_man_index = findMonMan()

            # configure truth jet finding ?
            from JetRec.JetRecFlags import jetFlags
            jetFlags.useTruth = True
            jetFlags.useTracks = False
            jetFlags.truthFlavorTags = ["BHadronsInitial", "BHadronsFinal", "BQuarksFinal",
                                        "CHadronsInitial", "CHadronsFinal", "CQuarksFinal",
                                        "TausFinal",
                                        "Partons",
                                        ]

            # tool to create truth jet finding inputs
            truth_part_copy_name = 'truthpartcopy'
            dir(jtm)
            create_truth_jet_input = None
            if not hasattr(jtm, truth_part_copy_name):

                from MCTruthClassifier.MCTruthClassifierConfig import firstSimCreatedBarcode
                from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
                truth_classifier_name = 'JetMCTruthClassifier'
                if not hasattr(jtm, truth_classifier_name):
                    from AthenaCommon.AppMgr import ToolSvc
                    if not hasattr(ToolSvc, truth_classifier_name):
                        truthClassifier = MCTruthClassifier(
                            name=truth_classifier_name,
                            barcodeG4Shift=firstSimCreatedBarcode(),
                            ParticleCaloExtensionTool="")
                    else:
                        truthClassifier = getattr(
                            ToolSvc, truth_classifier_name)
                        truthClassifier.barcodeG4Shift = firstSimCreatedBarcode()
                    jtm += truthClassifier
                else:
                    truthClassifier = getattr(jtm, truth_classifier_name)
                    truthClassifier.barcodeG4Shift = firstSimCreatedBarcode()

                from ParticleJetTools.ParticleJetToolsConf import CopyTruthJetParticles
                create_truth_jet_input = CopyTruthJetParticles(truth_part_copy_name, OutputName="JetInputTruthParticles",
                                                               MCTruthClassifier=truthClassifier)
                jtm += create_truth_jet_input
            else:
                create_truth_jet_input = getattr(jtm, truth_part_copy_name)

            jet_tools = []
            from JetRec.JetFlavorAlgs import scheduleCopyTruthParticles
            jet_tools += scheduleCopyTruthParticles()
            jet_tools += [create_truth_jet_input]
            jet_tools += jtm.jetrecs

            # add the jet finder in front of the monitoring
            from JetRec.JetRecConf import JetAlgorithm
            from JetRec.JetRecConf import JetToolRunner
            jtm += JetToolRunner("jetrun",
                                 Tools=jet_tools,
                                 EventShapeTools=[],
                                 # OutputLevel = 1,
                                 Timer=jetFlags.timeJetToolRunner()
                                 )

            # jet_finder_alg = JetAlgorithm(jet_finder_alg_name, jet_tools)
            jet_finder_alg = JetAlgorithm(jet_finder_alg_name)
            # jet_finder_alg.OutputLevel = 1
            jet_finder_alg.Tools = [jtm.jetrun]

            if mon_man_index is not None:
                topSequence.insert(mon_man_index, jet_finder_alg)
            else:
                topSequence += jet_finder_alg
