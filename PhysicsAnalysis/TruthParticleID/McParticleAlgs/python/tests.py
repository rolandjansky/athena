# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from McParticleTools.McParticleToolsConf import EtaPtFilterTool
from McParticleTools.McParticleToolsConf import OldSpclMcFilterTool
from McParticleTools.McParticleToolsConf import NoopFilterTool
from McParticleTools.McParticleToolsConf import TruthParticleCnvTool
from McParticleTools.McParticleToolsConf import TruthIsolationTool

from McParticleAlgs.McParticleAlgsConf import TruthParticleBuilder
class test_McAodBuilder( TruthParticleBuilder ):

    __slots__ = ( )

    def __init__( self,
                  name = "McAodBuilder" ):
        super( test_McAodBuilder, self ).__init__( name )
        return

    def setDefaults(cls, handle):

        if not hasattr(handle, 'FilterTool'):
            
            ##
            ## Default configuration of the filter tool
            ## [ESD-McEventCollection --> AOD-McEventCollection]
            ##
            handle.FilterTool = EtaPtFilterTool()
            handle.FilterTool.McEvents = "GEN_EVENT"
            handle.FilterTool.McEventsOutput = "GEN_AOD"
            pass

        if not hasattr(handle, 'CnvTool'):

            ##
            ## Configuration of the converter tool
            ## [AOD-McEventCollection --> TruthParticleContainer]
            ##
            handle.CnvTool = TruthParticleCnvTool()
            pass
        return

    pass # test_McAodBuilder

from McParticleTools.McParticleToolsConf import GenAodValidationTool
from McParticleTools.McParticleToolsConf import SpclMcValidationTool
from McParticleTools.McParticleToolsConf import HepMcWriterTool
from McParticleAlgs.McParticleAlgsConf   import McAodValidationAlg

class test_McAodValidation( McAodValidationAlg ):

    __slots__ = ( )

    def __init__( self, name = "McAodValidation" ):
        super( test_McAodValidation, self ).__init__( name )
        return
    
    def setDefaults(cls, handle):

        # declare the sub-tools (will be easier with ToolHandleArrayProperty)
        handle.ValidationTools = [
            "GenAodValidationTool/GenAodValidation",
            "SpclMcValidationTool/SpclMcValidation",
            ]

        if not hasattr(handle, "GenAodValidation"):
            genAodValidation = GenAodValidationTool( "GenAodValidation" )
            genAodValidation.RefMcEvents = "GEN_EVENT"
            genAodValidation += HepMcWriterTool("RefHepMcWriterTool")
            genAodValidation += HepMcWriterTool("CheckHepMcWriterTool")
            
            handle += genAodValidation
        return

    pass # test_McAodValidation

from McParticleTools.McParticleToolsConf import McVtxFilterTool
from McParticleTools.McParticleToolsConf import TruthParticleCnvTool
from McParticleAlgs.McParticleAlgsConf   import McAodFilter

class test_McAodFilter( McAodFilter ):

    __slots__ = ( )

    def __init__( self, name = "McAodFilter" ):
        super( test_McAodFilter, self ).__init__( name )
        return

    def setDefaults(cls, handle):

        handle.DoGenAod = True
        handle.McVtxFilterTool = McVtxFilterTool()
        handle.McVtxFilterTool.McEvents = "GEN_EVENT"
        handle.McVtxFilterTool.McEventsOutput = "MyGEN_EVENT"
        handle.McVtxFilterTool.DoSignalProcessVtx = True
        
        handle.DoTruthParticles = True
        handle.TruthParticleCnvTool = TruthParticleCnvTool()
        handle.TruthParticleCnvTool.McEvents = "MyGEN_EVENT"
        handle.TruthParticleCnvTool.TruthParticlesOutput = "MySpclMC"

        return

    pass # test_McAodFilter

