# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def downloadUsingProxy(url, filename=None):
    '''Download URL using proxy setup from environment (ATLAS_PROXIES, fallback to FRONTIER_SERVER)'''
    import os
    if filename is None:
        filename = os.path.basename(url)
    proxyList = []
    if 'http_proxy' in os.environ:
        proxyList.append( os.environ['http_proxy'] )
    elif 'ATLAS_PROXIES' in os.environ:
        proxyList.extend( os.environ['ATLAS_PROXIES'].split(',') )
    elif 'FRONTIER_SERVER' in os.environ:
        frontierConfig = [ conf.lstrip('(') for conf in os.environ['FRONTIER_SERVER'].split(')') ]
        for conf in frontierConfig:
            if conf.startswith('proxyurl=http://'):
                proxyList.append(conf.split('proxyurl=http://')[1])
    proxyList.append(None) # a last try with no proxy is flagged by None
    msg = "Found proxy list %s\n" % proxyList
    for proxy in proxyList:
        if proxy is not None:
            os.environ['http_proxy'] = proxy
        elif 'http_proxy' in os.environ:
            del os.environ['http_proxy']
        cmd = "wget --waitretry=5 --tries=3 --connect-timeout=20 --read-timeout=120 -O %s %s" % (filename, url)
        msg += "Trying to retrieve '%s' using proxy '%s' via: %s\n" % (url, proxy, cmd)
        import commands
        status, output = commands.getstatusoutput(cmd)
        if status == 0:
            msg += "Downloaded %s using proxy '%s'\n" % (url, proxy)
            break
        else:
            msg += "Failed to get '%s' via proxy '%s'. exit status=%d, output='%s'\n" % (url, proxy, status, output)
            if proxy is None:
                msg += "All download attempts failed"
                return 1, msg
    return 0, msg
