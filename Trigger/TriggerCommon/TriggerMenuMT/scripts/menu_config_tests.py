# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

'''
Tests to verify generated menus are valid.

Ported from TrigConfStorage/ConfigurationCheck.cxx and
HLT/Menu/TriggerConfigHLT.py, see [ATR-19830].

Designed to be used by the `verify_menu_config.py` script.
'''

import re
from enum import Enum
from collections import Counter

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLT.Combined' )
logging.getLogger().info("Importing %s",__name__)

class TriggerLevel(Enum):
    HLT = "HLT"
    L1 = "L1"


class MenuVerification(object):
    def __init__(self, description):
        self.description = description
        self.failures = []

    def run(self, config):
        raise NotImplementedError("ConfigVerification subclass must implement run()")


class UniqueChainNames(MenuVerification):
    def __init__(self):
        super(UniqueChainNames, self).__init__(
            description="Chain names are unique")

    def run(self, config):
        names = config["chains"].keys()
        counts = Counter(names)
        self.failures = [chain for chain, count
                         in counts.items() if count > 1]


class ConsecutiveChainCounters(MenuVerification):
    def __init__(self):
        super(ConsecutiveChainCounters, self).__init__(
            description="Chain counters are consecutive 1..N")

    def run(self, config):
        counters = [chain["counter"] for chain
                    in config["chains"].values()]
        prev_counter = 0
        for count in counters:
            if count != prev_counter + 1:
                self.failures.append(count)
            prev_counter = count


class StructuredChainNames(MenuVerification):
    '''
    Verifies that chain names start with the expected prefix, and have their
    parts in the correct order by type, as well as any trigger level specific
    restrictions.
    '''

    _SIGNATURE_TYPE_ORDER = {
        TriggerLevel.HLT: [
            "e", "g", "mu", "tau", "j", "xe",  "ht"
        ],
        TriggerLevel.L1: [
            "EM", "MU", "TAU", "J", "XE", "HT"
        ],
    }

    def __init__(self, trigger_level):
        super(StructuredChainNames, self).__init__(
            description="Chain names are structured in the expected way")
        self._trigger_level = trigger_level
        self._signature_type_order = \
            self._SIGNATURE_TYPE_ORDER[trigger_level]

    def run(self, config):
        if self._trigger_level == TriggerLevel.HLT:
            names = config["chains"].keys()
            self.failures = [n for n in names
                             if not self._name_matches_hlt_convention(n)]
        elif self._trigger_level == TriggerLevel.L1:
            names = [item["name"] for item in config["items"].values()]
            self.failures = [n for n in names
                             if not self._name_matches_l1_convention(n)]

    def _name_matches_hlt_convention(self, name):
        return "_L1" in name and self._matches_shared_conventions(name)

    def _name_matches_l1_convention(self, name):
        def FEX_items_in_order(name):
            '''
            Where multiple FEX algorithms are used for the same item type,
            they should be in alphabetical order.
            '''
            fex_pattern = r"\d*([egj])({})[A-Z]*\d+s?".format(
                "|".join(self._signature_type_order))
            fex_items = re.findall(fex_pattern, name)

            for item_type in self._signature_type_order:
                fexes_of_type = [fex for fex, match_type in fex_items
                                 if match_type == item_type]
                if not fexes_of_type == sorted(fexes_of_type):
                    return False
            return True

        return self._matches_shared_conventions(name) and FEX_items_in_order(name)

    def _matches_shared_conventions(self, name):
        '''
        True if name starts with level prefix, and all signature
        types are in the correct order, otherwise False.
        '''
        # The signature objects in each item should be ordered by type, in the
        # order defined in _SIGNATURE_TYPE_ORDER.
        signature_types = "|".join(self._signature_type_order)
        sig_type_pattern = re.compile(
            r"\d*[egj]?({})\d+s?".format(signature_types))
        def items_in_order(part):
            #part    = part.replace("leg","p") #if we leave the word leg, the findall(..) function will find a 'g' 
            indices = [self._signature_type_order.index(x) for x in 
                       sig_type_pattern.findall(part)]
            rr =  indices == sorted(indices)
            if not rr:
                log.error("[StructuredChainNames::items_in_order] %s NOT SORTED!", indices)

            return rr

        def are_signatures_in_order(name_parts):
            to_check = ["_".join(p for p in name_parts if "-" not in p)]
            # Sections of topo item parts are checked for ordering independently.
            topo_parts = [p for p in name_parts if "-" in p]
            for topo in topo_parts:
                to_check.extend(topo.split("-"))
            res = all(items_in_order(part) for part in to_check)
            if not res:
                for part in to_check:
                    if not items_in_order(part):
                        log.error("[StructuredChainNames::are_signatures_in_order] %s not in order!", part)
            return res

        # Name must begin with the trigger level, and contain at least one item.
        parts = name.split("_")

        result= all((len(parts) > 1,
                    parts[0] == self._trigger_level.value,
                    are_signatures_in_order(parts[1:])))
        if not result:
            log.error("[StructuredChainNames::_matches_shared_conventions] chain deosn't match convention: parts[0] = %s, value = %s, parts[1:] = %s, signature_types = %s", 
                      parts[0], self._trigger_level.value, parts[1:], signature_types)
        return result