not_yet = """        
## -- Generator helper
from Pythia_i.Pythia_iConf import Pythia
from Herwig_i.Herwig_iConf import Herwig
from Herwig_i.Herwig_iConf import Jimmy

def makeGenEvents( genName    = "Pythia",
                   genProcess = "ttbar",
                   cfgGenName = "EvGen" ):
    Little helper method to provide a quick way to produce Mc-events
    
    if genName.lower() not in [ "pythia", "jimmy", "herwig" ]:
        raise RuntimeError, "Unknown GENERATOR [%s]" % str(genName)
    if genProcess not in ["ttbar", "HiggsTo4Leptons", "Z+j"]:
        raise RuntimeError, "Unknown PROCESS [%s]" % str(genProcess)

    if genName.lower() == "pythia":
        genAlg = Pythia(cfgGenName)
        if genProcess == "Z+j":
            genAlg.PythiaCommand = [ "pysubs msel 13",
                                     "pysubs ckin 3 18.",
                                     "pypars mstp 43 2" ]
        elif genProcess == "ttbar":
            # semi-leptonic decay of a ttbar pair (lepton=e,mu)
            genAlg.PythiaCommand = [
                "pysubs msel 6",
                "pysubs ckin 3 18.",
                "pypars mstp 43 2",
                # Customisation of subprocess generation
                "pysubs msub 81 1",    # qqbar -> QQbar
                "pysubs msub 82 1",    # gq -> QQbar
                "pysubs msub 96 1",    # SemiHard QCD 2 -> 2
                # top
                "pydat3 mdme 41 1 0", # gt
                "pydat3 mdme 42 1 0", # gamma t
                "pydat3 mdme 43 1 0", # Z0 t
                "pydat3 mdme 44 1 0", # W+ d
                "pydat3 mdme 45 1 0", # W+ s
                "pydat3 mdme 46 1 1", # W+ b
                "pydat3 mdme 47 1 0", # W+ b'
                "pydat3 mdme 48 1 0", # h0 t
                "pydat3 mdme 49 1 0", # H+ b
                "pydat3 mdme 50 1 0", # ~chi_10 ~t_1
                "pydat3 mdme 51 1 0", # ~chi_20 ~t_1
                "pydat3 mdme 52 1 0", # ~chi_30 ~t_1
                "pydat3 mdme 53 1 0", # ~chi_40 ~t_1
                "pydat3 mdme 54 1 0", # ~g ~t_1
                "pydat3 mdme 55 1 0", # ~gravitino ~t_1
                # W
                "pydat3 mdme 190 1 2", # dbar u
                "pydat3 mdme 191 1 2", # dbar c
                "pydat3 mdme 192 1 2", # dbar t
                "pydat3 mdme 193 1 0", # dbar t'
                "pydat3 mdme 194 1 2", # sbar u
                "pydat3 mdme 195 1 2", # sbar c
                "pydat3 mdme 196 1 2", # sbar t
                "pydat3 mdme 197 1 0", # sbar t'
                "pydat3 mdme 198 1 2", # bbar u
                "pydat3 mdme 199 1 2", # bbar c
                "pydat3 mdme 200 1 2", # bbar t
                "pydat3 mdme 201 1 0", # bbar t'
                "pydat3 mdme 202 1 0", # b'bar u
                "pydat3 mdme 203 1 0", # b'bar c
                "pydat3 mdme 204 1 0", # b'bar t
                "pydat3 mdme 205 1 0", # b'bar t'
                "pydat3 mdme 206 1 0", # e+ nu_e
                "pydat3 mdme 207 1 3", # mu+ nu_mu
                "pydat3 mdme 208 1 0", # tau+ nu_tau
                "pydat3 mdme 209 1 0"  # tau'+ nu'_tau
                ]
        elif genProcess == "HiggsTo4Leptons":
            genAlg.PythiaCommand = [
                # Higgs mass set:
                "pydat2 pmas 25 1 150.",
                # Select Higgs production
                "pysubs msel 16",
                # Higgs Decays
                "pydat3 mdme 210 1 0",
                "pydat3 mdme 211 1 0",
                "pydat3 mdme 212 1 0",
                "pydat3 mdme 213 1 0",
                "pydat3 mdme 214 1 0",
                "pydat3 mdme 215 1 0",
                "pydat3 mdme 218 1 0",
                "pydat3 mdme 219 1 0",
                "pydat3 mdme 220 1 0",
                "pydat3 mdme 222 1 0",
                "pydat3 mdme 223 1 0",
                "pydat3 mdme 224 1 0",
                "pydat3 mdme 225 1 1",
                "pydat3 mdme 226 1 0",
                
                # Z Decays
                "pydat3 mdme 174 1 0",
                "pydat3 mdme 175 1 0",
                "pydat3 mdme 176 1 0",
                "pydat3 mdme 177 1 0",
                "pydat3 mdme 178 1 0",
                "pydat3 mdme 179 1 0",
                "pydat3 mdme 180 1 0",
                "pydat3 mdme 181 1 0",
                "pydat3 mdme 182 1 1",
                "pydat3 mdme 183 1 0",
                "pydat3 mdme 184 1 1",
                "pydat3 mdme 185 1 0",
                "pydat3 mdme 186 1 0",
                "pydat3 mdme 187 1 0",
                "pydat3 mdme 188 1 0",
                "pydat3 mdme 189 1 0"
                ]
        else:
            raise RuntimeError, "Unknown PROCESS [%s]" % str(genProcess)

        # Some ISR,FSR,MultipleInteractions and Hadronization parameters
        genAlg.PythiaCommand += [
            "pypars mstp 61 1", # ISR
            "pypars mstp 71 1", # FSR
            "pypars mstp 81 1", # MultInt 
            "pypars mstp 111 1", # Hadronization
            ]
        
        # Pythia listing documentation level (0-1-2)
        Pythia.PythiaCommand += [ "pypars mstp 125 2" ]

        
    elif genName.lower() == "jimmy":
        genAlg = Jimmy(cfgGenName)
        if genProcess == "Z+j":
            genAlg.JimmyCommand = [
                "iproc 12150",
                "modpdf 10042",
                "autpdf HWLHAPDF",
                "msflag 1",
                "jmbug 0"
                ]
        elif genProcess == "ttbar":
            import commands
            osCmd = commands.getoutput( "get_files inparmMcAtNlo.dat" )
            # osCmd = commands.getoutput( "get_files tt.events" )
            genAlg.JimmyCommand = [
                "iproc mcatnlo",
                "modpdf 10042", "autpdf HWLHAPDF",
                "msflag 1",
                "jmbug 0"
                ]
            del osCmd

        else:
            raise RuntimeError, "Unknown PROCESS [%s]" % std(genProcess)

    elif genName.lower() == "Herwig":
        genAlg = Herwig(cfgGenName)
        if genProcess == "Z+j":
            genAlg.HerwigCommand = [
                "iproc 2150",
                "modpdf 10042",
                "autpdf HWLHAPDF"
                ]

        elif genProcess == "ttbar":
            genAlg.HerwigCommand = [
                "iproc 1700",
                "modpdf 10042",
                "autpdf HWLHAPDF"
                ]

        else:
            raise RuntimeError, "Unknown PROCESS [%s]" % str(genProcess)

    else:
        raise RuntimeError, "Unknown GENERATOR [%s]" % str(genName)

    return genAlg
"""
