# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging

def makeGenEvents( genName    = "Pythia",
                   genProcess = "ttbar",
                   cfgGenName = "EvGen" ):
    """
    Little helper method to provide a quick way to produce Mc-events
    """
    msg = logging.getLogger( "McParticleTests" )
    
    if genName.lower() not in [ "pythia", "herwig" ]:
        raise RuntimeError, "Unknown GENERATOR [%s]" % str(genName)
    if genProcess not in ["ttbar", "HiggsTo4Leptons", "Z+j"]:
        raise RuntimeError, "Unknown PROCESS [%s]" % str(genProcess)

    if genName.lower() == "pythia":
        try:
            from Pythia_i.Pythia_iConf import Pythia
        except ImportError, err:
            msg.warning( "Could not load module Pythia_iConf" )
            raise err
        
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
        genAlg.PythiaCommand += [ "pypars mstp 125 2" ]

    elif genName.lower() == "herwig":
        try:
            from Herwig_i.Herwig_iConf  import Herwig
        except ImportError, err:
            msg.warning( "Could not load module Herwig_iConf" )
            raise err
        genAlg = Herwig(cfgGenName)
        if genProcess == "Z+j":
            genAlg.HerwigCommand = [
                "iproc 2150",
                "modpdf 10042",
                "autpdf HWLHAPDF"
                ]
        
        elif genProcess == "ttbar":
            genAlg.HerwigCommand = ["iproc 1706","modpdf 10042",
                                    "autpdf HWLHAPDF" ]
        else:
            raise RuntimeError, "Unknown PROCESS [%s]" % str(genProcess)
        pass
        
    else:
        raise RuntimeError, "Unknown GENERATOR [%s]" % str(genName)

    return genAlg

###------------------------------------------------

def doFloatValidation( refFileName, chkFileName ):
    """function to check 2 files, taking into account float rounding...
    """
    import math, os
    from TestTools.iobench import ScOutput
    
    print "## float validation :"
    print "## -ref: %s" % refFileName
    print "## -chk: %s" % chkFileName

    refFile = open( refFileName, "r" )
    chkFile = open( chkFileName, "r" )

    refFloatPrecision = refFile.readline()
    chkFloatPrecision = chkFile.readline()

    assert( refFloatPrecision == chkFloatPrecision )

    fltPrecision = float( refFloatPrecision.split( "=" )[1].replace("\n","") )
    # in a perfect world this should be: 1.19209e-07
    fltPrecision = [ 1.19209e-06 ] * 3 + [ 1.19209e-02 ] + [ 1.5e2 ]
    #fltPrecision = [1.5e-2, 1.5e-2, 1.5e-2, 1.5e-2, 1.5e-2]
    #fltPrecision = [ 1. ] * 5
    print "## float precision: %r" % fltPrecision
    
    out = []
    nErrors = 0
    for refLine,chkLine in zip( refFile, chkFile):
        if refLine.startswith( "#" ) and refLine != chkLine:
            out += [ "==> ERROR:",
                     " ref: %r" % refLine,
                     " chk: %r" % chkLine ]
            nErrors += 1
        if not refLine.startswith( "#" ):
            refFloats = [ float(f.strip()) for f in refLine.split(" ") ]
            chkFloats = [ float(f.strip()) for f in chkLine.split(" ") ]
            if len(chkFloats) != len(refFloats):
                out += [ "==> ERROR:",
                         " ref: %r" % refFloats,
                         " chk: %r" % chkFloats ]
                nErrors += 1
                continue
            for i in range(len(refFloats)):
                refFloat = refFloats[i]
                chkFloat = chkFloats[i]
                # delta = math.fabs(refFloat) - math.fabs(chkFloat)
                delta = (refFloat - chkFloat)/ (float(refFloat) or 1.)
                # delta = refFloat - chkFloat
                if math.fabs( delta ) > fltPrecision[i]:
                    out += ["==> ERROR:",
                            " ref: %r" % refLine,
                            " chk: %r" % chkLine,
                            " del: %r %i" % (delta,i)
                            ]
                    nErrors += 1
                    break
    if nErrors == 0:
        sc = 0
        print "==> (float) Validation [OK]"
    else:
        sc = 1
        print "## Errors during validation:",nErrors
        print "==> (float) Validation [ERROR]"
        
    return ScOutput( sc,os.linesep.join(out) )


###------------------------------------------------

