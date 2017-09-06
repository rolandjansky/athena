# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from logging import getLogger; log = getLogger("DCSC2.tile")
from .lib import (DCSC_DefectTranslate_Subdetector, DCSC_Variable, 
                  DCSC_Variable_With_Mapping, OUT_OF_CONFIG, GoodIOV)

WHITE, BLACK, GREY, RED, YELLOW, GREEN = None, -1, 0, 1, 2, 3

TILBA, TILBC, TIEBA, TIEBC = 232, 233, 234, 235
N_CHANNELS_PER_MODULE = 90

from itertools import product

# Magic, needed for functioning coral Blob
import cppyy
import PyCool 
Blob = cppyy.gbl.coral.Blob

from TileCalibBlobObjs.Classes import (
    TileCalibDrawerBch, TileBchDecoder, TileBchStatus, TileCalibUtils, TileBchPrbs)

def make_blob(string):
    b = Blob()
    b.write(string)
    b.seek(0)
    return b

def decode_status(chan, calib_blob):
    """
    Given a module number `chan` and a `calib_blob` string, return the number
    of bad, good and affected channels in this module.
    
    It also returns a dictionary `prob` containing as keys the problem identifier
    and values the number of channels affected by that problem.
    """
    
    if chan == 1000 or len(calib_blob) <= 24:
        # chan 1000 is a comment, and calib_blobs less than 24 in length cause 
        # a crash.
        return None
    
    # Decode a Coral BLOB into a string
    b = make_blob(calib_blob)
    bch = TileCalibDrawerBch.getInstance(b)
    
    bad, good, affected = [], [], []
    
    probs = {}
    
    decoder = TileBchDecoder(bch.getBitPatternVersion())
    chn_adcs = product(xrange(TileCalibUtils.max_chan()), xrange(TileCalibUtils.max_gain()))
    # Stolen from near
    # http://alxr.usatlas.bnl.gov/lxr/source/atlas/TileCalorimeter/TileCalib/
    #     TileCalibBlobPython/python/TileBchTools.py#072
    for chn, adc in chn_adcs:
        adcBits, chnBits = bch.getData(chn, adc, 0), bch.getData(chn, 2, 0)
        status = TileBchStatus(decoder.decode(chnBits, adcBits))
        if status.isBad(): bad.append((chn, adc))
        if status.isGood(): good.append((chn, adc))
        if status.isAffected(): affected.append((chn, adc))
        
        if not status.isGood():
            prbs = status.getPrbs()
            # Build a dictionary
            for prb in prbs:
                key = prb, TileBchPrbs.getDescription(prb)
                probs[key] = probs.get(key, 0) + 1
    
    return len(bad), len(good), len(affected), probs

def decode_tile_iov(i):
    """
    Rewrite the TileCalibBlob field to contain the result of `decode_status`
    """
    return i._replace(TileCalibBlob=decode_status(i.channel, i.TileCalibBlob))

def decode_tile_iovs(iovs):
    return iovs.empty(decode_tile_iov(i) for i in iovs)
    
class Tile_NoHighVoltage(DCSC_Variable_With_Mapping):
    """
    Records modules which have all of their channels in the NoHV state.
    """
    input_db = "COOLONL_TILE/COMP200"
    is_config_variable = True
    timewise_folder = False
    
    fetch_args = dict(tag="TileOfl01StatusAdc-HLT-UPD1-00")
            
    def make_good_iov(self, iov):
        return GoodIOV(iov.since, iov.until, iov.channel, OUT_OF_CONFIG) 
            
    def make_good_iovs(self, iovs):
        iovs = decode_tile_iovs(iovs)
        
        log.debug("Number of NHV iovs: %i", len(iovs))
        
        def too_many_nohv(iov):
            """
            Read the TileCalib blob to determine what problems are present.
            If all channels appear to have (2002, No HV), then count this module
            (containing 90 channels) as being OUT OF CONFIG.
            """
            if not iov.TileCalibBlob: return False
            nbad, ngood, naffected, problems = iov.TileCalibBlob
            if problems:
                log.debug("Tile problems %i %r", iov.channel, problems)
            return problems.get((2002, 'No HV'), 0) >= N_CHANNELS_PER_MODULE
        
        iovs = iovs.empty(iov for iov in iovs if too_many_nohv(iov))
        
        log.debug("Bad high voltage channels:")
        log.debug("= [%r]", ", ".join(str(i.channel) for i in iovs))
        
        iovs = super(Tile_NoHighVoltage, self).make_good_iovs(iovs)
        
        return iovs

