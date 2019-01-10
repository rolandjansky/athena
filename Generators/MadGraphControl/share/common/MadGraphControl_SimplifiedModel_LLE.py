include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' ) 

#
rangeID = lambda start, end: range(start, end + 1)

def SquarksB(): #up to sbottoms
    sqs =[]
    for i in rangeID(1,5):
        sqs += ["%d"%(1000000+i), "-%d"%(1000000+i)  ] + ["%d"%(2000000+i), "-%d"%(2000000+i)  ]
    return sqs
#
EnableFilters = False
 
if "LLE" not in runArgs.jobConfig[0]:
    raise RuntimeError("This File is made for RPV-LLE models.")

evgenConfig.keywords += ["simplifiedModel","RPV"]
evgenConfig.contact  = [ "johannes.josef.junggeburth@cern.ch" ]

#ReadIn the Parameters
JobOptionsFile = runArgs.jobConfig[0][runArgs.jobConfig[0].find("MC15.") : ]
JobArguments   = JobOptionsFile.strip(".py").split("_")
Model          = JobArguments[2]
NLSPMass       = int(JobArguments[3])

if len(JobArguments) > 5:
   LSPMass=int(JobArguments[4])
   decaytype = JobArguments[5]
else:
   LSPMass=-1
   decaytype=JobArguments[4]

if decaytype.endswith("LLE12k"):
    evgenConfig.keywords += ["Lambda121","Lambda122"]
elif decaytype.endswith("LLEi33"):
    evgenConfig.keywords += ["Lambda133","Lambda233"]
else:
    raise RuntimeError("Unknown RPV coupling.")

#Allow in Pythia8 off shell bosons
genSeq.Pythia8.Commands += ["25:mMin = 0.2"] # Higgs
genSeq.Pythia8.Commands += ["24:mMin = 0.2"] # Z
genSeq.Pythia8.Commands += ["23:mMin = 0.2"] # W

# sanity check for masses
if LSPMass > NLSPMass:
    raise RuntimeError("The given LSP Mass %i is greater than the NLSP mass %i"%(LSPMass,NLSPMass))

if LSPMass > -1:
    masses ['1000022' ] = LSPMass
    genSeq.Pythia8.Commands += ["1000022:mMin = "+str(LSPMass-1)]

# number of jets
njets = 2
### Wino triplet model where all 3 production modes are made in 
### One sample --> Reduces the amount of DSIDs needed and simplifies
### things if we'd like to request more grid-points
if  Model.startswith("Wino"):
    gentype = "Wino" 
    masses ['1000023' ] = NLSPMass
    masses ['1000024' ] = NLSPMass
    evgenConfig.keywords += ['gaugino', 'neutralino']
    process='''
define killpartx1 = susystrong h01 h2 h3 h- h+
define killpartx1n2 = susystrong   

generate p p > x1+ x1-        / killpartx1 @1
add process p p > x1+ x1- j   / killpartx1 @2
add process p p > x1+ x1- j j / killpartx1 @3

add process p p > x1+ n2     / killpartx1n2 @4
add process p p > x1- n2     / killpartx1n2 @5
add process p p > x1+ n2 j   / killpartx1n2 @6

add process p p > x1- n2 j   / killpartx1n2 @7
add process p p > x1+ n2 j j / killpartx1n2 @8
add process p p > x1- n2 j j / killpartx1n2 @9
'''
    
    ##############################################################
    ## Direct RPV decays of the chargino                         #
    ##    chinoone -> nu_{i} l_{j} nu_{k} <-- forbidden for wino #
    ##                                        due to RH slepton  #
    ##                                        in mediator        #
    ##    chinoone -> nu_{j/i} nu_{i/j} l_{k}                    #
    ##    chinoone -> l_{i}^{+-} l_{j}^{-+} l_{k}^{+}            #
    ##############################################################
    
    
  
    
    
    evgenConfig.description = "RPV-LLE simplified model considering a mass degnenerated Wino triplet. The chinoone/ninotwo can be produced in the pp collisions. "
    if not "direct" in decaytype: evgenConfig.description += "They then undergo a prompt decay into a W-boson or a Z/H boson and the ninoone via RPC coulings."
    else:   evgenConfig.description += "The produced gauginos decay then directly via non-zero RPV-LLE terms into charged leptons and neutrinos"
    if decaytype == "LLE12k": evgenConfig.description += "The LSP decays via non zero Lambda_12k into two charged leptons and one neutrino. m_{C1/N2} = %s GeV, m_N1 = %s GeV"%(masses['1000024'],masses['1000022'])
    elif decaytype == "LLEi33": evgenConfig.description += "The LSP decays via non zero Lambda_i33 into two charged leptons and one neutrino. m_{C1/N2} = %s GeV, m_N1 = %s GeV"%(masses['1000024'],masses['1000022'])
    if njets>0:
        genSeq.Pythia8.Commands += [ "Merging:Process = guess" ]
    
 
