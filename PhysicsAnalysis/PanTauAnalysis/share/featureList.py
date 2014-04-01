

# #########################################################################################
#
# New structured features --- setup variables
#
# #########################################################################################

Default_Values = {}
Default_Sigmas = {}

featureDefaultValue = {}
featureDefaultValueSmearingSigma = {}
featureType = {}


# #########################################################################################
#
# New structured features --- basic features from SeedBuilderAlg
#
# #########################################################################################

Basic_Name = "Basic"

Feature_Basic_Names = []
Feature_Basic_Names += ["NChargedEFO"]
Feature_Basic_Names += ["NQualChrgEFO"]
Feature_Basic_Names += ["NQualChrg2EFO"]
Feature_Basic_Names += ["NAllEFO"]
Feature_Basic_Names += ["NNeutralEFO"]
Feature_Basic_Names += ["NNeutral1EFO"]
Feature_Basic_Names += ["NNeutral2EFO"]
Feature_Basic_Names += ["NNeutCore1EFO"]
Feature_Basic_Names += ["NNeutCore2EFO"]
Feature_Basic_Names += ["NEmNeutEFO"]
Feature_Basic_Names += ["NHadNeutEFO"]
Feature_Basic_Names += ["NPi0NeutEFO"]
Feature_Basic_Names += ["NQualNeutEFO"]
Feature_Basic_Names += ["SumCharge"]
Feature_Basic_Names += ["SumChargeQualified"]
Feature_Basic_Names += ["SumChargeQualified2"]
Feature_Basic_Names += ["AbsSumCharge"]
Feature_Basic_Names += ["AbsSumChargeQualified"]
Feature_Basic_Names += ["AbsSumChargeQualified2"]

for iFeature in Feature_Basic_Names:

    curName = iFeature
    curFeature = Basic_Name + "_" + curName
    
    featureDefaultValue[curFeature] = -10.
    featureDefaultValueSmearingSigma[curFeature] = 0.1
    featureType[curFeature] = 'F'
#end loop over features in basic features



# #########################################################################################
#
# New structured features --- four momentum
#
# #########################################################################################

Momentum_Name = "Basic"

Momentum_Types = []
Momentum_Types += ["core"]
Momentum_Types += ["sum"]
Momentum_Types += ["ChargedEFOs"]
Momentum_Types += ["NeutralEFOs"]
Momentum_Types += ["Pi0NeutEFOs"]
Momentum_Types += ["Core10GeV"]
Momentum_Types += ["Core15GeV"]
Momentum_Types += ["Core20GeV"]
Momentum_Types += ["Core25GeV"]
Momentum_Types += ["Core10GevMax53"]
Momentum_Types += ["Core10GevMax54"]
Momentum_Types += ["Core10GevMax55"]
Momentum_Types += ["Core10GevMax57"]
Momentum_Types += ["Core10GevMax59"]
Momentum_Types += ["Core20GevMax53"]
Momentum_Types += ["Core20GevMax54"]
Momentum_Types += ["Core20GevMax55"]
Momentum_Types += ["Core20GevMax57"]
Momentum_Types += ["Core20GevMax59"]

Feature_Momentum_Names = []
#Feature_Momentum_Names += ["e"]
#Feature_Momentum_Names += ["et"]
Feature_Momentum_Names += ["pt"]
Feature_Momentum_Names += ["eta"]
Feature_Momentum_Names += ["phi"]
Feature_Momentum_Names += ["m"]

print("featureList.py: Adding momentum features...")
for iType in Momentum_Types:
    for iVar in Feature_Momentum_Names:
        
        curType = iType
        curName = iVar
        
        curFeature = Momentum_Name + "_" + curType + "_" + curName
        
        defaultValue = -9999
        smearValue = 500
        if iVar == "eta" or iVar == "phi":
            defaultValue = -7
            smearValue = 0.1
        if iVar == "m":
            defaultValue = -200
            smearValue = 10
        
        featureDefaultValue[curFeature] = defaultValue
        featureDefaultValueSmearingSigma[curFeature] = smearValue
        featureType[curFeature] = 'F'
        
        if curType == "ChargedEFOs" or curType == "NeutralEFOs" or curType == "Pi0NeutEFOs":
            featureType[curFeature] = 'V'
        
    #end loop over variables in momentum
#end loop over types of momentum



# #########################################################################################
#
# New structured features --- Reco Mode
#
# #########################################################################################

RecoMode_Types = []
RecoMode_Types += ["Loose"]
#RecoMode_Types += ["Medium"]
#RecoMode_Types += ["Tight"]

