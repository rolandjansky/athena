from __future__ import print_function
import ROOT
import cppyy

import sys
cppyy.loadDictionary("libDataModelTestDataCommonDict")
cppyy.loadDictionary("libDataModelTestDataReadDict")

def CHECK(sc):
    if not sc.isSuccess():
        raise Exception ('bad TReturnCode')
    return




#ROOT.xAOD.TEvent
#CHECK(ROOT.xAOD.Init())


reg=ROOT.SG.AuxTypeRegistry.instance()



cvec_cls=ROOT.DataVector(ROOT.DMTest.C_v1)
cel_cls=ROOT.ElementLink(cvec_cls)


# # Monkey-patch...
# ROOT.xAOD.TEvent.record_impl = ROOT.xAOD.TEvent.record
# def record (event, obj, key, basketSize=32000, splitLevel = 1):
#     return event.record_impl (obj, obj.__class__.__name__, key,
#                               basketSize, splitLevel)
# ROOT.xAOD.TEvent.record = record



def format_int(x): return '%d'%x
def format_float(x): return '%.1f'%x
def format_el(x): return '%s[%d]' % (x.dataID(), x.index())
def format_float_vec(v):
    l = [format_float(x) for x in v]
    return '[' + ','.join(l) + ']'
def format_int_vec(v):
    l = [format_int(x) for x in v]
    return '[' + ','.join(l) + ']'
accessors = {
    'int'   :  (ROOT.SG.AuxElement.ConstAccessor(int), format_int),
    'unsigned int'   :  (getattr (ROOT, 'SG::AuxElement::ConstAccessor<unsigned int>'), format_int),
    'float' :  (ROOT.SG.AuxElement.ConstAccessor(float), format_float),
    'std::vector<float>' : (getattr (ROOT, 'SG::AuxElement::ConstAccessor<std::vector<float> >'), format_float_vec),
    'std::vector<int>' : (getattr (ROOT, 'SG::AuxElement::ConstAccessor<std::vector<int> >'), format_int_vec),
    'ElementLink<DataVector<DMTest::C_v1> >' :
               (ROOT.SG.AuxElement.ConstAccessor(cel_cls), format_el),
    }

def dump_auxitem (x, auxid, f = sys.stdout):
    tname = reg.getTypeName (auxid)
    ac_p = accessors.get (tname)
    if not ac_p:
        print ('<unknown %s>'%tname, end='', file=f)
    else:
        (ac_cl, formatter) = ac_p
        val = ac_cl(reg.getName(auxid))(x)
        print (formatter(val) + '; ', end='', file=f)
    return


def dump_auxdata (x, exclude=[], f = sys.stdout):
    auxids = list(x.getAuxIDs())
    auxids = [(reg.getName(id), id) for id in auxids]
    auxids.sort()
    for name, auxid in auxids:
        if name in exclude: continue
        print (name + ': ', file=f, end='')
        dump_auxitem (x, auxid, f)
    return


def dump_c (c, f=sys.stdout):
    print ('  anInt1: %d; aFloat: %.1f; ' % (c.anInt(), c.aFloat()), end='')
    dump_auxdata (c, exclude = ['anInt1', 'aFloat'])
    print ('')
    return


def copy_obj (event, obj, key):
    copy = obj.__class__()
    copy_aux = obj.getConstStore().__class__()
    copy.setNonConstStore (copy_aux)
    copy.__assign__ (obj)
    CHECK (event.record (copy, key))
    CHECK (event.record (copy_aux, key + 'Aux.'))
    ROOT.SetOwnership (copy, False)
    ROOT.SetOwnership (copy_aux, False)
    return


def copy_vec (event, obj, key):
    copy = obj.__class__()
    copy_aux = obj.getConstStore().__class__()
    copy.setNonConstStore (copy_aux)
    for i in range(obj.size()):
        elt = obj[i].__class__()
        copy.push_back(elt)
        ROOT.SetOwnership (elt, False)
        elt.__assign__ (obj[i])
    CHECK (event.record (copy, key))
    CHECK (event.record (copy_aux, key + 'Aux.'))
    ROOT.SetOwnership (copy, False)
    ROOT.SetOwnership (copy_aux, False)
    return


