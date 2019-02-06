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
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, average 2012 conditions;MC nonclosure, fast simulation"
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
    compList="Gjet_#;DoubleRatio#;Topology;Pileup#"
    compLabels="#splitline{In-situ gamma+jet}{#scale[0.75]{valid up to 1000 GeV}};#splitline{Track/Calo Data/MC double ratio}{#scale[0.75]{used in the combination from 900 GeV}};MC topology;Pile-up uncertainty (#mu = 20)"
    options="isDijet=true;isLargeR=true;prefix=JET_"
}


function Final2012_unknown \
{
    # Final 2012 release, nominal
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config"
    outFile="JetUncertainties-Final2012-Unknown-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, average 2012 conditions;Punch-through, average 2012 conditions"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Final2012_dijet \
{
    # Final 2012 release, nominal
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config"
    outFile="JetUncertainties-Final2012-Dijet-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, average 2012 conditions;Punch-through, average 2012 conditions"
    options="isDijet=false;isLargeR=false;prefix=JET_;"
}

function Final2012_unknown_log \
{
    # Final 2012 release, nominal
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config"
    outFile="JetUncertainties-Final2012-Unknown-log-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, average 2012 conditions;Punch-through, average 2012 conditions"
    options="isDijet=false;isLargeR=false;prefix=JET_;doLogY=true"
}

function Final2012_dijet_log \
{
    # Final 2012 release, nominal
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config"
    outFile="JetUncertainties-Final2012-Dijet-log-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, average 2012 conditions;Punch-through, average 2012 conditions"
    options="isDijet=true;isLargeR=false;prefix=JET_;doLogY=true"
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
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, average 2012 conditions;MC nonclosure, fast simulation"
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
    Final2012_unknown
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

function Final2012_3NP_Scenario1_Dijet \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_3NP_Scenario1.config"
    outFile="JetUncertainties-Final2012-3NP-Scenario1-Dijet.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3"
    options="isDijet=true;isLargeR=false;prefix=JET_"
}

function Final2012_3NP_Scenario1_Unknown \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_3NP_Scenario1.config"
    outFile="JetUncertainties-Final2012-3NP-Scenario1-Unknown.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Final2012_3NP_Scenario1 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_3NP_Scenario1.config"
    outFile="JetUncertainties-Final2012-3NP-Scenario1.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Final2012_3NP_Scenario2 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_3NP_Scenario2.config"
    outFile="JetUncertainties-Final2012-3NP-Scenario2.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Final2012_3NP_Scenario3 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_3NP_Scenario3.config"
    outFile="JetUncertainties-Final2012-3NP-Scenario3.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Final2012_3NP_Scenario4 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/InsituJES2012_3NP_Scenario4.config"
    outFile="JetUncertainties-Final2012-3NP-Scenario4.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

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

    jetDefinition="AntiKt4LCTopo" #;AntiKt4LCTopo"
    MCtype="MC12"
    configFile="JES_2012/Final/CMS/FlavourTestGluon.config"
    outFile="ATLAS-CMS-Flavour-Gluon.pdf"
    #compList="FlavorResponse;gJES_CMS;bJES;bJES_HadIntModel;bJES_CMS"
    #compLabels="ATLAS gluon-jet PYTHIA8-Herwig++;#splitline{CMS#kern[2.5]{g}luon-jet PYTHIA6-Herwig++}{};ATLAS b-jet PYTHIA8-Herwig++;ATLAS b-jet hadronic interaction model;CMS#kern[2.3]{b}-jet hadronic interaction model"
    #compList="FlavorResponse;gJES_CMS;bJES;bJES_CMS"
    #compLabels="ATLAS gluon-jet PYTHIA8-Herwig++;#splitline{CMS#kern[2.5]{g}luon-jet PYTHIA6-Herwig++}{};ATLAS b-jet PYTHIA8-Herwig++;CMS#kern[2.3]{b}-jet hadronic interaction model"
    compList="Flavor_Response;Flavor_CMS"
    compLabels="ATLAS gluon-jet uncertainty;CMS #kern[2.1]{ }gluon-jet uncertainty"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=false;DoATLASLabel=true"
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
    compLabels="Absolute #it{in situ} JES (2012);Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup, predicted 2015 conditions;Punch-through, predicted 2015 conditions; 2012 to 2015 extrapolation uncertainty"

# compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
#    compLabels="Absolute #it{in situ} JES (2012);Relative #it{in situ} JES (scaled 2012);Flav. composition;Flav. response;Pileup, predicted 2015 conditions;Punch-through, predicted 2015 conditions;"
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
    compLabels="Absolute #it{in situ} JES (2012);Relative #it{in situ} JES (scaled 2012);Flav. composition;Flav. response;Pileup, predicted 2015 conditions;2012 to 2015 extrapolation uncertainty;MC nonclosure, fast simulation"
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
    compLabels="Absolute #it{in situ} JES (2012);Relative #it{in situ} JES (scaled 2012);Flav. composition;Flav. response;Pileup, predicted 2015 conditions;2012 to 2015 extrapolation uncertainty;MC nonclosure, fast simulation"
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

#######################################

function Moriond2016 \
{

    # Final 2012 release, nominal
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/Moriond2016/JES2015_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2016-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_"
}

function Moriond2016_CompareToPrerec \
{
    # Load default Moriond 2016 release
    Moriond2016
    # Override variables
    options="${options};doCompare=JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_25ns.config&MC15&Preliminary uncertainty"
}

function Moriond2016_Public \
{
    Moriond2016
    outFile="${outFile/Nominal/Nominal-Public}"
    options="${options};isPublic=True;ATLASLabel=Internal;Interp=True"
    compLabels="${compLabels/Pileup/Pile-up, average 2015 conditions}"
    compLabels="${compLabels/Punch-through/Punch-through, average 2015 conditions}"
}

function Moriond2016_Public_Dijet \
{
    Moriond2016_Public
    outFile="${outFile/Public/Public-dijet}"
    options="${options/isDijet=false/isDijet=true}"
}

function Moriond2016_Public_Compare \
{
    Moriond2016_Public
    outFile="${outFile/Public/Compare-Public}"
    options="${options};doCompare=JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_25ns.config&MC15&Preliminary uncertainty"
}

function Moriond2016_Public_Dijet_Compare \
{
    Moriond2016_Public_Dijet
    outFile="${outFile/Public/Compare-Public}"
    options="${options};doCompare=JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_25ns.config&MC15&Preliminary uncertainty"
}

# Unknown composition

function Moriond2016_Unknown \
{

    # Load default Moriond 2016 release
    Moriond2016
    # Override variables
    outFile="JetUncertainties-Moriond2016-Unknown-Nominal.pdf"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;"

}

function Moriond2016_AFII_Unknown \
{

    # Load default Prerec 2015 release
    Moriond2016
    # Override variables
    MCtype="AFII"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;RelativeNonClosure_#"
    compLabels="Absolute #it{in situ} JES (2012);Relative #it{in situ} JES (scaled 2012);Flav. composition;Flav. response;Pileup, predicted 2015 conditions;MC nonclosure, fast simulation"
    outFile="JetUncertainties-Prerec2015-25ns-Unknown-AFII.pdf"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;bunchSpacing=25"

}

# For comparison to result without propagated through uncertainties
function Moriond2016_Unknown_NoMJBProp \
{

    # Load default Moriond 2016 release
    Moriond2016
    # Override variables
    configFile="JES_2015/Moriond2016/JES2015_WithoutPropagatedPars.config" 
    outFile="JetUncertainties-Moriond2016-Unknown-Nominal-NoMJBPropagatedUncertainties.pdf"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;"

}


####################################### Moriond 2016 Specialty

function Moriond2016_OnlyEtaInt \
{
    # Load default Moriond 2016 release
    Moriond2016
    # Override variables
    outFile="JetUncertainties-Moriond2016-Unknown-OnlyEtaInt.pdf"
    compList="EtaIntercalibration_#;EtaIntercalibration_Modelling;EtaIntercalibration_TotalStat;EtaIntercalibration_NonClosure"
    compLabels="#eta-intercalibration, total (all components);#eta-intercalibration, modelling;#eta-intercalibration, statistics;#eta-intercalibration, non-closure"
}

function Moriond2016_OnlyPileup \
{
    # Load default Prerec 2015 release
    Moriond2016
    # Override variables
    outFile="JetUncertainties-Moriond2016-Unknown-OnlyPileup.pdf"
    compList="Pileup_#;Pileup_OffsetMu;Pileup_OffsetNPV;Pileup_PtTerm;Pileup_RhoTopology"
    compLabels="Pileup, total (all components);Pileup, <#mu> offset;Pileup, NPV offset;Pileup, #it{p}_{T} bias;Pileup, #rho topology"

}

####################################### Moriond 2016 Reductions

function Moriond2016_GlobalReductions \
{
    # Load default Moriond 2016 release
    Moriond2016
    # Override variables
    configFile="JES_2015/Moriond2016/JES2015_19NP.config" 
    outFile="JetUncertainties-Moriond2016-Unknown-GlobalReduction.pdf"
    compList="EffectiveNP#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;"

}

function Moriond2016_CategoryReductions \
{
    # Load default Moriond 2016 release
    Moriond2016
    # Override variables
    configFile="JES_2015/Moriond2016/JES2015_25NP_ByCategory.config" 
    outFile="JetUncertainties-Moriond2016-Unknown-CategoryReduction.pdf"
    compList="EffectiveNP#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;"

}

####################################### Moriond 2016 Reductions Specialty

function Moriond2016_GlobalReductions_Alone \
{
    # Load default Moriond 2016 release
    Moriond2016
    # Override variables
    configFile="JES_2015/Moriond2016/JES2015_19NP.config" 
    outFile="JetUncertainties-Moriond2016-Unknown-GlobalReduction-Alone.pdf"
    compList="EffectiveNP#,SingleParticle#;EffectiveNP_1;EffectiveNP_2;EffectiveNP_3;EffectiveNP_4;EffectiveNP_5;EffectiveNP_6restTerm;SingleParticle#"
    compLabels="In situ total;First global parameter;Second global parameter;Third global parameter;Fourth global parameter;Fifth global parameter;Remainder parameter;High-pT term"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;"

}

function Moriond2016_CategoryReductions_Alone \
{
    # Load default Moriond 2016 release
    Moriond2016
    # Override variables
    configFile="JES_2015/Moriond2016/JES2015_25NP_ByCategory.config" 
    outFile="JetUncertainties-Moriond2016-Unknown-CategoryReduction-Alone.pdf"
    compList="EffectiveNP#,SingleParticle#;EffectiveNP_Statistical#;EffectiveNP_Modelling#;EffectiveNP_Detector#;EffectiveNP_Mixed#;SingleParticle#"
    compLabels="In situ total;Statistical terms;Modelling terms;Detector terms;Mixed terms;High-pT term"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;"

}

#######################################

function ICHEP2016 \
{

    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo"
    MCtype="MC15"
    configFile="JES_2015/ICHEP2016/JES2015_AllNuisanceParameters.config"
    outFile="JetUncertainties-ICHEP2016-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_"
}

function ICHEP2016_CompareToMoriond \
{
    # Load default Ichep 2016 release
    ICHEP2016
    # Override variables
    outFile="JetUncertainties-ICHEP2016-Nominal-CompareToMoriond.pdf"
    options="${options};doCompare=JES_2015/Moriond2016/JES2015_AllNuisanceParameters.config&MC15&Moriond uncertainty"
}

function ICHEP2016_Public \
{
    ICHEP2016
    outFile="${outFile/Nominal/Nominal-Public}"
    options="${options};isPublic=True;ATLASLabel=Preliminary;Interp=True"
    compLabels="${compLabels/Pileup/Pileup, average 2015 conditions}"
    compLabels="${compLabels/Punch-through/Punch-through, average 2015 conditions}"
}

function ICHEP2016_OnlyPileup \
{
    # Load default Prerec 2015 release
    ICHEP2016
    # Override variables
    outFile="JetUncertainties-ICHEP2016-Unknown-OnlyPileup.pdf"
    compList="Pileup_#;Pileup_OffsetMu;Pileup_OffsetNPV;Pileup_PtTerm;Pileup_RhoTopology"
    compLabels="Pileup, total (all components);Pileup, <#mu> offset;Pileup, NPV offset;Pileup, #it{p}_{T} bias;Pileup, #rho topology"

}

#######################################

function PreMoriond2017_forMJB \
{

    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017/JESNuisanceParametersForMJB.config"
    outFile="JetUncertainties-IntoMJB_PreMoriond2017.pdf"

    compList="Zjet#,Gjet#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#;"
    compLabels="Absolute #it{in situ} JES (Z,#gamma+jet);Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"

    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;bunchSpacing=25"

}

function Moriond2017 \
{

    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017/JES2016_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2017-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_"
    #options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;path=\"/eos/atlas/atlascerngroupdisk/perf-jets/JetUncertainties/\";fixedEtaVals=0;fixedPtVals=NONE"
}

function Moriond2017_Public \
{
    Moriond2017
    outFile="${outFile/Nominal/Nominal-Public}"
    options="${options};isPublic=True;ATLASLabel=Preliminary;Interp=True;fixedPtVals=25,40,60,80,100,120;"
    compLabels="${compLabels/Pileup/Pileup, average 2016 conditions}"
    compLabels="${compLabels/Punch-through/Punch-through, average 2016 conditions}"
}

function Moriond2017_Public_Dijet \
{
    Moriond2017
    outFile="${outFile/Nominal/Nominal-Public-Dijet}"
    options="isDijet=true;isGSC=true;isLargeR=false;prefix=JET_;isPublic=True;ATLASLabel=Preliminary;Interp=True;fixedPtVals=25,40,60,80,100,120;"
    compLabels="${compLabels/Pileup/Pileup, average 2016 conditions}"
    compLabels="${compLabels/Punch-through/Punch-through, average 2016 conditions}"
}

function Moriond2017_OFCsOnly \
{

    # Load default Ichep 2016 release
    Moriond2017
    # Override config
    outFile="JetUncertainties-Moriond2017-Nominal-OnlyOFCs.pdf"
    configFile="JES_2016/Moriond2017/JES2016_AllNuisanceParameters_OFCs.config"

}

function Moriond2017_CompareToICHEP \
{
    # Load default Ichep 2016 release
    Moriond2017
    # Override variables
    outFile="JetUncertainties-Moriond2017-Nominal-CompareToICHEP2016.pdf"
    options="${options};doCompare=JES_2015/ICHEP2016/JES2015_AllNuisanceParameters.config&MC15&ICHEP uncertainty"
}

function Moriond2017_CompareToICHEP_Dijet \
{
    # Load default Ichep 2016 release
    Moriond2017
    # Override variables
    outFile="JetUncertainties-Moriond2017-Nominal-Dijet-CompareToICHEP2016.pdf"
    options="isDijet=true;isGSC=true;isLargeR=false;prefix=JET_;doCompare=JES_2015/ICHEP2016/JES2015_AllNuisanceParameters.config&MC15&ICHEP uncertainty"
}

function Moriond2017_OnlyPileup \
{
    # Load default Prerec 2015 release
    Moriond2017
    # Override variables
    outFile="JetUncertainties-Moriond2017-OnlyPileup.pdf"
    compList="Pileup_#;Pileup_OffsetMu;Pileup_OffsetNPV;Pileup_PtTerm;Pileup_RhoTopology"
    compLabels="Pileup, total (all components);Pileup, <#mu> offset;Pileup, NPV offset;Pileup, #it{p}_{T} bias;Pileup, #rho topology"

}

function Moriond2017_OnlyFlavour \
{
    # Load default Moriond 2013 release
    Moriond2017
    # Override variables
    outFile="JetUncertainties-Moriond2017-OnlyFlavour.pdf"
    compList="Flavor_#;Flavor_Comp#;Flavor_Resp#"
    compLabels="Flavour, total (all components);Flavour composition;Flavour response"
}

function Moriond2017_GlobalReductions \
{
    # Load default Moriond 2016 release
    Moriond2017
    # Override variables
    configFile="JES_2016/Moriond2017/JES2016_21NP.config"
    outFile="JetUncertainties-Moriond2017-Unknown-GlobalReduction.pdf"
    compList="EffectiveNP#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;"

}

function Moriond2017_CategoryReductions \
{
    # Load default Moriond 2016 release
    Moriond2017
    # Override variables
    configFile="JES_2016/Moriond2017/JES2016_29NP_ByCategory.config"
    outFile="JetUncertainties-Moriond2017-Unknown-CategoryReduction.pdf"
    compList="EffectiveNP#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;"

}

function Moriond2017_SR1 \
{
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017/JES2016_SR_Scenario1_forplotting.config"
    outFile="JetUncertainties-Moriond2017-3NP-Scenario1_test.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3;EtaIntercalibration_NonClosure"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3;#eta-intercalibration nonclosure"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Moriond2017_SR1_NP3Only_Dijet \
{
    # Load default Moriond 2016 release
    Moriond2017
    # Override variables
    configFile="JES_2016/Moriond2017/JES2016_21NP.config"
    outFile="JetUncertainties-Moriond2017-Dijet-NP3Only.pdf"
    compList="SingleParticle#;EtaIntercalibration_Modelling;PunchThrough_#"
    compLabels="Single particle; #eta-intercalibration modelling;Punch through"
    options="isDijet=true;isGSC=true;isLargeR=false;prefix=JET_;"

}

function Moriond2017_SR2 \
{
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017/JES2016_SR_Scenario2_forplotting.config"
    outFile="JetUncertainties-Moriond2017-3NP-Scenario2_test.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3;EtaIntercalibration_NonClosure"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3;#eta-intercalibration nonclosure"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Moriond2017_SR3 \
{
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017/JES2016_SR_Scenario3_forplotting.config"
    outFile="JetUncertainties-Moriond2017-3NP-Scenario3_test.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3;EtaIntercalibration_NonClosure"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3;#eta-intercalibration nonclosure"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Moriond2017_SR4 \
{
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017/JES2016_SR_Scenario4_forplotting.config"
    outFile="JetUncertainties-Moriond2017-3NP-Scenario4_test.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3;EtaIntercalibration_NonClosure"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3;#eta-intercalibration nonclosure"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Moriond2017_SpecialRequest_Top \
{
    Moriond2017
    jetDefinition="AntiKt4EMTopo"
    outFile="${outFile/Nominal/Nominal-TTbar}"
    options="isReginas=true;isGSC=true;isLargeR=false;prefix=JET_;isPublic=True;ATLASLabel=Preliminary;Interp=True;fixedPtVals=25,40,60,80,100,120;"
    compLabels="${compLabels/Pileup/Pileup, average 2016 conditions}"
    compLabels="${compLabels/Punch-through/Punch-through, average 2016 conditions}"

}


#######################################

function Moriond2017_PFlow \
{

    jetDefinition="AntiKt4EMPFlow"
    MCtype="MC15"
    configFile="JES_2016/PFlow2017/JES2016_AllNuisanceParameters.config"
    outFile="JetUncertainties-PFlow2017-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_"
}

function Moriond2017_PFlow_Public \
{
    Moriond2017_PFlow
    outFile="${outFile/Nominal/Nominal-Public}"
    options="${options};isPublic=True;ATLASLabel=Preliminary;Interp=True;fixedPtVals=25,40,60,80,100,120;"
    compLabels="${compLabels/Pileup/Pileup, average 2016 conditions}"
    compLabels="${compLabels/Punch-through/Punch-through, average 2016 conditions}"
}

function Moriond2017_PFlow_Dijet_Public \
{
    Moriond2017_PFlow
    outFile="${outFile/Nominal/Dijet-Public}"
    options="isDijet=true;isGSC=true;isLargeR=false;prefix=JET_;isPublic=True;ATLASLabel=Preliminary;Interp=True;fixedPtVals=25,40,60,80,100,120;"
    compLabels="${compLabels/Pileup/Pileup, average 2016 conditions}"
    compLabels="${compLabels/Punch-through/Punch-through, average 2016 conditions}"
}

#######################################


function ICHEP2016_CompareToMoriond \
{
    # Load default Ichep 2016 release
    ICHEP2016
    # Override variables
    outFile="JetUncertainties-ICHEP2016-Nominal-CompareToMoriond.pdf"
    options="${options};doCompare=JES_2015/Moriond2016/JES2015_AllNuisanceParameters.config&MC15&Moriond uncertainty"
}

function ICHEP2016_Public \
{
    ICHEP2016
    outFile="${outFile/Nominal/Nominal-Public}"
    options="${options};isPublic=True;ATLASLabel=Preliminary;Interp=True"
    compLabels="${compLabels/Pileup/Pileup, average 2015 conditions}"
    compLabels="${compLabels/Punch-through/Punch-through, average 2015 conditions}"
}

function Moriond2017_OnlyPileup \
{
    # Load default Moriond 2013 release
    Moriond2017
    # Override variables
    outFile="JetUncertainties-Moriond2013-OnlyPileup.pdf"
    compList="Pileup_#;Pileup_OffsetMu;Pileup_OffsetNPV;Pileup_PtTerm;Pileup_RhoTopology"
    compLabels="Pileup, total (all components);Pileup, <#mu> offset;Pileup, NPV offset;Pileup, #it{p}_{T} bias;Pileup, #rho topology"
}

#######################################
# Special request for Cate
function ICHEP2016_Compare_EtaIntercal \
{
    ICHEP2016
    outFile="${outFile/Public/Compare-Public}"
    compList="EtaIntercalib#"
    compLabels="Relative #it{in situ} JES"
    options="${options};doCompare=JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_25ns.config&MC15&Preliminary uncertainty"
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

function Prerec2015_3NP_Scenario1_25ns \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/Prerec/PrerecJES2015_3NP_Scenario1_25ns.config"
    outFile="JetUncertainties-Prerec2015-3NP-Scenario1-25ns.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Prerec2015_3NP_Scenario2_25ns \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/Prerec/PrerecJES2015_3NP_Scenario2_25ns.config"
    outFile="JetUncertainties-Prerec2015-3NP-Scenario2-25ns.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Prerec2015_3NP_Scenario3_25ns \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/Prerec/PrerecJES2015_3NP_Scenario3_25ns.config"
    outFile="JetUncertainties-Prerec2015-3NP-Scenario3-25ns.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Prerec2015_3NP_Scenario4_25ns \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/Prerec/PrerecJES2015_3NP_Scenario4_25ns.config"
    outFile="JetUncertainties-Prerec2015-3NP-Scenario4-25ns.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function Prerec2015_3NP_Scenario1_50ns \
{
    Prerec2015_3NP_Scenario1_25ns
    configFile=${configFile/25/50}
    outFile=${outFile/25/50}
}

function Prerec2015_3NP_Scenario2_50ns \
{
    Prerec2015_3NP_Scenario2_25ns
    configFile=${configFile/25/50}
    outFile=${outFile/25/50}
}

function Prerec2015_3NP_Scenario3_50ns \
{
    Prerec2015_3NP_Scenario3_25ns
    configFile=${configFile/25/50}
    outFile=${outFile/25/50}
}

function Prerec2015_3NP_Scenario4_50ns \
{
    Prerec2015_3NP_Scenario4_25ns
    configFile=${configFile/25/50}
    outFile=${outFile/25/50}
}

####################################### Specialty requirements

function PreMoriond2016_forMJB \
{

    # Load default Prerec 2015 release
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo"
    MCtype="MC15"
    configFile="JES_2015/Prerec/JESNuisanceParametersForMJB.config"
    outFile="JetUncertainties-IntoMJB_PreMoriond2016.pdf"

    compList="LAr#,Zjet#,Gjet#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#;"
    compLabels="Absolute #it{in situ} JES (LAr,Z,#gamma+jet);Relative #it{in situ} JES (2015 Prerec);Flav. composition;Flav. response;Pileup, predicted 2015 conditions;Punch-through, predicted 2015 conditions;"

    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;bunchSpacing=25"

}

function PreICHEP2016_forMJB \
{

    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo"
    MCtype="MC15"
    configFile="JES_2015/ICHEP2016/JESNuisanceParametersForMJB.config"
    outFile="JetUncertainties-IntoMJB_PreIchep2016.pdf"

    compList="LAr#,Zjet#,Gjet#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#;"
    compLabels="Absolute #it{in situ} JES (Z,#gamma+jet);Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"

    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;bunchSpacing=25"

}

################################################
## Moriond 2017 uncertainty plots
## for the four fitted JES possibilities
## with all of their relevant reductions

##-------------------------------------------###
## JESFit_0_4

function Moriond2017_0_4 \
{

    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017_0_4/JES2016_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2017_0_4-Unknown-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_"
}

function Moriond2017_0_4_GlobalReduction \
{
    # Load default Moriond 2016 release
    Moriond2017_0_4
    # Override variables
    configFile="JES_2016/Moriond2017_0_4/JES2016_17NP.config"
    outFile="JetUncertainties-Moriond2017_0_4-Unknown-GlobalReduction.pdf"
    compList="EffectiveNP#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;"
}

function Moriond2017_0_4_forTests \
{

    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017_0_4/JES2016_AllNuisanceParameters_forCorrTests.config"
    outFile="JetUncertainties-Moriond2017_0_4-Unknown-Nominal-test.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#;LAr#;Zjet#;Gjet#;MJB#;"
    compLabels="Absolute #it{in situ} JES;LAr;Z+jet;Gamma+jet;Multijet balance"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_"
}

function Moriond2017_0_4_GlobalReduction_forTests \
{
    # Load default Moriond 2016 release
    Moriond2017_0_4_forTests
    # Override variables
    configFile="JES_2016/Moriond2017_0_4/JES2016_17NP_forCorrTests.config"
    outFile="JetUncertainties-Moriond2017_0_4-Unknown-GlobalReduction-test.pdf"
    compList="EffectiveNP#;EffectiveNP_1;EffectiveNP_2;EffectiveNP_3;EffectiveNP_4restTerm;"
    compLabels="In situ total;First global parameter;Second global parameter;Third global parameter;Fourth global parameter"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;"

}


##-------------------------------------------###
## JESFit_m1_4
## Selected nominal: directory renamed Moriond2017_fitted

function Moriond2017_fitted \
{

    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017_fitted/JES2016_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2017_fitted-Unknown-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_"
}

function Moriond2017_fitted_GlobalReduction \
{
    # Load default Moriond 2016 release
    Moriond2017_fitted
    # Override variables
    configFile="JES_2016/Moriond2017_fitted/JES2016_18NP.config"
    outFile="JetUncertainties-Moriond2017_fitted-Unknown-GlobalReduction.pdf"
    compList="EffectiveNP#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;"
}

function Moriond2017_fitted_forLaurie \
{

    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017_fitted/JES2016_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2017_fitted-Unknown-Nominal-forLaurie.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;fixedPtVals=100,200,300"
}

##-------------------------------------------###
## JESFit_m2_4

function Moriond2017_m2_4 \
{

    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017_m2_4/JES2016_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2017_m2_4-Unknown-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_"
}

function Moriond2017_m2_4_GlobalReduction \
{
    # Load default Moriond 2016 release
    Moriond2017_m2_4
    # Override variables
    configFile="JES_2016/Moriond2017_m2_4/JES2016_19NP.config"
    outFile="JetUncertainties-Moriond2017_m2_4-Unknown-GlobalReduction.pdf"
    compList="EffectiveNP#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;"
}

##-------------------------------------------###
## JESFit_m2_5

function Moriond2017_m2_5 \
{

    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017_m2_5/JES2016_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2017_m2_5-Unknown-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_"
}

function Moriond2017_m2_5_GlobalReduction \
{
    # Load default Moriond 2016 release
    Moriond2017_m2_5
    # Override variables
    configFile="JES_2016/Moriond2017_m2_5/JES2016_20NP.config"
    outFile="JetUncertainties-Moriond2017_m2_5-Unknown-GlobalReduction.pdf"
    compList="EffectiveNP#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;"
}

####################################### Random


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



############# 2015 Large-R pre-rec uncertainties #############

#compareLargeRPrerec="true"
#fullSplitLargeRPrerec="true"
#splitScales1LargeRPrerec="true"
#splitScales2LargeRPrerec="true"
#noMassWindowLargeRPrerec="true"
#signalLargeRPrerec="true"

function LargeR_Prerec2015_Hbb \
{
    # July 2014 large-R jet release
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15"
    configFile="UJ_2015/Prerec/Prerec2015_HbbTagging.config"
    outFile="JetUncertainties-LargeR-Prerec2015-Hbb-pT.pdf"
    compList="Hbb_Run1;Hbb_CrossCalib"
    compLabels="Run 1 R_{trk};Cross-calibration"
    options="isDijet=false;isLargeR=true;prefix=JET_;scaleVar=pt"
}

function LargeR_Prerec2015_Hbb_nomerge \
{
    LargeR_Prerec2015_Hbb
    configFile=${configFile/HbbTagging/HbbTagging_NoMerge}
    compList="Hbb_Run1;Hbb_50vs25ns;Hbb_LArSampling;Hbb_TopoclusterAlg;Hbb_NoiseThrUp;Hbb_NoiseThrDown;Hbb_Material"
    compLabels="Run1;50 vs 25 ns;4 vs 5 LAr samples;Topocluster algorithm;Noise threshold up;Noise threshold down;Material"
}

function LargeR_Prerec2015_Hbb_splitscales1 \
{
    LargeR_Prerec2015_Hbb
    configFile=${configFile/HbbTagging/HbbTagging_SplitScales1}
    compList="Hbb_Run1_pT;Hbb_Run1_mass;Hbb_Run1_D2;Hbb_CrossCalib"
    compLabels="Run 1 R_{trk} (pT);Run 1 R_{trk} (mass);Run 1 R_{trk} (D2);Cross-calibration"
}

function LargeR_Prerec2015_Hbb_splitscales2 \
{
    LargeR_Prerec2015_Hbb
    configFile=${configFile/HbbTagging/HbbTagging_SplitScales2}
    compList="Hbb_Run1_pT;Hbb_Run1_mass;Hbb_Run1_D2;Hbb_CrossCalib_pT;Hbb_CrossCalib_mass;Hbb_CrossCalib_D2"
    compLabels="Run 1 R_{trk} (pT);Run 1 R_{trk} (mass);Run 1 R_{trk} (D2);Cross-calibration (pT);Cross-calibration (mass);Cross-calibration (D2)"
}

function LargeR_Prerec2015_Hbb_pT \
{
    if ! [ -z $fullSplitLargeRPrerec ] ; then
        LargeR_Prerec2015_Hbb_nomerge
    elif ! [ -z $splitScales1LargeRPrerec ] ; then
        LargeR_Prerec2015_Hbb_splitscales1
    elif ! [ -z $splitScales2LargeRPrerec ] ; then
        LargeR_Prerec2015_Hbb_splitscales2
    else
        LargeR_Prerec2015_Hbb
    fi
    if ! [ -z $noMassWindowLargeRPrerec ] ; then
        configFile=${configFile/HbbTagging/HbbTagging_NoMassWindow}
    fi
    if ! [ -z $signalLargeRPrerec ] ; then
        configFile=${configFile/HbbTagging/HbbTagging_Signal}
    fi
    if ! [ -z $compareLargeRPrerec ] ; then
        options="${options};doCompare=UJ_2015/Prerec/Prerec2015_HbbTagging_PhysicsLists.config&MC15&Total uncertainty, physics lists"
    fi
}

function LargeR_Prerec2015_Hbb_mass \
{
    if ! [ -z $fullSplitLargeRPrerec ] ; then
        LargeR_Prerec2015_Hbb_nomerge
    elif ! [ -z $splitScales1LargeRPrerec ] ; then
        LargeR_Prerec2015_Hbb_splitscales1
    elif ! [ -z $splitScales2LargeRPrerec ] ; then
        LargeR_Prerec2015_Hbb_splitscales2
    else
        LargeR_Prerec2015_Hbb
    fi
    if ! [ -z $noMassWindowLargeRPrerec ] ; then
        configFile=${configFile/HbbTagging/HbbTagging_NoMassWindow}
    fi
    if ! [ -z $signalLargeRPrerec ] ; then
        configFile=${configFile/HbbTagging/HbbTagging_Signal}
    fi
    outFile=${outFile/-pT/-mass}
    options=${options/scaleVar=pt/scaleVar=mass}
    if ! [ -z $compareLargeRPrerec ] ; then
        options="${options};doCompare=UJ_2015/Prerec/Prerec2015_HbbTagging_PhysicsLists.config&MC15&Total uncertainty, physics lists"
    fi
}

function LargeR_Prerec2015_Hbb_D2 \
{
    if ! [ -z $fullSplitLargeRPrerec ] ; then
        LargeR_Prerec2015_Hbb_nomerge
    elif ! [ -z $splitScales1LargeRPrerec ] ; then
        LargeR_Prerec2015_Hbb_splitscales1
    elif ! [ -z $splitScales2LargeRPrerec ] ; then
        LargeR_Prerec2015_Hbb_splitscales2
    else
        LargeR_Prerec2015_Hbb
    fi
    if ! [ -z $noMassWindowLargeRPrerec ] ; then
        configFile=${configFile/HbbTagging/HbbTagging_NoMassWindow}
    fi
    if ! [ -z $signalLargeRPrerec ] ; then
        configFile=${configFile/HbbTagging/HbbTagging_Signal}
    fi
    outFile=${outFile/-pT/-D2}
    options=${options/scaleVar=pt/scaleVar=D2Beta1}
    if ! [ -z $compareLargeRPrerec ] ; then
        options="${options};doCompare=UJ_2015/Prerec/Prerec2015_HbbTagging_PhysicsLists.config&MC15&Total uncertainty, physics lists"
    fi
}

function LargeR_Prerec2015_Top \
{
    # July 2014 large-R jet release
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15"
    configFile="UJ_2015/Prerec/Prerec2015_TopTagging.config"
    outFile="JetUncertainties-LargeR-Prerec2015-Top-pT.pdf"
    compList="Top_Run1;Top_CrossCalib"
    compLabels="Run 1 R_{trk};Cross-calibration"
    options="isDijet=false;isLargeR=true;prefix=JET_;scaleVar=pt"
}

function LargeR_Prerec2015_Top_nomerge \
{
    LargeR_Prerec2015_Top
    configFile=${configFile/TopTagging/TopTagging_NoMerge}
    compList="Top_Run1;Top_50vs25ns;Top_TopoclusterAlg;Top_NoiseThrUp;Top_NoiseThrDown;Top_Material"
    compLabels="Run1;50 vs 25 ns;Topocluster algorithm;Noise threshold up;Noise threshold down;Material"
}

function LargeR_Prerec2015_Top_splitscales1 \
{
    LargeR_Prerec2015_Top
    configFile=${configFile/TopTagging/TopTagging_SplitScales1}
    compList="Top_Run1_pT;Top_Run1_mass;Top_Run1_Tau32;Top_CrossCalib"
    compLabels="Run 1 R_{trk} (pT);Run 1 R_{trk} (mass);Run 1 R_{trk} (Tau32);Cross-calibration"
}

function LargeR_Prerec2015_Top_splitscales2 \
{
    LargeR_Prerec2015_Top
    configFile=${configFile/TopTagging/TopTagging_SplitScales2}
    compList="Top_Run1_pT;Top_Run1_mass;Top_Run1_Tau32;Top_CrossCalib_pT;Top_CrossCalib_mass;Top_CrossCalib_Tau32"
    compLabels="Run 1 R_{trk} (pT);Run 1 R_{trk} (mass);Run 1 R_{trk} (Tau32);Cross-calibration (pT);Cross-calibration (mass);Cross-calibration (Tau32)"
}

function LargeR_Prerec2015_Top_pT \
{
    if ! [ -z $fullSplitLargeRPrerec ] ; then
        LargeR_Prerec2015_Top_nomerge
    elif ! [ -z $splitScales1LargeRPrerec ] ; then
        LargeR_Prerec2015_Top_splitscales1
    elif ! [ -z $splitScales2LargeRPrerec ] ; then
        LargeR_Prerec2015_Top_splitscales2
    else
        LargeR_Prerec2015_Top
    fi
    if ! [ -z $noMassWindowLargeRPrerec ] ; then
        configFile=${configFile/TopTagging/TopTagging_NoMassWindow}
    fi
    if ! [ -z $signalLargeRPrerec ] ; then
        configFile=${configFile/TopTagging/TopTagging_Signal}
    fi
    if ! [ -z $compareLargeRPrerec ] ; then
        options="${options};doCompare=UJ_2015/Prerec/Prerec2015_TopTagging_PhysicsLists.config&MC15&Total uncertainty, physics lists"
    fi
}

function LargeR_Prerec2015_Top_mass \
{
    if ! [ -z $fullSplitLargeRPrerec ] ; then
        LargeR_Prerec2015_Top_nomerge
    elif ! [ -z $splitScales1LargeRPrerec ] ; then
        LargeR_Prerec2015_Top_splitscales1
    elif ! [ -z $splitScales2LargeRPrerec ] ; then
        LargeR_Prerec2015_Top_splitscales2
    else
        LargeR_Prerec2015_Top
    fi
    if ! [ -z $noMassWindowLargeRPrerec ] ; then
        configFile=${configFile/TopTagging/TopTagging_NoMassWindow}
    fi
    if ! [ -z $signalLargeRPrerec ] ; then
        configFile=${configFile/TopTagging/TopTagging_Signal}
    fi
    outFile=${outFile/-pT/-mass}
    options=${options/scaleVar=pt/scaleVar=mass}
    if ! [ -z $compareLargeRPrerec ] ; then
        options="${options};doCompare=UJ_2015/Prerec/Prerec2015_TopTagging_PhysicsLists.config&MC15&Total uncertainty, physics lists"
    fi
}

function LargeR_Prerec2015_Top_Tau32 \
{
    if ! [ -z $fullSplitLargeRPrerec ] ; then
        LargeR_Prerec2015_Top_nomerge
    elif ! [ -z $splitScales1LargeRPrerec ] ; then
        LargeR_Prerec2015_Top_splitscales1
    elif ! [ -z $splitScales2LargeRPrerec ] ; then
        LargeR_Prerec2015_Top_splitscales2
    else
        LargeR_Prerec2015_Top
    fi
    if ! [ -z $noMassWindowLargeRPrerec ] ; then
        configFile=${configFile/TopTagging/TopTagging_NoMassWindow}
    fi
    if ! [ -z $signalLargeRPrerec ] ; then
        configFile=${configFile/TopTagging/TopTagging_Signal}
    fi
    outFile=${outFile/-pT/-Tau32}
    options=${options/scaleVar=pt/scaleVar=Tau32WTA}
    if ! [ -z $compareLargeRPrerec ] ; then
        options="${options};doCompare=UJ_2015/Prerec/Prerec2015_TopTagging_PhysicsLists.config&MC15&Total uncertainty, physics lists"
    fi
}

function LargeR_Prerec2015_WZ \
{
    # July 2014 large-R jet release
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15"
    configFile="UJ_2015/Prerec/Prerec2015_WZTagging.config"
    outFile="JetUncertainties-LargeR-Prerec2015-WZ-pT.pdf"
    compList="WZ_Run1;WZ_CrossCalib"
    compLabels="Run 1 R_{trk};Cross-calibration"
    options="isDijet=false;isLargeR=true;prefix=JET_;scaleVar=pt"
}

function LargeR_Prerec2015_WZ_nomerge \
{
    LargeR_Prerec2015_WZ
    configFile=${configFile/WZTagging/WZTagging_NoMerge}
    compList="WZ_Run1;WZ_50vs25ns;WZ_LArSampling;WZ_TopoclusterAlg;WZ_NoiseThrUp;WZ_NoiseThrDown;WZ_Material"
    compLabels="Run1;50 vs 25 ns;4 vs 5 LAr samples;Topocluster algorithm;Noise threshold up;Noise threshold down;Material"
}

function LargeR_Prerec2015_WZ_splitscales1 \
{
    LargeR_Prerec2015_WZ
    configFile=${configFile/WZTagging/WZTagging_SplitScales1}
    compList="WZ_Run1_pT;WZ_Run1_mass;WZ_Run1_D2;WZ_CrossCalib"
    compLabels="Run 1 R_{trk} (pT);Run 1 R_{trk} (mass);Run 1 R_{trk} (D2);Cross-calibration"
}

function LargeR_Prerec2015_WZ_splitscales2 \
{
    LargeR_Prerec2015_WZ
    configFile=${configFile/WZTagging/WZTagging_SplitScales2}
    compList="WZ_Run1_pT;WZ_Run1_mass;WZ_Run1_D2;WZ_CrossCalib_pT;WZ_CrossCalib_mass;WZ_CrossCalib_D2"
    compLabels="Run 1 R_{trk} (pT);Run 1 R_{trk} (mass);Run 1 R_{trk} (D2);Cross-calibration (pT);Cross-calibration (mass);Cross-calibration (D2)"
}

function LargeR_Prerec2015_WZ_pT \
{
    if ! [ -z $fullSplitLargeRPrerec ] ; then
        LargeR_Prerec2015_WZ_nomerge
    elif ! [ -z $splitScales1LargeRPrerec ] ; then
        LargeR_Prerec2015_WZ_splitscales1
    elif ! [ -z $splitScales2LargeRPrerec ] ; then
        LargeR_Prerec2015_WZ_splitscales2
    else
        LargeR_Prerec2015_WZ
    fi
    if ! [ -z $noMassWindowLargeRPrerec ] ; then
        configFile=${configFile/WZTagging/WZTagging_NoMassWindow}
    fi
    if ! [ -z $signalLargeRPrerec ] ; then
        configFile=${configFile/WZTagging/WZTagging_Signal}
    fi
    if ! [ -z $compareLargeRPrerec ] ; then
        options="${options};doCompare=UJ_2015/Prerec/Prerec2015_WZTagging_PhysicsLists.config&MC15&Total uncertainty, physics lists"
    fi
}

function LargeR_Prerec2015_WZ_mass \
{
    if ! [ -z $fullSplitLargeRPrerec ] ; then
        LargeR_Prerec2015_WZ_nomerge
    elif ! [ -z $splitScales1LargeRPrerec ] ; then
        LargeR_Prerec2015_WZ_splitscales1
    elif ! [ -z $splitScales2LargeRPrerec ] ; then
        LargeR_Prerec2015_WZ_splitscales2
    else
        LargeR_Prerec2015_WZ
    fi
    if ! [ -z $noMassWindowLargeRPrerec ] ; then
        configFile=${configFile/WZTagging/WZTagging_NoMassWindow}
    fi
    if ! [ -z $signalLargeRPrerec ] ; then
        configFile=${configFile/WZTagging/WZTagging_Signal}
    fi
    outFile=${outFile/-pT/-mass}
    options=${options/scaleVar=pt/scaleVar=mass}
    if ! [ -z $compareLargeRPrerec ] ; then
        options="${options};doCompare=UJ_2015/Prerec/Prerec2015_WZTagging_PhysicsLists.config&MC15&Total uncertainty, physics lists"
    fi
}

function LargeR_Prerec2015_WZ_D2 \
{
    if ! [ -z $fullSplitLargeRPrerec ] ; then
        LargeR_Prerec2015_WZ_nomerge
    elif ! [ -z $splitScales1LargeRPrerec ] ; then
        LargeR_Prerec2015_WZ_splitscales1
    elif ! [ -z $splitScales2LargeRPrerec ] ; then
        LargeR_Prerec2015_WZ_splitscales2
    else
        LargeR_Prerec2015_WZ
    fi
    if ! [ -z $noMassWindowLargeRPrerec ] ; then
        configFile=${configFile/WZTagging/WZTagging_NoMassWindow}
    fi
    if ! [ -z $signalLargeRPrerec ] ; then
        configFile=${configFile/WZTagging/WZTagging_Signal}
    fi
    outFile=${outFile/-pT/-D2}
    options=${options/scaleVar=pt/scaleVar=D2Beta1}
    if ! [ -z $compareLargeRPrerec ] ; then
        options="${options};doCompare=UJ_2015/Prerec/Prerec2015_WZTagging_PhysicsLists.config&MC15&Total uncertainty, physics lists"
    fi
}





function LargeR_Moriond2016_Rtrk_Public \
{
    # July 2014 large-R jet release
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15"
    configFile="UJ_2015/Moriond2016/UJ2015_WZTagging_medium.config"
    outFile="JetUncertainties-Moriond2016-Rtrk-public-mass.pdf"
    compList="Rtrk_Baseline;Rtrk_Modelling;Rtrk_Tracking#"
    compLabels="R_{trk} baseline (Pythia);R_{trk} modelling (Herwig);R_{trk} tracking"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;isPublic=true;scaleVar=mass"
}
function LargeR_Moriond2016_Rtrk \
{
    # July 2014 large-R jet release
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15"
    configFile="UJ_2015/Moriond2016/UJ2015_WZTagging.config"
    outFile="JetUncertainties-Moriond2016-Rtrk-pT.pdf"
    compList="Rtrk_Baseline;Rtrk_Modelling;Rtrk_Tracking1;Rtrk_Tracking2;Rtrk_Tracking3"
    compLabels="R_{trk} baseline;R_{trk} modelling;R_{trk} tracking 1;R_{trk} tracking 2;R_{trk} tracking 3"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;totalUncName=\"Total Uncertainty, 13 TeV in situ\";scaleVar=pt"
}
function LargeR_Moriond2016_RtrkCombTrack \
{
    # July 2014 large-R jet release
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15"
    configFile="UJ_2015/Moriond2016/UJ2015_WZTagging.config"
    outFile="JetUncertainties-Moriond2016-RtrkComb-pT.pdf"
    compList="Rtrk_Baseline;Rtrk_Modelling;Rtrk_Tracking#"
    compLabels="R_{trk} baseline;R_{trk} modelling;R_{trk} tracking"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;totalUncName=\"Total Uncertainty, 13 TeV in situ\";scaleVar=pt"
}
function LargeR_Moriond2016_Tracking \
{
    # July 2014 large-R jet release
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15"
    configFile="UJ_2015/Moriond2016/UJ2015_TrackingComp.config"
    outFile="JetUncertainties-Moriond2016-Tracking-pT.pdf"
    compList="Rtrk_Tracking1;Rtrk_Tracking2;Rtrk_Tracking3"
    compLabels="R_{trk} tracking 1;R_{trk} tracking 2;R_{trk} tracking 3"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;drawTotal=false;scaleVar=pt"
}
function LargeR_Moriond2016_WZ \
{
    # July 2014 large-R jet release
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15"
    configFile="UJ_2015/Moriond2016/UJ2015_WZTagging.config"
    outFile="JetUncertainties-Moriond2016-WZ-pT.pdf"
    compList="Rtrk_Baseline;Rtrk_Modelling;Rtrk_Tracking1;Rtrk_Tracking2;Rtrk_Tracking3"
    compLabels="R_{trk} baseline;R_{trk} modelling;R_{trk} tracking 1;R_{trk} tracking 2;R_{trk} tracking 3"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;doCompare=UJ_2015/Prerec/Prerec2015_WZTagging.config&MC15&\"Pre-recommendation, W/Z tagged\";totalUncName=\"Total Uncertainty, 13 TeV in situ\";scaleVar=pt"
}
function LargeR_Moriond2016_Top \
{
    # July 2014 large-R jet release
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15"
    configFile="UJ_2015/Moriond2016/UJ2015_TopTagging.config"
    outFile="JetUncertainties-Moriond2016-top-pT.pdf"
    compList="Rtrk_Baseline;Rtrk_Modelling;Rtrk_Tracking1;Rtrk_Tracking2;Rtrk_Tracking3"
    compLabels="R_{trk} baseline;R_{trk} modelling;R_{trk} tracking 1;R_{trk} tracking 2;R_{trk} tracking 3"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;doCompare=UJ_2015/Prerec/Prerec2015_TopTagging.config&MC15&\"Pre-recommendation, top tagged\";totalUncName=\"Total Uncertainty, 13 TeV in situ\";scaleVar=pt"
}
function LargeR_Moriond2016_Hbb \
{
    # July 2014 large-R jet release
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15"
    configFile="UJ_2015/Moriond2016/UJ2015_HbbTagging.config"
    outFile="JetUncertainties-Moriond2016-Hbb-pT.pdf"
    compList="Rtrk_Baseline;Rtrk_Modelling;Rtrk_Tracking1;Rtrk_Tracking2;Rtrk_Tracking3"
    compLabels="R_{trk} baseline;R_{trk} modelling;R_{trk} tracking 1;R_{trk} tracking 2;R_{trk} tracking 3"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;doCompare=UJ_2015/Prerec/Prerec2015_HbbTagging.config&MC15&\"Pre-recommendation, Hbb tagged\";totalUncName=\"Total Uncertainty, 13 TeV in situ\";scaleVar=pt"
}
tagType=WZ
scale=D2Beta1
NMW="NMW"
corr=strong
function LargeR_Moriond2016_VALIDATE \
{
    # July 2014 large-R jet release
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15"
    if [ -z "$NMW" ] ; then
        configFile="UJ_2015/Moriond2016/UJ2015_${tagType}Tagging_${corr}.config"
        outFile="JetUncertainties-Moriond2016-${tagType}-$corr-${scale}.pdf"
    else
        configFile="UJ_2015/Moriond2016/UJ2015_${tagType}Tagging_NoMassWindow_${corr}.config"
        outFile="JetUncertainties-Moriond2016-${tagType}-NMW-$corr-${scale}.pdf"
    fi
    if [[ $corr = weak ]] ; then
        compList="Rtrk_Baseline_pT;Rtrk_Baseline_mass;Rtrk_Modelling_pT;Rtrk_Modelling_mass;Rtrk_Tracking_pT;Rtrk_Tracking_mass;${tagType}_Run1_#;${tagType}_Cross#"
        compLabels="R_{trk} baseline (pT);R_{trk} baseline (mass);R_{trk} modelling (pT);R_{trk} modelling (mass);R_{trk} tracking (pT);R_trk tracking (mass);Run-1 R_{trk} ($scale$NMW);Cross-calib ($scale$NMW)"
    elif [[ $corr = medium ]] ; then
        compList="Rtrk_Baseline;Rtrk_Modelling;Rtrk_Tracking;${tagType}_Run1_#;${tagType}_Cross#"
        compLabels="R_{trk} baseline (pT+mass);R_{trk} modelling (pT+mass);R_{trk} tracking (pT+mass);Run-1 R_{trk} ($scale$NMW);Cross-calib ($scale$NMW)"
    elif [[ $corr = strong ]] ; then
        compList="Rtrk_Baseline;Rtrk_ModellingAndTracking;${tagType}_Run1_#;${tagType}_Cross#"
        compLabels="R_{trk} baseline (pT+mass);R_{trk} modelling+tracking (pT+mass);Run-1 R_{trk} ($scale$NMW);Cross-calib ($scale$NMW)"
    fi
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;doCompare=UJ_2015/Prerec/Prerec2015_HbbTagging.config&MC15&\"Pre-recommendation, $tagType tagged\";totalUncName=\"Total Uncertainty, 13 TeV in situ\";scaleVar=${scale}"
}


# Kate going to use for 2015 paper


function Moriond2016_SR_Scenario1 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/Moriond2016/JES2015_SR_Scenario1.config"
    outFile="JetUncertainties-Moriond2016-SR-Scenario1.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3" #;EtaIntercalibration_NonClosure"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3" #;#eta-intercalibration nonclosure"
    #compList="GroupedNP_1;GroupedNP_2;GroupedNP_3;EtaIntercalibration_NonClosure;RelativeNonClosure_#"
    #compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3;#eta-intercalibration nonclosure;MC nonclosure (if applicable)"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}
function Moriond2016_SR_Scenario2 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/Moriond2016/JES2015_SR_Scenario2.config"
    outFile="JetUncertainties-Moriond2016-SR-Scenario2.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3" #;EtaIntercalibration_NonClosure"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3" #;#eta-intercalibration nonclosure"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}
function Moriond2016_SR_Scenario3 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/Moriond2016/JES2015_SR_Scenario3.config"
    outFile="JetUncertainties-Moriond2016-SR-Scenario3.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3" #;EtaIntercalibration_NonClosure"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3" #;#eta-intercalibration nonclosure"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}
function Moriond2016_SR_Scenario4 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/Moriond2016/JES2015_SR_Scenario4.config"
    outFile="JetUncertainties-Moriond2016-SR-Scenario4.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3" #;EtaIntercalibration_NonClosure"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3" #;#eta-intercalibration nonclosure"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}


function TLA2015 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="TLA_2015/JES2015_TLA_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2016-TLA.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;TLA_ScaleFactor"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Trigger Flav. composition;Trigger Flav. response;Trigger Pileup, average 2015 conditions;Trigger data-derived correction"
    options="isDijet=false;isLargeR=false;prefix=JET_;doCompare=JES_2015/Moriond2016/JES2015_AllNuisanceParameters.config&MC15&\"Total Uncertainty, Offline\";totalUncName=\"Total Uncertainty, Trigger\";ptBins=U&1000&75&1000;logPt=false;isPublic=true;isTLA=true"
}

function TLA2015_SR1 \
{
    TLA2015
    configFile="TLA_2015/JES2015_SR_TLA_Scenario1.config"
    outFile="JetUncertainties-Moriond2016-TLA-SR1.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3;EtaIntercalibration_NonClosure;TLA_ScaleFactor"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3;#eta-intercalibration nonclosure;TLA scale factor"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function TLA2015_SR2 \
{
    TLA2015_SR1
    configFile="${configFile/Scenario1/Scenario2}"
    outFile="${outFile/SR1/SR2}"
}

function TLA2015_SR3 \
{
    TLA2015_SR1
    configFile="${configFile/Scenario1/Scenario3}"
    outFile="${outFile/SR1/SR3}"
}

function TLA2015_SR4 \
{
    TLA2015_SR1
    configFile="${configFile/Scenario1/Scenario4}"
    outFile="${outFile/SR1/SR4}"
}

function TLA2015_flavour \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="TLA_2015/JES2015_TLA_AllNuisanceParameters.config;JES_2015/Moriond2016/JES2015_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2016-TLA-flavour-quadsum.pdf"
    #outFile="JetUncertainties-Moriond2016-TLA-flavour.pdf"
    compList="TOTAL;Flavor_Comp#,Flavor_Resp#@Flavor_Comp#,Flavor_Resp#"
    compLabels="Total uncertainty, TLA 2015;Flav. composition #oplus response (no GSC)@Flav. composition #oplus response (with GSC)"
    #compList="TOTAL;Flavor_Comp#;Flavor_Resp#@Flavor_Comp#;Flavor_Resp#"
    #compLabels="Total uncertainty, TLA 2015;Flav. composition (no GSC);Trigger Flav. response (no GSC)@Flav. composition (with GSC);Flav. response (with GSC)"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=false;ptBins=U&1000&75&1000;logPt=false;isPublic=false;isTLA=true"
}

###########################################
# Kate: TLA, 2016 dataset

function TLA2016 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="TLA_2016/TLA_2016/JES2016_TLA_AllNuisanceParameters.config"
    outFile="JetUncertainties-TLA2016.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;TLA_ScaleFactor"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Trigger Flav. composition;Trigger Flav. response;Trigger Pileup, average 2016 conditions;Trigger data-derived correction"
    options="isDijet=false;isLargeR=false;prefix=JET_;doCompare=JES_2016/Moriond2017/JES2016_AllNuisanceParameters.config&MC15&\"Total Uncertainty, Offline\";totalUncName=\"Total Uncertainty, Trigger\";ptBins=U&1500&55&1500;logPt=false;isPublic=true;isTLA=true"
}

function TLA2016_SR1 \
{
    TLA2016
    configFile="TLA_2016/TLA_2016/JES2016_SR_TLA_Scenario1.config"
    outFile="JetUncertainties-TLA2016-SR1.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3;EtaIntercalibration_NonClosure;TLA_ScaleFactor"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3;#eta-intercalibration nonclosure;TLA scale factor"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}

function TLA2016_SR2 \
{
    TLA2016_SR1
    configFile="${configFile/Scenario1/Scenario2}"
    outFile="${outFile/SR1/SR2}"
}

function TLA2016_SR3 \
{
    TLA2016_SR1
    configFile="${configFile/Scenario1/Scenario3}"
    outFile="${outFile/SR1/SR3}"
}

function TLA2016_SR4 \
{
    TLA2016_SR1
    configFile="${configFile/Scenario1/Scenario4}"
    outFile="${outFile/SR1/SR4}"
}

function TLA2016_flavour \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="TLA_2016/TLA_2016/JES2016_TLA_AllNuisanceParameters.config;JES_2016/Moriond2017/JES2016_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2017-TLA-flavour-quadsum.pdf"
    #outFile="JetUncertainties-Moriond2016-TLA-flavour.pdf"
    compList="TOTAL;Flavor_Comp#,Flavor_Resp#@Flavor_Comp#,Flavor_Resp#"
    compLabels="Total uncertainty, TLA 2016;Flav. composition #oplus response (no GSC)@Flav. composition #oplus response (with GSC)"
    #compList="TOTAL;Flavor_Comp#;Flavor_Resp#@Flavor_Comp#;Flavor_Resp#"
    #compLabels="Total uncertainty, TLA 2015;Flav. composition (no GSC);Trigger Flav. response (no GSC)@Flav. composition (with GSC);Flav. response (with GSC)"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=false;ptBins=U&1500&85&1500;logPt=false;isPublic=false;isTLA=true"
}

############# End TLA ####################


function testGlobalCompareBase \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC12;MC12;MC15;MC15"
    configFile="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config;JES_2012/Final/InsituJES2012_14NP.config;JES_2015/Moriond2016/JES2015_AllNuisanceParameters.config;JES_2015/Moriond2016/JES2015_19NP.config"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=False;axisMin=0;axisMax=0.05;absVal=false"
}

function testGlobalCompareBaseRed \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC12;MC15"
    configFile="JES_2012/Final/InsituJES2012_14NP.config;JES_2015/Moriond2016/JES2015_19NP.config"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=False;axisMin=-0.025;axisMax=0.025;absVal=false"
}


function testGlobalCompareNP12012 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC12;MC12"
    configFile="JES_2012/Final/InsituJES2012_14NP.config;JES_2012/Final/InsituJES2012_AllNuisanceParameters.config"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=False;axisMin=0;axisMax=0.05"
    outFile="JetUncertainties-Final2012-Moriond2016-GlobalReductions-NP1-2012.pdf"
    compList="EffectiveNP_1@Zjet_MC,Zjet_Kterm,Gjet_OOC;Zjet_MC;Zjet_Kterm;Gjet_OOC"
    compLabels="Global NP1 (2012)@Zjet MC #oplus Zjet K-term #oplus #gammajet OOC (2012);Zjet MC (2012);Zjet K-term (2012);#gammajet OOC (2012)"
}