class RestrictedCTPIDs(MenuVerification):
    def __init__(self):
        super(RestrictedCTPIDs, self).__init__(
            description="Less than 512 CTP items, and no CTP id greater than 512")

    def run(self, config):
        ctp_ids = {name: item["ctpid"] for
                    name, item in config["items"].items()}
        if len(ctp_ids) > 512:
            self.failures.append(
                "More than 512 CTP items defined")
        over_max_ids = [name for name, ctp_id in ctp_ids.items()
                        if ctp_id > 512]
        self.failures.extend(over_max_ids)


class PartialEventBuildingChecks(MenuVerification):
    def __init__(self):
        super(PartialEventBuildingChecks, self).__init__(
            description='Config consistency of Partial Event Building')

    def run(self, config):
        from TriggerMenuMT.HLT.Menu import EventBuildingInfo
        eb_identifiers = EventBuildingInfo.getAllEventBuildingIdentifiers()

        for chain_name, chain_config in config['chains'].items():
            peb_identifiers = [idf for idf in eb_identifiers if '_'+idf+'_' in chain_name]
            peb_writers = [seq for seq in chain_config['sequencers'] if 'PEBInfoWriter' in seq]

            is_peb_chain = (len(peb_identifiers) > 0 or len(peb_writers) > 0)

            # Check streaming configuration
            for stream_name in chain_config['streams']:
                if stream_name not in config['streams']:
                    self.failures.append(
                        'Stream {:s} for chain {:s} is not defined in streaming configuration'.format(
                            stream_name, chain_name))

                is_feb_stream = config['streams'][stream_name]['forceFullEventBuilding']

                if is_peb_chain and is_feb_stream:
                    self.failures.append(
                        'PEB chain {:s} streamed to a full-event-building stream {:s} '
                        '(forceFullEventBuilding=True)'.format(
                            chain_name, stream_name))

                elif not is_peb_chain and not is_feb_stream:
                    self.failures.append(
                        'Full-event-building chain {:s} streamed to the stream {:s} which allows partial '
                        'event building (forceFullEventBuilding=False)'.format(
                            chain_name, stream_name))

            if not is_peb_chain:
                # Not a PEB chain, skip further PEB-specific checks
                continue

            if len(peb_identifiers) != 1:
                self.failures.append(
                    '{:s} has {:d} event building identifiers'.format(chain_name, len(peb_identifiers)))

            if len(peb_writers) != 1:
                self.failures.append(
                    '{:s} has {:d} PEBInfoWriter sequences'.format(chain_name, len(peb_writers)))

            if peb_identifiers and peb_writers and not peb_writers[0].endswith(peb_identifiers[0]):
                    self.failures.append(
                        '{:s} PEB sequence name {:s} doesn\'t end with PEB identifier {:s}'.format(
                            chain_name, peb_writers[0], peb_identifiers[0]))



menu_tests = {
    TriggerLevel.HLT: [
        UniqueChainNames(),
        ConsecutiveChainCounters(),
        StructuredChainNames(TriggerLevel.HLT),
        PartialEventBuildingChecks()
    ],
    TriggerLevel.L1: [
        RestrictedCTPIDs(),
        StructuredChainNames(TriggerLevel.L1),
    ]
}