for iModeType in RecoMode_Types:
    
    curType = iModeType
    curFeature = "RecoMode" + curType
    
    featureDefaultValue[curFeature] = -4
    featureDefaultValueSmearingSigma[curFeature] = 0.1
    featureType[curFeature] = 'I'
#end loop over types for reco mode


# #########################################################################################
#
# New structured features --- single types
#
# #########################################################################################

Features_Prefix = "Eflow_"

EFO_Types = []
#EFO_Types += ["Charged"]
EFO_Types += ["QualChrg"]
#EFO_Types += ["QualChrg2"]
#EFO_Types += ["EmNeut"]
EFO_Types += ["Pi0Neut"]
EFO_Types += ["QualNeut"]
#EFO_Types += ["HadNeut"]
#EFO_Types += ["Neutral"]
#EFO_Types += ["Neutral1"]
#EFO_Types += ["Neutral2"]
#EFO_Types += ["NeutCore1"]
#EFO_Types += ["NeutCore2"]
#EFO_Types += ["All"]

SeedEtTypes = []
SeedEtTypes += ["EtAll"]
SeedEtTypes += ["EtQualEFOs"]
#SeedEtTypes += ["EtAll1GeV"]
#SeedEtTypes += ["EtAll2GeV"]
#SeedEtTypes += ["EtCore1GeV"]
#SeedEtTypes += ["EtCore2GeV"]

SeedETypes = []
SeedETypes += ["EAll"]
#SeedETypes += ["EAll1GeV"]
#SeedETypes += ["EAll2GeV"]
#SeedETypes += ["ECore1GeV"]
#SeedETypes += ["ECore2GeV"]


Feature_Type_Names = []
#######################
### Angles
#######################
Default_Values["Angle"] = -0.2
Default_Sigmas["Angle"] = 0.01
Feature_Type_Names += [ ["Angle", "1stTo2nd"] ]
Feature_Type_Names += [ ["Angle", "1stTo3rd"] ]
Feature_Type_Names += [ ["Angle", "1stToJetAxis"] ]
Feature_Type_Names += [ ["Angle", "2ndTo3rd"] ]
Feature_Type_Names += [ ["Angle", "2ndToJetAxis"] ]
Feature_Type_Names += [ ["Angle", "3rdToJetAxis"] ]
Feature_Type_Names += [ ["Angle", "Btw1213Planes"] ]
Feature_Type_Names += [ ["Angle", "MaxToJetAxis"] ]
Feature_Type_Names += [ ["Angle", "MeanValue123"] ]
Feature_Type_Names += [ ["Angle", "ToJetAxis"] ]

#######################
### Delta R
#######################
Default_Values["DeltaR"] = -0.2
Default_Sigmas["DeltaR"] = 0.01
Feature_Type_Names += [ ["DeltaR", "1stTo2nd" ] ]
Feature_Type_Names += [ ["DeltaR", "1stTo3rd" ] ]
Feature_Type_Names += [ ["DeltaR", "1stToJetAxis" ] ]
Feature_Type_Names += [ ["DeltaR", "2ndTo3rd" ] ]
Feature_Type_Names += [ ["DeltaR", "2ndToJetAxis" ] ]
Feature_Type_Names += [ ["DeltaR", "3rdToJetAxis" ] ]

Feature_Type_Names += [ ["DeltaR", "1stTo2nd_BDTSort" ] ]
Feature_Type_Names += [ ["DeltaR", "1stTo3rd_BDTSort" ] ]
Feature_Type_Names += [ ["DeltaR", "1stToJetAxis_BDTSort" ] ]
Feature_Type_Names += [ ["DeltaR", "2ndTo3rd_BDTSort" ] ]
Feature_Type_Names += [ ["DeltaR", "2ndToJetAxis_BDTSort" ] ]
Feature_Type_Names += [ ["DeltaR", "3rdToJetAxis_BDTSort" ] ]

Feature_Type_Names += [ ["DeltaR", "MaxToJetAxis" ] ]
Feature_Type_Names += [ ["DeltaR", "ToJetAxis" ] ]

#######################
### HLV
#######################
Default_Values["HLV"] = -9999.0
Default_Sigmas["HLV"] = 500.
Feature_Type_Names += [ [ "HLV", "1stE"] ]
Feature_Type_Names += [ [ "HLV", "1stEta"] ]
Feature_Type_Names += [ [ "HLV", "1stM"] ]
Feature_Type_Names += [ [ "HLV", "1stPhi"] ]
Feature_Type_Names += [ [ "HLV", "2ndE"] ]
Feature_Type_Names += [ [ "HLV", "2ndEta"] ]
Feature_Type_Names += [ [ "HLV", "2ndM"] ]
Feature_Type_Names += [ [ "HLV", "2ndPhi"] ]
Feature_Type_Names += [ [ "HLV", "3rdE"] ]
Feature_Type_Names += [ [ "HLV", "3rdEta"] ]
Feature_Type_Names += [ [ "HLV", "3rdM"] ]
Feature_Type_Names += [ [ "HLV", "3rdPhi"] ]
Feature_Type_Names += [ [ "HLV", "SumE"] ]
Feature_Type_Names += [ [ "HLV", "SumEta"] ]
Feature_Type_Names += [ [ "HLV", "SumM"] ]
Feature_Type_Names += [ [ "HLV", "SumPhi"] ]