function testGlobalCompareNP12015 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15;MC15"
    configFile="JES_2015/Moriond2016/JES2015_19NP.config;JES_2015/Moriond2016/JES2015_AllNuisanceParameters.config"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=False;axisMin=0;axisMax=0.05"
    outFile="JetUncertainties-Final2012-Moriond2016-GlobalReductions-NP1-2015.pdf"
    compList="EffectiveNP_1@Zjet_MC,Zjet_Kterm,Gjet_OOC;Zjet_MC;Zjet_Kterm;Gjet_OOC"
    compLabels="Global NP1 (2015)@Zjet MC #oplus Zjet K-term #oplus #gammajet OOC (2015);Zjet MC (2015);Zjet K-term (2015);#gammajet OOC (2015)"
}

function testGlobalCompareNP1all \
{
    testGlobalCompareBase
    outFile="JetUncertainties-Final2012-Moriond2016-GlobalReductions-NP1all.pdf"
    compList="Zjet_MC;Zjet_Kterm;Gjet_OOC@EffectiveNP_1@Zjet_Kterm;Zjet_MC;Gjet_OOC@EffectiveNP_1"
    compLabels="Zjet MC (2012);Zjet K-term (2012);#gammajet OOC (2012)@Global NP1 (2012)@Zjet K-term (2015);Zjet MC (2015);#gammajet OOC (2015)@Global NP1 (2015)"
}

