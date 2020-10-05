# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##
#
# @file PyDumper/python/Dumpers.py
# @author sss
# @date Jul 2007
# @brief python library to dump various EDM classes
#
# The code in this file is used to dump EDM classes to text.
# It should be usable from both ARA and Athena.
#

from __future__ import with_statement, print_function

__doc__ = """python library to dump various EDM classes"""
__version__ = "$Revision: 1.32 $"
__author__ = "Scott Snyder, Sebastien Binet"

### imports
from contextlib import contextmanager
import math
import sys
from io import StringIO
from functools import cmp_to_key
from math import \
     log as math_log,\
     sqrt as math_sqrt,\
     hypot as math_hypot

from AthenaPython import PyAthena
from PyUtils.fprint import fprint, fprintln, fwrite
import ROOT
import cppyy

cmp = lambda x, y: (x > y) - (x < y)

# not available due to a reflex bug.
etcone10 = 0
nucone10 = 8

# Without these, cling gets confused by forward declarations.
getattr (ROOT.xAOD, 'TrackParticleContainer_v1', None)
getattr (ROOT.xAOD, 'Jet_v1', None)

#Typed nullptr:
JetAssociationBase = getattr (cppyy.gbl, 'JetAssociationBase', None)
Analysis = getattr (cppyy.gbl, 'Analysis', None)
Muon = getattr (Analysis, 'Muon', None) if Analysis else None
jetAssocNull = cppyy.bind_object(cppyy.nullptr, JetAssociationBase) if JetAssociationBase else None
muonNull = cppyy.bind_object(cppyy.nullptr, Muon) if Muon else None


# Work around a cling bug.
if hasattr(ROOT,'TrackParticleTruthCollection'):
    ROOT.TrackParticleTruthCollection()[ROOT.Rec.TrackParticleTruthKey()]
    if hasattr(ROOT,'Jet'):
        ROOT.Jet().jetTagInfoVector()
    getattr(ROOT, 'ElementLinkVector<CaloClusterContainer>')
    if hasattr(ROOT,'Analysis::MuonContainer'):
        getattr(ROOT, 'ElementLink<Analysis::MuonContainer>')().isValid()
    getattr(ROOT, 'vector<xAOD::CaloClusterBadChannelData_v1>')().__assign__(getattr(ROOT, 'vector<xAOD::CaloClusterBadChannelData_v1>')())
if hasattr (ROOT, 'TrigInDetParticleTruth'):
    ROOT.TrigInDetTrackTruth().getFamilyTree()
    getattr(ROOT, 'ElementLinkVector<TrigInDetTrackCollection>')
    getattr(ROOT, 'std::vector<Trig3Momentum>')
    ROOT.TrigSpacePointCounts().droppedSctModules()


### helper methods ------------------------------------------------------------
def asinh (x):
    arg = math_sqrt (x*x+1) - x
    if arg == 0:
        return 1e30  # protect against loss of precision.
    return - math_log (arg)

def toiter (beg, end):
    while beg != end:
        yield beg.__deref__()
        beg.__preinc__()
    return

def toiter1 (c):
    return toiter (c.begin(), c.end())

def nolist (f):
    f.nolist = 1
    f.nmax = False
    return f

def nolist_nmax (f):
    f.nolist = 1
    f.nmax = True
    return f

def tonone (x):
    if not x: return None
    return x

def asint(x):
    if x>(1<<63):
        return x - (1<<64)
    return x

def asint32(x):
    if x>(1<<63):
        return x - (1<<64)
    elif x>(1<<31):
        return x - (1<<32)
    return x

def safe_float_vector(v):
    sa = ROOT.PyDumper.SafeFloatAccess(v)
    return [sa[i]for i in range(len(v))]

@contextmanager
def signalstate (o, state):
    if hasattr(o, 'setSignalState'):
        old = o.signalState()
        o.setSignalState (state)
        yield
        o.setSignalState (old)
    else:
        sh = PyAthena.SignalStateHelper (o)
        sh.setSignalState (state)
        yield
        sh.releaseObject
    return


def daz(f):
    if abs(f) < 1e-38:
        return 0.0
    return f


class forceInt:
    def __init__ (self, x):
        self.x = x
def formatItemUsingLong (x):
    """Helper for doing formatting compatibly with py2.

Convert X to a string, but add a `L' to the ends of integers.
Can be used for formatting expressions that using longs in py2.
"""
    if isinstance (x, list):
        sl = [formatItemUsingLong (xx) for xx in x]
        return '[' + ', '.join (sl)  + ']'
    if isinstance (x, tuple):
        sl = [formatItemUsingLong (xx) for xx in x]
        return '(' + ', '.join (sl)  + ')'
    if isinstance (x, int):
        return str(x) + 'L'
    if isinstance (x, forceInt):
        return str(x.x)
    return repr(x)


# For root 6.08, need to use __cppname__ rather than __name__
# for the name of a type if it's there.
def typename(t):
    return getattr (t, '__cpp_name__', t.__name__)

### library methods ------------------------------------------------------------

#change -0 -> 0
def fix_neg0 (x, thresh = 0):
    return 0 if x == 0 or abs(x) < thresh else x


def dump_H3V (v, f):
    fprint (f, "(%f %f %f)" %
            (v.perp(),v.eta(),v.phi()))
    return

def dump_HLV (v, f):
    m2 = v.e()**2 - v.px()**2 - v.py()**2 - v.pz()**2
    if m2 < 0:
        m = - math_sqrt (-m2)
    else:
        m = math_sqrt (m2)

    # Be insensitive to some rounding errors.
    m = fix_neg0 (m, 1e-3)
    #if abs(m-int(m)-0.5) < 1e-4: m += 0.01

    pt = math_hypot (v.px(), v.py())
    if pt < 1e-3:
        if v.pz() > 0:
            eta = 100
        else:
            eta = -100
    else:
        eta = asinh (v.pz() / pt)

    fprint (f, "(%f %f %f %f)" %
            (v.perp(),eta,v.phi(), m))
    return


def dump_Fourvec (v, f, parens=1):
    ParticleImpl_EEtaPhiM = getattr(PyAthena,'ParticleImpl<NavigableTerminalNode,P4ImplEEtaPhiM,ParticleEvent::Base>')

    #vmom = v.momentumBase() if hasattr(v, 'momentumBase') else None
    vmom = None
    if isinstance (v,    PyAthena.P4EEtaPhiMBase) or \
       isinstance (v,    ParticleImpl_EEtaPhiM) or \
       isinstance (vmom, PyAthena.P4BaseEEtaPhiM) or \
       isinstance (v, PyAthena.xAOD.IParticle):
        m = v.m()
        eta = v.eta()
        if m > v.e():
            pt = 0
        else:
            pt = v.pt()
    elif isinstance (v,    PyAthena.P4PtEtaPhiMBase) or \
         isinstance (vmom, PyAthena.P4BasePtEtaPhiM):
        m = v.m()
        eta = v.eta()
        pt = v.pt()
    elif isinstance (v,    PyAthena.P4IPtCotThPhiMBase) or \
         isinstance (vmom, PyAthena.P4BaseIPtCotThPhiM):
        m = v.m()
        ipt = v.iPt()
        cotth = v.cotTh()

        # Special case: avoid div-by-zero for null vector.
        if ipt == 0 and m == 0 and cotth == 0 and v.phi() == 0:
            pt = 0
            eta = 0
        else:
            aux = math_hypot(1, cotth)
            eta = math_log ((aux-cotth) / (aux+cotth))
            pt = 1 / ipt
    else: # PxPyPzE-based
        pt = v.pt()
        
        # Calculating m from px,py,pz,e is prone to FP significance
        # problems, as m may be small compared to momenta.
        # We can get different results from m() in debug/opt builds.
        # Do the calculation in python instead for consistency.
        m2 = v.e()**2 - v.px()**2 - v.py()**2 - v.pz()**2
        if m2 < 0:
            m = - math_sqrt (-m2)
        else:
            m = math_sqrt (m2)

        # Same thing shows up for eta.
        pt = math_hypot (v.px(), v.py())
        if pt == 0:
            if v.pz() > 0:
                eta = 100
            else:
                eta = -100
        else:
            eta = asinh (v.pz() / pt)

    #if abs(m-int(m)-0.5) < 1e-4: m += 0.01
    if parens: fprint (f, '(')
    fprint (f, "%f %f %f %f" % (pt, eta, v.phi(), m))
    if parens: fprint (f, ')')
    return

def dump_Threevec (v, f):
    fprint (f, "(%f %f %f)" % (fix_neg0(v.x(), thresh=1e-8),
                               fix_neg0(v.y(), thresh=1e-8),
                               fix_neg0(v.z(), thresh=1e-8)))
    return

def dump_Twovec (v, f):
    fprint (f, "(%f %f)" % (v.x(), v.y()))
    return

def dump_AmgMatrix (m, f, thresh=1e-38):
    fprint (f, '[')
    for r in range(m.rows()):
        fprint (f, '[')
        for c in range(m.cols()):
            v = m(r,c)
            if abs(v) < thresh: v = 0
            fprint (f, '%6.3g' % v)
        fprint (f, ']')
    fprint (f, ']')
    return

def dump_AmgVector (m, f, thresh=1e-38):
    if not m:
        fprint (f, '(null vector)')
        return
    fprint (f, '[')
    for r in range(m.rows()):
        v = m(r)
        if abs(v) < thresh: v = 0
        fprint (f, '%6.3g' % v)
    fprint (f, ']')
    return


def dump_EL (l, f):
    nm = typename(l.__class__)
    pos1 = nm.find('<')
    pos2 = nm.rfind ('>')
    nm = nm[pos1+1:pos2].strip()
    if l.isDefault():
        nm = nm + '/(null)'
    else:
        nm = nm + '/%s[%d]' % (l.dataID(), l.index())
    fprint (f, nm)
    return


def dump_ParticleBase (e, f):
    dump_Fourvec (e, f, 0)
    orig = e.originLink()
    if e.hasCharge():
        charge = "%f" % e.charge()
    else:
        charge = "N/A"
    fprint (f, " %1d %1d %4d %s" %
            (e.dataType(), e.hasPdgId(), asint32(e.pdgId()), charge))
    if orig.isValid():
        fprint (f, "%1d"%orig.index())
        dump_Threevec (e.origin().position(), f)
    else:
        fprint (f, "(no orig)")
    return


def dump_ParticleImpl (p, f):
    dump_Fourvec (p, f, 0)
    if  p.hasCharge():
        c = fix_neg0(p.charge())
        c = '%f' % c
    else:
        c = 'N/A'
    fprint (f, ' %d %s %5d ' %
            (p.dataType(), c, asint32(p.pdgId())))
    if p.origin():
        dump_Threevec (p.origin().position(), f)
    else:
        fprint (f, "(no orig)")
    return


def dump_EMConvert (d, f):
    fprint (f, d.convTrackMatch(), d.convAngleMatch())
    return

def dump_EMBremFit (d, f):
    ip = d.bremInvpT()
    iperr = d.bremInvpTerr()
    if ip==-999: ip = 0
    if iperr==-999: iperr = 0
    br = d.bremRadius()
    bx = d.bremX()
    if br == -999: br = 0
    if bx == -999: bx = 0
    nt = d.bremNTRT()
    nr = d.bremNR()
    if nt == -999: nt = 0
    if nr == -999: nr = 0
    d0 = d.bremD0()
    phi0 = d.bremPhi0()
    if d0 == -999: d0 = 0
    if phi0 == -999: phi0 = 0
    fprint (f, "%f %f %f %f %f %2d %2d %2d %2d %f %f" %
            (ip*1e6, br, bx, iperr*1e6,
             d.bremClusterRadius(), d.bremFitStatus(), d.bremErrorFlag(),
             nt, nr, d0, phi0))
    fprint (f, d.bremZ0(), d.bremDzDr(), d.bremNZ(), d.bremFitChi2())
    return

def dump_EMTrackMatch (d, f):
    fprint (f, "%f %f %4d %4d" %
            (d.deltaEta(2), d.deltaPhi(2),
             d.trackNumber(), d.tracksInBroadWindow()))
    fprint (f, d.trackRefit(), d.isPrimary())
    return

def dump_EMShower (d, f):
    weta1 = d.weta1()
    if weta1 < -10: weta1 = 0
    pos7 = d.pos7()
    if pos7 < -10: pos7 = 0
    # FIXME: backward compat
    if not hasattr(d, 'etconoisedR04Sig2'):
        d.etconoisedR04Sig2 = d.etconoise20
        d.etconoisedR04Sig3 = d.etconoise30
    fprint (f, d.e011(), d.e033(), d.ethad1(),
            d.ethad(), d.ehad1(), d.f1(), d.f3(), d.f1core(), d.f3core(),
            d.e233(), d.e235(), d.e255(), d.e237(), d.e277(), d.e333(),
            d.e335(), d.e337(), d.e377(), weta1, d.weta2(), d.e2ts1(),
            d.e2tsts1(), d.fracs1(), d.widths1(), d.widths2(),
            d.poscs1(), d.poscs2(), d.asy1(), d.pos(), pos7, d.barys1(),
            d.wtots1(), d.emins1(), d.emaxs1(), d.etcone(), d.etcone20(),
            d.etcone30(), d.etcone40(),
            d.etconoisedR04Sig2(), d.etconoisedR04Sig3(),
            d.ecore())
    if hasattr (d, 'e131'):
        fprint (f, d.e131(), d.e1153(), d.iso())
    else:
        fprint (f, d.e132(), d.e1152(), d.r33over37allcalo())
    return

# Sometimes, we can get denormal floats in the files we read.
# However, when reading, the denormals-as-zero flag can be set
# if we've loaded any library compiled with -ffast-math.
def uf(x):
    if abs(x) < 1e-38: return 0
    return x

def dump_EMErrorDetail (d, f):
    fprint (f, d.linkIndex())
    fprint (f, '\n     ',
            uf(d.EMconvertedphoton_perigee_d0()),
            uf(d.EMconvertedphoton_perigee_z0()),
            uf(d.EMconvertedphoton_perigee_phi0()),
            uf(d.EMconvertedphoton_perigee_theta()),
            uf(d.EMconvertedphoton_perigee_eta()),
            uf(d.EMconvertedphoton_perigee_momentum()),
            uf(d.EMconvertedphoton_perigee_Covd0d0 ()),
            uf(d.EMconvertedphoton_perigee_Covd0z0 ()),
            uf(d.EMconvertedphoton_perigee_Covd0phi ()),
            uf(d.EMconvertedphoton_perigee_Covd0theta ()),
            uf(d.EMconvertedphoton_perigee_Covz0z0   ()),
            uf(d.EMconvertedphoton_perigee_Covz0phi  ()),
            uf(d.EMconvertedphoton_perigee_Covz0theta ()),
            uf(d.EMconvertedphoton_perigee_Covphiphi ()),
            uf(d.EMconvertedphoton_perigee_Covphitheta ()),
            uf(d.EMconvertedphoton_perigee_Covthetatheta ()))
    fprint (f, '\n     ',
            uf(d.EMtrack_perigee_d0    ()),
            uf(d.EMtrack_perigee_phi0  ()),
            uf(d.EMtrack_perigee_z0    ()),
            uf(d.EMtrack_perigee_theta ()),
            uf(d.EMtrack_perigee_eta    ()),
            uf(d.EMtrack_perigee_momentum ()),
            uf(d.EMtrack_perigee_Covd0d0         ()),
            uf(d.EMtrack_perigee_Covd0z0         ()),
            uf(d.EMtrack_perigee_Covd0phi        ()),
            uf(d.EMtrack_perigee_Covd0theta      ()),
            uf(d.EMtrack_perigee_Covz0z0         ()),
            uf(d.EMtrack_perigee_Covz0phi        ()),
            uf(d.EMtrack_perigee_Covz0theta      ()),
            uf(d.EMtrack_perigee_Covphiphi       ()),
            uf(d.EMtrack_perigee_Covphitheta     ()),
            uf(d.EMtrack_perigee_Covthetatheta   ()))
    fprint (f, '\n     ',
            uf(d.EMphoton_eta ()),
            uf(d.EMphoton_phi0 ()),
            uf(d.EMphoton_Eclus ()),
            uf(d.EMphoton_Covetaeta      ()),
            uf(d.EMphoton_Covetaphi     ()),
            uf(d.EMphoton_CovetaEclus      ()),
            uf(d.EMphoton_Covphiphi    ()),
            uf(d.EMphoton_CovphiEclus    ()),
            uf(d.EMphoton_CovEclusEclus ()))
    fprint (f, '\n     ',
            uf(d.EMtrack_comb_Covd0d0         ()),
            uf(d.EMtrack_comb_Covd0z0         ()),
            uf(d.EMtrack_comb_Covd0phi        ()),
            uf(d.EMtrack_comb_Covd0eta      ()),
            uf(d.EMtrack_comb_Covd0P     ()),
            uf(d.EMtrack_comb_Covz0z0         ()),
            uf(d.EMtrack_comb_Covz0phi        ()),
            uf(d.EMtrack_comb_Covz0eta      ()),
            uf(d.EMtrack_comb_Covz0P     ()),
            uf(d.EMtrack_comb_Covphiphi       ()),
            uf(d.EMtrack_comb_Covphieta     ()),
            uf(d.EMtrack_comb_CovphiP    ()),
            uf(d.EMtrack_comb_Covetaeta   ()),
            uf(d.EMtrack_comb_CovetaP  ()),
            uf(d.EMtrack_comb_CovPP ()))
    return


def dump_EMTrackFit (d, f):
    fprint (f, '\n     ',
            d.track_perigee_d0    (),
            d.track_perigee_phi0  (),
            d.track_perigee_qOverP(),
            d.track_perigee_z0    (),
            d.track_perigee_theta (),
            d.track_perigee_Covd0d0         (),
            d.track_perigee_Covd0z0         (),
            d.track_perigee_Covd0phi        (),
            d.track_perigee_Covd0theta      (),
            d.track_perigee_Covd0qOverP     (),
            d.track_perigee_Covz0z0         (),
            d.track_perigee_Covz0phi        (),
            d.track_perigee_Covz0theta      (),
            d.track_perigee_Covz0qOverP     (),
            d.track_perigee_Covphiphi       (),
            d.track_perigee_Covphitheta     (),
            d.track_perigee_CovphiqOverP    (),
            d.track_perigee_Covthetatheta   (),
            d.track_perigee_CovthetaqOverP  (),
            d.track_perigee_CovqOverPqOverP ())
    fprint (f, '\n     ',
            d.track_LastM_loc1    (),
            d.track_LastM_loc2    (),
            d.track_LastM_phi     (),
            d.track_LastM_theta   (),
            d.track_LastM_qOverP  (),
            d.bremTrackAuthor   (),
            d.hasBrem           (),
            d.bremRadius      (),
            d.bremRadiusErr   (),
            d.bremDeltaZ      (),
            d.bremDeltaZerr   (),
            d.bremMaterialTraversed   (),
            d.bremFitChi2(),
            d.bremFitStatus   (),
            d.linkIndex         ())
    return

def dump_egamma (e, f):
    #cppyy.loadDictionary ('libElectronPhotonSelectorToolsDict')
    dump_ParticleImpl (e, f)
    fprint (f, "\n      %d %d %d" % (e.author(), e.isem(), e.isemse()))
    if e.clusterElementLink().isValid():
        fprint (f,  '\n      cl: %2d:' % e.clusterElementLink().index())
        dump_Fourvec (e.cluster(), f)
    for i in range(e.nTrackParticles()):
        tp = e.trackParticleElementLink(i)
        if tp.isValid():
            fprint (f,  '\n      tp: %2d:' %tp.index())
            dump_Fourvec (e.trackParticle(i), f)
    for i in range(e.nConversions()):
        cv = e.conversionElementLink(i)
        if cv.isValid():
            fprint (f,  '\n      cv: %2d:' % cv.index())
            dump_Threevec (e.conversion(i).recVertex().position(), f)
    for i in range (e.nDetails()):
        if e.detailElementLink(i).isValid():
            d = e.detail(i)
            fprint (f, '\n      %s: ' % d.className())
            if isinstance (d, PyAthena.EMConvert):
                dump_EMConvert (d, f)
            elif isinstance (d, PyAthena.EMBremFit):
                dump_EMBremFit (d, f)
            elif isinstance (d, PyAthena.EMTrackMatch):
                dump_EMTrackMatch (d, f)
            elif isinstance (d, PyAthena.EMShower):
                dump_EMShower (d, f)
            elif isinstance (d, PyAthena.EMErrorDetail):
                dump_EMErrorDetail (d, f)
            elif isinstance (d, PyAthena.EMTrackFit):
                dump_EMTrackFit (d, f)
            else:
                fprint (f, d)
        else:
            fprint (f, '\n      Detail link %d not valid; skipped.' % i)
    try:
        pid = PyAthena.egammaPIDObs
    except TypeError:
        pid = PyAthena.egammaPID
    fprint (f, '\n      pid: %g %g %f %g %f %f' % 
            (e.egammaID(pid.ElectronWeight), e.egammaID(pid.BgWeight),
             e.egammaID(pid.NeuralNet), e.egammaID(pid.Hmatrix),
             e.egammaID(pid.IsolationLikelihood_jets),
             e.egammaID(pid.IsolationLikelihood_HQDelectrons)))
    return

def dump_Electron (e, f):
    dump_egamma (e, f)
    return

def dump_Photon (e, f):
    dump_egamma (e, f)
    return


def dump_EnergyLoss (p, f):
    if not p:
        fprint (f, None)
        return
    fprint (f, p.deltaE(), p.sigmaDeltaE(), p.sigmaMinusDeltaE(), p.sigmaPlusDeltaE())
    fprint (f, p.meanIoni(), p.sigmaIoni(), p.meanRad(), p.sigmaRad(), p.length())
    return


def dump_CaloEnergy (p, f):
    dump_EnergyLoss (p, f)
    fprint (f, '%d %f %f %f %f' %
            (p.energyLossType(),
             p.caloMuonIdTag(),
             p.caloLRLikelihood(),
             p.fsrCandidateEnergy(),
             p.etCore()))
    fprint (f, p.deltaEParam(), p.sigmaMinusDeltaEParam(), p.sigmaPlusDeltaEParam(), p.deltaEMeas(), p.sigmaDeltaEMeas())
    fprint (f, '\n      deposits:')
    for d in p.depositInCalo():
        fprint (f, " (%d %f %f)" % 
                (d.subCaloId(), d.energyDeposited(), d.muonEnergyLoss()))
    return


