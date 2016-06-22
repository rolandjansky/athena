#!/bin/bash

BOOLT="true"
BOOLF="false"



function Flexible \
{
    # Try to fill variables
    if [ -z "$jetDefinition" ] ; then
        jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    fi
    if [ -z "$MCtype" ] ; then
        if ! [ -z "$configFiles" ] ; then
            if [[ $configFiles = *JES_2011* ]] ; then
                MCtype="MC11b"
            elif [[ $configFiles = *JES_2012* ]] ; then
                MCtype="MC12"
            elif [[ $configFiles = *JES_2015* ]] || [[ $configFiles = *TLA_2015* ]] ; then
                MCtype="MC15"
            fi
        fi
    fi

    if ! [ -z $outFile ] && [ -z $outHistFile ] ; then
        outHistFile="${outFile%.*}.root"
    fi
    
    # Now ensure everything is in place
    if   [ -z "$jetDefinition" ] ; then
        echo "Flexible: failed to determine jetDefinition"
        exit -1
    elif [ -z "$MCtype" ] ; then
        echo "Flexible: failed to determine MCtype"
        exit -2
    elif [ -z "$configFiles" ] ; then
        echo "Flexible: failed to determine configFiles"
        exit -3
    elif [ -z "$outFile" ] ; then
        echo "Flexible: failed to determine outFile"
        exit -4
    elif [ -z "$outHistFile" ] ; then
        echo "Flexible: failed to determine outHistFile"
        exit -5
    fi
}

function Final2012_Nominal_Stronger \
{
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFiles="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config;JES_2012/Final/InsituJES2012_StrongerCorrelations.config"
    outFile="CorrelationMatrix-Final2012-Nominal-Stronger.pdf"
    outHistFile="CorrelationMatrix-Final2012-Nominal-Stronger.root"
}

function Final2012_Nominal_Weaker \
{
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFiles="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config;JES_2012/Final/InsituJES2012_WeakerCorrelations.config"
    outFile="CorrelationMatrix-Final2012-Nominal-Weaker.pdf"
    outHistFile="CorrelationMatrix-Final2012-Nominal-Weaker.root"
}

function Final2012_Stronger_Weaker \
{
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFiles="JES_2012/Final/InsituJES2012_StrongerCorrelations.config;JES_2012/Final/InsituJES2012_WeakerCorrelations.config"
    outFile="CorrelationMatrix-Final2012-Stronger-Weaker.pdf"
    outHistFile="CorrelationMatrix-Final2012-Stronger-Weaker.root"
}

function Final2012_Nominal_3NP1 \
{
    # Final 2012 release, full vs 3NP scenario 1
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFiles="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config;JES_2012/Final/InsituJES2012_3NP_Scenario1.config"
    outFile="CorrelationMatrix-Final2012-Nominal-3NP1.pdf"
    outHistFile="CorrelationMatrix-Final2012-Nominal-3NP1.root"
}

function Final2012_Nominal_3NP2 \
{
    # Final 2012 release, full vs 3NP scenario 2
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFiles="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config;JES_2012/Final/InsituJES2012_3NP_Scenario2.config"
    outFile="CorrelationMatrix-Final2012-Nominal-3NP2.pdf"
    outHistFile="CorrelationMatrix-Final2012-Nominal-3NP2.root"
}

function Final2012_Nominal_3NP3 \
{
    # Final 2012 release, full vs 3NP scenario 3
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFiles="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config;JES_2012/Final/InsituJES2012_3NP_Scenario3.config"
    outFile="CorrelationMatrix-Final2012-Nominal-3NP3.pdf"
    outHistFile="CorrelationMatrix-Final2012-Nominal-3NP3.root"
}

function Final2012_Nominal_3NP4 \
{
    # Final 2012 release, full vs 3NP scenario 4
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    configFiles="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config;JES_2012/Final/InsituJES2012_3NP_Scenario4.config"
    outFile="CorrelationMatrix-Final2012-Nominal-3NP4.pdf"
    outHistFile="CorrelationMatrix-Final2012-Nominal-3NP4.root"
}