Feature_Type_Names += [ [ "HLV", "1stE_BDTSort"] ]
Feature_Type_Names += [ [ "HLV", "1stEta_BDTSort"] ]
Feature_Type_Names += [ [ "HLV", "1stM_BDTSort"] ]
Feature_Type_Names += [ [ "HLV", "1stPhi_BDTSort"] ]
Feature_Type_Names += [ [ "HLV", "2ndE_BDTSort"] ]
Feature_Type_Names += [ [ "HLV", "2ndEta_BDTSort"] ]
Feature_Type_Names += [ [ "HLV", "2ndM_BDTSort"] ]
Feature_Type_Names += [ [ "HLV", "2ndPhi_BDTSort"] ]
Feature_Type_Names += [ [ "HLV", "3rdE_BDTSort"] ]
Feature_Type_Names += [ [ "HLV", "3rdEta_BDTSort"] ]
Feature_Type_Names += [ [ "HLV", "3rdM_BDTSort"] ]
Feature_Type_Names += [ [ "HLV", "3rdPhi_BDTSort"] ]

#######################
### Isolation
#######################
Default_Values["Isolation"] = -0.2
Default_Sigmas["Isolation"] = 0.01
Feature_Type_Names += [ ["Isolation", "EtIn01OverEtIn02"] ]
Feature_Type_Names += [ ["Isolation", "EtIn01OverEtIn04"] ]
for etType in SeedEtTypes:
    Feature_Type_Names += [ ["Isolation", "EtIn01Over" + etType] ]
    Feature_Type_Names += [ ["Isolation", "EtIn02Over" + etType] ]
    Feature_Type_Names += [ ["Isolation", "EtIn03Over" + etType] ]
    Feature_Type_Names += [ ["Isolation", "EtIn04Over" + etType] ]
    Feature_Type_Names += [ ["Isolation", "EtIn02To04Over" + etType] ]
    Feature_Type_Names += [ ["Isolation", "EtIn00To02Over" + etType] ]
#


#######################
### JetMoments                                      ??? Variable range ???
#######################
Default_Values["JetMoment"] = -0.2
Default_Sigmas["JetMoment"] = 0.01
Feature_Type_Names += [ ["JetMoment", "EAngle"] ]
Feature_Type_Names += [ ["JetMoment", "EDR"] ]
Feature_Type_Names += [ ["JetMoment", "EDR2"] ]
Feature_Type_Names += [ ["JetMoment", "EDRprime"] ]
Feature_Type_Names += [ ["JetMoment", "EDRxTotalE"] ]
Feature_Type_Names += [ ["JetMoment", "EtAngle"] ]
Feature_Type_Names += [ ["JetMoment", "EtDR"] ]
Feature_Type_Names += [ ["JetMoment", "EtDR2"] ]
Feature_Type_Names += [ ["JetMoment", "EtDRprime"] ]
Feature_Type_Names += [ ["JetMoment", "EtDRxTotalEt"] ]

#######################
### Means
#######################
Default_Values["Mean"] = -0.2
Default_Sigmas["Mean"] = 0.01
for etType in SeedEtTypes:
    Feature_Type_Names += [ ["Mean", "Et_Wrt" + etType] ]
    Feature_Type_Names += [ ["Mean", "DRToJetAxis_Wrt" + etType] ]
    Feature_Type_Names += [ ["Mean", "DRToLeading_Wrt" + etType] ]
#
for eType in SeedETypes:
    Feature_Type_Names += [ ["Mean", "E_Wrt" + eType] ]
    Feature_Type_Names += [ ["Mean", "DRToJetAxis_Wrt" + eType] ]
    Feature_Type_Names += [ ["Mean", "DRToLeading_Wrt" + eType] ]
#

#######################
### Numbers
#######################
Default_Values["Num"] = -5.0
Default_Sigmas["Num"] = 0.2
Feature_Type_Names += [ ["Num", "EFOIn00To01"] ]
Feature_Type_Names += [ ["Num", "EFOIn01To02"] ]
Feature_Type_Names += [ ["Num", "EFOIn02To03"] ]
Feature_Type_Names += [ ["Num", "EFOIn03To04"] ]