function testGlobalCompareNP1 \
{
    testGlobalCompareBase
    outFile="JetUncertainties-Final2012-Moriond2016-GlobalReductions-NP1.pdf"
    compList="Zjet_MC,Zjet_Kterm,Gjet_OOC#@EffectiveNP_1@Zjet_Kterm,Zjet_MC,Gjet_OOC#@EffectiveNP_1"
    compLabels="Zjet MC #oplus Zjet K-term #oplus #gammajet OOC (2012)@Global NP1 (2012)@Zjet MC #oplus Zjet K-term #oplus #gammajet OOC (2015)@Global NP1 (2015)"
}

function testGlobalCompareNP2 \
{
    testGlobalCompareBaseRed
    outFile="JetUncertainties-Final2012-Moriond2016-GlobalReductions-NP2.pdf"
    compList="EffectiveNP_2@EffectiveNP_2"
    compLabels="Global NP2 (2012)@Global NP2 (2015)"
}

function testGlobalCompareNP3 \
{
    testGlobalCompareBaseRed
    outFile="JetUncertainties-Final2012-Moriond2016-GlobalReductions-NP3.pdf"
    compList="EffectiveNP_3@EffectiveNP_3"
    compLabels="Global NP3 (2012)@Global NP3 (2015)"
}

function testGlobalCompareNP4 \
{
    testGlobalCompareBaseRed
    outFile="JetUncertainties-Final2012-Moriond2016-GlobalReductions-NP4.pdf"
    compList="EffectiveNP_4@EffectiveNP_4"
    compLabels="Global NP4 (2012)@Global NP4 (2015)"
}

