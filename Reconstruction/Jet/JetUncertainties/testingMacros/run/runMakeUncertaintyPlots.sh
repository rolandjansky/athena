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
    options="${options};isPublic=True;ATLASLabel=Preliminary;Interp=True"
    compLabels="${compLabels/Pileup/Pileup, average 2015 conditions}"
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





function Moriond2016_SR_Scenario1 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/Moriond2016/JES2015_SR_Scenario1.config"
    outFile="JetUncertainties-Moriond2016-SR-Scenario1.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3;EtaIntercalibration_NonClosure"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3;#eta-intercalibration nonclosure"
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
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3;EtaIntercalibration_NonClosure"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3;#eta-intercalibration nonclosure"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}
function Moriond2016_SR_Scenario3 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/Moriond2016/JES2015_SR_Scenario3.config"
    outFile="JetUncertainties-Moriond2016-SR-Scenario3.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3;EtaIntercalibration_NonClosure"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3;#eta-intercalibration nonclosure"
    options="isDijet=false;isLargeR=false;prefix=JET_"
}
function Moriond2016_SR_Scenario4 \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFile="JES_2015/Moriond2016/JES2015_SR_Scenario4.config"
    outFile="JetUncertainties-Moriond2016-SR-Scenario4.pdf"
    compList="GroupedNP_1;GroupedNP_2;GroupedNP_3;EtaIntercalibration_NonClosure"
    compLabels="Grouped NP 1;Grouped NP 2;Grouped NP 3;#eta-intercalibration nonclosure"
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
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2015/ICHEP2016/TopTagging_CombMass_all.config;UJ_2015/ICHEP2016/TopTagging_CombMass_all.config;UJ_2015/ICHEP2016/TopTagging_all.config;UJ_2015/ICHEP2016/TopTagging_TAMass_all.config;UJ_2015/ICHEP2016/TopTagging_CombMass_all.config"
    outFile="JetUncertainties-LargeR-ICHEP2016-masscomp.pdf"
    compList="CaloWeight@TAWeight@TOTAL@TOTAL@TOTAL"
    compLabels="(Calorimeter weight)/10@(Track-assisted weight)/10@Calorimeter mass@Track-assisted mass@Combined mass"
    options="isDijet=false;isLargeR=true;prefix=JET_;specifyTagger=false;drawTotal=false;scaleVar=mass"
}

function LargeR_ICHEP2016_validation \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC15c"
    configFile="UJ_2015/ICHEP2016/WZTagging_CONFIGTYPE.config;UJ_2015/ICHEP2016/HbbTagging_CONFIGTYPE.config;UJ_2015/ICHEP2016/TopTagging_CONFIGTYPE.config;UJ_2015/ICHEP2016/MultiTagging_CONFIGTYPE.config"
    #configFile="UJ_2015/ICHEP2016/WZTagging_TAMass_CONFIGTYPE.config;UJ_2015/ICHEP2016/HbbTagging_TAMass_CONFIGTYPE.config;UJ_2015/ICHEP2016/TopTagging_TAMass_CONFIGTYPE.config;UJ_2015/ICHEP2016/MultiTagging_TAMass_CONFIGTYPE.config"
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
    else
        compListBase="Rtrk_Baseline_pT,Rtrk_Baseline_mass,Rtrk_Baseline_D2;Rtrk_Modelling_pT,Rtrk_Modelling_mass,Rtrk_Modelling_D2;Rtrk_Tracking_pT,Rtrk_Tracking_mass,Rtrk_Tracking_D2;Rtrk_TotalStat_pT,Rtrk_TotalStat_mass,Rtrk_TotalStat_D2"
    fi
    compList="$compListBase@$compListBase@${compListBase//D2/Tau32}"
    if [[ $configFile = *_TAMass_* ]] ; then
        compList="${compList}@Rtrk_Baseline_pT,TAM_Baseline_mass,Rtrk_Baseline_D2,Rtrk_Baseline_Tau32;Rtrk_Modelling_pT,TAM_Modelling_mass,Rtrk_Modelling_D2,Rtrk_Modelling_Tau32;Rtrk_Tracking_pT,TAM_Tracking_mass,Rtrk_Tracking_D2,Rtrk_Tracking_Tau32;Rtrk_TotalStat_pT,TAM_TotalStat_mass,Rtrk_TotalStat_D2,Rtrk_TotalStat_Tau32"
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
    else
        compListBase="Rtrk_Baseline_Kin,Rtrk_Baseline_D2;Rtrk_Modelling_Kin,Rtrk_Modelling_D2;Rtrk_Tracking_Kin,Rtrk_Tracking_D2;Rtrk_TotalStat_Kin,Rtrk_TotalStat_D2"
    fi
    compList="$compListBase@$compListBase@${compListBase//D2/Tau32}"
    if [[ $configFile = *_TAMass_* ]] ; then
        compList="${compList}@TAM_Baseline_Kin,Rtrk_Baseline_Sub;TAM_Modelling_Kin,Rtrk_Modelling_Sub;TAM_Tracking_Kin,Rtrk_Tracking_Sub;TAM_TotalStat_Kin,Rtrk_TotalStat_Sub"
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