#######################
### Ratios
#######################
Default_Values["Ratio"] = -0.2
Default_Sigmas["Ratio"] = 0.01
Feature_Type_Names += [ ["Ratio", "Log1stEtOver2ndEt"] ]
Feature_Type_Names += [ ["Ratio", "Log1stEtOver3rdEt"] ]
Feature_Type_Names += [ ["Ratio", "Log2ndEtOver3rdEt"] ]
Feature_Type_Names += [ ["Ratio", "Log1stEtOver2ndEt_BDTSort"] ]
Feature_Type_Names += [ ["Ratio", "Log1stEtOver3rdEt_BDTSort"] ]
Feature_Type_Names += [ ["Ratio", "Log2ndEtOver3rdEt_BDTSort"] ]
Feature_Type_Names += [ ["Ratio", "EFOsOverTotalEFOs"] ]
Feature_Type_Names += [ ["Ratio", "1stEtOverTypeEt"] ]
Feature_Type_Names += [ ["Ratio", "1stBDTEtOverTypeEt"] ]
for etType in SeedEtTypes:
    Feature_Type_Names += [ ["Ratio", "EtOver" + etType] ]
    Feature_Type_Names += [ ["Ratio", "1stEtOver" + etType] ]
    Feature_Type_Names += [ ["Ratio", "1stBDTEtOver" + etType] ]
#

#######################
### StdDevs
#######################
Default_Values["StdDev"] = -10000
Default_Sigmas["StdDev"] = 500.
Feature_Type_Names += [ ["StdDev", "DRToJetAxis"] ]
Feature_Type_Names += [ ["StdDev", "DRToLeading"] ]
Feature_Type_Names += [ ["StdDev", "E"] ]
Feature_Type_Names += [ ["StdDev", "Et"] ]
for etType in SeedEtTypes:
    Feature_Type_Names += [ ["StdDev", "Et_Wrt" + etType] ]
#
for eType in SeedETypes:
    Feature_Type_Names += [ ["StdDev", "E_Wrt" + eType] ]
#


#######################
### Pi0 BDT Values from eflowRec
#######################
#NOTE:  Make sure to only use these for neutral types
#       in the upcoming for loop 
Default_Values["eflowPID"] = -2
Default_Sigmas["eflowPID"] = 0.1
Feature_Type_Names += [ ["eflowPID", "Pi0Val_1st_BDTSort"] ]
Feature_Type_Names += [ ["eflowPID", "Pi0Val_2nd_BDTSort"] ]
Feature_Type_Names += [ ["eflowPID", "Pi0Val_3rd_BDTSort"] ]
Feature_Type_Names += [ ["eflowPID", "Pi0Val_Sum12_BDTSort"] ]
Feature_Type_Names += [ ["eflowPID", "Pi0Val_Sum123_BDTSort"] ]
Feature_Type_Names += [ ["eflowPID", "Pi0Val_1st"] ]
Feature_Type_Names += [ ["eflowPID", "Pi0Val_2nd"] ]
Feature_Type_Names += [ ["eflowPID", "Pi0Val_3rd"] ]
Feature_Type_Names += [ ["eflowPID", "Pi0Val_Sum12"] ]
Feature_Type_Names += [ ["eflowPID", "Pi0Val_Sum123"] ]



#######################
### Add the single type variables to the 
### list of features to be extracted
#######################
for iEFOType in EFO_Types:
    for iFeature in Feature_Type_Names:
        
        curType = iFeature[0]
        curName = iFeature[1]
        
        #veto exclude variables from extraction (not calculated, redundant
        if iEFOType != "All":
            if curType == "Isolation":
                continue;
            if curType == "Num":
                continue;
        
        if iEFOType == "All":
            if curType == "Angle" and curName == "ToJetAxis":
                continue;
            if curType == "DeltaR" and curName == "ToJetAxis":
                continue;
            if curType == "Ratio" and curName == "EFOsOverTotalEFOs":
                continue;
            if curType == "Ratio" and curName == "EtOver":
                continue;
        
        if curType == "eflowPID":
            if iEFOType.find("Neut") is -1:
                continue;
        
        curFeature = Features_Prefix + iEFOType + "_" + curType + "_" + curName
        
        curDefVal = Default_Values[curType]
        
        #update default values based on Variable in type:
        if curType == "HLV":
            if curName.find("Eta") is not -1:
                curDefVal = -7
            if curName.find("Phi") is not -1:
                curDefVal = -7
            if curName.find("M") is not -1:
                curDefVal = -200
        
        if curType == "JetMoment":
            if curName.find("xTotalE") is not -1:
                curDefVal = -10000
        
        if curType == "StdDev":
            if curName.find("DRTo") is not -1:
                curDefVal = -0.2
            if curName.find("WrtE") is not -1:
                curDefVal = -0.2
        
        curDefSig = curDefVal / 20.
        
        
        featureDefaultValue[curFeature] = curDefVal
        featureDefaultValueSmearingSigma[curFeature] = curDefSig
        featureType[curFeature] = 'F'





