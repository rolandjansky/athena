# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import ROOT, math, random
from ParticleGun.histsampling import TH1

## For convenience
PI = math.pi
TWOPI = 2*math.pi


class Sampler(object):
    "Base class for all samplers"

    def shoot(self):
        return RuntimeError("Can't sample from an abstract sampler object.")

    def __call__(self):
        """This is the call method that will actually be used (so that normal
        functions can also be passed in as samplers)."""
        return self.shoot()

    # TODO: add a sampling weight?


class ConstSampler(Sampler):
    "A special-case sampler which just returns one value rather than sampling."

    def __init__(self, val):
        self.val = val

    def shoot(self):
        return self.val

    def __repr__(self):
        return "ConstSampler[%s]" % str(self.val)


## Continuous distribution samplers

class ContinuousSampler(Sampler):
    "Base class for samplers from continuous distributions."
    pass


class UniformSampler(ContinuousSampler):
    "Uniformly sample in the range [low,high)."

    def __init__(self, low, high):
        assert(low <= high)
        self.low = float(low)
        self.high = float(high)

    def shoot(self):
        return random.uniform(self.low, self.high)


class ModUniformSampler(ContinuousSampler):
    "Uniformly sample in the modulus range (-high,low]+[low,high)."

    def __init__(self, low, high):
        assert(low == abs(low) and high == abs(high))
        assert(low <= high)
        self.low = float(low)
        self.high = float(high)

    def shoot(self):
        val = random.uniform(self.low, self.high)
        if random.random() > 0.5:
            val *= -1
        return val


class DisjointUniformSampler(ContinuousSampler):
    "Uniformly sample from a set of disjoint intervals."

    def __init__(self, ranges):
        """
        The ranges variable can either be a list of increasing numbers or a
        list of pairs of numbers.

        The former case will be treated as
        defining alternating on/off ranges for sampling, starting with an active
        one (i.e. it's a list of bin edges). The latter way specifically lists
        the 'on' regions only, with their start and end values in the pairs.

        The behaviour is undefined if the numbers are not ordered or overlap --
        i.e. it might work but hasn't been designed that way and might change in
        future. Don't rely on this behaviour!
        """
        if not ranges:
            raise Exception("You must supply at least one non-null sampling range")
        if hasattr(ranges[0], "__len__"):
            assert all(len(x) == 2 for x in ranges)
            self.ranges = ranges
        else:
            assert len(ranges) > 1
            lows = [x for x in ranges[:-1]]
            highs = [x for x in ranges[1:]]
            myranges = []
            for i, pair in enumerate(zip(lows, highs)):
                if i % 2 == 0:
                    myranges.append(pair)
            assert len(myranges) == len(ranges) // 2
            self.ranges = myranges

    def _getRanges(self):
        return self._ranges

    def _setRanges(self, ranges):
        # TODO: Check that ranges don't overlap
        self._ranges = ranges
        self._totalwidth = sum(r[1] - r[0] for r in ranges)

        runningwidth = 0.0
        self._divisions = [0.0]
        for r in ranges:
            assert(r[1] >= r[0])
            runningwidth += float(r[1] - r[0])
            self._divisions.append(runningwidth)
        self._totalwidth = runningwidth
        for i in range(len(self._divisions)):
            self._divisions[i] = float(self._divisions[i]) / float(self._totalwidth)

    ranges = property(_getRanges, _setRanges)

    def _map_unit_to_val(self, x):
        assert(x >= 0 and x <= 1)
        idx = None
        rem = None
        for i in range(len(self._divisions) - 1):
            if x >= self._divisions[i] and x < self._divisions[i+1]:
                idx = i
                rem = x - self._divisions[i]
                break
        if idx is None:
            raise ValueError("No matching division found in unit interval! How?")
        val = self.ranges[idx][0] + self._totalwidth * rem
        return val

    def shoot(self):
        rand = random.random()
        val = self._map_unit_to_val(rand)
        return val