def dump_Muon (m, f):
    dump_ParticleImpl (m, f)
    fprint (f, ' %1d' % m.author())
    if m.inDetTrackLink().isValid():
        fprint (f,  '\n      idtp: %2d:' % m.inDetTrackLink().index())
        dump_Fourvec (m.inDetTrackParticle(), f)
    # This isn't in 13.X.0 xxx
    if hasattr(m, 'muonSpTrackLink') and m.muonSpTrackLink().isValid():
        fprint (f,  '\n      sptp: %2d:' % m.muonSpTrackLink().index())
        dump_Fourvec (m.muonSpectrometerTrackParticle(), f)
    if m.muonExtrapTrackLink().isValid():
        fprint (f,  '\n      mxtp: %2d:' % m.muonExtrapTrackLink().index())
        dump_Fourvec (m.muonExtrapolatedTrackParticle(), f)
    if m.innerExtrapTrackLink().isValid():
        fprint (f,  '\n      ixtp: %2d:' % m.innerExtrapTrackLink().index())
        dump_Fourvec (m.innerExtrapolatedTrackParticle(), f)
    if m.combinedTrackLink().isValid():
        fprint (f,  '\n      cmtp: %2d:' % m.combinedTrackLink().index())
        dump_Fourvec (m.combinedMuonTrackParticle(), f)
    if m.clusterLink().isValid():
        fprint (f,  '\n      cl: %2d:' % m.clusterLink().index())
        dump_Fourvec (m.cluster(), f)
    fprint (f, '\n      a: %f %f %d %f %d %f %d' % 
            (m.matchChi2(), m.matchChi2OverDoF(), m.matchNumberDoF(),
             m.fitChi2(), m.fitNumberDoF(), m.fitChi2OverDoF(),
             m.bestMatch()))
    fprint (f, '\n      b: %d %d %d %d %d %d %d %d %d %d %d %d' % 
            (m.numberOfInnermostPixelLayerHits(),
             m.numberOfPixelHits(),
             m.numberOfSCTHits(),
             m.numberOfTRTHits(),
             m.numberOfTRTHighThresholdHits(),
             m.numberOfInnermostPixelLayerSharedHits(),
             m.numberOfPixelSharedHits(),
             m.numberOfPixelHoles(),
             m.numberOfSCTSharedHits(),
             m.numberOfSCTHoles(),
             m.numberOfTRTOutliers(),
             m.numberOfTRTHighThresholdOutliers()))
    fprint (f, '\n      c: %d %d %d %d %d %d %d %d %d %d %d %d %d %d' % 
            (m.numberOfMDTHits(),
             m.numberOfMDTHoles(),
             m.numberOfCSCEtaHits(),
             m.numberOfCSCEtaHoles(),
             m.numberOfCSCPhiHits(),
             m.numberOfCSCPhiHoles(),
             m.numberOfRPCEtaHits(),
             m.numberOfRPCEtaHoles(),
             m.numberOfRPCPhiHits(),
             m.numberOfRPCPhiHoles(),
             m.numberOfTGCEtaHits(),
             m.numberOfTGCEtaHoles(),
             m.numberOfTGCPhiHits(),
             m.numberOfTGCPhiHoles()))
    fprint (f, '\n      d: %d %d %f %f %f %f %f %f' % 
            (m.numberOfGangedPixels(),
             m.numberOfOutliersOnTrack(),
             m.segmentDeltaEta(),
             m.segmentChi2OverDoF(),
             m.annBarrel(),
             m.annEndCap(),
             m.innAngle(),
             m.midAngle()))
    fprint (f, '\n      e: %f %f %d %d' % 
            (m.parameter(etcone10),
             m.parameter(nucone10),
             m.alsoFoundByLowPt(),
             m.alsoFoundByCaloMuonId()))
    fprint (f, '\n      f:', [i for i in m.associatedEtaDigits()])
    fprint (f, '\n      h:', [i for i in m.associatedPhiDigits()])
    fprint (f, '\n      i:', [i for i in m.segmentEtaDigits()])
    fprint (f, '\n      j:', [i for i in m.segmentPhiDigits()])
    caloe = m.caloEnergyLoss()
    if caloe:
        fprint (f, '\n      k:')
        dump_CaloEnergy (caloe, f)
    return


def dump_TauDetail (t, f):
    nm = t.className()
    if nm.startswith('Analysis::'):
        nm = nm[10:]
    fprint (f, '%s:' % nm)
    return
    
def dump_Tau1P3PDetails (t, f):
    dump_TauDetail (t, f)
    fprint (f, ' %f %1d %f %f %1d %1d %f %f %f %f' %
            (t.emRadius(),
             t.numStripCells(),
             t.etChrgHAD(),
             t.etIsolHAD(),
             t.nAssocTracksCore(),
             t.nAssocTracksIsol(),
             t.etEMAtEMScale(),
             t.etChrgEM(),
             t.etNeuEM(),
             t.etChrgEM01Trk(0)))
    fprint (f, '\n        ')
    fprint (f, ' %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f' %
            (t.isolationFraction(),
             t.stripWidth2(),
             t.etIsolEM(),
             t.massTrk3P(),
             t.rWidth2Trk3P(),
             t.signD0Trk3P(),
             t.z0SinThetaSig(),
             t.etChrgHADoverPttot(),
             t.etIsolFrac(),
             t.etEflow(),
             t.etHadAtEMScale(),
             t.etEMCL(),
             t.etResNeuEM(),
             t.trFlightPathSig(),
             t.etResChrgEMTrk(0)))
    fprint (f, '\n        ')
    dump_HLV (t.sumEM(), f)
    if t.secVertex():
        dump_Threevec (t.secVertex().position(), f)
    if t.cellEM012Cluster():
        dump_Fourvec (t.cellEM012Cluster(), f)
    for i in range(t.numPi0()):
        fprint (f, '\n        pi: ')
        dump_Fourvec (t.pi0 (i), f)
    return
  

def dump_TauRecDetails (t, f):
    dump_TauDetail (t, f)
    # numTrack, trackCaloEta, trackCaloPhi
    fprint (f, '%f %f %f %f %2d %f %f %f' %
            (t.emRadius(),
             t.isolationFraction(),
             t.centralityFraction(),
             t.stripWidth2(),
             t.numStripCells(),
             t.etEMCalib(),
             t.etHadCalib(),
             t.leadingTrackPT()))
    fprint (f, '\n        ')
    for i in range(t.numTrack()):
        fprint (f, '(%f %f) ' % (t.trackCaloEta(i), t.trackCaloPhi(i)))
    fprint (f, '\n        ')
    dump_HLV (t.sumEM(), f)
    if t.secVertex():
        dump_Threevec (t.secVertex().position(), f)
    return

    
def dump_Tau1P3PExtraDetails (t, f):
    dump_TauDetail (t, f)
    fprint (f, t.sumPTTracks())
    # Can't unpack cells in ARA; don't dump:
    #  closestEtaTrkVertCell
    #  closestPhiTrkVertCell
    #  closestEtaTrkCell
    for tr in range(t.ntrack()):
        for s in range(t.nsamp()):
            fprint (f, '\n        ', tr, s,
                    t.etaTrackCaloSamp(tr,s), t.phiTrackCaloSamp(tr,s))
    return


def dump_TauRecExtraDetails (t, f):
    dump_TauDetail (t, f)
    fprint (f, t.numEMCells(), t.stripET(), t.emCentralityFraction(),
            t.etHadAtEMScale(), t.etEMAtEMScale(), t.energy(), t.emEnergy(),
            t.sumPTTracks(), t.seedType(), tonone(t.analysisHelper()))
    return


def dump_TauPi0Candidate (t, f):
    dump_HLV (t.hlv(), f)
    fprint (f, ' [')
    for c in t.pi0ClusterVec():
        fprint (f, c.clusterLink().index(), ', ')
    fprint (f, ']')
    return


def dump_TauPi0Cluster (t, f):
    fprint (f, t.clusterLink().index(),
            t.deltaPhi(),
            t.deltaTheta(),
            t.engFracEM(),
            t.engFracMAX(),
            t.engFracCore(),
            t.secondEngDens(),
            t.LATERAL(),
            t.LONGITUDINAL(),
            t.EM1CoreFrac(),
            t.asymmetryInEM1WRTTrk(),
            t.NHitsInEM1(),
            t.NPosECells_PS(),
            t.NPosECells_EM1(),
            t.NPosECells_EM2(),
            t.firstEtaWRTClusterPosition_EM1(),
            t.firstEtaWRTClusterPosition_EM2(),
            t.secondEtaWRTClusterPosition_EM1(),
            t.secondEtaWRTClusterPosition_EM2(),
            t.BDTScore())
    return
          


def dump_TauPi0Details (t, f):
    dump_TauDetail (t, f)
    dump_HLV (t.visTauhlv(), f)
    fprint (f, '  candidates')
    for c in t.pi0CandidateVector():
        fprint (f, '\n    ')
        dump_TauPi0Candidate (c, f)
    fprint (f, '  clusters')
    for c in t.pi0ClusterVector():
        fprint (f, '\n    ')
        dump_TauPi0Cluster (c, f)
    return


def dump_TauCommonDetails (t, f):
    dump_TauDetail (t, f)
    fprint (f, t.ipZ0SinThetaSigLeadTrk(), t.etOverPtLeadTrk(),
            t.leadTrkPt(), t.ipSigLeadTrk(), t.ipSigLeadLooseTrk(),
            t.etOverPtLeadLooseTrk(), t.leadLooseTrkPt(), t.chrgLooseTrk(),
            t.massTrkSys(), t.trkWidth2())
    fprint (f, '\n        ')
    fprint (f, t.trFlightPathSig(), t.etEflow(), t.seedCalo_EMRadius(),
            t.seedCalo_hadRadius(), t.seedCalo_etEMAtEMScale(),
            t.seedCalo_etHadAtEMScale(), t.seedCalo_isolFrac(),
            t.seedCalo_centFrac(), t.seedCalo_stripWidth2(), t.seedCalo_nStrip())
    fprint (f, '\n        ')
    fprint (f, t.seedCalo_etEMCalib(), t.seedCalo_etHadCalib(),
            t.seedCalo_eta(), t.seedCalo_phi(), t.seedCalo_nIsolLooseTrk(),
            t.seedTrk_EMRadius(), t.seedTrk_isolFrac(),
            t.seedTrk_etChrgHadOverSumTrkPt(), t.seedTrk_isolFracWide(),
            t.seedTrk_etHadAtEMScale())
    fprint (f, '\n        ')
    fprint (f, t.seedTrk_etEMAtEMScale(), t.seedTrk_etEMCL(),
            t.seedTrk_etChrgEM(), t.seedTrk_etNeuEM(),
            t.seedTrk_etResNeuEM(), t.seedTrk_hadLeakEt(),
            t.seedTrk_sumEMCellEtOverLeadTrkPt(), t.seedTrk_secMaxStripEt(),
            t.seedTrk_stripWidth2(), t.seedTrk_nStrip())
    fprint (f, '\n        ')
    fprint (f, t.seedTrk_etChrgHad(), t.seedTrk_nOtherCoreTrk(),
            t.seedTrk_nIsolTrk(), t.seedTrk_etIsolEM(), t.seedTrk_etIsolHad())
    fprint (f, '\n        %d loose tracks' % t.nLooseTrk())
    looseTrks = t.looseTrk()
    for i in range(looseTrks.size()):
        el = looseTrks[i]
        fprint (f, '\n          ', el.index(), el.dataID(),
               t.seedTrk_etChrgEM01Trk(i), t.seedTrk_etResChrgEMTrk(i))
    fprint (f, '\n        %d pi0s' % t.nPi0())
    vec = t.pi0LinkVec()
    for i in range(vec.size()):
        el = vec[i]
        # nb. indices may vary from run to run.  list isn't sorted?
        fprint (f, '\n          ', el.dataID(), el.e())
    fprint (f, '\n        pi0 sum:')
    dump_HLV (t.sumPi0Vec(), f)
    cl = t.cellEM012ClusterLink()
    fprint (f, '\n        cluster:')
    if cl.isValid():
        fprint (f, cl.index(), cl.dataID())
    else:
        fprintln (f, '(invalid)')
    fprint (f, '\n        secvertex:')
    if t.secVtx():
        dump_RecVertex (t.secVtx(), f)
    else:
        fprint (f, None)
    return


def dump_vvelccell (c, f):
    fprint (f, '[')
    for v in c:
        fprint (f, '[')
        for el in v:
            if el.isValid():
                fprint (f, '(', el.dataID(), el.index(), ') ')
            else:
                fprint (f, '(invEL) ')
        fprint (f, ']')
    fprint (f, ']')
    return

    
def dump_vvdouble (c, f):
    fprint (f, '[')
    for v in c:
        fprint (f, '[')
        for d in v:
            fprint (f, d, ',')
        fprint (f, ']')
    fprint (f, ']')
    return

    
def dump_TauCommonExtraDetails (t, f):
    dump_TauDetail (t, f)
    fprint (f, t.sumPtLooseTrk(), t.sumPtTrk(), t.seedCalo_nEMCell(),
            t.seedCalo_stripEt(), t.seedCalo_EMCentFrac(),
            t.seedCalo_sumCellEnergy(), t.seedCalo_sumEMCellEnergy())
    fprint (f, '\n    ')
    dump_vvelccell (t.closestEtaTrkVertCell(), f)
    fprint (f, '\n        ')
    dump_vvelccell (t.closestPhiTrkVertCell(), f)
    fprint (f, '\n        ')
    dump_vvelccell (t.closestEtaTrkCell(), f)
    fprint (f, '\n        ')
    dump_vvelccell (t.closestPhiTrkCell(), f)
    fprint (f, '\n        ')
    dump_vvdouble (t.etaTrkCaloSamp(), f)
    fprint (f, '\n        ')
    dump_vvdouble (t.phiTrkCaloSamp(), f)
    fprint (f, '\n        ')
    dump_vvdouble (t.etaLooseTrkCaloSamp(), f)
    fprint (f, '\n        ')
    dump_vvdouble (t.phiLooseTrkCaloSamp(), f)
    return


def dump_TauJet (t, f):
    dump_ParticleImpl (t, f)
    fprint (f, ' %1d' % (t.author(),))
    # xxx numberOfTracks doesn't seem to be initialized in reco --- don't dump.
    if t.clusterLink().isValid():
        fprint (f,  '\n      cl: %2d:' % t.clusterLink().index())
        dump_Fourvec (t.cluster(), f)
    if t.cellClusterLink().isValid():
        # Note: cluster container is unsorted.  Means that indices
        # are unstable.  So don't print them.
        fprint (f,  '\n      ce: ')
        dump_Fourvec (t.cellCluster(), f)
    if t.jetLink().isValid():
        fprint (f,  '\n      je: %2d:' % t.jetLink().index())
        j = t.jet()
        if j.e() == 0 and j.phi() == 0 and j.eta() == 0:
            with signalstate (j, PyAthena.P4SignalState.JETEMSCALE):
                dump_Fourvec (j, f)
        else:
            dump_Fourvec (j, f)
    for i in range (t.numTrack()):
        el = t.trackLinkVector()[i]
        if el.isValid():
            fprint (f,  '\n      t%d: %2d:' % (i,el.index()))
            dump_Fourvec (t.track(i), f)
    pid = t.tauID()
    TJP = PyAthena.TauJetParameters
    if pid:
        fprint (f, '\n      pid: %f %f %f %d %d %d' %
                (pid.discriminant(0),
                 pid.discriminant(1),
                 pid.discriminant(2),
                 pid.flag (TJP.ElectronFlag),
                 pid.flag (TJP.EgammaFlag),
                 pid.flag (TJP.MuonFlag)))
        if hasattr (TJP, 'TauCutLoose'):
            flags = [TJP.TauCutLoose,
                     TJP.TauCutMedium,
                     TJP.TauCutTight,
                     TJP.ElectronVeto,
                     TJP.MuonVeto,
                     TJP.TauLlhTight,
                     TJP.TauLlhMedium,
                     TJP.TauLlhLoose,
                     ]
            if hasattr (TJP, 'TauRecOld'):
                flags.append (TJP.TauRecOld)
            if hasattr (TJP, 'Tau1P3POld'):
                flags.append (TJP.Tau1P3POld)
            fprint (f,  '\n          ', [pid.isTau(ff) for ff in flags])
        else:
            fprint (f,  '\n          ', pid.isTau())
    for i in range (t.nDetails()):
        fprint (f, '\n      ')
        if t.detailElementLink(i).isValid():
            d = t.detail(i)
            if (hasattr (PyAthena.Analysis, 'Tau1P3PDetails') and
                isinstance (d, PyAthena.Analysis.Tau1P3PDetails)):
                dump_Tau1P3PDetails (d, f)
            elif (hasattr (PyAthena.Analysis, 'TauRecDetails') and
                  isinstance (d, PyAthena.Analysis.TauRecDetails)):
                dump_TauRecDetails (d, f)
            elif (hasattr (PyAthena.Analysis, 'Tau1P3PExtraDetails') and
                  isinstance (d, PyAthena.Analysis.Tau1P3PExtraDetails)):
                dump_Tau1P3PExtraDetails (d, f)
            elif (hasattr (PyAthena.Analysis, 'TauRecExtraDetails') and
                  isinstance (d, PyAthena.Analysis.TauRecExtraDetails)):
                dump_TauRecExtraDetails (d, f)
            elif (hasattr (PyAthena.Analysis, 'TauPi0Details') and
                  isinstance (d, PyAthena.Analysis.TauPi0Details)):
                dump_TauPi0Details (d, f)
            elif isinstance (d, PyAthena.Analysis.TauCommonDetails):
                dump_TauCommonDetails (d, f)
            elif isinstance (d, PyAthena.Analysis.TauCommonExtraDetails):
                dump_TauCommonExtraDetails (d, f)
            else:
                fprint (f, t.detailName(i), d, '(not dumped)')
    fprint (f, '\n      p: %f %f %f' %
            (t.parameter (0),
             t.parameter (3),
             t.parameter (6)))
    return


def dump_BaseTagInfo (info, f):
    fprint (f, ' '.join (['%f'%x for x in info.tagLikelihood()]),
            info.weight(), info.isValid())
    return


def dump_TruthInfo (info, f):
    dump_BaseTagInfo (info, f)
    fprint (f, ' %s %f %d ' %
            (info.jetTruthLabel(),
             info.deltaRMinTo ('b'),
             info.BHadronPdg()))
    dump_Threevec (info.BDecayVertex(), f)
    return


def dump_SLTrueInfo (info, f):
    fprint (f, '%d %d %d %d %d %d ' %
            (info.barcode(),
             info.pdgId(),
             info.pdgIdMother(),
             info.FromB(),
             info.FromD(),
             info.FromGH(),))
    dump_H3V (info.momentum(), f)
    fprint (f, ' ')
    dump_H3V (info.prodvtx(), f)
    return

def dump_SoftLeptonTruthInfo (info, f):
    dump_BaseTagInfo (info, f)
    for i in range (info.numSLTrueInfo()):
        fprint (f, '\n        ')
        dump_SLTrueInfo (info.getSLTrueInfo(i), f)
    return


def dump_SecVtxInfo (info, f):
    dump_BaseTagInfo (info, f)
    fprint (f, '\n        %d %d %d %d %f %f %f %f %f' %
            (info.fitType(),
             info.numSelTracksForFit(),
             info.mult(),
             info.NumberOfG2TrackVertices(),
             info.distance(),
             info.rphidistance(),
             info.probability(),
             info.mass(),
             info.energyFraction()))
    return dump_Threevec (info.secVertexPos().position(), f)


def dump_IPTrackInfo (info, f):
    dump_Fourvec (info.track(), f)
    fprint (f, ' %d %s %d  %f %f %f %f %f %f %f' %
            (info.trackGrade().gradeNumber(),
             info.trackGrade().gradeString(),
             info.isFromV0(),
             info.d0Value(),
             info.d0Significance(),
             info.z0Value(),
             info.z0Significance(),
             info.trackWeight2D(),
             info.trackWeight3D(),
             info.trackProbJP()))
    return

def dump_IPInfoPlus (info, f):
    dump_BaseTagInfo (info, f)
    fprint (f, ' %d' % info.numTrackInfo())
    for i in range(info.numTrackInfo()):
        fprint (f, '\n        ')
        dump_IPTrackInfo (info.getTrackInfo(i), f)
    return


def dump_IPInfoBase (info, f):
    dump_BaseTagInfo (info, f)
    fprint (f, info.nbTracks())
    return


def dump_SVInfoBase (info, f):
    dump_BaseTagInfo (info, f)
    return


def dump_SVTrackInfo(info, f):
    tp = info.track()
    if not tp:
        fprint (f, tp)
    else:
        dump_Fourvec (tp, f)
    return

    
def dump_SVInfoPlus (info, f):
    dump_BaseTagInfo (info, f)
    fprint (f, info.getNGTrackInJet(),
            info.getNGTrackInSvx(),
            info.getN2T(),
            info.getMass(),
            info.getEnergyFraction(),
            info.getNormDist(),
            info.numTrackInfo())
    fprint (f, '\n    vert: ')
    dump_RecVertex (info.getRecSvx(), f)
    fprint (f, '\n    tracks: ')
    # Track infos are unsorted.
    # Sort on pt for printing.
    ti = [info.getTrackInfo(i) for i in range(info.numTrackInfo())]
    ti.sort (key=lambda a: a.track().pt(), reverse=True)
    i = info.numTrackInfo()-1 # FIXME: wasn't counting
    for tt in ti:
        fprint (f, '\n      %d: ' % i)
        dump_SVTrackInfo (tt, f)
    return


def dump_MultiSVInfoPlus (info, f):
    dump_BaseTagInfo (info, f)
    fprint (f, info.getNGTrackInJet(),
            info.getNGTrackInSvx(),
            info.getN2T(),
            info.getNormDist(),
            info.numVtxInfo())
    fprint (f, '\n    verts: ')
    for i in range (info.numVtxInfo()):
        fprint (f, '\n    %d: ' % i)
        vx = info.getVtxInfo(i)
        fprint (f, vx.getMass(), vx.getPt(), vx.getEta(), vx.getPhi(),
                vx.getEnergyFraction(), vx.getNormDist(), vx.numTrackInfo())
        fprint (f, '\n      vert: ')
        dump_RecVertex (vx.getRecSvx(), f)
        fprint (f, '\n      tracks: ')
        # Track infos are unsorted.
        # Sort on pt for printing.
        ti = [vx.getTrackInfo(i) for i in range(vx.numTrackInfo())]
        ti.sort (key=lambda a: a.track().pt(), reverse=True)
        i = vx.numTrackInfo()-1 # FIXME: wasn't counting
        for tt in ti:
            fprint (f, '\n      %d: ' % i)
            dump_SVTrackInfo (tt, f)
    return


def dump_JetProbInfoBase (info, f):
    dump_BaseTagInfo (info, f)
    fprint (f, info.nbTracks())
    return


def dump_SETrackInfo (info, f):
    dump_Fourvec (info.electron(), f)
    fprint (f, '%f %f' % (info.d0Value(), info.pTrel()),
           [x for x in info.tagLikelihood()])
    return


def dump_SoftElectronInfo (info, f):
    dump_BaseTagInfo (info, f)
    fprint (f, ' '.join(['%f'%x for x in info.vectorTrackProb()]),
            info.nTrackProb(), info.numTrackInfo())
    for i in range(info.numTrackInfo()):
        fprint (f, '\n        ')
        dump_SETrackInfo (info.getTrackInfo(i), f)
    return


def dump_JetFitterTagInfo (info, f):
    dump_BaseTagInfo (info, f)
    fprint (f, '%d %d %d %f %f %f %f %f' %
            (info.nVTX(),
             info.nSingleTracks(),
             info.nTracksAtVtx(),
             info.energyFraction(),
             info.mass(),
             info.significance3d(),
             info.deltaphi(),
             info.deltaeta()))
    return


def dump_TrackCountingInfo (info, f):
    dump_BaseTagInfo (info, f)
    fprint (f, '%d %f %f %f %f' %
            (info.nTracks(),
             info.d0sig_2nd(),
             info.d0sig_abs_2nd(),
             info.d0sig_3rd(),
             info.d0sig_abs_3rd()))
    return


def dump_AtlfInfo (info, f):
    dump_BaseTagInfo (info, f)
    fprint (f, info.isBTagged(), info.isTauTagged(), info.isTau1PTagged(),
            info.isTau3PTagged(), info.LightHypoCalFactor(),
            info.TauHypoCalFactor(), info.Tau1P3PHypoCalFactor(),
            info.BHypoCalFactor(),
            info.deltaRMinTo("b"),
            info.deltaRMinTo("c"),
            info.deltaRMinTo("t"))
    return


def dump_SMTrackInfo (info, f):
    dump_ParticleImpl (info.muon(), f)
    fprint (f, info.d0Value(), info.pTrel(), list(info.tagLikelihood()))
    return


def dump_SoftMuonInfo (info, f):
    dump_BaseTagInfo (info, f)
    for i in range (info.numTrackInfo()):
        fprint (f, "      ti ")
        dump_SMTrackInfo (info.getTrackInfo(i), f)
    return


def dump_GbbNNTagInfo (info, f):
    dump_BaseTagInfo (info, f)
    fprint (f, info.nMatchingTracks(), info.trkJetWidth(),
            info.trkJetMaxDeltaR())
    return


def dump_JetConstituent (info, f):
    fprint (f, [info.energyInSample(i) for i in range(8)],
            info.energyInCryostat(), info.wtCryo(), info.jet())
#       const Jet* jet() const; //!< get jet directly without token      
#       double energyInSample(CaloSampling::CaloSample ) const;
#       double energyInCryostat( ) const;
#       double getJetWeight(const Jet* the_jet) const ;
#       double getJetWeight(const JetCollection* theContainer,
# 			  index_type& theIndex) const;
#       double wtCryo () const { return m_wtCryoLocal; }
    return