# #########################################################################################
#
# New structured features --- combined types
#
# #########################################################################################
Default_Values["Combined"] = -0.2
Default_Sigmas["Combined"] = 0.01

#  ###
# NOTE: Order of Types here should be the same as in the enumeration in
#       Reconstruction/PanTau/PanTauEvent/PanTauEvent/CorrectedEflowMomentum.h
#       Names mut be identical to what CorrectedEflowMomentum::typeName says!
# BECAUSE:
#   Features calculated and named by using that enumeration
#   ===> Extraction needs to apply same pattern!
# ###
EFO_Comb = []
#EFO_Comb += ["Charged"]
EFO_Comb += ["QualChrg"]
#EFO_Comb += ["EmNeut"]
EFO_Comb += ["Pi0Neut"]
#EFO_Comb += ["HadNeut"]
#EFO_Comb += ["Neutral"]
#EFO_Comb += ["Neutral1"]
#EFO_Comb += ["Neutral2"]
#EFO_Comb += ["NeutCore1"]
#EFO_Comb += ["NeutCore2"]
#EFO_Comb += ["QualChrg2"]
EFO_Comb += ["QualNeut"]


#######################
### Single features
#######################
Feature_Comb_Names_Single = []
Feature_Comb_Names_Single += [ ["Combined", "NumChargedOverNumNeutral"] ]
Feature_Comb_Names_Single += [ ["Combined", "NumChargedOverNumTotal"] ]
Feature_Comb_Names_Single += [ ["Combined", "AnglePlane1stCharged1st2ndNeutral"] ]

#######################
### Type-vs-Type features
#######################
Feature_Comb_Names_TypeVsType = []
Feature_Comb_Names_TypeVsType += [ ["Combined", "Log", "EtOver", "Et"] ]
Feature_Comb_Names_TypeVsType += [ ["Combined", "Angle", "To", ""] ]
Feature_Comb_Names_TypeVsType += [ ["Combined", "DeltaR1st", "To1st", ""] ]
Feature_Comb_Names_TypeVsType += [ ["Combined", "Angle1st", "To1st", ""] ]

Feature_Comb_Names_TypeVsType_Full = []
Feature_Comb_Names_TypeVsType_Full += [ ["Combined", "Log1st", "EtOver", "Et"] ]

#######################
### Selected Type combinations
#######################
EFO_Comb_Selected_Charged = []
#EFO_Comb_Selected_Charged += ["Charged"]
EFO_Comb_Selected_Charged += ["QualChrg"]
#EFO_Comb_Selected_Charged += ["QualChrg2"]

EFO_Comb_Selected_Neutral = []
#EFO_Comb_Selected_Neutral += ["EmNeut"]
#EFO_Comb_Selected_Neutral += ["Neutral"]
EFO_Comb_Selected_Neutral += ["Pi0Neut"]
#EFO_Comb_Selected_Neutral += ["NeutCore1"]
#EFO_Comb_Selected_Neutral += ["NeutCore2"]
EFO_Comb_Selected_Neutral += ["QualNeut"]

Feature_Comb_Names_SelTypes = []
for etType in SeedEtTypes:
    Feature_Comb_Names_SelTypes += [ ["Combined", "Mean", "Et_Wrt" + etType] ]
#
Feature_Comb_Names_SelTypes += [ ["Combined", "InvMass", ""] ]
Feature_Comb_Names_SelTypes += [ ["Combined", "Angle1st2nd", ""] ]


#######################
### Single features
#######################
print("featureList.py: Adding features (combined, single)")
for iFeatCombSgl in Feature_Comb_Names_Single:
    
    curType = iFeatCombSgl[0]
    curName = iFeatCombSgl[1]
    
    curFeature = Features_Prefix + curType + "_" + curName
    
    curDefVal = Default_Values[curType]
    curDefSig = curDefVal /20.
    
    featureDefaultValue[curFeature] = curDefVal
    featureDefaultValueSmearingSigma[curFeature] = curDefSig
    featureType[curFeature] = 'F'
#end for loop over combined features (single)


