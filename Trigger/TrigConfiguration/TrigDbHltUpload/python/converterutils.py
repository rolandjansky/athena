# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


__all__ = ['pythonize', 'xmlize', 'LineIter']

def pythonize(v):
    rd = [ ('[ ','['), (' ,',','), (' ]',']') ]
    return substitute(v, rd)

def xmlize(v):
    """ replaces tokens that are not valid in xml with corresponding values
    """
    rd = [ ('&',' &amp; '), ('>','&gt;'), ('<','&lt;'), ('\'','&#39;'), ('"','&quot;') ]
    return substitute(v, rd)

def substitute(v, rd):
    strv = v
    if not isinstance(strv,(unicode,str)) :
      strv = str(v)
    if v == None :
      strv=''
    for r in rd:
        strv = strv.replace(r[0],r[1])
    return strv

class LineIter:
    def __init__(self,lines):
        self.it = iter(lines)
        self.lc = 0
        self.end = False
        self.content = None
    def next(self):
        self.lc+=1
        try: self.content = self.it.next()
        except StopIteration: return False
        return True
    def __call__(self):
        return self.content
    def linecount(self):
        return self.lc
