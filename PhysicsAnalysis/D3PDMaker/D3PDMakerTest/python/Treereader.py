# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


class Treereader (object):
    leaftypes = (int, float, complex)
    import builtins
    if 'long' in builtins.__dict__:
        leaftypes = leaftypes + (builtins.__dict__['long'],)
    if 'buffer' in builtins.__dict__:
        leaftypes = leaftypes + (builtins.__dict__['buffer'],)
    def __init__ (*self):
        raise Exception ("Use treereader() rather than creating Treereader directly.")

    def real_init (self, tree, parent = None, subpattern = None):
        self.parent = parent
        self._tree = tree
        self._subpattern = subpattern
        self._subreaders = []
        if self._subpattern is None:
            tree.SetNotify (self)
        return

    def __getattr__ (self, attr):
        if attr[0] == '_':
            raise AttributeError
        if self._subpattern:
            lname = self._subpattern % attr
        else:
            lname = attr
        ret = getattr (self._tree, lname)
        if (isinstance (ret, Treereader.leaftypes) or
            str(type(ret)) == "<type 'buffer'>"):
            def getter (self, tree = self._tree, lname = lname):
                return getattr (tree, lname)
            setattr (self.__class__, attr, property (getter))
        else:
            setattr (self, attr, ret)
        return ret
    
    def Notify (self):
        newdict = { '_tree' : self._tree,
                    '_subpattern' : self._subpattern,
                    '_subreaders' : self._subreaders}
        for (k, v) in self.__dict__.items():
            if isinstance (v, Treereader):
                newdict[k] = v
        self.__dict__ = newdict
        #print 'notify!', self.__dict__
        for s in self._subreaders:
            s.Notify()
        return
    

    def subreader (self, subpattern):
        sub = treereader (self._tree, self, subpattern)
        self._subreaders.append (sub)
        return sub


    def GetEntry (self, i):
        return self._tree.GetEntry (i)


def treereader (tree, parent = None, subpattern = None):
    class Treereader_Specific (Treereader):
        def __init__ (self, tree, subpattern = None):
            Treereader.real_init (self, tree, parent, subpattern)
            return
    return Treereader_Specific (tree, subpattern)
    

#tr=treereader(tt)

#tr55=tr.subreader('cl_%s')
#tr35=tr.subreader('cl_%s_35')