#######################
### Type-vs-Type
#######################
for iType in range(len(EFO_Comb)):
    for jType in range(len(EFO_Comb)):
        
        if jType == iType:
            continue
        
        #Add combined features NOT symmetric in the two systems:
        for iFeatCombTvTF in Feature_Comb_Names_TypeVsType_Full:
            EFO_I = EFO_Comb[iType]
            EFO_J = EFO_Comb[jType]
            
            curType = iFeatCombTvTF[0]
            curFrag_A = iFeatCombTvTF[1]
            curFrag_B = iFeatCombTvTF[2]
            curFrag_C = iFeatCombTvTF[3]
            
            curName = curFrag_A + EFO_J + curFrag_B + EFO_I + curFrag_C
            curFeature = Features_Prefix + curType + "_" + curName
            
            curDefVal = Default_Values[curType]
            curDefSig = Default_Sigmas[curType]
            
            if curName.find("Log1st") is not -1:
                curDefVal = -5.
            
            curDefSig = curDefVal/20.
            
            featureDefaultValue[curFeature] = curDefVal
            featureDefaultValueSmearingSigma[curFeature] = curDefSig
            featureType[curFeature] = 'F'
        
        #For the symmetric ones, no need to get A-B and B-A, hence skip
        if jType < iType:
            continue
        
        for iFeatCombTvT in Feature_Comb_Names_TypeVsType:
            
            EFO_I = EFO_Comb[iType]
            EFO_J = EFO_Comb[jType]
            
            curType = iFeatCombTvT[0]
            curFrag_A = iFeatCombTvT[1]
            curFrag_B = iFeatCombTvT[2]
            curFrag_C = iFeatCombTvT[3]
            
            curName = curFrag_A + EFO_J + curFrag_B + EFO_I + curFrag_C
            curFeature = Features_Prefix + curType + "_" + curName
            
            curDefVal = Default_Values[curType]
            
            
            if curName.find("Log") is not -1:
                curDefVal = -5.
            
            curDefSig = curDefVal/20.
            
            featureDefaultValue[curFeature] = curDefVal
            featureDefaultValueSmearingSigma[curFeature] = curDefSig
            featureType[curFeature] = 'F'
#end for loop over combined features (single)


#######################
### Selected Type combinations
#######################
for cType in EFO_Comb_Selected_Charged:
    for nType in EFO_Comb_Selected_Neutral:
        for iFeatCombSelT in Feature_Comb_Names_SelTypes:
            
            curType = iFeatCombSelT[0]
            curFrag_A = iFeatCombSelT[1]
            curFrag_B = iFeatCombSelT[2]
            
            curName = curFrag_A + cType + nType + curFrag_B
            curFeature = Features_Prefix + curType + "_" + curName
            
            curDefVal = Default_Values[curType]
            curDefSig = Default_Sigmas[curType]
            
            if curName.find("InvMass") is not -1:
                curDefVal = -500.
            
            curDefSig = curDefVal/20.
            
            featureDefaultValue[curFeature] = curDefVal
            featureDefaultValueSmearingSigma[curFeature] = curDefSig
            featureType[curFeature] = 'F'
#end for loop over combined features (single)



# #########################################################################################
#
# New structured features --- Generic Jet Features
#
# #########################################################################################
JetShape_Name = "JetShape"

Default_Values[JetShape_Name] = -0.5
Default_Sigmas[JetShape_Name] = 0.01

Feature_JetShape_Names = []
Feature_JetShape_Names += ["JetThrust"]
Feature_JetShape_Names += ["JetThrustMajor"]
Feature_JetShape_Names += ["JetThrustMinor"]
Feature_JetShape_Names += ["JetOblateness"]
Feature_JetShape_Names += ["JetFoxWolfram1"]
Feature_JetShape_Names += ["JetFoxWolfram2"]
Feature_JetShape_Names += ["JetFoxWolfram3"]
Feature_JetShape_Names += ["JetFoxWolfram4"]
Feature_JetShape_Names += ["JetFoxWolframRatioFW2OverFW1"]
Feature_JetShape_Names += ["JetFoxWolframRatioFW4pow4OverFW1"]
Feature_JetShape_Names += ["JetFoxWolframRatioFW234OverFW1pow4"]
Feature_JetShape_Names += ["JetFoxWolframRatioFW1PlusFW2OverFW4"]
Feature_JetShape_Names += ["JetSphericity"]
Feature_JetShape_Names += ["JetSphericityXEt"]
Feature_JetShape_Names += ["JetAplanarity"]
Feature_JetShape_Names += ["JetPlanarity"]

#######################
### Selected Type combinations
#######################
print("featureList.py: Adding features (JetShape)")
for iJetShape in Feature_JetShape_Names:
    
    curFeature = JetShape_Name + "_" + iJetShape
    
    curDefVal = Default_Values[JetShape_Name]
    
    if iJetShape.find("XEt") is not -1:
        curDefVal = -10000
    
    curDefSig = curDefVal/20.
    
    featureDefaultValue[curFeature] = curDefVal
    featureDefaultValueSmearingSigma[curFeature] = curDefSig
    featureType[curFeature] = 'F'
