# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from CaloDetDescr.CaloDetDescrConf import CaloDepthTool

# provides generic configuration of CaloDepthTool

class CaloDepthToolBase ( CaloDepthTool )  :
    # explicit list of additional data member to catch typo
    __slots__ = ( )
    def __init__(self, name="CaloDepthToolBaseUNSET",depth="CDTBunset"):        

        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( name+'(Base)::__init__ depth='+depth )
        mlog.info('called with depth='+depth )        


        CaloDepthTool.__init__( self,name) # call base class constructor

        if name=="CaloDepthToolBaseUNSET":
            raise RuntimeError,"CaloDepthToolBase should not be called with default name"            

        if depth=="CDTBunset":
            mlog.info("depth should be explicitly set. OK only if copy")
            return

        if depth=="showerdefault":
            # get optimal version of shower parameterisation            
            from AthenaCommon.GlobalFlags import globalflags
            detdescrversion=globalflags.DetDescrVersion()
            if detdescrversion[0:4]=="Rome" or detdescrversion[0:3]=="DC1" or detdescrversion[0:3]=="DC2" or detdescrversion[0:9]=="ATLAS-DC3":
                depthchoice="egparam"
                mlog.info('picked showerdefault=egparam for detdescrversion='+detdescrversion)
            else:
                depthchoice="cscopt2"
                mlog.info('picked showerdefault='+depthchoice+' for detdescrversion='+detdescrversion)                
        else:
            depthchoice=depth

        mlog.info("Setting up CaloDepthTool/"+name+" with depth "+depthchoice)   

        # depthchoice should be one of 

        depthList=["egparam","cscopt","cscopt2","TBparam","entrance","middle","flat"]
        if not depthchoice in depthList:
            raise RuntimeError, "depth not among autorised value",depthchoice

        # assign property value
        self.DepthChoice=depthchoice


# factory : autogenerates the name
# usage :
# from CaloDetDescr.CaloDepthToolBase import CaloDepthToolFactory
# theTool=CaloDepthToolFactory(depth="showerdefault")
def CaloDepthToolFactory( depth ):
# build the tool name by appending  depth valure
   return CaloDepthToolBase( "CaloDepthTool"+depth, depth=depth ) 


#####
# now provide a few fully configured instances
# usage :
# from CaloDetDescr.CaloDepthToolBase import caloDepthToolshowerdefault
# theTool=caloDepthToolshowerdefault
# note that this is just an illustration redundant with the factory above
__all__ = [ 'caloDepthToolshowerdefault','caloDepthToolegparam','caloDepthToolTBparam','caloDepthToolcscopt','caloDepthToolcscopt2','caloDepthToolmiddle','caloDepthToolentrance','caloDepthToolflat' ] # and the others, etc ...
# the following facade will masquerade as this module in order to have
# a __getattr__ method at the module level


class _ModuleFacade( object ):
   def __init__( self, module ):
      self._module = module

   def __getattr__( self, name ):
       # see whether the name is known in the original module ...
       try:
           return getattr( self._module, name )
       except AttributeError:
           pass

       # ... guess not, instantiate name if known, then store it in real module
       if name == 'caloDepthToolshowerdefault':
           setattr( self._module, name, CaloDepthToolBase("CaloDepthToolshowerdefault",depth="showerdefault") )
       elif name == 'caloDepthToolegparam':
           setattr( self._module, name, CaloDepthToolBase("CaloDepthToolegparam",depth="egparam") )
       elif name == 'caloDepthToolTBparam':
           setattr( self._module, name, CaloDepthToolBase("CaloDepthToolTBparam",depth="TBparam") )
       elif name == 'caloDepthToolcscopt':
           setattr( self._module, name, CaloDepthToolBase("CaloDepthToolcscopt",depth="cscopt") )
       elif name == 'caloDepthToolcscopt2':
           setattr( self._module, name, CaloDepthToolBase("CaloDepthToolcscopt2",depth="cscopt2") )
       elif name == 'caloDepthToolmiddle':
           setattr( self._module, name, CaloDepthToolBase("CaloDepthToolmiddle",depth="middle") )
       elif name == 'caloDepthToolentrance':
           setattr( self._module, name, CaloDepthToolBase("CaloDepthToolentrance",depth="entrance") )
       elif name == 'caloDepthToolflat':
           setattr( self._module, name, CaloDepthToolBase("CaloDepthToolflat",depth="flat") )
       else:
           raise RuntimeError,("unknown name %s " % name)

       # now try again, will succeed if proper tool has been created in block above
       return getattr( self._module, name )

# install facade, be vigilant against circular refs 
import sys
sys.modules[ __name__ ] = _ModuleFacade( sys.modules[ __name__ ] )
del _ModuleFacade, sys