class Tile(DCSC_DefectTranslate_Subdetector):
    """
    Check the TILE/DCS/STATES FORDAQ_MBHV code for modules in a good state.
    Excludes modules which are known to be entirely bad due to No HV so that 
    shifters will notice problems with the STATE which are unrelated to this
    known problem.
    """
    
    folder_base = "/TILE/DCS"
    
    # Key: output_channel
    # Value: input_channel
    #mapping = {
    #    TILBA: range(  1,  64) + [65],
    #    TILBC: range( 66, 130),
    #    TIEBA: range(193, 257),
    #    TIEBC: [64] + range(130, 193),
    #}

    # New mapping, ordered so that the channels match up with
    # The channels in the next mapping, mapping2.
    # I'm not a big fan of the way this is done, so I should
    # come up with a better solution.  I at least need to clean it up
    mapping = {
        TILBA: range(49, 52) + [65] + [55] + range(61, 64) + [16] + 
               range(1, 16) + range(17, 49) + range(52, 55) + range(56, 61),

        TILBC: range(114, 118) + [121] + range(127, 130) +  [81] + range(66, 81) + 
               range(82, 114) + range(118, 121) + range( 122, 127),

        TIEBA: range(241, 245) + [248] + range(254, 257) + [208] + range(193, 208) + 
               range(209, 241) + range(245, 248) + range(249, 254),

        TIEBC: range(177, 181) + [184] + range(190, 193) + [145] + range(131, 134) + 
               [64] + range(134, 145) + range(146, 163) + [130] + range(163, 177) + 
               range(181, 184) + range(185, 190)
    }

    mapping2 = dict( zip(range( 20,  84), mapping[TILBA]) +
                     zip(range( 84, 148), mapping[TILBC]) +
                     zip(range(148, 212), mapping[TIEBA]) +
                     zip(range(212, 276), mapping[TIEBC])
                   )

    variables = [
        #DCSC_Variable("STATES", lambda iov: iov.FORDAQ_MBHV == 212222),
        DCSC_Variable("STATES", lambda iov: iov.FORDAQ_MBHV in (212222, 202221)),
        Tile_NoHighVoltage("/TILE/OFL01/STATUS/ADC", None)
    ]

    dead_fraction_caution = 0.01
    dead_fraction_bad = 0.75



