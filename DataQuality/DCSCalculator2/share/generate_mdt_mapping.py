#! /usr/bin/env python
"""
generate_mdt_mapping.py -- a script to generate the MDT channel mapping for HV, LV, JTAG from the COOL database.

from:
        BEE1A02__HV   <=> X
        BEE1A02__LV   <=> Y
        BEE1A02__JTAG <=> Z

to:
        X Y Z BEE1A02 302

https://svnweb.cern.ch/trac/atlasoff/browser/DataQuality/DQUtils/tags/DQUtils-00-01-04/python/db.py
https://svnweb.cern.ch/trac/atlasoff/browser/DataQuality/DQUtils/tags/DQUtils-00-01-04/python/channel_mapping.py

Run like:

[afs] asetup 20.1.4.7,here
[afs] python generate_mdt_mapping.py > mdt_codes.dat

Two exceptions, thanks to George Iakovidis and Stephanie Zimmerman:
 * BOG0B12, BOG0B14 are associated to BOG0A12, BOG0A14 because they are located at z=0.
 * BME5A13, BME5C13 share the same HV, LV with BME4A13, BME4C13 respectively (bigger chambers with 2xCSM)

Alex Tuna <tuna@cern>
"""

from DQUtils                 import Databases
from DQUtils.channel_mapping import get_channel_ids_names

database = "COOLOFL_DCS/CONDBR2"

MDTBA, MDTBC, MDTEA, MDTEC = 302, 303, 304, 305

def detector_region(key):
    """ e.g., translate BOF7C12 to MDTBC """

    if key[0] == "B" and key[4] == "A": return MDTBA
    if key[0] == "B" and key[4] == "B": return MDTBA # exception
    if key[0] == "B" and key[4] == "C": return MDTBC
    if key[0] == "E" and key[4] == "A": return MDTEA
    if key[0] == "E" and key[4] == "C": return MDTEC

def main():

    _, _, hv = get_channel_ids_names(Databases.get_folder("/MDT/DCS/HV",   database))
    _, _, lv = get_channel_ids_names(Databases.get_folder("/MDT/DCS/LV",   database))
    _, _, jt = get_channel_ids_names(Databases.get_folder("/MDT/DCS/JTAG", database))

    all_keys    = hv.keys() + lv.keys() + jt.keys()
    string_keys = filter(lambda key: isinstance(key, str), all_keys)
    region_keys = set()

    for key in string_keys:

        region, folder = key.split("__")
        region_keys.add(region)

    for key in sorted(region_keys):
        line = "%-6s %-6s %-6s %-8s %3s" % (hv.get(key+"__HV",   0),
                                            lv.get(key+"__LV",   0),
                                            jt.get(key+"__JTAG", 0),
                                            key,
                                            detector_region(key),
                                            )

        # exceptions
        if key == "BOG0A12" and "BOG0B12" in region_keys: continue
        if key == "BOG0A14" and "BOG0B14" in region_keys: continue
        if key == "BOG0B12": line = line.replace("%-6s BOG0B12" % (0), "%-6s BOG0A12" % (jt["BOG0A12__JTAG"]))
        if key == "BOG0B14": line = line.replace("%-6s BOG0B14" % (0), "%-6s BOG0A14" % (jt["BOG0A14__JTAG"]))
        if key == "BME5A13": line = line.replace("%-6s %-6s" % (0, 0), "%-6s %-6s" % (hv["BME4A13__HV"], lv["BME4A13__LV"]))
        if key == "BME5C13": line = line.replace("%-6s %-6s" % (0, 0), "%-6s %-6s" % (hv["BME4C13__HV"], lv["BME4C13__LV"]))

        if line:
            print line


if __name__ == "__main__":
    main()