function testGlobalCompareNP5 \
{
    testGlobalCompareBaseRed
    outFile="JetUncertainties-Final2012-Moriond2016-GlobalReductions-NP5.pdf"
    compList="EffectiveNP_5@EffectiveNP_5"
    compLabels="Global NP5 (2012)@Global NP5 (2015)"
}

function testGlobalCompareNP6 \
{
    testGlobalCompareBaseRed
    outFile="JetUncertainties-Final2012-Moriond2016-GlobalReductions-NP6.pdf"
    compList="EffectiveNP_6restTerm@EffectiveNP_6restTerm"
    compLabels="Global NP6, residual term (2012)@Global NP6, residual term (2015)"
}

function testGlobalCompareFlavour \
{
    testGlobalCompareBase
    outFile="JetUncertainties-Final2012-Moriond2016-GlobalReductions-flavour.pdf"
    compList="Flavor_Composition@Flavor_Response@Flavor_Composition@Flavor_Response"
    compLabels="Flavor composition (2012)@Flavor response (2012)@Flavor composition (2015)@Flavor response (2015)"
    options=${options/absVal=false/absVal=true}
}



function TAM2016 \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="LRJ_2016/Prerec/TopTagging.config"
    outFile="JetUncertainties-TAM-Top-D2.pdf"
    #compList="Rtrk_Baseline;Rtrk_Modelling;Rtrk_Tracking1;Rtrk_Tracking2;Rtrk_Tracking3;TAM_Hadronisation#;TAM_Efficiency#;TAM_Fakes#"
    #compLabels="Rtrk baseline;Rtrk modelling;Rtrk tracking1;Rtrk tracking2;Rtrk tracking3;TAM had;TAM eff;TAM fakes"
    compList="Rtrk_Baseline;Rtrk_Modelling;Rtrk_Tracking1;Rtrk_Tracking2;Rtrk_Tracking3"
    compLabels="Rtrk baseline;Rtrk modelling;Rtrk tracking1;Rtrk tracking2;Rtrk tracking3"
    #compList="Rtrk_Baseline;TAM_Hadronisation;TAM_Efficiency;TAM_Fakes"
    #compLabels="Rtrk baseline (pT);TAM hadronisation;TAM efficiency;TAM fakes"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;doCompare=UJ_2015/Moriond2016/UJ2015_TopTagging_medium.config&MC15&\"Moriond 2016 uncertainty, top tagged\";totalUncName=\"2016 data pre-recommendation\";scaleVar=D2Beta1"
    #options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;totalUncName=\"2016 data pre-recommendation\";scaleVar=mass"
}