#function Final2012_Nominal_3NP5 \
#{
#    # Final 2012 release, full vs 3NP scenario 5
#    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
#    MCtype="MC12"
#    configFiles="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config;JES_2012/Final/InsituJES2012_3NP_Scenario5.config"
#    outFile="CorrelationMatrix-Final2012-Nominal-3NP5.pdf"
#    outHistFile="CorrelationMatrix-Final2012-Nominal-3NP5.root"
#}

#function Final2012_Nominal_4NP1 \
#{
#    # Final 2012 release, full vs 4NP scenario 1
#    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
#    MCtype="MC12"
#    configFiles="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config;JES_2012/Final/InsituJES2012_4NP_Scenario1.config"
#    outFile="CorrelationMatrix-Final2012-Nominal-4NP1.pdf"
#    outHistFile="CorrelationMatrix-Final2012-Nominal-4NP1.root"
#}
#
#function Final2012_Nominal_4NP2 \
#{
#    # Final 2012 release, full vs 4NP scenario 2
#    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
#    MCtype="MC12"
#    configFiles="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config;JES_2012/Final/InsituJES2012_4NP_Scenario2.config"
#    outFile="CorrelationMatrix-Final2012-Nominal-4NP2.pdf"
#    outHistFile="CorrelationMatrix-Final2012-Nominal-4NP2.root"
#}
#
#function Final2012_Nominal_4NP3 \
#{
#    # Final 2012 release, full vs 4NP scenario 3
#    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
#    MCtype="MC12"
#    configFiles="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config;JES_2012/Final/InsituJES2012_4NP_Scenario3.config"
#    outFile="CorrelationMatrix-Final2012-Nominal-4NP3.pdf"
#    outHistFile="CorrelationMatrix-Final2012-Nominal-4NP3.root"
#}

function Prerec2015_Nominal_3NP1_50ns \
{
    # Prerec 2015 release, full vs 3NP scenario 1
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFiles="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_50ns.config;JES_2015/Prerec/PrerecJES2015_3NP_Scenario1_50ns.config"
    outFile="CorrelationMatrix-Prerec2015-Nominal-3NP1-50ns.pdf"
    outHistFile="CorrelationMatrix-Prerec2015-Nominal-3NP1-50ns.root"
}

function Prerec2015_Nominal_3NP2_50ns \
{
    # Prerec 2015 release, full vs 3NP scenario 2
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFiles="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_50ns.config;JES_2015/Prerec/PrerecJES2015_3NP_Scenario2_50ns.config"
    outFile="CorrelationMatrix-Prerec2015-Nominal-3NP2-50ns.pdf"
    outHistFile="CorrelationMatrix-Prerec2015-Nominal-3NP2-50ns.root"
}

function Prerec2015_Nominal_3NP3_50ns \
{
    # Prerec 2015 release, full vs 3NP scenario 3
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFiles="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_50ns.config;JES_2015/Prerec/PrerecJES2015_3NP_Scenario3_50ns.config"
    outFile="CorrelationMatrix-Prerec2015-Nominal-3NP3-50ns.pdf"
    outHistFile="CorrelationMatrix-Prerec2015-Nominal-3NP3-50ns.root"
}

function Prerec2015_Nominal_3NP4_50ns \
{
    # Prerec 2015 release, full vs 3NP scenario 4
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFiles="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_50ns.config;JES_2015/Prerec/PrerecJES2015_3NP_Scenario4_50ns.config"
    outFile="CorrelationMatrix-Prerec2015-Nominal-3NP4-50ns.pdf"
    outHistFile="CorrelationMatrix-Prerec2015-Nominal-3NP4-50ns.root"
}

function Prerec2015_Nominal_3NP1_25ns \
{
    # Prerec 2015 release, full vs 3NP scenario 1
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFiles="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_25ns.config;JES_2015/Prerec/PrerecJES2015_3NP_Scenario1_25ns.config"
    outFile="CorrelationMatrix-Prerec2015-Nominal-3NP1-25ns.pdf"
    outHistFile="CorrelationMatrix-Prerec2015-Nominal-3NP1-25ns.root"
}

