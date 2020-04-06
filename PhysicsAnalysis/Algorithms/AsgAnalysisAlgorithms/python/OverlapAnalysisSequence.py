# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeOverlapAnalysisSequence( dataType,
                                 inputLabel = '', outputLabel = 'passesOR',
                                 linkOverlapObjects = False, doMuPFJetOR=False,
                                 doEleEleOR = False, doElectrons = True,
                                 doMuons = True, doJets = True, doTaus = True,
                                 doPhotons = True, doFatJets = False,
                                 bJetLabel = '',
                                 boostedLeptons = False,
                                 postfix = '',
                                 enableCutflow = False ):
    """Function creating the overlap removal algorithm sequence

    The function sets up a multi-input/multi-output analysis algorithm sequnce,
    which needs to be used in a quite particular way. First off you need to set
    the arguments of this function correctly.

    Then, you need to call the configure(...) method on the algorithm sequence
    returned by this function in the following way:

      overlapSequence.configure(
         inputName = {
            'electrons' : 'AnalysisElectrons_%SYS%',
            'photons'   : 'AnalysisPhotons_%SYS%',
            'muons'     : 'AnalysisMuons_%SYS%',
            'jets'      : 'AnalysisJets_%SYS%',
            'taus'      : 'AnalysisTauJets_%SYS%' },
         outputName = {
            'electrons' : 'AnalysisElectronsOR_%SYS%',
            'photons'   : 'AnalysisPhotonsOR_%SYS%',
            'muons'     : 'AnalysisMuonsOR_%SYS%',
            'jets'      : 'AnalysisJetsOR_%SYS%',
            'taus'      : 'AnalysisTauJetsOR_%SYS%' },
         affectingSystematics = {
            'electrons' : '(^$)|(^EG_.*)|(^EL_.*)',
            'photons'   : '(^$)|(^EG_.*)|(^PH_.*)',
            'muons'     : '(^$)|(^MUON_.*)',
            'jets'      : '(^$)|(^JET_.*)',
            'taus'      : '(^$)|(^TAUS_.*)' } )

    Where:
      - You need to provide input and output names in pairs, you must not skip
        specifying an output name if you specified an input name, and vice
        versa.
      - You only define inputs/outputs that your analysis uses. The "labels" of
        the possible inputs/outputs are: "electrons", "photons", "muons",
        "jets", "taus" and "fatJets".
      - You have to define with affectingSystematics which systematic variations
        are affecting the containers you passed to the sequence as inputs. If
        left empty, the configuration assumes that no systematic variation is
        affecting the input(s).

    Function keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      inputLabel -- Any possible label to pick up the selected objects with. If
                    left empty, all objects from the input containers are
                    considered.
      outputLabel -- Decoration put on the output variables. Set to "true" for
                     objects passing the overlap removal.
      linkOverlapObjects -- Set up an element link between overlapping objects
      doMuPFJetOR -- Set up overlap removal for PFlow jets that are acutally muons
      doEleEleOR -- Set up electron-electron overlap removal
      doXXXX     -- these flags enable/disable object types to
                    configure tools for: doElectrons, doMuons,
                    doJets, doTaus, doPhotons, doFatJets.
      bJetLabel -- Flag to select b-jets with. If left empty, no b-jets are used
                   in the overlap removal.
      boostedLeptons -- Set to True to enable boosted lepton overlap removal
      enableCutflow -- Whether or not to dump the cutflow
    """

    if dataType not in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( 'OverlapAnalysisSequence' + postfix )

    # Create the overlap removal algorithm:
    alg = createAlgorithm( 'CP::OverlapRemovalAlg', 'OverlapRemovalAlg' + postfix )

    # Create its main tool, and set its basic properties:
    addPrivateTool( alg, 'overlapTool', 'ORUtils::OverlapRemovalTool' )
    alg.overlapTool.InputLabel = inputLabel
    alg.overlapTool.OutputLabel = outputLabel

    # By default the OverlapRemovalTool would flag objects that need to be
    # suppressed, with a "true" value. But since the analysis algorithms expect
    # the opposite behaviour from selection flags, we need to tell the tool
    # explicitly to use the "true" flag on objects that pass the overlap
    # removal.
    alg.overlapTool.OutputPassValue = True

    # Set up overlap removal for PFlow jets that are acutally muons, if requested.
    if doMuPFJetOR:
       addPrivateTool( alg, 'overlapTool.MuPFJetORT',
                       'ORUtils::MuPFJetOverlapTool' )
       alg.overlapTool.MuPFJetORT.InputLabel = inputLabel
       alg.overlapTool.MuPFJetORT.OutputLabel = outputLabel
       alg.overlapTool.MuPFJetORT.LinkOverlapObjects = linkOverlapObjects
       alg.overlapTool.MuPFJetORT.OutputPassValue = True
       pass

    # Set up the electron-electron overlap removal, if requested.
    if doElectrons and doEleEleOR:
        addPrivateTool( alg, 'overlapTool.EleEleORT',
                        'ORUtils::EleEleOverlapTool' )
        alg.overlapTool.EleEleORT.InputLabel = inputLabel
        alg.overlapTool.EleEleORT.OutputLabel = outputLabel
        alg.overlapTool.EleEleORT.LinkOverlapObjects = linkOverlapObjects
        alg.overlapTool.EleEleORT.OutputPassValue = True
        pass

    # Set up the electron-muon overlap removal.
    if doElectrons and doMuons:
        addPrivateTool( alg, 'overlapTool.EleMuORT',
                        'ORUtils::EleMuSharedTrkOverlapTool' )
        alg.overlapTool.EleMuORT.InputLabel = inputLabel
        alg.overlapTool.EleMuORT.OutputLabel = outputLabel
        alg.overlapTool.EleMuORT.LinkOverlapObjects = linkOverlapObjects
        alg.overlapTool.EleMuORT.OutputPassValue = True
        pass

    # Set up the electron-(narrow-)jet overlap removal.
    if doElectrons and doJets:
        addPrivateTool( alg, 'overlapTool.EleJetORT',
                        'ORUtils::EleJetOverlapTool' )
        alg.overlapTool.EleJetORT.InputLabel = inputLabel
        alg.overlapTool.EleJetORT.OutputLabel = outputLabel
        alg.overlapTool.EleJetORT.LinkOverlapObjects = linkOverlapObjects
        alg.overlapTool.EleJetORT.BJetLabel = bJetLabel
        alg.overlapTool.EleJetORT.UseSlidingDR = boostedLeptons
        alg.overlapTool.EleJetORT.OutputPassValue = True
        pass

    # Set up the muon-(narrow-)jet overlap removal.
    if doMuons and doJets:
        addPrivateTool( alg, 'overlapTool.MuJetORT',
                        'ORUtils::MuJetOverlapTool' )
        alg.overlapTool.MuJetORT.InputLabel = inputLabel
        alg.overlapTool.MuJetORT.OutputLabel = outputLabel
        alg.overlapTool.MuJetORT.LinkOverlapObjects = linkOverlapObjects
        alg.overlapTool.MuJetORT.BJetLabel = bJetLabel
        alg.overlapTool.MuJetORT.UseSlidingDR = boostedLeptons
        alg.overlapTool.MuJetORT.OutputPassValue = True
        pass

    # Set up the tau-electron overlap removal.
    if doTaus and doElectrons:
        addPrivateTool( alg, 'overlapTool.TauEleORT',
                        'ORUtils::DeltaROverlapTool' )
        alg.overlapTool.TauEleORT.InputLabel = inputLabel
        alg.overlapTool.TauEleORT.OutputLabel = outputLabel
        alg.overlapTool.TauEleORT.LinkOverlapObjects = linkOverlapObjects
        alg.overlapTool.TauEleORT.DR = 0.2
        alg.overlapTool.TauEleORT.OutputPassValue = True
        pass

    # Set up the tau-muon overlap removal.
    if doTaus and doMuons:
        addPrivateTool( alg, 'overlapTool.TauMuORT',
                        'ORUtils::DeltaROverlapTool' )
        alg.overlapTool.TauMuORT.InputLabel = inputLabel
        alg.overlapTool.TauMuORT.OutputLabel = outputLabel
        alg.overlapTool.TauMuORT.LinkOverlapObjects = linkOverlapObjects
        alg.overlapTool.TauMuORT.DR = 0.2
        alg.overlapTool.TauMuORT.OutputPassValue = True
        pass

    # Set up the tau-(narrow-)jet overlap removal.
    if doTaus and doJets:
        addPrivateTool( alg, 'overlapTool.TauJetORT',
                        'ORUtils::DeltaROverlapTool' )
        alg.overlapTool.TauJetORT.InputLabel = inputLabel
        alg.overlapTool.TauJetORT.OutputLabel = outputLabel
        alg.overlapTool.TauJetORT.LinkOverlapObjects = linkOverlapObjects
        alg.overlapTool.TauJetORT.DR = 0.2
        alg.overlapTool.TauJetORT.OutputPassValue = True
        pass

    # Set up the photon-electron overlap removal.
    if doPhotons and doElectrons:
        addPrivateTool( alg, 'overlapTool.PhoEleORT',
                        'ORUtils::DeltaROverlapTool' )
        alg.overlapTool.PhoEleORT.InputLabel = inputLabel
        alg.overlapTool.PhoEleORT.OutputLabel = outputLabel
        alg.overlapTool.PhoEleORT.LinkOverlapObjects = linkOverlapObjects
        alg.overlapTool.PhoEleORT.OutputPassValue = True
        pass

    # Set up the photon-muon overlap removal.
    if doPhotons and doMuons:
        addPrivateTool( alg, 'overlapTool.PhoMuORT',
                        'ORUtils::DeltaROverlapTool' )
        alg.overlapTool.PhoMuORT.InputLabel = inputLabel
        alg.overlapTool.PhoMuORT.OutputLabel = outputLabel
        alg.overlapTool.PhoMuORT.LinkOverlapObjects = linkOverlapObjects
        alg.overlapTool.PhoMuORT.OutputPassValue = True
        pass

    # Set up the photon-(narrow-)jet overlap removal.
    if doPhotons and doJets:
        addPrivateTool( alg, 'overlapTool.PhoJetORT',
                        'ORUtils::DeltaROverlapTool' )
        alg.overlapTool.PhoJetORT.InputLabel = inputLabel
        alg.overlapTool.PhoJetORT.OutputLabel = outputLabel
        alg.overlapTool.PhoJetORT.LinkOverlapObjects = linkOverlapObjects
        alg.overlapTool.PhoJetORT.OutputPassValue = True
        pass

    # Set up the electron-fat-jet overlap removal.
    if doElectrons and doFatJets:
        addPrivateTool( alg, 'overlapTool.EleFatJetORT',
                        'ORUtils::DeltaROverlapTool' )
        alg.overlapTool.EleFatJetORT.InputLabel = inputLabel
        alg.overlapTool.EleFatJetORT.OutputLabel = outputLabel
        alg.overlapTool.EleFatJetORT.LinkOverlapObjects = linkOverlapObjects
        alg.overlapTool.EleFatJetORT.DR = 1.0
        alg.overlapTool.EleFatJetORT.OutputPassValue = True
        pass

    # Set up the (narrow-)jet-fat-jet overlap removal.
    if doJets and doFatJets:
        addPrivateTool( alg, 'overlapTool.JetFatJetORT',
                        'ORUtils::DeltaROverlapTool' )
        alg.overlapTool.JetFatJetORT.InputLabel = inputLabel
        alg.overlapTool.JetFatJetORT.OutputLabel = outputLabel
        alg.overlapTool.JetFatJetORT.LinkOverlapObjects = linkOverlapObjects
        alg.overlapTool.JetFatJetORT.DR = 1.0
        alg.overlapTool.JetFatJetORT.OutputPassValue = True
        pass

    # Add the algorithm to the analysis sequence.
    seq.append( alg,
                inputPropName = { 'electrons' : 'electrons',
                                  'muons'     : 'muons',
                                  'jets'      : 'jets',
                                  'taus'      : 'taus',
                                  'photons'   : 'photons',
                                  'fatJets'   : 'fatJets' },
                outputPropName = { 'electrons' : 'electronsOut',
                                   'muons'     : 'muonsOut',
                                   'jets'      : 'jetsOut',
                                   'taus'      : 'tausOut',
                                   'photons'   : 'photonsOut',
                                   'fatJets'   : 'fatJetsOut' } )

    # Add view container creation algorithms for all types.
    for container in [ ( 'electrons', doElectrons ),
                       ( 'muons',     doMuons ),
                       ( 'jets',      doJets ),
                       ( 'taus',      doTaus ),
                       ( 'photons',   doPhotons ),
                       ( 'fatJets',   doFatJets ) ]:

        # Skip setting up a view container if the type is not being processed.
        if not container[ 1 ]:
            continue

        # Set up a cutflow alg.
        if enableCutflow:
            alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg',
                                   'OverlapRemovalCutFlowDumperAlg_%s' % container[ 0 ] + postfix )
            alg.histPattern = container[ 0 ] + postfix + '_OR_cflow_%SYS%'
            if inputLabel:
                alg.selection = [ '%s,as_char' % inputLabel,
                                  '%s,as_char' % outputLabel ]
                alg.selectionNCuts = [1, 1]
            else:
                alg.selection = [ '%s,as_char' % outputLabel ]
                alg.selectionNCuts = [1]
            seq.append( alg, inputPropName = { container[ 0 ] : 'input' } )

        # Set up a view container for the type.
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                               'OverlapRemovalViewMaker_%s' % container[ 0 ] + postfix )
        alg.selection = [ '%s,as_char' % outputLabel ]
        seq.append( alg, inputPropName = { container[ 0 ] : 'input' },
                    outputPropName = { container[ 0 ] : 'output' } )
        pass

    # Return the sequence:
    return seq
