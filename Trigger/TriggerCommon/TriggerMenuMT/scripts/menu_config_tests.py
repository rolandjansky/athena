# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

'''
Tests to verify generated menus are valid.

Ported from TrigConfStorage/ConfigurationCheck.cxx and
HLTMenuConfig/Menu/TriggerConfigHLT.py, see [ATR-19830].

Designed to be used by the `verify_menu_config.py` script.
'''

import re
from enum import Enum
from collections import Counter


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
            indices = [self._signature_type_order.index(x) for x in 
                       sig_type_pattern.findall(part)]
            return indices == sorted(indices)

        def are_signatures_in_order(name_parts):
            to_check = ["_".join(p for p in name_parts if "-" not in p)]
            # Sections of topo item parts are checked for ordering independently.
            topo_parts = [p for p in name_parts if "-" in p]
            for topo in topo_parts:
                to_check.extend(topo.split("-"))
            return all(items_in_order(part) for part in to_check)

        # Name must begin with the trigger level, and contain at least one item.
        parts = name.split("_")
        return all((len(parts) > 1,
                    parts[0] == self._trigger_level.value,
                    are_signatures_in_order(parts[1:])))


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

menu_tests = {
    TriggerLevel.HLT: [
        UniqueChainNames(),
        ConsecutiveChainCounters(),
        StructuredChainNames(TriggerLevel.HLT),
    ],
    TriggerLevel.L1: [
        RestrictedCTPIDs(),
        StructuredChainNames(TriggerLevel.L1),
    ]
}