class xAODTestRead:
    def __init__ (self, readPrefix = ''):
        self.readPrefix = readPrefix
        return


    def execute (self, tree, event=None):
        print (self.readPrefix + 'cvec')
        vec = getattr (tree, self.readPrefix + 'cvec')
        for c in vec:
            dump_c (c)

        print (self.readPrefix + 'cinfo')
        cinfo = getattr (tree, self.readPrefix + 'cinfo')
        dump_c (cinfo)

        print (self.readPrefix + 'ctrig')
        ctrig = getattr (tree, self.readPrefix + 'ctrig')
        for c in ctrig:
            dump_c (c)

        return
    

class xAODTestCopy:
    def __init__ (self, readPrefix = '', writePrefix = None):
        self.readPrefix = readPrefix
        self.writePrefix = writePrefix
        return

    def execute (self, tree, event):
        CHECK (event.copy (self.readPrefix + 'cvec'))
        CHECK (event.copy (self.readPrefix + 'cinfo'))
        CHECK (event.copy (self.readPrefix + 'ctrig'))

        if self.writePrefix != None:
            cinfo = getattr (tree, self.readPrefix + 'cinfo')
            copy_obj (event, cinfo, self.writePrefix + 'cinfo')

            cvec = getattr (tree, self.readPrefix + 'cvec')
            copy_vec (event, cvec, self.writePrefix + 'cvec')

            ctrig = getattr (tree, self.readPrefix + 'ctrig')
            copy_vec (event, ctrig, self.writePrefix + 'ctrig')
            
        return


class xAODTestDecor:
    def __init__ (self, decorName, offset=0, readPrefix = ''):
        self.readPrefix = readPrefix
        self.offset = offset
        self.decor = ROOT.SG.AuxElement.Decorator(int)(decorName)
        return

    
    def execute (self, tree, event=None):
        cvec = getattr (tree, self.readPrefix + 'cvec')
        for c in cvec:
            self.decor.set(c, self.offset + c.anInt())

        ctrig = getattr (tree, self.readPrefix + 'ctrig')
        for c in cvec:
            self.decor.set(c, self.offset + c.anInt())

        cinfo = getattr (tree, self.readPrefix + 'cinfo')
        self.decor.set(cinfo, self.offset + cinfo.anInt())
        return

    
class xAODTestPDecor:
    def __init__ (self, decorName, offset=0, readPrefix = ''):
        self.readPrefix = readPrefix
        self.offset = offset
        self.decor = ROOT.SG.AuxElement.Decorator(int)(decorName)
        return

    
    def execute (self, tree, event=None):
        cvec = getattr (tree, self.readPrefix + 'cvec')
        assert cvec.setOption (self.decor.auxid(), ROOT.SG.AuxDataOption ('nbins', 23))
        for c in cvec:
            self.decor.set(c, self.offset + c.anInt())
        return

    
class xAODTestClearDecor:
    def __init__ (self, readPrefix = ''):
        self.readPrefix = readPrefix
        return

    
    def execute (self, tree, event=None):
        cvec = getattr (tree, self.readPrefix + 'cvec')
        cvec.clearDecorations()

        ctrig = getattr (tree, self.readPrefix + 'ctrig')
        ctrig.clearDecorations()

        cinfo = getattr (tree, self.readPrefix + 'cinfo')
        cinfo.clearDecorations()
        return

    

class Analysis:
    def __init__ (self, ifname, ofname = None):
        self.algs = []
        self.f = ROOT.TFile (ifname)
        self.event = ROOT.xAOD.TEvent (ROOT.xAOD.TEvent.kClassAccess)
        CHECK (self.event.readFrom (self.f, True, 'CollectionTree'))
        self.tree = ROOT.xAOD.MakeTransientTreeFromEvent(self.event, 'CollectionTree')
        self.fout = None
        if ofname:
            self.fout = ROOT.TFile.Open (ofname, 'recreate')
            CHECK (self.event.writeTo (self.fout))
        return

    def add (self, alg):
        self.algs.append (alg)
        return

    def run (self, n=None):
        nent = self.tree.GetEntries()
        if n != None:
            nent = min (n, nent)
        for i in range(nent):
            self.tree.GetEntry(i)
            for a in self.algs:
                a.execute (self.tree, self.event)
            if self.fout != None:
                self.event.fill()
        return

    def finalize (self):
        CHECK (self.event.finishWritingTo (self.fout))
        return
    
    