#end for loop over combined features (single)




# #########################################################################################
#
# New structured features --- Impact Parameters
#
# #########################################################################################
ImpactParams_Name = "ImpactParams"

Default_Values[ImpactParams_Name] = -10.
Default_Sigmas[ImpactParams_Name] = 1.

Feature_ImpactParams_Names = []
Feature_ImpactParams_Names += ["TransIPTrack1"]
Feature_ImpactParams_Names += ["TransIPSignfTrack1"]
Feature_ImpactParams_Names += ["LongIPTrack1"]
Feature_ImpactParams_Names += ["LongIPSignfTrack1"]
Feature_ImpactParams_Names += ["TransIPTrack2"]
Feature_ImpactParams_Names += ["TransIPSignfTrack2"]
Feature_ImpactParams_Names += ["LongIPTrack2"]
Feature_ImpactParams_Names += ["LongIPSignfTrack2"]
Feature_ImpactParams_Names += ["TransIPTrack3"]
Feature_ImpactParams_Names += ["TransIPSignfTrack3"]
Feature_ImpactParams_Names += ["LongIPTrack3"]
Feature_ImpactParams_Names += ["LongIPSignfTrack3"]
Feature_ImpactParams_Names += ["Largest"]
Feature_ImpactParams_Names += ["LargestSignf"]
Feature_ImpactParams_Names += ["SecondLargest"]
Feature_ImpactParams_Names += ["SecondLargestSignf"]
Feature_ImpactParams_Names += ["ThirdLargest"]
Feature_ImpactParams_Names += ["ThirdLargestSignf"]


#######################
### Impact Parameters
#######################
for iImpactParam in Feature_ImpactParams_Names:
    
    curFeature = ImpactParams_Name + "_" + iImpactParam
    
    curDefVal = Default_Values[ImpactParams_Name]
    
    if iImpactParam.find("Signf") is not -1: #if it's a significance
            curDefVal = -40
    
    curDefSig = curDefVal/20.
    
    featureDefaultValue[curFeature] = curDefVal
    featureDefaultValueSmearingSigma[curFeature] = curDefSig
    featureType[curFeature] = 'F'
#end for loop over combined features (single)








### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### 
## ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### 
# ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### 

#NOTE
# Need to have the old features here because the old names are used in the discriminant tools
# They cannot be neglected, because otherwise TMVA throws an error
# NOTE: Once the BDTs are retrained, remove this part below
#NOTE

DefaultVal_Angle = -5.0
DefaultSig_Angle =  0.2

DefaultVal_AbsE = -10000.
DefaultSig_AbsE =    250.

DefaultVal_Ratios = -2.0
DefaultSig_Ratios =  0.1

DefaultVal_DeltaR = -4.0
DefaultSig_DeltaR =  0.2

DefaultVal_InvSigm = -25.0
DefaultSig_InvSigm =   0.1

DefaultVal_Unsigned = -5.0
DefaultSig_Unsigned =  0.5

DefaultVal_JetParam = -5.0
DefaultSig_JetParam =  0.2

DefaultVal_Misc = -20.0
DefaultSig_Misc =   0.2

featureDefaultValue['EflowDRLeadingChargedLeadingNeutral'] = DefaultVal_DeltaR
featureDefaultValueSmearingSigma['EflowDRLeadingChargedLeadingNeutral'] = DefaultSig_DeltaR
featureType['EflowDRLeadingChargedLeadingNeutral'] = 'F'

featureDefaultValue['EflowChargedToNeutralEt'] = DefaultVal_Ratios
featureDefaultValueSmearingSigma['EflowChargedToNeutralEt'] = DefaultSig_Ratios
featureType['EflowChargedToNeutralEt'] = 'F'

featureDefaultValue['NEFOIn01'] = DefaultVal_Unsigned
featureDefaultValueSmearingSigma['NEFOIn01'] = DefaultSig_Unsigned
featureType['NEFOIn01'] = 'F'

featureDefaultValue['EflowNeutralSumEt'] = DefaultVal_AbsE
featureDefaultValueSmearingSigma['EflowNeutralSumEt'] = DefaultSig_AbsE
featureType['EflowNeutralSumEt'] = 'F'

featureDefaultValue['EflowMeanEtChrgToSumEt'] = DefaultVal_Ratios
featureDefaultValueSmearingSigma['EflowMeanEtChrgToSumEt'] = DefaultSig_Ratios
featureType['EflowMeanEtChrgToSumEt'] = 'F'

