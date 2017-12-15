# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
class Norm1:
  def __init__(self):
    from TrigT2CaloEgamma.RingerConstants import Section, Normalisation

    # this part config the TrigRingerPrepoc
    self.NormalisationRings = [Normalisation.EVENT,
                               Normalisation.EVENT,
                               Normalisation.EVENT,
                               Normalisation.EVENT,
                               Normalisation.EVENT,
                               Normalisation.EVENT,
                               Normalisation.EVENT];

    self.SectionRings = [Section.EM,
                         Section.EM,
                         Section.EM,
                         Section.EM,
                         Section.HAD,
                         Section.HAD,
                         Section.HAD];

    # Only change this if you know what you are doing
    # default configuration from RingerFex
    self.NRings = [8, 64, 8, 8, 4, 4, 4]

 
class Sequential:
  def __init__(self):
    from TrigT2CaloEgamma.RingerConstants import Section, Normalisation

    # this part config the TrigRingerPrepoc
    self.NormalisationRings = [Normalisation.SEQUENTIAL,
                               Normalisation.SEQUENTIAL,
                               Normalisation.SEQUENTIAL,
                               Normalisation.SEQUENTIAL,
                               Normalisation.SEQUENTIAL,
                               Normalisation.SEQUENTIAL,
                               Normalisation.SEQUENTIAL];

    self.SectionRings = [Section.EM,
                         Section.EM,
                         Section.EM,
                         Section.EM,
                         Section.HAD,
                         Section.HAD,
                         Section.HAD];

    # Only change this if you know what you are doing
    # default configuration from RingerFex
    self.NRings = [8, 64, 8, 8, 4, 4, 4]

 
