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
    self.decorated.add_parameter( 'nEvents', 10000,                desc='Number of points to be generated/calculated.' )
    self.decorated.add_parameter( 'nskip',       0,                desc='Number of events to skip in the input LHE file (nskip>=0).' )
    self.decorated.add_parameter( 'runningMode', "unweighted",    desc='running mode. Possible choices: weighted/unweighted' )
    self.decorated.add_parameter( 'maxDiffWidth', 0,               desc='max. differential partial width. if > 0, no maximum search performed' )
    self.decorated.add_parameter( 'channel', "mmmm",               desc='Final state particles (eemm/eeee/mmmm)' )
    self.decorated.add_parameter( 'radCorr', "matched",            desc='Order of radiative corrections: born = LO decay, born level; exoal = includes NLO electroweak corrections; matched = NLOPS accuracy, NLO EW + multi-photon emission. Best prediction.' )
    self.decorated.add_parameter( 'random_seed', 0,                desc='use default random numbers' )
    self.decorated.add_parameter( 'higgsMass', 125,                desc='Higgs mass (GeV)' )
    self.decorated.add_parameter( 'weakCorr', 1,                   desc='0 = only QED corrections, 1 = full EW NLO.' )
    self.decorated.add_parameter( 'mpairCut', 0,                   desc='cut on lepton low mass pair' )
    self.decorated.add_parameter( 'eftAnalysis', 0,                desc='EFT analysis (0=no, >0 yes)' )
    self.decorated.add_parameter( 'D6_CP', 1,                      desc='CP of the D=6 coefficients: 1=even, 2=odd, 3=both' )
    self.decorated.add_parameter( 'basis', "higgs",                desc='choice of the basis: higgs, silh, warsaw')
    self.decorated.add_parameter( 'dim6DataCards', "dim6-check-user-v2.0.dat", desc='Dimension 6 parameter card' )
    self.decorated.add_parameter( 'dim6QuadEffects', 0,            desc='dim6 quad effects: 0 - inter only, 1 - quad included' )
    self.decorated.add_parameter( 'mode', "lhe",                   desc="'standalone' or 'lhe' mode" )
    self.decorated.add_parameter( 'outDir', ".",                   desc="output directory" )
    self.decorated.add_parameter( 'lheInput', "LHE.events",        desc='LHE input file' )
    self.decorated.add_parameter( 'lheOutput', "LHEout.events",    desc='LHE output event file' )


  def finalise(self) :
    ## Force parameters to integer values
    [ setattr( self.decorated, parameter, int( getattr(self.decorated, parameter) ) ) for parameter in ('nEvents', 'weakCorr', 'eftAnalysis', 'D6_CP', 'dim6QuadEffects','random_seed') ]

    ## Fix integration parameters before printing list for user
    ## [ self.decorated.fix_parameter( parameter ) for parameter in ('runningMode','maxDiffWidth','radCorr','weakCorr','mpairCut','eftAnalysis','D6_CP','random_seed' ) ]


