#! /usr/bin/env python

from argparse import ArgumentParser
from os.path import basename

from DQUtils import IOVSet
from DQUtils.periods import get_period_iovset_from_ami

def split_grl(ptag, periods, grl_file, by_period):
    """
    Doc
    """
    grl = IOVSet.from_grl(grl_file)
    
    name_left, _, extension = basename(grl_file).rpartition(".")
    assert name_left, "No '.' in filename?"

    iovs_total = IOVSet()
    name_all_period = []

    for period in periods:

        period_iovset = get_period_iovset_from_ami(ptag, period)
        
        iovs = grl & period_iovset
        iovs_total |= iovs

        name_all_period.append(period)

        if by_period:
            output_name = "{0}_period{1}.{2}".format(name_left, period, extension)
            print "{0:4s} : {1:3} runs, {2:5} lumiblocks" .format(
                period, len(iovs.runs), iovs.lb_counts)
        
            iovs.to_grl(output_name)

    iovs_total = grl &  iovs_total 
    output_name = "{0}_period{1}.{2}".format(name_left,"to".join(name_all_period), extension)
    print "{0:4s} : {1:3} runs, {2:5} lumiblocks" .format(
        name_all_period, len(iovs_total.runs), iovs_total.lb_counts)
    iovs_total.to_grl(output_name)
   
def main():
    parser = ArgumentParser(description="Break a GRL down by period")
    a = parser.add_argument
    
    a("-t", "--project-tag", type=str, required=True)
    a("--by-period", action="store_true", help="Generate one GRL per period specified as well as the merge one.")
    a("-p", "--period", dest="periods", action="append", default=None, required=True,
      help="One or more periods (specifiy -p repeatedly)")
    a("grl", type=str)

    args = parser.parse_args()
    split_grl(args.project_tag, args.periods, args.grl, args.by_period)

if __name__ == "__main__":
    main()