featureDefaultValue['NChargedQualifiedEFO'] = DefaultVal_Unsigned
featureDefaultValueSmearingSigma['NChargedQualifiedEFO'] = DefaultSig_Unsigned
featureType['NChargedQualifiedEFO'] = 'I'

featureDefaultValue['EflowRatioDeltaPt12To13LeadingChargedQualifiedComponents'] = DefaultVal_Ratios
featureDefaultValueSmearingSigma['EflowRatioDeltaPt12To13LeadingChargedQualifiedComponents'] = DefaultSig_Ratios
featureType['EflowRatioDeltaPt12To13LeadingChargedQualifiedComponents'] = 'F'

featureDefaultValue['EflowEtLeadingChrgQualifiedOverTotalEt'] = DefaultVal_Ratios
featureDefaultValueSmearingSigma['EflowEtLeadingChrgQualifiedOverTotalEt'] = DefaultSig_Ratios
featureType['EflowEtLeadingChrgQualifiedOverTotalEt'] = 'F'

featureDefaultValue['EflowStdDevEtToSumEt'] = DefaultVal_Ratios
featureDefaultValueSmearingSigma['EflowStdDevEtToSumEt'] = DefaultSig_Ratios
featureType['EflowStdDevEtToSumEt'] = 'F'

featureDefaultValue['EflowChargedToSumEtInvSigm'] = DefaultVal_InvSigm
featureDefaultValueSmearingSigma['EflowChargedToSumEtInvSigm'] = DefaultSig_InvSigm
featureType['EflowChargedToSumEtInvSigm'] = 'F'

featureDefaultValue['EflowJetMomentDRChrg'] = DefaultVal_JetParam
featureDefaultValueSmearingSigma['EflowJetMomentDRChrg'] = DefaultSig_JetParam
featureType['EflowJetMomentDRChrg'] = 'F'

featureDefaultValue['EflowMeanEtNeutToSumEt'] = DefaultVal_Ratios
featureDefaultValueSmearingSigma['EflowMeanEtNeutToSumEt'] = DefaultSig_Ratios
featureType['EflowMeanEtNeutToSumEt'] = 'F'

featureDefaultValue['EflowChargedToSumEt'] = DefaultVal_Ratios
featureDefaultValueSmearingSigma['EflowChargedToSumEt'] = DefaultSig_Ratios
featureType['EflowChargedToSumEt'] = 'F'

featureDefaultValue['EflowJetMomentDRChrg'] = DefaultVal_JetParam
featureDefaultValueSmearingSigma['EflowJetMomentDRChrg'] = DefaultSig_JetParam
featureType['EflowJetMomentDRChrg'] = 'F'

featureDefaultValue['EflowMeanEtNeutToSumEt'] = DefaultVal_Ratios
featureDefaultValueSmearingSigma['EflowMeanEtNeutToSumEt'] = DefaultSig_Ratios
featureType['EflowMeanEtNeutToSumEt'] = 'F'

featureDefaultValue['EflowDihedralAngleLeadingChargedQualified'] = DefaultVal_Angle
featureDefaultValueSmearingSigma['EflowDihedralAngleLeadingChargedQualified'] = DefaultSig_Angle
featureType['EflowDihedralAngleLeadingChargedQualified'] = 'F'

featureDefaultValue['EflowMaxDRLeadingChargedQualifiedComponents'] = DefaultVal_DeltaR
featureDefaultValueSmearingSigma['EflowMaxDRLeadingChargedQualifiedComponents'] = DefaultSig_DeltaR
featureType['EflowMaxDRLeadingChargedQualifiedComponents'] = 'F'

featureDefaultValue['EflowRatioDeltaPt23To13LeadingChargedQualifiedComponents'] = DefaultVal_Ratios
featureDefaultValueSmearingSigma['EflowRatioDeltaPt23To13LeadingChargedQualifiedComponents'] = DefaultSig_Ratios
featureType['EflowRatioDeltaPt23To13LeadingChargedQualifiedComponents'] = 'F'

featureDefaultValue['AbsSumCharge'] = DefaultVal_Unsigned
featureDefaultValueSmearingSigma['AbsSumCharge'] = DefaultSig_Unsigned
featureType['AbsSumCharge'] = 'F'

featureDefaultValue['NEFOIn02'] = DefaultVal_Unsigned
featureDefaultValueSmearingSigma['NEFOIn02'] = DefaultSig_Unsigned
featureType['NEFOIn02'] = 'F'

featureDefaultValue['SumCharge'] = DefaultVal_Misc
featureDefaultValueSmearingSigma['SumCharge'] = DefaultSig_Misc
featureType['SumCharge'] = 'I'