function TAM2016Grouped \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="LRJ_2016/Prerec/TopTaggingGrouped.config"
    outFile="JetUncertainties-TAM-TopGrouped-mass.pdf"
    compList="Baseline_calo;Baseline_TA;Modelling;Tracking1;Tracking2;Tracking3"
    compLabels="Baseline (calo);Baseline (TA);Modelling;Tracking 1;Tracking 2;Tracking 3"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;doCompare=UJ_2015/Moriond2016/UJ2015_TopTagging_medium.config&MC15&\"Moriond 2016 uncertainty, top tagged\";totalUncName=\"2016 data pre-recommendation\";scaleVar=CombMass_calo&CombMass_TA&mass"
}

function TAM2016Grouped2 \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="LRJ_2016/Prerec/TopTaggingGrouped2.config"
    outFile="JetUncertainties-TAM-TopGrouped-mass.pdf"
    compList="Baseline_Calo;Baseline_TA;Modelling;Tracking1;Tracking2;Tracking3"
    compLabels="Baseline (calo);Baseline (TA);Modelling (both);Tracking 1 (both);Tracking 2 (both);Tracking 3 (calo)"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;doCompare=UJ_2015/Moriond2016/UJ2015_TopTagging_medium.config&MC15&\"Moriond 2016 uncertainty, top tagged\";totalUncName=\"2016 data pre-recommendation\";scaleVar=mass"
}

function CompareLargeR2016ByComp \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15;MC15c;MC15c"
    configFile="UJ_2015/Moriond2016/UJ2015_TopTagging.config;LRJ_2016/Prerec/TopTagging_v1.config;LRJ_2016/Prerec/TopTagging.config"
    outFile="JetUncertainties-LRJ2016-Comparison-mass.pdf"
    compList="Rtrk_Baseline@Rtrk_Baseline@Rtrk_Baseline"
    compLabels="Rtrk Baseline (Moriond)@Rtrk Baseline (last week, unsmoothed)@Rtrk Baseline (now, unsmoothed)"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;DrawTotal=false;scaleVar=mass"
}

function CompareLargeR2016ByComp2 \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c;MC15c"
    configFile="LRJ_2016/Prerec/TopTagging_v1.config;LRJ_2016/Prerec/TopTagging.config"
    outFile="JetUncertainties-LRJ2016-Comparison2-mass.pdf"
    compList="TAM_Fakes_all@TAM_Fakes_all"
    compLabels="TAM Fakes (last week, unsmoothed)@TAM Fakes (now, unsmoothed)"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;DrawTotal=false;scaleVar=mass"
}

function CompareLargeR2016ByCompEta \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c;MC15c;MC15c"
    configFile="LRJ_2016/Prerec/TopTagging_central.config;LRJ_2016/Prerec/TopTagging_forward.config;LRJ_2016/Prerec/TopTagging.config"
    outFile="JetUncertainties-LRJ2016-ComparisonEta-mass.pdf"
    compList="TAM_Fakes_central@TAM_Fakes_forward@TAM_Fakes_all"
    compLabels="TAM Fakes (|#eta|<0.4)@TAM Fakes (0.4<|#eta|<2.0)@TAM Fakes (|#eta|<2.0)"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;DrawTotal=false;scaleVar=mass"
}

function CompareLargeR2016ByTotalEta \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c;MC15c;MC15c"
    configFile="LRJ_2016/Prerec/TopTagging_central.config;LRJ_2016/Prerec/TopTagging_forward.config;LRJ_2016/Prerec/TopTagging.config"
    outFile="JetUncertainties-LRJ2016-ComparisonEtaTotal-mass.pdf"
    compList="TOTAL@TOTAL@TOTAL"
    compLabels="Total uncertainty, |#eta|<0.4@Total uncertainty, 0.4<|#eta|<2.0@Total uncertainty, |#eta|<2.0"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;DrawTotal=false;scaleVar=mass"
}

function CompareLargeR2016ByTotalEtaGrouped \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c;MC15c;MC15c"
    configFile="LRJ_2016/Prerec/TopTaggingGrouped2_central.config;LRJ_2016/Prerec/TopTaggingGrouped2_forward.config;LRJ_2016/Prerec/TopTaggingGrouped2.config"
    outFile="JetUncertainties-LRJ2016-ComparisonEtaTotal-mComb.pdf"
    compList="TOTAL@TOTAL@TOTAL"
    compLabels="Total uncertainty, |#eta|<0.4@Total uncertainty, 0.4<|#eta|<2.0@Total uncertainty, |#eta|<2.0"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;DrawTotal=false;scaleVar=mass"
}


function LargeR_Final2012_Moriond2016 \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15"
    configFile="UJ_2015/Moriond2016/UJ2015_TopTagging_medium.config"
    outFile="JetUncertainties-LargeR-Final2012-Moriond2016-mass.pdf"
    compList="Rtrk_Baseline;Rtrk_Modelling;Rtrk_Tracking"
    compLabels="R_{trk} baseline;R_{trk} modelling;R_{trk} tracking"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;doCompare=UJ_2012/JMS2012_Jul242014_Combined.config&MC12a&\"Final 2012 uncertainty\"&AntiKt10LCTopoTrimmedPtFrac5SmallR30;totalUncName=\"Moriond 2016 uncertainty\";scaleVar=mass"
}


function LargeR_ICHEP2016 \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2015/ICHEP2016/MultiTagging_all.config"
    outFile="JetUncertainties-LargeR-ICHEP2016-SCALEVAR.pdf"
    #compList="#Baseline#;#Modelling#;#Tracking#;#TotalStat#"
    #compLabels="Baseline;Modelling;Tracking;Total stat."
    compList="#Baseline#;#Modelling#;#Tracking1#;#Tracking2#;#Tracking3#;#TotalStat#"
    compLabels="Baseline;Modelling;Tracking (efficiency);Tracking (fakes);Tracking (q/p_{T});Total stat."
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;doCompare=UJ_2015/Moriond2016/UJ2015_TOPOLOGY_weak.config&MC15&\"Moriond2016 total uncertainty\";totalUncName=\"ICHEP2016 total uncertainty\";scaleVar=SCALEVAR"
}

function LargeR_ICHEP2016_pt \
{
    LargeR_ICHEP2016
    outFile="${outFile/SCALEVAR/pt}"
    options="${options/TOPOLOGY/TopTagging}"
    options="${options/SCALEVAR/pt}"
}

function LargeR_ICHEP2016_mass \
{
    LargeR_ICHEP2016
    outFile="${outFile/SCALEVAR/mass}"
    options="${options/TOPOLOGY/TopTagging}"
    options="${options/SCALEVAR/mass}"
    options="${options};massDef=calo"
}

function LargeR_ICHEP2016_mTA \
{
    LargeR_ICHEP2016
    configFile="${configFile/MultiTagging/MultiTagging_TAMass}"
    outFile="${outFile/SCALEVAR/mTA}"
    options="${options/TOPOLOGY/TopTagging}"
    options="${options/SCALEVAR/mass}"
    options="${options};massDef=TA"
}

function LargeR_ICHEP2016_mComb \
{
    LargeR_ICHEP2016
    configFile="${configFile/MultiTagging/MultiTagging_CombMass}"
    outFile="${outFile/SCALEVAR/mComb}"
    options="${options/TOPOLOGY/TopTagging}"
    options="${options/SCALEVAR/mass}"
    options="${options};massDef=comb"
}

function LargeR_ICHEP2016_tau32 \
{
    LargeR_ICHEP2016
    outFile="${outFile/SCALEVAR/tau32}"
    options="${options/TOPOLOGY/TopTagging}"
    options="${options/SCALEVAR/Tau32WTA}"
}

function LargeR_ICHEP2016_D2 \
{
    LargeR_ICHEP2016
    outFile="${outFile/SCALEVAR/D2}"
    options="${options/TOPOLOGY/WZTagging}"
    options="${options/SCALEVAR/D2Beta1}"
}

function LargeR_ICHEP2016_allmass \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2015/ICHEP2016/AllMass.config"
    outFile="JetUncertainties-LargeR-ICHEP2016-allmass.pdf"
    compList="Calo_Baseline;TAM_Baseline;Calo_Modelling;TAM_Modelling;Calo_Tracking1;TAM_Tracking1;Calo_Tracking2;TAM_Tracking2;Calo_Tracking3"
    compLabels="Baseline (calo);Baseline (TA);Modelling (calo);Modelling (TA);Tracking eff. (calo);Tracking eff. (TA);Tracking fakes (calo);Tracking fakes (TA);Tracking q/pT (calo)"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;drawTotal=false;scaleVar=mass;massDef=calo+TA"
}

function LargeR_ICHEP2016_compareMass \
{
    local appendConfig="" #"_withcap"
    local topology="Multi" #"Top"

    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2015/ICHEP2016/${topology}Tagging_CombMass_all${appendConfig}.config;UJ_2015/ICHEP2016/${topology}Tagging_CombMass_all${appendConfig}.config;UJ_2015/ICHEP2016/${topology}Tagging_all.config;UJ_2015/ICHEP2016/${topology}Tagging_TAMass_all.config;UJ_2015/ICHEP2016/${topology}Tagging_CombMass_all${appendConfig}.config"
    outFile="JetUncertainties-LargeR-ICHEP2016-masscomp-${topology}${appendConfig}.pdf"
    compList="CaloWeight@TAWeight@TOTAL@TOTAL@TOTAL"
    compLabels="(Calorimeter weight)/10@(Track-assisted weight)/10@Calorimeter mass@Track-assisted mass@Combined mass"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;drawTotal=false;scaleVar=mass"
}
function LargeR_ICHEP2016_combVsCalo \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2015/ICHEP2016/MultiTagging_CombMass_all.config"
    outFile="JetUncertainties-LargeR-ICHEP2016-CombMass-CaloMass.pdf"
    compList="#Baseline#;#Modelling#;#Tracking1#;#Tracking2#;#Tracking3#;#TotalStat#"
    compLabels="Baseline;Modelling;Tracking (efficiency);Tracking (fakes);Tracking (q/p_{T});Total stat."
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;doCompare=UJ_2015/ICHEP2016/MultiTagging_weak.config&MC15c&\"Calorimeter mass total uncertainty\";totalUncName=\"Combined mass total uncertainty\";scaleVar=mass"
}

function LargeR_ICHEP2016_validation \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    #configFile="UJ_2015/ICHEP2016/WZTagging_CONFIGTYPE.config;UJ_2015/ICHEP2016/HbbTagging_CONFIGTYPE.config;UJ_2015/ICHEP2016/TopTagging_CONFIGTYPE.config;UJ_2015/ICHEP2016/MultiTagging_CONFIGTYPE.config"
    #configFile="UJ_2015/ICHEP2016/WZTagging_TAMass_CONFIGTYPE.config;UJ_2015/ICHEP2016/HbbTagging_TAMass_CONFIGTYPE.config;UJ_2015/ICHEP2016/TopTagging_TAMass_CONFIGTYPE.config;UJ_2015/ICHEP2016/MultiTagging_TAMass_CONFIGTYPE.config"
    configFile="UJ_2015/ICHEP2016/WZTagging_CombMass_CONFIGTYPE.config;UJ_2015/ICHEP2016/HbbTagging_CombMass_CONFIGTYPE.config;UJ_2015/ICHEP2016/TopTagging_CombMass_CONFIGTYPE.config;UJ_2015/ICHEP2016/MultiTagging_CombMass_CONFIGTYPE.config"
    outFile="JetUncertainties-LargeR-ICHEP2016-validation-CONFIGTYPE-SCALEVAR.pdf"
    compLabels="Baseline (WZ);Modelling (WZ);Tracking (WZ);Stat (WZ)@Baseline (Hbb);Modelling (Hbb);Tracking (Hbb);Stat (Hbb)@Baseline (Top);Modelling (Top);Tracking (Top);Stat (Top)@Baseline (MT);Modelling (MT);Tracking (MT);Stat (MT)"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;drawTotal=false;scaleVar=SCALEVAR"
}

function LargeR_ICHEP2016_validation_weak \
{
    LargeR_ICHEP2016_validation
    configFile="${configFile//CONFIGTYPE/weak}"
    outFile="${outFile//CONFIGTYPE/weak}"
    if [[ $configFile = *_TAMass_* ]] ; then
        compListBase="Rtrk_Baseline_pT,TAM_Baseline_mass,Rtrk_Baseline_D2;Rtrk_Modelling_pT,TAM_Modelling_mass,Rtrk_Modelling_D2;Rtrk_Tracking_pT,TAM_Tracking_mass,Rtrk_Tracking_D2;Rtrk_TotalStat_pt,TAM_TotalStat_mass,Rtrk_TotalStat_D2"
    elif [[ $configFile = *_CombMass_* ]] ; then
        compListBase="Rtrk_Baseline_pT,Comb_Baseline_mass,Rtrk_Baseline_D2;Rtrk_Modelling_pT,Comb_Modelling_mass,Rtrk_Modelling_D2;Rtrk_Tracking_pT,Comb_Tracking_mass,Rtrk_Tracking_D2;Rtrk_TotalStat_pt,Comb_TotalStat_mass,Rtrk_TotalStat_D2"
    else
        compListBase="Rtrk_Baseline_pT,Rtrk_Baseline_mass,Rtrk_Baseline_D2;Rtrk_Modelling_pT,Rtrk_Modelling_mass,Rtrk_Modelling_D2;Rtrk_Tracking_pT,Rtrk_Tracking_mass,Rtrk_Tracking_D2;Rtrk_TotalStat_pT,Rtrk_TotalStat_mass,Rtrk_TotalStat_D2"
    fi
    compList="$compListBase@$compListBase@${compListBase//D2/Tau32}"
    if [[ $configFile = *_TAMass_* ]] ; then
        compList="${compList}@Rtrk_Baseline_pT,TAM_Baseline_mass,Rtrk_Baseline_D2,Rtrk_Baseline_Tau32;Rtrk_Modelling_pT,TAM_Modelling_mass,Rtrk_Modelling_D2,Rtrk_Modelling_Tau32;Rtrk_Tracking_pT,TAM_Tracking_mass,Rtrk_Tracking_D2,Rtrk_Tracking_Tau32;Rtrk_TotalStat_pT,TAM_TotalStat_mass,Rtrk_TotalStat_D2,Rtrk_TotalStat_Tau32"
    elif [[ $configFile = *_CombMass_* ]] ; then
        compList="${compList}@Rtrk_Baseline_pT,Comb_Baseline_mass,Rtrk_Baseline_D2,Rtrk_Baseline_Tau32;Rtrk_Modelling_pT,Comb_Modelling_mass,Rtrk_Modelling_D2,Rtrk_Modelling_Tau32;Rtrk_Tracking_pT,Comb_Tracking_mass,Rtrk_Tracking_D2,Rtrk_Tracking_Tau32;Rtrk_TotalStat_pT,Comb_TotalStat_mass,Rtrk_TotalStat_D2,Rtrk_TotalStat_Tau32"
    else
        compList="${compList}@Rtrk_Baseline_pT,Rtrk_Baseline_mass,Rtrk_Baseline_D2,Rtrk_Baseline_Tau32;Rtrk_Modelling_pT,Rtrk_Modelling_mass,Rtrk_Modelling_D2,Rtrk_Modelling_Tau32;Rtrk_Tracking_pT,Rtrk_Tracking_mass,Rtrk_Tracking_D2,Rtrk_Tracking_Tau32;Rtrk_TotalStat_pT,Rtrk_TotalStat_mass,Rtrk_TotalStat_D2,Rtrk_TotalStat_Tau32"
    fi
}

