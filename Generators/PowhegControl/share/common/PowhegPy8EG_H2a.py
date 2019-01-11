#--------------------------------------------------------------
# Use LHE files as input
#--------------------------------------------------------------

# In 20.7.9.9.6, LHE merging means this is no longer needed
#evgenConfig.inputfilecheck = "TXT"

#--------------------------------------------------------------
# Modify the events.lhe, since Pythia doesn't like to decay the
# SM higgs to BSM products: 25 --> 35
#--------------------------------------------------------------

import os, sys, glob
for f in glob.glob("*.events"):
    infile = f
    f1 = open( infile )
    newfile = infile+'.temp'
    f2 = open(newfile,'w')
    for line in f1:
        if line.startswith('      25     1'):
            f2.write(line.replace('      25     1','      35     1'))
        else:
            f2.write(line)
    f1.close()
    f2.close()
    os.system('mv %s %s '%(infile, infile+'.old') )
    os.system('mv %s %s '%(newfile, infile) )

#--------------------------------------------------------------
# Defining the function to extract parameters
#--------------------------------------------------------------
dict_pdgIds = {}
dict_pdgIds["b"]   = 5
dict_pdgIds["mu"]  = 13
dict_pdgIds["tau"] = 15
dict_pdgIds["g"]   = 21
dict_pdgIds["y"]   = 22

def getParameters():
    import re

    #--- Read parts of the job option
    jonamelist = str(runArgs.jobConfig[0]).replace(".py", "").split("_")
    process = jonamelist[1]
    ma = float(jonamelist[3].split("a")[-1].replace("p", "."))
    decayChan = str(jonamelist[4])
    partFilter = None
    if len(jonamelist)>5:
        partFilter = str(jonamelist[5])

    #--- list of decays, e.g. [mu, tau] for 2mu2tau
    decayProducts = []
    for part in dict_pdgIds.keys():
        decay = re.findall("[1-4]%s" % part, decayChan)
        if len(decay)>0:
            decayProducts.append(decay[0][1:]) # remove the number in front of the letter

    return process, ma, decayChan, decayProducts, partFilter

#    MC15.999999.PowhegPy8EG_ggH_H125_a60a60_2mu2tau_[filtXXX].py
process, ma, decayChan, decayProducts, partFilter = getParameters()
print "Parameters: "
print process, ma, decayChan, decayProducts, partFilter

#--------------------------------------------------------------
# Pythia8 showering
#--------------------------------------------------------------
include('Pythia8_i/Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py')


#--------------------------------------------------------------
# Pythia8 main31 matching
#--------------------------------------------------------------
if process=="ggH":
    genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 2']
elif process=="VBF":
    genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 3']
elif process=="WpH":
    genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 3']
elif process=="WmH":
    genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 3']
elif process=="ZH":
    genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 3']
elif process=="ggZH":
    genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 2']


#--------------------------------------------------------------
# Higgs->aa at Pythia8
#--------------------------------------------------------------

genSeq.Pythia8.Commands += [
                            'Higgs:useBSM = on',
                            '35:m0 = 125',
                            '35:mWidth = 0.00407',
                            '35:doForceWidth = on',
                            '35:onMode = off',
                            '35:onIfMatch = 36 36', # h->aa
                            '36:onMode = off',
                            ]


#--------------------------------------------------------------
# a->XX at Pythia8
#--------------------------------------------------------------

if len(decayProducts)==1: # a->4X
    genSeq.Pythia8.Commands += [
                                '36:onIfAny = %d' % dict_pdgIds[decayProducts[0]], # decay a->XX
                                '36:m0 = %.1f' % ma, #scalar mass
                                '36:mMin = 0',
                                '36:tau0 = 0',
                                ]
