#!/bin/bash

#executable="MakeUncertaintyPlots.exe"
#source="../code/MakeUncertaintyPlots.cxx"
#
#echo "Compiling executable"
#g++ -o $executable $source -Wall $($ROOTSYS/bin/root-config --cflags --glibs) -I $ROOTCOREBIN/include -L $ROOTCOREBIN/lib/* -DXAOD_STANDALONE `toAdd=""; for aLib in $ROOTCOREBIN/lib/x*/*so ; do clean="${aLib##*/}" ; clean="${clean%.so}" ; toAdd="$toAdd -l ${clean/lib/}" ; done ; echo $toAdd`
#
#if test $? -ne 0 ; then
#    echo "Failed to compile: $executable"
#    exit 1
#fi
#echo "Successfully compiled code: $executable"


BOOLT="true"
BOOLF="false"


function Moriond2013 \
{
    # Moriond 2013 release
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12a"
    configFile="JES_2012/Moriond2013/InsituJES2012_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2013-Nominal.pdf"
    compList="Insitu#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, average 2012 conditions"
    options="isDijet=true;isLargeR=false;prefix=JET_"
}

function Moriond2013_AFII \
{
    # Moriond 2013 release
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo"
    MCtype="AFII"
    configFile="JES_2012/Moriond2013/InsituJES2012_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2013-AFII-Nominal.pdf"
    compList="Insitu#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;RelativeNonClosure_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, average 2012 conditions;MC nonclosure, AFII"
    options="isDijet=true;isLargeR=false;prefix=JET_"
}

function Moriond2013_Stronger \
{
    # Moriond 2013 release
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12a"
    configFile="JES_2012/Moriond2013/InsituJES2012_StrongerCorrelations.config"
    outFile="JetUncertainties-Moriond2013-Stronger.pdf"
    compList="Insitu#,Zjet#,Gjet#,MJB#,Correlated_#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, average 2012 conditions"
    options="isDijet=true;isLargeR=false;prefix=JET_"
}

function Moriond2013_OnlyLAr \
{
    # Load default Moriond 2013 release
    Moriond2013
    # Override variables
    outFile="JetUncertainties-Moriond2013-OnlyLAr.pdf"
    compList="Insitu_LAr#;Insitu_LArESmaterial;Insitu_LArESpresampler;Insitu_LArESZee"
    compLabels="e/#gamma, total (all components);e/#gamma energy scale, material;e/#gamma energy scale, presampler;e/#gamma energy scale, Zee"
}

function Moriond2013_OnlyZjet \
{
    # Load default Moriond 2013 release
    Moriond2013
    # Override variables
    outFile="JetUncertainties-Moriond2013-OnlyZjet.pdf"
    compList="Zjet_#;Zjet_Extrapolation;Zjet_MC;Zjet_JVF;Zjet_KTerm;Zjet_Veto;Zjet_Stat#"
    compLabels="Z+jet, total (all components);Z+jet, extrapolation;Z+jet, MC generator;Z+jet, pileup rejection;Z+jet, out-of-cone;Z+jet, radiation suppression;Z+jet, combined statistics (x10)"
}

function Moriond2013_OnlyGjet \
{
    # Load default Moriond 2013 release
    Moriond2013
    # Override variables
    outFile="JetUncertainties-Moriond2013-OnlyGjet.pdf"
    compList="Gjet_#;Gjet_dPhi;Gjet_Generator;Gjet_OOC;Gjet_Purity;Gjet_Veto;Gjet_Stat#"
    compLabels="#gamma+jet, total (all components);#gamma+jet, extrapolation;#gamma+jet, MC generator;#gamma+jet, out-of-cone;#gamma+jet, photon purity;#gamma+jet, radiation suppression; #gamma+jet, combined statistics (x9)"
}