function LargeR_ICHEP2016_validation_weak_pt \
{
    LargeR_ICHEP2016_validation_weak
    outFile="${outFile/SCALEVAR/pt}"
    options="${options/SCALEVAR/pt}"
}

function LargeR_ICHEP2016_validation_weak_mass \
{
    LargeR_ICHEP2016_validation_weak
    outFile="${outFile/SCALEVAR/mass}"
    options="${options/SCALEVAR/mass}"
}

function LargeR_ICHEP2016_validation_weak_sub \
{
    LargeR_ICHEP2016_validation_weak
    outFile="${outFile/SCALEVAR/sub}"
    options="${options/SCALEVAR/D2Beta1&Tau32WTA}"
}

function LargeR_ICHEP2016_validation_weak_D2 \
{
    LargeR_ICHEP2016_validation_weak
    outFile="${outFile/SCALEVAR/D2}"
    options="${options/SCALEVAR/D2Beta1}"
}

function LargeR_ICHEP2016_validation_weak_tau32 \
{
    LargeR_ICHEP2016_validation_weak
    outFile="${outFile/SCALEVAR/tau32}"
    options="${options/SCALEVAR/Tau32WTA}"
}

function LargeR_ICHEP2016_validation_medium \
{
    LargeR_ICHEP2016_validation
    configFile="${configFile//CONFIGTYPE/medium}"
    outFile="${outFile//CONFIGTYPE/medium}"
    if [[ $configFile = *_TAMass_* ]] ; then
        compListBase="TAM_Baseline_Kin,Rtrk_Baseline_D2;TAM_Modelling_Kin,Rtrk_Modelling_D2;TAM_Tracking_Kin,Rtrk_Tracking_D2;TAM_TotalStat_Kin,Rtrk_TotalStat_D2"
    elif [[ $configFile = *_CombMass_* ]] ; then
        compListBase="Comb_Baseline_Kin,Rtrk_Baseline_D2;Comb_Modelling_Kin,Rtrk_Modelling_D2;Comb_Tracking_Kin,Rtrk_Tracking_D2;Comb_TotalStat_Kin,Rtrk_TotalStat_D2"
    else
        compListBase="Rtrk_Baseline_Kin,Rtrk_Baseline_D2;Rtrk_Modelling_Kin,Rtrk_Modelling_D2;Rtrk_Tracking_Kin,Rtrk_Tracking_D2;Rtrk_TotalStat_Kin,Rtrk_TotalStat_D2"
    fi
    compList="$compListBase@$compListBase@${compListBase//D2/Tau32}"
    if [[ $configFile = *_TAMass_* ]] ; then
        compList="${compList}@TAM_Baseline_Kin,Rtrk_Baseline_Sub;TAM_Modelling_Kin,Rtrk_Modelling_Sub;TAM_Tracking_Kin,Rtrk_Tracking_Sub;TAM_TotalStat_Kin,Rtrk_TotalStat_Sub"
    elif [[ $configFile = *_CombMass_* ]] ; then
        compList="${compList}@Comb_Baseline_Kin,Rtrk_Baseline_Sub;Comb_Modelling_Kin,Rtrk_Modelling_Sub;Comb_Tracking_Kin,Rtrk_Tracking_Sub;Comb_TotalStat_Kin,Rtrk_TotalStat_Sub"
    else
        compList="${compList}@Rtrk_Baseline_Kin,Rtrk_Baseline_Sub;Rtrk_Modelling_Kin,Rtrk_Modelling_Sub;Rtrk_Tracking_Kin,Rtrk_Tracking_Sub;Rtrk_TotalStat_Kin,Rtrk_TotalStat_Sub"
    fi
}

function LargeR_ICHEP2016_validation_medium_pt \
{
    LargeR_ICHEP2016_validation_medium
    outFile="${outFile/SCALEVAR/pt}"
    options="${options/SCALEVAR/pt}"
}

function LargeR_ICHEP2016_validation_medium_mass \
{
    LargeR_ICHEP2016_validation_medium
    outFile="${outFile/SCALEVAR/mass}"
    options="${options/SCALEVAR/mass}"
}

function LargeR_ICHEP2016_validation_medium_sub \
{
    LargeR_ICHEP2016_validation_medium
    outFile="${outFile/SCALEVAR/sub}"
    options="${options/SCALEVAR/D2Beta1&Tau32WTA}"
}

function LargeR_ICHEP2016_validation_medium_D2 \
{
    LargeR_ICHEP2016_validation_medium
    outFile="${outFile/SCALEVAR/D2}"
    options="${options/SCALEVAR/D2Beta1}"
}

function LargeR_ICHEP2016_validation_medium_tau32 \
{
    LargeR_ICHEP2016_validation_medium
    outFile="${outFile/SCALEVAR/tau32}"
    options="${options/SCALEVAR/Tau32WTA}"
}

function LargeR_ICHEP2016_validation_strong \
{
    LargeR_ICHEP2016_validation
    configFile="${configFile//CONFIGTYPE/strong}"
    outFile="${outFile//CONFIGTYPE/strong}"
    if [[ $configFile = *_TAMass_* ]] ; then
        compListBase="TAM_Baseline_All;TAM_Modelling_All;TAM_Tracking_All;TAM_TotalStat_All"
    elif [[ $configFile = *_CombMass_* ]] ; then
        compListBase="Comb_Baseline_All;Comb_Modelling_All;Comb_Tracking_All;Comb_TotalStat_All"
    else
        compListBase="Rtrk_Baseline_All;Rtrk_Modelling_All;Rtrk_Tracking_All;Rtrk_TotalStat_All"
    fi
    compList="$compListBase@$compListBase@$compListBase@$compListBase"
}

function LargeR_ICHEP2016_validation_strong_pt \
{
    LargeR_ICHEP2016_validation_strong
    outFile="${outFile/SCALEVAR/pt}"
    options="${options/SCALEVAR/pt}"
}

function LargeR_ICHEP2016_validation_strong_mass \
{
    LargeR_ICHEP2016_validation_strong
    outFile="${outFile/SCALEVAR/mass}"
    options="${options/SCALEVAR/mass}"
}

function LargeR_ICHEP2016_validation_strong_sub \
{
    LargeR_ICHEP2016_validation_strong
    outFile="${outFile/SCALEVAR/sub}"
    options="${options/SCALEVAR/D2Beta1&Tau32WTA}"
}

function LargeR_ICHEP2016_validation_strong_D2 \
{
    LargeR_ICHEP2016_validation_strong
    outFile="${outFile/SCALEVAR/D2}"
    options="${options/SCALEVAR/D2Beta1}"
}

function LargeR_ICHEP2016_validation_strong_tau32 \
{
    LargeR_ICHEP2016_validation_strong
    outFile="${outFile/SCALEVAR/tau32}"
    options="${options/SCALEVAR/Tau32WTA}"
}


function LargeR_ICHEP2016_mCalo \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2015/ICHEP2016/MultiTagging_all.config"
    outFile="JetUncertainties-LargeR-ICHEP2016-public-mCalo.pdf"
    compList="#Baseline#;#Modelling#;#Tracking1#;#Tracking2#;#Tracking3#;#TotalStat#"
    compLabels="Baseline;Modelling;Tracking (TIDE);Tracking (fakes);Tracking (q/p_{T});Statistical"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;totalUncName=\"Total m_{calo} uncertainty\";scaleVar=mass;isPublic=true;axisMax=0.30"
}
function LargeR_ICHEP2016_mTA \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2015/ICHEP2016/MultiTagging_TAMass_all.config"
    outFile="JetUncertainties-LargeR-ICHEP2016-public-mTA.pdf"
    compList="#Baseline#;#Modelling#;#Tracking1#;#Tracking2#;#Tracking3#;#TotalStat#"
    compLabels="Baseline;Modelling;Tracking (TIDE);Tracking (fakes);Tracking (q/p_{T});Statistical"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;doCompare=UJ_2015/ICHEP2016/MultiTagging_weak.config&MC15c&\"Total m_{calo} uncertainty\";totalUncName=\"Total m_{TA} uncertainty\";scaleVar=mass;isPublic=true;axisMax=0.30"
}
function LargeR_ICHEP2016_pT \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2015/ICHEP2016/MultiTagging_all.config"
    outFile="JetUncertainties-LargeR-ICHEP2016-public-pT.png"
    compList="#Baseline#;#Modelling#;#Tracking1#,#Tracking2#,#Tracking3#;#TotalStat#"
    compLabels="R_{trk} baseline (Pythia 8);R_{trk} modelling (Herwig++);R_{trk} tracking;Statistical"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;totalUncName=\"Total uncertainty\";scaleVar=pT;isPublic=true;axisMax=0.30"
}
function LargeR_ICHEP2016_D2 \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2015/ICHEP2016/MultiTagging_all.config"
    outFile="JetUncertainties-LargeR-ICHEP2016-public-D2.png"
    compList="#Baseline#;#Modelling#;#Tracking1#,#Tracking2#,#Tracking3#;#TotalStat#"
    compLabels="R_{trk} baseline (Pythia 8);R_{trk} modelling (Herwig++);R_{trk} tracking;Statistical"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;totalUncName=\"Total uncertainty\";scaleVar=D2Beta1;isPublic=true;axisMax=0.30"
}
function LargeR_ICHEP2016_Tau32 \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2015/ICHEP2016/MultiTagging_all.config"
    outFile="JetUncertainties-LargeR-ICHEP2016-public-Tau32.png"
    compList="#Baseline#;#Modelling#;#Tracking1#,#Tracking2#,#Tracking3#;#TotalStat#"
    compLabels="R_{trk} baseline (Pythia 8);R_{trk} modelling (Herwig++);R_{trk} tracking;Statistical"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;totalUncName=\"Total uncertainty\";scaleVar=Tau32WTA;isPublic=true;axisMax=0.30"
}




SRNum=1
function ICHEP2016_validation_SR \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/ICHEP2016/JES2015_SR_ScenarioAll.config;JES_2015/ICHEP2016/JES2015_SR_Scenario${SRNum}.config"
    outFile="JetUncertainties-ICHEP2016-SRval-${SRNum}.pdf"
    compList="GroupedNP_SR${SRNum}_1;GroupedNP_SR${SRNum}_2;GroupedNP_SR${SRNum}_3@GroupedNP_1;GroupedNP_2;GroupedNP_3"
    compLabels="SRAll Grouped NP 1;SRAll Grouped NP 2;SRAll Grouped NP 3@SR${SRNum} Grouped NP 1;SR${SRNum} Grouped NP 2;SR${SRNum} Grouped NP 3"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=false"
}
function ICHEP2016_SRall \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/ICHEP2016/JES2015_SR_ScenarioAll.config"
    outFile="JetUncertainties-ICHEP2016-SRAll.pdf"
    compList="EtaIntercalibration_NonClosure;RelativeNonClosure_MC15;GroupedNP_SR1_1;GroupedNP_SR1_2;GroupedNP_SR1_3;GroupedNP_SR2_1;GroupedNP_SR2_2;GroupedNP_SR2_3;GroupedNP_SR3_1;GroupedNP_SR3_2;GroupedNP_SR3_3;GroupedNP_SR4_1;GroupedNP_SR4_2;GroupedNP_SR4_3"
    compLabels="EtaIntercalibration_NonClosure;RelativeNonClosure_MC15;GroupedNP_SR1_1;GroupedNP_SR1_2;GroupedNP_SR1_3;GroupedNP_SR2_1;GroupedNP_SR2_2;GroupedNP_SR2_3;GroupedNP_SR3_1;GroupedNP_SR3_2;GroupedNP_SR3_3;GroupedNP_SR4_1;GroupedNP_SR4_2;GroupedNP_SR4_3"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=false"
}
function ICHEP2016_SR1_NP1 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/ICHEP2016/JES2015_SR_ScenarioAll.config;JES_2015/ICHEP2016/JES2015_19NP.config"
    outFile="JetUncertainties-ICHEP2016-SR1-NP1.pdf"
    compList="GroupedNP_SR1_1@EffectiveNP_1;Pileup_OffsetMu;Pileup_OffsetNPV;Pileup_PtTerm;Pileup_RhoTopology;Flavor_Composition;Flavor_Response"
    compLabels="GroupedNP_SR1_1@EffectiveNP_1;Pileup_OffsetMu;Pileup_OffsetNPV;Pileup_PtTerm;Pileup_RhoTopology;Flavor_Composition;Flavor_Response"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=false"
}
function ICHEP2016_SR1_NP2 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/ICHEP2016/JES2015_SR_ScenarioAll.config;JES_2015/ICHEP2016/JES2015_19NP.config"
    outFile="JetUncertainties-ICHEP2016-SR1-NP2.pdf"
    compList="GroupedNP_SR1_2@EffectiveNP_2;EffectiveNP_3;EffectiveNP_4;EffectiveNP_5;EffectiveNP_6restTerm;EtaIntercalibration_TotalStat"
    compLabels="GroupedNP_SR1_2@EffectiveNP_2;EffectiveNP_3;EffectiveNP_4;EffectiveNP_5;EffectiveNP_6restTerm;EtaIntercalibration_TotalStat"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=false"
}
function ICHEP2016_SR1_NP3 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/ICHEP2016/JES2015_SR_ScenarioAll.config;JES_2015/ICHEP2016/JES2015_19NP.config"
    outFile="JetUncertainties-ICHEP2016-SR1-NP3.pdf"
    compList="GroupedNP_SR1_3@EtaIntercalibration_Modelling;SingleParticle_HighPt"
    compLabels="GroupedNP_SR1_3@EtaIntercalibration_Modelling;SingleParticle_HighPt"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=false"
}





function LargeR_Moriond2016_Run1 \
{
    # July 2014 large-R jet release
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15"
    configFile="UJ_2015/Moriond2016/UJ2015_WZTagging.config"
    outFile="JetUncertainties-Moriond2016-Run1-pT.pdf"
    compList="Rtrk_Baseline;Rtrk_Modelling;Rtrk_Tracking1;Rtrk_Tracking2;Rtrk_Tracking3"
    compLabels="R_{trk} baseline;R_{trk} modelling;R_{trk} tracking 1;R_{trk} tracking 2;R_{trk} tracking 3"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;totalUncName=\"Total Uncertainty, Moriond2016\";scaleVar=pT;doCompare=UJ_2015/Prerec/Prerec2015_OnlyRun1.config&MC15&\"Total R_{trk} uncertainty, 2012\""
}


function ICHEP2016_AllEtaInt \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/ICHEP2016/JES2015_AllNuisanceParameters.config;JES_2015/ICHEP2016/JES2015_AllNuisanceParameters_AllEtaIntStat.config"
    outFile="JetUncertainties-ICHEP2016-AllEtaIntStat.pdf"
    compList="EtaIntercalibration_TotalStat@EtaIntercalibration_Stat#;EtaIntercalibration_TotalStat#"
    compLabels="TotalStat term (all combined)@246 split stat NPs;TotalStat propagated through MJB"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=false;axisMax=0.02"
}


function YearCompare_GlobalReduction \
{
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt4EMTopo;AntiKt4LCTopo;AntiKt4EMTopo"
    MCtype="MC11c;MC11c;MC12;MC12;MC15"
    configFile="JES_2011/Final/InsituJES2011_12NP.config;JES_2011/Final/InsituJES2011_12NP.config;JES_2012/Final/InsituJES2012_14NP.config;JES_2012/Final/InsituJES2012_14NP.config;JES_2015/ICHEP2016/JES2015_19NP.config"
    outFile="JetUncertainties-YearCompare-${COMPNAME}.pdf"
    compList="$COMPNAME@$COMPNAME@$COMPNAME@$COMPNAME@$COMPNAME"
    compLabels="$COMPNAME (EM4, 2011)@$COMPNAME (LC4, 2011)@$COMPNAME (EM4, 2012)@$COMPNAME (LC4, 2012)@$COMPNAME (EM4, 2016)"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=false;fixedPtVals=100;fixedEtaVals=0;axisMax=0.04"
}