### Chagino model split into production modes
elif Model.startswith("C1C1"):
    masses ['1000024' ] = NLSPMass
    gentype="C1C1" if LSPMass > -1 else "WW"
    
    evgenConfig.keywords += ['gaugino','neutralino']
    process='''
define killpartx1 = susystrong h01 h2 h3 h- h+
generate p p > x1+ x1- / killpartx1 @1
add process p p > x1+ x1- j / killpartx1 @2
add process p p > x1+ x1- j j / killpartx1 @3
'''    
    if decaytype == "LLE12k":
        evgenConfig.description = 'RPV-LLE simplified model. ~chi1+~chi1- production, decay into W + LSP. LSP decays via non zero Lambda_12k. m_C1 = %s GeV, m_N1 = %s GeV'%(masses['1000024'],masses['1000022'])
    elif decaytype=="directLLE12k":
        evgenConfig.description = 'RPV-LLE simplified model. ~chi1+~chi1- production. The charginos undergo directly a RPV deccay into three charged leptons or two neutrinos and one charge lepton via non zero Lambda_12k. For this model the mass of the chargino is m_C1 = %s GeV'%(masses['1000024'])
    elif decaytype == "LLEi33":
        evgenConfig.description = 'RPV-LLE simplified model. ~chi1+~chi1- production, decay into W + LSP. LSP decays via non zero Lambda_i33. m_C1 = %s GeV, m_N1 = %s GeV'%(masses['1000024'],masses['1000022'])
    elif decaytype=="directLLEi33":
        evgenConfig.description = 'RPV-LLE simplified model. ~chi1+~chi1- production. The charginos undergo directly a RPV deccay into three charged leptons or two neutrinos and one charge lepton via non zero Lambda_i33. For this model the mass of the chargino is m_C1 = %s GeV'%(masses['1000024'])
 
    if njets>0: genSeq.Pythia8.Commands += [ "Merging:Process = pp>{x1+,1000024}{x1-,-1000024}",
                                     "1000024:spinType = 1" ]