def doTupleValidation( refFileName, chkFileName, diffFileName = None ):
    """function to check 2 tuple files
    """
    import math, os
    from TestTools.iobench import ScOutput

    if not diffFileName:
        diffFileName = os.path.join( os.path.dirname(refFileName),
                                     "diff."+os.path.basename(refFileName) )

    print "## tuple validation :"
    print "## -ref:  %s" % refFileName
    print "## -chk:  %s" % chkFileName
    print "## -diff: %s" % diffFileName

    import sys
    oldArgs = sys.argv
    sys.argv = sys.argv[:1] + ['-b'] + sys.argv[1:]

    print "## loading ROOT..."
    import ROOT
    print "## loading ROOT... [DONE]"
    sys.argv = oldArgs
    del oldArgs
    ROOT.gROOT.SetStyle("Plain")
    ROOT.gStyle.SetOptStat(111111)
    
    print "## opening tuple files..."
    refFile = ROOT.TFile( refFileName, "READ" )
    chkFile = ROOT.TFile( chkFileName, "READ" )
    print "## opening tuple files... [DONE]"

    refTree = refFile.Get( "hepmc" )
    chkTree = chkFile.Get( "hepmc" )
    nEntries = refTree.GetEntries()
    print "## ref tuple [%i] entries" % nEntries
    print "## chk tuple [%i] entries" % chkTree.GetEntries()
    assert( nEntries == chkTree.GetEntries() )

    from AthenaCommon.SystemOfUnits import MeV,GeV
    nBins = 1000
    xMin  = -2.*GeV
    xMax  = +2.*GeV
    histos = {}
    histos['px'] = ROOT.TH1D( 'diffPx', "[diff] Px",   nBins, xMin, xMax )
    histos['py'] = ROOT.TH1D( 'diffPy', "[diff] Py",   nBins, xMin, xMax )
    histos['pz'] = ROOT.TH1D( 'diffPz', "[diff] Pz",   nBins, xMin, xMax )
    histos['m' ] = ROOT.TH1D( 'diffM',  "[diff] Mass", nBins, xMin, xMax )
    histos['e' ] = ROOT.TH1D( 'diffEne',"[diff] Ene",  nBins, xMin, xMax )
    hKeys = histos.keys()
    hKeys.sort()
    
    for iEntry in range(nEntries):
        refTree.LoadTree(iEntry)
        nb = refTree.GetEntry(iEntry)
        if nb <= 0:
            print "## Could not load [ref] entry [%i] !!" % iEntry
            continue
        chkTree.LoadTree(iEntry)
        nb = chkTree.GetEntry(iEntry)
        if nb <= 0:
            print "## Could not load [chk] entry [%i] !!" % iEntry
            continue
        nParts = refTree.nParts
        assert(nParts == chkTree.nParts)
        diff = { 'px' : 0.,
                 'py' : 0.,
                 'pz' : 0.,
                 'm'  : 0.,
                 'e'  : 0. }
        for iPart in range(nParts):
            diff['px'] += ( refTree.px[iPart] - chkTree.px[iPart] )
            diff['py'] += ( refTree.py[iPart] - chkTree.py[iPart] )
            diff['pz'] += ( refTree.pz[iPart] - chkTree.pz[iPart] )
            diff['m' ] += ( refTree.m[iPart]  - chkTree.m[iPart]  )
            diff['e' ] += ( refTree.e[iPart]  - chkTree.e[iPart]  )
        for k in diff.keys():
            histos[k].Fill( diff[k], 1. )
                            
    out = []
    nErrors = 0
    c = ROOT.TCanvas('c_diff', 'Differences')
    c.Divide(2,3); i = 1
    for k in hKeys:
        c.cd(i).SetLogy()
        h = histos[k]
        h.SetLineColor( ROOT.kRed )
        h.SetLineWidth( 2 )
        h.Draw()
        i += 1
        data = [ h.GetName(),
                 h.GetMean(), h.GetRMS(),
                 h.GetBinContent(0),
                 h.GetBinContent(h.GetNbinsX()+1) ]
        if math.fabs(data[1]) > .1*GeV or \
           math.fabs(data[2]) > 1.*GeV or \
           data[3] >= 10. or \
           data[4] >= 10. :
            nErrors += 1
        outBeg = len(out)-1
        out += [ "",
                 "== [%s] ==" % data[0],
                 " Mean:   %f MeV" % data[1],
                 " RMS:    %f" % data[2],
                 " U-flow: %f" % data[3],
                 " O-flow: %f" % data[4]
                 ]
        print os.linesep.join( out[outBeg+1:len(out)] )
        pass
    c.Print( diffFileName.replace(".root", ".pdf"),
             "Landscape" )
    
    diffRootFile = ROOT.TFile( diffFileName, "RECREATE" )
    diffRootFile.cd()
    for h in histos.values():
        h.Write()
        pass
    diffRootFile.Close()
    
    if nErrors == 0:
        sc = 0
        print "==> (tuple) Validation [OK]"
    else:
        sc = 1
        print "## Errors during validation:",nErrors
        print "==> (tuple) Validation [ERROR]"
        
    return ScOutput( sc,os.linesep.join(out) )

###------------------------------------------------