def dump_TrackConstituents (info, f):
    tr = info.tracks()
    ROOT.SetOwnership (tr, True)
    for t in tr:
        fprint (f, '\n          tr: ')
        dump_Fourvec (t, f)
        fprint (f, info.getTrackWeight(t))
    return


def dump_ElectronConstituent (info, f):
    if info.electron():
        dump_Fourvec (info.electron(), f)
        fprint (f, info.getElectronWeight (info.electron()))
    else:
        fprint (f, None)
    return


def dump_MuonConstituent (info, f):
    if info.muon():
        dump_Fourvec (info.muon(), f)
        fprint (f, info.getMuonWeight (info.muon()))
    else:
        fprint (f, None)
    return

def dump_ParticleJet (j, f):
    dump_ParticleBase (j, f)
    fprint (f, '\n      %s %f %f' %
            (j.jetAuthor(),
             j.weight(),
             j.lhSig()))
    fprint (f, [x for x in j.combinedLikelihood()])
    ti = j.jetTagInfoVector()
    for info in ti:
        fprint (f, '\n      %s: ' % info.infoType())
        if isinstance (info, PyAthena.Analysis.TruthInfo):
            dump_TruthInfo (info, f)
        elif isinstance (info, PyAthena.Analysis.SoftLeptonTruthInfo):
            dump_SoftLeptonTruthInfo (info, f)
        elif isinstance (info, PyAthena.Analysis.SecVtxInfo):
            dump_SecVtxInfo (info, f)
        elif isinstance (info, PyAthena.Analysis.IPInfoPlus):
            dump_IPInfoPlus (info, f)
        elif isinstance (info, PyAthena.Analysis.IPInfoBase):
            dump_IPInfoBase (info, f)
        elif isinstance (info, PyAthena.Analysis.SVInfoBase):
            dump_SVInfoBase (info, f)
        elif isinstance (info, PyAthena.Analysis.JetProbInfoBase):
            dump_JetProbInfoBase (info, f)
        elif isinstance (info, PyAthena.Analysis.SoftElectronInfo):
            dump_SoftElectronInfo (info, f)
        elif isinstance (info, PyAthena.Analysis.JetFitterTagInfo):
            dump_JetFitterTagInfo (info, f)
        elif isinstance (info, PyAthena.Analysis.BaseTagInfo):
            dump_BaseTagInfo (info, f)
        else:
            fprint (f, info)

    for c in j.constituentKeys():
        fprint (f, '\n      %s: ' % c)
        info = j.constituent (c)
        if isinstance (info, PyAthena.Analysis.JetConstituent):
            dump_JetConstituent (info, f)
        elif isinstance (info, PyAthena.Analysis.TrackConstituents):
            dump_TrackConstituents (info, f)
        elif isinstance (info, PyAthena.Analysis.ElectronConstituent):
            dump_ElectronConstituent (info, f)
        elif isinstance (info, PyAthena.Analysis.MuonConstituent):
            dump_MuonConstituent (info, f)
        else:
            fprint (f, info)
    return


def dump_TruthParticle (p, f):
    dump_ParticleImpl (p, f)
    poltheta = fix_neg0 (p.polarization().theta())
    fprint (f, ' %3d %3d %f %f %d %d' %
            (p.barcode(),
             p.status(),
             poltheta,
             p.polarization().phi(),
             p.nParents(),
             p.nDecay()))
    fprint (f, '\n        ', p.hasEtIsol())
    dump_HLV (p.genParticle().momentum(), f)
    if p.nParents()>0:
        fprint (f, '\n        p0:')
        # Note: ordering of entries in the parent/child lists is unpredictable.
        # Here, we'll pick the one with the smallest barcode.
        mother = None
        for i in range(p.nParents()):
            if not mother or p.genMother(i).barcode() < mother.barcode():
                mother = p.genMother(i)
                mni = i
        dump_HLV (mother.momentum(), f)
        dump_Fourvec (p.mother(mni), f)
    if p.nDecay()>0:
        fprint (f, '\n        c0:')
        child = None
        for i in range(p.nDecay()):
            if not child or p.genChild(i).barcode() < child.barcode():
                child = p.genChild(i)
                mni = i
        dump_HLV (child.momentum(), f)
        dump_Fourvec (p.child(mni), f)
    return


def dump_TrackParticleTruth (p, f):
    fprint (f, '%4d %f' % (p.barcode(), p.probability()))
    gp = p.particleLink().cptr()
    if gp:
        dump_HLV (gp.momentum(), f)
    else:
        fprint (f, ' (no part)')
    return


@nolist
def dump_TrackParticleTruthCollection (c, f):
    fprintln (f, ' ', c.trackParticleContainerLink().isValid())
    elclass = PyAthena.ElementLink (PyAthena.Rec.TrackParticleContainer)
    for i in range (c.size()):
        fprint (f, '  ')
        el = elclass()
        cont = PyAthena.Rec.TrackParticleContainer()
        el.toIndexedElement (cont, i)
        dump_TrackParticleTruth (c[PyAthena.Rec.TrackParticleTruthKey(el)], f)
        fwrite (f, '\n')
    return


def dump_SubDetHitStatistics (s, f):
    fprint (f, [s[i].value() for i in range(s.NUM_SUBDETECTORS)])
    return


def dump_TruthTrajectory (t, f):
    fprint (f, [l.barcode() for l in t])
    return


@nolist
def dump_DetailedTrackTruthCollection (c, f):
    fprint (f, ' ', c.trackCollectionLink().key(), c.size())
    for p in toiter1(c):
        fprint (f, '\n   %3d' % p.first.index(), ' common ')
        dump_SubDetHitStatistics (p.second.statsCommon(), f),
        fprint (f, '\n      ', ' track ')
        dump_SubDetHitStatistics (p.second.statsTrack(), f),
        fprint (f, '\n      ', ' truth ')
        dump_SubDetHitStatistics (p.second.statsTruth(), f),
        fprint (f, '\n      ', ' trajectory ')
        dump_TruthTrajectory (p.second.trajectory(), f),
    return
              
        
def dump_FitQuality (info, f):
    if not info:
        fprint (f, '(null)')
        return
    fprint (f, '%f %f' % (info.chiSquared(), info.doubleNumberDoF()))
    return


def dump_TrackSummary (info, f):
    if not info:
        fprint (f, '(null)')
        return
    fprint (f, [info.get(i) for i in range(32)],
            [info.isHit(i) for i in range(21)])
    return


def dump_Surface (info, f):
    fprint (f, typename(info.__class__) + ':')
    dump_Threevec (info.center(), f)
    dump_Threevec (info.normal(), f)
    if (isinstance (info, PyAthena.Trk.DiscSurface) and
        typename(info.bounds().__class__).find ('NoBounds') >= 0):
        bd_class = info.bounds().__class__
        fprint (f, '(no bounds)')
    elif (isinstance (info, PyAthena.Trk.CylinderSurface) and
          (not info.hasBounds() or not info.bounds())):
        fprint (f, '(no bounds)')
        bd_class = PyAthena.Trk.CylinderBounds
    else:
        dump_Threevec (info.globalReferencePoint(), f)
        bd_class = info.bounds().__class__
    if isinstance (info, PyAthena.Trk.CylinderSurface):
        dump_AmgVector (info.rotSymmetryAxis(), f)
    fprint (f, '\n          tf')
    dump_AmgMatrix (info.transform().matrix(), f, thresh=1e-8)
#    fprint (f, '\n          de', info.associatedDetectorElement())
    fprint (f, '\n          ly', tonone (info.associatedLayer()))
    fprint (f, '\n          bd', typename(bd_class))
    fprint (f, '\n          id',
            info.associatedDetectorElementIdentifier().getString())
    return

def dump_PerigeeSurface (info, f):
    dump_Surface (info, f)
    return


def dump_PlaneSurface (info, f):
    dump_Surface (info, f)
    return


def dump_StraightLineSurface (info, f):
    dump_Surface (info, f)
    return


def dump_CylinderSurface (info, f):
    dump_Surface (info, f)
    return


def dump_DiscSurface (info, f):
    dump_Surface (info, f)
    return


def dump_LineSaggingDescriptor (info, f):
    os = ROOT.ostringstream()
    info.dump (os)
    s = os.str().replace ('\n', ';')
    fprint (f, s)
    return


def dump_SaggedLineSurface (info, f):
    dump_StraightLineSurface (info, f)
    fprint (f, '\n          ds')
    dump_LineSaggingDescriptor (info.distortionDescriptor())
    return


def dump_surface (p, f):
    if not p:
        fprint (f, None)
    elif isinstance (p, PyAthena.Trk.PerigeeSurface):
        dump_PerigeeSurface (p, f)
    elif isinstance (p, PyAthena.Trk.PlaneSurface):
        dump_PlaneSurface (p, f)
    elif isinstance (p, PyAthena.Trk.StraightLineSurface):
        dump_StraightLineSurface (p, f)
    elif isinstance (p, PyAthena.Trk.CylinderSurface):
        dump_CylinderSurface (p, f)
    elif isinstance (p, PyAthena.Trk.DiscSurface):
        dump_DiscSurface (p, f)
    elif isinstance (p, PyAthena.Trk.SaggedLineSurface):
        dump_SaggedLineSurface (p, f)
    else:
        fprint (f, p)
    return


def dump_associatedSurface (p, f):
    if hasattr(p, 'hasSurface') and not p.hasSurface():
        fprint (f, None)
    else:
        dump_surface (p.associatedSurface(), f)
    return


def dump_ParametersBase (info, f):
    dump_AmgVector (info.parameters(), f)
    dump_Threevec (info.momentum(), f)
    dump_Threevec (info.position(), f)
    dump_Twovec (info.localPosition(), f)
    fprint (f, "%f" % (info.charge(),))
    if info.covariance():
        fprint (f, '\n          cov')
        dump_AmgMatrix (info.covariance(), f)
    fprint (f, '\n          sf')
    dump_associatedSurface (info, f)
    return


def dump_CurvilinearParameters (info, f):
    dump_ParametersBase (info, f)
    fprint (f, '\n          curvilinear')
    fprint (f, info.cIdentifier())
    mat = info.measurementFrame()
    dump_AmgVector (mat.col(0), f)
    dump_AmgVector (mat.col(1), f)
    dump_AmgVector (mat.col(2), f)
    return


def dump_parameters (p, f):
    if not p:
        fprint (f, '(nil)')
        return
    if typename(p.__class__).startswith ('DataModel_detail::ElementProxy<'):
        p = p.__follow__()
    if not p:
        fprint (f, '(nil)')
    elif (typename(p.__class__).startswith ('Trk::ParametersT<') or
        typename(p.__class__).startswith ('Trk::ParametersBase<')):
        dump_ParametersBase (p, f)
    elif typename(p.__class__).startswith ('Trk::CurvilinearParametersT<'):
        dump_CurvilinearParameters (p, f)
    else:
        fprint (f, p)
    return


def dump_TrackParticle (p, f):
    dump_Fourvec (p, f)
    dp = None
    if p.trackParameters().size() > 0 and p.trackParameters()[-1]:
        dp = p.definingParameters()
    if dp:
        fprint (f, "%f" % p.charge())
    else:
        fprint (f, "(nil)")
    fprint (f, "%1d" % p.particleOriginType())
    # This crashes --- needs dd.
    #if p.originalTrack():
    #    fprint (f, '\n        tr', p.originalTrack())
    fprint (f, '\n        vx')
    vx = p.reconstructedVertex()
    if vx:
        dump_Threevec (vx.recVertex().position(), f)
    else:
        fprint (f, tonone (vx))
    if p.fitQuality():
        fprint (f, '\n        fq')
        dump_FitQuality (p.fitQuality(), f)
    if p.trackSummary():
        fprint (f, '\n        ts')
        dump_TrackSummary (p.trackSummary(), f)
    fprint (f, '\n        df')
    dump_parameters (dp, f)
    fprint (f, '\n        pm')
    for x in p.trackParameters():
        fprintln (f, '\n')
        dump_parameters (x, f)
    return


def dump_LocalParameters (p, f):
    dump_AmgVector (p, f)
    fprint (f, p.parameterKey())
    return


def dump_MeasurementBase (p, f):
    dump_LocalParameters (p.localParameters(), f)
    dump_AmgMatrix (p.localCovariance(), f)
    return


def dump_RIO_OnTrack (p, f):
    dump_MeasurementBase (p, f)
    fprint (f, p.identify().getString())
    return


def dump_SiClusterOnTrack (p, f):
    dump_RIO_OnTrack (p, f)
    fprint (f, p.idDE().value(), p.isBroadCluster())
    dump_AmgVector (p.globalPosition(), f)
    return


def dump_PixelClusterOnTrack (p, f):
    dump_SiClusterOnTrack (p, f)
    dump_EL (p.prepRawDataLink(), f)
    fprint (f, p.hasClusterAmbiguity(), p.isFake(), p.energyLoss())
    fprint (f, p.detectorElement().identifyHash().value())
    return


def dump_SCT_ClusterOnTrack (p, f):
    dump_SiClusterOnTrack (p, f)
    dump_EL (p.prepRawDataLink(), f)
    fprint (f, p.positionAlongStrip())
    fprint (f, p.detectorElement().identifyHash().value())
    return


def dump_TRT_DriftCircleOnTrack (p, f):
    dump_RIO_OnTrack (p, f)
    dump_AmgVector (p.globalPosition(), f)
    fprint (f, p.idDE().value())
    dump_EL (p.prepRawDataLink(), f)
    fprint (f, p.status(), p.localAngle(), p.positionAlongWire())
    fprint (f, p.highLevel(), p.timeOverThreshold())
    fprint (f, p.detectorElement().identifyHash().value())
    return


def dump_MdtDriftCircleOnTrack (p, f):
    dump_RIO_OnTrack (p, f)
    dump_AmgVector (p.globalPosition(), f, thresh=1e-8)
    dump_EL (p.prepRawDataLink(), f)
    fprint (f, p.status(), p.localAngle(), p.positionAlongWire())
    fprint (f, p.driftTime(), p.errorStrategy().getBits().to_string())
    fprint (f, p.detectorElement().identifyHash().value())
    dump_StraightLineSurface (p.associatedSurface(), f)
    return
    

def dump_CompetingRIOsOnTrack (p, f):
    dump_MeasurementBase (p, f)
    fprint (f, p.indexOfMaxAssignProb())
    sz = p.numberOfContainedROTs()
    fprint (f, [p.assignmentProbability(i) for i in range(sz)])
    return


def dump_MuonClusterOnTrack (p, f):
    dump_RIO_OnTrack (p, f)
    dump_AmgVector (p.globalPosition(), f)
    fprint (f, p.positionAlongStrip())
    return
    


def dump_CompetingMuonClustersOnTrack (p, f):
    dump_CompetingRIOsOnTrack (p, f)
    dump_AmgVector (p.globalPosition(), f)
    dump_associatedSurface (p, f)
    for r in p.containedROTs():
        fprint (f, '\n    mc ')
        dump_MuonClusterOnTrack (r, f)
    return


def dump_RpcClusterOnTrack (p, f):
    dump_MuonClusterOnTrack (p, f)
    dump_EL (p.prepRawDataLink(), f)
    fprint (f, p.time())
    fprint (f, p.detectorElement().identifyHash().value())
    return
    

def dump_TgcClusterOnTrack (p, f):
    dump_MuonClusterOnTrack (p, f)
    dump_EL (p.prepRawDataLink(), f)
    fprint (f, p.detectorElement().identifyHash().value())
    return
    

def dump_CscClusterOnTrack (p, f):
    dump_MuonClusterOnTrack (p, f)
    dump_EL (p.prepRawDataLink(), f)
    fprint (f, p.status(), p.timeStatus(), p.time())
    if p.detectorElement():
        fprint (f, p.detectorElement().identifyHash().value())
    else:
        fprint (f, '(null detEl)')
    return


def dump_PseudoMeasurementOnTrack (p, f):
    dump_MeasurementBase (p, f)
    dump_AmgVector (p.globalPosition(), f)
    dump_associatedSurface (p, f)
    return
    

def dump_measurement (p, f):
    if not p:
        fprint (f, '(null)')
        return
    nm = typename(p.__class__)
    fprint (f, nm + ': ')
    if nm == 'InDet::PixelClusterOnTrack':
        dump_PixelClusterOnTrack (p, f)
    elif nm == 'InDet::SCT_ClusterOnTrack':
        dump_SCT_ClusterOnTrack (p, f)
    elif nm == 'InDet::TRT_DriftCircleOnTrack':
        dump_TRT_DriftCircleOnTrack (p, f)
    elif nm == 'Muon::MdtDriftCircleOnTrack':
        dump_MdtDriftCircleOnTrack (p, f)
    elif nm == 'Muon::MdtDriftCircleOnTrack':
        dump_MdtDriftCircleOnTrack (p, f)
    elif nm == 'Muon::CompetingMuonClustersOnTrack':
        dump_CompetingMuonClustersOnTrack (p, f)
    elif nm == 'Muon::RpcClusterOnTrack':
        dump_RpcClusterOnTrack (p, f)
    elif nm == 'Muon::TgcClusterOnTrack':
        dump_TgcClusterOnTrack (p, f)
    elif nm == 'Muon::CscClusterOnTrack':
        dump_CscClusterOnTrack (p, f)
    elif nm == 'Trk::PseudoMeasurementOnTrack':
        dump_PseudoMeasurementOnTrack (p, f)
    else:
        fprint (f, p)
    return


def dump_MaterialEffectsBase (p, f):
    fprint (f, p.dumpType(), p.thicknessInX0())
    dump_associatedSurface (p, f)
    return


def dump_ScatteringAngles (p, f):
    if not p:
        fprint (f, None)
        return
    fprint (f, daz(p.deltaPhi()), daz(p.deltaTheta()))
    fprint (f, p.sigmaDeltaPhi(), p.sigmaDeltaTheta())
    return


def dump_MaterialEffectsOnTrack (p, f):
    dump_MaterialEffectsBase (p, f)
    dump_ScatteringAngles (p.scatteringAngles(), f)
    dump_EnergyLoss (p.energyLoss(), f)
    return


def dump_materialeffects (p, f):
    if not p:
        fprint (f, '(null)')
        return
    nm = typename(p.__class__)
    fprint (f, nm + ': ')
    if nm == 'Trk::MaterialEffectsOnTrack':
        dump_MaterialEffectsOnTrack (p, f)
    return


def dump_AlignmentEffectsOnTrack (p, f):
    if not  p:
        fprint (f, '(null)')
        return
    fprint (f, p.deltaTranslation(), p.sigmaDeltaTrranslation(), p.deltaAngle(), p.sigmaDeltaAngle())
    for t in p.vectorOfAffectedTSOS():
        fprint (f, '\n     ts ')
        dump_parameters (t.trackparameters(), f)
    fprint (f, '\n     sf ')
    dump_Surface (p.associatedSurface(), f)
    return


def dump_TrackStateOnSurface (p, f):
    dump_FitQuality (p.fitQualityOnSurface(), f), p.types().to_string(),
    fprint (f, '\n    pm ')
    dump_parameters (p.trackParameters(), f)
    fprint (f, '\n    ms ')
    dump_measurement (p.measurementOnTrack(), f)
    fprint (f, '\n    me ')
    dump_materialeffects (p.materialEffectsOnTrack(), f)
    fprint (f, '\n    ae ')
    dump_AlignmentEffectsOnTrack (p.alignmentEffectsOnTrack(), f)
    return


def dump_TrackInfo (p, f):
    fprint (f, p.trackFitter(), p.particleHypothesis())
    fprint (f, p.properties().to_string())
    fprint (f, p.patternRecognition().to_string())
    return
   

def dump_Track (p, f):
    fprint (f, '\n        pm')
    pm = p.trackParameters()
    for i in range(len(pm)):
        fprint (f, '\n  ')
        dump_parameters (pm[i], f)
    fprint (f, '\n        ms')
    for x in p.measurementsOnTrack():
        fprint (f, '\n  ')
        dump_measurement (x, f)
    fprint (f, '\n        ol')
    for x in p.outliersOnTrack():
        fprint (f, '\n')
        dump_measurement (x, f)
    fprint (f, '\n        ts')
    for x in p.trackStateOnSurfaces():
        fprint (f, '\n  ')
        dump_TrackStateOnSurface (x, f)
    fprint (f, '\n        pp ')
    dump_parameters (p.perigeeParameters(), f)
    fprint (f, '\n        fq ')
    dump_FitQuality (p.fitQuality(), f)
    fprint (f, '\n        tm ')
    dump_TrackSummary (p.trackSummary(), f)
    fprint (f, '\n        ti ')
    dump_TrackInfo (p.info(), f)
    return


def dump_Segment (p, f):
    dump_MeasurementBase (p, f)
    fprint (f, p.author())
    dump_FitQuality (p.fitQuality(), f)
    for x in p.containedMeasurements():
        dump_measurement (x, f)
    return


@nolist
def dump_MissingET (m, f):
    fprint (f, '%d %f %f %f %f %f' %
            (m.getSource(),
             m.etx(),
             m.ety(),
             m.sumet(),
             m.et(),
             m.phi()))
    r = m.getRegions()
    if r:
        for i in range(3):
            fprint (f, '\n   %d %f %f %f' %
                    (i,
                     r.exReg(i),
                     r.eyReg(i),
                     r.etSumReg(i)))
    fwrite (f, '\n')
    return


@nolist
def dump_MissingETSig (m, f):
    fprint (f, "%f %f" %
            (m.metx(),
             m.mety()))
    fprint (f, "\n  %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f" %
            (m.pL()     ,
             m.sigL()   ,
             m.sL()     ,
             m.d0varL() ,
             m.pT()     ,
             m.sigT()   ,
             m.sT()     ,
             m.d0varT() ,
             m.pA()     ,
             m.sigA()   ,
             m.sA()     ,
             m.d0varA() ,
             m.pB()     ,
             m.sigB()   ,
             m.sB()     ,
             m.d0varB()))
    fprint (f, "\n  %f %f %f %f %f %f %f %f" %
            (m.phiA(),
             m.phiAall() ,
             m.tA()      ,
             m.tAall()   ,
             m.phiB()    ,
             m.phiBall() ,
             m.tB()      ,
             m.tBall()))
    fprint (f, "\n  %f %f %f %f" %
            (m.met(),
             m.phi(),
             m.rho(),
             m.oblat()))
    return


def dump_MissingETSigHypo (h, f):
    if not h:
        fprint (f, h)
        return
    fprint (f, '\n    ')
    dump_Fourvec (h, f)
    fprint (f, "%d %d %d %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d %d %d" %
            (h.isLinkValid(),
             h.useLinkForNav(),
             h.type(),
             h.getType().type(),
             h.eRaw(),
             h.etaRaw(),
             h.phiRaw(),
             h.mRaw(),
             h.e(),
             h.eta(),
             h.phi(),
             h.m(),
             h.exMiss(),
             h.eyMiss(),
             h.etMiss(),
             h.phiMiss(),
             h.ex(),
             h.ey(),
             h.ez(),
             h.scale(),
             h.getConeSize(),
             h.sumEt(),
             h.getReg(),
             h.getReg(0.5),
             h.isEmpty(),
             ))
    if h.getObject():
        fprint (f, '\n     ')
        dump_Fourvec (h.getObject(), f),
    for r in range(3):
        fprint (f, '\n     %d %f %f %f %f %f' %
                (r,
                 h.sumEt(r),
                 h.exMiss(r),
                 h.eyMiss(r),
                 h.etMiss(r),
                 h.phiMiss(r)))
    fprint (f, '\n     ',  [x for x in toiter1(h)])
    return