# Chargino - neutralino2 production 
elif Model.startswith("C1N2"):
    gentype = Model if LSPMass > -1 else "WW"
    masses ['1000023' ] = NLSPMass    
    masses ['1000024' ] = NLSPMass
    
    evgenConfig.keywords += ['gaugino','neutralino']
    process='''
define killpartx1n2 = susystrong   
generate p p > x1+ n2 / killpartx1n2 @1
add process p p > x1- n2  / killpartx1n2 @2
add process p p > x1+ n2 j / killpartx1n2 @3
add process p p > x1- n2 j / killpartx1n2 @4
add process p p > x1+ n2 j j / killpartx1n2 @5
add process p p > x1- n2 j j / killpartx1n2 @6
'''   
    Boson = "Z boson" if Model == "C1N2z" else "Higgs boson"        
    if decaytype == "LLE12k": evgenConfig.description = "RPV-LLE simplified model considering ~chi1+- ~chi2 production. The ~chi1+- decay into a W+- and the LSP, while the ~chi2 decays into a " + Boson + " and the LSP .LSP decays via non zero Lambda_12k into two charged leptons and one neutrino. m_{C1/N2} = %s GeV, m_N1 = %s GeV"%(masses['1000024'],masses['1000022'])
    elif decaytype == "directLLE12k": evgenConfig.description = "RPV-LLE simplified model considering ~chi1+- ~chi2 production. Both produced gauginos directly undergo a RPV-decay into leptons and neutrinos. The considered RPV terms lead to light-lepton final-states only. The mass of the gauginos has been choosen to be %s GeV"%(masses['1000024'])
    elif decaytype == "LLEi33": evgenConfig.description = "RPV-LLE simplified model considering ~chi1+- ~chi2 production. The ~chi1+- decay into a W+- and the LSP, while the ~chi2 decays into a " + Boson + " and the LSP .LSP decays via non zero Lambda_i33 into two charged leptons and one neutrino. m_{C1/N2} = %s GeV, m_N1 = %s GeV"%(masses['1000024'],masses['1000022'])
    elif decaytype == "directLLEi33": evgenConfig.description = "RPV-LLE simplified model considering ~chi1+- ~chi2 production. Both produced gauginos directly undergo a RPV-decay into leptons and neutrinos. The considered RPV terms lead to tau-rich final states. The mass of the gauginos has been choosen to be %s GeV"%(masses['1000024'])
    
    if njets>0:
        genSeq.Pythia8.Commands += [ "Merging:Process = pp>{x1+,1000024}{n2,1000023}",
                                     "Merging:Process = pp>{x1-,-1000024}{n2,1000023}",
                                      "1000024:spinType = 1", "1000023:spinType = 1" ]