#For book keeping purposes, here is a historical summary of thresholds
#
#24/06/10 
#Runs start of the year -  
#    dead_fraction_map = {
#        TILBA: 0.048,
#        TILBC: 0.048,
#        TIEBA: dead_fraction_caution,
#        TIEBC: 0.0314  
#    }
#
#23/07/10 - Lost EBC47
#Runs   - 160801
#    dead_fraction_map = {
#        TILBA: 0.048,
#        TILBC: 0.048,
#        TIEBA: dead_fraction_caution,
#        TIEBC: 0.048  
#    }
#
#    
#11/08/10 - Lost LBA09 due to interface card issue.  Lost LBC08 
#Runs  160878 - 177167
#    dead_fraction_map = {
#        TILBA: 0.064,
#        TILBC: 0.064,
#        TIEBA: dead_fraction_caution,
#        TIEBC: 0.048  
#    }
#
#21/03/11 - After winter maintanence, many power supplies were repaired 
#           but the frequent power cutscost us several modules
#Runs 177167  - 178624
#    dead_fraction_map = {
#        TILBA: 0.032,
#        TILBC: 0.017,
#        TIEBA: dead_fraction_caution,
#        TIEBC: 0.017  
#    }
#
#08/04/11 - Lost LBC07 but gained EBC37    
#Runs 178740 - 178908
#    dead_fraction_map = {
#        TILBA: 0.032,
#        TILBC: 0.032,
#        TIEBA: dead_fraction_caution,
#        TIEBC: dead_fraction_caution
#    }
#
#08/04/11 - Lost LBC02
#Runs 178933 - present 
#    dead_fraction_map = {
#        TILBA: 0.032,
#        TILBC: 0.048,
#        TIEBA: dead_fraction_caution,
#        TIEBC: dead_fraction_caution
#    }
#
#
#27/04/11 - Lost EBC37, gained LBA24 back
#Runs 178933 - present 
#    dead_fraction_map = {
#        TILBA: 0.032,
#        TILBC: 0.048,
#        TIEBA: dead_fraction_caution,
#        TIEBC: dead_fraction_caution
#    }
#
#
#27/04/11 - Lost EBC37, gained LBA24 back
#    dead_fraction_map = {
#        TILBA: 0.017, # Mask dead modules
#        TILBC: 0.048, # Mask dead modules
#        TIEBA: dead_fraction_caution,
#        TIEBC: 0.017
#    }
#
#30/04/11 - Lost LBA24 again
#    dead_fraction_map = {
#        TILBA: 0.032, # Mask dead modules
#        TILBC: 0.048, # Mask dead modules
#        TIEBA: dead_fraction_caution,
#        TIEBC: 0.017
#    }
#
# 
#    dead_fraction_map = {
#        TILBA: 0.048, # Mask dead modules
#        TILBC: 0.048, # Mask dead modules
#        TIEBA: dead_fraction_caution,
#        TIEBC: 0.017
#    }
# 4/July/11 - EBC is back again
#    dead_fraction_map = {
#        TILBA: 0.048, # Mask dead modules
#        TILBC: 0.048, # Mask dead modules
#        TIEBA: dead_fraction_caution,
#        TIEBC: 0.0
#    }
# 10 July, EBC 37 lost again
#    dead_fraction_map = {
#        TILBA: 0.032, # Mask dead modules
#        TILBC: 0.048, # Mask dead modules
#        TIEBA: dead_fraction_caution,
#        TIEBC: 0.017
#    }
    dead_fraction_map = {
        TILBA: 0.032, # Mask dead modules
        TILBC: 0.064, # Mask dead modules
        TIEBA: dead_fraction_caution,
        TIEBC: 0.017
    }



    
    def calculate_dead_fraction(self, since, until, output_channel, states, 
                                state_iovs):
        """
        This is for masking the unpowered Tile Cells.  It returns the actual fraction of Tile that is off
        but the R/Y/G is adjusted so that the unpowered modules don't hide new problems.  
        """
        cdf = super(Tile, self).calculate_dead_fraction
        result = cdf(since, until, output_channel, states, state_iovs)
        
        code, dead_fraction, thrust, n_config, n_working = result

        # Don't include channels which are "out of config" (see Tile_NoHighVoltage)
        dead_fraction = 1 - (n_working / n_config)

        # If n_working == n_config then we're good.
        if dead_fraction == 0:
            # Mask dead channels
            code = GREEN
        
        return code, dead_fraction, thrust, n_config, n_working

    def __init__(self, *args, **kwargs):
        kwargs['keep_dcsofl'] = True
        super(Tile, self).__init__(*args, **kwargs)
        self.translators = [Tile.color_to_defect_translator(flag, defect, [RED, YELLOW])
                            for flag, defect in ((TILBA, 'TILE_LBA_TRIP'),
                                                 (TILBC, 'TILE_LBC_TRIP'),
                                                 (TIEBA, 'TILE_EBA_TRIP'),
                                                 (TIEBC, 'TILE_EBC_TRIP'),
                                                 )]
        self.set_input_mapping("/TILE/OFL01/STATUS/ADC", self.mapping2)