class LogSampler(ContinuousSampler):
    "Randomly sample from an exponential distribution (i.e. uniformly on a log scale)."

    def __init__(self, low, high):
        self.low = float(low)
        self.high = float(high)

    def shoot(self):
        rand = random.random()
        logval = rand * math.log(self.high) + (1 - rand) * math.log(self.low)
        val = math.exp(logval)
        return val


class GaussianSampler(ContinuousSampler):
    "Randomly sample from a 1D Gaussian distribution."

    def __init__(self, mean, sigma):
        self.mean = float(mean)
        self.sigma = float(sigma)

    def shoot(self):
        return random.gauss(self.mean, self.sigma)


class InvSampler(ContinuousSampler):
    "Randomly sample from a 1/x distribution."

    def __init__(self, low, high):
        self.low = float(low)
        self.high = float(high)

    def shoot(self):
        invx = random.uniform(1/self.high, 1/self.low) #< limit inversion not actually necessary
        return 1./invx


########################################


class TH1Sampler(ContinuousSampler):
    "Randomly sample from a 1D ROOT histogram."

    def __init__(self, *args):
        self.hist = TH1(*args)
        if self.hist.GetEntries() < 1:
            raise Exception("Histogram %s is EMPTY! Cannot sample" % self.hist.GetName())

    def shoot(self):
        return self.hist.GetRandom()


########################################


## Discrete sequence samplers

class DiscreteSampler(Sampler):
    "Base class for samplers from lists of discrete values"
    pass


class RandomSeqSampler(DiscreteSampler):
    "Uniformly random sample from a list of values."

    def __init__(self, *args):
        if len(args) == 1:
            self.sequence = args[0]
        else:
            self.sequence = args

    def shoot(self):
        return random.choice(self.sequence)
# Alias:
RndmSeq = RandomSeqSampler


class CyclicSeqSampler(DiscreteSampler):
    "Sequentially sample from a list of values, returning to the beginning once exhausted."

    def __init__(self, *args):
        if len(args) == 1:
            self.sequence = args[0]
        else:
            self.sequence = args
        self.index = 0

    def shoot(self):
        self.index = (self.index + 1) % len(self.sequence)
        return self.sequence[self.index]
## Alias:
Sequence = CyclicSeqSampler


########################################


## Convenience function for sampler-making from Python literals

def mksampler(x):
    """
    Automatically cast the provided object to a sampler type. This is used
    extensively inside the particle and position samplers, so that the user
    can pass in a primitive type like a number or list and it will be
    treated as if the more verbose sampler constructors had been called.

    Behaviour:
     - if x can be called, i.e. x() is valid, we just return x;
     - a Python list (square brackets) will be converted to a continuous
       UniformSampler or DisjointUniformSampler;
     - a Python tuple (round brackets/parentheses) will be treated
       as a discrete CyclicSeqSampler;
     - a Python set (curly brackets/braces) will be treated
       as a discrete RandomSeqSampler;
     - otherwise a ConstSampler will be created from x, so that x is
       returned when the sampler is called.
    """
    if hasattr(x, "__call__"):
        return x
    elif type(x) is list:
        # NB: disjoint ranges can be given as nested lists, e.g. [(1,2), (4,5)]
        if len(x) == 2 and type(x[0]) in (int,float) and type(x[1]) in (int,float):
            #print "MKSAMPLER: Casting %s to UniformSampler" % str(x)
            return UniformSampler(*x)
        elif len(x) > 2 or (len(x) > 0 and type(x[0]) not in (int,float)):
            #print "MKSAMPLER: Casting %s to DisjointUniformSampler" % str(x)
            return DisjointUniformSampler(x)
        if len(x) < 2:
            raise Exception("Supplied list could not be converted to a continuous sampler")
    elif type(x) is tuple:
        #print "MKSAMPLER: Casting %s to CyclicSeqSampler" % str(x)
        return CyclicSeqSampler(*x)
    elif type(x) is set:
        #print "MKSAMPLER: Casting %s to RandomSeqSampler" % str(x)
        return RandomSeqSampler(*x)
    else:
        #print "MKSAMPLER: Casting %s to ConstSampler" % str(x)
        return ConstSampler(x)