def dump_MissingETSigObject (m, f):
    dump_Fourvec (m, f)
    fprint (f, " %d %d %d %d %d %d %d %d %d %f %f %f %f %f %f %f %f %f %f %f " %
            (m.isValidDefault(),
             m.isValidCurrent(),
             m.isValid(),
             m.isDefault(),
             m.getType().type(),
             m.getDefaultType().type(),
             m.defaultType(),
             m.type(0),
             m.getType(0).type(),
             m.eRaw(),
             m.etaRaw(),
             m.phiRaw(),
             m.mRaw(),
             m.e(),
             m.eta(),
             m.phi(),
             m.m(),
             m.ex(),
             m.ey(),
             m.ez()))
    fprint (f, '\n  %f %f %f %f %f %f %f %d %d %d' %
            (m.exMiss(),
             m.eyMiss(),
             m.etMiss(),
             m.phiMiss(),
             m.scale(),
             m.getConeSize(),
             m.sumEt(),
             m.numHypos(),
             m.idxDefault(),
             m.idxCurrent()))
    if m.isValid():
        fprint (f, '\n  ', [i for i in m.types()])
    for r in range (3):
        fprint (f, '\n  %d %f %f %f %f %f' %
                (r,
                 m.sumEt(r),
                 m.exMiss(r),
                 m.eyMiss(r),
                 m.etMiss(r),
                 m.phiMiss(r)))
    fprint (f, '\n  ', 
            [x for x in toiter1 (m.getExcess())],
            [x for x in toiter1 (m.getDeficit())])
    dump_MissingETSigHypo (m.getHypo(), f)
    dump_MissingETSigHypo (m.getDefaultHypo(), f)
    dump_MissingETSigHypo (m.getHypo(0), f)
    return


@nolist
def dump_MissingEtCalo (m, f):
    dump_MissingET (m, f)
    fprintln (f, '   ', m.calibType())
    for i in range(7):
        fprintln (f, '   %d %d %f %f %f' %
                  (i,
                   m.ncellCalo(i),
                   m.exCalo(i),
                   m.eyCalo(i),
                   m.etSumCalo(i)))
    return


@nolist
def dump_MissingEtTruth (m, f):
    dump_MissingET (m, f)
    for i in range(6):
        fprintln (f, '   %d %f %f %f' %
                  (i,
                   m.exTruth(i),
                   m.eyTruth(i),
                   m.etSumTruth(i)))
    return


def dump_CaloCluster (c, f):
    # ??? cells, getNumberOfCells
    dump_Fourvec (c, f)
    fprint (f, "%3d %f %f %f %f %f %d %d %d" %
            (c.getClusterSize(),
             c.getBasicEnergy(),
             c.getTime(),
             c.eta0(),
             c.phi0(),
             c.energy(),
             c.inBarrel(),
             c.inEndcap(),
             c.allLocked()))
    fprint (f, c.getRecoStatus().getStatusWord())
    beg = c.beginMoment()
    end = c.endMoment()
    while beg != end:
        fprint (f, '\n      mom %d %f' % (c.getMomentType(beg),
                                          c.getMomentValue(beg)))
        beg.__preinc__()
        
    for i in range (c.nSamples()):
        fprint (f, '\n      %2d %d %d %d %f %f %f %f %f %f %f %f %f %f %f' %
                (i,
                 c.is_valid_sampling(i),
                 c.isEMSampling(i),
                 c.hasSampling(i),
                 c.eSample(i),
                 c.etaSample(i),
                 c.phiSample(i),
                 c.etasize(i),
                 c.phisize(i),
                 c.energy_max(i),
                 c.etamax(i),
                 c.phimax(i),
                 c.etaBE(i),
                 c.phiBE(i),
                 c.energyBE(i)))
    return


@nolist_nmax
def dump_CaloClusters_sorted (l, f, nmax = None):
    ll = [x for x in l]
    ll.sort (key=lambda a: a.pt(), reverse=True)
    dump_list (ll, f, dump_CaloCluster, nmax=nmax)
    return


def dump_CaloCellLink (l, f):
    beg = l.begin()
    end = l.end()
    last = None
    while beg != end:
        el = beg.getElement()
        if last != el.dataID():
            last = el.dataID()
            fprint (f, last)
        fprint (f, el.index())
        beg.next()
    return


def dump_CaloClusterCellLink (l, f):
    beg = l.begin()
    end = l.end()
    while beg != end:
        fprint (f, '%d/%f' % (beg.index(), beg.weight()))
        beg.__preinc__()
    return


def dump_CaloCell (l, f):
    fprint (f, l.ID().getString())
    if l.__class__ == PyAthena.TileCell:
        fprint (f, '%.2f %.2f %d %d %d ' % (l.ene1(), l.time1(), l.qual1(), l.qbit1(), l.gain1()))
        fprint (f, '%.2f %.2f %d %d %d ' % (l.ene2(), l.time2(), l.qual2(), l.qbit2(), l.gain2()))
    else:
        fprint (f, '%.2f %.2f %d %d %d ' % (l.energy(), l.time(), l.quality(), l.provenance(), l.gain()))
    return


def dump_Vertex (v, f):
    dump_Threevec (v.position(), f)
    return


def dump_RecVertex (v, f):
    dump_Vertex (v, f)
    dump_FitQuality (v.fitQuality(), f)
    dump_AmgMatrix (v.covariancePosition(), f) # eigen
    return


def dump_ITrackLink (l, f):
    perigee = None
    trk = l.cptr()
    if trk:
        pm = trk.trackParameters()
        if pm and len(pm) > 0:
            perigee = pm[-1]
    dump_parameters (perigee, f)
    return


def dump_LinkToTrack (l, f):
    dump_ITrackLink (l, f)
    fprint (f, '\n             ', l.isValid(), l.index(), l.dataID())
    return


def dump_LinkToTrackParticleBase (l, f):
    dump_ITrackLink (l, f)
    fprint (f, l.isValid(), l.index(), l.dataID())
    return


def dump_VxTrackAtVertex (t, f):
    dump_FitQuality (t.trackQuality(), f),
    fprint (f, '%f %f' %
            (t.vtxCompatibility(),
             t.weight()))
    fprint (f, tonone(t.linState()), tonone(t.ImpactPoint3dAtaPlane()))
    if t.perigeeAtVertex():
        fprint (f, '\n      pv')
        dump_parameters (t.perigeeAtVertex(), f)

    # Methods below will try to dereference the link given by
    # trackOrParticleLink, which is an EL to either a Trk::Track
    # or a TrackParticle.  TrackParticle we can handle, but we
    # shouldn't try to dereference an EL to a Track, since ARA
    # can't handle that.
    tel = PyAthena.ElementLink ('DataVector<Trk::Track>')
    if not isinstance (t.trackOrParticleLink(), tel):
        fprint (f, '\n      ip')
        perigee = None
        trk = t.trackOrParticleLink().cptr()
        if trk:
            pm = trk.trackParameters()
            if pm and len(pm) > 0:
                perigee = pm[-1]
        dump_parameters (perigee, f)
        fprint (f, '\n      pl')
        if isinstance (t.trackOrParticleLink(), PyAthena.Trk.LinkToTrack):
            dump_LinkToTrack (t.trackOrParticleLink(), f)
        elif isinstance (t.trackOrParticleLink(),PyAthena.Trk.LinkToTrackParticleBase):
            dump_LinkToTrackParticleBase (t.trackOrParticleLink(), f)
        else:
            fprint (f, t.trackOrParticleLink())
    return


def dump_MVFVxTrackAtVertex (t, f):
    dump_VxTrackAtVertex (t, f)
    fprint (f, '\n      ', tonone(t.linkToVertices()))
    return

    
def dump_VxCandidate1 (v, f):
    dump_RecVertex (v.recVertex(), f)
    for t in v.vxTrackAtVertex():
        fprint (f, '\n    ', typename(t.__class__))
        if isinstance (t, PyAthena.Trk.MVFVxTrackAtVertex):
            dump_MVFVxTrackAtVertex (t, f)
        elif t.__class__ == PyAthena.Trk.VxTrackAtVertex:
            dump_VxTrackAtVertex (t, f)
        else:
            fprint (f, tonone(t))
    return


def dump_MvfFitInfo (v, f):
    fprint (f, tonone(v.constraintVertex()),
            tonone(v.seedVertex()),
            tonone(v.linearizationVertex()))
    return


def dump_MVFVxCandidate (v, f):
    dump_VxCandidate1 (v, f)
    fprint (f, '\n      ', v.isInitialized(), 'fi')
    dump_MvfFitInfo (v.vertexFitInfo(), f)
    return


def dump_VxCandidate (v, f):
    fprint (f, typename(v.__class__))
    if isinstance (v, PyAthena.Trk.MVFVxCandidate):
        dump_MVFVxCandidate (v, f)
    elif (v.__class__ == PyAthena.Trk.VxCandidate or
          isinstance (v, PyAthena.Trk.V0Hypothesis)):
        dump_VxCandidate1 (v, f)
    elif isinstance (v, PyAthena.Trk.ExtendedVxCandidate):
          dump_ExtendedVxCandidate (v, f)
    else:
        fprint (f, tonone(v))
        dump_VxCandidate1 (v, f)
    return


def dump_EventID (e, f):
    fprint (f, '%9d %9d %9d %9d %9d %9d' %
            (e.run_number(),
             e.event_number(),
             e.time_stamp(),
             e.time_stamp_ns_offset(),
             e.lumi_block(),
             e.bunch_crossing_id()))
    return


def dump_EventType (e, f):
    fprint (f, e.typeToString(), e.user_type(), e.mc_event_weight())
    return


def dump_TriggerInfo (e, f):
    fprint (f, e.statusElement(), e.extendedLevel1ID(), e.level1TriggerType())
    fprint (f, '\n    l2:', formatItemUsingLong ([x for x in e.level2TriggerInfo()]))
    fprint (f, '\n    ef:', formatItemUsingLong ([x for x in e.eventFilterInfo()]))
    fprint (f, '\n    st:', [(x.name(), x.type(), x.obeysLumiblock()) for x in e.streamTags()])
    return


@nolist
def dump_EventInfo (e, f):
    fprint (f, e.clID())
    dump_EventID (e.event_ID(), f)
    fprint (f, '\n   ')
    fprint (f, e.actualInteractionsPerCrossing(), e.averageInteractionsPerCrossing())
    for i in range(ROOT.EventInfo.nDets):
        fprint (f, '\n     det', i, e.eventFlags(i), e.errorState(i))
    fprint (f, '\n   ')
    dump_EventType (e.event_type(), f)
    fprint (f, '\n   tg')
    dump_TriggerInfo (e.trigger_info(), f)
    return


@nolist
def dump_PileUpEventInfo (e, f):
    dump_EventInfo (e, f)
    for (i,s) in enumerate (toiter (e.beginSubEvt(), e.endSubEvt())):
        fprint (f, '\n   subevt', i, s.time(), s.index(), s.BCID(), s.type())
    return


@nolist
def dump_EventStreamInfo (e, f):
    fprint (f, 'nevents: ', e.getNumberOfEvents())
    fprint (f, '\nrun numbers: ', formatItemUsingLong(list(e.getRunNumbers())))
    fprint (f, '\nlb numbers: ', formatItemUsingLong(list(e.getLumiBlockNumbers())))
    fprint (f, '\nproc tags: ', list(e.getProcessingTags()))
    fprint (f, '\nitem list: ', formatItemUsingLong([(p.first, p.second) for p in e.getItemList()]))
    for typ in list(e.getEventTypes()):
        fprint (f, '\n  ')
        dump_EventType (typ, f)
    return


def barcodes (beg, end, sz, f):
    #out = [x.barcode() for x in toiter (beg, end)]

    # Work around pyroot iterator comparison breakage.
    out = []
    while beg != end and sz > 0:
        sz -= 1
        x = beg.__deref__()
        beg.__preinc__()
        out.append (x.barcode())

    out.sort()
    for i in out:
        fprint (f, i)
    return
def dump_GenVertex (v, f):
    fprint (f, "%d %d %d %d %d" %
            (v.id(),
             v.barcode(),
             v.parent_event().event_number(),
             v.particles_in_size(),
             v.particles_out_size()))
    dump_Threevec (v.point3d(), f)
    dump_HLV (v.position(), f)
    if v.weights().size() == 0:
        ww = []
    else:
        ww = list(v.weights())
    fprintln (f, [w for w in ww])
    fprint (f, '(')
    barcodes(v.particles_in_const_begin(),
             v.particles_in_const_end(),
             v.particles_in_size(), f)
    fprint (f, ')(')
    barcodes(v.particles_out_const_begin(),
             v.particles_out_const_end(),
             v.particles_out_size(), f)
    fprint (f, ')')

    return


def dump_GenParticle (p, f):
    fprint (f, "%d %d %d %d" %
            (p.barcode(),
             p.pdg_id(),
             p.parent_event().event_number(),
             p.status(),))
    if p.production_vertex():
        fprint (f, p.production_vertex().barcode())
    else:
        fprint (f, None)
    if p.end_vertex():
        fprint (f, p.end_vertex().barcode())
    else:
        fprint (f, None)
    dump_HLV (p.momentum(), f)
    pol = p.polarization()
    poltheta = fix_neg0 (pol.theta())
    fprint (f, "%f %f %f %f" %
            (poltheta, pol.phi(), pol.normal3d().theta(), pol.normal3d().phi()))
    return


def dump_GenEvent (e, f):
    fprint (f, '%d %d %f %f %f %d %d' %
            (e.event_number(),
             e.signal_process_id(),
             e.event_scale(), 
             e.alphaQCD(),
             e.alphaQED(),
             e.particles_size(),
             e.vertices_size()))
    fprint (f, '\n   wt', [w for w in e.weights()], [i for i in e.random_states()])
    fprint (f, '\n   v0')
    if e.signal_process_vertex():
        dump_GenVertex (e.signal_process_vertex(), f)
    else:
        fprint (f, None)
    for v in toiter (e.vertices_begin(), e.vertices_end()):
        fprint (f, '\n   v')
        dump_GenVertex (v, f)

    for p in toiter (e.particles_begin(), e.particles_end()):
        fprint (f, '\n   p')
        if p:
            dump_GenParticle (p, f)
        else:
            fprint (f, None)
    return


@nolist
def dump_CTP_Decision (t, f):
    fprint (f, t.dump())
    return


def dump_Muon_ROI (m, f):
    dump_Fourvec (m, f)
    fprint (f, "%d %s %f %d %d %d %d %d %d %d %d" %
            (m.getROIWord(),
             m.getThrName(),
             m.getThrValue(),
             m.getThrNumber(),
             m.getRoI(),
             m.getSectorAddress(),
             m.isFirstCandidate(),
             m.isMoreCandInRoI(),
             m.isMoreCandInSector(),
             m.getSource(),
             m.getHemisphere()))
    return


def dump_EmTau_ROI (m, f):
    dump_Fourvec (m, f)
    fprint (f, "%d %f %f %f %f %f %f %d" %
            (m.getROIWord(),
             m.getCore(),
             m.getEMClus(),
             m.getTauClus(),
             m.getEMIsol(),
             m.getHadIsol(),
             m.getHadCore(),
             m.getThrPattern()))
    for (n,v) in zip (m.getThresholdNames(), m.getThresholdValues()):
        fprint (f, n, v)
    return


def dump_Jet_ROI (m, f):
    dump_Fourvec (m, f)
    fprint (f, "%d %d %f %f %f" %
            (m.getROIWord(),
             m.getThrPattern(),
             m.getET4x4(),
             m.getET6x6(),
             m.getET8x8()))
    for (n,v) in zip (m.getThresholdNames(), m.getThresholdValues()):
        fprint (f, n, v)
    return


def dump_JetET_ROI (m, f):
    fprint (f, "%d %d" %
            (m.getROIWord(),
             m.getThrPattern(),))
    fprint (f, [t for t in m.getThresholds()])
    return


def dump_EnergySum_ROI (m, f):
    fprint (f, "%d %d %d %f %f %f %f %f %d %d %d %d %d %d" %
            (m.getROIWord0(),
             m.getROIWord1(),
             m.getROIWord2(),
             m.getEnergyX(),
             m.getEnergyY(),
             m.getEnergyT(),
             m.getExMiss(),
             m.getEyMiss(),
             m.getOverflowX(),
             m.getOverflowY(),
             m.getOverflowT(),
             m.getOverflows(),
             m.getThrPatSummET(),
             m.getThrPatMissET()))
    fprint (f, [t for t in m.getThresholds()])
    return


@nolist
def dump_LVL1_ROI (t, f):
    for m in t.getMuonROIs():
        fprint (f, '\nmu')
        dump_Muon_ROI (m, f)
    for m in t.getEmTauROIs():
        fprint (f, '\nem')
        dump_EmTau_ROI (m, f)
    for m in t.getJetROIs():
        fprint (f, '\njt')
        dump_Jet_ROI (m, f)
    for m in t.getJetEtROIs():
        fprint (f, '\nje')
        dump_JetET_ROI (m, f)
    for m in t.getEnergySumROIs():
        fprint (f, '\nes')
        dump_EnergySum_ROI (m, f)
    return


def dump_GenericResult (t, f):
    fprint (f, "%d %d %d" %
            (t.accepted(),
             t.error(),
             t.error_bits(),))
    return


@nolist
def dump_HLTResult (t, f):
    dump_GenericResult (t, f)
    fprint (f, "%d %d %d %d %d %d %d %d %d %d %d %d" %
            (t.getHLTResultClassVersion(),
             t.getLvl1Id(),
             t.isPassThrough(),
             t.getHLTStatus().code,
             t.getLvlConverterStatus().code,
             t.getHLTLevel(),
             t.isValid(),
             t.getNumOfSatisfiedSigs(),
             t.isCreatedOutsideHLT(),
             t.isHLTResultTruncated(),
             t.size(),
             t.isEmpty(),
             ))
    fprint (f, [i for i in t.partSizes()])
    fprint (f, [i for i in t.getNavigationResult()])
    fprint (f, [i for i in t.getChainResult()])
    fprint (f, [i for i in t.getNavigationResultCuts()])
    return


def dump_Lvl1Result (t, f):
    fprint (f, t.isConfigured(),
            t.isAccepted(),
            t.anyActiveL1ItemAfterVeto(),
            t.nItems())
    fprint (f, '\n   ', [i for i in t.itemsPassed()])
    fprint (f, '\n   ', [i for i in t.itemsBeforePrescale()])
    fprint (f, '\n   ', [i for i in t.itemsAfterPrescale()])
    fprint (f, '\n   ', [i for i in t.itemsAfterVeto()])
    return


@nolist
def dump_TrigDecision (t, f):
    fprint (f, t.masterKey())
    fprint (f, '\nl1')
    dump_Lvl1Result (t.getL1Result(), f)
    fprint (f, '\nl2')
    dump_HLTResult (t.getL2Result(), f)
    fprint (f, '\nef')
    dump_HLTResult (t.getEFResult(), f)
    return


def dump_HLTTriggerElement (e, f):
    fprint (f, e.id(), e.name())
    return

    
def dump_HLTSignature (s, f):
    fprint (f, '(', s.signature_counter(),
            s.logic(),
            s.label())
    for e in s.outputTEs():
        dump_HLTTriggerElement (e, f)
    fprint (f, ')')
    return


def dump_HLTStreamTag (s, f):
    fprint (f, '(',
            s.stream(),
            s.type(),
            s.obeyLB(),
            s.prescale(),
            ')')
    return

def dump_HLTChain (c, f):
    fprint (f, c.chain_name(),
            c.chain_version(),
            c.chain_counter(),
            c.level(),
            c.lower_chain_name(),
            c.lower_chain_counter(),
            c.prescale(),
            c.pass_through(),
            c.chain_hash_id(),
            c.lower_chain_hash_id())
    for s in c.signatureList():
        dump_HLTSignature (s, f)
    fprint (f, [t.bit() for t in c.triggerTypeList()])
    for s in c.streamTagList():
        dump_HLTStreamTag (s, f)
    fprint (f, [s for s in c.groupList()])
    return


def dump_HLTSequence (s, f):
    fprint (f, '(')
    dump_HLTTriggerElement (s.outputTE(), f)
    dump_HLTTriggerElement (s.topoStartTE(), f)
    for t in s.inputTEs():
        dump_HLTTriggerElement (t, f)
    fprint (f, [a for a in s.algorithms()], ')')
    return


@nolist
def dump_HLTAODConfigData (d, f):
    fprint (f, "%d" %
            (d.masterKey()))
    for c in d.getChainList().chains():
        fprint (f, '\nch')
        dump_HLTChain (c, f)
    for c in d.getSequenceList().sequences():
        fprint (f, '\nsq')
        dump_HLTSequence (c, f)
    return


def dump_L1DataBaseclass (c, f):
    fprint (f, c.id())
    return


def dump_CTPConfig (c, f):
    dump_L1DataBaseclass (c, f)
    fprint (f, c)
#     Menu* menu() const;
#     void setMenu( Menu* m );   
#     PrescaleSet prescaleSet() const;
#     void setPrescaleSet( PrescaleSet m );  
#     BunchGroupSet bunchGroupSet() const;
#     void setBunchGroupSet( BunchGroupSet m );  
#     PrescaledClock prescaledClock() const;
#     void setPrescaledClock( PrescaledClock m );   
#     TriggerType triggerType() const;
#     void setTriggerType( TriggerType m );   
#     DeadTime deadTime() const;
#     void setDeadTime( DeadTime m );   
#     Random random() const;
#     void setRandom( Random m );   
    
#     int lvl1MasterTableId() const {return m_Lvl1MasterTableId;}
#     void setLvl1MasterTableId (const int& id){m_Lvl1MasterTableId=id;}
    return


@nolist
def dump_Lvl1AODConfigData (d, f):
    fprint (f, d.masterKey())
    fprint (f, '\nit', [s for s in d.getItems()])
    fprint (f, '\nmu', [s for s in d.getMuonThresholds()])
    fprint (f, '\nem', [s for s in d.getEMTauThresholds()])
    fprint (f, '\njt', [s for s in d.getJetThresholds()])
    fprint (f, '\nfj', [s for s in d.getForwardJetThresholds()])
    fprint (f, '\net', [s for s in d.getTotalEtThresholds()])
    fprint (f, '\nje', [s for s in d.getJetEtThresholds()])
    fprint (f, '\nms', [s for s in d.getMissingEtThresholds()])
    fprint (f, '\nctp')
    if d.ctpConfig():
        dump_CTPConfig (d.ctpConfig(), f)
    else:
        fprint (f, None)
    fprint (f, '\ntcf')
    if d.thresholdConfig():
        fprint (f, d.thresholdConfig())
    else:
        fprint (f, None)
    return


@nolist
def dump_Lvl1AODPrescaleConfigData (d, f):
    fprint (f, d.masterKey())
    fprint (f, [i for i in d.getPrescales()])
    return


def dump_TrackRecord (t, f):
    dump_H3V (t.GetPosition(), f)
    dump_H3V (t.GetMomentum(), f)
    fprint (f, t.GetEnergy(),
            t.GetPDGCode(),
            t.GetTime(),
            t.GetBarCode(),
            t.GetVolName())
    return


def dump_DetStatus (s, f):
    fprintln (f, s.code(),
              s.fullCode(),
              s.deadFrac(),
              s.deadThrust(),
              s.colour())
    return


@nolist
def dump_DetStatusMap (m, f):
    for e in m:
        fprint (f, e.first)
        dump_DetStatus (e.second, f)
    return


def dump_TrigInDetTrackTruth (t, f):
    fprint (f, t.nrMatches())
    if t.nrMatches() == 0: return
    fprint (f, t.bestMatch().barcode(),
            t.bestSiMatch().barcode(),
            t.bestTRTMatch().barcode(),
            t.nrMatches(),
            t.nrCommonHitsBestSi(),
            t.nrCommonHitsBestTRT())
    fprint (f, formatItemUsingLong([(forceInt(t.truthMatch(i).barcode()),
                                     t.nrCommonHits(i),
                                     t.nrCommonSiHits(i),
                                     t.nrCommonTRTHits(i))
                                    for i in range(t.nrMatches())]))
    fprint (f, formatItemUsingLong ([(p.first, p.second) for p in t.getFamilyTree()]))
    return


def dump_TrigInDetTrackFitPar (p, f):
    fprint (f, "%f %f %f %f %f %f %f %f %f %f %f %d" %
            (p.a0,
             p.z0,
             p.phi0,
             p.eta,
             p.pT,
             p.ea0,
             p.ez0,
             p.ephi0,
             p.eeta,
             p.epT,
             p.surfaceCoordinate,
             p.surfaceType))
    if p.cov:
        fprint (f, [x for x in p.cov])
    return