function Prerec2015_Nominal_3NP2_25ns \
{
    # Prerec 2015 release, full vs 3NP scenario 2
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFiles="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_25ns.config;JES_2015/Prerec/PrerecJES2015_3NP_Scenario2_25ns.config"
    outFile="CorrelationMatrix-Prerec2015-Nominal-3NP2-25ns.pdf"
    outHistFile="CorrelationMatrix-Prerec2015-Nominal-3NP2-25ns.root"
}

function Prerec2015_Nominal_3NP3_25ns \
{
    # Prerec 2015 release, full vs 3NP scenario 3
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFiles="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_25ns.config;JES_2015/Prerec/PrerecJES2015_3NP_Scenario3_25ns.config"
    outFile="CorrelationMatrix-Prerec2015-Nominal-3NP3-25ns.pdf"
    outHistFile="CorrelationMatrix-Prerec2015-Nominal-3NP3-25ns.root"
}

function Prerec2015_Nominal_3NP4_25ns \
{
    # Prerec 2015 release, full vs 3NP scenario 4
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFiles="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_25ns.config;JES_2015/Prerec/PrerecJES2015_3NP_Scenario4_25ns.config"
    outFile="CorrelationMatrix-Prerec2015-Nominal-3NP4-25ns.pdf"
    outHistFile="CorrelationMatrix-Prerec2015-Nominal-3NP4-25ns.root"
}

################################################
## Moriond 2016 correlation plots

function Moriond2016_Nominal_GlobalReduction \
{
    # Prerec 2015 release, full vs 3NP scenario 4
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFiles="JES_2015/Moriond2016/JES2015_AllNuisanceParameters.config;JES_2015/Moriond2016/JES2015_19NP.config"
    outFile="CorrelationMatrix-Moriond2016-Nominal-GlobalReduction.pdf"
    outHistFile="CorrelationMatrix-Moriond2016-NominalGlobalReduction.root"
}

function Moriond2016_Nominal_CategoryReduction \
{
    # Prerec 2015 release, full vs 3NP scenario 4
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC15"
    configFiles="JES_2015/Moriond2016/JES2015_AllNuisanceParameters.config;JES_2015/Moriond2016/JES2015_25NP_ByCategory.config"
    outFile="CorrelationMatrix-Moriond2016-Nominal-CategoryReduction.pdf"
    outHistFile="CorrelationMatrix-Moriond2016-Nominal-CategoryReduction.root"
}

function CoarseGrid \
{
    fixedEta="0;1.5;3.0;4.5"
    fixedPt="25;60;200;1000"
}

function CoarseGridFixedEta \
{
    fixedEta="0;1.5;3.0;4.5"
    fixedPt=""
}

function CoarseGridFixedPt \
{
    fixedEta=""
    fixedPt="25;60;200;1000"
}

function FineGrid \
{
    jetDefinition="AntiKt4EMTopo"
    fixedEta="0;0.5;1;1.5;2;2.5;3;3.5;4;4.5"
    fixedPt="25;40;60;100;150;200;300;400;500;1000"
}

function FineGridFixedEta \
{
    jetDefinition="AntiKt4EMTopo"
    fixedEta="0;0.5;1;1.5;2;2.5;3;3.5;4;4.5"
    fixedPt=""
}

function FineGridFixedEtaTLA \
{
    jetDefinition="AntiKt4EMTopo"
    fixedEta="0;0.5;1;1.5;2;2.4;2.8"
    fixedPt=""
}

function FineGridFixedPt \
{
    jetDefinition="AntiKt4EMTopo"
    fixedEta=""
    fixedPt="25;40;60;100;150;200;300;400;500;1000"
}


function JER_R1 \
{
    configFiles="JER_2012/JER_AllNuisanceParameters.config;JER_2012/JER_Reduced1.config"
    outFile="CorrelationMatrix-JER-R1.pdf"
    outHistFile="CorrelationMatrix-JER-R1.root"
}