function Moriond2013_OnlyMJB \
{
    # Load default Moriond 2013 release
    Moriond2013
    # Override variables
    outFile="JetUncertainties-Moriond2013-OnlyMJB.pdf"
    compList="MJB_#;MJB_Alpha;MJB_Beta;MJB_Fragmentation;MJB_Asym;MJB_Threshold;MJB_Stat#"
    compLabels="MJB, total (all components);MJB, #alpha selection;MJB, #beta selection;MJB, fragmentation;MJB, #it{p}_{T} asymmetry;MJB, #it{p}_{T} threshold;MJB, combined statistics (x10)"
}

function Moriond2013_OnlyEtaInt \
{
    # Load default Moriond 2013 release
    Moriond2013
    # Override variables
    outFile="JetUncertainties-Moriond2013-OnlyEtaInt.pdf"
    compList="EtaIntercalibration_#;EtaIntercalibration_Modelling;EtaIntercalibration_StatAndMethod"
    compLabels="#eta-intercalibration, total (all components);#eta-intercalibration, modelling;#eta-intercalibration, statistics"
}

function Moriond2013_OnlyPileup \
{
    # Load default Moriond 2013 release
    Moriond2013
    # Override variables
    outFile="JetUncertainties-Moriond2013-OnlyPileup.pdf"
    compList="Pileup_#;Pileup_OffsetMu;Pileup_OffsetNPV;Pileup_PtTerm;Pileup_RhoTopology"
    compLabels="Pileup, total (all components);Pileup, <#mu> offset;Pileup, NPV offset;Pileup, #it{p}_{T} bias;Pileup, #rho topology"
}

function Moriond2013_OnlyFlavour \
{
    # Load default Moriond 2013 release
    Moriond2013
    # Override variables
    outFile="JetUncertainties-Moriond2013-OnlyFlavour.pdf"
    compList="Flavor_#;Flavor_Comp#;Flavor_Resp#"
    compLabels="Flavour, total (all components);Flavour composition;Flavour response"
}


function LargeR_July2014 \
{
    # July 2014 large-R jet release
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR30"
    MCtype="MC12a"
    configFile="UJ_2012/JMS2012_Jul242014_Combined.config"
    outFile="JetUncertainties-LargeR-July2014.pdf"
    compList="Gjet_#;DoubleRatio#;Topology;Pileup#;prefix=JET_"
    compLabels="#splitline{In-situ gamma+jet}{#scale[0.75]{valid up to 1000 GeV}};#splitline{Track/Calo Data/MC double ratio}{#scale[0.75]{used in the combination from 900 GeV}};MC topology;Pile-up uncertainty (#mu = 20)"
    options="isDijet=true;isLargeR=true"
}