def dump_TrigInDetTrack (t, f):
    fprint (f, "%d %f %d %d %d %d" %
            (t.algorithmId,
             t.chi2,
             t.NStrawHits,
             t.NStraw,
             t.NStrawTime,
             t.NTRHits))
    fprint (f, tonone(t.siSpacePoints), tonone(t.trtDriftCircles))
    fprint (f, '\n    par')
    if t.param:
        dump_TrigInDetTrackFitPar (t.param, f)
    else:
        fprint (f, None)
    fprint (f, '\n    end')
    if t.endParam:
        dump_TrigInDetTrackFitPar (t.endParam, f)
    else:
        fprint (f, None)
    return


class PyTrigInDetTrackFitPar:
    def __init__ (self, t):
        self.a0 = t.a0()
        self.z0 = t.z0()
        self.phi0 = t.phi0()
        self.eta = t.eta()
        self.pT = t.pT()
        self.ea0 = t.ea0()
        self.ez0 = t.ez0()
        self.ephi0 = t.ephi0()
        self.eeta = t.eeta()
        self.epT = t.epT()
        self.surfaceCoordinate = t.surfaceCoordinate()
        self.surfaceType = t.surfaceType()
        if t.cov():
            self.cov = [x for x in t.cov()]
        else:
            self.cov = None
        return
    
class PyTrigInDetTrack:
    def __init__ (self, t):
        self.algorithmId = t.algorithmId()
        self.chi2 = t.chi2()
        self.NStrawHits = t.NStrawHits()
        self.NStraw = t.NStraw()
        self.NStrawTime = t.NStrawTime()
        self.NTRHits = t.NTRHits()
        self.siSpacePoints = t.siSpacePoints()
        self.trtDriftCircles = t.trtDriftCircles()
        if t.param():
            self.param = PyTrigInDetTrackFitPar (t.param())
        else:
            self.param = None
        if t.endParam():
            self.endParam = PyTrigInDetTrackFitPar (t.endParam())
        else:
            self.endParam = None
        return


def _tmcmp (x, y):
    xbc = 0
    if x[0].nrMatches() > 0:
        xbc = x[0].bestSiMatch().barcode()
    ybc = 0
    if y[0].nrMatches() > 0:
        ybc = y[0].bestSiMatch().barcode()
    c = xbc - ybc
    if c < 0:
        return -1
    elif c > 0:
        return 1
    return cmp (x[1].chi2, y[1].chi2)


@nolist
def dump_TrigInDetTrackTruthMap (m, f):
    #fprint (f, m.size())
    #fprint (f, m.tracki(0))
    tm = [(m.truthi(i), PyTrigInDetTrack(m.tracki(i)))
          for i in range(m.size()) if m.trackiLink(i).isValid()]
    tm.sort (key = cmp_to_key (_tmcmp))
    for (i, (truth, track)) in enumerate(tm):
        fprint (f, '\n  ', i)
        dump_TrigInDetTrackTruth (truth, f)
        fprint (f, '\n    ')
        dump_TrigInDetTrack (track, f)
    return


# Deal with the possibility that some of the ELs in the target set
# may be invalid.  Doesn't seem to be any way to know that other than
# by trying to do the deref.  (I.e., we can't get a hold of the EL directly.)
def safe_assocs (a, obj, coll, f):
    bv = a.beginAssociation(obj)
    ev = a.endAssociation(obj)
    res = []
    err = False
    while bv != ev:
        try:
            targ = bv.__deref__()
            res.append (targ)
        except RuntimeError as e:
            if e.args[0].find ('dereferencing invalid ElementLink') >= 0:
                err = True
            else:
                raise
        bv.__preinc__()

    if err:
        errflag = True
        for o in res:
            coll.push_back (o)
    else:
        errflag = False
        a.assocs (obj, coll)
    return errflag


@nolist
def dump_Assocs (a, f, colltype):
    bo = a.beginObject()
    eo = a.endObject()
    l = []
    while bo != eo:
        obj = a.getObject(bo)
        coll = colltype(1)
        errflag = safe_assocs (a, obj, coll, f)
        l.append ((obj, coll, errflag))
        bo.next()

    l.sort (key=lambda a: a[0].pt(), reverse=True)

    for obj, coll, errflag in l:
        fprint (f, '\n', typename(obj.__class__))
        dump_Fourvec (obj, f)
        fprint (f, '->')
        for p in coll:
            fprint (f, typename(p.__class__))
            dump_Fourvec (p, f),
        if errflag:
            fprint (f, '  [Got invalid EL error]')
    return


@nolist
def dump_INav4MomAssocs (a, f):
    return dump_Assocs (a, f, PyAthena.DataVector(PyAthena.INavigable4Momentum))


@nolist
def dump_TrackParticleAssocs (a, f):
    return dump_Assocs (a, f, PyAthena.Rec.TrackParticleContainer)


def dump_ElectronMuonTopoInfo (a, f):
    fprint (f, a.RoiWord(), a.DeltaPhi(), a.DeltaR(), a.InvMass(),
            a.ElecValid(), a.OppositeCharge(), a.VertexState())
    return


def dump_MuonSpShower (m, f):
    fprint (f, "%f %f %d %d %d %d %d %d %d" %
            (m.eta(),
             m.phi(),
             m.numberOfTriggerHits(),
             m.numberOfInnerHits(),
             m.numberOfMiddleHits(),
             m.numberOfOuterHits(),
             m.numberOfInnerSegments(),
             m.numberOfMiddleSegments(),
             m.numberOfOuterSegments()))
    return


def dump_JetAssociationBase (a, f):
    fprint (f, a.name())
    return


def dump_ElectronAssociation (a, f):
    dump_JetAssociationBase (a, f)
    ele = a.electron()
    if ele:
        fprint (f, a.getElectronWeight (ele))
        dump_Fourvec (ele, f)
    return


def dump_PhotonAssociation (a, f):
    dump_JetAssociationBase (a, f)
    gam = a.photon()
    if gam:
        fprint (f, a.getPhotonWeight (gam))
        dump_Fourvec (gam, f)
    return


def dump_MuonAssociation (a, f):
    dump_JetAssociationBase (a, f)
    muo = a.muon()
    if muo != muonNull:
        fprint (f, a.getMuonWeight (muo))
        dump_Fourvec (muo, f)
    return


def dump_TrackAssociation (a, f):
    dump_JetAssociationBase (a, f)
    fprint (f, a.nTracks())
    tr = a.tracks()
    ROOT.SetOwnership (tr, True)
    for t in tr:
        fprint (f, a.getTrackWeight (t))
        dump_Fourvec (t, f)
    return


def dump_ISvxAssociation (a, f):
    dump_JetAssociationBase (a, f)
    vi = a.vertexInfo()
    if vi:
        for v in vi.vertices():
            dump_Threevec (v.position(), f)
    return


def _infoType (i):
    return i.infoType()

def dump_Jet (j, f):
    if j.e() == 0 and j.phi() == 0 and j.eta() == 0:
        with signalstate (j, PyAthena.P4SignalState.JETEMSCALE):
            dump_ParticleImpl (j, f)
    else:
        dump_ParticleImpl (j, f)
    fprint (f, '\n      %s %f' %
            (j.jetAuthor(),
             j.getFlavourTagWeight(),))
    # ELs to towers will always be invalid (towers aren't actually saved...)
    tower_constituents_p = False
    if j.firstConstituent() != j.lastConstituent():
        if isinstance (j.getContainer(j.firstConstituent()),
                       PyAthena.CaloTowerContainer):
            tower_constituents_p = True
        elif j.firstConstituent().__deref__():
            ss = asint(j.constituentSignalState())
            j.setConstituentSignalState (PyAthena.P4SignalState.CALIBRATED)
            dump_HLV (j.constituent_sum4Mom(), f)
            j.setConstituentSignalState (ss)
    fprint (f, [x for x in j.combinedLikelihood()])
    fprint (f, '\n      moms ')
    for mk in j.getMomentKeys():
        if mk in ['Timing', 'LArQuality']: continue
        mom = j.getMoment (mk, False)
        if  mom != 0:
            fprint (f, mk, j.getMoment (mk, True))
    if hasattr(j, 'getJetTime'):
        fprint (f, '\n      timing ', j.getJetTime(),
                ' qual ', j.getJetQuality())
    else:
        fprint (f, '\n      timing ', j.getMoment('Timing',True),
                ' qual ', j.getMoment('LArQuality',True))
    fprint (f, '\n      assoc ')
    for ak in j.getAssociationKeys():
        ass = j.getAssociationBase(ak)
        if ass == jetAssocNull : continue
        fprint (f, '\n         ', ak)
        if isinstance (ass, PyAthena.Analysis.ElectronAssociation):
            dump_ElectronAssociation (ass, f)
        elif isinstance (ass, PyAthena.Analysis.PhotonAssociation):
            dump_PhotonAssociation (ass, f)
        elif isinstance (ass, PyAthena.Analysis.MuonAssociation):
            dump_MuonAssociation (ass, f)
        elif isinstance (ass, PyAthena.Analysis.TrackAssociation):
            dump_TrackAssociation (ass, f)
        elif isinstance (ass, PyAthena.Analysis.ISvxAssociation):
            dump_ISvxAssociation (ass, f)
        else:
            fprint (f, ass)
    ti = j.jetTagInfoVector()
    fprint (f, '\n      tag info:')
    if ti:
        #ROOT.SetOwnership (ti, True)
        ti = list(ti)
        ti.sort (key=_infoType)
        for info in ti:
            if not info:
                fprint (f, '\n      (null)')
                continue
            fprint (f, '\n      %s %s: %f: '
                    % (info.infoType(), typename(info.__class__), j.getFlavourTagWeight (info.infoType())))
            if isinstance (info, PyAthena.Analysis.TruthInfo):
                dump_TruthInfo (info, f)
            elif isinstance (info, PyAthena.Analysis.SoftLeptonTruthInfo):
                dump_SoftLeptonTruthInfo (info, f)
            elif isinstance (info, PyAthena.Analysis.SecVtxInfo):
                dump_SecVtxInfo (info, f)
            elif isinstance (info, PyAthena.Analysis.IPInfoPlus):
                dump_IPInfoPlus (info, f)
            elif isinstance (info, PyAthena.Analysis.IPInfoBase):
                dump_IPInfoBase (info, f)
            elif isinstance (info, PyAthena.Analysis.SVInfoBase):
                dump_SVInfoBase (info, f)
            elif isinstance (info, PyAthena.Analysis.SVInfoPlus):
                dump_SVInfoPlus (info, f)
            elif isinstance (info, PyAthena.Analysis.MultiSVInfoPlus):
                dump_MultiSVInfoPlus (info, f)
            elif isinstance (info, PyAthena.Analysis.JetProbInfoBase):
                dump_JetProbInfoBase (info, f)
            elif isinstance (info, PyAthena.Analysis.SoftElectronInfo):
                dump_SoftElectronInfo (info, f)
            elif isinstance (info, PyAthena.Analysis.JetFitterTagInfo):
                dump_JetFitterTagInfo (info, f)
            elif (hasattr (PyAthena.Analysis, 'TrackCountingInfo') and
                  isinstance (info, PyAthena.Analysis.TrackCountingInfo)):
                dump_TrackCountingInfo (info, f)
            elif isinstance (info, PyAthena.Analysis.AtlfInfo):
                dump_AtlfInfo (info, f)
            elif isinstance (info, PyAthena.Analysis.SoftMuonInfo):
                dump_SoftMuonInfo (info, f)
            elif info.__class__ is PyAthena.Analysis.BaseTagInfo:
                dump_BaseTagInfo (info, f)
            elif info.__class__ is PyAthena.Analysis.GbbNNTagInfo:
                dump_GbbNNTagInfo (info, f)
            else:
                fprint (f, info)
    if tower_constituents_p:
        fprint (f, '\n      (not dumping tower constituents)')
    else:
        fprint (f, '\n      constituents:')
        for c in toiter (j.firstConstituent(), j.lastConstituent()):
            fprint (f, '\n        ')
            if not c:
                fprint (f, tonone(c))
            else:
                fprint (f, typename(c.__class__))
                fprint (f, j.getWeight (c))
                dump_Fourvec (c, f)
    return


def dump_ExtendedVxCandidate (c, f):
    dump_VxCandidate1 (c, f)
    fprint (f, '\n          em')
    if c.fullCovariance():
        dump_AmgMatrix (c.fullCovariance(), f)
    else:
        fprint (f, None)
    return


def dump_V0Hypothesis (h, f):
    if h == None:
        fprint (f, None)
        return
    fprint (f, 'V0Hypothesis', h.positiveTrackID(),
            h.negativeTrackID(), h.hypothesisID())
    fprint (f, '\n     ')
    dump_ExtendedVxCandidate (h, f)
    return


def dump_V0Candidate (v, f):
    fprint (f, "V0Candidate", len(v.v0Hypothesis()))
    for h in v.v0Hypothesis():
        fprint (f, '\n     ')
        dump_V0Hypothesis (h, f)
    return


def dump_TrigVertex (v, f):
    dump_Threevec (v, f)
    fprint (f, v.algorithmId(), v.chi2(), v.ndof(), v.mass(),
            v.massVariance(), v.energyFraction(), v.nTwoTracksSecVtx())
    fprint (f, '\n   cov: ', [v.cov()[i] for i in range(6)])
    if v.tracks() and v.tracks().size() > 0:
        for (i, t) in enumerate (v.tracks()):
            fprint (f, '\n   t%d'%i)
            p = t.param()
            fprint (f, p.a0(), p.z0(), p.phi0(), p.eta(), p.pT())
    if v.getMotherTrack():
        fprint (f, '\n   mother: ')
        dump_TrigInDetTrackFitPar (v.getMotherTrack(), f)
    return


def dump_clist (l, f):
    fprint (f, '[')
    last = None
    n = 0
    for i in l:
        if i != last:
            if n > 0:
                if n == 1:
                    fprint (f, '%f, ' % last)
                else:
                    fprint (f, '%d*%f, ' % (n, last))
            last = i
            n = 1
        else:
            n = n + 1
    if n > 0:
        if n == 1:
            fprint (f, '%f, ' % last)
        else:
            fprint (f, '%d*%f, ' % (n, last))
    fprint (f, ']')
    return
            
def dump_TrigTrackCounts (t, f):
    fprint (f, 'z0_pt:')
    dump_TrigHisto2D (t.z0_pt(), f)
    fprint (f, '\neta_phi:')
    dump_TrigHisto2D (t.eta_phi(), f)
    return
@nolist
def dump_TrigTrackCounts_nolist (t, f):
    dump_TrigTrackCounts (t, f)
    return


def dump_TrigTau (t, f):
    dump_Fourvec (t, f)
    fprint (f, t.roiId(), t.Zvtx(), t.err_Zvtx(), t.etCalibCluster(),
            t.simpleEtFlow(), t.nMatchedTracks())
    fprint (f, '\n   ', tonone(t.tauCluster()),
            tonone(t.trackCollection()),
            tonone(t.tracksInfo()))
    return
@nolist
def dump_TrigTau_nolist (t, f):
    dump_TrigTau (t, f)
    return


def dump_TrigCaloCluster (c, f):
    fprint (f, c.rawEnergy(), c.rawEt(), c.rawEta(), c.rawPhi(), c.RoIword(),
            c.nCells(), c.quality())
    fprint (f, '\n    ', [c.rawEnergy(i) for i in range(25)])
    return


def dump_TrigEMCluster (c, f):
    dump_TrigCaloCluster (c, f)
    fprint (f, '\n    ', c.energy(), c.et(), c.eta(), c.phi(),
            c.e237(), c.e277(), c.fracs1(), c.weta2(),
            c.ehad1(), c.Eta1(), c.emaxs1(), c.e2tsts1())
    fprint (f, '\n    ', [c.energyInSample(i) for i in range(25)])
    return
@nolist
def dump_TrigEMCluster_nolist (c, f):
    dump_TrigEMCluster (c, f)
    return


def dump_TrigElectron (p, f):
    dump_Fourvec (p, f)
    fprint (f, p.isValid(), p.roiId())
    fprint (f, '\n   ', p.trackAlgo(), p.trackIndx(), p.charge(),
            p.Zvtx())
    fprint (f, '\n   ', p.err_Pt(), p.err_eta(), p.err_phi(), p.err_Zvtx())
    fprint (f, '\n   ', p.trkClusDeta(), p.trkClusDphi(), p.EtOverPt())
    return


def dump_TrigPhoton (p, f):
    dump_Fourvec (p, f)
    fprint (f, p.isValid(), p.roiId())
    fprint (f, '\n   ', p.Et(), p.HadEt(), p.energyRatio(), p.rCore(),
            p.dPhi(), p.dEta())
    fprint (f, '\n   ')
    if p.cluster():
        dump_TrigEMCluster (p.cluster(), f)
    return


@nolist
def dump_TrigInDetTrackCollection (t, f):
    fprintln (f, ' ROI ', t.RoI_ID())
    t2 = [PyTrigInDetTrack(tt) for tt in t]
    dump_list (t2, f, dump_TrigInDetTrack)
    return


def dump_TrigEFBjet (j, f):
    dump_Fourvec (j, f)
    fprint (f, j.isValid(), j.roiId())
    fprint (f, '\n   ', j.prmVtx(), j.xComb(), j.xIP1D(), j.xIP2D(),
            j.xIP3D(), j.xCHI2(), j.xSV(), j.xMVtx(), j.xEVtx(), j.xNVtx())
    fprint (f, '\n   ', tonone(j.TrackCollection()),
            tonone(j.PrmVertexCollection()),
            tonone(j.SecVertexCollection()))
    return


def dump_TrigEFBphys (j, f):
    fprint (f, j.roiId(), j.particleType(), j.eta(), j.phi(),
            j.mass(), j.fitmass(), j.fitchi2(), j.fitndof(),
            j.fitx(), j.fity(), j.fitz())
    if j.pSecondDecay():
        fprint (f, '\n     second:')
        dump_TrigEFBphys (j.pSecondDecay(), f)
    vec = j.trackVector()
    for i in range(len(vec)):
        t = vec[i]
        fprint (f, '\n     tv:')
        if t.isValid():
            fprint (f, t.dataID(), t.index())
        else:
            fprint (f, '(invalid)')
    return


def dump_TrigL2Bjet (j, f):
    dump_Fourvec (j, f)
    fprint (f, j.isValid(), j.roiId())
    fprint (f, '\n   ', j.prmVtx(), j.xComb(), j.xIP1D(), j.xIP2D(),
            j.xIP3D(), j.xCHI2(), j.xSV(), j.xMVtx(), j.xEVtx(), j.xNVtx())
    fprint (f, '\n   ',
            tonone(j.TrackCollection()),
            tonone(j.PrmVertexCollection()),
            tonone(j.SecVertexCollection()))
    return


def dump_TrigL2Bphys (j, f):
    fprint (f, j.roiId(), j.particleType(), j.eta(), j.phi(),
            j.mass(), j.fitmass(), j.fitchi2(), j.fitndof(),
            j.fitx(), j.fity(), j.fitz())
    if j.pSecondDecay():
        fprint (f, '\n     second:')
        dump_TrigL2Bphys (j.pSecondDecay(), f)
    # ??? Straightforward iteration fails if jets are also dumped,
    #     for 15.2.0 samples.  Why?
    #for t in j.trackVector():
    tv = j.trackVector()
    for i in range(tv.size()):
        t = tv[i]
        fprint (f, '\n     tv:', t.dataID(), t.index())
    return


def dump_TrigMissingET (m, f):
    fprint (f, '  ', m.ex(), m.ey(), m.ez(), m.sumEt(), m.sumE(), m.et(),
            m.e(), m.RoIword(), m.getFlag(), m.getNumOfComponents())
    for ic in range(m.getNumOfComponents()):
        fprint (f, '\n   ', m.getNameOfComponent(ic),
                m.getExComponent(ic), m.getEyComponent(ic),
                m.getEzComponent(ic), m.getSumEtComponent(ic),
                m.getSumEComponent(ic), m.getComponentCalib0(ic),
                m.getComponentCalib1(ic), m.getSumOfSigns(ic),
                m.getUsedChannels(ic), m.getStatus(ic))
    fprintln (f, ' ')
    return
@nolist
def dump_TrigMissingET_nolist (m, f):
    dump_TrigMissingET (m, f)
    return


def dump_RoiDescriptor (d, f):
    fprint (f, '  ',
            d.version(), d.isFullscan(),
            d.eta(), d.etaPlus(), d.etaMinus(),
            d.phi(), d.phiPlus(), d.phiMinus(),
            d.zed(), d.zedPlus(), d.zedMinus(),
            d.composite(), d.manageConstituents(), d.size(),
            formatItemUsingLong ([d.at(i).roiId() for i in range(d.size())]))
    return


def dump_TrigRoiDescriptor (d, f):
    fprint (f, '  ', d.roiId(), d.l1Id(), d.roiWord())
    dump_RoiDescriptor (d, f)
@nolist
def dump_TrigRoiDescriptor_nolist (d, f):
    dump_TrigRoiDescriptor (d, f)
    return


def dump_Trig3Momentum (m, f):
    fprint (f, m.e(), m.eta(), m.phi(), m.inCone(), m.caloSample())
    return


def dump_TrigT2Jet (j, f):
    fprint (f, '  ')
    dump_Fourvec (j, f)
    fprint (f, '  ', j.e(), j.ehad0(), j.eem0(), j.eta(), j.phi(), j.m(),
            j.RoIword())
    if j.grid():
        for m in j.grid():
            fprint (f, '\n   ')
            dump_Trig3Momentum (m, f)
    fprintln (f, ' ')
    return
@nolist
def dump_TrigT2Jet_nolist (j, f):
    dump_TrigT2Jet (j, f)
    return


def dump_TrigTauClusterDetails (t, f):
    if not t: return
    for s in range(4):
        fprint (f, '\n    EM%d: '% s)
        fprint (f, t.EMRadius(s), t.EMenergyWidth(s), t.EMenergyMedium(s),
                t.EMenergyWide(s), t.EMenergyNarrow(s))
    for s in range(3):
        fprint (f, '\n    HAD%d: '% s)
        fprint (f, t.HADRadius(s), t.HADenergyWidth(s), t.HADenergyMedium(s),
                t.HADenergyWide(s), t.HADenergyNarrow(s))
    return

def dump_TrigTauCluster (t, f):
    fprint (f, '  ')
    dump_TrigCaloCluster (t, f)
    fprint (f, '\n   ', t.energy(), t.et(), t.EMCalibEnergy(), t.EMenergy(),
            t.HADenergy(), t.eta(), t.phi(), t.IsoFrac(), t.numStripCells(),
            t.stripWidth(), t.eCalib(), t.eEMCalib(), t.EMRadius2())
    for s in range(25):
        fprint (f, '\n    %2d: '% s)
        fprint (f, t.energy(s))
    dump_TrigTauClusterDetails (t.clusterDetails(), f)
    fprintln (f, ' ')
    return
@nolist
def dump_TrigTauCluster_nolist (t, f):
    dump_TrigTauCluster (t, f)
    return


def dump_TrigHisto (h, f):
    fprint (f, "x:", h.nbins_x(), h.min_x(), h.max_x(), '[')
    for x in h.contents():
        fprint (f, x)
    fprint (f, ']')
    return
def dump_TrigHisto1D (h, f):
    dump_TrigHisto (h, f)
    return
def dump_TrigHisto2D (h, f):
    fprint (f, "y:", h.nbins_y(), h.min_y(), h.max_y())
    dump_TrigHisto (h, f)
    return

def dump_TrigSpacePointCounts (c, f):
    fprint (f, c.sctSpEndcapC(), c.sctSpBarrel(), c.sctSpEndcapA(), '[')
    for id in c.droppedSctModules():
        fprint (f, id.getString())
    fprint (f, ']')
    fprint (f, '\n   clus endcap c: ')
    dump_TrigHisto2D (c.pixelClusEndcapC(), f)
    fprint (f, '\n   clus barrel: ')
    dump_TrigHisto2D (c.pixelClusBarrel(), f)
    fprint (f, '\n   clus endcap a: ')
    dump_TrigHisto2D (c.pixelClusEndcapA(), f)
    return
@nolist
def dump_TrigSpacePointCounts_nolist (c, f):
    dump_TrigSpacePointCounts (c, f)
    return