########################################


## Beam-spot (origin vertex) sampling

class PosSampler(Sampler):
    """
    Sampler of position 3-vectors, for modelling a beamspot.
    """

    def __init__(self, x, y, z, t=0):
        self.x = x
        self.y = y
        self.z = z
        self.t = t

    @property
    def x(self):
        "x position sampler"
        return self._x
    @x.setter
    def x(self, x):
        self._x = mksampler(x)

    @property
    def y(self):
        "y position sampler"
        return self._y
    @y.setter
    def y(self, y):
        self._y = mksampler(y)

    @property
    def z(self):
        "z position sampler"
        return self._z
    @z.setter
    def z(self, z):
        self._z = mksampler(z)

    @property
    def t(self):
        "Time sampler"
        return self._t
    @t.setter
    def t(self, t):
        self._t = mksampler(t)

    def shoot(self):
        x = self.x()
        y = self.y()
        z = self.z()
        t = self.t()
        #print "POS =", x, y, z, t
        return ROOT.TLorentzVector(x, y, z, t)


# TODO: Make a 3-Gaussian BeamspotSampler


## Momentum sampling

class MomSampler(Sampler):
    """
    Base class for four-momentum sampling.

    There are many ways to unambiguously specify four-momenta. Not all are sensible/useful,
    though. The following are implemented here:
     * M,px,py,pz
     * E,M,phi,eta
     * E,M,phi,y
     * E,M,phi,theta
     * pT,M,phi,eta
     * pT,M,phi,y
     * pT,M,phi,theta

    Possibly the following (not yet implemented) could be useful: let us know if you
    need one of these:
     * E,px,py,pz
     * pT,E,M,phi
    """
    pass


class NullMomSampler(MomSampler):
    "A momentum sampler which just returns a null vector with the given mass."

    def __init__(self, mass=0.0):
        self.mass = mass

    @property
    def mass(self):
        "Mass sampler"
        return self._m
    @mass.setter
    def mass(self, x):
        self._m = mksampler(x)

    def shoot(self):
        v4 = ROOT.TLorentzVector(0, 0, 0, self.mass)
        return v4


class MXYZSampler(MomSampler):
    "Create a 4-momentum vector from mass, px, py, pz distributions/samplers."

    def __init__(self, px, py, pz, mass=0.0):
        self.mass = mass
        self.px = px
        self.py = py
        self.pz = pz

    @property
    def mass(self):
        "Mass sampler"
        return self._m
    @mass.setter
    def mass(self, x):
        self._m = mksampler(x)

    @property
    def px(self):
        "px sampler"
        return self._px
    @px.setter
    def px(self, x):
        self._px = mksampler(x)

    @property
    def py(self):
        "py sampler"
        return self._py
    @py.setter
    def py(self, x):
        self._py = mksampler(x)

    @property
    def pz(self):
        "pz sampler"
        return self._pz
    @pz.setter
    def pz(self, x):
        self._pz = mksampler(x)

    def shoot(self):
        m = self.mass()
        px = self.px()
        py = self.py()
        pz = self.pz()
        e = math.sqrt(px**2 + py**2 + pz**2 + m**2)
        v4 = ROOT.TLorentzVector(px, py, pz, e)
        return v4