function ReclusteredUsingLargeR_validation \
{
    var="pT"
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    #configFile="UJ_2015/ICHEP2016/ReclusteredJets_strong.config"
    configFile="UJ_2015/ICHEP2016/ReclusteredJets_onlymass.config"
    outFile="JetUncertainties-LargeR-RC-ICHEP2016-${var}.pdf"
    compList="#Baseline#;#Modelling#;#Tracking#;#TotalStat#"
    compLabels="Baseline;Modelling;Tracking;Statistical"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;scaleVar=${var};isPublic=false;axisMax=0.30"
}

function Moriond2017_LargeR \
{
    var="pT"
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2016/Moriond2017/UJ2016_CombinedMass_weak_all.config"
    outFile="JetUncertainties-LargeR-Moriond2017-${var}.pdf"
    compList="#Baseline#;#Modelling#;#Tracking#;#TotalStat#"
    compLabels="Baseline;Modelling;Tracking;Statistical"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;scaleVar=${var};isPublic=false;axisMax=0.30;VariablesToShift=Tau32WTA,D2Beta1,Split12"
}

function LargeR_Moriond2017_compareMass \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2016/Moriond2017/UJ2016_CombinedMass_weak.config;UJ_2016/Moriond2017/UJ2016_CombinedMass_weak.config;UJ_2016/Moriond2017/UJ2016_CaloMass_weak.config;UJ_2016/Moriond2017/UJ2016_TrackAssistedMass_weak.config;UJ_2016/Moriond2017/UJ2016_CombinedMass_weak.config"
    outFile="JetUncertainties-LargeR-Moriond2017-masscomp.pdf"
    compList="CaloWeight@TAWeight@TOTAL@TOTAL@TOTAL"
    compLabels="(Calorimeter weight)/10@(Track-assisted weight)/10@Calorimeter mass@Track-assisted mass@Combined mass"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;drawTotal=false;scaleVar=mass"
    #options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;drawTotal=false;scaleVar=mass;xAxisRange=20&1000"
}

function SmallRMass_Extrap \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017/JES2016_AllNuisanceParameters_JMSExtrap.config"
    outFile="JetUncertainties-SmallRMass-Extrap.pdf"
    compList="Rtrk_Baseline#;Rtrk_Modelling#;Rtrk_Tracking#;Rtrk_TotalStat#"
    compLabels="Basline;Modelling;Tracking;Statistical"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;scaleVar=mass;totalUncName=\"Total uncertainty, extrapolated\";ptBins=L&2000&20&3000;xAxisRange=20&3000"
}

function SmallRMass_Frozen \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017/JES2016_AllNuisanceParameters_JMSFrozen.config"
    outFile="JetUncertainties-SmallRMass-Frozen.pdf"
    compList="Rtrk_Baseline#;Rtrk_Modelling#;Rtrk_Tracking#;Rtrk_TotalStat#"
    compLabels="Basline;Modelling;Tracking;Statistical"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;scaleVar=mass;totalUncName=\"Total uncertainty, frozen\";ptBins=L&2000&20&3000;xAxisRange=20&3000"
}

function ValidateSmallRMassConfig \
{
    scale="pT"
    #scale="FourVec"
    #scale="mass"
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017/JES2016_AllNuisanceParameters.config;JES_2016/Moriond2017/JES2016_AllNuisanceParameters_JMSExtrap.config"
    outFile="JetUncertainties-SmallRMass-Validation-${scale}.pdf"
    compList="TOTAL@TOTAL"
    compLabels="Config without JMS@Config with JMS"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;drawTotal=false;scaleVar=${scale};axisMax=0.05;ptBins=L&2000&20&3000;xAxisRange=20&3000"
}

function ValidateSmallRMassConfigs \
{
    #scale="pT"
    scale="mass"
    JMSTYPE="JMSExtrap"
    #JMSTYPE="JMSFrozen"

    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017/JES2016_AllNuisanceParameters_${JMSTYPE}.config;JES_2016/Moriond2017/JES2016_29NP_ByCategory_${JMSTYPE}.config;JES_2016/Moriond2017/JES2016_21NP_${JMSTYPE}.config;JES_2016/Moriond2017/JES2016_SR_Scenario1_${JMSTYPE}.config;JES_2016/Moriond2017/JES2016_SR_Scenario2_${JMSTYPE}.config;JES_2016/Moriond2017/JES2016_SR_Scenario3_${JMSTYPE}.config;JES_2016/Moriond2017/JES2016_SR_Scenario4_${JMSTYPE}.config"
    outFile="JetUncertainties-SmallRMass-ValidationSet-${scale}-${JMSTYPE}.pdf"
    compList="TOTAL@TOTAL@TOTAL@TOTAL@TOTAL@TOTAL@TOTAL"
    compLabels="All NP@Category@Global@SR1@SR2@SR3@SR4"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;drawTotal=false;scaleVar=${scale};axisMax=0.1;ptBins=L&2000&20&3000;xAxisRange=20&3000"
}

function LargeRstatBug \
{
    scale="pT"
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2016/Moriond2017/UJ2016_CombinedMass_weak_all.config;UJ_2016/Moriond2017/ShowBadStatUnc.config"
    outFile="JetUncertainties-LargeR-StatBug-${scale}.pdf"
    compList="#TotalStat#;TOTAL@#TotalStat#;TOTAL"
    compLabels="Statistical term (fixed);Total uncertainty (fixed)@Statistical term (buggy);Total uncertainty (buggy)"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;drawTotal=false;scaleVar=${scale}"
}

function EtaIntModComp \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC11b;MC12"
    configFile="JES_2011/Final/InsituJES2011_AllNuisanceParameters.config;JES_2012/Final/InsituJES2012_AllNuisanceParameters.config"
    outFile="JetUncertainties-EtaIntModComp.pdf"
    compList="EtaIntercalibration_Mod#;TOTAL@EtaIntercalibration_Mod#;TOTAL"
    compLabels="2011 #eta-intercal modelling;2011 total@2012 #eta-intercal modelling;2012 total"
    options="isDijet=false;isLargeR=false;prefix=JET_;drawTotal=false"
}

function LargeR_Moriond2017_ICHEP2016 \
{
    #scale="pT"
    scale="mass"
    #scale="D2Beta1"
    #scale="Tau32WTA"
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2016/Moriond2017/UJ2016_CombinedMass_weak_all.config"
    outFile="JetUncertainties-Moriond2017-ICHEP2016-${scale}.pdf"
    compList="#Baseline#;#Modelling#;#Tracking#;#TotalStat#"
    compLabels="Baseline;Modelling;Tracking;Total stat."
    #options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;scaleVar=${scale};totalUncName=\"Total uncertainty\""
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;scaleVar=${scale};totalUncName=\"Total uncertainty\";doCompare=UJ_2015/ICHEP2016/MultiTagging_all.config&MC15c&\"ICHEP2016\""
}

function Moriond2017_OnlyFlavourSplit \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017/JES2016_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2017-Flavour-dijet.pdf"
    compList="Flavor_Response_orig;Flavor_Response_prop;Flavor_Composition_orig;Flavor_Composition_prop"
    compLabels="Flavour Response;MJB-propagated Flavour Response;Flavour Composition;MJB-propagated Flavour Composition"
    options="isDijet=true;prefix=JET_"
}



function Moriond2018_LargeR \
{
    var="pT"
    massDef="CombMass"
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC16a"
    configFile="rel21/Moriond2018/R10_${massDef}_all.config"
    outFile="JetUncertainties-LargeR-Moriond2018-${var}.pdf"
    compList="#Baseline#;#Modelling#;#Tracking#;#TotalStat#"
    compLabels="Baseline;Modelling;Tracking;Statistical"
    totalUnc="Total uncertainty"
    if [[ $var = "mass" ]] ; then
        if [[ $massDef = "CombMass" ]] ; then
            totalUnc="$totalUnc, m_{comb}"
        elif [[ $massDef = "TAMass" ]] ; then
            totalUnc="$totalUnc, m_{TA}"
        elif [[ $massDef = "CaloMass" ]] ; then
            totalUnc="$totalUnc, m_{calo}"
        else
            totalUnc="$totalUnc, mass"
        fi
    else
        totalUnc="$totalUnc, $var"
    fi
        
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;scaleVar=${var};isPublic=false;axisMax=0.30;totalUncName=\"$totalUnc\""
}

function Moriond2018_LargeR_Validation \
{
    var="pT"
    massDef="CombMass"
    scenario="strong"
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC16a"
    configFile="rel21/Moriond2018/R10_${massDef}_all.config;rel21/Moriond2018/R10_${massDef}_${scenario}.config"
    outFile="JetUncertainties-LargeR-Moriond2018-${scenario}-${massDef}-${var}.pdf"
    compList="#Baseline#;#Modelling#;#Tracking#;#TotalStat#@#Baseline#;#Modelling#;#Tracking#;#TotalStat#"
    compLabels="Baseline (all);Modelling (all);Tracking (all);Statistical (all)@Baseline ($scenario);Modelling ($scenario);Tracking ($scenario);Statistical ($scenario)"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;scaleVar=${var};isPublic=false;axisMax=0.30"
}

function Moriond2018 \
{

    jetDefinition="AntiKt4EMTopo;AntiKt4EMPFlow"
    MCtype="MC16"
    configFile="rel21/Moriond2018/R4_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2018-Nominal.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_"
}

function Moriond2018_EMvsPF \
{

    jetDefinition="AntiKt4EMPFlow"
    MCtype="MC16"
    configFile="rel21/Moriond2018/R4_AllNuisanceParameters.config"
    outFile="JetUncertainties-Moriond2018-Nominal-Compare.pdf"
    compList="LAr#,Zjet#,Gjet#,MJB#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;doCompare=rel21/Moriond2018/R4_AllNuisanceParameters.config&MC16&Total uncertainty, EMTopo&AntiKt4EMTopo;totalUncName=\"Total uncertainty, EMPFlow\""
}

#######################################
## Fall 2018 functions

function PreFall2018_forMJB \
{

    jetDefinition="AntiKt4EMTopo;AntiKt4EMPFlow"
    MCtype="MC16"
    configFile="JetUncertainties/rel21/Fall2018/R4_JESNuisanceParametersForMJB.config"
    outFile="Fall2018/JetUncertainties-IntoMJB_Fall2018.pdf"
    compList="Zjet#,Gjet#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#;"
    compLabels="Absolute #it{in situ} JES (Z,#gamma+jet);Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;bunchSpacing=25;fixedPtVals=25,40,60,80,100,120;path=/afs/cern.ch/work/k/kpachal/JetUncertainties/JetUncertainties/athena/Reconstruction/Jet/JetUncertainties/share/"

}

function PreFall2018_forMJB_comparePFlowEMTopo \
{

    # Load default Fall 2018 release
    PreFall2018_forMJB
    # Override variables
    jetDefinition="AntiKt4EMTopo"
    outFile="Fall2018/JetUncertainties-Fall2018-intoMJB-ComparePFlowEMTopo.pdf"
    options="${options};doCompare=JetUncertainties/rel21/Fall2018/R4_JESNuisanceParametersForMJB.config&MC16&PFlow uncertainty&AntiKt4EMPFlow"

}

function Fall2018 \
{

    jetDefinition="AntiKt4EMTopo;AntiKt4EMPFlow"
    MCtype="MC16"
    configFile="rel21/Fall2018/R4_AllNuisanceParameters_AllJERNP.config"
    outFile="Fall2018/JetUncertainties-Fall2018-AllComponents-unknownComp.pdf"
    compList="LAr#,Zjet#,Gjet#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;path=/afs/cern.ch/work/k/kpachal/JetUncertainties/JetUncertainties/athena/Reconstruction/Jet/JetUncertainties/share/"
}

function Fall2018_Dijet \
{

    Fall2018
    outFile="Fall2018/JetUncertainties-Fall2018-AllComponents-dijetComp.pdf"    
    options="isDijet=true;isGSC=true;isLargeR=false;prefix=JET_;path=/afs/cern.ch/work/k/kpachal/JetUncertainties/JetUncertainties/athena/Reconstruction/Jet/JetUncertainties/share/"
}

function Fall2018_GlobalReduction \
{
    # Load default Moriond 2018 release
    Fall2018
    # Override variables
    configFile="rel21/Fall2018/R4_GlobalReduction_FullJER.config"
    outFile="Fall2018/JetUncertainties-Fall2018-Unknown-GlobalReduction.pdf"
    compList="EffectiveNP#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"

}

function Fall2018_CategoryReduction \
{
    # Load default Moriond 2018 release
    Fall2018
    # Override variables
    configFile="rel21/Fall2018/R4_CategoryReduction.config"
    outFile="Fall2018/JetUncertainties-Fall2018-Unknown-CategoryReduction.pdf"
    compList="EffectiveNP#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"

}


#######################################
## Fall 2018 special

function Fall2018_fullStat_20152016 \
{

    jetDefinition="AntiKt4EMTopo;AntiKt4EMPFlow"
    MCtype="MC16"
    configFile="rel21/Fall2018_fullStatUncertanties/R4_AllNuisanceParameters_2015_16_AllJERNP.config"
    outFile="Fall2018_fullStatUncertanties/JetUncertainties-Fall2018-AllStatTerms-2016-2016-unknownComp.pdf"
    compList="Zjet#,Gjet#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;path=/afs/cern.ch/work/k/kpachal/JetUncertainties/JetUncertainties/athena/Reconstruction/Jet/JetUncertainties/share/"
}

function Fall2018_fullStat_2017 \
{

    jetDefinition="AntiKt4EMTopo;AntiKt4EMPFlow"
    MCtype="MC16"
    configFile="rel21/Fall2018_fullStatUncertanties/R4_AllNuisanceParameters_2017_AllJERNP.config"
    outFile="Fall2018_fullStatUncertanties/JetUncertainties-Fall2018-AllStatTerms-2017-unknownComp.pdf"
    compList="Zjet#,Gjet#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;path=/afs/cern.ch/work/k/kpachal/JetUncertainties/JetUncertainties/athena/Reconstruction/Jet/JetUncertainties/share/"
}

function Fall2018_fullStat_20152016_testNoJER \
{

    jetDefinition="AntiKt4EMTopo;AntiKt4EMPFlow"
    MCtype="MC16"
    configFile="rel21/Fall2018_fullStatUncertanties/R4_AllNuisanceParameters_2015_16_NoJER.config"
    outFile="Fall2018_fullStatUncertanties/JetUncertainties-Fall2018-AllStatTerms-2016-2016-unknownComp-noJER.pdf"
    compList="Zjet#,Gjet#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;path=/afs/cern.ch/work/k/kpachal/JetUncertainties/JetUncertainties/athena/Reconstruction/Jet/JetUncertainties/share/"
}

function Fall2018_fullStat_2017_testNoJER \
{

    jetDefinition="AntiKt4EMTopo;AntiKt4EMPFlow"
    MCtype="MC16"
    configFile="rel21/Fall2018_fullStatUncertanties/R4_AllNuisanceParameters_2017_NoJER.config"
    outFile="Fall2018_fullStatUncertanties/JetUncertainties-Fall2018-AllStatTerms-2017-unknownComp-noJER.pdf"
    compList="Zjet#,Gjet#,SingleParticle#;EtaIntercalib#;Flavor_Comp#;Flavor_Resp#;Pileup#;PunchThrough_#"
    compLabels="Absolute #it{in situ} JES;Relative #it{in situ} JES;Flav. composition;Flav. response;Pileup;Punch-through;"
    options="isDijet=false;isGSC=true;isLargeR=false;prefix=JET_;path=/afs/cern.ch/work/k/kpachal/JetUncertainties/JetUncertainties/athena/Reconstruction/Jet/JetUncertainties/share/"
}