def dump_TrigTauTracksInfo (t, f):
    fprint (f, '  ')
    dump_Fourvec (t, f)
    fprint (f, '  ', t.roiId(), t.nCoreTracks(), t.nSlowTracks(),
            t.nIsoTracks(),
            t.charge(), t.leadingTrackPt(), t.scalarPtSumCore(),
            t.scalarPtSumIso(), t.ptBalance())
    dump_Fourvec (t.threeFastestTracks(), f)
    if t.trackCollection():
        fprint (f, '\n   ')
        dump_TrigInDetTrackCollection (t.trackCollection(), f)
    fprintln (f, ' ')
    return
@nolist
def dump_TrigTauTracksInfo_nolist (t, f):
    dump_TrigTauTracksInfo (t, f)
    return


def dump_MuonFeature (m, f):
    fprint (f, '  ', m.roiId(), m.saddress(), m.pt(), m.radius(),
            m.eta(), m.phi(), m.dir_phi(), m.zeta(), m.dir_zeta(), m.beta(),
            m.sp1_r(), m.sp1_z(), m.sp1_slope(),
            m.sp2_r(), m.sp2_z(), m.sp2_slope(),
            m.sp3_r(), m.sp3_z(), m.sp3_slope(),
            m.br_radius(), m.br_sagitta(), m.ec_alpha(), m.ec_beta(),
            m.dq_var1(), m.dq_var2(), m.algoId())
    return
@nolist
def dump_MuonFeature_nolist (m, f):
    dump_MuonFeature (m, f)
    return


def dump_MuonFeatureDetails (m, f):
    fprint (f, m.extension_capacity(),
            m.max_rob_capacity(),
            m.max_csm_capacity(),
            m.max_lvl1_emu_capacity(),
            m.max_rpc_hits_capacity(),
            m.max_tgc_hits_capacity(),
            m.max_mdt_hits_capacity(),
            m.id(), m.te_id(), m.error())
    fprint (f, '\n  ',
            m.lvl1_id(), m.lumi_block(), m.muondetmask(),
            m.roi_id(), m.roi_system(), m.roi_subsystem(),
            m.roi_sector(), m.roi_number(), m.roi_threshold(),
            daz(m.roi_eta()), daz(m.roi_phi()),
            m.rpc_pad_error(), m.tgc_rdo_error())
    fprint (f, '\n  ',
            m.rpc1_x(), m.rpc1_y(), m.rpc1_z(),
            m.rpc2_x(), m.rpc2_y(), m.rpc2_z(),
            m.rpc3_x(), m.rpc3_y(), m.rpc3_z())
    fprint (f, '\n  ',
            m.tgc_Mid1_eta(), m.tgc_Mid1_phi(), m.tgc_Mid1_r(), m.tgc_Mid1_z(),
            m.tgc_Mid2_eta(), m.tgc_Mid2_phi(), m.tgc_Mid2_r(), m.tgc_Mid2_z(),
            m.tgc_Mid_rho_chi2(), m.tgc_Mid_rho_N(),
            m.tgc_Mid_phi_chi2(), m.tgc_Mid_phi_N())
    fprint (f, '\n  ',
            m.tgc_Inn_eta(),     m.tgc_Inn_phi(),
            m.tgc_Inn_r(),       m.tgc_Inn_z(),
            m.tgc_Inn_rho_std(), m.tgc_Inn_rho_N(),
            m.tgc_Inn_phi_std(), m.tgc_Inn_phi_N(),
            m.tgc_PT())
    fprint (f, '\n  ',
            m.mdt_Inner_slope(),
            m.mdt_Inner_intercept(),
            m.mdt_Inner_Z(),
            m.mdt_Inner_R(),
            m.mdt_Inner_fit_chi(),
            m.mdt_Middle_slope(),
            m.mdt_Middle_intercept(),
            m.mdt_Middle_Z(),
            m.mdt_Middle_R(),
            m.mdt_Middle_fit_chi(),
            m.mdt_Outer_slope(),
            m.mdt_Outer_intercept(),
            m.mdt_Outer_Z(),
            m.mdt_Outer_R(),
            m.mdt_Outer_fit_chi())
    fprint (f, '\n  ',
            m.Address(), m.Sagitta(), m.Radius(), m.Slope(), m.Intercept(),
            m.Alpha(), m.Beta(), m.DeltaR(), m.Speed_over_c(),
            m.PhiMap(), m.Phi(), m.PhiDir(), m.Pt(), m.Charge())
    fprint (f, '\n  ',
            m.eta_pivot_lay0(),
            m.eta_pivot_lay1(),
            m.eta_low_0_lay0(),
            m.eta_low_1_lay0(),
            m.eta_low_0_lay1(),
            m.eta_low_1_lay1(),
            m.eta_high_0_lay0(),
            m.eta_high_1_lay0(),
            m.eta_high_0_lay1(),
            m.eta_high_1_lay1(),
            m.phi_pivot_lay0(),
            m.phi_pivot_lay1(),
            m.phi_low_0_lay0(),
            m.phi_low_1_lay0(),
            m.phi_low_0_lay1(),
            m.phi_low_1_lay1(),
            m.phi_high_0_lay0(),
            m.phi_high_1_lay0(),
            m.phi_high_0_lay1(),
            m.phi_high_1_lay1())

    fprint (f, '\n  ', formatItemUsingLong (list(m.rob_id())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.csm_id())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.csm_size())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.csm_error())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.removed_rob_id())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.removed_csm_id())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.lvl1_emulation())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.pad_hit_onlineId())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.pad_hit_code())))
    fprint (f, '\n  ', safe_float_vector(m.pad_hit_x()))
    fprint (f, '\n  ', safe_float_vector(m.pad_hit_y()))
    fprint (f, '\n  ', safe_float_vector(m.pad_hit_z()))
    fprint (f, '\n  ', safe_float_vector(m.pad_hit_r()))
    fprint (f, '\n  ', safe_float_vector(m.pad_hit_p()))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Inn_rho_hit_phi()))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Inn_rho_hit_r()))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Inn_rho_hit_z()))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Inn_rho_hit_width()))
    fprint (f, '\n  ', formatItemUsingLong (list(m.tgc_Inn_rho_hit_in_seg())))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Inn_phi_hit_phi()))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Inn_phi_hit_r()))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Inn_phi_hit_z()))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Inn_phi_hit_width()))
    fprint (f, '\n  ', formatItemUsingLong (list(m.tgc_Inn_phi_hit_in_seg())))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Mid_rho_hit_phi()))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Mid_rho_hit_r()))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Mid_rho_hit_z()))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Mid_rho_hit_width()))
    fprint (f, '\n  ', formatItemUsingLong (list(m.tgc_Mid_rho_hit_in_seg())))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Mid_phi_hit_phi()))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Mid_phi_hit_r()))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Mid_phi_hit_z()))
    fprint (f, '\n  ', safe_float_vector(m.tgc_Mid_phi_hit_width()))
    fprint (f, '\n  ', formatItemUsingLong (list(m.tgc_Mid_phi_hit_in_seg())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.mdt_onlineId())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.mdt_offlineId())))
    fprint (f, '\n  ', safe_float_vector(m.mdt_tube_r()))
    fprint (f, '\n  ', safe_float_vector(m.mdt_tube_z()))
    fprint (f, '\n  ', safe_float_vector(m.mdt_tube_residual()))
    fprint (f, '\n  ', safe_float_vector(m.mdt_tube_time()))
    fprint (f, '\n  ', safe_float_vector(m.mdt_tube_space()))
    fprint (f, '\n  ', safe_float_vector(m.mdt_tube_sigma()))
    fprint (f, '\n  ', formatItemUsingLong (list(m.extension0())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.extension1())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.extension2())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.extension3())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.extension4())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.extension5())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.extension6())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.extension7())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.extension8())))
    fprint (f, '\n  ', formatItemUsingLong (list(m.extension9())))
    return


def dump_TileMuFeature (m, f):
    fprint (f, '  ', m.eta(), m.phi(), m.quality(), list(m.enedep()))
    return


def dump_TileTrackMuFeature (m, f):
    dump_Fourvec (m, f)
    fprint (f, m.PtTR_Trk(), m.EtaTR_Trk(), m.PhiTR_Trk(), m.Typ_IDTrk())
    fprint (f, '\n   ', m.pt(), m.eta(), m.phi(), m.m())
    fprint (f, '\n   ', m.TileMuLink().isValid())
    if m.TileMuLink().isValid():
        fprintln (f, m.TileMuLink().dataID(), m.TileMuLink().index())
    fprint (f, '\n   ', m.IDScanLink().isValid())
    if m.IDScanLink().isValid():
        fprintln (f, m.IDScanLink().dataID(), m.IDScanLink().index())
    return


def dump_CombinedMuonFeature (m, f):
    fprint (f, '  ')
    dump_Fourvec (m, f)
    fprint (f, m.pt(), m.eta(), m.phi(), m.m(), m.sigma_pt())
    if m.IDTrackLink().isValid() and m.IDTrack():
        fprint (f, '\n  id')
        dump_TrigInDetTrack (PyTrigInDetTrack(m.IDTrack()), f)
    if m.muFastTrackLink().isValid() and m.muFastTrack():
        fprint (f, '\n  fast')
        dump_MuonFeature (m.muFastTrack(), f)
    else:
        fprintln (f, ' ')
    return
@nolist
def dump_CombinedMuonFeature_nolist (m, f):
    dump_CombinedMuonFeature (m, f)
    return


def dump_TrigMuonEF (m, f):
    dump_Fourvec (m, f)
    fprint (f, m.MuonCode(), m.RoINum(), m.charge())
    return


def dump_TileMu (m, f):
    fprint (f, m.eta(), m.phi(), m.quality(), list(m.enedep()))
    return


def dump_CaloClusterMomentStore (m, f):
    beg = m.begin()
    end = m.end()
    while beg != end:
        fprint (f, '\n     ', beg.getMomentType(), beg.getMoment().getValue())
        beg.next()
    return


def dump_CaloSamplingData (s, f):
    nsamp = s.getNumberOfSamplings()
    for i in range(min(10, s.getNumberOfVariableTypes())):
        if s.contains(i):
            slist = [s.retrieveData(i, samp) for samp in range(nsamp)]
            fprint (f, '\n     ', i, slist)
    return


def dump_CaloShower (s, f):
    fprint (f, '   moments:')
    dump_CaloClusterMomentStore (s.getMomentStore(), f)
    fprint (f, '\n   sampling data:')
    dump_CaloSamplingData (s.getSamplingStore(), f)
    return


@nolist
def dump_CaloTowerContainer (t, f):
    # Note: container contents aren't saved...
    fprintln (f, t.neta(), t.etamin(), t.deta(),
              t.nphi(), t.phimin(), t.dphi())
    return


@nolist
def dump_CaloTopoTowerContainer (t, f):
    # Note: container contents aren't saved...
    dump_CaloTowerContainer (t, f)
    fprint (f, '  ', t.GetMinimumCellEnergy(),
            t.GetMinimumClusterEnergy(),
            t.GetUseCellWeights(),
            t.GetUseNoiseTool(),
            t.GetUsePileUpNoise(),
            t.GetNoiseSigma(),
            t.GetCellESignificanceThreshold(),
            t.GetCaloSelection())
    ci = t.GetCaloIndices()
    # list(ci) broken in root 6.04.12.
    fprint (f, [ci[i] for i in range(ci.size())])
    def dl(l):
        fprint (f, '  ')
        if l.isValid(): fprintln (f, l.dataID())
        else: fprintln (f, '(null)')
    dl(t.GetClusters())
    dl(t.GetTowers())
    dl(t.GetCells())
    if t.GetCellToClusterMap():
        fprintln (f, '  ', GetCellToClusterMap().size())
    else:
        fprintln (f, '  (null)')
    return


def dump_CaloCalibrationHit (t, f):
    fprint (f, t.cellID().getString(), t.particleID(),
            [t.energy(i) for i in range(4)])
    return


def dump_LArHit (t, f):
    fprint (f, t.cellID().getString(), t.energy(), t.time())
    return


def dump_TileHit (t, f):
    sz = t.size()
    fprint (f, t.identify().getString(), [t.energy(i) for i in range(sz)],
            [t.time(i) for i in range(sz)])
    return


def dump_HepMcParticleLink (p, f):
    fprint (f, p.barcode(), p.eventIndex())
    return


def dump_SiHit (t, f):
    fprint (f, t.identify())
    dump_Threevec (t.localStartPosition(), f)
    dump_Threevec (t.localEndPosition(), f)
    fprint (f, t.energyLoss(), t.meanTime(), t.trackNumber())
    dump_HepMcParticleLink (t.particleLink(), f)
    return


def dump_TRTUncompressedHit (t, f):
    fprint (f, t.GetHitID(), t.GetTrackID(), t.GetParticleEncoding(),
            t.GetKineticEnergy(), t.GetEnergyDeposit(),
            t.GetPreStepX(),  t.GetPreStepY(),  t.GetPreStepZ(),
            t.GetPostStepX(), t.GetPostStepY(), t.GetPostStepZ(),
            t.GetGlobalTime())
    return


def dump_TrigT2MbtsBits (t, f):
    if hasattr(t, 'triggerEnergies'):
        fprint (f, list(t.triggerEnergies()), list(t.triggerTimes()))
    else:
        fprint (f, t.mbtsWord(), list(t.triggerTimes()))
    return


def dump_CosmicMuon (m, f):
    # Not really implemented.
    # The AODs i saw had containers of these, which were always empty.
    # Print as the address, so that if we do see one with contents,
    # we'll see a miscompare and know to fill this in.
    fprint (f, id(m))
    #fprint (f, m.p(), m.theta(), m.phi(), m.t(), m.radius(),
    #        m.isIngoing, m.NRpcPairs(), m.NTgcPairs(), m.NMdtHits(), 
    #        m.NMdtSegs())
#   void getRefPoint(double vec[3]) const;
#   double residual(double z, double r) const;
#   double longPos(double z, double r) const;
#   void perigee(double point[3], double x0=0, double y0=0, double z0=0) const;
#   void perigee2D(double point[2], double x0=0, double y0=0) const;
#   double dca3D(double x0=0.0, double y0=0.0, double z0=0.0) const;
#   double dcaXY(double x0=0.0, double y0=0.0) const;
#   double dca2D_Z(double x0=0.0, double y0=0.0) const;
    return


def dump_MdtTrackSegment (t, f):
    # Not really implemented.
    # The AODs i saw had containers of these, which were always empty.
    # Print as the address, so that if we do see one with contents,
    # we'll see a miscompare and know to fill this in.
    fprint (f, id(t))
    return


def dump_IsoMuonFeature (m, f):
    dump_Fourvec (m, f)
    fprint (f, '\n      ', m.getEtInnerConeEC(), m.getEtOuterConeEC(), 
            m.getEtInnerConeHC(), m.getEtOuterConeHC())
    fprint (f, '\n      ', m.getNTracksCone(), m.getSumPtTracksCone(),
            m.getPtMuTracksCone())
    fprint (f, '\n      ', m.getRoiIdMu(), m.getPtMu(), m.getEtaMu(),
            m.getPhiMu(), m.getLArWeight(), m.getTileWeight())
    return


def dump_TrigMuonEFTrack (t, f):
    if not t:
        fprint (f, None)
        return
    dump_Fourvec (t, f)
    fprint (f, t.charge())
    return


def dump_TrigMuonEFInfoTrack (t, f):
    fprint (f, '\n ', t.MuonType())
    fprint (f, '\n    spectrometer: ')
    dump_TrigMuonEFTrack (t.SpectrometerTrack(), f)
    fprint (f, '\n    extrapolated: ')
    dump_TrigMuonEFTrack (t.ExtrapolatedTrack(), f)
    fprint (f, '\n    combined: ')
    dump_TrigMuonEFTrack (t.CombinedTrack(), f)
    return


def dump_TrigMuonEFInfo (t, f):
    fprint (f, t.RoINum())
    for tt in t.TrackContainer():
        dump_TrigMuonEFInfoTrack (tt, f)
    return


def dump_RingerRings (r, f):
    fprint (f, list(r.rings()))
    return


def dump_TrigTrtHitCounts (p, f):
    fprint (f, '  barrel')
    dump_TrigHisto1D (p.barrel(), f)
    fprint (f, '\n      endcapA')
    dump_TrigHisto1D (p.endcapA(), f)
    fprint (f, '\n      endcapC')
    dump_TrigHisto1D (p.endcapC(), f)
    return


def dump_TrigRNNOutput (p, f):
    fprint (f, list(p.output()))
    #if p.isValid():
    #    cl = p.cluster()
    #    fprint (f, '  [', cl.rawEnergy(), cl.rawEta(), cl.rawPhi(), ']')
    return


def dump_InDetLowBetaCandidate (p, f):
    if p == None:
        fprint (f, '(null)')
        return
    if hasattr (p, 'getTRTInverseBeta'):
        fprint (f, p.getTRTCorrBitsOverThreshold(),
                p.getTRTInverseBeta(),
                p.getTRTInverseBetaError(),
                p.getTRTNLastBits())
    else:
        fprint (f, p.getTRTCorrBitsOverThreshold(),
                p.getTRTTrailingEdge(),
                p.getTRTTrailingEdgeError(),
                p.getTRTNLastBits())
        others = (p.getTRTdEdx(),
                  p.getTRTLikelihoodBeta(),
                  p.getTRTLikelihoodError(),
                  p.getTRTHighTbits(),)
        if max(others)!=0 or min(others) != 0:
            for o in others: fprint (f, o)
    return


@nolist
def dump_LArNoisyROSummary (p, f):
    fprintln (f, '  noisy febs', [id.getString() for id in p.get_noisy_febs()])
    # Dictionary problem here --- punt for now.
    #fprint (f, '  noisy preamps',
    #       [(p.first.getString(),p.second) for p in p.get_noisy_preamps()])
    return


def dump_SkimDecision (p, f):
    fprint (f, p.getName(), p.isAccepted())
    return


@nolist
def dump_RawInfoSummaryForTag (p, f):
    for a in dir(p):
        if a.startswith ('get'):
            fprintln (f, a[3:]+':', getattr(p, a)())
    return


@nolist
def dump_MissingETComposition (m, f):
    for p in toiter1 (m):
        fprint (f, '  ', typename(p.__class__))
        if not p:
            fprint (f, '(null)')
            continue
        dump_Fourvec (p, f)
        try:
            w = m.getWeight (p)
            fprint (f, ' [', w.wet(), w.wpx(), w.wpy(), w.statusWord(), ']')
        except RuntimeError:
            pass
        fprintln (f, '')
    return

@nolist
def dump_ChamberT0s (m, f):
    for p in m.getAllT0s():
        fprintln (f, '   ', p.first.getString(), p.second)
    return


def dump_TrigMuonClusterFeature (m, f):
    dump_Fourvec (m, f)
    fprint (f, m.getNRoi(), m.getNTRK(), m.getNJet())
    return


def dump_TrigPassBits (b, f):
    fprint (f, [b.isPassing(i) for i in range(b.size())])
    return


def dump_TrigOperationalInfo (b, f):
    p = b.infos()
    fprint (f, list(p.first), list(p.second))
    return


def dump_TrigVertexCounts (v, f):
    fprint (f,
            formatItemUsingLong (list(v.vtxNtrks())),
            list(v.vtxTrkPtSqSum()))
    return

def dump_TrigMuonEFIsolation (m, f):
    fprint (f, m.sumTrkPtCone02(), m.sumTrkPtCone03(), m.trackPosition())
    return


def dump_TrigPassFlags (m, f):
    # The interface changed, sigh...
    if hasattr (m, 'getNumObjects'):
        for i in range (m.getNumObjects()):
            fwrite (f, ' \n')
            fprint (f, i, list(m.getFlag(i,j) for j in range(m.getNumFlags(i))))
    else:
        for i in range (m.size()):
            fwrite (f, ' \n')
            fprint (f, i, list(m.getFlagBit(i,j) for j in range(m.flagSize())))
    return


def dump_eflowObject (e, f):
    dump_Fourvec (e, f)
    fprint (f, e.eflowType(), e.isValid(), e.d0(), e.z0(),
            e.getPassEOverPCheck(), e.getIsSubtracted(),
            e.getIsDuplicated(), e.getCaloRecoStatus().getStatusWord())
    for i in range(e.numTrack()):
        fprint (f, '\n    tk: ')
        dump_EL (e.trackLink(i), f)
    for i in range(e.numClus()):
        fprint (f, '\n    cl: ')
        dump_EL (e.clusLink(i), f)
    fprint (f, '\n    mu: ')
    dump_EL (e.muonLink(), f)
    fprint (f, '\n    cv: ')
    dump_EL (e.conversionLink(), f)
    return


@nolist
def dump_eflowObjectContainer (c, f):
    print (c.missET(), c.sumET(), c.pTx(), c.pTy(), c.numEf(),
           c.numPhot(), c.numEle(), c.numMuo(), c.isValid(),
           c.circularity(), c.thrust(), c.oblateness(),
           file = f)
    dump_list (c, f, dump_eflowObject)
    return

@nolist
def dump_RecoTimingObj (c, f):
    fprintln (f, list (c))
    return


@nolist
def dump_TrigConfKeys (p, f):
    fprint (f, p.smk(), p.l1psk(), p.hltpsk())
    return


def dump_IdentContIndex (p, f):
    fprint (f, p.collHash(), p.objIndex())
    return


def dump_PrepRawData (p, f):
    fprint (f, p.identify().getString())
    dump_IdentContIndex (p.getHashAndIndex(), f)
    dump_AmgVector (p.localPosition(), f)
    dump_AmgMatrix (p.localCovariance(), f)
    fprint (f, '[')
    for i in p.rdoList():
        fprint (f, i.getString())
    fprint (f, ']')
    return


def dump_MuonCluster (p, f):
    dump_PrepRawData (p, f)
    dump_AmgVector (p.globalPosition(), f, thresh=1e-8)
    return


def dump_CscPrepData (p, f):
    dump_MuonCluster (p, f)
    fprint (f, p.charge(), p.time(), p.status(), p.timeStatus())
    fprint (f, p.detectorElement().identifyHash().value())
    return


def dump_CscStripPrepData (p, f):
    dump_MuonCluster (p, f)
    fprint (f, p.timeOfFirstSample(), p.samplingPhase(), p.samplingTime())
    fprint (f, list(p.sampleCharges()))
    fprint (f, p.detectorElement().identifyHash().value())
    return


def dump_TgcPrepData (p, f):
    dump_MuonCluster (p, f)
    fprint (f, p.getBcBitMap())
    fprint (f, p.detectorElement().identifyHash().value())
    return


def dump_TgcCoinData (p, f):
    fprint (f, p.type(), p.channelIdIn().getString(), p.channelIdOut().getString(), p.identifyHash().value())
    dump_IdentContIndex (p.getHashAndIndex(), f)
    fprint (f, p.isAside(), p.phi(), p.isInner(), p.isForward(), p.isStrip(), p.trackletId(), p.trackletIdStrip())
    fprint (f, p.widthIn(), p.widthOut())
    fprint (f, p.delta(), p.roi(), p.pt(), p.veto(), p.sub(), p.inner(), p.isPositiveDeltaR())
    if p.channelIdIn().get_compact() != 0:
        dump_AmgVector (p.posIn(), f, thresh=1e-8)
        dump_AmgVector (p.globalposIn(), f, thresh=1e-8)
    if p.channelIdOut().get_compact() != 0:
        dump_AmgVector (p.posOut(), f, thresh=1e-8)
        dump_AmgVector (p.globalposOut(), f, thresh=1e-8)
    if p.hasErrMat():
        dump_AmgMatrix (p.errMat(), f)
    if p.detectorElementIn():
        fprint (f, p.detectorElementIn().identifyHash().value())
    else:
        fprint (f, '(null detel)')
    if p.detectorElementOut():
        fprint (f, p.detectorElementOut().identifyHash().value())
    else:
        fprint (f, '(null detel)')
    return


def dump_MdtPrepData (p, f):
    dump_MuonCluster (p, f)
    fprint (f, p.tdc(), p.adc(), p.status())
    fprint (f, p.detectorElement().identifyHash().value())
    return


def dump_RpcPrepData (p, f):
    fprint (f, p.time(), p.ambiguityFlag())
    fprint (f, p.detectorElement().identifyHash().value())
    return

    