class EEtaMPhiSampler(MomSampler):
    "Create a 4-momentum vector from E, eta, m and phi distributions/samplers."

    # TODO: ensure that E >= m!

    def __init__(self, energy, eta, mass=0.0, phi=[0, TWOPI]):
        self.energy = energy
        self.eta = eta
        self.mass = mass
        self.phi = phi

    @property
    def energy(self):
        "Energy sampler"
        return self._e
    @energy.setter
    def energy(self, x):
        self._e = mksampler(x)

    @property
    def eta(self):
        "Pseudorapidity sampler"
        return self._eta
    @eta.setter
    def eta(self, x):
        self._eta = mksampler(x)

    @property
    def mass(self):
        "Mass sampler"
        return self._m
    @mass.setter
    def mass(self, x):
        self._m = mksampler(x)

    @property
    def phi(self):
        "Azimuthal angle sampler"
        return self._phi
    @phi.setter
    def phi(self, x):
        self._phi = mksampler(x)

    def shoot(self):
        """
        eta = - ln(tan(theta/2)) / 2
        => theta = 2 atan( exp(-eta) )
        """
        eta = self.eta()
        theta = 2 * math.atan(math.exp(-eta))
        e = self.energy()
        m = self.mass()
        p = math.sqrt( e**2 - m**2 )
        pz = p * math.cos(theta)
        pt = p * math.sin(theta)
        phi = self.phi()
        px = pt * math.cos(phi)
        py = pt * math.sin(phi)
        v4 = ROOT.TLorentzVector(px, py, pz, e)
        return v4


class ERapMPhiSampler(MomSampler):
    "Create a 4-momentum vector from E, y, m and phi distributions."

    # TODO: ensure that E >= m!

    def __init__(self, energy, rap, mass=0.0, phi=[0, TWOPI]):
        self.energy = energy
        self.rap = rap
        self.mass = mass
        self.phi = phi

    @property
    def energy(self):
        "Energy sampler"
        return self._e
    @energy.setter
    def energy(self, x):
        self._e = mksampler(x)

    @property
    def rap(self):
        "Rapidity sampler"
        return self._rap
    @rap.setter
    def rap(self, x):
        self._rap = mksampler(x)

    @property
    def mass(self):
        "Mass sampler"
        return self._m
    @mass.setter
    def mass(self, x):
        self._m = mksampler(x)

    @property
    def phi(self):
        "Azimuthal angle sampler"
        return self._phi
    @phi.setter
    def phi(self, x):
        self._phi = mksampler(x)

    def shoot(self):
        """
        y = 0.5 * ln((E+pz)/(E-pz))
        -> (E^2 - pz^2) exp(2y) = (E+pz)^2
         & (E^2 - pz^2) exp(-2y) = (E-pz)^2
        -> E = sqrt(pt^2 + m^2) cosh(y)
        -> pz = sqrt(pt^2 + m^2) sinh(y)
        -> sqrt(pt^2 + m^2) = E / cosh(y)
        """
        e = self.energy()
        y = self.rap()
        sqrt_pt2_m2 = e / math.cosh(y)
        pz = sqrt_pt2_m2 * math.sinh(y)
        m = self.mass()
        pt = math.sqrt( sqrt_pt2_m2**2 - m**2 )
        phi = self.phi()
        px = pt * math.cos(phi)
        py = pt * math.sin(phi)
        v4 = ROOT.TLorentzVector(px, py, pz, e)
        return v4


class EThetaMPhiSampler(MomSampler):
    "Create a 4-momentum vector from E, theta, m and phi distributions/samplers."

    # TODO: ensure that E >= m!

    def __init__(self, energy, theta, mass=0.0, phi=[0, TWOPI]):
        self.energy = energy
        self.theta = theta
        self.mass = mass
        self.phi = phi

    @property
    def energy(self):
        "Energy sampler"
        return self._e
    @energy.setter
    def energy(self, x):
        self._e = mksampler(x)

    @property
    def theta(self):
        "Polar angle sampler"
        return self._theta
    @theta.setter
    def theta(self, x):
        self._theta = mksampler(x)

    @property
    def mass(self):
        "Mass sampler"
        return self._m
    @mass.setter
    def mass(self, x):
        self._m = mksampler(x)

    @property
    def phi(self):
        "Azimuthal angle sampler"
        return self._phi
    @phi.setter
    def phi(self, x):
        self._phi = mksampler(x)

    def shoot(self):
        """
        p = sqrt(e^2 - m^2)
        pz = p cos(theta)
        pt = p sin(theta)
        """
        e = self.energy()
        m = self.mass()
        p = math.sqrt( e**2 - m**2 )
        theta = self.theta()
        pz = p * math.cos(theta)
        pt = p * math.sin(theta)
        phi = self.phi()
        px = pt * math.cos(phi)
        py = pt * math.sin(phi)
        v4 = ROOT.TLorentzVector(px, py, pz, e)
        return v4