def doMcAodTupleValidation( refFileName, chkFileName, diffFileName = None ):
    """function to check 2 mcaod tuple files
    """
    import math, os
    from TestTools.iobench import ScOutput

    if not diffFileName:
        diffFileName = os.path.join( os.path.dirname(refFileName),
                                     "diff."+os.path.basename(refFileName) )

    print "## tuple validation :"
    print "## -ref:  %s" % refFileName
    print "## -chk:  %s" % chkFileName
    print "## -diff: %s" % diffFileName

    import sys
    oldArgs = sys.argv
    sys.argv = sys.argv[:1] + ['-b'] + sys.argv[1:]

    print "## loading ROOT..."
    import ROOT
    print "## loading ROOT... [DONE]"
    sys.argv = oldArgs
    del oldArgs
    ROOT.gROOT.SetStyle("Plain")
    ROOT.gStyle.SetOptStat(111111)
    
    print "## opening tuple files..."
    refFile = ROOT.TFile( refFileName, "READ" )
    chkFile = ROOT.TFile( chkFileName, "READ" )
    print "## opening tuple files... [DONE]"

    refTree = refFile.Get( "mcaod" )
    chkTree = chkFile.Get( "mcaod" )
    nEntries = refTree.GetEntries()
    print "## ref tuple [%i] entries" % nEntries
    print "## chk tuple [%i] entries" % chkTree.GetEntries()
    assert( nEntries == chkTree.GetEntries() )

    from AthenaCommon.SystemOfUnits import MeV,GeV
    nBins = 1000
    xMin  = -0.2*MeV
    xMax  = +0.2*MeV
    histos = {}
    etCones = [ 'etcone%i' % i for i in [10,20,30,40,45,50,60,70] ]
    for etCone in etCones:
        histos[etCone] = ROOT.TH1D( etCone, "[diff] %s" % etCone,
                                    nBins, xMin, xMax )
    hKeys = histos.keys()
    hKeys.sort()
    
    for iEntry in range(nEntries):
        refTree.LoadTree(iEntry)
        nb = refTree.GetEntry(iEntry)
        if nb <= 0:
            print "## Could not load [ref] entry [%i] !!" % iEntry
            continue
        chkTree.LoadTree(iEntry)
        nb = chkTree.GetEntry(iEntry)
        if nb <= 0:
            print "## Could not load [chk] entry [%i] !!" % iEntry
            continue
        nParts = refTree.nParts
        assert(nParts == chkTree.nParts)
        diff = { 'etcone10' : 0.,
                 'etcone20' : 0.,
                 'etcone30' : 0.,
                 'etcone40' : 0.,
                 'etcone45' : 0.,
                 'etcone50' : 0.,
                 'etcone60' : 0.,
                 'etcone70' : 0.,
                 }
        for iPart in range(nParts):
            for k in diff.keys():
                diff[k] += ( getattr(refTree,k)[iPart] -
                             getattr(chkTree,k)[iPart] )
        for k in diff.keys():
            histos[k].Fill( diff[k], 1. )
                            
    out = []
    nErrors = 0
    canvas = []
    for k in hKeys:
        c = ROOT.TCanvas('c_diff_'+k, 'Differences [%s]' % k )
        canvas.append( c )
        c.cd(0)
        h = histos[k]
        h.SetLineColor( ROOT.kRed )
        h.SetLineWidth( 2 )
        h.Draw()
        data = [ h.GetName(),
                 h.GetMean(), h.GetRMS(),
                 h.GetBinContent(0),
                 h.GetBinContent(h.GetNbinsX()+1) ]
        if math.fabs(data[1]) > .01*MeV or \
           math.fabs(data[2]) > .05*MeV or \
           data[3] > 1. or \
           data[4] > 1. :
            nErrors += 1
        outBeg = len(out)-1
        out += [ "",
                 "== [%s] ==" % data[0],
                 " Mean:   %f MeV" % data[1],
                 " RMS:    %f" % data[2],
                 " U-flow: %f" % data[3],
                 " O-flow: %f" % data[4]
                 ]
        print os.linesep.join( out[outBeg+1:len(out)] )
        pass

    pdfFileName = diffFileName.replace(".root", '.ps' )
    pdfOrientation = "Landscape"
    
    canvas[0].Print( pdfFileName + "[", pdfOrientation )
    for c in canvas:
        c.Print( pdfFileName, pdfOrientation )
    canvas[0].Print( pdfFileName + "]", pdfOrientation )
    import commands
    sc,out = commands.getstatusoutput(
        "ps2pdf %s %s" % ( pdfFileName,
                           pdfFileName.replace( ".ps", ".pdf" ) )
        )
    if os.path.exists( pdfFileName ): os.remove( pdfFileName )
    
    diffRootFile = ROOT.TFile( diffFileName, "RECREATE" )
    diffRootFile.cd()
    for h in histos.values():
        h.Write()
        pass
    diffRootFile.Close()
    
    if nErrors == 0:
        sc = 0
        print "==> (tuple) Validation [OK]"
    else:
        sc = 1
        print "## Errors during validation:",nErrors
        print "==> (tuple) Validation [ERROR]"
        
    return ScOutput( sc,os.linesep.join(out) )