def dump_RpcCoinData (p, f):
    dump_RpcPrepData (p, f)
    fprint (f, p.ijk(), p.threshold(), p.overlap(), p.parentCmId(), p.parentPadId(), p.parentSectorId())
    return


def dump_SiWidth (p, f):
    dump_AmgVector (p.colRow(), f)
    dump_AmgVector (p.widthPhiRZ(), f)
    return

    
def dump_SiCluster (p, f):
    dump_PrepRawData (p, f)
    fprint (f, p.gangedPixel())
    dump_AmgVector (p.globalPosition(), f)
    dump_SiWidth (p.width(), f)
    fprint (f, p.detectorElement().identifyHash().value())
    return


def dump_PixelCluster (p, f):
    dump_SiCluster (p, f)
    fprint (f, p.omegax(), p.omegay(), list (p.totList()))
    fprint (f, p.totalToT(), list(p.chargeList()), p.totalCharge())
    fprint (f, p.isFake(), p.isAmbiguous(), p.LVL1A(), p.splitInfoRaw(), p.tooBigToBeSplit())
    return


def dump_SCT_Cluster (p, f):
    dump_SiCluster (p, f)
    fprint (f, p.hitsInThirdTimeBin())
    return


def dump_TRT_DriftCircle (p, f):
    dump_PrepRawData (p, f)
    fprint (f, p.getWord())
    fprint (f, p.detectorElement().identifyHash().value())
    return


def dump_InDetRawData (p, f):
    fprint (f, p.identify().getString(), p.getWord())
    return


def dump_PixelRDORawData (p, f):
    dump_InDetRawData (p, f)
    fprint (f, p.getToT(), p.getBCID(), p.getLVL1A(), p.getLVL1ID())
    return


def dump_TRT_RDORawData (p, f):
    dump_InDetRawData (p, f)
    fprint (f, p.highLevel(), p.timeOverThreshold(), p.driftTimeBin())
    return


def dump_SCT_RDORawData (p, f):
    dump_InDetRawData (p, f)
    fprint (f, p.getGroupSize())
    return


def dump_IDC (payload_dumper, p, f):
    beg = p.begin()
    end = p.end()
    nextfunc = beg.__next__ if hasattr (beg.__class__, '__next__') else beg.next
    while beg != end:
        coll = beg.cptr()
        fprint (f, 'IDC', beg.hashId().value(), coll.identifyHash().value(), coll.size())
        if hasattr (coll, 'type'):
            fprint (f, coll.type())
        for x in coll:
            fprint (f, '\n  ')
            payload_dumper (x, f)
        fwrite (f, '\n')
        nextfunc()
    fwrite (f, '\n')
    return


@nolist
def dump_CscPrepDataContainer (p, f):
    dump_IDC (dump_CscPrepData, p, f)
    return


@nolist
def dump_CscStripPrepDataContainer (p, f):
    dump_IDC (dump_CscStripPrepData, p, f)
    return


@nolist
def dump_RpcCoinDataContainer (p, f):
    dump_IDC (dump_RpcCoinData, p, f)
    return


@nolist
def dump_RpcPrepDataContainer (p, f):
    dump_IDC (dump_RpcPrepData, p, f)
    return


@nolist
def dump_TgcPrepDataContainer (p, f):
    dump_IDC (dump_TgcPrepData, p, f)
    return


@nolist
def dump_TgcCoinDataContainer (p, f):
    dump_IDC (dump_TgcCoinData, p, f)
    return


@nolist
def dump_MdtPrepDataContainer (p, f):
    dump_IDC (dump_MdtPrepData, p, f)
    return


@nolist
def dump_PixelClusterContainer (p, f):
    dump_IDC (dump_PixelCluster, p, f)
    return


@nolist
def dump_SCT_ClusterContainer (p, f):
    dump_IDC (dump_SCT_Cluster, p, f)
    return


@nolist
def dump_TRT_DriftCircleContainer (p, f):
    dump_IDC (dump_TRT_DriftCircle, p, f)
    return


@nolist
def dump_PixelRawDataContainer (p, f):
    dump_IDC (dump_PixelRDORawData, p, f)
    return


@nolist
def dump_TRT_RawDataContainer (p, f):
    dump_IDC (dump_TRT_RDORawData, p, f)
    return


@nolist
def dump_SCT_RawDataContainer (p, f):
    dump_IDC (dump_SCT_RDORawData, p, f)
    return


def dump_CscMcData (p, f):
    fprint (f, p.energy(), p.ypos(), p.zpos(), p.charge())
    return


def dump_CscSimData (p, f):
    fprint (f, p.word())
    for d in p.getdeposits():
        dump_HepMcParticleLink (d.first, f)
        dump_CscMcData (d.second, f)
    return


@nolist
def dump_CscSimDataCollection (p, f):
    for elt in p:
        fprint (f, elt.first.getString())
        dump_CscSimData (elt.second, f)
        fwrite (f, '\n')
    return


def dump_MuonMcData (p, f):
    fprint (f, p.firstEntry(), p.secondEntry())
    return


def dump_MuonSimData (p, f):
    fprint (f, p.word())
    dump_AmgVector (p.globalPosition(), f)
    for d in p.getdeposits():
        dump_HepMcParticleLink (d.first, f)
        dump_MuonMcData (d.second, f)
    return


@nolist
def dump_MuonSimDataCollection (p, f):
    for elt in p:
        fprint (f, elt.first.getString())
        dump_MuonSimData (elt.second, f)
        fwrite (f, '\n')
    return


@nolist
def dump_PixelGangedClusterAmbiguities (p, f):
    for elt in p:
        dump_SiCluster (elt.first, f)
        dump_SiCluster (elt.second, f)
        fwrite (f, '\n')
    return


def dump_TileRawData (p, f):
    fprint (f, p.identify().getString())
    return


def dump_TileDigits (p, f):
    dump_TileRawData (p, f)
    fprint (f, list(p.get_digits()))
    return


@nolist
def dump_TileDigitsContainer (p, f):
    fprint (f, p.get_unit(), p.get_type(), p.get_bsflags())
    fprint (f, p.hashFunc().max(), p.hashFunc().offset(), p.hashFunc().type())
    fwrite (f, '\n')
    beg = p.begin()
    end = p.end()
    nextfunc = beg.__next__ if hasattr (beg.__class__, '__next__') else beg.next
    while beg != end:
        coll = beg.cptr()
        fprint (f, 'TDC',
                beg.hashId().value(),
                list(coll.getFragChipCRCWords()),
                list(coll.getFragChipCRCWordsHigh()),
                list(coll.getFragChipHeaderWords()),
                list(coll.getFragChipHeaderWordsHigh()),
                formatItemUsingLong(list(coll.getFragExtraWords())),
                coll.getFragBCID(), coll.getFragSize())
        for x in coll:
            fprint (f, '\n  ')
            dump_TileDigits (x, f)
        fwrite (f, '\n')
        nextfunc()
    fwrite (f, '\n')
    return


def dump_TileRawChannel (data, f):
    dump_TileRawData (data, f)
    fprint (f, [data.amplitude(i) for i in range(data.size())])
    fprint (f, [data.time(i) for i in range(data.sizeTime())])
    fprint (f, [data.quality(i) for i in range(data.sizeQuality())])
    fprint (f, data.pedestal())
    return



def dump_TileRawDataCollection (data, f):
    fprint (f, '\n  ', data.identify(),
            data.getLvl1Id(),
            data.getLvl1Type(),
            data.getDetEvType(),
            data.getRODBCID())
    for x in data:
        fprint (f, '\n    ')
        dump_TileRawChannel (x, f)
    return
          
          
def dump_TileRawChannelCollection (data, f):
    fprint (f, '\n  ', data.getFragGlobalCRC(),
            data.getFragDSPBCID(),
            data.getFragBCID(),
            data.getFragMemoryPar(),
            data.getFragSstrobe(),
            data.getFragDstrobe(),
            data.getFragHeaderBit(),
            data.getFragHeaderPar(),
            data.getFragSampleBit(),
            data.getFragSamplePar(),
            data.getFragFEChipMask(),
            data.getFragRODChipMask())
    dump_TileRawDataCollection (data, f)
    return


@nolist
def dump_TileRawChannelContainer (data, f):
    fprintln (f, data.get_unit(), data.get_type(), data.get_bsflags())
    beg = data.begin()
    end = data.end()
    while beg != end:
        coll = beg.cptr()
        fprint (f, 'Coll', beg.hashId().value(), coll.size())
        dump_TileRawChannelCollection (coll, f)
        fwrite (f, '\n')
        beg.next()
    fwrite (f, '\n')
  
    return


def dump_TileL2 (p, f):
    fprint (f, p.identify(), p.phi(0), p.cosphi(), p.sinphi())
    fprint (f, [p.sumE(i) for i in range(p.NsumE())])
    fprint (f, [p.eta(i) for i in range(p.NMuons())])
    fprint (f, [p.enemu0(i) for i in range(p.NMuons())])
    fprint (f, [p.enemu1(i) for i in range(p.NMuons())])
    fprint (f, [p.enemu2(i) for i in range(p.NMuons())])
    fprint (f, formatItemUsingLong ([p.qual(i) for i in range(p.NMuons())]))
    fprint (f, formatItemUsingLong ([p.val(i) for i in range(p.Ndata())]))
    return


def dump_TileTTL1 (p, f):
    fprint (f, p.identify().getString(), list(p.fsamples()))
    return


def dump_CMMCPHits (p, f):
    fprint (f, p.crate(), p.dataID(), p.peak(),
            formatItemUsingLong (list(p.HitsVec0())),
            formatItemUsingLong (list(p.HitsVec1())),
            list(p.ErrorVec0()),
            list(p.ErrorVec1()))
    return


def dump_CMMEtSums (p, f):
    fprint (f, p.crate(), p.dataID(), p.peak(),
            formatItemUsingLong (list(p.EtVec())),
            formatItemUsingLong (list(p.ExVec())),
            formatItemUsingLong (list(p.EyVec())),
            list(p.EtErrorVec()),
            list(p.ExErrorVec()),
            list(p.EyErrorVec()))
    return

 
def dump_CMMJetHits (p, f):
   fprint (f, p.crate(), p.dataID(), p.peak(),
           formatItemUsingLong (list(p.HitsVec())),
           list(p.ErrorVec()))
   return


@nolist
def dump_CMMRoI (p, f):
    fprint (f, p.jetEtRoiWord(),
            p.energyRoiWord0(),
            p.energyRoiWord1(),
            p.energyRoiWord2())
    return


def dump_CPMHits (p, f):
    fprint (f, p.crate(), p.module(), p.peak(),
            formatItemUsingLong (list(p.HitsVec0())),
            formatItemUsingLong (list(p.HitsVec1())))
    return


def dump_CPMRoI (p, f):
    fprint (f, p.roiWord())
    return


def dump_CPMTower (p, f):
    fprint (f, p.eta(), p.phi(), p.peak(),
            list(p.emEnergyVec()),
            list(p.hadEnergyVec()),
            list(p.emErrorVec()),
            list(p.hadErrorVec()))
    return


@nolist
def dump_CTP_RDO (p, f):
    fprint (f, p.getCTPVersionNumber(),
            p.getCTPVersion().getVersionNumber(),
            formatItemUsingLong (list(p.getDataWords())),
            p.getL1AcceptBunchPosition(),
            p.getTurnCounter(),
            p.getNumberOfBunches(),
            p.getNumberOfAdditionalWords())
    return


@nolist
def dump_MuCTPI_RDO (p, f):
    fprint (f,
            formatItemUsingLong (list(p.getAllCandidateMultiplicities())),
            formatItemUsingLong (list(p.dataWord())))
    return


def dump_JEMEtSums (p, f):
    fprint (f, p.crate(), p.module(), p.peak(),
            formatItemUsingLong (list(p.EtVec())),
            formatItemUsingLong (list(p.ExVec())),
            formatItemUsingLong (list(p.EyVec())))
    return


def dump_JEMHits (p, f):
    fprint (f, p.crate(), p.module(), p.peak(),
            formatItemUsingLong (list(p.JetHitsVec())))
    return


def dump_JEMRoI (p, f):
    fprint (f, p.roiWord())
    return


def dump_JetElement (p, f):
    fprint (f, p.eta(), p.phi(), p.key(), p.peak(), 
            list(p.emEnergyVec()),
            list(p.hadEnergyVec()),
            list(p.emErrorVec()),
            list(p.hadErrorVec()),
            list(p.linkErrorVec()))
    return


def dump_ROIBHeader (p, f):
    fprint (f, formatItemUsingLong (list(p.header())))
    return


def dump_ROIBTrailer (p, f):
    fprint (f, formatItemUsingLong (list(p.trailer())))
    return


def dump_MuCTPIRoI (p, f):
    fprint (f, p.roIWord())
    return


def dump_MuCTPIResult (p, f):
    dump_ROIBHeader (p.header(), f)
    for r in p.roIVec():
        dump_MuCTPIRoI (r, f)
    dump_ROIBTrailer (p.trailer(), f)
    return


def dump_CTPRoI (p, f):
    fprint (f, p.roIWord())
    return


def dump_CTPResult (p, f):
    dump_ROIBHeader (p.header(), f)
    for r in p.roIVec():
        dump_CTPRoI (r, f)
    dump_ROIBTrailer (p.trailer(), f)
    return


def dump_JetEnergyRoI (p, f):
    fprint (f, p.roIWord())
    return


def dump_JetEnergyResult (p, f):
    dump_ROIBHeader (p.header(), f)
    for r in p.roIVec():
        dump_JetEnergyRoI (r, f)
    dump_ROIBTrailer (p.trailer(), f)
    return


def dump_EMTauRoI (p, f):
    fprint (f, p.roIWord())
    return


def dump_EMTauResult (p, f):
    dump_ROIBHeader (p.header(), f)
    for r in p.roIVec():
        dump_EMTauRoI (r, f)
    dump_ROIBTrailer (p.trailer(), f)
    return


def dump_L1TopoRDO (p, f):
    fprint (f, p.getSourceID(),
            list(p.getErrors()),
            list(p.getDataWords()),
            list(p.getStatusWords()))
    return


def dump_L1TopoResult (p, f):
    dump_ROIBHeader (p.header(), f)
    dump_L1TopoRDO (p.rdo())
    dump_ROIBTrailer (p.trailer(), f)
    return


@nolist
def dump_RoIBResult (p, f):
    fprint (f, 'MuCTPI:')
    dump_MuCTPIResult (p.muCTPIResult(), f)
    fprint (f, '\nCTP:')
    dump_CTPResult (p.cTPResult(), f)
    for r in p.jetEnergyResult():
        fprint (f, '\nJetEnergy:')
        dump_JetEnergyResult(r, f)
    for r in p.eMTauResult():
        fprint (f, '\nEMTau:')
        dump_EMTauResult(r, f)
    for r in p.l1TopoResult():
        fprint (f, '\nL1Topo:')
        dump_L1TopoResult(r, f)
    return


def dump_TriggerTower (p, f):
    fprint (f, p.eta(), p.phi(), p.key(), p.emError(), p.hadError(),
            p.emPeak(), p.emADCPeak(), p.hadPeak(), p.hadADCPeak(),
            list(p.emLUT()), list(p.hadLUT()),
            list(p.emADC()), list(p.hadADC()),
            list(p.emBCIDvec()), list(p.hadBCIDvec()),
            list(p.emBCIDext()), list(p.hadBCIDext()))
    return


def dump_LUCID_RawData (p, f):
    fprint (f, p.getStatus(),
            p.getWord0(),  p.getWord1(),  p.getWord2() , p.getWord3(),
            p.getWord0p(), p.getWord1p(), p.getWord2p(), p.getWord3p(),
            p.getWord0n(), p.getWord1n(), p.getWord2n(), p.getWord3n())
    return


def dump_BCM_RDO (p, f):
    fprint (f, '[', p.getWord1(), p.getWord2(), ']')
    return


def dump_BCM_RDO_Collection (p, f):
    fprint (f, p.getChannel())
    for r in p:
        dump_BCM_RDO(r, f)
    return


def dump_CSCSimHit (p, f):
    fprint (f, p.CSCid(), p.globalTime(), p.energyDeposit())
    dump_Threevec (p.getHitStart(), f)
    dump_Threevec (p.getHitEnd(), f)
    fprint (f, p.particleID(), p.kineticEnergy())
    dump_HepMcParticleLink (p.particleLink(), f)
    return


def dump_MDTSimHit (p, f):
    fprint (f, p.MDTid(), p.globalTime(), p.driftRadius())
    dump_Threevec (p.localPosition(), f)
    fprint (f, p.stepLength(), p.energyDeposit(), p.particleEncoding(), p.kineticEnergy())
    dump_HepMcParticleLink (p.particleLink(), f)
    return


def dump_RPCSimHit (p, f):
    fprint (f, p.RPCid(), p.globalTime())
    dump_Threevec (p.localPosition(), f)
    dump_Threevec (p.postLocalPosition(), f)
    fprint (f, p.stepLength(), p.energyDeposit(), p.particleEncoding(), p.kineticEnergy())
    dump_HepMcParticleLink (p.particleLink(), f)
    return


def dump_TGCSimHit (p, f):
    fprintln (f, p.TGCid(), p.globalTime())
    dump_Threevec (p.localPosition(), f)
    dump_Threevec (p.localDireCos(), f)
    fprint (f, p.stepLength(), p.energyDeposit(), p.particleEncoding(), p.kineticEnergy())
    dump_HepMcParticleLink (p.particleLink(), f)
    return


def dump_LArDigit (p, f):
    fprint (f, p.channelID().getString(), p.gain(), list(p.samples()))
    return


def dump_ZdcRawData (p, f):
    fprint (f, p.identify().getString())
    return


def dump_ZdcDigits (p, f):
    dump_ZdcRawData (p, f)
    fprint (f, list(p.get_digits_gain0_delay0()))
    fprint (f, list(p.get_digits_gain0_delay1()))
    fprint (f, list(p.get_digits_gain1_delay0()))
    fprint (f, list(p.get_digits_gain1_delay1()))
    return


def dump_EventBookkeeper (p, f, level=0):
    fprint (f, p.getName(), p.getDescription(), p.getInputStream(), p.getOutputStream(), p.getLogic(), p.getNAcceptedEvents(), p.getNWeightedAcceptedEvents(), p.getCycle())
    for c in list(p.getChildrenEventBookkeepers()):
        fprint (f, '\n    ' + ('  '*level))
        dump_EventBookkeeper (c, f, level+1)
    return


def format_int(x): return '%d'%x
def format_float(x): return '%.1f'%x
def format_float_vector(v): return ''.join ([format_float(x) for x in v])
def format_el(x):
    if x.isDefaultIndex():
        return '(null)'
    return '%s[%d]' % (x.dataID(), x.index())
char_accessor_ = getattr (ROOT, 'SG::AuxElement::ConstAccessor<char>')
class char_accessor:
    def __init__ (self, name):
        self.ac = char_accessor_ (name)
        return
    def __call__ (self, x):
        return ord(self.ac(x))
uchar_accessor_ = getattr (ROOT, 'SG::AuxElement::ConstAccessor<unsigned char>')
class uchar_accessor:
    def __init__ (self, name):
        self.ac = uchar_accessor_ (name)
        return
    def __call__ (self, x):
        return ord(self.ac(x))
tlist = ['bool',
         'int',
         'short',
         'long',
         'unsigned int',
         'unsigned short',
         'unsigned long',
         'unsigned long long',
         'std::vector<float>',
         'float',
         'double']
accessors = {
    'char' : char_accessor,
    'unsigned char' : uchar_accessor,
    }
for t in tlist:
    aname = 'SG::AuxElement::ConstAccessor<' + t
    if t[-1] == '>': aname += ' '
    aname += '>'
    accessors[t] = getattr (ROOT, aname)


atomic_accessors = {
    'unsigned int' : getattr (ROOT, 'SG::AtomicConstAccessor<unsigned int>'),
    }


def format_obj (x, name=None):
    if type(x) == type(1.5):
        return format_float (x)
    if type(x) == type(1):
        return format_int (x)
    tname = typename(type(x))
    if tname.startswith ('ROOT.'):
        tname = tname[5:]
    if tname.startswith ('ElementLink<'):
        return format_el (x)
    if tname.startswith ('std::vector<') or tname.startswith ('vector<'):
        ipos = tname.find('<')
        tname2 = tname[ipos+1:]
        if (tname2.startswith('char,') or
            tname2.startswith('char>') or
            tname2.startswith ('unsigned char,') or
            tname2.startswith ('unsigned char>')):
            l = ', '.join ([str(ord(x[i])) for i in range(len(x))])
        elif tname2.startswith('bool,') or tname2 == 'bool>':
            l = ', '.join ([str(bool(xx)) for xx in x])
        else:
            l = ', '.join ([format_obj(x[i], name) for i in range(x.size())])
        return '[' + l + ']'
    if type(x) == PyAthena.xAOD.CaloClusterBadChannelData_v1:
        return '<BadChannel: %6.3f/%6.3f/%2d: %04x>' % \
               (x.eta(), x.phi(), x.layer(), x.badChannel())
    if tname == 'set<unsigned int>':
        acls=getattr(PyAthena, 'PyDumper::PySTLAdaptor<std::set<unsigned int>')
        if acls:
            return str(list(toiter1(acls(x))))

    if tname.startswith ('std::pair<') or tname.startswith ('pair<'):
        return '(' + format_obj(x.first, name) + ', ' + format_obj(x.second, name) + ')'

    if tname == 'Trk::VxTrackAtVertex':
        fout = StringIO()
        dump_VxTrackAtVertex (x, fout)
        out = fout.getvalue()
        return '{' + out.replace('\n', '; ') + '}'
    
    return str(x)


def generic_dump_auxitem (x, auxid, f):
    if hasattr (x, 'container'):
        try:
             x.container().getConstStore().getData(auxid)[0]
        except IndexError:
            fprint (f, '<unavailable>')
            return
            
    reg=ROOT.SG.AuxTypeRegistry.instance()
    tname = reg.getTypeName (auxid)
    ac = ROOT.SG.AuxElement.TypelessConstAccessor (reg.getName(auxid))
    try:
        buf = ac(x)
    except TypeError:
        fprint (f, '<unavailable>')
        return
    try:
        obj = ROOT.TPython.CPPInstance_FromVoidPtr (buf, tname)
    except TypeError:
        fprint (f, '<unknown %s>'%tname)
        return
    fprint (f, format_obj(obj, reg.getName(auxid)))
    return


def dump_auxitem (x, auxid, f = sys.stdout):
    if hasattr (x, 'container'):
        auxdata = x.container().getConstStore().getData(auxid)
        if not auxdata:
            fprint (f, '<unavailable>')
            return
        try:
            auxdata[0]
        except IndexError:
            fprint (f, '<unavailable>')
            return

    reg=ROOT.SG.AuxTypeRegistry.instance()
    tname = reg.getTypeName (auxid)
    atomic = reg.getFlags (auxid) & ROOT.SG.AuxTypeRegistry.Atomic
    if atomic:
        ac_cl = atomic_accessors.get (tname)
    else:
        ac_cl = accessors.get (tname)
    #print x, auxid, reg.getName(auxid)
    if ac_cl:
        ac = ac_cl(reg.getName(auxid))
        try:
            val = ac(x)
        except TypeError:
            # Can happen due to schema evolution
            val = '<unavailable>'
        fprint (f,  format_obj(val))
    else:
        generic_dump_auxitem (x, auxid, f)
    return


def dump_auxdata (x, exclude=[], f = sys.stdout):
    reg=ROOT.SG.AuxTypeRegistry.instance()
    #auxids = list(x.getAuxIDs())
    try:
        auxids = ROOT.PyDumper.Utils.getAuxIDVector (x)
    except TypeError:
        print ('xxx', x)
        raise
    auxids = [(reg.getName(id), id) for id in auxids]
    auxids.sort()
    #fprint (f, auxids)
    for name, auxid in auxids:
        if name in exclude: continue
        fprint (f,  name + ': ')
        dump_auxitem (x, auxid, f)
        fprint (f, '\n    ')
    return


def dump_xAOD(o, f):
    fprint (f, typename(o.__class__), '\n    ')
    dump_auxdata (o, f=f)
    return