class PtEtaMPhiSampler(MomSampler):
    "Create a 4-momentum vector from pt, eta, m and phi distributions/samplers."

    def __init__(self, pt, eta, mass=0.0, phi=[0, TWOPI]):
        self.pt = pt
        self.eta = eta
        self.mass = mass
        self.phi = phi

    @property
    def pt(self):
        "Transverse momentum sampler"
        return self._pt
    @pt.setter
    def pt(self, x):
        self._pt = mksampler(x)

    @property
    def eta(self):
        "Pseudorapidity sampler"
        return self._eta
    @eta.setter
    def eta(self, x):
        self._eta = mksampler(x)

    @property
    def mass(self):
        "Mass sampler"
        return self._m
    @mass.setter
    def mass(self, x):
        self._m = mksampler(x)

    @property
    def phi(self):
        "Azimuthal angle sampler"
        return self._phi
    @phi.setter
    def phi(self, x):
        self._phi = mksampler(x)

    def shoot(self):
        """
        eta = - ln(tan(theta/2)) / 2
        => theta = 2 atan( exp(-eta) )
        """
        eta = self.eta()
        theta = 2 * math.atan(math.exp(-eta))
        pt = self.pt()
        p = pt / math.sin(theta)
        phi = self.phi()
        px = pt * math.cos(phi)
        py = pt * math.sin(phi)
        pz = p * math.cos(theta)
        m = self.mass()
        e = math.sqrt( p**2 + m**2 )
        v4 = ROOT.TLorentzVector(px, py, pz, e)
        return v4


class PtRapMPhiSampler(MomSampler):
    "Create a 4-momentum vector from pt, y, m and phi distributions/samplers."

    def __init__(self, pt, rap, mass=0.0, phi=[0, TWOPI]):
        self.pt = pt
        self.rap = rap
        self.mass = mass
        self.phi = phi

    @property
    def pt(self):
        "Transverse momentum sampler"
        return self._pt
    @pt.setter
    def pt(self, x):
        self._pt = mksampler(x)

    @property
    def rap(self):
        "Rapidity sampler"
        return self._rap
    @rap.setter
    def rap(self, x):
        self._rap = mksampler(x)

    @property
    def mass(self):
        "Mass sampler"
        return self._m
    @mass.setter
    def mass(self, x):
        self._m = mksampler(x)

    @property
    def phi(self):
        "Azimuthal angle sampler"
        return self._phi
    @phi.setter
    def phi(self, x):
        self._phi = mksampler(x)

    def shoot(self):
        """
        y = 0.5 * ln((E+pz)/(E-pz))
        -> (E^2 - pz^2) exp(2y) = (E+pz)^2
         & (E^2 - pz^2) exp(-2y) = (E-pz)^2
        -> E = sqrt(pt^2 + m^2) cosh(y)
        -> pz = sqrt(pt^2 + m^2) sinh(y)
        -> sqrt(pt^2 + m^2) = E / cosh(y)
        """
        pt = self.pt()
        assert pt >= 0
        m = self.mass()
        assert m >= 0
        sqrt_pt2_m2 = math.sqrt( pt**2 + m**2 )
        y = self.rap()
        e = sqrt_pt2_m2 * math.cosh(y)
        pz = sqrt_pt2_m2 * math.sinh(y)
        phi = self.phi()
        px = pt * math.cos(phi)
        py = pt * math.sin(phi)
        v4 = ROOT.TLorentzVector(px, py, pz, e)
        return v4


