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
    'caloswcorr_pool_v1' : 'LFN:oflcond.000001.conditions.recon.pool.v0000._0005.pool.root',

    # Added calib hits-derived correction v1.
    'caloswcorr_pool_v2' : 'LFN:oflcond.000001.conditions.recon.pool.v0000._0011.pool.root',

    # Bugfix for calh v1.
    'caloswcorr_pool_v3' : 'LFN:oflcond.000001.conditions.recon.pool.v0000._0013.pool.root',

    # Updated calhits correction + etaoff v4_1.
    'caloswcorr_pool_v4' : 'LFN:oflcond.000001.conditions.recon.pool.v0000._0031.pool.root',

    # Bug fix for v2 calhits correction.
    'caloswcorr_pool_v5' : 'LFN:oflcond.000001.conditions.recon.pool.v0000._0044.pool.root',
    
    # adding atlfast longitudinal weights.
    'caloswcorr_pool_v6' : 'LFN:oflcond.000001.conditions.recon.pool.v0000._0135.pool.root',
    
    # adding/updating calib hits, TDR like and atlfast longitudinal weights
    'caloswcorr_pool_v7' : 'LFN:oflcond.000001.conditions.recon.pool.v0000._0141.pool.root',

    'caloswcorr_pool_v8' : 'LFN:cond08_mc.000003.gen.COND._0050.pool.root',

    # Adding lwc_v6.
    'caloswcorr_pool_v9' : 'LFN:cond08_mc.000004.gen.COND._0063.pool.root',

    # Adding DeadOTX.
    'caloswcorr_pool_v10' : 'LFN:cond09_mc.000017.gen.COND._0003.pool.root',

    # Adding phimod, phioff with reversed phi sense.
    'caloswcorr_pool_v11' : 'LFN:cond09_mc.000019.gen.COND._0011.pool.root',

    # Fix last change: reverse phi only in the barrel.
    'caloswcorr_pool_v12' : 'LFN:cond09_mc.000019.gen.COND._0012.pool.root',

    # Same as v12, but rewritten due to DB update problem.
    'caloswcorr_pool_v13' : 'LFN:cond09_mc.000019.gen.COND._0013.pool.root',

    #  add data driven out of cone correction for calib hits
    'caloswcorr_pool_v14' : 'LFN:cond09_mc.000025.gen.COND._0001.pool.root',

    # add v8 correction (MC + leakage corrected for data) with crack ending at 1.5
    'caloswcorr_pool_v15' : 'LFN:cond09_mc.000027.gen.COND._0002.pool.root',

    # add v8 correction (MC + leakage corrected for data) with old crack definition (ending at 1.55)
    'caloswcorr_pool_v16' : 'LFN:cond09_mc.000027.gen.COND._0003.pool.root',

    # add v8 correction (MC + leakage corrected for data) with old crack definition (ending at 1.55) + fix for photon at eta=2.3
    'caloswcorr_pool_v17' : 'LFN:cond09_mc.000027.gen.COND._0004.pool.root',

    # add v9 correction (MC + leakage corrected for data)
    'caloswcorr_pool_v18' : 'LFN:cond09_mc.000033.gen.COND._0001.pool.root',
    # New v5, v6 gap corrections.
    'caloswcorr_pool_v19' : 'LFN:cond09_mc.000041.gen.COND._0001.pool.root',
    # Updated v6 gap correction numbers.
    'caloswcorr_pool_v20' : 'LFN:cond09_mc.000041.gen.COND._0002.pool.root',

    # v12, with new position corrections.
    'caloswcorr_pool_v21' : 'LFN:cond09_mc.000065.gen.COND._0003.pool.root',

    #'caloswcorr_pool_xx' : 'CaloSwCorrections_13-03.pool.root',
    
    
    #######################################################################
    # Files for EM topo corrections.
    #
    'calotopoemcorr_pool_v1' : 'LFN:oflcond.000001.conditions.recon.pool.v0000._0009.pool.root',

    # Add version with no extra eta offset in endcap.
    'calotopoemcorr_pool_v2' : 'LFN:oflcond.000001.conditions.recon.pool.v0000._0030.pool.root',

    'calotopoemcorr_pool_v3' : 'LFN:cond08_mc.000003.gen.COND._0049.pool.root',

    #'calotopoemcorr_pool_xx' : 'CaloTopoEMCorrections_13-01.pool.root',
}

# Aliases for the latest versions.
poolfiles['caloswcorr_pool'] = poolfiles['caloswcorr_pool_v21']
#poolfiles['caloswcorr_pool'] = poolfiles['caloswcorr_pool_xx']

poolfiles['calotopoemcorr_pool'] = poolfiles['calotopoemcorr_pool_v3']
