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
    options="isDijet=true;isGSC=false;isLargeR=false;prefix=JET_"
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
    options="isDijet=true;isGSC=false;isLargeR=false;prefix=JET_"
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
    options="isDijet=true;isGSC=false;isLargeR=false;prefix=JET_"
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
    options="isDijet=true;isGSC=false;isLargeR=true"
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
    options="isDijet=true;isGSC=true;isLargeR=false;prefix=JET_"
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
    options="isDijet=true;isGSC=true;isLargeR=false;prefix=JET_"
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
    options="isDijet=true;isGSC=true;isLargeR=false;prefix=JET_"
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
    options="isDijet=true;isGSC=true;isLargeR=false;prefix=JET_"
}

function Final2012_OnlyLAr \
{
    # Load default Moriond 2013 release
    Final2012
    # Override variables
    outFile="JetUncertainties-Final2012-OnlyLAr.pdf"
    compList="LAr_#;LAr_ESmaterial;LAr_ESpresampler;LAr_ESZee;LAr_Esmear"
    compLabels="e/#gamma, total (all components);e/#gamma energy scale, material;e/#gamma energy scale, presampler;e/#gamma energy scale, Zee;e/#gamma energy smearing"
}

function Final2012_OnlyZjet \
{
    # Load default Moriond 2013 release
    Final2012
    # Override variables
    outFile="JetUncertainties-Final2012-OnlyZjet.pdf"
    compList="Zjet_#;Zjet_dPhi;Zjet_MC;Zjet_MuScale;Zjet_MuSmearID;Zjet_MuSmearMS;Zjet_JVF;Zjet_KTerm;Zjet_Veto;Zjet_Stat#"
    compLabels="Z+jet, total (all components);Z+jet, extrapolation;Z+jet, MC generator;Z+jet, #mu scale;Z+jet, #mu smearing (ID);Z+jet, #mu smearing (MS);Z+jet, pileup rejection;Z+jet, out-of-cone;Z+jet, radiation suppression;Z+jet, combined statistics (x11)"
}

function Final2012_OnlyGjet \
{
    # Load default Moriond 2013 release
    Final2012
    # Override variables
    outFile="JetUncertainties-Final2012-OnlyGjet.pdf"
    compList="Gjet_#;Gjet_dPhi;Gjet_Generator;Gjet_OOC;Gjet_Purity;Gjet_Veto;Gjet_Stat#"
    compLabels="#gamma+jet, total (all components);#gamma+jet, extrapolation;#gamma+jet, MC generator;#gamma+jet, out-of-cone;#gamma+jet, photon purity;#gamma+jet, radiation suppression; #gamma+jet, combined statistics (x13)"
}

function Final2012_OnlyMJB \
{
    # Load default Moriond 2013 release
    Final2012
    # Override variables
    outFile="JetUncertainties-Final2012-OnlyMJB.pdf"
    compList="MJB_#;MJB_Alpha;MJB_Beta;MJB_Fragmentation;MJB_Asym;MJB_Threshold;MJB_Stat#"
    compLabels="MJB, total (all components);MJB, #alpha selection;MJB, #beta selection;MJB, fragmentation;MJB, #it{p}_{T} asymmetry;MJB, #it{p}_{T} threshold;MJB, combined statistics (x10)"
}

function Final2012_OnlyEtaInt \
{
    # Load default Moriond 2013 release
    Final2012
    # Override variables
    outFile="JetUncertainties-Final2012-OnlyEtaInt.pdf"
    compList="EtaIntercalibration_#;EtaIntercalibration_Modelling;EtaIntercalibration_TotalStat"
    compLabels="#eta-intercalibration, total (all components);#eta-intercalibration, modelling;#eta-intercalibration, statistics"
}

function Final2012_OnlyPileup \
{
    # Load default Moriond 2013 release
    Final2012
    # Override variables
    outFile="JetUncertainties-Final2012-OnlyPileup.pdf"
    compList="Pileup_#;Pileup_OffsetMu;Pileup_OffsetNPV;Pileup_PtTerm;Pileup_RhoTopology"
    compLabels="Pileup, total (all components);Pileup, <#mu> offset;Pileup, NPV offset;Pileup, #it{p}_{T} bias;Pileup, #rho topology"
}

function Final2012_OnlyFlavour \
{
    # Load default Moriond 2013 release
    Final2012
    # Override variables
    outFile="JetUncertainties-Final2012-OnlyFlavour.pdf"
    compList="Flavor_#;Flavor_Comp#;Flavor_Resp#"
    compLabels="Flavour, total (all components);Flavour composition;Flavour response"
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

