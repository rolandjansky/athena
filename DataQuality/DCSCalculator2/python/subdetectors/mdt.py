# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from pkg_resources import resource_string

from .lib import (DCSC_DefectTranslate_Subdetector,
                  DCSC_Variable_With_Mapping,
                  make_multi_mapping)

MDTBA, MDTBC, MDTEA, MDTEC = 302, 303, 304, 305

def generate_mdt_mappings():
    """
    Creates dictionaries representing the mappings
    for different systems onto the chamber name.
    """
    def fix_line(line):
        if not line:
            return None
        hv, lv, jtag, name, output_channel = line.split()
        return int(hv), int(lv), int(jtag), name, int(output_channel)

    mdtcodes = resource_string("DCSCalculator2.subdetectors.data", "mdt_codes.dat").strip().split("\n")

    lines = [line for line in [fix_line(raw_line) for raw_line in mdtcodes if raw_line] if line]

    name_to_output = make_multi_mapping((name, output_channel) for hv, lv, jtag, name, output_channel in lines)
    hv_to_name     = make_multi_mapping((hv,   name)           for hv, lv, jtag, name, output_channel in lines)
    lv_to_name     = make_multi_mapping((lv,   name)           for hv, lv, jtag, name, output_channel in lines)
    jtag_to_name   = make_multi_mapping((jtag, name)           for hv, lv, jtag, name, output_channel in lines)
    name_to_name   = make_multi_mapping((name, name)           for hv, lv, jtag, name, output_channel in lines)

    return name_to_output, hv_to_name, lv_to_name, jtag_to_name, name_to_name


def evaluator_HV(iov):
    """ 
    Some chambers dont have a second multilayer (ML2). 
    Dont flag these as bad.
    """
    hv =   (
               (
                 iov.fsmCurrentState_ML1 in ["ON"] and iov.v1set_ML1 >= 3050 and
               ((iov.fsmCurrentState_ML2 in ["ON"] and iov.v1set_ML2 >= 3050) or iov.fsmCurrentState_ML2 == "")
               )

               or

               (
               iov.fsmCurrentState_ML1 == "STANDBY" and iov.v0set_ML1 >= 3050 and
               iov.fsmCurrentState_ML2 == "STANDBY" and iov.v0set_ML2 >= 3050
               )
           )

    # uncomment me for debugging
    # if not hv and "2016-05-17 01:3" in str(iov.since):
    #    try:
    #        name_to_output, hv_to_name, lv_to_name, jtag_to_name, name_to_name = generate_mdt_mappings()
    #        print "%25s %12s %12s %10.3f %10.3f  |  %s - %s" % (hv_to_name[iov.channel], 
    #                                                            iov.fsmCurrentState_ML1, iov.fsmCurrentState_ML2, 
    #                                                            iov.iMon_ML1, iov.iMon_ML2, 
    #                                                            iov.since, iov.until)
    #    except:
    #        pass
        
    return hv

def evaluator_LV(iov):
    return iov.fsmCurrentState_LV == "ON"

def evaluator_JTAG(iov):
    return iov.fsmCurrentState_JTAG == "INITIALIZED"

class MDT(DCSC_DefectTranslate_Subdetector):
    """
    The MDTs require a channel mapping to map channelids from different folders
    onto a consistent numbering scheme. Here we use the chamber name (e.g., BIS2C08)
    as the "master" channel. Everything else is mapped on to this.
    """
    folder_base = "/MDT/DCS"

    def __init__(self, *args, **kwargs):
        """
        Initialize mappings.
        """
        super(MDT, self).__init__(*args, **kwargs)

        mappings = generate_mdt_mappings()
        name_to_output, hv_to_name, lv_to_name, jtag_to_name, name_to_name = mappings

        # save the reverse mapping, too
        self.input_to_output_map = name_to_output
        self.mapping = {}
        for key, value in self.input_to_output_map.iteritems():
            self.mapping.setdefault(value, []).append(key)

        self.set_input_mapping("HV",     hv_to_name)
        self.set_input_mapping("LV",     lv_to_name)
        self.set_input_mapping("JTAG", jtag_to_name)

        self.translators = [MDT.color_to_defect_translator(flag, defect) for flag, defect in ((MDTBA, 'MS_MDT_BA_STANDBY_HV'),
                                                                                              (MDTBC, 'MS_MDT_BC_STANDBY_HV'),
                                                                                              (MDTEA, 'MS_MDT_EA_STANDBY_HV'),
                                                                                              (MDTEC, 'MS_MDT_EC_STANDBY_HV'),
                                                                                              )]

    variables = [DCSC_Variable_With_Mapping("HV",   evaluator_HV),
                 DCSC_Variable_With_Mapping("LV",   evaluator_LV),
                 DCSC_Variable_With_Mapping("JTAG", evaluator_JTAG),
                 ]

    # If you change this please consult with the Muon groups.
    # It was decided to make it the same across CSC, MDT, RPC and TGC.
    dead_fraction_caution = None
    dead_fraction_bad = 0.1

