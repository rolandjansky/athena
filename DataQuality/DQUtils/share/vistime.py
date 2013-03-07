#! /usr/bin/env python
"""
This script shows a time varying folder (DQMFONL) as a HTML table.

The algorithmic complexity is approximately linear with the number of input iovs.

The stdout of this script can be piped into a .html file.
"""

from cPickle import loads
from datetime import datetime, timedelta
from textwrap import dedent
from sys import argv

from logging import getLogger; log = getLogger("vistime")

from DQUtils import fetch_iovs, process_iovs_changed, IOVSet, init_logger
from DQUtils.general import timer

HTML_COLMAP = {None: "#eee", 0: "grey", 1: "red", 2: "yellow", 3: "green"}

def build_table_data(*states):
    """
    Given `states`, a list of IOVSets, returns (`ranges`, `chan_statechanges`),
    where `ranges` is a the list of (since, until) represented in the 
    `chan_statechanges` dictionary.
    """
    chan_statechanges = {}
    
    def add_statechange(state_idx, change_idx, state):
        """
        One channel changed
        """
        if change_idx in chan_statechanges:
            # Update the previous record's length
            chan_statechanges[change_idx][-1][1] = state_idx
        else:            
            if not state_idx:
                # We're the first record, so start with an empty list.
                chan_statechanges[change_idx] = []
            else:
                # If we're after the first record, we must have a gap from the
                # beginning to here.
                chan_statechanges[change_idx] = [[0, state_idx, state.empty()]]
                
        chan_statechanges[change_idx].append([state_idx, None, state])
    
    # The generation of the table data happens here
    ranges = []
    for i, (since, until, states, changes) in enumerate(process_iovs_changed(*states)):
        ranges.append((since, until))
        # Consider the states which changed this iteration
        for change_idx in changes:
            add_statechange(i, change_idx, states[change_idx])

    # Update the end index of all records so that they represent the same 
    # table length.
    for change, statechanges in chan_statechanges.iteritems():
        statechanges[-1][1] = i+1
    
    return ranges, chan_statechanges

HTML_HEADER = dedent("""
    <html>
    <head>
    <style type="text/css">
    td { background-color: #aaa; }
    .incomplete { background: url(hatching.png); }
    </style>
    </head>
    <body>""")

def table_data_to_html(channels, data):
    """
    Build a html table given visualising DQ flags
    
    `channels` : A list of channel names
    `data` : A list of table data. The first row is assumed to be lbtime data
    """
    ranges, celldata = data
    result = []
    a = result.append
    
    a(HTML_HEADER)
    a("<table>")
    
    # The list of ranges which are populated
    #a("<tr>")
    #for i, r in enumerate(ranges):
        #a('<td width="10px">&nbsp;</td>')
    #a("</tr>")
        
    def build_cell(width, content):
        """
        Append a single table cell.
        `width` : Number of columns in the table which `content` should span
        `content` : An IOVType containing (since, until, channel, code)
        """
        args = []
        if width > 1:
            # This IoV is active for many columns in the table
            args.append('colspan="%i"' % width)
            
        if content.until == 2**63-1:
            # Open ended IoVs
            args.append('class="incomplete"')
        
        if "Code" in content._fields:
            color = HTML_COLMAP[content.Code]
            args.append('style="background-color: %s"' % color)
            
        args = (" " + " ".join(args)) if args else ""
        #a('<td%s>%s</td>' % (args, content.Code))
        a('<td%s></td>' % args)
    
    # Build the luminosity blocks row at the top of the table
    lbs = celldata.pop(0)
    a("<tr>")
    a("<td>lb</td>")
    for scol, ucol, lb in lbs:
        build_cell(ucol-scol, lb)
    a("</tr>")
    
    # Build one row per channel represented. The bulk of the table is turned
    # into HTML here
    for channel_idx, items in sorted(celldata.iteritems()):
        channel_name = channels[channel_idx-1]
        a("<tr>")
        a("<td>%s</td>" % channel_name)
        for scol, ucol, content in items:
            build_cell(ucol-scol, content)
        a("</tr>")
        
    a("</table>")
    
    return "".join(result)

def load_iovs(filename):
    """
    Load iovs saved in a filename. Useful for testing where COOL is unavailable
    """
    with open(filename) as fd:
        return loads(fd.read().decode("bz2"))

def main():
    init_logger(verbose=2)
    
    # Query the last four days of information
    start_time = datetime.now() - timedelta(days=2)
    
    with timer("Query DQMFONL"):
        iovs = fetch_iovs("DQMFONL", start_time)
        log.info("Got iovs from DQMFONL: %r", iovs)
        
    with timer("Query LBTIME"):
        lbiovs = fetch_iovs("LBTIME", start_time)
        
    chans, states = iovs.chans_iovsets
    with timer("run build_table_data"):
        data = build_table_data(lbiovs, *states)
        
    with timer("run table_data_to_html"): 
        table = table_data_to_html(chans, data)
        
    print table

if __name__ == "__main__":
    main()