function JER_R2 \
{
    configFiles="JER_2012/JER_AllNuisanceParameters.config;JER_2012/JER_Reduced2.config"
    outFile="CorrelationMatrix-JER-R2.pdf"
    outHistFile="CorrelationMatrix-JER-R2.root"
}

function JER_R3 \
{
    configFiles="JER_2012/JER_AllNuisanceParameters.config;JER_2012/JER_Reduced3.config"
    outFile="CorrelationMatrix-JER-R3.pdf"
    outHistFile="CorrelationMatrix-JER-R3.root"
}

function JER_R4 \
{
    configFiles="JER_2012/JER_AllNuisanceParameters.config;JER_2012/JER_Reduced4.config"
    outFile="CorrelationMatrix-JER-R4.pdf"
    outHistFile="CorrelationMatrix-JER-R4.root"
}

function JER_grid \
{
    jetDefinition="AntiKt4EMTopo" #"AntiKt4EMTopo;AntiKt4LCTopo;AntiKt6EMTopo;AntiKt6LCTopo"
    MCtype="MC12"
    fixedEta="0.01;0.81;1.21;2.11"
    fixedPt=""
}

function testGrid \
{
    jetDefinition="AntiKt4LCTopo"
    MCtype="MC12"
    #configFiles="JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_50ns.config;JES_2015/Prerec/PrerecJES2015_Dijet2.config"
    configFiles="JES_2012/Final/InsituJES2012_14NP.config;JES_2012/Final/InsituJES2012_AllNuisanceParameters.config"
    outFile="CorrelationMatrix-NominalInSitu-14NPInSitu-LC4.eps"
    outHistFile="" #"CorrelationMatrix-Prerec2015-Dijet2.root"
    fixedEta="0"
    fixedPt="" #"60"
    #fixedEta="0;0.5;1;1.5;2"
    #fixedPt=""
}

function testATLASCMS \
{
    scenario="TopMassDilep"
    jetDefinition="AntiKt4EMTopo;AntiKt4LCTopo"
    MCtype="MC12"
    configFiles="JES_2012/Final/CMS/FlavourTest${scenario}.config;JES_2012/Final/CMS/FlavourTest${scenario}Comb.config"
    outFile="ATLAS-CMS-CorrelationMatrix-${scenario}.pdf"
    outHistFile="ATLAS-CMS-CorrelationMatrix-${scenario}.root"
}



scenario="Scenario1"
if ! [[ -z $SCENARIO ]] ; then
    scenario=$SCENARIO
fi

# Check arguments
if test $# -lt 2 ; then
    echo "USAGE: $0 <scenario to run> <grid type to run>"
    echo "Scenarios available:"
    echo "  Final2012_Nominal_Stronger"
    echo "  Final2012_Nominal_Weaker"
    echo "  Final2012_Stronger_Weaker"
    echo "  Final2012_Nominal_3NP1"
    echo "  Final2012_Nominal_3NP2"
    echo "  Final2012_Nominal_3NP3"
    echo "  Final2012_Nominal_3NP4"
    echo "Grid types available:"
    echo "  CoarseGrid"
    echo "  CoarseGridFixedEta"
    echo "  CoarseGridFixedPt"
    echo "  FineGrid"
    echo "  FineGridFixedEta"
    echo "  FineGridFixedPt"
    exit 1
fi

# Setup the scenario and sanity check
$1
if [ -z "$outFile" ] ; then
    echo "Output file is not specified"
    echo "Check that you spelled the desired scenario correctly"
    exit 2
fi

# Setup the grid and sanity check
$2
if [ -z "$fixedEta" ] && [ -z "$fixedPt" ] ; then
    echo "Grid fixed values not specified"
    echo "Check that you spelled the desired scenario correctly"
    exit 3
fi

# Check if they specified whether this is relative differences or not
isRelative="false"
if test $# -gt 2 ; then
    isRelative="$3"
fi

MakeCorrelationMatrixPlots "$outFile" "$jetDefinition" "$MCtype" "$configFiles" "$fixedEta" "$fixedPt" "$outHistFile" "$isRelative"

