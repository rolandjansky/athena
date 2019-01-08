## Filters out duplicate event(s) for use with the Afterburn steering, in which the
## first event seems to be duplicate
## @todo: should be part of TestHepMC?
if not hasattr(filtSeq, "DuplicateEventFilter"):
    from GeneratorFilters.GeneratorFiltersConf import DuplicateEventFilter
    filtSeq += DuplicateEventFilter()