# LV: slepton/sneutrino decays
elif Model.startswith("LV"):    
    process=""
    ### Avoid key errors risen by MadGraph control
    for sq in SquarksB(): masses[sq] = runArgs.ecmEnergy *2/3
    #Set the slepton/sneutrino masses to the NLSP mass
    for i in rangeID(11,16): masses [ '10000%i'%(i) ] = NLSPMass
    
    evgenConfig.keywords += ['sneutrino', 'neutralino', 'slepton']
     ### Define the generator description
    gentype = Model
    if LSPMass > -1:
        process='''
define cl+ = e+ mu+ ta+
define cl- = e- mu- ta-
define lscl+ = el+ mul+ ta1+
define lscl- = el- mul- ta1-

generate p p > lscl+ susyv, lscl+ > cl+ n1, susyv > vl n1 @ 1
add process p p > lscl- susyv~, lscl- > cl- n1, susyv~ > vl~ n1 @ 2
add process p p > lscl- lscl+, lscl- > cl- n1, lscl+ > cl+ n1 @ 3
add process p p > susyv susyv~, susyv > vl n1, susyv~ > vl~ n1 @ 4

add process p p > lscl+ susyv j, lscl+ > cl+ n1, susyv > vl n1 @ 5
add process p p > lscl- susyv~ j, lscl- > cl- n1, susyv~ > vl~ n1 @ 6
add process p p > lscl- lscl+ j, lscl- > cl- n1, lscl+ > cl+ n1 @ 7
add process p p > susyv susyv~ j, susyv > vl n1, susyv~ > vl~ n1 @ 8

add process p p > lscl+ susyv j j, lscl+ > cl+ n1, susyv > vl n1 @ 9
add process p p > lscl- susyv~ j j, lscl- > cl- n1, susyv~ > vl~ n1 @ 10
add process p p > lscl- lscl+ j j, lscl- > cl- n1, lscl+ > cl+ n1 @ 11
add process p p > susyv susyv~ j j, susyv > vl n1, susyv~ > vl~ n1 @ 12
'''    

        if njets>0:
            genSeq.Pythia8.Commands += [ "Merging:Process = pp>{sve,1000012}{sve~,-1000012}"  , 
                                         "Merging:Process = pp>{svm,1000014}{svm~,-1000014}"  , 
                                         "Merging:Process = pp>{svt,1000016}{svt~,-1000016}", 
                                    
                                         "Merging:Process = pp>{el-,1000011}{el+,-1000011}"  , 
                                         "Merging:Process = pp>{mul-,1000013}{mul+,-1000013}"  , 
                                         "Merging:Process = pp>{ta1-,1000015}{ta1+,-1000015}"  , 
                                       
                                         "Merging:Process = pp>{sve,1000012}{el+,-1000011}"  , 
                                         "Merging:Process = pp>{svm,1000014}{mul+,-1000013}"  , 
                                         "Merging:Process = pp>{svt,1000016}{ta1+,-1000015}"  , 
                                     
                                         "Merging:Process = pp>{sve~,-1000012}{el-,1000011}"  , 
                                         "Merging:Process = pp>{svm~,-1000014}{mul-,1000013}"  , 
                                         "Merging:Process = pp>{svt~,-1000016}{ta1-,1000015}"  ,
                                     ]

       
        if decaytype == "LLE12k": evgenConfig.description = 'RPV-LLE simplified model. Sneutrino-sneutrino slepton-slepton  and sneutrino-slepton pair production. The produced sleptons decay into leptons and the lightest neutralinowhich is the LSP. The latter decays via non zero Lambda_12k coupling into two charged leptons and a neutrino. m_SNU = %s GeV, m_N1 = %s GeV'%(masses['1000012'],masses['1000022'])
        elif decaytype == "LLEi33":evgenConfig.description = 'RPV-LLE simplified model. Sneutrino-sneutrino slepton-slepton  and sneutrino-slepton pair production. The produced sleptons decay into leptons and the lightest neutralinowhich is the LSP. The latter decays via non zero Lambda_i33 coupling into two charged leptons and a neutrino. m_SNU = %s GeV, m_N1 = %s GeV'%(masses['1000012'],masses['1000022'])
        else: raise RuntimeError("Unkown decay type "+decaytype)
        
    else:
        ## Allowed RPV decay modes of the sleptons 
        ###     slep_{L}^{i} -> lep_{k} + nu_{j}
        ###     slep_{L}^{j} -> lep_{k} + nu_{i}
        ###     slep_{R}^{k} -> lep_{j/i} + nu_{i/j}
        ###     snu^{i}      -> lep_{k} + lep_{j}           
        if decaytype == "directLLE12k":
            for i in rangeID(12,14): masses [ '10000%i'%(i) ] = NLSPMass
            ### Since lambda_{121} & lambda_{122} are symmtetric under the exchange of i & k only the first two generations can contribute to this model
            evgenConfig.description = "RPV-LLE simplified model considering slepton production with m_SNU = %s GeV. The sneutrinos direclty undergo a RPV decay into a charged ee/emu/mumu lepton pair. The charged sleptons decay directly into lepton + neutrino"%(NLSPMass)  
            process = '''
define sneuv   = sve svm 
define sneuv~  = sve~ svm~    
define lscl+   = mul+ el+
define lscl-   = mul- el-
'''
            if njets>0: genSeq.Pythia8.Commands += [  "Merging:Process = pp>{sve,1000012}{sve~,-1000012}"  , 
                                                      "Merging:Process = pp>{svm,1000014}{svm~,-1000014}"  , 
                                                        
                                                      "Merging:Process = pp>{el-,1000011}{el+,-1000011}"  , 
                                                      "Merging:Process = pp>{mul-,1000013}{mul+,-1000013}"  , 
                                                           
                                                      "Merging:Process = pp>{sve,1000012}{el+,-1000011}"  , 
                                                      "Merging:Process = pp>{svm,1000014}{mul+,-1000013}"  , 
                                                         
                                                      "Merging:Process = pp>{sve~,-1000012}{el-,1000011}"  , 
                                                      "Merging:Process = pp>{svm~,-1000014}{mul-,1000012}"  ]
                                                     
        elif decaytype == "directLLEi33":
            evgenConfig.description = "RPV-LLE simplified model considering sneutrino-pair production with m_SNU = %s GeV. The sneutrinos direclty undergo a RPV decay into a charged etau/mu-tau/tau-tau lepton pair"%(NLSPMass)
            #In case of tau rich couplings we also have the tau sneutrino which can decay directly via RPV
            for i in [12,15,16]: masses [ '10000%i'%(i) ] = NLSPMass
            masses ['2000015'] = NLSPMass
            #  stau_{L}/smu_{L}/sel_{L}  --> nu + tau
            #  stau_{R}                  --> nu + mu/e
            #  smuon_{L}                 --> nu + tau
            #  selectron_{L}             --> nu + tau
            #  --> No need to generate 3 left-handed slepton generations. 
            #      The total cross-section can be reweighted by 3 to include the other two generations
            #  sneutrino (el)            --> tau+ tau-
            #  sneutrino (mu)            --> tau+ tau-
            #  --> Also summarize the first two sneutrino generations into one & reweight by 2
            #  sneutrino (tau)           --> tau+ e-
            #  sneutrino (tau)           --> tau+ mu-                        
            process = '''
define sneuv = sve svt
define sneuv~ = sve~ svt~
define lscl+ = el+ ta1+ ta2+
define lscl- = el- ta1- ta2-
'''
            
            if njets>0: genSeq.Pythia8.Commands += [ "Merging:Process = pp>{sve,1000012}{sve~,-1000012}"  , 
                                                     "Merging:Process = pp>{svt,1000016}{svt~,-1000016}", 
                                                     "Merging:Process = pp>{el-,1000011}{el+,-1000011}"  , 
                                                     
                                                     "Merging:Process = pp>{ta1-,1000015}{ta1+,-1000015}"  , 
                                                     "Merging:Process = pp>{ta2-,2000015}{ta2+,-2000015}"  , 
                                                     
                                                     "Merging:Process = pp>{sve~,-1000012}{el-,1000011}"  , 
                                                     "Merging:Process = pp>{svt~,-1000016}{ta1-,1000015}"  ,
                                                    ]
        
        else: raise RuntimeError("Unkown decay type "+decaytype)
        
        process +='''
generate p p > lscl+ sneuv         @ 1
add process p p > lscl- sneuv~     @ 2
add process p p > lscl- lscl+      @ 3
add process p p > sneuv sneuv~     @ 4

add process p p > lscl+ sneuv   j   @ 5
add process p p > lscl- sneuv~  j   @ 6
add process p p > lscl- lscl+   j   @ 7
add process p p > sneuv sneuv~  j   @ 8

add process p p > lscl+ sneuv   j j  @ 9
add process p p > lscl- sneuv~  j j  @ 10
add process p p > lscl- lscl+   j j  @ 11
add process p p > sneuv sneuv~  j j  @ 12
'''
       
       
    
