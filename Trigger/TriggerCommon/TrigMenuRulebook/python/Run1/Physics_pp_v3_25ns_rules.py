# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Physics_pp_v3'

rules = {


"""
The 25ns rules have been updated to the PS/rates used during the 25ns run on Oct 7 2011.

"""

  #####################################################################
  # muons
  # primary 1e33
  'EF_mu15i':                  {1000: TriggerRule(PS=0,rerun=True,comment='Disabled for 3e33',),},
  'EF_mu20':                   {1000: TriggerRule(PS=1.0,comment='Baseline primary',),
                                2000: TriggerRule(comment='No PS to ensure some rate for passthrough')},
  'L2_mu20':                   {1000: TriggerRule(inputRate=0.1,PTValue=1,comment='Introduce PT instead of streaming to L1Muon',)},

  'L2_mu18':                   {1000: TriggerRule(inputRate=2,PTValue=1,comment='Introduce PT instead of streaming to L1Muon',)},

  'EF_mu20_MG':                {1000: TriggerRule(PS=1.0,depends_on='EF_mu20',comment='Baseline primary',),
                                2000: TriggerRule(PS=-1.0)}, 
  #'EF_mu15_mu7_EFFS':         {1000: TriggerRule(PS=1.0,comment='primary',)},  #pending approval 
  'EF_mu15_mu10_EFFS':         {1000: TriggerRule(PS=1.0,comment='primary',),
                                2000: TriggerRule(PS=-1.0)},
  'EF_3mu6_MSonly':            {3000: TriggerRule(PS=1.0,comment='Priority 1 primary, exotics',)}, 
  'EF_mu40_MSonly_barrel':     {1000: TriggerRule(PS=1.0,comment='Baseline primary, exotics',),
                                2000: TriggerRule(PS=-1.0)}, 
  'EF_2mu10_loose':            {3000: TriggerRule(PS=1.0,comment='Baseline primary',)},
  'EF_2mu10':                  {3000: TriggerRule(PS=1.0,ESRate=0.3,comment='Backup primary')},
  'L2_2mu10':                  {1000: TriggerRule(PTRate=1.0,comment='Introduce PT instead of streaming to L1Muon',)},
  'EF_2mu10_loose_noOvlpRm':   {1000: TriggerRule(PS=-1.0,comment='Not for high lumi',)},
  'EF_2mu6_MSonly_g10_loose' :{3000: TriggerRule(PS=1.0,comment='Baseline primary',)},
  'EF_mu40_MSonly_tighter':    {1000: TriggerRule(PS=-1.0, comment=' Disable, MCG 28/4',)}, 
  'EF_mu40_MSonly_tight':      {1000: TriggerRule(rate=0.5, comment='Control sample for other MSonly items',),
                                2000: TriggerRule(PS=-1, comment='Priority 2 primary, exotics - Disable, MCG 28/4',)}, 
  'EF_mu40_MSonly_tight_L1MU11': {2000: TriggerRule(rate=0.5, comment='Control sample for other MSonly items',)}, 


  'EF_mu15_xe30_noMu' : {1000: TriggerRule(PS=1.0, comment='Charged Higgs,SUSY item',)}, 
  'EF_2mu4T_EFxe30_noMu':    {3000: TriggerRule(PS=-1.0, comment='SUSY;disabled due to L2 rate',)},
  'EF_2mu4T_xe30_noMu':      {3000: TriggerRule(PS=1.0, comment='SUSY',)},
  'EF_2mu4T_xe40_noMu':      {3000: TriggerRule(PS=1.0, comment='SUSY; backup item',)}, 
  'EF_2mu4T_EFxe20_noMu':    {3000: TriggerRule(PS=-1.0, comment='SUSY; high rate',)},
  'EF_mu15_2mu4_EFFS_medium':  {3000: TriggerRule(PS=1.0, comment='SUSY; multi-lepton trigger',)},

  # 2e33 backup
  'EF_mu15i_medium':                  {#1000: TriggerRule(PS=1.0,comment='Backup Baseline primary',),
                                       2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_mu20i_medium':                  {3000: TriggerRule(PS=-1.0,comment='Obsolete isolation')}, 
  'EF_mu18_medium':                   {1000: TriggerRule(PS=1.0,comment='Backup Baseline primary',),
                                       2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},
  'EF_mu18_MG_medium':                {1000: TriggerRule(PS=1.0,comment='Backup Baseline primary',),
                                       2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},
  'EF_mu20_medium':                   {1000: TriggerRule(PS=1.0,comment='Backup Baseline primary',),
                                       2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},
  'EF_mu20_MG_medium':                {1000: TriggerRule(PS=1.0,depends_on='EF_mu20_medium',comment='Backup Baseline primary',),
                                       2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},
  'EF_mu22_medium':                   {3000: TriggerRule(PS=1.0,comment='Baseline primary',)},
  'EF_mu22_MG_medium':                {3000: TriggerRule(PS=1.0,depends_on='EF_mu22_medium',comment='Backup Baseline primary',)}, 
  'EF_mu15_mu10_EFFS_medium':         {3000: TriggerRule(PS=1.0,comment='Baseline primary',)},
  'EF_mu40_MSonly_barrel_medium':     {3000: TriggerRule(PS=1.0,comment='Baseline primary, exotics',)}, 

  'EF_mu24_medium':                   {1000: TriggerRule(PS=1.0,comment='Backup Baseline primary',)},
  'EF_mu24_MG_medium':                {1000: TriggerRule(PS=1.0,comment='Backup Baseline primary',)},
  'EF_mu24_tight':                    {1000: TriggerRule(PS=1.0,comment='Backup Baseline primary',)},
  'EF_mu24_MG_tight':                 {1000: TriggerRule(PS=1.0,comment='Backup Baseline primary',)},

  'EF_mu30_medium':                   {1000: TriggerRule(PS=1.0,comment='Backup Baseline primary',)},
  'EF_mu30_MG_medium':                {1000: TriggerRule(PS=1.0,comment='Backup Baseline primary',)},
  'EF_mu30_tight':                    {1000: TriggerRule(PS=1.0,comment='Backup Baseline primary',)},
  'EF_mu30_MG_tight':                 {1000: TriggerRule(PS=1.0,comment='Backup Baseline primary',)},

  'EF_mu15_mu10_EFFS_tight':         {1000: TriggerRule(PS=1.0,comment='Backup primary',)},
  'EF_mu20_mu10_EFFS_tight':         {1000: TriggerRule(PS=1.0,comment='Backup primary',)},

#isolation
  'EF_mu18i3_medium':                        {4000: TriggerRule(PS=1.0,comment='Isolation for 4e33',)},## Nav ERROR => fixed (87108)
  'EF_mu18i4_medium':                        {4000: TriggerRule(PS=1.0,comment='Isolation for 5e33',)},## Nav ERROR => fixed (87108)
  'EF_mu22i1_medium':                        {4000: TriggerRule(PS=1.0,comment='Backup Isolation for 5e33')},## Nav ERROR => fixed (87108)
  'EF_mu22i2_medium':                        {4000: TriggerRule(PS=1.0,comment='Backup Isolation for 5e33')},## Nav ERROR => fixed (87108)
  'EF_mu22i3_medium':                        {4000: TriggerRule(PS=1.0,comment='Backup Isolation for 5e33')},## Nav ERROR => fixed (87108)
  'EF_mu22i4_medium':                        {4000: TriggerRule(PS=1.0,comment='Backup Isolation for 5e33')},## Nav ERROR => fixed (87108)

#muon+jets:
  'EF_mu18_medium_2j20_a4tc_EFFS':           {1000: TriggerRule(PS=1.0,comment='Need real rule when mu18 is disabled'),},
  'EF_mu18_medium_3j20_a4tc_EFFS':           {1000: TriggerRule(PS=1.0,comment='Need real rule when mu18 is disabled'),},
  'EF_mu18_medium_4j15_a4tc_EFFS':           {1000: TriggerRule(PS=1.0,comment='Need real rule when mu18 is disabled'),},
  'EF_mu18_medium_4j20_a4tc_EFFS':           {1000: TriggerRule(PS=1.0,comment='Need real rule when mu18 is disabled'),},
  
  'EF_mu40_slow':              {1000: TriggerRule(PS=1.0,comment='Baseline primary',),
                                2000: TriggerRule(PS=-1.0)}, 
  'EF_mu40_slow_medium':       {1000: TriggerRule(PS=1.0,comment='Backup Baseline primary',)}, 
  'EF_mu40_slow_empty':        {1000: TriggerRule(PS=1.0,comment='supporting for primary',)}, 
  #'L2_mu40_slow_outOfTime':    {1000: TriggerRule(inputRate=7000,comment='limit L2 rate',)}, 
  'EF_mu40_slow_outOfTime':    {1000: TriggerRule(PS=1,comment='supporting for primary (with L2 limit introduced)',),
                                2000: TriggerRule(PS=-1.0)}, 
  'EF_mu40_slow_outOfTime_medium':{3000: TriggerRule(PS=1,comment='supporting for primary')}, 
  
  # supporting
  #
  # low pt eff
  'EF_mu4T':                  {1000: TriggerRule(rate=0.5, comment='eff for BPhys (J/Psi T&P)')},
  'EF_mu6':                  {1000: TriggerRule(rate=0.5, comment='eff for BPhys (J/Psi T&P)')},
  'EF_2mu4T':                 {1000: TriggerRule(rate=1.0, comment='monitoring for BPhys')},
  
  # eff for primaries and DQ
  'EF_mu15':                 {1000: TriggerRule(rate=0.5, rerun=True,ESRate=0.3,comment='bootstrap for mu20')},
  'EF_mu10_loose':           {1000: TriggerRule(PS=0,rerun=True, comment='eff for 2mu10')},
  'EF_mu20_muCombTag_NoEF':  {1000: TriggerRule(rate=0.5, ESRate=0.3, comment='unbiased EF eff, muFast eff')},
  'L2_mu20_IDTrkNoCut':      {1000: TriggerRule(rate=50,maxRate=50, comment='unbiased combiner eff')},
  'EF_mu20_IDTrkNoCut':      {1000: TriggerRule(rate=0.5, ESRate=0.8, comment='unbiased combiner eff')},
  'EF_mu20_IDTrkNoCut_ManyVx': {1000: TriggerRule(rate=0.3, comment='unbiased combiner eff')},

  'EF_mu10_muCombTag_NoEF':  {1000: TriggerRule(PS=-1,comment='toroid OFF chain')},
  'EF_mu13_muCombTag_NoEF':  {1000: TriggerRule(PS=-1,comment='toroid OFF chain')},
  
  
  # backup of primaries
  'EF_mu22':           {1000: TriggerRule(PS=1.0,comment='backup of primary',),
                        2000: TriggerRule(PS=-1.0)},
  'EF_mu22_MG':        {1000: TriggerRule(PS=1.0,depends_on='EF_mu22',comment='backup of primary',),
                        2000: TriggerRule(PS=-1.0)},
  'EF_mu18':           {1000: TriggerRule(maxRate=40,PS=1,comment='Priority 2 primary',),
                        2000: TriggerRule(PS=-1.0)},
  'EF_mu18_MG':        {1000: TriggerRule(maxRate=40,PS=1,depends_on='EF_mu18',comment='Priority 2 primary',),
                        2000: TriggerRule(PS=-1.0)},
  'EF_mu18_L1J10':     {1000: TriggerRule(PS=1,comment='Emergency for 2e33',),
                        2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},
  'EF_mu18_MG_L1J10':  {1000: TriggerRule(PS=1,comment='Emergency for 2e33',),
                        2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},
  'EF_mu20i':          {1000: TriggerRule(PS=1.0,comment='backup of primary mu15i',),
                        2000: TriggerRule(PS=-1.0)},
  #    'EF_mu20_mu7_EFFS':  {1000: TriggerRule(PS=1.0,comment='backup of primary mu15_mu7',)},  #pending approval
  'EF_3mu6':           {1000: TriggerRule(PS=1.0,comment='backup for 3mu6_MSonly;',)},     
  
  
  # for comparison to 2010 studies
  'EF_mu13':           { 100: TriggerRule(PS=-1, comment='off since 81486',),},
  #'EF_mu13_MG':        { 100: TriggerRule(rate=0.5, depends_on='EF_mu13', comment='off since 81486',),},
  'EF_mu13_MG':        { 1000: TriggerRule(rate=0.5, comment='off since 81486',),},
  'EF_2mu6':           { 100: TriggerRule(rate=0.5, comment='comparison to 2010 studies',),},
  
  # muon calibration
  'L2_mu10_cal':      { 500: TriggerRule(PS=1, comment='muon calibration;does not accept events in normal streams',),
                        2000: TriggerRule(PS=-1.0),},
  'L2_mu10_cal_medium':{ 3000: TriggerRule(PS=1, comment='muon calibration;does not accept events in normal streams',),},
  'L2_mu4_cal_empty':      { 500: TriggerRule(PS=-1, comment='muon calibration;does not accept events in normal streams',),},
  
  # beam background - most of these should all be low rate and get PS=1 by construction
  'EF_mu4_empty_NoAlg':         { 500: TriggerRule(PS=-1, comment='beam/cosmic background',),},
  'EF_mu4_unpaired_iso_NoAlg':  { 500: TriggerRule(rate=0.5, scaling="bunches", comment='beam/cosmic background',),},
  'EF_mu20_empty':              { 500: TriggerRule(rate=0.5, scaling="bunches", comment='beam/cosmic background',),},
  'EF_2mu10_loose_empty':       { 500: TriggerRule(rate=0.5, scaling="bunches", comment='beam/cosmic background',),},
  'EF_2mu10_empty':             { 500: TriggerRule(rate=0.5, scaling="bunches", comment='beam/cosmic background',),},
  'EF_mu40_MSonly_empty':       { 500: TriggerRule(rate=0.5, scaling="bunches", comment='beam/cosmic background',),},
  'EF_mu4_firstempty_NoAlg':    { 500: TriggerRule(PS=5150, comment='~ rate of 2Hz at 1e32'),},
  
  'EF_L1MU11_NoAlg':       { 500: TriggerRule(rate=2, comment='L1 passthrough',),},
  'EF_L1MU15_NoAlg':       { 500: TriggerRule(rate=0.1, comment='L1 passthrough',),},
  'EF_L1MU20_NoAlg':       { 500: TriggerRule(rate=0.1, comment='L1 passthrough',),},

  
  #####################################################################
  # Egamma
  
  #--- Primary Electrons (PS=1)
  'EF_e12Tvh_medium_2e6T_medium': { 800: TriggerRule(PS=1.0, comment='SUSY primary; D')},
  'EF_2e15vh_medium':          { 800: TriggerRule(PS=1.0,ESRate=0.4, comment='Baseline primary; D')},
  'EF_2e12_medium':          { #800: TriggerRule(PS=1.0, ESRate=0.4,comment='Baseline primary; DY, H->4l - MCG 28/4, once 2e10_medium is prescaled out put this at the ES'),
                              2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_2e12Tvh_medium':          { 3000: TriggerRule(PS=1.0,comment='Baseline primary; DY, H->4l')},
  'EF_2e10_medium':          { 800: TriggerRule(PS=-1.0, comment='Prescaled out (MCG 28/4)')},
##  'L2_e20_medium':           { #500: TriggerRule(PTRate=1.0,comment='Introduce PT instead of streaming to L1Calo'),
##                              2301: TriggerRule(inputRate=1,PTValue=1,comment='PT for EM14')},
  'EF_e22vh_medium':           {#1000: TriggerRule(PS=1.0,comment='Priority 1 Backup to e20_medium for high lumi (L>10^33)'),
                                2301: TriggerRule(PS=-1.0,comment='Extra for 3e33;special request')},
  'EF_e22vh_medium1':          {3000: TriggerRule(PS=1.0,comment='Backup Baseline primary; SM, top, Higgs, Susy, exotics')},
  'EF_e22vh_medium2':          {#1000: TriggerRule(PS=1.0,comment='Backup Baseline primary; SM, top, Higgs, Susy, exotics'),
                                2301: TriggerRule(PS=-1.0,comment='Extra for 3e33;special request')},
#  'EF_e33_medium':           {1000: TriggerRule(PS=1.0,comment='Backup Baseline primary; SM, top, Higgs, Susy, exotics'),
#                              2301: TriggerRule(PS=-1.0,comment='Extra for 3e33;special request')},
#  'EF_e35_medium':           {1000: TriggerRule(PS=1.0,comment='Backup Baseline primary; SM, top, Higgs, Susy, exotics'),
#                              2301: TriggerRule(PS=-1.0,comment='Extra for 3e33;special request')},
  'EF_e40_medium':           {1000: TriggerRule(PS=-1.0,comment='Backup Baseline primary; SM, top, Higgs, Susy, exotics'),
                              2301: TriggerRule(PS=-1.0,comment='Extra for 3e33;special request')},
  'EF_e45_medium':           {1000: TriggerRule(PS=-1.0,comment='Backup Baseline primary; SM, top, Higgs, Susy, exotics'),
                              2301: TriggerRule(PS=-1.0,comment='Extra for 3e33;special request')},
  'L2_e45_medium1':          {3000: TriggerRule(PTRate=1,comment='1 Hz of EM30'),},
  'EF_e45_medium1':          {3000: TriggerRule(PS=1.0,comment='Backup Baseline primary; SM, top, Higgs, Susy, exotics'),},
#  'L2_e60_loose':            {#1000: TriggerRule(PTRate=1,comment='Efficiency Bootstrap, LAr mon'),},
#                              2301: TriggerRule(inputRate=1,PTValue=1,comment='PT for EM30')},# to be used if e60_loose is disabled from extras
  'EF_e60_loose':            {#1000: TriggerRule(PS=1.0,comment='Priority 1 Primary; Exotics'),
                              2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33'),
                             },
  'EF_3e10_medium':          {1000: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
                                
  'EF_e10_medium_mu10':      {3000: TriggerRule(PS=1.0,comment='Baseline primary; H->4l, H->WW->lnulnu, A->tau+tau')},
  'EF_e10_medium_mu6':       {1000: TriggerRule(PS=1.0,comment='Priority 1 primary, A->tau+tau'),
                              2301: TriggerRule(PS=1.0,comment='Extra for 3e33')}, #needs 0.5 Hz if disabled as support trigger
  'EF_e10_medium_2mu6':      {3000: TriggerRule(PS=1.0,comment='Priority 1  primary')},
  'EF_2e10_medium_mu6':      {3000: TriggerRule(PS=1.0,comment='Priority 1  primary')},
  
#  'EF_e15vh_medium_xe30_noMu':  {1000: TriggerRule(PS=1.0, ESRate=0.4,comment='Priority 1 Primary'),
#                                 2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33 - needs ES to be moved for low lumi')},
  'EF_e15vh_medium_xe40_noMu':  {1000: TriggerRule(PS=1.0,ESRate=0.4, comment='Backup'),
                                 2301: TriggerRule(PS=1.0,ESRate=0.4,comment='Extra for 3e33')},
  'EF_e15vh_medium_xe50_noMu':  {1000: TriggerRule(PS=1.0, comment='Backup'),
                                 2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},

  
  'EF_e5_medium_mu6_topo_medium': {1000: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_e5_medium_mu6':             {1000: TriggerRule(rate=0.5,comment='Supporting trigger;'),
                                   2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_e10_medium_mu6_topo_medium': {1000: TriggerRule(PS=1.0,comment='Priority 1 primary;'),
                                    2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},

  'EF_e12Tvh_medium_mu6_topo_medium': {3000: TriggerRule(PS=1,comment='Backup trigger')},
  'EF_e12Tvh_medium_mu6': {3000: TriggerRule(rate=0.5,comment='Supporting trigger' )},


  #--- Primary Potons (PS=1)
  'EF_g80_loose':            {3000: TriggerRule(PS=1.0,comment='Baseline primary; Direct photons')},
  'EF_g100_loose':            {3000: TriggerRule(PS=1.0,comment='Baseline primary; Direct photons; #86785')},
  'EF_g150_etcut':           {3000: TriggerRule(PS=1.0,comment='Baseline Primary; Exotics')},
  'L2_2g20_loose':           {1000: TriggerRule(PTRate=1.0,comment='Baseline primary; Di-photons, H->gamma+gamma')},
  'EF_2g20_loose':           {3000: TriggerRule(PS=1.0,comment='Baseline primary; Di-photons, H->gamma+gamma')},
  'EF_g40_loose_EFxe40_noMu':{#1000: TriggerRule(PS=1.0,comment='Priority 1 Primary '),
                              2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_g40_loose_xe45_medium_noMu':{3000: TriggerRule(PS=1.0,comment='Backup Primary ')},
  'EF_g40_loose_xe55_medium_noMu':{3000: TriggerRule(PS=1.0,comment='Backup Primary ')},  
    
  #--- Primary Electrons (PSed)
  'L1_2EM3':                   {1000: TriggerRule(rate=5000,comment='Rate Control;J/psi calibration'),
                                1501: TriggerRule(rate=3000,comment='Rate Control;J/psi calibration'),
                                2301: TriggerRule(rate=1500,comment='Rate Control;J/psi calibration'),
                                3000: TriggerRule(rate=800,comment='Rate Control;J/psi calibration'),
                                4000: TriggerRule(rate=300,comment='Rate Control;J/psi calibration'),},
  'L1_2EM3_EM7':               {1000: TriggerRule(rate=6000,comment='J/psi calibration'),
                                2301: TriggerRule(rate=2000,comment='Rate Control;J/psi calibration'),
                                3000: TriggerRule(rate=1000,comment='Rate Control;J/psi calibration'),
                                4000: TriggerRule(rate=500,comment='Rate Control;J/psi calibration'),},
  'L1_2EM3_EM12':              {1000: TriggerRule(PS=1,comment='feeds e5_tight_e14_etcut_Jpsi'),
                                1501: TriggerRule(rate=4000,comment='feeds e5_tight_e14_etcut_Jpsi'),
                                2301: TriggerRule(rate=2000,comment='feeds e5_tight_e14_etcut_Jpsi'),
                                3000: TriggerRule(rate=1000,comment='feeds e5_tight_e14_etcut_Jpsi'),
                                4000: TriggerRule(rate=500,comment='Rate Control;J/psi calibration'),},

  'EF_2e5_tight_Jpsi':         {1000: TriggerRule(rate=1.0,ESRate=0.4,comment='Primary; J/Psi, ')},
  'EF_e5_tight_e4_etcut_Jpsi': {1000: TriggerRule(rate=3,comment='Primary; Performance from JPsi, rate mostly limited by L1')},
  'EF_e5_tight_e9_etcut_Jpsi': {1000: TriggerRule(rate=6,comment='Primary; Performance from JPsi, rate mostly limited by L1')},
  'EF_e5_tight_e14_etcut_Jpsi': {1000: TriggerRule(PS=1,comment='Primary; Performance from JPsi, rate mostly limited by L1'),},
  'EF_e9_tight_e4_etcut_Jpsi':  {1000: TriggerRule(rate=6,comment='Primary; Performance from JPsi, rate mostly limited by L1')},
  'EF_e14_tight_e4_etcut_Jpsi': {1000: TriggerRule(PS=1,comment='Primary; Performance from JPsi, rate mostly limited by L1'),},
  
  
  'EF_e7_tight_e14_etcut_Jpsi':{2301: TriggerRule(PS=-1.0,comment='Obsolete - Disabled for 3e33')},
 
  #need to limit the L2 rate - should not be needed anymore
#  'L2_e5_tight_e4_etcut_Jpsi': {500: TriggerRule(rate=500.,maxRate=500.,propagate=True,comment='Primary; Performance from JPsi,'),
#                                1000:TriggerRule(rate=200.,maxRate=200.,propagate=True,comment='Rate Control;Primary; Performance from JPsi,'),},
#  'L2_e5_tight_e9_etcut_Jpsi': {500: TriggerRule(rate=800.,maxRate=800.,propagate=True,comment='Primary; Performance from JPsi,'),
#                                750: TriggerRule(rate=500.,maxRate=500.,propagate=True,comment='Primary; Performance from JPsi,'),
#                                1000:TriggerRule(rate=200.,maxRate=200.,propagate=True,comment='Rate Control;Primary; Performance from JPsi,'),},
#  'L2_e9_tight_e5_tight_Jpsi': {500: TriggerRule(rate=500.,maxRate=500.,propagate=True,comment='Primary; Performance from JPsi,'),
#                                1000:TriggerRule(rate=200.,maxRate=200.,propagate=True,comment='Rate Control;Primary; Performance from JPsi,'),},
  'EF_e5_tight_e4_etcut_Jpsi_TRT': {1000: TriggerRule(PS=-1,comment='Alternative Tracking; Coh PS with EF_jpsi_e5_tight_e4_etcut')},

  #--- Primary Photons (PSed)
  'L2_g20_loose':              {1000: TriggerRule(PTRate=1.0,comment='Bootstrap and LAr studies')},
  'EF_g20_loose':              {1000: TriggerRule(rate=2.0, rerun=True,ESRate=0.4, comment='Baseline prescaled primary; Direct photons, support for 2g20')},
  'EF_g40_loose':              {1000: TriggerRule(rate=2.0, rerun=True,comment='Baseline prescaled primary; Direct photons')},
  'EF_g60_loose':              {1000: TriggerRule(rate=2.0,comment='Baseline prescaled Primary; Direct photons')},
  'EF_g100_etcut_g50_etcut':   {1000: TriggerRule(PS=-1,comment='Disable, MCG 28/4')},
  
  'EF_g40_tight':              {1000: TriggerRule(PS=-1,comment='Priority 2 primary; extra')},
  'EF_g40_tight_b10_medium':   {1000: TriggerRule(PS=1,comment='Primary; gammaZ(bb)')},
  
  
  #--- Monitoring
  'EF_e20_tight_e15_NoCut_Zee':        {1000: TriggerRule(PS=1,comment='Monitoring')},

    'EF_e15vh_loose1_e12Tvh_loose1_Zee': {1000: TriggerRule(PS=1,comment='HSG2 -- #86782')},

  
  
  #--- Alterantive Tracking
  'EF_e5_tight_e4_etcut_Jpsi_SiTrk': {1000: TriggerRule(rate=1.0,comment='Alternative Tracking; Coh PS with EF_jpsi_e5_tight_e4_etcut;')},
  'EF_e5_tight_e5_NoCut':            {1000: TriggerRule(rate=0.5,comment='Performance from JPsi')},
  

  #--- Efficiency and Background extractions
  'EF_g11_etcut':                    { 1000: TriggerRule(PS=-1.0,comment='Bootstrapping for photons - replaced by g20_etcut')},
  'EF_e11_etcut':                    { 1000: TriggerRule(rate=0.5,comment='Background estimation for low-pt electrons')},
  'EF_g20_etcut':                    {1000: TriggerRule(rate=1.5,comment='Background estimations via cut reversal')},
#  'EF_g20_etcut_xe30_noMu':          {500: TriggerRule(rate=1.0,comment='HLT/offline eff from Wenu'),
#                                      800: TriggerRule(rate=0.5,comment='Replaced by xs+EM10 allow 0.5 81377'),},
  
  'EF_xs75_noMu_L1EM10XS50':         { 1000 : TriggerRule(PS=-1)},
  
  'EF_xs60_noMu_L1EM10XS45': {500: TriggerRule(PS=-1,comment="prescale out, there is e13_etcut now - effi with Wenu"),},
  'EF_e13_etcutTrk_xs60_noMu': {1000: TriggerRule(rate=0.5,comment='Wenu performance trigger, # 85832')}, 
  'EF_e13_etcutTrk_xs60_noMu_dphi2j10xe20': {3000: TriggerRule(rate=5,maxRate=6.,comment='Wenu performance trigger, # 85832')},  
  'EF_e13_etcutTrk_xs60_noMu_dphi2j10xe07': {1000: TriggerRule(rate=5,comment='old Wenu performance trigger - enabled until above one fixed, # 85832')},
  'EF_e13_etcutTrk_xs45_noMu_dphi2j10xe20': {3000: TriggerRule(PS=-1,comment='backup Wenu performance trigger, # 85832')},
  'EF_e20_etcutTrk_xs40_noMu_dphi2j10xe20': {3000: TriggerRule(PS=-1,comment='backup Wenu performance trigger, # 85832')},

  'EF_e13_etcut_xs70_noMu': {1000: TriggerRule(PS=-1.0,comment='Wenu backup, poor efficiency')},

  'EF_e20_loose':                    {#1000: TriggerRule(rate=1.5,comment='Cross-check for e20_medium, background estimation via loose selection'),
                                      2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_e20_loose1':                   {#1000: TriggerRule(rate=1.0,comment='Cross-check for e20_medium, background estimation via loose selection'),
                                      2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_e20_looseTrk':                 {#1000: TriggerRule(rate=1.0,comment='Cross-check for e20_medium, background estimation via loose selection'),
                                      2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_e22vh_loose_2j20_a4tc_EFFS':    {3000: TriggerRule(PS=-1, comment='For backgrounds; rate not defined yet'),},
  'EF_e22vh_loose_3j20_a4tc_EFFS':    {3000: TriggerRule(rate=0.5, comment='For backgrounds; rate defined: #85066'),},
  'EF_e22vh_loose_4j15_a4tc_EFFS':    {3000: TriggerRule(rate=1.0, comment='For backgrounds; rate defined #85066'),},

  'EF_e6T_medium':                   {1000: TriggerRule(PS=0,rerun=True,comment='Support for combined e11T_2e6T trigger')},
  'EF_e11T_medium':                  {1000: TriggerRule(PS=0,rerun=True,comment='Support for combined e11T_2e6T trigger')},
  'EF_e10_medium':                   {1000: TriggerRule(rate=0.2,rerun=True,comment='Support for combined 2e10 trigger')},
  'EF_e12_medium':                   {1000: TriggerRule(rate=0.,rerun=True,comment='Support for combined 2e12 trigger')},
  'EF_e12Tvh_medium':                {1000: TriggerRule(rate=0.,rerun=True,comment='Support for combined 2e12 trigger')},
  'EF_e15vh_medium':                   {1000: TriggerRule(rate=0.2,rerun=True,comment='Support for e+xe trigger')},
  'EF_e5_tight':                     {1000: TriggerRule(comment='Trigger eff. for J/Psi')}, 
  'L2_e5_tight':                     {1000: TriggerRule(PTValue=1, inputRate=1,comment='Trigger eff. for J/Psi')}, 
  'EF_2e5_tight':         {1000: TriggerRule(rate=1.0,comment='Supporting for 2e5_tight_JPsi')},
  'EF_e22vh_loose':                    {1000: TriggerRule(rate=1.5,comment='Cross-check for e22vh_medium, background estimation via loose selection')},
  'EF_e22vh_loose1':                   {1000: TriggerRule(rate=1.0,comment='Cross-check for e22vh_medium, background estimation via loose selection')},
  'EF_e22vh_looseTrk':                 {1000: TriggerRule(rate=1.0,comment='Cross-check for e22vh_medium, background estimation via loose selection')},
  'EF_e22vh_medium_SiTrk':             {1000: TriggerRule(fraction=0.1, depends_on='EF_e22vh_medium',comment='Alternative Tracking')},
  'EF_e22vh_medium_TRT':               {1000: TriggerRule(rate=0.5,comment='Alternative Tracking')},
  'EF_e22vh_medium_IDTrkNoCut':        {3000: TriggerRule(rate=0.5,ESRate=0.4,comment='monitoring ')},
  
  'EF_e20_etcutTrk_xe40_noMu_dphi2j10xe20': {3000: TriggerRule(PS=-1)},
  'EF_e20_etcutTrk_xs60_noMu_dphi2j10xe07': {3000: TriggerRule(PS=-1)},
  'EF_e22_etcut_xs60_noMu_dphi2j10xe07': {3000: TriggerRule(PS=-1)},
  'EF_e20_etcut_xs60_noMu': {3000: TriggerRule(PS=-1)},
  'EF_e22_etcut_xs60_noMu': {3000: TriggerRule(PS=-1)}, 

  #####################################################################
  # Taus
  #prims
  'EF_tau100_medium':    {#1000: TriggerRule(PS=1.0, comment='Baseline primary'),
                          2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_tau125_medium':    {#1000: TriggerRule(PS=1.0, comment='Backup primary'),
                          2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'L2_tau125_medium1':    {1000: TriggerRule(PTRate=0.2)},
  'EF_tau125_medium1':   {3000: TriggerRule(PS=1.0, comment='Baseline primary')},
  'L2_2tau29_medium1':   {1000: TriggerRule(PTRate=0.2)},
  'EF_2tau29_medium1':   {3000: TriggerRule(PS=1.0, comment='Baseline primary')},
  'EF_2tau29i_medium1':   {3000: TriggerRule(PS=1.0, comment='Baseline primary - backup')},
  'EF_2tau29T_medium1':  {3000: TriggerRule(PS=1.0, comment='Baseline primary backup')},

  'EF_tau29T_medium2_tau20T_medium2':      {3000: TriggerRule(PS=1.0, comment='Baseline di-tau trigger (Higgs)'),},
  'EF_tau29Ti_medium2_tau20Ti_medium2':    {3000: TriggerRule(PS=1.0, comment='Baseline di-tau trigger (Higgs)'),},
  'EF_tau29T_medium1_1P_tau20T_medium2':    {3000: TriggerRule(PS=1.0, comment='Backup di-tau trigger (Higgs)'),},
  'EF_tau29Ti_medium1_1P_tau20Ti_medium2': {3000: TriggerRule(PS=1.0, comment='Backup di-tau trigger (Higgs)'),},
  'EF_tau20_medium_e15vh_medium':  {1000: TriggerRule(PS=1.0, comment='Baseline primary'),
                                    2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},

  'EF_tau20i_medium_e15vh_medium':  {1000: TriggerRule(PS=1.0, comment='Baseline primary')},
  'EF_tau50_medium_e15vh_medium':  {1000: TriggerRule(PS=1.0, comment='Baseline primary')},
  'EF_tau20v_medium_e15vh_medium':  {1000: TriggerRule(PS=1.0, comment='Backup primary')},                                    

  'EF_tau20T_medium_e15vh_medium':  {3000: TriggerRule(PS=1.0, comment='Baseline primary')},


  'EF_tau20_medium_mu15':         {3000: TriggerRule(PS=1.0, comment='Baseline primary')},
  'EF_tau20i_medium_mu15':         {3000: TriggerRule(PS=1.0, comment='Baseline primary backup')},  
  'EF_tau20T_medium_mu15':        {3000: TriggerRule(PS=1.0, comment='Backup Baseline primary')},
  
  #'EF_tau29_medium_xs80_loose_noMu': {1000: TriggerRule(PS=1.0)}, #needs commissioning
  'EF_tau29_medium_xe40_loose_noMu':{1000: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_tau29_medium_xe35_noMu':    {1000: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_tau29T_medium_xe35_noMu_3L1J10':    {3000: TriggerRule(PS=1.0,  comment='Primary for H+;86009')},
  'EF_tau29T_medium1_xe35_noMu_3L1J10':    {3000: TriggerRule(PS=1.0,  comment='Backup Primary for H+;')},

  'EF_tau29_loose_xs70_loose_noMu':    {#1000: TriggerRule(PS=1.0,  comment='Primary W trigger'),
                                        1700: TriggerRule(PS=-1.0, comment='Emergency for 2e33'),
                                        2301: TriggerRule(PS=-1.0, comment='Disabled for 3e33')},
  'EF_tau29_loose_xs80_loose_noMu':    {#1000: TriggerRule(PS=1.0,  comment='Backup trigger'),
                                        1700: TriggerRule(PS=-1.0, comment='Emergency for 2e33'),
                                        2301: TriggerRule(PS=-1.0, comment='Disabled for 3e33')},

  'EF_tau29_loose_xs45_loose_noMu_3L1J10' : {1000: TriggerRule(PS=-1.0),
                                             3000: TriggerRule(PS=-1.0)}, 
  'EF_tau29_loose1_xs45_loose_noMu_3L1J10' : {1000: TriggerRule(PS=-1.0),
                                              3000: TriggerRule(PS=-1.0)}, 
#  'EF_tau29T_medium_xs45_loose_noMu_3L1J10': {1000: TriggerRule(PS=1.0,comment='Primary Higgs trigger'),
#                                              2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},
  'EF_tau29T_medium_xs45_noMu_3L1J10': {1000: TriggerRule(PS=1.0,comment='Backup Higgs trigger'),
                                        2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},
#  'L2_tau29_medium1_xs20_noMu_3L1J10' :       {3000: TriggerRule(PTRate=0.2,comment='Primary Higgs trigger')},
  'EF_tau29_medium1_xs45_loose_noMu_3L1J10' : {3000: TriggerRule(PS=-1.0,comment='Disabled to save L1 rate')},
  'EF_tau29T_medium1_xs45_loose_noMu_3L1J10': {3000: TriggerRule(PS=-1.0,comment='Disabled to save L1 rate')},
  'EF_tau29T_medium1_xs45_noMu_3L1J10': {3000: TriggerRule(PS=1.0,comment='Commissioning Higgs trigger')},

  'EF_tau29_medium_xs70_noMu'  :    {3000: TriggerRule(PS=-1.0,  comment='Primary W trigger')},  
  'EF_tau29_medium_xs75_noMu'  :    {3000: TriggerRule(PS=-1.0,  comment='Primary W trigger')},  
  'L2_tau29T_medium_xs35_noMu' :    {3000: TriggerRule(PTRate=0.2,  comment='Backup Primary W trigger')},  
  'EF_tau29T_medium_xs75_loose_noMu' : {3000: TriggerRule(PS=1.0,  comment='Backup Primary W trigger')},
  'EF_tau29T_medium_xs75_noMu' :       {3000: TriggerRule(PS=1.0,  comment='Backup Primary W trigger')},  
 
  #cosmics
#  'EF_tauNoCut_cosmic': {  200: TriggerRule(PS=75)}, Definition moved to LAr Rules
  #1000: TriggerRule(rate=0.5)},
  
  #extra
  'EF_tau29_medium1_tau20_medium1': {1000: TriggerRule(PS=-1.0, comment='Disabled for 3e33')},
       
  # Monitoring/supporting :
  'EF_tau125_IDTrkNoCut': {1000:  TriggerRule(rate=0.4,ESValue=1)},
  'EF_tau29_IDTrkNoCut': {1000:  TriggerRule(rate=0.4,ESValue=1)},
  'EF_tauNoCut_L1TAU50': {1000:  TriggerRule(rate=0.4, ESValue=1)},
  'L2_tauNoCut_L1TAU50': {1000:  TriggerRule(PTRate=0.5)},
  'EF_tauNoCut':      {1000: TriggerRule(rate=0.4,ESValue=1.0)}, #ESValue means PS for ES stream, which is multiplied with rate
  'L2_tauNoCut':      {1000: TriggerRule(PTRate=0.5)},
  
  'EF_tau29_loose':   {#1000: TriggerRule(rerun=True,PS=0),
                       2301: TriggerRule(PS=-1,comment='Disabled for 3e33')},
  'EF_tau20_medium':  {1000: TriggerRule(rerun=True,rate=0.2)},
  'EF_tau20_medium1': {#1000: TriggerRule(rerun=True,rate=0.2),
                       2301: TriggerRule(rerun=True,rate=0.4,ESValue=1,comment='Monitoring')},
  'EF_tau20i_medium':  {1000: TriggerRule(rerun=True,rate=0)},
  'EF_tau20i_medium1': {1000: TriggerRule(rerun=True,rate=0)},
  'EF_tau20T_medium2': {1000: TriggerRule(rerun=True,rate=0)},
  'EF_tau20T_medium':  {1000: TriggerRule(rerun=True,rate=0.2)},
  'EF_tau29_medium':   {1000: TriggerRule(rerun=True,rate=0.4)},
  'EF_tau29T_medium':  {1000: TriggerRule(rerun=True,rate=0.4)},
  'EF_tau29_medium1':  {1000: TriggerRule(rerun=True,rate=0.4)},
  'EF_tau29i_medium':  {1000: TriggerRule(rerun=True,rate=0)},
  'EF_tau29i_medium1': {1000: TriggerRule(rerun=True,rate=0)},
  'EF_tau29T_medium2': {1000: TriggerRule(rerun=True,rate=0)},
  'EF_tau29T_medium1_1P': {1000: TriggerRule(rerun=True,rate=0)},
  
  'EF_tau29_loose1': {3000: TriggerRule(rerun=True,PS=0)},
  'EF_tau29T_medium1_tau20T_medium1': {2301: TriggerRule(PS=1,comment='Extra for 3e33')},
  'EF_tau29Ti_medium1_tau20Ti_medium1': {2301: TriggerRule(PS=1,comment='Extra for 3e33')},
  'EF_tau29Ti_medium1_tau20i_medium1': {3000: TriggerRule(PS=-1,comment='non-affordable')}, 

  #####################################################################
  # MET signature

  # 'L1_XE60': {1000: TriggerRule(rate=0.1,comment='temporary ',)},
  # 'L1_TE300': {1000: TriggerRule(rate=0.1,comment='temporary ',)},
  
  'EF_xe20_noMu': {1000: TriggerRule(rate=0.2,ESRate=0.25,comment='Efficiency measurement',)},
  'EF_xe30_noMu': {1000: TriggerRule(rate=0,maxRate=1.0,comment='_',)},
  'EF_xe40_noMu': {1000: TriggerRule(rate=0.5,maxRate=1.0,comment='2010 primary',)},
  'EF_xe50_noMu': {1000: TriggerRule(rate=0,maxRate=1.0,comment='_',)},#OI default is rate=0
  'EF_xe60_noMu': {#1000: TriggerRule(PS=1.0,comment='primary as of Apr 1st 2011',),
                   2000: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_xe60_tight_noMu': {3000: TriggerRule(PS=-1.0,comment='Disabled due to pileup',)},
  'EF_xe60_verytight_noMu': {1000: TriggerRule(PS=1.0,comment='Priority 1 primary',),
                             2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},
  'EF_xe70_noMu': {1000: TriggerRule(PS=1.0,comment='primary',)},
  'EF_xe70_tight_noMu': {1000: TriggerRule(PS=1.0,comment='primary',)},
  'EF_xe80_noMu': {1000: TriggerRule(PS=1.0,comment='primary',)},
  'EF_xe80_tight_noMu': {1000: TriggerRule(PS=1.0,comment='primary',)},
  'EF_xe90_noMu': {1000: TriggerRule(PS=1.0,comment='Baseline primary;should not exceed 1 Hz at 1e33',)},
  'EF_xe90_tight_noMu': {1000: TriggerRule(PS=1.0,comment='Baseline primary;',)},
  'EF_xe100_noMu': {1000: TriggerRule(PS=1.0,comment='primary',)},
  'EF_xe100_tight_noMu': {1000: TriggerRule(PS=1.0,comment='primary',)},
  'EF_xe110_noMu': {1000: TriggerRule(PS=1.0,comment='primary',)},

  
  'L2_xeL2Mon': {500: TriggerRule(PS=-1,comment='Commissioning chain')},
  'L2_xeL2Mon_L1XE50': {500: TriggerRule(PS=-1,comment='Commissioning chain')},
  'L2_xeL2Mon_L1RD0FILLED': {500: TriggerRule(PS=-1,comment='Commissioning chain')},
  'L2_xe_NoCut_allL1': {500: TriggerRule(PS=0,rerun=True,comment='Monitoring chain')},
  'EF_xe_NoCut_allL1': {500: TriggerRule(PS=0,rerun=True,comment='Monitoring chain')},
  
  'EF_te550': {1000: TriggerRule(rate=0.,maxRate=1.0,comment='_',)},
  'EF_te700': {1000: TriggerRule(rate=0.05,maxRate=0.10,comment='_',)},
  'EF_te900': {1000: TriggerRule(rate=0.05,maxRate=0.10,comment='Extra for 3e33',),},
  'EF_te1000': {1000: TriggerRule(rate=0.,maxRate=1.0,comment='_',)}, 
  'EF_te1100': {1000: TriggerRule(rate=0.,maxRate=1.0,comment='_',)},
  'EF_te1200': {1000: TriggerRule(PS=1.0,comment='Baseline primary;should not exceed 1 Hz at 1e33',)},

  ################################################################
  # Bphysics signature
  
  #'L1_MU0':            { 1000: TriggerRule(rate=5000, comment='temporary Baseline primary BPhys')}, #OI This need care!

  'L1_MU4':            { 1501: TriggerRule(rate=200, comment='end-of-fill primary BPhys'),
                         1250: TriggerRule(rate=2500, comment='end-of-fill primary BPhys'),
                         1000: TriggerRule(rate=5000, comment='end-of-fill primary BPhys'),
                         100:  TriggerRule(rate=7500, comment='end-of-fill primary BPhys'),
                         }, 
  'L1_MU6':            {   100: TriggerRule(PS=1.0, comment='end-of-fill Baseline primary BPhys'),
                           800: TriggerRule(rate=7500, comment='end-of-fill Baseline primary BPhys'),
                           1250: TriggerRule(rate=5000, comment='Rate Control;end-of-fill Baseline primary BPhys'),
                           1501: TriggerRule(rate=2000, comment='Rate Control;end-of-fill Baseline primary BPhys'),
                         },
  'L2_mu4T_Upsimumu_FS':{ 100: TriggerRule(PS=-1.0, comment='disabled 83012')},
  'L2_2mu4T_DiMu':      { 200: TriggerRule(PTRate=1.0,  comment='Priority 1 primary BPhys')},
  'EF_2mu4T_DiMu':      { #1000: TriggerRule(PS=1.0,  comment='Priority 1 primary BPhys'),
                         2301: TriggerRule(rate=1.0,comment='Prescaled for 3e33')},
  'EF_2mu4T_DiMu_SiTrk':{ 1000: TriggerRule(PS=-1, comment=' Tracking studies, in RATE:CPS:BphysDiMu')},  # should be in CPS
  'EF_2mu4T_Jpsimumu':  { 1000: TriggerRule(PS=1.0, comment='Baseline primary BPhys'),
                         2301: TriggerRule(PS=1.0, comment='Extra for 3e33'),}, #set PS=10 instead of disabling it at 5e33 (#86904)
  'EF_2mu4T_Upsimumu':  { 1000: TriggerRule(PS=1.0, ESRate=0.1,comment='Baseline primary BPhys'),},
  'EF_2mu4T_Bmumu':     { 1000: TriggerRule(PS=1.0, comment='Baseline primary BPhys'),},
  'EF_2mu4T_Bmumux':    { 1000: TriggerRule(PS=1.0, comment='Baseline primary BPhys'),
                         2301: TriggerRule(PS=1.0, comment='Extra for 3e33'),},

  'EF_mu4Tmu6_Jpsimumu':  { 3000: TriggerRule(PS=1.0, comment='Backup Baseline primary BPhys')},
  'EF_mu4Tmu6_Upsimumu':  { 3000: TriggerRule(PS=1.0, ESRate=0.1,comment='Backup Baseline primary BPhys')},
  'EF_mu4Tmu6_Bmumu':     { 3000: TriggerRule(PS=1.0, comment='Backup Baseline primary BPhys')},
  'EF_mu4Tmu6_Bmumux':    { 3000: TriggerRule(PS=1.0, comment='Backup Baseline primary BPhys')},
  'EF_mu4Tmu6_DiMu':      { 1000: TriggerRule(PS=1.0, comment='Backup Baseline primary BPhys'),
                           2301: TriggerRule(PS=1.0,  comment='Extra for 3e33')},
  'EF_mu4Tmu6_DiMu_noVtx_noOS':     {500: TriggerRule(PS=-1, comment='support mu4Tmu6 chains when they are primary')},

  'EF_2mu6_DiMu':        { 1000: TriggerRule(PS=1.0,  comment='Baseline primary BPhys')},
  'EF_2mu6_Bmumux':      { 1000: TriggerRule(PS=1.0,  comment='Backup primary BPhys')},


  # Drell-Yann
  'EF_2mu4T_DiMu_DY':      { 1000: TriggerRule(PS=-1.0,comment='use mu4Tmu6 instead')},
  'EF_2mu4T_DiMu_DY20':      { 1000: TriggerRule(PS=-1.0, comment='Priority 2 primary; DY')},
  'EF_2mu4T_DiMu_DY_noVtx_noOS':      { 1000: TriggerRule(PS=-1.0, comment='Priority 2 primary; DY')},
  'EF_2mu4Ti_DiMu_DY':      { 1000: TriggerRule(PS=-1.0, comment='Priority 2 primary; DY')},

  'EF_mu4Tmu6_DiMu_DY20':      { 1000: TriggerRule(PS=1.0, comment='Priority 1 primary; DY'),
                                5000: TriggerRule(rate=4.0,maxRate=8)},
  'EF_mu4Timu6i_DiMu_DY14_noVtx_noOS':      { 1000: TriggerRule(rate=0.5, comment='Priority 1 primary; DY')},
  'EF_mu4Timu6i_DiMu_DY':      { 1000: TriggerRule(rate=2.0, comment='Priority 1 primary; DY')},


    # single mu Bphys

  'EF_mu4T_Upsimumu_FS':       {  100: TriggerRule(PS=-1.0, comment='disabled 83012')},
  'EF_mu4T_Upsimumu_tight_FS': { 1000: TriggerRule(PS=1.0, comment='end-of-fill trigger - controlled by L1, MCG 28/4'),
                                5000: TriggerRule(PS=-1.0)},
  'EF_mu10_Upsimumu_FS':      { 1000: TriggerRule(PS=-1.0, comment='replaced by tight version')},  # prescale if problem with ROS access
  'EF_mu10_Upsimumu_tight_FS':{  500: TriggerRule(PS=1.0, comment='primary, replacing above (introduced in 16.1.2.10)'),
                                2000: TriggerRule(PS=-1.0,comment='End-of-fill trigger')},
  'EF_mu10_Jpsimumu':         { 1000: TriggerRule(PS=1.0, comment='primary BPhys'),
                                2000: TriggerRule(PS=-1.0,comment='End-of-fill trigger')},
  'EF_mu6_Jpsimumu':          {  100: TriggerRule(PS=-1.0,comment='Disabled 83012')},#maxRate=5.,
  'EF_mu6_Jpsimumu_SiTrk':    {  500: TriggerRule(PS=-1, comment=' Tracking studies, in RATE:CPS:Bphysmu6')},

  'EF_mu6_Jpsimumu_tight':   {  1000: TriggerRule(PS=1.0,comment='end-of-fill trigger - controlled by L1, MCG 28/4'),
                                5000: TriggerRule(PS=-1.0)},#maxRate=5.,                                            
  'EF_mu4T_Jpsimumu':   { 250:  TriggerRule(PS=1, comment='supporting BPhys'),
                         500:  TriggerRule(rate=1,maxRate=2, comment='supporting BPhys'),
                         1000: TriggerRule(rate=0,maxRate=2,comment='end-of-fill trigger')},#maxRate=5.,                                            
  'EF_mu4T_DiMu':       { 250:  TriggerRule(PS=1, comment='supporting BPhys'),
                         500:  TriggerRule(rate=1,maxRate=2, comment='supporting BPhys'),
                         1000: TriggerRule(rate=0,maxRate=2,comment='end-of-fill trigger')},#maxRate=5.,                                            
  'EF_mu4T_Trk_Jpsi':             {500: TriggerRule(PS=-1.0, comment='replaced by mu6_Trk_Jpsi')},
  'EF_mu6_Trk_Jpsi_loose':       {500: TriggerRule(rate=3.0, comment='supporting BPhys')},
  'EF_mu4T_Trk_Upsi_FS':          {500: TriggerRule(rate=0.5, comment='supporting BPhys')},
  'EF_2mu4T_Jpsimumu_IDTrkNoCut'  : {500: TriggerRule(rate=1.5,ESRate=0.1)},## Has been generating crashes, remove for now.=> added in 86721
  'EF_mu4Tmu6_Jpsimumu_IDTrkNoCut': {500: TriggerRule(rate=1.5,ESRate=0.1)},
  'EF_mu4T_Upsimumu_SiTrk_FS':    {500: TriggerRule(PS=-1, comment='supporting BPhys;end-of-fill trigger')},
  'EF_2mu13_Zmumu_IDTrkNoCut':   {500: TriggerRule(rate=0.2,ESRate=0.1, comment='monitoring, #78946')},
  'EF_2mu4T_DiMu_noVtx_noOS':     {500: TriggerRule(rate=1.0, comment='supporting BPhys')},
  'EF_mu6_DiMu_noOS':            {500: TriggerRule(rate=0.5, comment='supporting BPhys')},
  'EF_mu4T_DiMu_FS_noOS':         {500: TriggerRule(rate=0.5, comment='supporting BPhys')},


  'EF_4mu4T':    { 3000: TriggerRule(PS=1.0, comment='Baseline primary BPhys')},
  'EF_3mu4T':    { 3000: TriggerRule(PS=1.0, comment='Baseline primary BPhys')},
  'EF_3mu4T_DiMu':    { 3000: TriggerRule(PS=-1.0, comment='Baseline primary BPhys;disabled due to missing overlap removal')},
  'EF_3mu4T_Jpsi':    { 3000: TriggerRule(PS=-1.0, comment='Baseline primary BPhys;disabled due to missing overlap removal')},
  'EF_3mu4T_Upsi':    { 3000: TriggerRule(PS=-1.0, comment='Baseline primary BPhys;disabled due to missing overlap removal')},
  'EF_3mu4T_Tau':     { 3000: TriggerRule(PS=-1.0, comment='Baseline primary BPhys;disabled due to missing overlap removal')},


  
  #######################################################################
  # Jets
  #
  # E/P TRIGGERS
  'EF_hadCalib_trk9':{610:TriggerRule(rate=0.2,comment='isolated pions for hadronic calibration')},
  'EF_hadCalib_trk9phi':{610:TriggerRule(rate=0.2,comment='isolated pions for hadronic calibration')},
    
  # PT
  # RANDOM-SEEDED JETS
  'L2_rd0_filled_NoAlg':{100:TriggerRule(comment='input to low pt jets',rate=500,maxRate=600.),
                         610: TriggerRule(comment='input to low pt jets',rate=300.,maxRate=300.),
                         1000: TriggerRule(comment='RateControl',rate=10.,maxRate=10.),
                         },
  'EF_j15_a2tc_EFFS':    {3000:TriggerRule(comment='Validation of triggers for HI run',rate=0.5)},
  'EF_j15_a2hi_EFFS':    {3000:TriggerRule(comment='Validation of triggers for HI run',rate=0.5)},
  'EF_j20_a2tc_EFFS':    {3000:TriggerRule(comment='Validation of triggers for HI run',rate=0.5)},
  'EF_j20_a2hi_EFFS':    {3000:TriggerRule(comment='Validation of triggers for HI run',rate=0.5)},
  'EF_j20_a4hi_EFFS':    {3000:TriggerRule(comment='Validation of triggers for HI run',rate=0.5)},
  
  'EF_j10_a4tc_EFFS':{610:TriggerRule(comment='QCD jet physics and JES',rate=10,PTRate=0.1)},
  'EF_j10_a4tc_EFFS_1vx':{1000:TriggerRule(PS=-1,comment='not yet signed off')},
  'EF_fj10_a4tc_EFFS':{610:TriggerRule(comment='QCD jet physics and JES',rate=10,PTRate=0.1)},
   #doesn't work  'EF_j10_a4tc_EFFS_1vx':{1000:TriggerRule(comment='QCD jet physics and JES; single vertex; Pending',rate=0.5)},
  'EF_fj10_a4tc_EFFS_1vx':{1000:TriggerRule(PS=-1,comment='not yet signed off')},
  'EF_j15_a4tc_EFFS':{610:TriggerRule(comment='QCD jet physics and JES',rate=10)},
  'EF_fj15_a4tc_EFFS':{610:TriggerRule(comment='QCD jet physics and JES',rate=10)},
  'EF_j20_a4tc_EFFS':{610:TriggerRule(comment='QCD jet physics and JES',rate=10)},
  'EF_fj20_a4tc_EFFS':{610:TriggerRule(comment='QCD jet physics and JES',rate=10)},
  # CENTRAL JETS - low-lumi rules (L=1) are only meant for startup phase
  'L2_j25':{1000:TriggerRule(comment='Force L2 pass through rate for lowest threshold jet trigger',PTRate=0.5)},
  'EF_j30_a4tc_EFFS':{500:TriggerRule(comment='QCD jet physics and jet/calorimeter monitoring',rate=1.0,PTRate=0.2,ESRate=0.4,maxRate=1.0)},
  'EF_j30_a4tc_EFFS_l2cleanph':{500:TriggerRule(comment='QCD jet physics and jet/calorimeter monitoring',depends_on='EF_j30_a4tc_EFFS',fraction=1)},
  'EF_j40_a4tc_EFFS':{500:TriggerRule(comment='QCD jet physics',rate=0.5,maxRate=0.5)},
  'EF_j55_a4tc_EFFS':{500:TriggerRule(comment='QCD jet physics',rate=0.5,maxRate=0.5)},
  'L2_j70':{1000:TriggerRule(comment='Force L2 pass through rate for jet trigger',PTRate=0.2)},
  'EF_j75_a4tc_EFFS':{500:TriggerRule(comment='QCD jet physics',rate=0.5,maxRate=0.5)},
  'L1_J75':{1000:TriggerRule(comment='QCD jet physics; unprescaled trigger for all jet and j+xe triggers',PS=1)},
  # OI can not have L2_j95 PT as EF rates get messed up. Should introduce L1J75_NoAlg 
  #'EF_L1J75_NoAlg':{1000:TriggerRule(comment=' PT for efficiencies and monitoring',rate=0.5)},
  'L2_j95':{1000:TriggerRule(comment='QCD jet physics; unprescaled trigger for all jet and j+xe triggers; ',PS=1, PTRate=0.5)},
  'EF_j100_a4tc_EFFS':{500:TriggerRule(comment='QCD jet physics',rate=0.5,PTRate=0.1,maxRate=0.5)},
  'EF_j135_a4tc_EFFS':{#500:TriggerRule(comment='QCD jet physics; Exotics special request (prescaled by 2, MCG 28/4)',PS=2),
                       1200:TriggerRule(comment='QCD jet physics; Exotics special request',rate=0.5,maxRate=0.5)},
  'EF_j180_a4tc_EFFS':{500:TriggerRule(comment='QCD jet physics and Exotics',rate=0.5,maxRate=0.5)},
  'EF_j240_a4tc_EFFS':{500:TriggerRule(comment='QCD jet physics and Exotics',PS=1, ESRate=0.4),
                       5500:TriggerRule(comment='QCD jet physics and Exotics',rate=0.5,maxRate=12.0,ESRate=0.4)}, 
  'EF_j240_a4tc_EFFS_l2cleanph':{1000:TriggerRule(comment='QCD jet physics and Exotics; never prescaled!',depends_on='EF_j240_a4tc_EFFS',fraction=1)},
  'EF_j320_a4tc_EFFS':{1000:TriggerRule(comment='QCD jet physics and Exotics; never prescaled!',PS=1)},
  'EF_j425_a4tc_EFFS':{1000:TriggerRule(comment='Primary for 5e33',PS=1)},
  # no HLT for searches
  'EF_L1J250_NoAlg':{1000:TriggerRule(comment='QCD jet physics; unprescaled trigger, no HLT',PS=1)},
  'EF_L1J350_NoAlg':{1000:TriggerRule(comment='QCD jet physics; no HLT',PS=1)},
  # FORWARD JETS
  'L2_fj25':{1000:TriggerRule(comment='Force L2 pass through rate for lowest threshold jet trigger',PTRate=0.25)},
  'EF_fj30_a4tc_EFFS':{500:TriggerRule(comment='QCD jet physics and jet/calorimeter monitoring',rate=1.0,PTRate=0.2,ESRate=0.4,maxRate=1.0)},
  'EF_fj30_a4tc_EFFS_l2cleanph':{500:TriggerRule(comment='QCD jet physics and jet/calorimeter monitoring',depends_on='EF_fj30_a4tc_EFFS',fraction=1)},
  'EF_fj55_a4tc_EFFS':{1000:TriggerRule(comment='QCD jet physics',rate=0.5,maxRate=0.5)},
  'EF_fj75_a4tc_EFFS':{1000:TriggerRule(comment='QCD jet physics',rate=0.5,maxRate=0.5)},
  'L1_FJ75':{1000:TriggerRule(comment='QCD jet physics; unprescaled trigger',PS=1)},
  'L2_fj95':{1000:TriggerRule(comment='QCD jet physics; unprescaled trigger for all jet triggers; PT for efficiencies and monitoring',PTRate=0.25,PS=1)},
  'EF_fj100_a4tc_EFFS':{1000:TriggerRule(comment='QCD jet physics; unprescaled trigger',PS=1)},
  'EF_fj135_a4tc_EFFS':{1000:TriggerRule(comment='QCD jet physics; unprescaled trigger',PS=1)},
  'EF_L1FJ75_NoAlg':{#1000:TriggerRule(comment='QCD jet physics; unprescaled trigger, no HLT',PS=1),
                     2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  # FAT JETS
  'EF_j240_a10tc_EFFS':{1000:TriggerRule(comment='Exotics fat jets and boosted top',depends_on='EF_j240_a4tc_EFFS',rate=0.5, maxRate=99 ),
                        2301: TriggerRule(PS=1.0,comment='Extra for 3e33')}, 
  'EF_j320_a10tc_EFFS':{1000:TriggerRule(comment='Exotics fat jets and boosted top; never prescaled!',PS=1)},
  'EF_j425_a10tc_EFFS':{1000:TriggerRule(comment='Exotics fat jets and boosted top; never prescaled!',PS=1)},
  # MULTIJETS
  # Central-forward dijets
  'EF_j30_fj30_a4tc_EFFS':{610:TriggerRule(comment='JES calibration',rate=0.5,maxRate=0.5)},
  'EF_j40_fj40_a4tc_EFFS':{610:TriggerRule(comment='JES calibration',rate=0.5,maxRate=0.5)},
  'EF_j55_fj55_a4tc_EFFS':{610:TriggerRule(comment='JES calibration',rate=0.5,maxRate=0.5)},
  'EF_j75_fj75_a4tc_EFFS':{610:TriggerRule(comment='JES calibration',rate=0.5,maxRate=0.5)}, 
  # central multijets
  'L2_3j25':{610:TriggerRule(comment='Jet Trigger',PTRate=0.1)}, 
  'EF_3j30_a4tc_EFFS':{2:TriggerRule(comment='QCD jet physics and SUSY',rate=0.5,PTRate=0.1,maxRate=0.5)  },
  'EF_4j30_a4tc_EFFS':{2:TriggerRule(comment='QCD jet physics and SUSY',rate=0.5,maxRate=0.5)},
  'EF_5j30_a4tc_EFFS':{#2:TriggerRule(comment='QCD jet physics and SUSY',rate=0.5,maxRate=5.0),
                       2301: TriggerRule(PS=1.0,comment='Extra for 3e33')}, 
  'EF_6j30_a4tc_EFFS':{2:TriggerRule(comment='QCD jet physics and SUSY; unprescaled',PS=1)},
  'EF_6j30_a4tc_EFFS_L15J10':{2:TriggerRule(comment='QCD jet physics and SUSY; unprescaled',PS=1)},
  'EF_7j30_a4tc_EFFS_L16J10':{2:TriggerRule(comment='QCD jet physics and SUSY; unprescaled',PS=1)},
  'EF_7j30_a4tc_EFFS_L15J10':{2:TriggerRule(comment='QCD jet physics and SUSY; unprescaled',PS=1)},
  #random seeded - rate driven by RD0:
  'EF_3j10_a4tc_EFFS': {1000: TriggerRule(comment='QCD jet physics and JES',rate=0.5)},
  'EF_4j10_a4tc_EFFS': {1000: TriggerRule(comment='QCD jet physics and JES',rate=0.5)},
  'EF_5j10_a4tc_EFFS': {1000: TriggerRule(comment='QCD jet physics and JES',rate=0.5)},



  #
  'EF_3j40_a4tc_EFFS':{2:TriggerRule(comment='disabled in favor of nJ30',PS=-1),},
  'EF_4j40_a4tc_EFFS':{2:TriggerRule(comment='disabled in favor of nJ30',PS=-1),},
  'EF_5j40_a4tc_EFFS':{3000:TriggerRule(comment='Baseline primary',PS=1)},
  #'EF_6j40_a4tc_EFFS':{2:TriggerRule(comment='QCD jet physics and SUSY; unprescaled',PS=1)},
  #
  'EF_3j45_a4tc_EFFS':{2:TriggerRule(comment='disabled in favor of nJ40',PS=-1),},
  'EF_5j45_a4tc_EFFS':{10:TriggerRule(comment='disabled in favor of nJ40',PS=1)},  # 
  #                     3000:TriggerRule(comment='QCD jet physics and SUSY',rate=0.5,maxRate=5.0)},
  #'EF_6j45_a4tc_EFFS':{#2:TriggerRule(comment='disabled in favor of nJ40',PS=-1),
  #                     3000:TriggerRule(comment='QCD jet physics and SUSY; unprescaled',PS=1)},

  # EF_4j45 is enabled to enhance the SUSY searches normally associated with EF_4j55
  'L2_4j40':{1000:TriggerRule(comment='SUSY',PTRate=0.2)},
  'EF_4j45_a4tc_EFFS':{#1000:TriggerRule(comment='QCD jet physics and SUSY',rate=0.5,maxRate=5.0),
                       2301: TriggerRule(PS=1.0,comment='Extra for 3e33')}, 
  'L2_4j50':{1000:TriggerRule(comment='SUSY',PTRate=0.2,PS=1)},
  'EF_4j55_a4tc_EFFS':{1000:TriggerRule(comment='SUSY baseline',PS=1)},
  'EF_4j60_a4tc_EFFS':{1000:TriggerRule(comment='SUSY backup',PS=1)},
  'L2_3j70':{1000:TriggerRule(comment='Exotics black hole search',PTRate=0.2)},
  'EF_3j75_a4tc_EFFS':{#1000:TriggerRule(comment='Exotics black hole search',PS=1),
                       2301: TriggerRule(PS=-1,comment='Disabled for 3e33')},
  'EF_3j100_a4tc_EFFS':{1000:TriggerRule(comment='Exotics black hole search',PS=1)},
  'EF_3j100_a4tc_EFFS_L1J75':{1000:TriggerRule(comment='Exotics black hole search',PS=1)},

  ## deta
  #OI too high rate of 2fj30* at L1
  'L1_J10_FJ10':{   1:TriggerRule(comment='has to limitf for ROS access',rate=1000,maxRate=1000),
                 1000:TriggerRule(comment='has to limitf for ROS access',rate=500,maxRate=500),
                 1300:TriggerRule(comment='has to limitf for ROS access',rate=100,maxRate=100)},
  'L1_FJ10'    :{   1:TriggerRule(comment='has to limitf for ROS access',rate=1000,maxRate=1000),
                 1000:TriggerRule(comment='has to limitf for ROS access',rate=500,maxRate=500),
                 1300:TriggerRule(comment='has to limitf for ROS access',rate=100,maxRate=100)},
  'L1_2FJ10'   :{   1:TriggerRule(comment='has to limitf for ROS access',rate=1000,maxRate=1000),
                 1000:TriggerRule(comment='has to limitf for ROS access',rate=500,maxRate=500),
                 1300:TriggerRule(comment='has to limitf for ROS access',rate=100,maxRate=100)},
  'EF_2fj30_a4tc_EFFS_deta50_FB':{1000:TriggerRule(comment='QCD jet physics; savannah #81609',rate=0.25,maxRate=0.5)},
  'EF_2fj30_a4tc_EFFS_deta50_FC':{1000:TriggerRule(comment='QCD jet physics; savannah #81609',rate=0.25,maxRate=0.5)},
  'EF_2fj55_a4tc_EFFS_deta50_FB':{1000:TriggerRule(comment='QCD jet physics; savannah #81609',rate=0.25,maxRate=0.5)},
  'EF_2fj55_a4tc_EFFS_deta50_FC':{1000:TriggerRule(comment='QCD jet physics; savannah #81609',rate=0.25,maxRate=0.5)},
  'EF_2fj75_a4tc_EFFS_deta50_FB':{1000:TriggerRule(comment='QCD jet physics; savannah #81609',rate=0.25,maxRate=0.5)},
  'EF_2fj75_a4tc_EFFS_deta50_FC':{1000:TriggerRule(comment='QCD jet physics; savannah #81609',rate=0.25,maxRate=0.5)},
  'EF_2fj100_a4tc_EFFS_deta50_FB':{1000:TriggerRule(comment='QCD jet physics; savannah #83198',PS=1)},
  'EF_2j100_a4tc_EFFS_deta35_FC':{1000:TriggerRule(comment='QCD jet physics; savannah #81609',rate=0.5,maxRate=1.0)},
  'EF_2j135_a4tc_EFFS_deta35_FC':{1000:TriggerRule(comment='QCD jet physics; savannah #81609',rate=0.5,maxRate=1.0)},
  'EF_2j180_a4tc_EFFS_deta35_FC':{1000:TriggerRule(comment='QCD jet physics; savannah #81609',rate=0.5,maxRate=1.0)},
  'EF_2j240_a4tc_EFFS_deta35_FC':{1000:TriggerRule(comment='QCD jet physics; savannah #83198',PS=1)},

  ## mct
  'L2_j70_2j25_L2anymct100':{1000:TriggerRule(comment='Force L2 pass through rate for SUSY trigger',PTRate=0.2)},
  'EF_j75_j30_a4tc_EFFS_anymct150':{#1000:TriggerRule(comment='Priority 1; SUSY ',PS=1, PTRate=0.2),
                                    2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')}, 
  'EF_j75_j30_a4tc_EFFS_anymct175':{#1000:TriggerRule(comment='Primary;SUSY',PS=1),
                                    2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_2j45_a4tc_EFFS_leadingmct100_xe40_medium_noMu':{3000:TriggerRule(comment='Primary;SUSY',PS=1)},
  'EF_2j55_a4tc_EFFS_leadingmct100_xe40_medium_noMu':{3000:TriggerRule(comment='Backup Primary;SUSY',PS=1)},
  # dphi

  'EF_j135_j30_a4tc_EFFS_dphi04':{1000:TriggerRule(comment='SUSY support',rate=0.5)},
  'EF_j180_j30_a4tc_EFFS_dphi04':{1000:TriggerRule(comment='SUSY support',rate=0.5)},
  # dphijxe
  'L2_j60_xe45_noMu':{1000:TriggerRule(comment='SUSY support',PTRate=0.2)},
  'EF_j65_a4tc_EFFS_xe65_noMu_dphi2j30xe10':{1000:TriggerRule(comment='Primary;SUSY',PS=1)},
  'EF_j55_a4tc_EFFS_xe55_medium_noMu_dphi2j30xe10':{1000:TriggerRule(comment='Primary;SUSY',PS=1),
                                                    2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},
  'EF_j55_a4tc_EFFS_xe55_medium_noMu_dphi2j30xe10_l2cleancons':{1000:TriggerRule(comment='jet development',depends_on='EF_j55_a4tc_EFFS_xe55_medium_noMu_dphi2j30xe10',fraction=1)},
  # ht
  'EF_j75_2j30_a4tc_EFFS_ht350':{  #1000:TriggerRule(comment='Priority 2; SUSY',PS=1),
                                   2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')}, 

    # Keep this as long as possible at 2e33. Replace with j100_ht400 when not    afforded anymore
  'EF_j100_a4tc_EFFS_ht350':{ #1000:TriggerRule(comment='Primary; SUSY',PS=1),
                              2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},

    # Keep this as long as possible at 3e33. Replace with j100_ht500 when not afforded anymore
  'EF_j100_a4tc_EFFS_ht400':{1000:TriggerRule(comment='Primary; SUSY',PS=1),
                             2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},

# Keep this as long as possible. Replace with j135_ht500, which should have very small rate even at 5e33.
    'EF_j100_a4tc_EFFS_ht500':{1000:TriggerRule(comment='Primary; SUSY',PS=1),},
                               #4500:TriggerRule(comment='4e33 Priority 1; SUSY',PS=-1),},

    'EF_j135_a4tc_EFFS_ht500':{1000:TriggerRule(comment='>5e33 Primary; SUSY',PS=1),},

    # Complementary, increases significantly acceptance on top of a single jet
    #HT. Keep the following as long as possible, replace with 4j40_ht400, which
    #will have very small rate even at 5e33.
  'EF_4j40_a4tc_EFFS_ht350':  {1000:TriggerRule(comment='Primary; SUSY',PS=1),},

  'EF_4j40_a4tc_EFFS_ht400':{1000:TriggerRule(comment='>5e33 Primary; SUSY',PS=1),  },

  'EF_ht350_a4tc_EFFS_L2je255': {1000:TriggerRule(comment='commissioning, large overlap with EF_j100_a4tc_EFFS_ht350',depends_on='EF_j100_a4tc_EFFS_ht350',fraction=1)},
  
  #OI does not exist yet 'EF_j100_a4tc_EFFS_ht400':{1000:TriggerRule(comment='SUSY, Pending',PS=1)},
  # je
  #'EF_j10_a4tc_EFFS_L2je135':{1000:TriggerRule(comment='technical validation trigger; Pending',rate=0.1,maxRate=0.1)},
  #'EF_ht195_a4tc_EFFS_L2je195':{1000:TriggerRule(comment='technical validation trigger; Pending',rate=0.1,maxRate=0.1)},
  #'EF_ht350_a4tc_EFFS_L2je255':{1000:TriggerRule(comment='SUSY; Pending',rate=10.0,maxRate=10.0)},
  #'EF_ht500_a4tc_EFFS_L2je350':{1000:TriggerRule(comment='Exotics; Pending',rate=0.0,maxRate=10.0)},
  #'EF_ht600_a4tc_EFFS_L2je350':{1000:TriggerRule(comment='Exotics; Pending',PS=1)},

  
  #######################################################
  #  j+xe triggers 
  'L2_j70_xe25_loose_noMu':{1000:TriggerRule(comment='Force L2 pass through rate for SUSY trigger',PTRate=0.3)},
  'L2_j70_xe20_loose_noMu':{1000:TriggerRule(comment='Force L2 pass through rate for SUSY trigger',PTRate=0.2)},

  'EF_j75_a4tc_EFFS_xe55_noMu_l2cleancons': {3000 : TriggerRule(comment='Baseline primary',depends_on='EF_j75_a4tc_EFFS_xe55_noMu',fraction=1)},
  'EF_j75_a4tc_EFFS_xe55_noMu'            : {3000 : TriggerRule(PS=1,comment='Baseline primary')},
  'EF_j75_a4tc_EFFS_xe55_loose_noMu'      : {3000 : TriggerRule(PS=-1,comment='Backup')}, #OI when to use it?
  'EF_j75_a4tc_EFFS_xe45_loose_noMu'      : {1000 : TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_j75_a4tc_EFFS_xe40_loose_noMu'      : {200 : TriggerRule(PS=-1,comment='old item')},  
  'EF_j75_a4tc_EFFS_jetNoEF_EFxe25_noMu'  : {1000 : TriggerRule(PS=-1, comment='2010 trigger , not used anymore')},

  'EF_mu4T_j45_a4tc_EFFS_xe45_loose_noMu':  {3000: TriggerRule(PS=1.0,comment='Primary; SUSY')},
  'EF_mu4T_j45_a4tc_EFFS_xe55_noMu':        {3000: TriggerRule(PS=1.0,comment='Backup Primary; SUSY')}, 
  'EF_j80_a4tc_EFFS_xe60_noMu'            : {5000 : TriggerRule(PS=1,comment='Baseline primary')},
  
    
  ############################################################################
  # Bjets
  'EF_mu4_j10_a4tc_EFFS':   {100 : TriggerRule(PS=-1.,comment='Disabled according to #82420')},
  'EF_mu4_j10_a4tc_EFFS_matched':{100 : TriggerRule(rate=1.,comment='Baseline prescaled primary'), 
                                  500 : TriggerRule(rate=1.3,comment='Baseline prescaled primary'),
                                  1000 : TriggerRule(rate=1.0,comment='Baseline prescaled primary'),
                                  }, 
  'EF_mu4_L1J10_matched': {200 : TriggerRule(rate=1.,ESRate=0.4,comment='Baseline prescaled primary'),
                           500 : TriggerRule(rate=1.3,ESRate=0.4,comment='Baseline prescaled primary'),
                           1000 : TriggerRule(rate=1,ESRate=0.4,comment='Baseline prescaled primary'),
                           },
  'EF_mu4_L1J15_matched': { 200 : TriggerRule(PS=-1,comment='backup for mu4_L1J20_matched')},
  'EF_mu4_L1J20_matched': { 200 : TriggerRule(rate=0.3,comment='Baseline prescaled primary'),
                            500 : TriggerRule(rate=0.5,comment='Baseline prescaled primary'), },
  'EF_mu4_L1J30_matched': { 200 : TriggerRule(rate=0.7,comment='Baseline prescaled primary'),
                            500 : TriggerRule(rate=0.5,comment='Baseline prescaled primary')},
  'EF_mu4_L1J50_matched': {200 : TriggerRule(rate=1.,comment='Baseline prescaled primary'),
                           500 : TriggerRule(rate=0.8,comment='Baseline prescaled primary')},
  'EF_mu4_L1J75_matched': {200 : TriggerRule(rate=1.,comment='Baseline prescaled primary'),
                           500 : TriggerRule(rate=0.8,comment='Baseline prescaled primary')},
  #'EF_mu4_j100_a4tc_EFFS_L1matched' : {1000 : TriggerRule(rate=1.,comment='Baseline prescaled primary for b-tagging calibration')},
  'EF_mu4_j135_a4tc_EFFS_L1matched' : {  200 : TriggerRule(rate=1.,comment='Baseline prescaled primary for b-tagging calibration'),
                                         500 : TriggerRule(rate=0.8,comment='Baseline prescaled primary for b-tagging calibration')},
  'EF_mu4_j180_a4tc_EFFS_L1matched': {  1000 : TriggerRule(rate=1.5,comment='Baseline prescaled primary')},

  'EF_mu22_j30_a4tc_EFFS':           {  3000 : TriggerRule(PS=1,depends_on='EF_mu22_medium',comment='B-tagging monitoring chain')},
  
  'EF_b10_IDTrkNoCut'   : {1000 : TriggerRule(rate=1, ESRate=0.4,comment='Monitoring')},
  'EF_b10_j30_a4tc_EFFS_IDTrkNoCut'   : {1000 : TriggerRule(rate=1, ESRate=0.4,comment='Monitoring')},
  
  'EF_b10_tight_L1JE140' :     {1000 : TriggerRule(PS=-1,maxRate=10,comment='obsolete')},
  'EF_b10_tight_4L1J10' :      {1000 : TriggerRule(PS=-1,maxRate=10,comment='obsolete')},
  'EF_b10_medium_EFxe25_noMu_L1JE140' : {10  : TriggerRule(PS=-1,maxRate=10,comment='obsolete'),},
  
  
  ##    'EF_2b10_medium_3L1J10' :    {  10 : TriggerRule(PS=1,maxRate=10,comment='Primary trigger;Pending'),
  ##                                   100 : TriggerRule(rate=2,maxRate=10,comment='Primary trigger;Pending')},
  'EF_2b15_medium_3L1J15' :    {1000 : TriggerRule(PS=-1,comment='not requested')},
  'EF_2b20_medium_3L1J20' :    {#1000 : TriggerRule(PS=1,maxRate=10,comment='Need extra info from Higgs;Temporary'),
                                2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},

  'EF_2b10_medium_L1JE100' :   {1000 : TriggerRule(PS=-1,maxRate=10,comment='obsolete')},
  'EF_2b10_medium_L1JE140' :   {1000 : TriggerRule(PS=-1,maxRate=10,comment='obsolete')},
  'EF_2b10_medium_L1_2J10J50' :   {#1000 : TriggerRule(PS=1,maxRate=10,comment='Primary trigger for SUSY with 0-lepton searches;Temporary'),
                                   2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_2b10_medium_4L1J10':     {#1000 : TriggerRule(PS=1,comment='Baseline Primary triggerfor Higgs'),
                                2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},
  'EF_3b10_loose_4L1J10' :     {1000 : TriggerRule(PS=-1,maxRate=10,comment='not requested')},
  'EF_3b15_loose_4L1J15' :     {#1000 : TriggerRule(PS=1,maxRate=10,comment='Primary trigger;'),
                                2301: TriggerRule(PS=-1.0,comment='Disabled for 3e33')},

  'EF_2b10_medium_j75_j30_a4tc_EFFS' : {3000 : TriggerRule(PS=1,comment='Primary trigger for SUSY')},
  #'EF_2b15_medium_3j40_a4tc_EFFS' : {3000 : TriggerRule(PS=1,comment='Primary trigger ')},
  'EF_b10_medium_4j30_a4tc_EFFS' : {1000 : TriggerRule(PS=1,comment='Primary trigger for hadronic top'),
                                    2301: TriggerRule(PS=1.0,comment='Extra for 3e33')},
  'EF_b10_medium_j75_j55_2j30_a4tc_EFFS':{3000 : TriggerRule(PS=1,comment='Primary trigger for hadronic top'),},
  'EF_2b10_medium_4j30_a4tc_EFFS' : {3000 : TriggerRule(PS=1,comment='Primary trigger for hadronic top')},

  
#####  'EF_b10_EFj10_a4tc_EFFS_IDTrkNoCut' : {1000 : TriggerRule(rate=1,comment='Validation (in CPS with b10_IDTrkNoCut)')},

  #######################################################
  #  Hidden Valley triggers
  
  'EF_2MUL1_l2j30_HV'                    : {1000 : TriggerRule(PS=1,comment='Baseline primary')},
  'EF_l2j30_Trackless_HV'                : {#1000 : TriggerRule(PS=1,comment='Baseline primary'),
                                            #1700 : TriggerRule(PS=-1,comment='Emergency for 2e33'),
                                            3000 : TriggerRule(PS=-1,comment='Migrate to MU10_J15') },
  'EF_l2j30_Trackless_HV_L1MU10'         : {3000 : TriggerRule(PS=1,comment='Primary for 3e33'),},
  'L1_MU10_J10'      : {1000 : TriggerRule(PS=1,comment='high lumi backup for Trackless')},

       
  'EF_2MUL1_l2j30_HV_unpaired_iso'       : {1000 : TriggerRule(rate=0.2,comment='Background monitoring')}, #needs signoff
  'EF_2MUL1_l2j30_HV_unpaired_noniso'    : {1000 : TriggerRule(rate=0.2,comment='Background monitoring')}, #needs signoff
  'EF_2MUL1_l2j30_HV_firstempty'         : {1000 : TriggerRule(PS=1.0,comment='Background monitoring')}, #no rate estimate PJB 80176
                                                            #rate=0.2,comment='Background monitoring')}, #needs signoff
  #'EF_2MUL1_l2j30_HV_cosmic'            : {1000 : TriggerRule(rate=0.2,comment='Background monitoring')}, #needs signoff


  'EF_j35_a4tc_EFFS_L1TAU_HVtrk'           : {1000 : TriggerRule(PS=1,comment='Old Baseline primary')},
  'EF_j35_a4tc_EFFS_L1TAU_HVtrk_LOF'       : {1000 : TriggerRule(PS=1,comment='Baseline primary')},
  'EF_j35_a4tc_EFFS_L1TAU_HVtrk_cosmic': { 100 : TriggerRule(PS=1, comment='HV background trigger, can be prescaled'), },
  'EF_j35_a4tc_EFFS_L1TAU_HVtrk_unpaired_iso'    : {1000 : TriggerRule(rate=0.2,comment='Background monitoring')}, #needs signoff
  'EF_j35_a4tc_EFFS_L1TAU_HVtrk_unpaired_noniso' : {1000 : TriggerRule(rate=0.2,comment='Background monitoring')}, #needs signoff
  'EF_j35_a4tc_EFFS_L1TAU_HVtrk_firstempty'      : {1000 : TriggerRule(PS=1.0,comment='Background monitoring')}, #no rate estimate PJB 80176
                                                                 #rate=0.2,comment='Background monitoring')}, #needs signoff
  #'EF_j35_L1TAU_HV_jetNoEF_cosmic'           : {1000 : TriggerRule(rate=0.2,comment='Background monitoring')}, #needs signoff

      
  'EF_l2j30_Trackless_HV_unpaired_iso'     : {1000 : TriggerRule(rate=0.2,comment='Background monitoring')}, #needs signoff
  'EF_l2j30_Trackless_HV_unpaired_noniso'  : {1000 : TriggerRule(rate=0.2,comment='Background monitoring')}, #needs signoff
  'EF_l2j30_Trackless_HV_firstempty'       : {1000 : TriggerRule(PS=1.0,comment='Background monitoring')}, #no rate estimate PJB 80176
                                                              #rate=0.2,comment='Background monitoring')}, #needs signoff
  #'EF_l2j30_Trackless_HV_cosmic'          : {1000 : TriggerRule(rate=0.2,comment='Background monitoring')}, #needs signoff

  


  ############################################################
  # XS items, Compensating for wrong rate predictions.
  ############################################################
  
    ##Aiming for 4-8 Hz per chain
  #'L1_XS15': {500: TriggerRule(PS=30000,comment="for popoulating XS thresholds") },
  #'L1_XS20': {500: TriggerRule(PS=20000,comment="for popoulating XS thresholds") },
  #'L1_XS25': {500: TriggerRule(PS=10000,comment="for popoulating XS thresholds") },
  #'L1_XS30': {500: TriggerRule(PS=4000,comment="for popoulating XS thresholds") },
  #'L1_XS35': {500: TriggerRule(PS=1400,comment="for popoulating XS thresholds") },
  #'L1_XS40': {500: TriggerRule(PS=400,comment="for popoulating XS thresholds") },
  #'L1_XS45': {500: TriggerRule(PS=200,comment="for popoulating XS thresholds") },
  #'L1_XS50': {500: TriggerRule(PS=50,comment="for popoulating XS thresholds") },



  'EF_xs120_noMu': {1000: TriggerRule(rate=0.25,comment="rate to be seen")},
  'EF_xs100_noMu': {1000: TriggerRule(rate=0.25,comment="Support as not affordable at high lumi")},
                    #500: TriggerRule(PS=-1,comment="Primary"),
                    #3000: TriggerRule(PS=-1.,comment="Can not afford at L1") },
  'EF_xs80_noMu': {500: TriggerRule(PS=-1,comment="not requested?"), }, #OI
  'EF_xs90_noMu': {500: TriggerRule(PS=-1,comment="not requested?"), }, #OI
  'EF_xs30_noMu': {500: TriggerRule(rate=0.25,comment="Support") },
  'EF_xs45_noMu': {500: TriggerRule(rate=0.25,comment="Support") },
  'EF_xs60_noMu': {500: TriggerRule(rate=0.25,comment="Support") },
  'EF_xs75_noMu': {500: TriggerRule(rate=0.25,comment="Support") }, 

  'EF_xs70_noMu': {500: TriggerRule(PS=-1.,comment="Commissioning of xs HLT"), },
  'EF_xs70_tight_noMu': {500: TriggerRule(PS=-1,comment="Commissioning of xs HLT"), },



  ############################################################
  # minbias
  ############################################################
  
  'L1_TE20': {1000: TriggerRule(PS=-1,comment="for ridge CMS-like; end-of-fill trigger")          },
  'EF_mbSpTrkVtxMh': { 100 : TriggerRule(PS=-1, comment='high-multiplicity trigger, primary; low-lumi trigger'), }, # OI broken
  'EF_mbSpTrkVtxMh_medium': { 100 : TriggerRule(PS=-1, comment='high-multiplicty trigger, back-up trigger for higher pile up; low-lumi trigger'), }, # OI broken
  'EF_mbSpTrkVtxMh_tight': { 100 : TriggerRule(PS=-1, comment='high-multiplicty trigger, back-up trigger for higher pile up; low-lumi trigger'), }, # OI broken

  
  'EF_mbLucid_eff':      {100: TriggerRule(rate=0.1,comment="TBC"),},# 500: TriggerRule(PS=-1.0,)},
  'EF_InDetMon_FS':      {100: TriggerRule(rate=0.4,ESValue=1,comment="Trigger monitoring")},
  #OI removed 'EF_mbMbts_1_eff':     {100: TriggerRule(rate=0.25,ESValue=0,comment="old primary")},
  'EF_mbZdc_eff':        {100: TriggerRule(rate=0.1,comment="standard Minbias analysis")},
  
  'L1_RD0_FILLED':       {100: TriggerRule(rate=2000,scaling="bunches")},  
  'L1_RD1_FILLED':       {100: TriggerRule(rate=130,scaling="bunches")},

  'L1_RD0_FIRSTEMPTY':       {100: TriggerRule(rate=1,scaling="bunches")},
  
  'EF_mbSpTrk':           {100: TriggerRule(rate=0.1,comment="standard Minbias analysis")},
  'EF_rd0_filled_NoAlg':  {100: TriggerRule(rate=5,ESRate=1.0,comment="standard Minbias analysis"), 
                           1000: TriggerRule(rate=5,ESRate=1.0,comment="standard Minbias analysis")},
  'EF_mbRmMbts_1_NoCut_Time': {100: TriggerRule(rate=0.1,comment="standard Minbias analysis")},
    
  'L1_BCM_Wide_BGRP0': {100: TriggerRule(PS=600000)}, 
  'EF_mb_BCM_Wide_eff': {100: TriggerRule(PS=1500)}, 
  #     'EF_mb_BCM_HT_eff': {100: TriggerRule(PS=1000000)},
  #     'EF_mb_BCM_AC_CA_eff': {100: TriggerRule(PS=200000)},
  
  
  'L1_RD0_UNPAIRED_ISO': {100: TriggerRule(rate=1)},
#  'L1_RD0_UNPAIRED_ISO': {100: TriggerRule(PS=60000)},
  'EF_mbSpBg_unpaired_iso': {100: TriggerRule(PS=1)},
  'EF_mbRd0_eff_unpaired_iso': {100: TriggerRule(PS=10)},
  'EF_mbSpTrk_unpaired_iso': {100: TriggerRule(PS=1)},
  
  ############################################################
  # Cosmic, detector, etc
  
  #L1 items
  'L1_RD0_EMPTY' : { 100 : TriggerRule(rate=1,scaling="bunches",comment='cosmic'), }, 
  'L1_TRT' : {100: TriggerRule(PS=-1, comment='primary for cosmic with TRT Fast Or') },
  #HLT
  'L2_PixelBeam' : {100: TriggerRule(PS=-1, comment='luminosity calibration. Disabled 12Sep2011.')    },
  'L2_PixelNoise' : {100: TriggerRule(rate=10, comment='calibration for InDet')    },
  'L2_SCTNoise' : {100: TriggerRule(rate=10, comment='calibration for InDet')    },
  'EF_rd0_empty_NoAlg' : {100: TriggerRule(rate=1,ESRate=0.5, comment='streamer for L1_RD0_EMPTY')    },
  'EF_L1TRT_NoAlg' : {100: TriggerRule(PS=-1, comment='streamer for L1_TRT')    },
#  'EF_PreS_L1EM3_empty' : {100: TriggerRule(PS=-1, comment='rejection of pre-sampler noise in LAr, during cosmic') },
#  'EF_PreS_L1EM5_empty' : {100: TriggerRule(PS=10, comment='rejection of pre-sampler noise in LAr') },
#  'EF_PreS_L1J10_empty' : {100: TriggerRule(PS=10, comment='rejection of pre-sampler noise in LAr') },
#  'EF_PreS_L1J30_empty' : {100: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr, backup for L1_J10') },
#  'EF_PreS_L1TAU8_empty' : {100: TriggerRule(PS=90, comment='rejection of pre-sampler noise in LAr') },
#  'EF_PreS_L1J10_firstempty' : {100: TriggerRule(PS=50, ESValue=10, comment='rejection of pre-sampler noise in LAr') },
  'EF_PreS_L1J30_firstempty' : {100: TriggerRule(PS=10, ESValue=0, comment='rejection of pre-sampler noise in LAr, physics chain') },
  'EF_mu4_cosmic' : {100: TriggerRule(PS=-1, comment='cosmic chain for muons')    },
  'EF_mu4_MSonly_cosmic' : {100: TriggerRule(PS=-1,  comment='cosmic chain for muons') },
  'EF_mu4_L1MU11_cosmic' : {100: TriggerRule(PS=1, comment='cosmic chain for muons')    }, #to match online
  #rate=0.1, comment='cosmic chain for muons')    },
  #'EF_mu4_L1MU11_MSonly_cosmic' : {100: TriggerRule(rate=0.1, comment='cosmic chain for muons')    },
  'EF_mu4_L1MU11_MSonly_cosmic' : {100: TriggerRule(PS=-1, comment='cosmic chain for muons')    },
  'EF_TileCalib_laser' : {100: TriggerRule(PS=1, comment='Tile calibration')    },
  'EF_CosmicsAllTeIDSCAN_AllPhysics_TrkHypo' : {100: TriggerRule(PS=-1,  comment='cosmic chain for InDet studies')    },
  'EF_CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo' : {100: TriggerRule(PS=-1, comment='cosmic chain for InDet studies')    },
  'EF_CosmicsAllTeSiTrack_AllPhysics_TrkHypo' : {100: TriggerRule(PS=-1,  comment='cosmic chain for InDet studies')    },
  'EF_CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo' : {100: TriggerRule(PS=-1,  comment='cosmic chain for InDet studies')    },
  'EF_CosmicsAllTeTRTxK_TRTTrkHypo_Pixel' : {100: TriggerRule(PS=-1,  comment='cosmic chain for InDet studies')    },
  'EF_CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics' : {100: TriggerRule(PS=-1, comment='cosmic chain for InDet studies')    },
  'EF_Cosmics_MU11_TRTxK_TRTTrkHypo_AllID': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
  'EF_Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel': { 100 : TriggerRule(PS=1, comment='cosmic'), },
  'EF_mu11_empty_NoAlg': { 100 : TriggerRule(PS=100000, comment='cosmic'), },
  'EF_CosmicsAllTeEFID' : {100: TriggerRule(PS=-1,comment='cosmic chain for InDet studies')    },
#  'EF_g5_NoCut_cosmic' : { 200: TriggerRule(PS=500, comment='cosmic chain for egamma')    },
#  'EF_g10_NoCut_cosmic' : { 200: TriggerRule(PS=500, comment='cosmic chain for egamma')    },
  #100: TriggerRule(rate=0.1, comment='cosmic chain for egamma')    },


### LAr Menu (savannah #82252)
#  -------------------------- 
#In physics_CosmicCalo: 
  'L1_EM3_EMPTY'  : { 1000: TriggerRule(PS=500, comment='Efficiently disabled during collisions') } ,
  'L1_EM5_EMPTY' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0 - FIX') } ,
  'L1_TAU8_EMPTY' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
  'L1_J10_EMPTY' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
  'L1_FJ10_EMPTY' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
  'L1_J30_EMPTY' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
  'L2_g10_NoCut_cosmic' : { 1300: TriggerRule(PS=120, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') },
  'L2_PreS_L1EM5_empty' : { 1300: TriggerRule(PS=25, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
  'L2_tauNoCut_cosmic' : { 1300: TriggerRule(PS=17, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
  'L2_PreS_L1TAU8_empty' : { 1300: TriggerRule(PS=21, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') }, 
  'L2_L1J10_empty_NoAlg' : { 1300: TriggerRule(PS=10, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
  'L2_PreS_L1J10_empty' : { 1300: TriggerRule(PS=12, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') },
  'L2_L1FJ10_empty_NoAlg' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') }, 
  'L2_L1J30_empty_NoAlg' : { 1300: TriggerRule(PS=14, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
  'L2_PreS_L1J30_empty' : { 1300: TriggerRule(PS=14, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') },
#-------------------------- 
#In calibration_LArCellsEmpty:
  'L2_em3_empty_larcalib'  : { 1000: TriggerRule(PS=-1, comment='Disabled during collisions') } ,
  'L2_em5_empty_larcalib' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') }, 
  'L2_tau8_empty_larcalib' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') }, 
  'L2_j10_empty_larcalib' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
  'L2_fj10_empty_larcalib' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') },

#-------------------------- 
#Disabled during stable beams: 
#  'L2_g5_NoCut_cosmic' : { 1000: TriggerRule(PS=-1, comment='Disabled during collisions') } ,
  'EF_g5_NoCut_cosmic'  : { 1000: TriggerRule(PS=-1, comment='Disabled during collisions') } ,
#  'L2_PreS_L1EM3_empty' : {1000: TriggerRule(PS=-1, comment='Disabled during collisions') } ,
  'EF_PreS_L1EM3_empty' : { 1000: TriggerRule(PS=-1, comment='Disabled during collisions') } ,
#-------------------------- 
#All EF-equivalent should therefore have a PS=1, since they are protected by L2: 
  'EF_g10_NoCut_cosmic' : { 1000: TriggerRule(PS=1, comment='cosmic chain for egamma') } ,
  'EF_tauNoCut_cosmic'  : { 1000: TriggerRule(PS=1, comment='cosmic chain for taus') } ,
  'EF_PreS_L1EM5_empty' : { 1000: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr') } ,
  'EF_PreS_L1J10_empty' : { 1000: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr') } ,
  'EF_PreS_L1J30_empty' : { 1000: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr, backup for L1_J10') } ,
  'EF_PreS_L1TAU8_empty'  : { 1000: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr') } ,
  'EF_L1FJ10_empty_NoAlg' : { 1000: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 700b scaled by 0.8') } ,
  'EF_L1J10_empty_NoAlg'  : { 1000: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 700b scaled by 0.8') } ,

  
  #######################################
  # LAr calib
  
#  'L2_em3_empty_larcalib' :  {100: TriggerRule(PS=-1, comment='cosmic chain for LAr, enabled during cosmic run only')    },
#  'L2_em5_empty_larcalib' :  {100: TriggerRule(PS=20, comment='cosmic chain for LAr')    },
#  'L2_tau8_empty_larcalib' : {100: TriggerRule(PS=16, comment='cosmic chain for LAr')    },
#  'L2_j10_empty_larcalib' :  {100: TriggerRule(PS=1, comment='cosmic chain for LAr')    },
#  'L2_fj10_empty_larcalib' : {100: TriggerRule(PS=1, comment='cosmic chain for LAr')    },
#  'L2_L1J10_empty_NoAlg' :   {100: TriggerRule(PS=10, comment='cosmic chain for LAr')    },
#  'L2_L1FJ10_empty_NoAlg' :  {100: TriggerRule(PS=1, comment='cosmic chain for LAr')    },
  
  'EF_g11_etcut_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g11_etcut',comment='LAr PEB calibration stream')},
  'EF_g20_loose_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g20_loose',comment='LAr PEB calibration stream')},
  'EF_g40_loose_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g40_loose',comment='LAr PEB calibration stream')},
  'EF_g60_loose_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g60_loose',comment='LAr PEB calibration stream')},
  'EF_g80_loose_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g80_loose',comment='LAr PEB calibration stream')},
  
  'L2_j25_larcalib'      :  {100: TriggerRule(fraction=1,depends_on='EF_fj30_a4tc_EFFS',comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
  'L2_j50_larcalib'      :  {100: TriggerRule(fraction=1,depends_on='EF_j55_a4tc_EFFS',comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
  'L2_j95_larcalib'      :  {100: TriggerRule(fraction=1,depends_on='EF_j100_a4tc_EFFS',comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
  
  'L2_fj25_larcalib'      :  {100: TriggerRule(fraction=1,depends_on='EF_fj30_a4tc_EFFS',comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
  'L2_fj50_larcalib'      :  {100: TriggerRule(fraction=1,depends_on='EF_fj55_a4tc_EFFS',comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
  
  #######################################
  # ID Calib

  'L2_trk9_Central_Tau_IDCalib' :  {100: TriggerRule(PS=12, comment='IDTracks')    },
  'L2_trk9_Fwd_Tau_IDCalib'  :  {100: TriggerRule(PS=12, comment='IDTracks')    },
  'L2_trk16_Central_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },
  'L2_trk16_Fwd_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },
  'L2_trk29_Central_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },
  'L2_trk29_Fwd_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },
  
  #######################################
  
  'EF_L1J30_empty_NoAlg'        :{100:  TriggerRule(PS=1, comment='Baseline primary; long-lived particles')    },
  'EF_L1J10_firstempty_NoAlg'   :{#real rule##100:  TriggerRule(rate=1.0,ESRate=1.0, comment='Calorimeter noise monitoring'),
                                  500: TriggerRule(PS=2.5,ESRate=0.1,scaling="bunches", comment="beam background")}, #temporary hack
  'EF_L1J10_unpaired_noniso_NoAlg'     :{#real rule##100:  TriggerRule(rate=0.5, comment='Baseline primary; long-lived particles'),
    500:  TriggerRule(PS=1, comment='Baseline primary; long-lived particles')    },#temporary hack
  'EF_L1J10_unpaired_iso_NoAlg'     :{#real rule##100:  TriggerRule(rate=0.5, comment='Baseline primary; long-lived particles'),
    500:  TriggerRule(PS=1, comment='Baseline primary; long-lived particles')    },#temporary hack

  'EF_L1J30_firstempty_NoAlg'   :{#real rule##100:  TriggerRule(rate=1.0,ESRate=1.0, comment='Baseline primary; long-lived particles')
    500:  TriggerRule(PS=10,ESRate=0.1, comment='Baseline primary; long-lived particles')    },#temporary hack
  'EF_L1J30_unpaired_noniso_NoAlg'     :{#real rule##100:  TriggerRule(rate=0.5, comment='Baseline primary; long-lived particles'),
    500:  TriggerRule(PS=1, comment='Baseline primary; long-lived particles')    },#temporary hack
  'EF_L1J30_unpaired_iso_NoAlg'     :{#real rule##100:  TriggerRule(rate=0.5, comment='Baseline primary; long-lived particles'),
    500:  TriggerRule(PS=1, comment='Baseline primary; long-lived particles')    },#temporary hack
  
  
  
  'EF_ROBRequestMon': { 100 : TriggerRule(PS=-1, comment='test trigger'), },
  'EF_rd1_empty_NoAlg': { 100 : TriggerRule(rate=1.0, comment='random trigger'), }, 
  'EF_mbRd1_eff': { 100 : TriggerRule(rate=0.5, comment='random triggers'), },     
  
  'EF_L1Muon_NoAlg': {    100 : TriggerRule(PS=-1, comment='Streamers'), },
  'EF_L1Calo_NoAlg': {    100 : TriggerRule(PS=-1, comment='Streamers'), }, 
  'EF_L1MinBias_NoAlg': { 100 : TriggerRule(PS=-1, comment='Streamers'), },
  
  'L2_TRTMon_empty':  {500:TriggerRule(comment='Monitoring - rate=1',rate=1.0)},
  'L2_TRTMon_filled': {500:TriggerRule(comment='Monitoring',rate=1)},

  'L2_VdM_BGRP7' : {500:TriggerRule(PS=-1, comment='Monitoring')},
  'L2_VdM_MBTS_2_BGRP7' : {500:TriggerRule(PS=-1, comment='Monitoring')},
  
  # Beamspot
  'L2_vtxbeamspot_activeTE_SiTrk_peb': { 500 : TriggerRule(rate=3), },
  'L2_vtxbeamspot_FSTracks_SiTrk'    : { 500 : TriggerRule(inputRate=1000), },
  'L2_vtxbeamspot_allTE_SiTrk_peb'   : { 500 : TriggerRule(rate=30), },
## remove if CPU in XPUs reaches limits -->
  'L2_vtxbeamspot_activeTE_IDScan_peb': { 500 : TriggerRule(PS=-1,comment="For studies only"), },
  'L2_vtxbeamspot_FSTracks_IDScan'    : { 500 : TriggerRule(PS=-1,comment="For studies only"), },
  #removed 'L2_vtxbeamspot_allTE_IDScan_peb'   : { 500 : TriggerRule(rate=30), },
  'L2_vtxbeamspot_activeTE_SiTrk_peb_4L1J10': { 1000: TriggerRule(PS=-1,comment="For studies only"), },
  'L2_vtxbeamspot_allTE_SiTrk_peb_4L1J10':    { 1000: TriggerRule(PS=-1,comment="For studies only"), },
  'L2_vtxbeamspot_activeTE_SiTrk_peb_L12J10_J50': { 1000: TriggerRule(PS=-1,comment="For studies only"), },
  'L2_vtxbeamspot_allTE_SiTrk_peb_L12J10_J50':    { 1000: TriggerRule(PS=-1,comment="For studies only"), },
# Switch to these rules for one run
#   'L2_vtxbeamspot_activeTE_SiTrk_peb_4L1J10': { 1000: TriggerRule(PS=-1,comment="For studies only"), },
#   'L2_vtxbeamspot_allTE_SiTrk_peb_4L1J10':    { 1000: TriggerRule(inputRate=30,comment="For studies only"), },
#   'L2_vtxbeamspot_activeTE_SiTrk_peb_L12J10_J50': { 1000: TriggerRule(PS=-1,comment="For studies only"), },
#   'L2_vtxbeamspot_allTE_SiTrk_peb_L12J10_J50':    { 1000: TriggerRule(inputRate=30,comment="For studies only"), },
  

  # predictions are lower by 30% so correct for it here  
  #'L2_vtxbeamspot_activeTE_SiTrk_peb_MBTS': { 200 : TriggerRule(rate=3), },
  #'L2_vtxbeamspot_FSTracks_SiTrk_MBTS'    : { 500 : TriggerRule(rate=600), },
  #'L2_vtxbeamspot_allTE_SiTrk_peb_MBTS'   : { 500 : TriggerRule(rate=30), },		
  

  ##############################################################
    #  EB chains
    'EF_eb_random':                  { 100 : TriggerRule(PS=1, comment='cosmic'), },
    'EF_eb_physics':                 { 100 : TriggerRule(PS=4, comment='EnhancedBias chains'), },
    'EF_eb_physics_unpaired_iso':    { 100 : TriggerRule(PS=2, comment='EnhancedBias chains'), },
    'EF_eb_physics_firstempty':      { 100 : TriggerRule(PS=300, comment='EnhancedBias chains'), },
    'EF_eb_physics_empty':           { 100 : TriggerRule(PS=150, comment='EnhancedBias chains'), },

    'EF_eb_random_unpaired_iso':     { 100 : TriggerRule(PS=1, comment='EnhancedBias chains'), },
    'EF_eb_random_firstempty':       { 100 : TriggerRule(PS=1, comment='EnhancedBias chains'), },
    'EF_eb_random_empty':            { 100 : TriggerRule(PS=1, comment='cosmic'), },

    'EF_high_eb_physics':            { 100 : TriggerRule(PS=1, comment='EnhancedBias chains'), },
    'EF_eb_physics_noL1PS':          { 100 : TriggerRule(PS=8, comment='EnhancedBias chains'), },  
    'EF_eb_physics_unpaired_noniso': { 100 : TriggerRule(PS=1, comment='EnhancedBias chains'), },

  'L2_L1MU10_firstempty_NoAlg':       { 100: TriggerRule(PS=1.0,comment='L1 passthrough',),},
  'EF_L1MU10_firstempty_NoAlg':       { 100: TriggerRule(PS=1.0,comment='L1 passthrough',),},


  ##############################################################
  # Streamers

  'EF_Standby': { 100 : TriggerRule(PS=-1, comment='Standby streamer'),},
  'L2_Background': { 100 : TriggerRule(PS=1, comment='cosmic'), },
  'EF_Background': { 100 : TriggerRule(PS=1, comment='cosmic'), },
  #'EF_SeededStreamerL1CaloXS': { 100 : TriggerRule(PS=1, comment='Commissioning'), },
  'EF_SeededStreamerL1CaloXS': { 100 : TriggerRule(PS=-1, comment='Commissioning'), },

  'L2_cost_monitor':  { 100: TriggerRule(PS=2000, propagate=False, comment="Trigger monitoring"), },
  'EF_cost_monitor':  { 100: TriggerRule(PS=100, propagate=False, comment="Trigger monitoring"), },

    ##############################################################
    # Set scaling for non-lumi scaling items:


    'L1_2FJ30':   { 100: TriggerRule(PS=1) },
    'L1_2J20_XE20':   { 100: TriggerRule(PS=1) },
    'L1_2MU4':   { 100: TriggerRule(PS=1) },
    'L1_2TAU11':   { 100: TriggerRule(PS=1) },
    'L1_2TAU8_EM10VH':   { 100: TriggerRule(PS=1) },
    'L1_EM10VH_XE30':   { 100: TriggerRule(PS=1) },
    'L1_EM16VH':   { 100: TriggerRule(PS=1) },
    'L1_EM5_MU6':   { 100: TriggerRule(PS=1) },
    #'L1_FJ75':   { 100: TriggerRule(PS=1) },
    'L1_J50_XE35':   { 100: TriggerRule(PS=1) },
    #'L1_J75':   { 100: TriggerRule(PS=1) },
    'L1_MU11':   { 100: TriggerRule(PS=1) },
    'L1_MU4_J20_XE20':   { 100: TriggerRule(PS=1) },
    'L1_TAU15_XE10_3J10':   { 100: TriggerRule(PS=1) },
    'L1_TAU30':   { 100: TriggerRule(PS=1) },
    'L1_TAU8_MU10':   { 100: TriggerRule(PS=1) },
    'L1_3J20':   { 100: TriggerRule(PS=1) },
    'L1_TAU11_2J10_J20_XE20':   { 100: TriggerRule(PS=1) },
    'L1_TAU11_XE20':   { 100: TriggerRule(PS=1) },


  
  'L1_J30_UNPAIRED_NONISO':     {500: TriggerRule(PS=1,scaling="bunches", comment="sgluino searches, beam background") },
  'L1_J30_UNPAIRED_ISO':     {500: TriggerRule(PS=1,scaling="bunches", comment="sgluino searches,beam background") },
  'L1_J30_FIRSTEMPTY':   {500: TriggerRule(PS=1,comment="beam background") },
  'L1_J10_FIRSTEMPTY':   {500: TriggerRule(PS=100,comment="beam background, high rate with pileup") },
  'L1_MU4_FIRSTEMPTY':   {500: TriggerRule(PS=80,scaling="bunches", comment="beam background") },
  'L1_MU6_FIRSTEMPTY':   {500: TriggerRule(PS=250,scaling="bunches", comment="beam background") },
  'L1_2MU6_FIRSTEMPTY':   {500: TriggerRule(PS=4,scaling="bunches", comment="beam background") },
  'L1_2MU4_FIRSTEMPTY':   {500: TriggerRule(PS=50,scaling="bunches", comment="beam background") },
  'L1_MU10_FIRSTEMPTY':  {500: TriggerRule(PS=17,scaling="bunches", comment="beam background") },
  'L1_EM3_FIRSTEMPTY':   {500: TriggerRule(PS=-1,scaling="bunches", comment="beam background, not used so diabled") },
  'L1_TAU8_FIRSTEMPTY':   {500: TriggerRule(PS=40,scaling="bunches", comment="beam background") },
  'L1_FJ10_FIRSTEMPTY':   {500: TriggerRule(PS=-1,scaling="bunches", comment="beam background") },
  'L1_RD1_EMPTY':        {500: TriggerRule(rate=1.0,scaling="bunches", comment="beam background") },  
    
  'L1_MU4_J15_FIRSTEMPTY':   { 100: TriggerRule(PS=1) },
  'L1_MU4_UNPAIRED_NONISO':  { 100: TriggerRule(PS=5000,comment="0.2Hz for background"), },
  'L1_MU4_UNPAIRED_ISO': { 100 : TriggerRule(PS=1200, comment='0.3Hz for background'), },
  
  #PJB these are for background stream, pscales apply for 326 and 620 bunch configs - double for 480 bunches
  #The following are already increased for 768b. (UNPAIRED ISO increased by 3 - the # bunches is increased by more than that)
  'L1_EM3_UNPAIRED_ISO':  { 100 : TriggerRule(PS=75), },# was 25
  'L1_TAU8_UNPAIRED_ISO': { 100 : TriggerRule(PS=15), },# was 5
  'L1_J10_UNPAIRED_ISO':  { 100 : TriggerRule(PS=15), },# was 5
  'L1_J10_UNPAIRED_NONISO':  { 100 : TriggerRule(PS=5), },
  'L1_BCM_AC_CA_BGRP0':        { 100: TriggerRule(PS=10000) },
  'L1_BCM_AC_CA_UNPAIRED_ISO': { 100: TriggerRule(PS=100) },
  'L1_BCM_Wide_EMPTY':         { 100: TriggerRule(PS=1) },
  'L1_BCM_Wide_UNPAIRED_ISO':  { 100: TriggerRule(PS=15) },# was 5
  'L1_BCM_Wide_UNPAIRED_NONISO':  { 100: TriggerRule(PS=30) },
  'L1_LUCID_A_C_EMPTY':        { 100: TriggerRule(PS=5000) },
  'L1_LUCID_A_C_UNPAIRED_ISO': { 100: TriggerRule(PS=3000) },# was 3000
  'L1_LUCID_A_C_UNPAIRED_NONISO': { 100: TriggerRule(PS=5000) },
  'L1_LUCID_EMPTY':        { 100: TriggerRule(PS=400000) },
  'L1_LUCID_UNPAIRED_ISO': { 100: TriggerRule(PS=60000) },# was 20000
  'L1_ZDC_A_C_EMPTY':      { 100: TriggerRule(PS=1) },
  'L1_ZDC_EMPTY':          { 100: TriggerRule(PS=4000) },
  'L1_ZDC_UNPAIRED_ISO':   { 100: TriggerRule(PS=1000) },
  'L1_ZDC_A_C_UNPAIRED_ISO': { 100: TriggerRule(PS=1) },
  'L1_2MU6_UNPAIRED_ISO':  { 500 : TriggerRule(PS=1), },
  'L1_2MU6_UNPAIRED_NONISO': { 500 : TriggerRule(PS=1), },
  'L1_EM3_UNPAIRED_NONISO':  { 500 : TriggerRule(PS=40), },
  'L1_TAU8_UNPAIRED_NONISO': { 500 : TriggerRule(PS=4), },
  'L1_FJ10_UNPAIRED_ISO': { 500 : TriggerRule(PS=1), },

  'L2_ALFA_Calib': { 100 : TriggerRule(PS=-1, comment='commissioning trigger for ALFA, not to be used during stable beams'), },
  'EF_ALFA_Phys': { 100 : TriggerRule(PS=-1, comment='commissioning trigger for ALFA, not to be used during stable beams '), },


  
  ##############################################################
  # DISABLE FOR RATE CALCULATION
  #    'L1_MU4_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
  #    'L1_2MU4_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
  #    'L1_2MU6_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
  #    'L1_MU10_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
  #    'L1_MU11_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
  #    'L1_TAU5_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
  #    'L1_EM3_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
  #    'L1_J30_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
  #    'L1_J10_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
  #    'L1_FJ10_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
  #    'L1_TE500': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
  #    'L1_multiseed': { 100 : TriggerRule(PS=1,comment='needed for rulebook'),},


  'EF_zerobias_NoAlg': { 100 : TriggerRule(PS=1, comment='zeroBias, prescaled primary'), },
  'L1_ZB': { 100 : TriggerRule(PS=70, comment='Data for MC overlay studies'), }, # OI agreed on 1Hz in average during the fill. Needs to be updated once lumi > 2e33


  # need menu expert at P1 to enable these
  'EF_l2j30_Trackless_HV_cosmic': { 100 : TriggerRule(PS=1, comment='HV background trigger, can be prescaled'), },
  'EF_2MUL1_l2j30_HV_cosmic': { 100 : TriggerRule(PS=1, comment='HV background trigger, can be prescaled'), },
  'EF_2mu6_MSonly_g10_loose_nonfilled': { 100 : TriggerRule(PS=1, comment=' background trigger, can be prescaled'), },
  'EF_3mu6_MSonly_nonfilled': { 100 : TriggerRule(PS=1, comment=' background trigger, can be prescaled'), },
  'EF_j30_eta13_a4tc_EFFS_EFxe30_noMu_empty': { 100 : TriggerRule(PS=1, comment=' background trigger, can be prescaled'), },
  'EF_j50_eta13_a4tc_EFFS_EFxe50_noMu_empty': { 100 : TriggerRule(PS=1, comment=' background trigger, can be prescaled'), },
  'EF_j50_eta25_a4tc_EFFS_EFxe50_noMu_empty': { 100 : TriggerRule(PS=1, comment=' background trigger, can be prescaled'), },
  'EF_j50_eta13_a4tc_EFFS_EFxe50_noMu_firstempty': { 100 : TriggerRule(PS=1, comment=' background trigger, can be prescaled'), },
  'EF_j50_eta25_a4tc_EFFS_EFxe50_noMu_firstempty': { 100 : TriggerRule(PS=1, comment=' background trigger, can be prescaled'), },
  'EF_j30_eta13_a4tc_EFFS_EFxe30_noMu_firstempty': { 100 : TriggerRule(PS=1, comment='Longlived background trigger, can be prescaled'), },

}

