# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

class DecoratorFromDefault(object) :

  ## Define decorator name string
  name = 'fromDefault'

  def __init__( self, decorated ) :

    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

#   ATTENTION output file name must not be set in the input file otherwise there will be no log (i.e. stdout of the job)!!
#    self.decorated.add_parameter( 'outputLHE', 'ProphecyOTF.lhe',  desc='name of outputfile', parameter='outputfile' )
    self.decorated.add_parameter( 'nEvents_weighted', 10000000,     desc='nevents: number of weighted events', parameter='nevents' )
    self.decorated.add_parameter( 'nEvents', 10000,                desc='nunwevents: number of unweighted events', parameter='nunwevents' )
    self.decorated.add_parameter( 'WriteOutput', 1,                desc='do (not) write lhe event files: 1 (0)', parameter='lheoutput' )
    self.decorated.add_parameter( 'contrib', 1,                    desc='contrib: 1=best 2=IBA 3=Born' )
    self.decorated.add_parameter( 'qqcd', 1,                       desc='qqcd: 0=EW 1=EW+QCD 2=QCD corrections incl.' )
    self.decorated.add_parameter( 'qsoftcoll', 2,                  desc='qsoftcoll: 1=subtraction, 2=slicing' )
    self.decorated.add_parameter( 'channel', "e anti-e e anti-e",  desc='final state' )
    self.decorated.add_parameter( 'qrecomb', 0,                    desc='qrecomb: 0=no recomb., 1=photon recomb.' )
    self.decorated.add_parameter( 'invrecomb', 5,                  desc='recombination condition (for qrecomb=1)' )
    self.decorated.add_parameter( 'qhist', 0,                      desc='no histograms' )
    self.decorated.add_parameter( 'qrecombcolle', 0,               desc='1=recomb. electrons and photons inside the slicing cone, 0=do not recomb.' )
    self.decorated.add_parameter( 'mass_H', 125.5,                 desc='Higgs boson mass', parameter='mh' )
    self.decorated.add_parameter( 'alphaem_inv', 137.0359997,      desc='alpha(0)', parameter='1/alpha0' )
    self.decorated.add_parameter( 'alphas', 0.119,                 desc='strong coupling constant' )
    self.decorated.add_parameter( 'gfermi', 0.0000116637,          desc='Fermi constant', parameter='gf' )
    self.decorated.add_parameter( 'mass_Z', 91.1876,               desc='Z boson mass', parameter='mz' )
    self.decorated.add_parameter( 'mass_W', 80.398,                desc='W boson mass', parameter='mw' )
    self.decorated.add_parameter( 'width_Z', 2.4952,               desc='on-shell Z-boson width (only to calculate pole mass)', parameter='gammaz' )
    self.decorated.add_parameter( 'width_W', 2.141,                desc='on-shell W-boson width (only to calculate pole mass)', parameter='gammaw' )
    self.decorated.add_parameter( 'mass_e', 0.00051099891,         desc='electron mass', parameter='me' )
    self.decorated.add_parameter( 'mass_mu', 0.105658367,          desc='muon mass', parameter='mmu' )
    self.decorated.add_parameter( 'mass_tau', 1.77684,             desc='tau mass', parameter='mtau' )
    self.decorated.add_parameter( 'mass_d', 0.190,                 desc='d-quark mass', parameter='md' )
    self.decorated.add_parameter( 'mass_u', 0.190,                 desc='u-quark mass', parameter='mu' )
    self.decorated.add_parameter( 'mass_s', 0.190,                 desc='s-quark mass', parameter='ms' )
    self.decorated.add_parameter( 'mass_c', 1.4,                   desc='c-quark mass', parameter='mc' )
    self.decorated.add_parameter( 'mass_b', 4.75,                  desc='b-quark mass', parameter='mb' )
    self.decorated.add_parameter( 'mass_t', 172.5,                 desc='t-quark mass', parameter='mt' )
    self.decorated.add_parameter( 'qsm4', 0,                       desc='0,1,2  4th fermion gen. off,NLO,NLO+improvements' )
    self.decorated.add_parameter( 'mass_l4', 600,                  desc='mass of  l in 4th fermion generation', parameter='ml4' )
    self.decorated.add_parameter( 'mass_n4', 600,                  desc='mass of nu in 4th fermion generation', parameter='mn4' )
    self.decorated.add_parameter( 'mass_d4', 600,                  desc='mass of  d in 4th fermion generation', parameter='md4' )
    self.decorated.add_parameter( 'mass_u4', 650,                  desc='mass of  u in 4th fermion generation', parameter='mu4' )
    self.decorated.add_parameter( 'random_seed', 0,                desc='use default random numbers', parameter='randomseed' )

  def finalise(self) :
    ## Force parameters to integer values
    [ setattr( self.decorated, parameter, int( getattr(self.decorated, parameter) ) ) for parameter in ('nEvents_weighted','nEvents','WriteOutput','contrib','qqcd','qsoftcoll','qrecomb','qrecombcolle','qsm4','random_seed') ]

    ## Fix integration parameters before printing list for user
    [ self.decorated.fix_parameter( parameter ) for parameter in ('WriteOutput','contrib','qqcd','qsoftcoll','qrecomb','qrecombcolle','qsm4','random_seed' ) ]


