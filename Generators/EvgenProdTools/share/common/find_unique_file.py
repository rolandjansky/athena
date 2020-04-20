#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
## Helper functions for input file handling
def find_unique_file(pattern):
    "Return a matching file, provided it is unique"
    import glob
    files = glob.glob(pattern)
    ## Check that there is exactly 1 match
    if not files:
        raise RuntimeError("No '%s' file found" % pattern)
    elif len(files) > 1:
        raise RuntimeError("More than one '%s' file found" % pattern)
    return files[0]