#GLUINO-GLUINO
elif Model.startswith("GG"):
    if LSPMass > -1:
        masses ['1000022' ] = LSPMass
        genSeq.Pythia8.Commands += ["1000022:mMin = "+str(LSPMass-1)]
    else:
        raise RuntimeError("The gluino-gluino model assumes that NLSPs decay into the associated quark-pair and the neutralino. LSP mass must be defined for that model.")

    masses ['1000021' ] = NLSPMass    
    gentype="GG"
    evgenConfig.keywords += ['gluino','neutralino']
    process='''
define killPart = susyweak
define vetoOnShellPart = susysq susysq~
generate    p p > go go     / killpart $ vetoOnShellPart @1 
add process p p > go go j   / killpart $ vetoOnShellPart @2 
add process p p > go go j j / killpart $ vetoOnShellPart @3
'''

    if decaytype == "LLE12k":
        evgenConfig.description = 'RPV-LLE simplified model. ~gluino~gluino production, decay into qq + LSP. LSP decays via non zero Lambda_12k. m_glu = %s GeV, m_N1 = %s GeV. Squarks are decoupled.'%(masses['1000021'],masses['1000022'])
    elif decaytype == "LLEi33":
        evgenConfig.description = 'RPV-LLE simplified model. ~gluino~gluino production, decay into qq + LSP. LSP decays via non zero Lambda_i33. m_glu = %s GeV, m_N1 = %s GeV. Squarks are decoupled.'%(masses['1000021'],masses['1000022'])

    if njets>0:

        genSeq.Pythia8.Commands += [ "Merging:Process = pp>{go,1000021}{go,1000021}" ]

