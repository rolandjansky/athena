# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.get_tag_diff
# @purpose Get the list of tag differences between 2 releases (CERN centric)
# @author Sebastien Binet
# @date February 2010

__version__ = "$Revision: 276362 $"
__doc__ = "Get the list of tag differences between 2 releases (CERN centric)."
__author__ = "Sebastien Binet"


### imports -------------------------------------------------------------------
import PyUtils.acmdlib as acmdlib

@acmdlib.command(
    name='get-tag-diff'
    )
@acmdlib.argument(
    'old',
    help="The description string of the reference release (eg: 12.0.X,rel_3,AtlasOffline)"
    )
@acmdlib.argument(
    'new',
    help="The description string of the to-be-compared release (eg: 12.0.X,rel_3 or 12.0.3)"
    )
def main(args):
    """Get the list of tag differences between 2 releases (CERN centric)
    """

    import PyCmt.Cmt as Cmt
    diffs = Cmt.get_tag_diff(ref=args.old,
                             chk=args.new,
                             verbose=True)

    if len(diffs) > 0:
        return 1
    return 0