elif len(decayProducts)==2: # a->2X2Y

    sign = -1
    for part in decayProducts:
        pdgId = dict_pdgIds[part]
        if pdgId==21 or pdgId==22:
            sign = 1

    genSeq.Pythia8.Commands += [
                                '36:oneChannel = 1 0.5 100 %d %d' % (dict_pdgIds[decayProducts[0]], sign*dict_pdgIds[decayProducts[0]]), #a->XX
                                '36:addChannel = 1 0.5 100 %d %d' % (dict_pdgIds[decayProducts[1]], sign*dict_pdgIds[decayProducts[1]]), #a->YY
                                '36:m0 %.1f' % ma, #scalar mass
                                '36:mMin %.1f' % (ma-0.5), #scalar mass
                                '36:mMax %.1f' % (ma+0.5), #scalar mass
                                '36:mWidth 0.01', # narrow width
                                '36:tau0 0', #scalarlife time
                                ]


#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.keywords    = [ "BSM", "Higgs", "BSMHiggs", "mH125"]

if process=="ggH" and (decayChan=="2b2mu" or decayChan=="2mu2b"):
    evgenConfig.description = "POWHEG+Pythia8 H+jet production with NNLOPS and the A14 tune, H->aa->bbmumu mh=125 GeV"
    evgenConfig.process     = "ggH H->aa->bbmumu"
    evgenConfig.contact     = [ 'christopher.hayes@cern.ch','ljiljana.morvaj@cern.ch' ]

if process=="VBF" and (decayChan=="2b2mu" or decayChan=="2mu2b"):
    evgenConfig.description = "POWHEG+Pythia8 VBF H production, H->aa->bbmumu mh=125 GeV"
    evgenConfig.process     = "VBF H->aa->bbmumu"
    evgenConfig.contact     = [ 'christopher.hayes@cern.ch','ljiljana.morvaj@cern.ch' ]

if process=="ggH" and decayChan=="4tau":
    evgenConfig.description = "POWHEG+Pythia8 H+jet production with NNLOPS and the A14 tune, H->aa->4tau mh=125 GeV"
    evgenConfig.process     = "ggH H->aa->4tau"
    evgenConfig.contact     = [ 'rachel.smith@cern.ch', 'huacheng.cai@cern.ch', 'roger.caminal.armadans@cern.ch' ]

if process=="ggH" and (decayChan=="2mu2tau" or decayChan=="2tau2mu"):
    evgenConfig.description = "POWHEG+Pythia8 H+jet production with NNLOPS and the A14 tune, H->aa->mumutautau mh=125 GeV"
    evgenConfig.process     = "ggH H->aa->mumutautau"
    evgenConfig.contact     = [ 'josefina.alconada@cern.ch' ]

if process=="ggH" and (decayChan=="2b2tau" or decayChan=="2tau2b"):
    evgenConfig.description = "POWHEG+Pythia8 H+jet production with NNLOPS and the A14 tune, H->aa->bbtautau mh=125 GeV"
    evgenConfig.process      = "ggH H->aa->bbtautau"
    evgenConfig.contact     = [ 'christopher.hayes@cern.ch','ljiljana.morvaj@cern.ch' ]

if process=="VBF" and (decayChan=="2b2tau" or decayChan=="2tau2b"):
    evgenConfig.description = "POWHEG+Pythia8 VBF H production, H->aa->bbtautau mh=125 GeV"
    evgenConfig.process      = "VBF H->aa->bbtautau"
    evgenConfig.contact     = [ 'christopher.hayes@cern.ch','ljiljana.morvaj@cern.ch' ]

if process=="ggH" and (decayChan=="2y2g" or decayChan=="2g2y"):
    evgenConfig.description = "POWHEG+Pythia8 H+jet production with NNLOPS and the A14 tune, H->aa->gamgamgg mh=125 GeV"
    evgenConfig.process     = "ggH H->aa->gamgamgg"
    evgenConfig.contact     = [ 'rubbo@cern.ch' ]

if process=="VBF" and (decayChan=="2y2g" or decayChan=="2g2y"):
    evgenConfig.description = "POWHEG+Pythia8 VBF H production, H->aa->gamgamgg mh=125 GeV"
    evgenConfig.process     = "VBF H->aa->gamgamgg"
    evgenConfig.contact     = [ 'rubbo@cern.ch' ]

