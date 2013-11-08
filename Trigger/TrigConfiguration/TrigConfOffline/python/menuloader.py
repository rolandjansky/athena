
__all__ = [ 'l1menuloader', 'hltmenuloader' ]


def _findfile(filename):
    """resolve filename to full file name
    1) look if the file exists as named, if yes then return it
    2) if filename starts with . or / then assume it is a explicit path it should have been found at 1)
    3) then look for the file in the release
    3a) if there is a '/' in the filename then look through the XML path
    3b) if not, then assume the file is in TriggerMenuXML
    
    """
    import os
    if filename=="":
        raise RuntimeException("provided empty filename")
    # check if file exists
    if os.path.exists( filename ):
        return filename
    # must not start with "."
    if filename[0]=='/' or filename[0]=='.':
        raise RuntimeException("file %s does not exist" %s)

    if not '/' in filename:
        filename = "TriggerMenuXML/" + filename
    for xmldir in [x for x in os.getenv("XMLPATH").split(':')]:
        fullhltfile = xmldir + "/" + filename  
        if os.path.exists( fullhltfile ):
            return fullhltfile

    raise RuntimeException("Did not find file %s in any package directory in XML path")


class hltmenuloader:
    def __init__(self,filename):
        import ROOT
        self.menu     = ROOT.TrigConf.HLTFrame()
        self.filename = _findfile(filename)

        print "Loading HLT menu from %s" % self.filename 
        loader = ROOT.TrigConf.XMLStorageMgr(self.filename)
        loader.hltFrameLoader().load(self.menu)


class l1menuloader:
    def __init__(self,filename):
        import ROOT
        self.menu     = ROOT.TrigConf.CTPConfig()
        self.filename = _findfile(filename)
        self.load()

    def load(self):
        import ROOT
        print "Loading HLT menu from %s" % self.filename 
        loader = ROOT.TrigConf.XMLStorageMgr(self.filename)
        loader.ctpConfigLoader().load(self.menu)