#SQUARK-ANTISQUARK or/and SQUARK-SQUARK (SS)
elif Model.startswith("SQSQ"):
    if LSPMass > -1:
        masses ['1000022' ] = LSPMass
        genSeq.Pythia8.Commands += ["1000022:mMin = "+str(LSPMass-1)]
    else:
        raise RuntimeError("The squark-antisquark model assumes that the NLSP decays into the associated quark and the neutralino. LSP mass must be defined for that model.")

    for q in SquarksB():
        masses[q] = NLSPMass

    gentype="SQSQ"
    evgenConfig.keywords += ['squark','neutralino']
    process='''
define susyq1gen  = ul ur dl dr
define susyq1gen~ = ul~ ur~ dl~ dr~
define killPart = susyweak
define vetoOnShellPart = go
generate    p p > susyq1gen  susyq1gen        / killPart $ vetoOnShellPart @1
add process p p > susyq1gen~ susyq1gen        / killPart $ vetoOnShellPart @2
'''
  
    if decaytype == "LLE12k" or decaytype == "LLEi33":
        evgenConfig.description = '(Anti)squark production, sq->q+LSP in RPV-%s simplified model, m_sq = %s GeV, m_N1 = %s GeV. Gluinos and stops are decoupled.'%(decaytype,masses[squarks[0]],masses['1000022'])
    else: raise RuntimeError("Unrecognised decay type %s"%(decaytype))

    if njets>0: genSeq.Pythia8.Commands += ["Merging:Process = pp>{ul,1000002}{ul~,-1000002}{ur,2000002}{ur~,-2000002}{dl,1000001}{dl~,-1000001}{dr,2000001}{dr~,-2000001"]
     
#SQUARK - GLUINO
elif Model.startswith("SG"):
    if LSPMass > -1:
        masses ['1000022' ] = LSPMass
        genSeq.Pythia8.Commands += ["1000022:mMin = "+str(LSPMass-1)]
    else:
        raise RuntimeError("The squark-gluino model assumes that the NLSP decays into the associated quark(s) and the neutralino. LSP mass must be defined for that model.")

    for q in SquarksB(): 
        masses[q] = NLSPMass
    masses ['1000021' ] = NLSPMass

    gentype="SG"
    evgenConfig.keywords += ['squark','gluino','neutralino']
    process ='''
define killPart = susyweak
define susylqb = susylq b1 b2
define susylqb~ = susylq~ b1~ b2~
generate    p p > go susylqb       / killPart @1
add process p p > go susylqb j     / killPart @2
add process p p > go susylqb j j   / killPart @3
add process p p > go susylqb~      / killPart @4
add process p p > go susylqb~ j    / killPart @5
add process p p > go susylqb~ j j  / killPart @6
'''
    if decaytype == "LLE12k" or decaytype == "LLEi33":
        evgenConfig.description = 'Squark-gluino production: go->qq+LSP and sq->q+LSP in RPV-%s simplified model, m_sq or m_go = %s GeV, m_N1 = %s GeV. Stops are decoupled.'%(decaytype, masses[squarks[0]], masses['1000022'])
    else:
        raise RuntimeError("Unrecognised decay type %s"%(decaytype))

    if njets>0:
        genSeq.Pythia8.Commands += ["Merging:Process = pp>{ul,1000002}{ul~,-1000002}{ur,2000002}{ur~,-2000002}{dl,1000001}{dl~,-1000001}{dr,2000001}{dr~,-2000001}{sl,1000003}{sl~,-1000003}{sr,2000003}{sr~,-2000003}{cl,1000004}{cl~,-1000004}{cr,2000004}{cr~,-2000004}{b1,1000005}{b1~,-1000005}{b2,2000005}{b2~,-2000005}{go,1000021}"]

# END
else:
    raise RuntimeError("Unrecognised production mode %s of input %s"%(Model, JobArguments[2]))

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )
