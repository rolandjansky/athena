#! /usr/bin/env python

from IPython.Shell import IPShellEmbed; ip = IPShellEmbed(["-pdb"])

from DCSCalculator2.subdetectors.tgc import TGC, DCSC_Variable_TGC_HV, TGC_Database, get_mapping
from DQUtils import fetch_iovs
from DQUtils.iov_arrangement import inverse_lblb
from itertools import chain

runs = 165732, 165733

iovs = inverse_lblb(fetch_iovs("LBLB", runs=runs))
start, end = iovs.first.since, iovs.last.until

TGC_data = DCSC_Variable_TGC_HV(None, None)
TGC_data.db = TGC_Database()
mapping = get_mapping(TGC_data.db)
TGC_data.all_channels = set(chain(*mapping.values()))

tgc_conditions = TGC_data.read_tgc_conditions(start.date, end.date)
tgc_channels_nonempty = set(iov.channel for iov in tgc_conditions if iov)

print "Total IoVs for query range:", len(tgc_conditions)
print "Channels with IoV data:", len(tgc_conditions.channels)
print "Channels with nonempty IoV data:", len(tgc_channels_nonempty)
print "Channels in mapping table:", len(TGC_data.all_channels)
