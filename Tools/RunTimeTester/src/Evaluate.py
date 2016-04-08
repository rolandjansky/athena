# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Replace xpath version Evaluate because I (PS)cant get xpath to work at cern
for the moment (14/6/05).
"""

from xml.dom.minidom import parse
from exc2string2     import exc2string2
from RTTSException   import RTTCodingError

import os.path
import re
import string

# ==================================================================
# ==================================================================
# ==================================================================

# This is the main function of this module, call this one
def Evaluate(path, element):
    # This function does everything except the washing up.
    # It can be used to grab just about any elements or attributes you wish
    # by an appropriately formed path parameter string.
    # 
    # ---- Elements:
    # 'a/b/c' ==> return a list of all matching elements c
    # 'a/b/c/text()' ==> return a list of the text content of all matching elements c
    # 'a/b[0]/c' ==> return a list of all c tags below the first b tag
    # '//b' ==> return a list of all b elements wherever they are situated
    # 
    # ---- Attributes
    # 'a/b["hello"]' ==> return a list of value of the 'hello' attribute in all matching b elements

    def isIndexSelector(sel):
        sel = sel[1:-1]
        try:
            int(sel)
            return True
        except:
            return False
    def isAttrSelector(sel):
        sel = sel[1:-1]
        return sel[0]==sel[-1] and sel[0] in ['"', "'"]
    def hasSelectors(path): # has at least one element like a[0] or a["attr"]?
        return len(re.findall('\[.*?\]', path)) > 0
    def getSelectors(path, onlyAttrs=False):
        sels = re.findall('\[.*?\]', path)
        if not onlyAttrs: return sels
        return [s for s in sels if not isIndexSelector(s)] # return items which have non-integers in them
    def wantsText(path):
        return path.endswith('/text()')
    def illegalPath(path):
        # Illegal characters in path?
        legal = ['.',',','-','_','"',"'",'(',')','/', '[', ']']
        punctuation = string.punctuation
        for b in legal:
            punctuation = punctuation.replace(b, '')

        ok = True not in [p in string.whitespace for p in path] # no whitespace chars
        ok = ok and (False not in [p in string.printable for p in path]) # must only contain printable chars
        ok = ok and (True not in [p in punctuation for p in path]) # must not contain punctuation chars
        ok = ok and (path.count('text()')==path.count('(') and path.count('text()')==path.count(')'))
        if not ok: return (True, "Illegal characters found in %s" % path)

        # Badly constructed path?
        bad = path.count('text()') > 1 or ('text()' in path and not path.endswith('text()'))
        if bad: return (True, "%s: Illegally placed 'text()'." % path)

        # Bad use of selectors in path?
        selectors = getSelectors(path, True) # get only attribute selectors i.e. a['hello'], rather than, say, a[9]
        if selectors:
            bad = len(selectors) > 1
            bad = bad or (selectors[0] not in path.split('/')[-1]) # bad = selector not at end of path
            bad = bad or (False in [a[0]=='/' for a in path.split(']')[1:] if a]) # bad = selector not followed by '/' or end of path
            bad = bad or (not isAttrSelector(selectors[0]) and not isIndexSelector(selectors[0]))
            if bad: return (True, "%s: Illegally placed attribute selector(s)." % path)
            
        # All ok
        return (False, "")
    def next(path):
        for thing in path.split('/'): yield thing
    def applySelector(els, sel):
        isAttr  = isAttrSelector(sel)
        isIndex = isIndexSelector(sel)
        if isAttr:
            sel = sel[2:-2] # remove [ and ], as well as quotes
            attrs = [el.getAttribute(sel) for el in els]
            if len(attrs)==1: return str(attrs[0])
            if len(attrs)>1: return attrs
            return None
        if isIndex:
            sel = sel[1:-1] # remove [ and ], as well as quotes
            return els[int(sel)] # may throw an out of bounds exception; list return required to allow selector chaining
        
    # -----------------------------------------------------------------------
    # ---- Method proper starts here, the above are nested helper functions
    # -----------------------------------------------------------------------
    isIllegal, message = illegalPath(path)
    if isIllegal:
        m = 'Evaluate: IllegalXPathExpression: %s\nTraceback:\n' % (message, exc2string2())
        raise RTTCodingError(m)
        

    # Does not have any selectors like a[0] or a['attr']
    if not hasSelectors(path):
        if not wantsText(path):
            return _Evaluate(path, element)

        # has a text() at the end
        pathMinusText = path[:path.rfind('/')]
        els = _Evaluate(pathMinusText, element)
        if not els: return ''
        blah = [_Evaluate('text()', el) for el in els]
        if len(blah)==1: return blah[0]
        return blah

    # OK, it does have selectors    
    parents = [element]
    result = None
    for tok in next(path):
        sels = getSelectors(tok)
        tok = tok.split('[')[0].strip() # remove any selectors from the string

        newparents = []
        for parent in parents:
            children = _Evaluate(tok, parent)
            for sel in sels:
                children = applySelector(children, sel)
                if type(children)==type('') or type(children)==type(u''): return str(children) # got a final answer, just return it
            if type(children)==type('') or type(children)==type(u''): return children # got a final answer, just return it

            try:
                newparents.extend(children)
            except:
                newparents.append(children)

        parents = newparents


    if wantsText(path):
        blah = [_Evaluate('text()', p) for p in parents]
        if len(blah)==1: return blah[0]
        return blah

    if len(parents)==1 and path.endswith(']'):
        return parents[0]
    return parents

# ==================================================================
# ==================================================================
# ==================================================================

def _Evaluate(path, element):
    paths = path.split('/')
    curGenEls = [element]
    for p in paths:
        nextGenEls = []
        if p == 'text()':
            texts = []
            [texts.extend(getText(e)) for e in curGenEls]
            text = reduce(lambda x,y:x+y, texts,'')
            text = text.strip()
            return text
        else:
            [nextGenEls.extend(e.childNodes) for e in curGenEls]
            nextGenEls = [n for n in nextGenEls if n.nodeName == p]
            
        curGenEls = nextGenEls
    return curGenEls

# ==================================================================
# ==================================================================
# ==================================================================

def getText(element):
    texts = [node.data for node in element.childNodes if node.nodeType == node.TEXT_NODE]
    text = reduce(lambda x,y:x+y, texts, '')
    return text.strip()

# ==================================================================
# ==================================================================
# ==================================================================

if __name__=="__main__":

    import getopt, sys, xml.dom.minidom
    from findRTThome import findRTThome

    from setupLoggerForDebug import setupLoggerForDebug
    setupLoggerForDebug()

    try:
        opts = getopt.getopt(sys.argv[1:],'c:')
    except getopt.GetoptError:
        print 'bad options'
        sys.exit(2)

    cfg = None
    for opt in opts[0]:
        print opt
        if opt[0] == '-c': cfg = opt[1]


    (base, cfg, src) = findRTThome(cfg)

    if not os.path.exists:
        print 'could not find cfg file',cfg

    dom = xml.dom.minidom.parse(cfg)

    pathsToEvaluate = ['rttconfig', 'mode', 'mode/text()']
    for p in pathsToEvaluate:
        print 'path to find ',p
        print Evaluate(p, dom.documentElement)
        
