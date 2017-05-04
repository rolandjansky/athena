# dictionary defining a watch list for MR notifications
# keys   ... must be valid regular expression
# values ... must be sets containing Gitlab usernames as strings
WATCH_LIST = {}

WATCH_LIST['^CI$']       = set(['cgumpert'])
WATCH_LIST['Simulation'] = set(['ritsch','jchapman'])
WATCH_LIST['Digitization'] = set(['jchapman'])
WATCH_LIST['Overlay'] = set(['jchapman','ahaas','tkharlam'])
WATCH_LIST['TrkiPatFitter'] = set(['pop'])
WATCH_LIST['MooPerformance'] = set(['pop'])
WATCH_LIST['JetCalibTools'] = set(['jbossios'])
WATCH_LIST['AFP'] = set(['ggach'])
WATCH_LIST['^Database/A'] = set(['mnowak'])
WATCH_LIST['^Database/TPTools'] = set(['mnowak'])
WATCH_LIST['^Database/PersistentDataModel'] = set(['mnowak'])
WATCH_LIST['MuonSpectrometer'] = set(['jomeyer','wleight'])
WATCH_LIST['MuonIdentification'] = set(['jomeyer','wleight'])
WATCH_LIST['AGDD'] = set(['jomeyer'])
WATCH_LIST['(eflow)|(PFlow)|(PFO)'] = set(['mhodgkin'])