@nolist
def dump_xAODObject(o, f):
    dump_xAOD(o, f)
    return


@nolist
def dump_xAODObjectNL(o, f):
    dump_xAOD(o, f)
    fwrite (f, '\n')
    return


def dump_list (l, f, dumper, nmax = None):
    i = 0
    for x in l:
        if nmax != None and i >= nmax: break
        i += 1
        fprint (f, '  ')
        dumper (x, f)
        fwrite (f, '\n')
    return


##############################################################################

"""The list of tuples (cpp_type, dumper_fct)
"""
dumpspecs = [
    ["ElectronContainer",                    dump_Electron],
    ["PhotonContainer",                      dump_Photon],
    ["Analysis::MuonContainer",              dump_Muon],
    ["Analysis::TauJetContainer",            dump_TauJet],
    ["TruthParticleContainer",               dump_TruthParticle],
    ["ParticleJetContainer",                 dump_ParticleJet],
    ["Rec::TrackParticleContainer",          dump_TrackParticle],
    ["DataVector<Trk::Track>",               dump_Track],
    ["DataVector<Trk::Segment>",             dump_Segment],
    ["MissingET",                            dump_MissingET],
    ["MissingEtCalo",                        dump_MissingEtCalo],
    ["MissingEtTruth",                       dump_MissingEtTruth],
    ["CaloClusterContainer",                 dump_CaloClusters_sorted],
    ['CaloCellLinkContainer',                dump_CaloCellLink],
    ['CaloCellContainer',                    dump_CaloCell],
    ['CaloClusterCellLinkContainer',         dump_CaloClusterCellLink],
    ["VxContainer",                          dump_VxCandidate],
    ["EventInfo",                            dump_EventInfo],
    ["PileUpEventInfo",                      dump_PileUpEventInfo],
    ["EventStreamInfo",                      dump_EventStreamInfo],
    ["McEventCollection",                    dump_GenEvent],
    ["CTP_Decision",                         dump_CTP_Decision],
    ["LVL1_ROI",                             dump_LVL1_ROI],
    ["TrigDec::TrigDecision",                dump_TrigDecision],
    ["TrigConf::HLTAODConfigData",           dump_HLTAODConfigData],
    ["TrigConf::Lvl1AODConfigData",          dump_Lvl1AODConfigData],
    ["TrigConf::Lvl1AODPrescaleConfigData",  dump_Lvl1AODPrescaleConfigData],
    ["TrackParticleTruthCollection",         dump_TrackParticleTruthCollection],
    ["DetailedTrackTruthCollection",         dump_DetailedTrackTruthCollection],
    ["Rec::MuonSpShowerContainer",           dump_MuonSpShower],
    ["HLT::HLTResult",                       dump_HLTResult],
    ["MissingETSig",                         dump_MissingETSig],
    ["INav4MomAssocs",                       dump_INav4MomAssocs],
    ["TrigInDetTrackTruthMap",               dump_TrigInDetTrackTruthMap],
    ["MissingETSigObjContainer",             dump_MissingETSigObject],
    ["DetStatusMap",                         dump_DetStatusMap],
    ["AtlasHitsVector<TrackRecord>",         dump_TrackRecord],
    ["AthenaHitsVector<TrackRecord>",        dump_TrackRecord],
    ["MissingETSigHypoContainer",            dump_MissingETSigHypo],
    ["JetCollection",                        dump_Jet],
    ["V0Container",                          dump_V0Candidate],
    ["egammaContainer",                      dump_egamma],
    ["DataVector<TrigVertex>",               dump_TrigVertex],
    ["TrigVertexCollection",                 dump_TrigVertex],
    ["TrigTrackCounts",                      dump_TrigTrackCounts_nolist],
    ["TrigTau",                              dump_TrigTau_nolist],
    ["DataVector<TrigElectron>",             dump_TrigElectron],
    ["DataVector<TrigPhoton>",               dump_TrigPhoton],
    ["TrigEMCluster",                        dump_TrigEMCluster_nolist],
    ["TrigInDetTrackCollection",             dump_TrigInDetTrackCollection],
    ["DataVector<TrigEFBjet>",               dump_TrigEFBjet],
    ["DataVector<TrigEFBphys>",              dump_TrigEFBphys],
    ["DataVector<TrigL2Bjet>",               dump_TrigL2Bjet],
    ["DataVector<TrigL2Bphys>",              dump_TrigL2Bphys],
    ["TrigCaloClusterContainer",             dump_TrigCaloCluster],
    ["TrigTauClusterDetailsContainer",       dump_TrigTauClusterDetails],
    ["TrigMissingET",                        dump_TrigMissingET_nolist],
    ["TrigRoiDescriptor",                    dump_TrigRoiDescriptor_nolist],
    ["TrigT2Jet",                            dump_TrigT2Jet_nolist],
    ["TrigTauCluster",                       dump_TrigTauCluster_nolist],
    ["TrigTauTracksInfo",                    dump_TrigTauTracksInfo_nolist],
    ["TrigSpacePointCounts",                 dump_TrigSpacePointCounts_nolist],
    ["CombinedMuonFeature",                  dump_CombinedMuonFeature_nolist],
    ["MuonFeature",                          dump_MuonFeature_nolist],
    ["TrigMuonEFContainer",                  dump_TrigMuonEF],
    ["TileContainer<TileMu>",                dump_TileMu],
    ["CaloShowerContainer",                  dump_CaloShower],
    ["CaloTowerContainer",                   dump_CaloTowerContainer],
    ["CaloTopoTowerContainer",               dump_CaloTopoTowerContainer],
    ["CaloCalibrationHitContainer",          dump_CaloCalibrationHit],
    ['LArHitContainer',                      dump_LArHit],
    ['TileHitVector',                        dump_TileHit],
    ['AtlasHitsVector<TileHit>',             dump_TileHit],
    ['TileRawChannelContainer',              dump_TileRawChannelContainer],
    ['SiHitCollection',                      dump_SiHit],
    ['AtlasHitsVector<SiHit>',               dump_SiHit],
    ['TRTUncompressedHitCollection',         dump_TRTUncompressedHit],
    ['AtlasHitsVector<TRTUncompressedHit>',  dump_TRTUncompressedHit],
    ['AtlasHitsVector<CSCSimHit>',           dump_CSCSimHit],
    ['AtlasHitsVector<MDTSimHit>',           dump_MDTSimHit],
    ['AtlasHitsVector<RPCSimHit>',           dump_RPCSimHit],
    ['AtlasHitsVector<TGCSimHit>',           dump_TGCSimHit],
    ["CombinedMuonFeatureContainer",         dump_CombinedMuonFeature],
    ["MuonFeatureContainer",                 dump_MuonFeature],
    ["TrigEFBjetContainer",                  dump_TrigEFBjet],
    ["TrigEFBphysContainer",                 dump_TrigEFBphys],
    ["TrigEMClusterContainer",               dump_TrigEMCluster],
    ["TrigElectronContainer",                dump_TrigElectron],
    ["TrigL2BjetContainer",                  dump_TrigL2Bjet],
    ["TrigL2BphysContainer",                 dump_TrigL2Bphys],
    ["TrigMissingETContainer",               dump_TrigMissingET],
    ["TrigPhotonContainer",                  dump_TrigPhoton],
    ["TrigT2JetContainer",                   dump_TrigT2Jet],
    ["TrigTauClusterContainer",              dump_TrigTauCluster],
    ["TrigTauContainer",                     dump_TrigTau],
    ["TrigTauTracksInfoCollection",          dump_TrigTauTracksInfo],
    ["TrigSpacePointCountsCollection",       dump_TrigSpacePointCounts],
    ["TrigTrackCountsCollection",            dump_TrigTrackCounts],
    ["TrigT2MbtsBitsContainer",              dump_TrigT2MbtsBits],
    ["TrigRoiDescriptorCollection",          dump_TrigRoiDescriptor],
    ["TileMuFeatureContainer",               dump_TileMuFeature],
    ["TileTrackMuFeatureContainer",          dump_TileTrackMuFeature],
    ["CosmicMuonCollection",                 dump_CosmicMuon],
    ["MdtTrackSegmentCollection",            dump_MdtTrackSegment],
    ["IsoMuonFeatureContainer",              dump_IsoMuonFeature],
    ["TrigMuonEFInfoContainer",              dump_TrigMuonEFInfo],
    ["RingerRingsContainer",                 dump_RingerRings],
    ["TrigTrtHitCountsCollection",           dump_TrigTrtHitCounts],
    ["MuonFeatureDetailsContainer",          dump_MuonFeatureDetails],
    ["TrigRNNOutputContainer",               dump_TrigRNNOutput],
    ["InDet::InDetLowBetaContainer",         dump_InDetLowBetaCandidate],
    ["LArNoisyROSummary",                    dump_LArNoisyROSummary],
    ["SkimDecisionCollection",               dump_SkimDecision],
    ["RawInfoSummaryForTag",                 dump_RawInfoSummaryForTag],
    ["MissingETComposition",                 dump_MissingETComposition],
    ["Muon::ChamberT0s",                     dump_ChamberT0s],
    ["TrigMuonClusterFeatureContainer",      dump_TrigMuonClusterFeature],
    ["TrigPassBitsCollection",               dump_TrigPassBits],
    ["TrigOperationalInfoCollection",        dump_TrigOperationalInfo],
    ["TrigVertexCountsCollection",           dump_TrigVertexCounts],
    ["TrigMuonEFIsolationContainer",         dump_TrigMuonEFIsolation],
    ["eflowObjectContainer",                 dump_eflowObjectContainer],
    ["TrigPassFlagsCollection",              dump_TrigPassFlags],
    ["TrackParticleAssocs",                  dump_TrackParticleAssocs],
    ["ElectronMuonTopoInfoContainer",        dump_ElectronMuonTopoInfo],
    ["RecoTimingObj",                        dump_RecoTimingObj],
    ['MuonCaloEnergyContainer',              dump_CaloEnergy],
    ['CscSimDataCollection',                 dump_CscSimDataCollection],
    ['MuonSimDataCollection',                dump_MuonSimDataCollection],
    ['Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::CscPrepData> >', dump_CscPrepDataContainer],
    ['Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::CscStripPrepData> >', dump_CscStripPrepDataContainer],
    ['Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::TgcPrepData> >', dump_TgcPrepDataContainer],
    ['Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::MdtPrepData> >', dump_MdtPrepDataContainer],
    ['Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::RpcPrepData> >', dump_RpcPrepDataContainer],
    ['Muon::MuonCoinDataContainer<Muon::MuonCoinDataCollection<Muon::RpcCoinData> >', dump_RpcCoinDataContainer],
    ['Muon::MuonCoinDataContainer<Muon::MuonCoinDataCollection<Muon::TgcCoinData> >', dump_TgcCoinDataContainer],
    ['Trk::PrepRawDataContainer<Trk::PrepRawDataCollection<InDet::PixelCluster> >', dump_PixelClusterContainer],
    ['Trk::PrepRawDataContainer<Trk::PrepRawDataCollection<InDet::SCT_Cluster> >', dump_SCT_ClusterContainer],
    ['Trk::PrepRawDataContainer<Trk::PrepRawDataCollection<InDet::TRT_DriftCircle> >', dump_TRT_DriftCircleContainer],
    ['InDetRawDataContainer<InDetRawDataCollection<PixelRDORawData> >', dump_PixelRawDataContainer],
    ['InDetRawDataContainer<InDetRawDataCollection<TRT_RDORawData> >', dump_TRT_RawDataContainer],
    ['InDetRawDataContainer<InDetRawDataCollection<SCT_RDORawData> >', dump_SCT_RawDataContainer],
    ['std::multimap<const InDet::SiCluster*,const InDet::SiCluster*,InDet::compare_SiCluster>', dump_PixelGangedClusterAmbiguities],
    ['TileDigitsContainer',                  dump_TileDigitsContainer],
    ['TileContainer<TileL2>',                dump_TileL2],
    ['TileContainer<TileTTL1>',              dump_TileTTL1],
    ['DataVector<LVL1::CMMCPHits>',          dump_CMMCPHits],
    ['DataVector<LVL1::CMMEtSums>',          dump_CMMEtSums],
    ['DataVector<LVL1::CMMJetHits>',         dump_CMMJetHits],
    ['LVL1::CMMRoI',                         dump_CMMRoI],
    ['DataVector<LVL1::CPMHits>',            dump_CPMHits],
    ['DataVector<LVL1::CPMRoI>',             dump_CPMRoI],
    ['DataVector<LVL1::CPMTower>',           dump_CPMTower],
    ['CTP_RDO',                              dump_CTP_RDO],
    ['MuCTPI_RDO',                           dump_MuCTPI_RDO],
    ['DataVector<LVL1::JEMEtSums>',          dump_JEMEtSums],
    ['DataVector<LVL1::JEMHits>',            dump_JEMHits],
    ['DataVector<LVL1::JEMRoI>',             dump_JEMRoI],
    ['DataVector<LVL1::JetElement>',         dump_JetElement],
    ['DataVector<LVL1::TriggerTower>',       dump_TriggerTower],
    ['ROIB::RoIBResult',                     dump_RoIBResult],
    ['LUCID_RawDataContainer',               dump_LUCID_RawData],
    ['BCM_RDO_Container',                    dump_BCM_RDO_Collection],
    ['LArDigitContainer',                    dump_LArDigit],
    ['EventBookkeeperCollection',            dump_EventBookkeeper],
    ['ZdcDigitsCollection',                  dump_ZdcDigits],

    ['DataVector<xAOD::BTagVertex_v1>',      dump_xAOD],
    ['xAOD::BTagVertexContainer',            dump_xAOD],
    ['DataVector<xAOD::BTagging_v1>',        dump_xAOD],
    ['xAOD::BTaggingContainer',              dump_xAOD],
    ['DataVector<xAOD::CaloCluster_v1>',     dump_xAOD],
    ['xAOD::CaloClusterContainer',           dump_xAOD],
    ['DataVector<xAOD::Electron_v1>',        dump_xAOD],
    ['DataVector<xAOD::EmTauRoI_v1>',        dump_xAOD],
    ['DataVector<xAOD::EmTauRoI_v2>',        dump_xAOD],
    ['xAOD::EmTauRoIContainer',              dump_xAOD],
    ['DataVector<xAOD::JetRoI_v1>',          dump_xAOD],
    ['DataVector<xAOD::JetRoI_v2>',          dump_xAOD],
    ['xAOD::JetRoIContainer',                dump_xAOD],
    ['DataVector<xAOD::Jet_v1>',             dump_xAOD],
    ['xAOD::JetContainer',                   dump_xAOD],
    ['DataVector<xAOD::L2CombinedMuon_v1>',  dump_xAOD],
    ['xAOD::L2CombinedMuonContainer',        dump_xAOD],
    ['DataVector<xAOD::L2StandAloneMuon_v1>',dump_xAOD],
    ['DataVector<xAOD::L2StandAloneMuon_v2>',dump_xAOD],
    ['xAOD::L2StandAloneMuonContainer',      dump_xAOD],
    ['DataVector<xAOD::MuonRoI_v1>',         dump_xAOD],
    ['xAOD::MuonRoIContainer',               dump_xAOD],
    ['DataVector<xAOD::MuonSegment_v1>',     dump_xAOD],
    ['xAOD::MuonSegmentContainer',           dump_xAOD],
    ['DataVector<xAOD::Muon_v1>',            dump_xAOD],
    ['xAOD::MuonContainer',                  dump_xAOD],
    ['DataVector<xAOD::PFO_v1>',             dump_xAOD],
    ['xAOD::PFOContainer',                   dump_xAOD],
    ['DataVector<xAOD::Photon_v1>',          dump_xAOD],
    ['DataVector<xAOD::SlowMuon_v1>',        dump_xAOD],
    ['xAOD::SlowMuonContainer',              dump_xAOD],
    ['DataVector<xAOD::TauJet_v1>',          dump_xAOD],
    ['DataVector<xAOD::TauJet_v2>',          dump_xAOD],
    ['DataVector<xAOD::TauJet_v3>',          dump_xAOD],
    ['xAOD::TauJetContainer',                dump_xAOD],
    ['DataVector<xAOD::TauTrack_v1>',        dump_xAOD],
    ['xAOD::TauTrackContainer',              dump_xAOD],
    ['DataVector<xAOD::Particle_v1>',        dump_xAOD],
    ['xAOD::ParticleContainer',              dump_xAOD],
    ['DataVector<xAOD::TrackParticle_v1>',   dump_xAOD],
    ['xAOD::TrackParticleContainer',         dump_xAOD],
    ['DataVector<xAOD::TrigBphys_v1>',       dump_xAOD],
    ['xAOD::TrigBphysContainer',             dump_xAOD],
    ['DataVector<xAOD::TrigComposite_v1>',   dump_xAOD],
    ['xAOD::TrigCompositeContainer',         dump_xAOD],
    ['DataVector<xAOD::TrigEMCluster_v1>',   dump_xAOD],
    ['xAOD::TrigEMClusterContainer',         dump_xAOD],
    ['DataVector<xAOD::TrigElectron_v1>',    dump_xAOD],
    ['xAOD::TrigElectronContainer',          dump_xAOD],
    ['DataVector<xAOD::TrigMissingET_v1>',   dump_xAOD],
    ['xAOD::TrigMissingETContainer',         dump_xAOD],
    ['DataVector<xAOD::TrigPhoton_v1>',      dump_xAOD],
    ['xAOD::TrigPhotonContainer',            dump_xAOD],
    ['DataVector<xAOD::TrigRNNOutput_v1>',   dump_xAOD],
    ['DataVector<xAOD::TrigRNNOutput_v2>',   dump_xAOD],
    ['xAOD::TrigRNNOutputContainer',         dump_xAOD],
    ['DataVector<xAOD::TrigRingerRings_v1>', dump_xAOD],
    ['DataVector<xAOD::TrigRingerRings_v2>', dump_xAOD],
    ['xAOD::TrigRingerRingsContainer',       dump_xAOD],
    ['DataVector<xAOD::TrigSpacePointCounts_v1>',dump_xAOD],
    ['xAOD::TrigSpacePointCountsContainer',  dump_xAOD],
    ['DataVector<xAOD::TrigT2MbtsBits_v1>',  dump_xAOD],
    ['xAOD::TrigT2MbtsBitsContainer',        dump_xAOD],
    ['DataVector<xAOD::TrigTrackCounts_v1>', dump_xAOD],
    ['xAOD::TrigTrackCountsContainer',       dump_xAOD],
    ['DataVector<xAOD::TrigVertexCounts_v1>',dump_xAOD],
    ['xAOD::TrigVertexCountsContainer',      dump_xAOD],
    ['DataVector<xAOD::TruthEvent_v1>',      dump_xAOD],
    ['xAOD::TruthEventContainer',            dump_xAOD],
    ['DataVector<xAOD::TruthParticle_v1>',   dump_xAOD],
    ['xAOD::TruthParticleContainer',         dump_xAOD],
    ['DataVector<xAOD::TruthVertex_v1>',     dump_xAOD],
    ['xAOD::TruthVertexContainer',           dump_xAOD],
    ['DataVector<xAOD::Vertex_v1>',          dump_xAOD],
    ['xAOD::VertexContainer',                dump_xAOD],
    ['DataVector<xAOD::L2IsoMuon_v1>',       dump_xAOD],
    ['xAOD::L2IsoMuonContainer',             dump_xAOD],
    ['DataVector<xAOD::TrigT2ZdcSignals_v1>',dump_xAOD],
    ['xAOD::TrigT2ZdcSignalsContainer',      dump_xAOD],
    ['DataVector<xAOD::HIEventShape_v1>',    dump_xAOD],
    ['DataVector<xAOD::HIEventShape_v2>',    dump_xAOD],
    ['xAOD::HIEventShapeContainer',          dump_xAOD],
    ['DataVector<xAOD::DiTauJet_v1>',        dump_xAOD],
    ['xAOD::DiTauJetContainer',              dump_xAOD],
    ['DataVector<xAOD::TrigPassBits_v1>',    dump_xAOD],
    ['xAOD::TrigPassBitsContainer',          dump_xAOD],
    ['DataVector<xAOD::TrackParticleClusterAssociation_v1>', dump_xAOD],
    ['xAOD::TrackParticleClusterAssociationContainer', dump_xAOD],
    ['DataVector<xAOD::TruthPileupEvent_v1>',dump_xAOD],
    ['xAOD::TruthPileupEventContainer_v1',   dump_xAOD],
    ['DataVector<xAOD::CaloRings_v1>',       dump_xAOD],
    ['xAOD::CaloRingsContainer',             dump_xAOD],
    ['DataVector<xAOD::RingSet_v1>',         dump_xAOD],
    ['xAOD::RingSetContainer',               dump_xAOD],
    ['DataVector<xAOD::ForwardEventInfo_v1>',dump_xAOD],
    ['xAOD::ForwardEventInfoContainer',      dump_xAOD],
    ['DataVector<xAOD::MBTSModule_v1>',      dump_xAOD],
    ['xAOD::MBTSModuleContainer',            dump_xAOD],
    ['DataVector<xAOD::ZdcModule_v1>',       dump_xAOD],
    ['xAOD::ZdcModuleContainer',             dump_xAOD],
    ['DataVector<xAOD::CPMTower_v2>',        dump_xAOD],
    ['xAOD::CPMTowerContainer',              dump_xAOD],
    ['DataVector<xAOD::JetElement_v2>',      dump_xAOD],
    ['xAOD::JetElementContainer',            dump_xAOD],
    ['DataVector<xAOD::TriggerTower_v2>',    dump_xAOD],
    ['xAOD::TriggerTowerContainer',          dump_xAOD],
    ['xAOD::MissingETContainer_v1',          dump_xAOD],
    ['xAOD::MissingETContainer',             dump_xAOD],
    ['xAOD::MissingETComponentMap_v1',       dump_xAOD],
    ['xAOD::MissingETComponentMap',          dump_xAOD],
    ['xAOD::EventInfo_v1',                   dump_xAODObject],
    ['xAOD::EventInfo',                      dump_xAODObject],
    ['xAOD::EventShape_v1',                  dump_xAODObjectNL],
    ['xAOD::EventShape',                     dump_xAODObjectNL],
    ['xAOD::MissingETAssociationMap_v1',     dump_xAODObjectNL],
    ['xAOD::MissingETAssociationMap',        dump_xAODObjectNL],
    ['xAOD::TrigDecision_v1',                dump_xAODObject],
    ['xAOD::TrigConfKeys_v1',                dump_TrigConfKeys],
    ['xAOD::JetEtRoI_v1',                    dump_xAODObject],
    ['xAOD::EnergySumRoI_v1',                dump_xAODObject],
    ['xAOD::EnergySumRoI_v2',                dump_xAODObject],
    ['xAOD::TrigNavigation_v1',              dump_xAODObject],
    ['xAOD::TrigNavigation',                 dump_xAODObject],
    ['xAOD::RoiDescriptorStore_v1',          dump_xAODObject],
    ['xAOD::RoiDescriptorStore',             dump_xAODObject],

    # Make some of these more compact?

]

## user-friendly interface ----------------------------------------------------
import sys
def get_dumper_fct(klass, ofile=sys.stdout, nmax = None):
    """helper function to automatically retrieve the suitable dumper function
    given the name of a class or the class-type
     @param `klass' a string containing the name of a C++ type or a type
     @param `ofile' a file-like instance where to dump the objects' content
     @param `nmax` maximum number of container elements to dump
    """
    if isinstance(klass, type):
        # ok
        pass
    elif isinstance(klass, str):
        klass = getattr(PyAthena,klass)
        pass
    else:
        raise TypeError('expected a type or a string')
    global dumpspecs

    klname = typename(klass)
    dumpers = [ i for i in dumpspecs if i[0] == klname ]
    if len(dumpers) != 1:
        raise RuntimeError('no suitable dumper function for class [%s]'%
                           klname )
    fct = dumpers[0][1]
    nolist = hasattr (fct, 'nolist') and fct.nolist
    from functools import partial as _partial
    if nolist:
        if fct.nmax:
            dumper = _partial(fct, f=ofile, nmax=nmax)
        else:
            dumper = _partial(fct, f=ofile)
    else:
        dumper = _partial(dump_list, f=ofile, dumper=fct, nmax=nmax)
    return dumper