#######################################
# Older

function testRel21AFII \
{
    jetDefinition="AntiKt4EMTopo;AntiKt4EMPFlow"
    MCtype="AFII"
    configFile="rel21/Moriond2018/R4_AllNuisanceParameters.config"
    outFile="JetUncertainties-rel21-R4-${MCtype}-RhoTop.pdf"
    compList="RelativeNonClosure#;Pileup_RhoTopology"
    compLabels="RelativeNonClosure_${MCtype};Pileup_RhoTopology"
    options="isDijet=false;prefix=JET_"
}

function Rel20p7_EffNP1 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017/JES2016_AllNuisanceParameters.config;JES_2016/Moriond2017/JES2016_21NP.config"
    outFile="JetUncertainties-rel20p7-EffNP1.pdf"
    compList="Zjet_MC,Zjet_Veto,LAr_Jvt@EffectiveNP_1"
    compLabels="Zjet (MC+Veto) + LAr JVT@EffectiveNP_1"
    options="isDijet=false;prefix=JET_;fixedEtaVals=0;fixedPtVals=40;drawTotal=false"
}

function Rel20p7_EffNP2 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2016/Moriond2017/JES2016_AllNuisanceParameters.config;JES_2016/Moriond2017/JES2016_21NP.config"
    outFile="JetUncertainties-rel20p7-EffNP2.pdf"
    compList="LAr_ESZee,Gjet_Generator,INV__Gjet_Purity@EffectiveNP_2"
    compLabels="#gamma+jet (Generator-Purity) + LAr ES(Zee)@EffectiveNP_2"
    options="isDijet=false;prefix=JET_;fixedEtaVals=0;fixedPtVals=40;drawTotal=false"
}

function Rel21_EffNP1 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC16"
    CalibArea="CalibArea-02"
    configFile="rel21/Moriond2018/R4_AllNuisanceParameters.config;rel21/Moriond2018/R4_GlobalReduction.config"
    outFile="JetUncertainties-rel21-EffNP1.pdf"
    compList="Zjet_MC,Zjet_Veto,Zjet_JVT@EffectiveNP_1"
    compLabels="Zjet MC+JVT+Veto@EffectiveNP_1"
    options="isDijet=false;prefix=JET_;fixedEtaVals=0;fixedPtVals=40;drawTotal=false"
}

function Rel21_EffNP2 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC16"
    CalibArea="CalibArea-02"
    configFile="rel21/Moriond2018/R4_AllNuisanceParameters.config;rel21/Moriond2018/R4_GlobalReduction.config"
    outFile="JetUncertainties-rel21-EffNP2.pdf"
    compList="Gjet_GamESZee,Gjet_Generator,INV__Gjet_Purity@EffectiveNP_2"
    compLabels="#gamma+jet ES(Zee)+Generator-Purity@EffectiveNP_2"
    options="isDijet=false;prefix=JET_;fixedEtaVals=0;fixedPtVals=40;drawTotal=false"
}

function testDefaultNjets \
{
    njet=0

    jetDefinition="AntiKt4EMTopo"
    MCtype="MC16"
    CalibArea="CalibArea-03"
    configFile="rel21/Moriond2018/R4_AllNuisanceParameters.config"
    outFile="JetUncertainties-Njets-${njet}.pdf"
    compList="Flavor_Response;Flavor_Composition"
    compLabels="Flavor_Response;Flavor_Composition"
    options="prefix=JET_;fixedEtaVals=0;fixedPtVals=40;NjetFlavour=${njet};Composition=/eos/atlas/atlascerngroupdisk/perf-jets/JetUncertainties/Inputs/ANALYSISTO-370.root"
}

function testPerJetFlavour \
{
    label=21

    jetDefinition="AntiKt4EMTopo"
    MCtype="MC16"
    CalibArea="CalibArea-03"
    configFile="rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour.config"
    outFile="JetUncertainties-TruthLabel-${label}.pdf"
    compList="FlavorResponse_PerJet#;Flavor_Composition"
    compLabels="FlavorResponse_PerJet ${label};Flavor_Composition"
    options="prefix=JET_;fixedEtaVals=0;fixedPtVals=40;TruthLabel=${label};path=/eos/atlas/atlascerngroupdisk/perf-jets/JetUncertainties/"
}

function CompareFlavourBinning \
{
    label=21

    jetDefinition="AntiKt4EMTopo"
    MCtype="MC16"
    CalibArea="CalibArea-04"
    #configFile="rel21/Moriond2018/R4_AllNuisanceParameters.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_OneBin.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_CoarseBinning.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_FineBinning.config"
    configFile="rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_OneBin.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_CoarseBinning.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_FineBinning.config"
    outFile="JetUncertainties-TruthLabel-CompareBinning-${label}.pdf"
    compList="FlavorResponse_PerJet#;Flavor_Composition@FlavorResponse_PerJet#;Flavor_Composition@FlavorResponse_PerJet#;Flavor_Composition"
    compLabels="Response ${label} OneBin;Composition OneBin@Response ${label} CoarseBin;Composition CoarseBin@Response ${label} FineBin;Composition FineBin"
    options="prefix=JET_;fixedEtaVals=0,1,2.5;fixedPtVals=20,40,60,80,100;TruthLabel=${label};path=/eos/atlas/atlascerngroupdisk/perf-jets/JetUncertainties/;drawTotal=False"
}

function CompareFlavourBinningComp \
{
    label=1

    jetDefinition="AntiKt4EMTopo"
    MCtype="MC16"
    CalibArea="CalibArea-04"
    configFile="rel21/Moriond2018/R4_AllNuisanceParameters.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_OneBin.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_CoarseBinning.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_FineBinning.config"
    #configFile="rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_OneBin.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_CoarseBinning.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_FineBinning.config"
    outFile="JetUncertainties-TruthLabel-CompareBinning-Composition-${label}.pdf"
    compList="Flavor_Composition@Flavor_Composition@Flavor_Composition@Flavor_Composition"
    compLabels="Current (50% gluon fraction uncertainty)@Composition OneBin@Composition CoarseBin@Composition FineBin"
    options="prefix=JET_;fixedEtaVals=0,1,2.5,4.5;fixedPtVals=20,40,60,80,100;TruthLabel=${label};path=/eos/atlas/atlascerngroupdisk/perf-jets/JetUncertainties/;drawTotal=False"
}

function CompareFlavourBinningResp \
{
    label=11

    jetDefinition="AntiKt4EMTopo"
    MCtype="MC16"
    CalibArea="CalibArea-04"
    configFile="rel21/Moriond2018/R4_AllNuisanceParameters.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_OneBin.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_CoarseBinning.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_FineBinning.config"
    #configFile="rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_OneBin.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_CoarseBinning.config;rel21/Moriond2018/R4_AllNuisanceParameters_PerJetFlavour_FineBinning.config"
    outFile="JetUncertainties-TruthLabel-CompareBinning-Response-${label}.pdf"
    compList="Flavor_Response@FlavorResponse_PerJet#@FlavorResponse_PerJet#@FlavorResponse_PerJet#"
    compLabels="Current (50% gluon = PDG 21)@Response to PDG ${label} OneBin@Response to PDG ${label} CoarseBin@Response to PDG ${label} FineBin"
    options="prefix=JET_;fixedEtaVals=0,1,2.5,4.5;fixedPtVals=20,40,60,80,100,200;TruthLabel=${label};path=/eos/atlas/atlascerngroupdisk/perf-jets/JetUncertainties/;drawTotal=False"
}

function DumpHistos \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    CalibArea=""
    configFile="JES_2016/Moriond2017/JES2016_21NP.config"
    outFile="test.pdf"
    compList="EffectiveNP_1;EffectiveNP_2;EffectiveNP_3;EffectiveNP_4;EffectiveNP_5;EffectiveNP_6;EffectiveNP_7;EffectiveNP_8restTerm;Flavor_Composition;Flavor_Response;Pileup_RhoTopology"
    compLabels="EffectiveNP_1;EffectiveNP_2;EffectiveNP_3;EffectiveNP_4;EffectiveNP_5;EffectiveNP_6;EffectiveNP_7;EffectiveNP_8restTerm;Flavor_Composition_UNKNOWN;Flavor_Response_UNKNOWN;Pileup_RhoTopology"
    options="prefix=JET_;isDijet=false;fixedEtaVals=0;fixedPtVals=NONE;drawTotal=False;path=\"/eos/atlas/atlascerngroupdisk/perf-jets/JetUncertainties/\";dumpFile=test.root"
    #compList="Flavor_Composition;Flavor_Response"
    #compLabels="Flavor_Composition_DIJET;Flavor_Response_DIJET"
    #options="prefix=JET_;isDijet=true;fixedEtaVals=0;fixedPtVals=NONE;drawTotal=False;path=\"/eos/atlas/atlascerngroupdisk/perf-jets/JetUncertainties/\";dumpFile=test.root"
    #compList="BJES_Response"
    #compLabels="BJES_Response"
    #options="prefix=JET_;isDijet=false;fixedEtaVals=0;fixedPtVals=NONE;drawTotal=False;path=\"/eos/atlas/atlascerngroupdisk/perf-jets/JetUncertainties/\";dumpFile=test.root"
}

function HLInputs \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    CalibArea=""
    configFile="HL/HL-Run2.config"
    outFile="test.pdf"
    compList="EffectiveNP_#;Flavor_Comp#;FlavorResp#;Pileup#"
    compLabels="Absolute #it{in situ} JES;Flav. composition;Flav. response;Pileup"
    options="prefix=JET_;fixedEtaVals=0;fixedPtVals=NONE;path=\"/afs/cern.ch/work/s/sschramm/private/rel21/athena/Reconstruction/Jet/JetUncertainties/share/\""
}

function HLCompare \
{
    composition="DIJET"

    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    CalibArea=""
    configFile="HL2018/HL-Optimistic-${composition}.config"
    #outFile="HL-compare-Run2-Optimistic-UNKNOWN.pdf"
    outFile="HL-compare-Baseline-Optimistic-${composition}.pdf"
    compList="EffectiveNP_#;Flavor_Comp#;FlavorResp#;Pileup#"
    #compLabels="Absolute #it{in situ} JES;Flav. composition, unknown composition;Flav. response, unknown composition;Pileup"
    compLabels="Absolute #it{in situ} JES;Flav. composition, inclusive jets;Flav. response, inclusive jets;Pileup"
    #options="prefix=JET_;isDijet=false;fixedEtaVals=0;fixedPtVals=NONE;path=\"/afs/cern.ch/work/s/sschramm/private/rel21/athena/Reconstruction/Jet/JetUncertainties/share/\";doCompare=JES_2016/Moriond2017/JES2016_21NP.config&MC15&Run 2 uncertainty;totalUncName=\"HL-LHC uncertainty, optimistic\""
    #options="prefix=JET_;isDijet=true;fixedEtaVals=0;fixedPtVals=NONE;path=\"/afs/cern.ch/work/s/sschramm/private/rel21/athena/Reconstruction/Jet/JetUncertainties/share/\";doCompare=HL/HL-Baseline.config&MC15&HL-LHC uncertainty, baseline;totalUncName=\"HL-LHC uncertainty, optimistic\""
    options="prefix=JET_;isDijet=true;fixedEtaVals=0;fixedPtVals=NONE;path=\"/eos/atlas/atlascerngroupdisk/perf-jets/JetUncertainties/\";doCompare=HL2018/HL-Baseline-${composition}.config&MC15&HL-LHC uncertainty, baseline;totalUncName=\"HL-LHC uncertainty, optimistic\""
}

function JERTest \
{
    scaleVar="MassRes"
    topology="Top"
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC16"
    CalibArea=""
    configFile="JER/R4_GlobalReduction_JER.config"
    outFile="JER-test.pdf"
    compList="PtResUnc;FourVecResUnc;MassResUncQCD;MassResUncWZ;MassResUncHbb;MassResUncTop;NOMINALRESMC;NOMINALRESDATA"
    compLabels="pT resolution;four-vector resolution;mass resolution, QCD;mass resolution, WZ;mass resolution, Hbb;mass resolution, top;nominal MC resolution;nominal data resolution"
    options="prefix=JET_;fixedEtaVals=0,2;fixedPtVals=NONE;path=\"/afs/cern.ch/work/s/sschramm/private/rel21/athena/Reconstruction/Jet/JetUncertainties/share/\";scaleVar=${scaleVar};topology=${topology}"
}

function JER_August_2018_Compare \
{
    scaleVar="FourVecResAbs"
    jetdef="EMTopo"
    jetDefinition="AntiKt4${jetdef};AntiKt4${jetdef};AntiKt4${jetdef}"
    MCtype="MC16"
    CalibArea=""
    configFile="rel21/Summer2018/R4_GlobalReduction_SimpleJER.config;rel21/Summer2018/R4_GlobalReduction_FullJER.config;rel21/Summer2018/R4_AllNuisanceParameters_AllJERNP.config"
    outFile="JER-August2018-Compare-${jetdef}.pdf"
    compList="JER#@JER#@JER#"
    compLabels="Total JER uncertainty, ${jetdef}, 7NP@Total JER uncertainty, ${jetdef}, 12NP@Total JER uncertainty, ${jetdef}, All NP"
    options="prefix=JET_;fixedEtaVals=0;fixedPtVals=25;path=\"/eos/atlas/atlascerngroupdisk/perf-jets/JetUncertainties/CalibArea-05/\";drawTotal=false;scaleVar=${scaleVar};IsData=false"
}

function JER_August_2018_Nominal \
{
    scaleVar="FourVecResAbs"
    jetDefinition="AntiKt4EMTopo;AntiKt4EMPFlow"
    MCtype="MC16"
    CalibArea=""
    configFile="rel21/Summer2018/R4_GlobalReduction_SimpleJER.config;rel21/Summer2018/R4_GlobalReduction_SimpleJER.config"
    outFile="JER-August2018-Nominal.pdf"
    compList="NOMINALRESMC;NOMINALRESDATA@NOMINALRESMC;NOMINALRESDATA"
    compLabels="Nominal MC JER, EMTopo;Nominal data JER, EMTopo@Nominal MC JER, EMPFlow;Nominal data JER,EMPFlow"
    options="prefix=JET_;fixedEtaVals=0;fixedPtVals=25;path=\"/eos/atlas/atlascerngroupdisk/perf-jets/JetUncertainties/CalibArea-05/\";drawTotal=false;scaleVar=${scaleVar};IsData=false;axisMax=0.4"
}

function JER_August2018 \
{
    scaleVar="FourVecResAbs"
    jetDefinition="AntiKt4EMTopo;AntiKt4EMPFlow"
    MCtype="MC16"
    CalibArea=""
    configFile="rel21/Summer2018/R4_AllNuisanceParameters_AllJERNP.config"
    outFile="JER-Summer2018-Recommendations.pdf"
    compList="JER_DataVsMC;JER_N_#;JER_dijet_c#,JER_dijet_j#,JER_dijet_m#,JER_dijet_p#;JER_dijet_stat#"
    compLabels="Nominal data vs MC difference;Noise term, random cones method;Dijet #it{in situ} JER (systematics);Dijet #it{in situ} JER (statistics)"
    options="prefix=JET_;fixedEtaVals=0.202;fixedPtVals=40;path=\"/eos/atlas/atlascerngroupdisk/perf-jets/JetUncertainties/CalibArea-05/\";scaleVar=${scaleVar};IsData=false;axisMax=0.05"
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
#echo "$BOOLT"
for anArg in $@ ; do
    #echo "start loop"
    #echo "$firstArg"
    #echo "$BOOLT"
    #echo "$anArg"
    if [ "$firstArg" = "$BOOLT" ] ; then
        # Run the specified scenario
        #echo "running"
        #echo "$1"
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

echo "Running with options: $options"
MakeUncertaintyPlots "$outFile" "$jetDefinition" "$MCtype" "$configFile" "$compList" "$compLabels" "$options"

