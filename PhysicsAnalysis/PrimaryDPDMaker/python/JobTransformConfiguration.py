# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

############################################
#                                          #
#      JobTransformConfiguration.py        #
#                                          #
# Creation: David Cote (DESY), August 2008 #
############################################

__doc__ = """Set of functions to configure DPD from PATJobTransforms"""
    
def ApplyPrescale(arg):
    #Supported args are 'SET_StreamName_Value' where arg is a string and value is an integer
    if isinstance(arg,str):
        if arg.startswith('SET_'):
            name_val=arg[4:]
            try:
                i=name_val.rindex('_')
            except Exception:
                raise SyntaxError("ApplyPrescale arg needs to have the form 'SET_StreamName_val', arg= "+arg)
            name=name_val[:i]
            val=name_val[i+1:]
            try:
                prescale=int(val)
            except Exception:
                raise TypeError("ApplyPrescale val needs to be an integer: val= "+val)
        else:
            raise SyntaxError("ApplyPrescale arg needs to start with 'SET_'. arg= "+arg)
    else:
        raise TypeError("ApplyPrescale arg of %s has to be a string. "%type(arg)+arg)

    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 
    if MSMgr.StreamExists( name ):
        stream=MSMgr.GetStream( name )
        origVal=stream.GetPrescale()
        stream.SetPrescale( prescale )
        print ("INFO ApplyPrescale - SET prescale value %i to stream"%prescale,name,"(original value:",origVal,")")
    else:
        print ("WARNING ApplyPrescale - unable to get stream "+name+". No additional prescale applied.")
    return
