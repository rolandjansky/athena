# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def filter_bad_objects(records, method="runlumi"):
    """
    Some basic filtering of records that are likely bad.
    `method` determines whether records are in time or runlumi
    For runlumi, records must:
        * Start after run 70,000
        * End before run 1,000,000
        * not be longer than 5 runs (unless open-ended)
    In time, records must
        * Start after 2004
        * End before 2037 (unixtime breaks down ends anyway)
        * Not be longer than 10 years (unless open-ended)
    """
    from PyCool import cool
    
    if method == "runlumi":
        MIN_RUN = 70000
        MAX_RUN = 1000000
        MAX_LENGTH = 5 * (1 << 32) # Five runs
        keep_function = lambda x: (MIN_RUN < x.since.run < MAX_RUN
                              and (MIN_RUN < x.until.run < MAX_RUN
                                    or x.until == cool.ValidityKeyMax)
                               and (x.length < MAX_LENGTH
                                    or x.until == cool.ValidityKeyMax))
    elif method == "time":
        MIN_TIME = date_to_nanounix("01/01/2004")
        MAX_TIME = date_to_nanounix("01/01/2037")
        MAX_LENGTH = 10 * 365.25 * 24 * 60 * 60 * 1e9 # 10 years
        keep_function = lambda x: (MIN_TIME < x.since < MAX_TIME
                              and (MIN_TIME < x.until < MAX_TIME
                                   or x.until == cool.ValidityKeyMax)
                               and x.length < MAX_LENGTH)
    else:
        raise RuntimeError, "method must be 'runlumi' or 'time'"

    after = filter(keep_function, records)
    return after