function Final2012 \
{
    # Final 2012 release, nominal
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config"
    outFile="JetUncertainties-Final2012-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, average 2012 conditions;Punch-through, average 2012 conditions"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function CompareAll2012 \
{

    # Load default Prerec 2015 release
    Final2012
    # Override variables
    outFile="JetUncertainties-CompareAll2012.pdf"
    options="isDijet=false;isLargeR=false;prefix=JET_;compareOnly=true;compareConfig1=JES_2012/HCP2012/JESUncertainty2012_Sept2012.config&MC12a&HCP 2012;compareConfig2=JES_2012/Moriond2013/InsituJES2012_AllNuisanceParameters.config&MC12a&Moriond 2013;compareConfig3=JES_2012/Final/InsituJES2012_AllNuisanceParameters.config&MC12&Final 2012"

}


function Final2012_AFII \
{
    # Final 2012 release, nominal
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="AFII"
    configFile="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config"
    outFile="JetUncertainties-Final2012-AFII-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;RelativeNonClosure_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, average 2012 conditions;MC nonclosure, AFII"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Final2012_Stronger \
{
    # Final 2012 release, stronger correlations
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_StrongerCorrelations.config"
    outFile="JetUncertainties-Final2012-Stronger.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#,Correlated_#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, average 2012 conditions;Punch-through, average 2012 conditions"
    options="isDijet=true;isLargeR=false;prefix=JET_"
}

function Final2012_Weaker \
{
    # Final 2012 release, weaker correlations
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_WeakerCorrelations.config"
    outFile="JetUncertainties-Final2012-Weaker.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, average 2012 conditions;Punch-through, average 2012 conditions"
    options="isDijet=true;isLargeR=false;prefix=JET_"
}

function Final2012_OnlyLAr \
{
    # Load default Final 2012 release
    Final2012
    # Override variables
    outFile="JetUncertainties-Final2012-OnlyLAr.pdf"
    compList="LAr_#;LAr_ESmaterial;LAr_ESpresampler;LAr_ESZee;LAr_Esmear"
    compLabels="e/#gamma, total (all components);e/#gamma energy scale, material;e/#gamma energy scale, presampler;e/#gamma energy scale, Zee;e/#gamma energy smearing"
}

function Final2012_OnlyZjet \
{
    # Load default Final 2012 release
    Final2012
    # Override variables
    outFile="JetUncertainties-Final2012-OnlyZjet.pdf"
    compList="Zjet_#;Zjet_dPhi;Zjet_MC;Zjet_MuScale;Zjet_MuSmearID;Zjet_MuSmearMS;Zjet_JVF;Zjet_KTerm;Zjet_Veto;Zjet_Stat#"
    compLabels="Z+jet, total (all components);Z+jet, extrapolation;Z+jet, MC generator;Z+jet, #mu scale;Z+jet, #mu smearing (ID);Z+jet, #mu smearing (MS);Z+jet, pileup rejection;Z+jet, out-of-cone;Z+jet, radiation suppression;Z+jet, combined statistics (x11)"
}

function Final2012_OnlyGjet \
{
    # Load default Final 2012 release
    Final2012
    # Override variables
    outFile="JetUncertainties-Final2012-OnlyGjet.pdf"
    compList="Gjet_#;Gjet_dPhi;Gjet_Generator;Gjet_OOC;Gjet_Purity;Gjet_Veto;Gjet_Stat#"
    compLabels="#gamma+jet, total (all components);#gamma+jet, extrapolation;#gamma+jet, MC generator;#gamma+jet, out-of-cone;#gamma+jet, photon purity;#gamma+jet, radiation suppression; #gamma+jet, combined statistics (x13)"
}

function Final2012_OnlyMJB \
{
    # Load default Final 2012 release
    Final2012
    # Override variables
    outFile="JetUncertainties-Final2012-OnlyMJB.pdf"
    compList="MJB_#;MJB_Alpha;MJB_Beta;MJB_Fragmentation;MJB_Asym;MJB_Threshold;MJB_Stat#"
    compLabels="MJB, total (all components);MJB, #alpha selection;MJB, #beta selection;MJB, fragmentation;MJB, #it{p}_{T} asymmetry;MJB, #it{p}_{T} threshold;MJB, combined statistics (x10)"
}

function Final2012_OnlyEtaInt \
{
    # Load default Final 2012 release
    Final2012
    # Override variables
    outFile="JetUncertainties-Final2012-OnlyEtaInt.pdf"
    compList="EtaIntercalibration_#;EtaIntercalibration_Modelling;EtaIntercalibration_TotalStat"
    compLabels="#eta-intercalibration, total (all components);#eta-intercalibration, modelling;#eta-intercalibration, statistics"
}

function Final2012_OnlyPileup \
{
    # Load default Final 2012 release
    Final2012
    # Override variables
    outFile="JetUncertainties-Final2012-OnlyPileup.pdf"
    compList="Pileup_#;Pileup_OffsetMu;Pileup_OffsetNPV;Pileup_PtTerm;Pileup_RhoTopology"
    compLabels="Pileup, total (all components);Pileup, <#mu> offset;Pileup, NPV offset;Pileup, #it{p}_{T} bias;Pileup, #rho topology"
}

function Final2012_OnlyFlavour \
{
    # Load default Final 2012 release
    Final2012
    # Override variables
    outFile="JetUncertainties-Final2012-OnlyFlavour.pdf"
    compList="Flavor_#;Flavor_Comp#;Flavor_Resp#"
    compLabels="Flavour, total (all components);Flavour composition;Flavour response"
}

# For Dimitris Varouchas Dec 11 2014
function Final2012_FHttbar \
{
    jetDefinition="AntiKt4LCTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_23NP_ByCategory.config"
    outFile="JetUncertainties-Final2012-FHttbar.pdf"
    compList="EffectiveNP_Modelling1;Pileup_RhoTopology;EffectiveNP_Modelling1,Pileup_RhoTopology;Effective#,Pileup#,SingleParticle#,EtaIntercalib#"
    compLabels="Modelling1;Pileup, #rho topology;Modelling1 #oplus #rho topology;All except flavour"
    options="isDijet=true;isLargeR=false;prefix=JET_"
}

function Final2012_3NP_Scenario1 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_3NP_Scenario1.config"
    outFile="JetUncertainties-Final2012-3NP-Scenario1.eps"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Final2012_3NP_Scenario2 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_3NP_Scenario2.config"
    outFile="JetUncertainties-Final2012-3NP-Scenario2.eps"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Final2012_3NP_Scenario3 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_3NP_Scenario3.config"
    outFile="JetUncertainties-Final2012-3NP-Scenario3.eps"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Final2012_3NP_Scenario4 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_3NP_Scenario4.config"
    outFile="JetUncertainties-Final2012-3NP-Scenario4.eps"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

#function Final2012_4NP_Scenario1 \
#{
#    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
#    MCtype="MC12"
#    configFile="JES_2012/Final/InsituJES2012_4NP_Scenario1.config"
#    outFile="JetUncertainties-Final2012-4NP-Scenario1.pdf"
#    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3;GroupedNP_4"
#    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3;Grouped NP 4"
#    options="isDijet=false;isLargeR=false;prefix=JET_"
#}
#
#function Final2012_4NP_Scenario2 \
#{
#    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
#    MCtype="MC12"
#    configFile="JES_2012/Final/InsituJES2012_4NP_Scenario2.config"
#    outFile="JetUncertainties-Final2012-4NP-Scenario2.pdf"
#    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3;GroupedNP_4"
#    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3;Grouped NP 4"
#    options="isDijet=false;isLargeR=false;prefix=JET_"
#}
#
#function Final2012_4NP_Scenario3 \
#{
#    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
#    MCtype="MC12"
#    configFile="JES_2012/Final/InsituJES2012_4NP_Scenario3.config"
#    outFile="JetUncertainties-Final2012-4NP-Scenario3.pdf"
#    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3;GroupedNP_4"
#    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3;Grouped NP 4"
#    options="isDijet=false;isLargeR=false;prefix=JET_"
#}

function TestConfig \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/test.config"
    outFile="JetUncertainties-SplitTest.pdf"
    compList="EtaIntercalibration_#;#_T1F;#_T1C"
    compLabels="#eta intercalibration, f_{1}^{#it{p}_{T}} #oplus f_{2}^{#it{p}_{T}};#eta intercalibration, f_{1}^{#it{p}_{T}};#eta intercalibration, f_{2}^{#it{p}_{T}}"
    #compList="EtaIntercalibration_#;#_T2F;#_T2C"
    #compLabels="#eta intercalibration, f_{1}^{#eta} #oplus f_{2}^{#eta};#eta intercalibration, f_{1}^{#eta};#eta intercalibration, f_{2}^{#eta}"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function TestATLASCMS \
{

    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/CMS/FlavourTest.config"
    outFile="ATLAS-CMS-Flavour.pdf"
    #compList="FlavorResponse;gJES_CMS;bJES;bJES_HadIntModel;bJES_CMS"
    #compLabels="ATLAS gluon-jet PYTHIA8-Herwig++;#splitline{CMS#kern[2.5]{g}luon-jet PYTHIA6-Herwig++}{};ATLAS b-jet PYTHIA8-Herwig++;ATLAS b-jet hadronic interaction model;CMS#kern[2.3]{b}-jet hadronic interaction model"
    #compList="FlavorResponse;gJES_CMS;bJES;bJES_CMS"
    #compLabels="ATLAS gluon-jet PYTHIA8-Herwig++;#splitline{CMS#kern[2.5]{g}luon-jet PYTHIA6-Herwig++}{};ATLAS b-jet PYTHIA8-Herwig++;CMS#kern[2.3]{b}-jet hadronic interaction model"
    compList="FlavorResponse;gJES_CMS"
    compLabels="ATLAS gluon-jet PYTHIA8-Herwig++;CMS#kern[2.5]{g}luon-jet PYTHIA6-Herwig++"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=false;DoATLASLabel=false"
}

function Final2012_Pileup \
{
    # Final 2012 release, nominal
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/TestPileup.config"
    outFile="JetUncertainties-Final2012-Nominal-Pileup.pdf"
    
    #compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    #compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, average 2012 conditions;Punch-through, average 2012 conditions"
    #options="isDijet=true;isLargeR=false;prefix=JET_;isEtaDepPileup=true;isPublic=true"

    compList="Pileup_OffsetMu;Pileup_OffsetNPV;Pileup_PtTerm;Pileup_RhoTopology"
    compLabels="Pileup #mu;Pileup NPV;Pileup p_{T};Pileup #rho"
    options="isDijet=true;isLargeR=false;prefix=JET_;isEtaDepPileup=true;isPublic=false"
}

function Prerec2015 \
{

    # Final 2012 release, nominal
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/Prerec/InsituJES2015_AllNuisanceParameters.config"
    outFile="JetUncertainties-Prerec2015-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#;Xcalib_#"
    compLabels="Absolute #it{in situ} JES (2012);Relative #it{in situ} JES (scaled 2012);Flav. composition;Flav. response;Pileup, predicted 2015 conditions;Punch-through, predicted 2015 conditions; 2012 to 2015 extrapolation uncertainty"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_"
}

# Unknown composition

function Prerec2015_50ns_Unknown \
{

    # Load default Prerec 2015 release
    Prerec2015
    # Override variables
    configFile="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_50ns.config"
    outFile="JetUncertainties-Prerec2015-50ns-Unknown-Nominal.pdf"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;bunchSpacing=50"

}

function Prerec2015_50ns_AFII_Unknown \
{

    # Load default Prerec 2015 release
    Prerec2015
    # Override variables
    MCtype="AFII"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;Xcalib_#;RelativeNonClosure_#"
    compLabels="Absolute #it{in situ} JES (2012);Relative #it{in situ} JES (scaled 2012);Flav. composition;Flav. response;Pileup, predicted 2015 conditions;2012 to 2015 extrapolation uncertainty;MC nonclosure, AFII"
    configFile="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_50ns.config"
    outFile="JetUncertainties-Prerec2015-50ns-Unknown-AFII.pdf"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;bunchSpacing=50"

}

function Prerec2015_50ns_Unknown_Compare \
{

    # Load default Prerec 2015 release
    Prerec2015
    # Override variables
    configFile="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_50ns.config"
    outFile="JetUncertainties-Prerec2015-50ns-Unknown-Nominal-CompareTo2012.pdf"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;bunchSpacing=50;doCompare=JES_2012/Final/InsituJES2012_AllNuisanceParameters.config&MC12&Total uncertainty, 2012"

}

function Prerec2015_50ns_Unknown_OnlyFlavour \
{

    # Load default Prerec 2015 release
    Prerec2015_50ns_Unknown
    outFile="JetUncertainties-Prerec2015-50ns-Unknown-OnlyFlavour.pdf"
    compList="Flavor_#;Flavor_Comp#;Flavor_Resp#"
    compLabels="Flavour, total (all components);Flavour composition;Flavour response"

}

function Prerec2015_50ns_Unknown_OnlyXcalib \
{

    # Load default Prerec 2015 release
    Prerec2015_50ns_Unknown
    outFile="JetUncertainties-Prerec2015-50ns-Unknown-OnlyXcalib.pdf"
    compList="Xcalib_#;Xcalib_1#;Xcalib_2#;Xcalib_3#;Xcalib_4#;Xcalib_5#;Xcalib_6#"
    compLabels="2012 to 2015 extrapolation, total;Bunch spacing;Digitization;Topoclustering;Early data term;Noise threshold;Underlying event"

}

function Prerec2015_50ns_Unknown_OnlyPileup \
{
    # Load default Prerec 2015 release
    Prerec2015_50ns_Unknown
    # Override variables
    outFile="JetUncertainties-Prerec2015-50ns-Unknown-OnlyPileup.pdf"
    compList="Pileup_#;Pileup_OffsetMu;Pileup_OffsetNPV;Pileup_PtTerm;Pileup_RhoTopology"
    compLabels="Pileup, total (all components);Pileup, <#mu> offset;Pileup, NPV offset;Pileup, #it{p}_{T} bias;Pileup, #rho topology"
}


function Prerec2015_25ns_Unknown \
{

    # Load default Prerec 2015 release
    Prerec2015
    # Override variables
    configFile="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_25ns.config"
    outFile="JetUncertainties-Prerec2015-25ns-Unknown-Nominal.pdf"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;bunchSpacing=25"

}

function Prerec2015_25ns_AFII_Unknown \
{

    # Load default Prerec 2015 release
    Prerec2015
    # Override variables
    MCtype="AFII"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;Xcalib_#;RelativeNonClosure_#"
    compLabels="Absolute #it{in situ} JES (2012);Relative #it{in situ} JES (scaled 2012);Flav. composition;Flav. response;Pileup, predicted 2015 conditions;2012 to 2015 extrapolation uncertainty;MC nonclosure, AFII"
    configFile="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_25ns.config"
    outFile="JetUncertainties-Prerec2015-25ns-Unknown-AFII.pdf"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;bunchSpacing=25"

}


function Prerec2015_25ns_Unknown_Compare \
{

    # Load default Prerec 2015 release
    Prerec2015
    # Override variables
    configFile="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_25ns.config"
    outFile="JetUncertainties-Prerec2015-25ns-Unknown-Nominal-CompareTo2012.pdf"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;bunchSpacing=25;doCompare=JES_2012/Final/InsituJES2012_AllNuisanceParameters.config&MC12&Total uncertainty, 2012"

}



function Prerec2015_25ns_Unknown_OnlyFlavour \
{

    # Load default Prerec 2015 release
    Prerec2015_25ns_Unknown
    outFile="JetUncertainties-Prerec2015-25ns-Unknown-OnlyFlavour.pdf"
    compList="Flavor_#;Flavor_Comp#;Flavor_Resp#"
    compLabels="Flavour, total (all components);Flavour composition;Flavour response"

}

function Prerec2015_25ns_Unknown_OnlyXcalib \
{

    # Load default Prerec 2015 release
    Prerec2015_25ns_Unknown
    outFile="JetUncertainties-Prerec2015-25ns-Unknown-OnlyXcalib.pdf"
    compList="Xcalib_#;Xcalib_1#;Xcalib_2#;Xcalib_3#;Xcalib_4#;Xcalib_5#;Xcalib_6#"
    compLabels="2012 to 2015 extrapolation, total;Bunch spacing;Digitization;Topoclustering;Early data term;Noise threshold;Underlying event"

}

function Prerec2015_25ns_Unknown_OnlyPileup \
{
    # Load default Prerec 2015 release
    Prerec2015_25ns_Unknown
    # Override variables
    outFile="JetUncertainties-Prerec2015-25ns-Unknown-OnlyPileup.pdf"
    compList="Pileup_#;Pileup_OffsetMu;Pileup_OffsetNPV;Pileup_PtTerm;Pileup_RhoTopology"
    compLabels="Pileup, total (all components);Pileup, <#mu> offset;Pileup, NPV offset;Pileup, #it{p}_{T} bias;Pileup, #rho topology"

}

# Dijet composition

function Prerec2015_50ns_Dijet \
{

    # Load default Prerec 2015 release
    Prerec2015
    # Override variables
    configFile="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_50ns.config"
    outFile="JetUncertainties-Prerec2015-50ns-Dijet-Nominal.pdf"
    options="isDijet=true;isGSC=true;isLargeR=false;prefix=JET_"

}

function Prerec2015_50ns_Dijet_OnlyFlavour \
{

    # Load default Prerec 2015 release
    Prerec2015_50ns_Dijet
    outFile="JetUncertainties-Prerec2015-50ns-Dijet-OnlyFlavour.pdf"
    compList="Flavor_#;Flavor_Comp#;Flavor_Resp#"
    compLabels="Flavour, total (all components);Flavour composition;Flavour response"

}

function Prerec2015_50ns_Dijet_OnlyXcalib \
{

    # Load default Prerec 2015 release
    Prerec2015_50ns_Dijet
    outFile="JetUncertainties-Prerec2015-50ns-Dijet-OnlyXcalib.pdf"
    compList="Xcalib_#;Xcalib_1#;Xcalib_2#;Xcalib_3#;Xcalib_4#;Xcalib_5#;Xcalib_6#"
    compLabels="2012 to 2015 extrapolation, total;Bunch spacing;Digitization;Topoclustering;Early data term;Noise threshold;Underlying event"

}

function Prerec2015_25ns_Dijet \
{

    # Load default Prerec 2015 release
    Prerec2015
    # Override variables
    configFile="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_25ns.config"
    outFile="JetUncertainties-Prerec2015-25ns-Dijet-Nominal.pdf"
    options="isDijet=true;isGSC=true;isLargeR=false;prefix=JET_"

}

function Prerec2015_25ns_Dijet_OnlyFlavour \
{

    # Load default Prerec 2015 release
    Prerec2015_25ns_Dijet
    outFile="JetUncertainties-Prerec2015-25ns-Dijet-OnlyFlavour.pdf"
    compList="Flavor_#;Flavor_Comp#;Flavor_Resp#"
    compLabels="Flavour, total (all components);Flavour composition;Flavour response"

}

function Prerec2015_25ns_Dijet_OnlyXcalib \
{

    # Load default Prerec 2015 release
    Prerec2015_25ns_Dijet
    outFile="JetUncertainties-Prerec2015-25ns-Dijet-OnlyXcalib.pdf"
    compList="Xcalib_#;Xcalib_1#;Xcalib_2#;Xcalib_3#;Xcalib_4#;Xcalib_5#;Xcalib_6#"
    compLabels="2012 to 2015 extrapolation, total;Bunch spacing;Digitization;Topoclustering;Early data term;Noise threshold;Underlying event"

}

####################################### Old stuff

function Prerec2015_OnlyFlavour \
{
    # Load default Prerec 2015 release
    Prerec2015
    # Override variables
    outFile="JetUncertainties-Prerec2015-OnlyFlavour.pdf"
    compList="Flavor_#;Flavor_Comp#;Flavor_Resp#"
    compLabels="Flavour, total (all components);Flavour composition;Flavour response"
}

function Prerec2015_OnlyXcalib \
{
    # Load default Prerec 2015 release
    Prerec2015
    # Override variables
    outFile="JetUncertainties-Prerec2015-OnlyXcalib.pdf"
    compList="Xcalib_#;Xcalib_1#;Xcalib_2#;Xcalib_3#;Xcalib_4#;Xcalib_5#;Xcalib_6#"
    compLabels="2012 to 2015 extrapolation, total;Bunch spacing;Digitization;Topoclustering;Early data term;Noise threshold;Underlying event"
}


function JER2012
{
    # Final 2012 JER
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFile="JER_2012/InsituJER2012_AllNuisanceParameters.config"
    outFile="JER-Final2012.pdf"
    compList="JER_NoiseTerm;JER_Dijet_#;JER_Gjet_#;JER_Zjet_#"
    compLabels="Noise term;Dijet;#gamma+jet;Z+jet"
    options="isJER=true;isDijet=false;isLargeR=false;prefix=JET_"
}

function JER2015
{
    # Prerec 2015 JER
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JER_2015/PrerecJER2015_AllNuisanceParameters.config"
    outFile="JER-Prerec2015.pdf"
    compList="JER_NoiseTerm;JER_Dijet_#;JER_Gjet_#;JER_Zjet_#;Xcalib_#"
    compLabels="Noise term;Dijet #it{in situ} measurement;#gamma+jet #it{in situ} measurement;Z+jet #it{in situ} measurement;2012 to 2015 extrapolation"
    options="isJER=true;isDijet=false;isLargeR=false;prefix=JET_"
}

function JER2015_xcal
{
    JER2015
    compList="Xcalib_50nsVs25ns;Xcalib_5Vs4sample;Xcalib_TopoClustering;Xcalib_EarlyData;Xcalib_NoiseThreshold;Xcalib_UnderlyingEvent;Xcalib_PhiModulation"
    compLabels="50 vs 25 ns;5 vs 4 samples;Topo-clustering;Early data;Noise threshold;Underlying event;Phi modulation"
    outFile="JER-Prerec2015_xCalib.pdf"
}

function JER_2015_2012_Compare \
{

    # Load default prerec JER 2015
    JER2015
    # Override variables
    outFile="JER-Prerec2015-CompareTo2012.pdf"
    options="isJER=true;isDijet=false;isGSC=false;isLargeR=false;prefix=JET_;doCompare=JER_2012/InsituJER2012_AllNuisanceParameters.config&MC12&Total uncertainty, 2012"

}




# Check arguments
if test $# -lt 1 ; then
    echo "USAGE: $0 <scenario to run> (options addition or overwrite)"
    echo "Scenarios available:"
    echo "  Moriond2013"
    echo "  LargeR_July2014"
    echo "  Final2012"
    echo "  Final2012_Stronger"
    echo "  Final2012_Weaker"
    exit 1
fi

firstArg=$BOOLT
for anArg in $@ ; do
    if [ "$firstArg" = "$BOOLT" ] ; then
        # Run the specified scenario
        $1
        
        # Check that it looks semi-reasonable
        if [ -z "$outFile" ] ; then
            echo "Output file is not specified"
            echo "Check that you spelled the desired scenario correctly"
            exit 2
        fi
        
        # Move on to options
        firstArg=$BOOLF
        continue
    elif ! [ -z "$anArg" ] ; then
        # Overwrite option(s) if needed
        key=`echo $anArg | tr "=" " " | awk '{print $1}'`
        val=`echo $anArg | tr "=" " " | awk '{print $2}'`
        if [ -z "$key" ] || [ -z "$val" ] || [ "$key" = "$val" ] ; then
            echo "Bad option formatting: $anArg"
            exit 3
        fi
        if [[ "$options" = *$key* ]] ; then
            #Overwrite
            options=`echo "$options" | sed s/"${key}=[^;]*"/"${key}=${val}"/g`
        else
            # Extend
            options="${options};${key}=${val}"
        fi
    fi
done

echo "$options"
MakeUncertaintyPlots "$outFile" "$jetDefinition" "$MCtype" "$configFile" "$compList" "$compLabels" "$options"

