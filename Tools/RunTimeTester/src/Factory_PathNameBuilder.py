# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Factory_BaseClass    import Factory
from PathNameBuilder      import PathNameBuilder
from RTTSException        import RTTCodingError
from exc2string2          import exc2string2

class PathNameBuilderFactory(Factory):
  def __init__(self, logger):
    Factory.__init__(self, logger, self.__class__.__name__)
    
    self.dict = {
      'project': PathNameBuilder
      }

    self.logger = logger
    
  def create(self, mode, argBag, hardCoded, legals, siteSpecificsObj): 

    try:
      pnb = self.dict[mode](argBag,
                            hardCoded,
                            legals,
                            siteSpecificsObj,
                            self.logger)
    except KeyError:
      msg = '%s unknown key %s, could not instantiate PathNameBuilder: '% (self.__class__.__name__,str(mode))
      self.logger.error(msg)
      raise RTTCodingError(msg)
    except Exception, e:
      msg = '%s Error constructing PathNameBuilder from key  %s.\nException: %s\nTraceback:\n%s '% (self.__class__.__name__,
                                                                                                    str(mode),
                                                                                                    str(e),
                                                                                                    exc2string2()
                                                                                                    )
      self.logger.error(msg)
      raise RTTCodingError(msg)
    else:
      return pnb
 


