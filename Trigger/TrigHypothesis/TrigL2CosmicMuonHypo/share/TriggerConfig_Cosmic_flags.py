include.block('TrigL2CosmicMuonHypo/TriggerConfig_Cosmic_flags.py')

triggerMenu.addSignatures(['cosmic_IDSCAN', 'cosmic_SiTrack',
                           'cosmic_trtxk',
                           'cosmic_muonOnly',
                           'cosmic_tileRodMu', 'cosmic_tileLookForMu',
                           'cosmic_em10cl' ])
triggerMenu.enableSignatures(['cosmic_IDSCAN', 'cosmic_SiTrack',
                              'cosmic_trtxk', 
                              'cosmic_muonOnly',
                              'cosmic_tileRodMu', 'cosmic_tileLookForMu',
                              #'cosmic_em10cl'
                              ])

# triggerMenu.disableSignatures([])
# TriggerFlags.CosmicSlice.addFlags(['doFullScan'], False)