class PtThetaMPhiSampler(MomSampler):
    "Create a 4-momentum vector from pt, theta, m and phi distributions/samplers."

    def __init__(self, pt, theta, mass=0.0, phi=[0, TWOPI]):
        self.pt = pt
        self.theta = theta
        self.mass = mass
        self.phi = phi

    @property
    def pt(self):
        "Transverse momentum sampler"
        return self._pt
    @pt.setter
    def pt(self, x):
        self._pt = mksampler(x)

    @property
    def theta(self):
        "Polar angle sampler"
        return self._theta
    @theta.setter
    def theta(self, x):
        self._theta = mksampler(x)

    @property
    def mass(self):
        "Mass sampler"
        return self._m
    @mass.setter
    def mass(self, x):
        self._m = mksampler(x)

    @property
    def phi(self):
        "Azimuthal angle sampler"
        return self._phi
    @phi.setter
    def phi(self, x):
        self._phi = mksampler(x)

    def shoot(self):
        """
        p = pt / math.sin(theta)
        pz = p cos(theta)
        pt = p sin(theta)
        E = sqrt(p^2 + m^2)
        """
        theta = self.theta()
        pt = self.pt()
        p = pt / math.sin(theta)
        phi = self.phi()
        px = pt * math.cos(phi)
        py = pt * math.sin(phi)
        pz = p * math.cos(theta)
        m = self.mass()
        e = math.sqrt( p**2 + m**2 )
        v4 = ROOT.TLorentzVector(px, py, pz, e)
        return v4


# TODO: add the missing ways to specify/sample 4-momenta


###########################################################


## Combined samplers returning a particle configuration


## A default dictionary of particle masses (in MeV)
MASSES = { 22   :    0.0, # photon
           11   :    0.5, # electron
           12   :    0.0, # nu_e
           13   :  105.7, # muon
           14   :    0.0, # nu_mu
           15   : 1777.8, # tau
           16   :    0.0, # nu_tau
           2212 :  938.0, # proton
           2112 :  940.0, # neutron
           111  :  135.0, # pi0
           211  :  140.0, # pi+-
           221  :  547.0, # eta
           321  :  494.0, # K+-
           311  :  598.0  # K0
           }


class SampledParticle(object):
    """
    A particle object for use as a return value from the particle samplers.
    """
    def __init__(self, pid=None, mom=ROOT.TLorentzVector(0,0,0,0), pos=ROOT.TLorentzVector(0,0,0,0)):
        """
        Constructor/initializer: PID is the (int) PDG particle ID code
        of this particle, mom is its momentum 4-vector, and pos is
        the vertex 4-position (both as ROOT.TLorentzVector, in MeV).
        """
        self.pid = pid
        self.mom = mom
        self.pos = pos
        self.mass = None


class ParticleSampler(Sampler):
    """
    A simple N-independent-particle sampler.
    """

    def __init__(self, pid=999,
                 mom=NullMomSampler(),
                 n=1,
                 pos=PosSampler(0, 0, 0)):
        self.pid = pid
        self.mom = mom
        self.n = n
        self.pos = pos
        self.massdict = MASSES
        self.mass_override = True

    @property
    def pid(self):
        "Particle ID code sampler"
        return self._pid
    @pid.setter
    def pid(self, x):
        self._pid = mksampler(x)

    @property
    def n(self):
        "Particle number sampler"
        return self._n
    @n.setter
    def n(self, x):
        self._n = mksampler(x)

    def shoot(self):
        "Return a vector of sampled particles"
        numparticles = self.n()
        rtn = []
        for i in range(numparticles):
            ## Sample the particle ID and create a particle
            pid = self.pid()
            p = SampledParticle(pid)
            ## Pass mass info to the v4 sampler and set same generated mass
            if self.mass_override and abs(pid) in self.massdict:
                m = self.massdict[abs(pid)]
                self.mom.mass = m
                p.mass = m
            # TODO: Should the particle generated_mass be set from the sampler by default?
            ## Sample momentum and vertex positions into the particle
            p.mom = self.mom()
            p.pos = self.pos()
            ## Add particle to output list
            rtn.append(p)
        return rtn