if process=="ggH" and (decayChan=="4mu"):
    evgenConfig.description = "POWHEG+Pythia8 H+jet production with NNLOPS and the A14 tune, H->aa->4mu mh=125 GeV"
    evgenConfig.process     = "ggH H->aa->4mu"
    evgenConfig.contact     = [ 'roger.caminal.armadans@cern.ch' ]

if process=="WpH" and decayChan=="4b":
    evgenConfig.description = "POWHEG+MiNLO+Pythia8 H+W+jet->l+vbbbarbbbar production"
    evgenConfig.process     = "WpH, H->aa->4b, W->lv"
    evgenConfig.contact     = [ 'roger.caminal.armadans@cern.ch' ]

if process=="WmH" and decayChan=="4b":
    evgenConfig.description = "POWHEG+MiNLO+Pythia8 H+W-jet->l-vbbbarbbbar production"
    evgenConfig.process     = "WmH, H->aa->4b, W->lv"
    evgenConfig.contact     = [ 'roger.caminal.armadans@cern.ch' ]

if process=="ZH" and decayChan=="4b":
    evgenConfig.description = "POWHEG+MiNLO+Pythia8 H+Z+jet->l+l-bbbarbbbar production"
    evgenConfig.process     = "ZH, H->aa->4b, Z->ll"
    evgenConfig.contact     = [ 'roger.caminal.armadans@cern.ch' ]

if process=="ggZH" and decayChan=="4b":
    evgenConfig.description = "POWHEG+MiNLO+Pythia8 gg->H+Z+jet->l+l-bbbarbbbar production"
    evgenConfig.process     ="ggZH, H->2a->4b, Z->ll"
    evgenConfig.contact     = [ 'roger.caminal.armadans@cern.ch' ]


#--------------------------------------------------------------
# FILTERS (if needed)
#--------------------------------------------------------------
if partFilter=="filterXXYY" or partFilter=="filterlh" or partFilter=="filterll":
    if not hasattr(filtSeq, "XtoVVDecayFilter"):
        from GeneratorFilters.GeneratorFiltersConf import XtoVVDecayFilter
        filtSeq += XtoVVDecayFilter()
        if(partFilter=="filterlh" or partFilter=="filterll"):
          filtSeq+= XtoVVDecayFilter("tauscalarFilter")
#    ## Add this filter to the algs required to be successful for streaming
#    if "XtoVVDecayFilter" not in StreamEVGEN.RequireAlgs:
#        StreamEVGEN.RequireAlgs += ["XtoVVDecayFilter"]
    filtSeq.XtoVVDecayFilter.PDGGrandParent = 35
    filtSeq.XtoVVDecayFilter.PDGParent = 36
    filtSeq.XtoVVDecayFilter.StatusParent = 22
    print [dict_pdgIds[decayProducts[0]]], [dict_pdgIds[decayProducts[1]]]
    filtSeq.XtoVVDecayFilter.PDGChild1 = [dict_pdgIds[decayProducts[0]]]
    filtSeq.XtoVVDecayFilter.PDGChild2 = [dict_pdgIds[decayProducts[1]]]
    if(partFilter=="filterlh" or partFilter=="filterll"):
      filtSeq.tauscalarFilter.PDGGrandParent = 36
      filtSeq.tauscalarFilter.PDGParent = 15
      filtSeq.tauscalarFilter.StatusParent = 2
      filtSeq.tauscalarFilter.PDGChild1 = [11,13]
      if partFilter=="filterll":
        filtSeq.tauscalarFilter.PDGChild2 = [11,13]
      elif partFilter=="filterlh":
        filtSeq.tauscalarFilter.PDGChild2 = [111,130,211,221,223,310,311,321,323]

elif partFilter=="filter2taulep2tauhad":
    from GeneratorFilters.GeneratorFiltersConf import FourTauLepLepHadHadFilter
    filtSeq += FourTauLepLepHadHadFilter()
