# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: poolfiles.py,v 1.14 2009-04-25 17:57:00 ssnyder Exp $
#
# File: CaloClusterCorrection/python/poolfiles.py
# Created: Feb 2007, sss
# Purpose: Define pool files in which corrections are stored.
#
# One of the ways in which the corrections constants may be accessed
# is by reading them from a pool file.  Each correction declares
# which pool file in which it is resident, using a short string
# to identify the file.  In order to actually access the file, we need
# the LFN of the file.  Here, we declare a dictionary that maps from
# these short strings to the LFNs.
#
# It's OK for top-level configuration code to import this module and then
# made additions to the dictionary.
#

poolfiles = {
    #######################################################################
    # Files for EM SW corrections.
    # Original version for v13.
    #oflcond.000001.conditions.recon.pool.v0000._0005.pool.root'
    'caloswcorr_pool_v1' : 'FID:7EC81A07-00C9-DB11-98CA-003048836743',

    # Added calib hits-derived correction v1.
    #oflcond.000001.conditions.recon.pool.v0000._0011.pool.root
    'caloswcorr_pool_v2' : 'FID:32020F09-A2E3-DB11-8228-000423D659FE',

    # Bugfix for calh v1.
    #oflcond.000001.conditions.recon.pool.v0000._0013.pool.root
    'caloswcorr_pool_v3' : 'FID:F493E0D2-9C08-DC11-8DD6-000423D6618A',

    # Updated calhits correction + etaoff v4_1.
    #oflcond.000001.conditions.recon.pool.v0000._0031.pool.root
    'caloswcorr_pool_v4' : 'FID:58E394DD-D6C9-DC11-8CBE-000423D67746',

    # Bug fix for v2 calhits correction.
    #oflcond.000001.conditions.recon.pool.v0000._0044.pool.root
    'caloswcorr_pool_v5' : 'FID:C8E77C8B-87C8-DC11-8A25-000423D67746',
    
    # adding atlfast longitudinal weights.
    #oflcond.000001.conditions.recon.pool.v0000._0135.pool.root
    'caloswcorr_pool_v6' : 'FID:F6DE4EE9-C052-DD11-9A12-000423D989B4',
    
    # adding/updating calib hits, TDR like and atlfast longitudinal weights
    #oflcond.000001.conditions.recon.pool.v0000._0141.pool.root
    'caloswcorr_pool_v7' : 'FID:002D04F2-5F61-DD11-ADF6-000423D9524C',

    #cond08_mc.000003.gen.COND._0050.pool.root
    'caloswcorr_pool_v8' : 'FID:1C1E1AD5-8C23-DE11-B1C2-000423D6572A',

    # Adding lwc_v6.
    #cond08_mc.000004.gen.COND._0063.pool.root
    'caloswcorr_pool_v9' : 'FID:1C6FA7BD-7CA8-DE11-A3F1-000423D2B9E8',

    # Adding DeadOTX.
    #cond09_mc.000017.gen.COND._0003.pool.root
    'caloswcorr_pool_v10' : 'FID:D03792A8-A3AB-DF11-9D48-001E4F3E5C37',

    # Adding phimod, phioff with reversed phi sense.
    #cond09_mc.000019.gen.COND._0011.pool.root
    'caloswcorr_pool_v11' : 'FID:36A20CB1-D5CB-DF11-9BE9-000423D94A18',

    # Fix last change: reverse phi only in the barrel.
    #cond09_mc.000019.gen.COND._0012.pool.root
    'caloswcorr_pool_v12' : 'FID:3C177203-15CD-DF11-AA8C-000423D94A18',

    # Same as v12, but rewritten due to DB update problem.
    #cond09_mc.000019.gen.COND._0013.pool.root
    'caloswcorr_pool_v13' : 'FID:7A458618-C0CD-DF11-85B0-000423D94A18',

    #  add data driven out of cone correction for calib hits
    #cond09_mc.000025.gen.COND._0001.pool.root
    'caloswcorr_pool_v14' : 'FID:A4EE1D65-4C25-E011-B992-003048F0E7D8',

    # add v8 correction (MC + leakage corrected for data) with crack ending at 1.5
    #cond09_mc.000027.gen.COND._0002.pool.root
    'caloswcorr_pool_v15' : 'FID:FA104462-FA35-E011-B9FB-003048D2BBC0',

    # add v8 correction (MC + leakage corrected for data) with old crack definition (ending at 1.55)
    #cond09_mc.000027.gen.COND._0003.pool.root
    'caloswcorr_pool_v16' : 'FID:C82E841E-A737-E011-89D7-003048F0E7AA',

    # add v8 correction (MC + leakage corrected for data) with old crack definition (ending at 1.55) + fix for photon at eta=2.3
    #cond09_mc.000027.gen.COND._0004.pool.root
    'caloswcorr_pool_v17' : 'FID:7CC6AE30-5A38-E011-B664-003048F0E786',

    # add v9 correction (MC + leakage corrected for data)
    #cond09_mc.000033.gen.COND._0001.pool.root
    'caloswcorr_pool_v18' : 'FID:EA383389-C7A8-E011-83D6-001E4F3DA869',
    # New v5, v6 gap corrections.
    #cond09_mc.000041.gen.COND._0001.pool.root
    'caloswcorr_pool_v19' : 'FID:B2D54CA9-B16A-E111-92DF-003048F0E7D6',
    # Updated v6 gap correction numbers.
    #cond09_mc.000041.gen.COND._0002.pool.root
    'caloswcorr_pool_v20' : 'FID:EEA1795F-866E-E111-88D6-003048F0E7D6',

    # v12, with new position corrections.
    # This one is broken: eta offset correction parameters are missing
    # for all versions other than v12.
    #cond09_mc.000065.gen.COND._0003.pool.root
    'caloswcorr_pool_v21' : 'FID:4DB69345-375C-4141-84C3-DD1EBBB914A2',

    # v12, with new position corrections.
    # Fixed version.
    #cond09_mc:cond09_mc.000079.gen.COND._0002.pool.root
    'caloswcorr_pool_v22' : 'FID:BC292F26-AE73-9041-BF5C-BCE6C5C651EC',

    'caloswcorr_pool_xx' : 'CaloSwCorrections_13-03.pool.root',
    
    
    #######################################################################
    # Files for EM topo corrections.
    #
    #oflcond.000001.conditions.recon.pool.v0000._0009.pool.root
    'calotopoemcorr_pool_v1' : 'FID:AC24975B-B5DB-DB11-91B9-000423D675CA',

    # Add version with no extra eta offset in endcap.
    #oflcond.000001.conditions.recon.pool.v0000._0030.pool.root
    'calotopoemcorr_pool_v2' : 'FID:562F8151-27CA-DC11-82A0-000423D67746',

    #cond08_mc.000003.gen.COND._0049.pool.root
    'calotopoemcorr_pool_v3' : 'FID:2605504F-6C2F-DE11-A73D-000423D6572A',

    #'calotopoemcorr_pool_xx' : 'CaloTopoEMCorrections_13-01.pool.root',
}

# Aliases for the latest versions.
poolfiles['caloswcorr_pool'] = poolfiles['caloswcorr_pool_v22']
#poolfiles['caloswcorr_pool'] = poolfiles['caloswcorr_pool_xx']

poolfiles['calotopoemcorr_pool'] = poolfiles['calotopoemcorr_pool_v3']
