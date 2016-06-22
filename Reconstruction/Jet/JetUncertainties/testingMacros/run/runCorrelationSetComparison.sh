#!/bin/bash

corrPlotScript="bash runMakeCorrelationMatrixPlots.sh"
corrCompScript="python ../python_scripts/Make4DCorrelationMatrix.py"
GridType="FineGridFixedEta"
corrUncFile="CorrUnc.root"
jetType="AntiKt4EMTopo"
spacing="50ns"
isRelative="false"


# outFile=CorrUnc.pdf configFiles="JES_2012/Final/InsituJES2012_AllNuisanceParameters.config;JES_2012/Final/InsituJES2012_StrongerCorrelations.config;JES_2012/Final/InsituJES2012_WeakerCorrelations.config" bash runMakeCorrelationMatrixPlots.sh Flexible FineGridFixedEta


# Check arguments
if test $# -ne 1 && test $# -ne 4; then
    echo "USAGE: $0 <scenario type>"
    echo "Scenario types available:"
    echo "  1. \"3NP\" for set of 4 configs with 3NP each"
    echo "  2. \"4NP\" for set of 3 configs with 4NP each"
    echo "  3. \"Nominal\" for the uncertainties on the correlations from the nominal sets"
    echo "  4. \"generic\" for interpreting command line arguments (more help if only this is specified)"
    exit 1
fi


#if ! [[ -e "$corrUncFile" ]] ; then
#    $corrPlotScript Final2012_Nominal_Stronger $GridType $isRelative
#    if test $? -ne 0 ; then
#        exit -1
#    fi
#    mv CorrelationMatrix-Final2012-Nominal-Stronger.pdf ${corrUncFile/.root/.pdf}
#    mv CorrelationMatrix-Final2012-Nominal-Stronger.root $corrUncFile
#fi

if [[ $1 = generic ]] ; then
    if test $# -ne 4 ; then
        echo "USAGE of generic: $0 generic <release path> <base of config name> < ref config name>"
        echo "  Release path example: JES_2015/Moriond2016"
        echo "  Base of config name example: JES2015_SR"
        echo "  Reference config name example: JES2015_AllNuisanceParameters.config"
        exit 1
    fi
    configBase="$PWD/../../share/$2/$3"
    refConfig="$PWD/../../share/$2/$4"
    config1="${configBase}_Scenario1.config"
    config2="${configBase}_Scenario2.config"
    config3="${configBase}_Scenario3.config"
    config4="${configBase}_Scenario4.config"

    # Ensure these worked
    if ! [ -f $refConfig ] ; then
        echo "Reference config doesn't exist: $refConfig"
        exit 1
    elif ! [ -f $config1 ] ; then
        echo "Config1 doesn't exist: $config1"
        exit 1
    elif ! [ -f $config2 ] ; then
        echo "Config2 doesn't exist: $config2"
        exit 1
    elif ! [ -f $config3 ] ; then
        echo "Config3 doesn't exist: $config3"
        exit 1
    elif ! [ -f $config4 ] ; then
        echo "Config4 doesn't exist: $config4"
        exit 1
    fi

    plot1="NONE" #"$PWD/SR1.pdf"
    plot2="NONE" #"$PWD/SR2.pdf"
    plot3="NONE" #"$PWD/SR3.pdf"
    plot4="NONE" #"$PWD/SR4.pdf"

    root1="$PWD/SR1.root"
    root2="$PWD/SR2.root"
    root3="$PWD/SR3.root"
    root4="$PWD/SR4.root"

    outFile="$PWD/SR-4D.png"

    grid="FineGridFixedEta"
    if [[ $2 = TLA* ]] ; then
        echo "Switching to TLA grid"
        grid="FineGridFixedEtaTLA"
    fi
    
    
    # Ensure root files are newer than config files, otherwise re-produce root files
    if ! [[ -e "$root1" ]] || [[ "$config1" -nt "$root1" ]] ; then
        jetDefinition="AntiKt4EMTopo" configFiles="$refConfig;$config1" outFile="$plot1" outHistFile="$root1" $corrPlotScript Flexible $grid
        #$corrPlotScript Final2012_Nominal_3NP1 $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        #mv CorrelationMatrix-Final2012-Nominal-3NP1.pdf $plot1
        #mv CorrelationMatrix-Final2012-Nominal-3NP1.root $root1
    fi
    if ! [[ -e "$root2" ]] || [[ "$config2" -nt "$root2" ]] ; then
        jetDefinition="AntiKt4EMTopo" configFiles="$refConfig;$config2" outFile="$plot2" outHistFile="$root2" $corrPlotScript Flexible $grid
        #$corrPlotScript Final2012_Nominal_3NP2 $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        #mv CorrelationMatrix-Final2012-Nominal-3NP2.pdf $plot2
        #mv CorrelationMatrix-Final2012-Nominal-3NP2.root $root2
    fi
    if ! [[ -e "$root3" ]] || [[ "$config3" -nt "$root3" ]] ; then
        jetDefinition="AntiKt4EMTopo" configFiles="$refConfig;$config3" outFile="$plot3" outHistFile="$root3" $corrPlotScript Flexible $grid
        #$corrPlotScript Final2012_Nominal_3NP3 $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        #mv CorrelationMatrix-Final2012-Nominal-3NP3.pdf $plot3
        #mv CorrelationMatrix-Final2012-Nominal-3NP3.root $root3
    fi
    if ! [[ -e "$root4" ]] || [[ "$config4" -nt "$root4" ]] ; then
        jetDefinition="AntiKt4EMTopo" configFiles="$refConfig;$config4" outFile="$plot4" outHistFile="$root4" $corrPlotScript Flexible $grid
        #$corrPlotScript Final2012_Nominal_3NP4 $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        #mv CorrelationMatrix-Final2012-Nominal-3NP4.pdf $plot4
        #mv CorrelationMatrix-Final2012-Nominal-3NP4.root $root4
    fi
    
    # Now run comparison if any are newer
    if ! [[ -e "$outFile" ]] || [[ "$outFile" -ot "$root1" ]] || [[ "$outFile" -ot "$root2" ]] || [[ "$outFile" -ot "$root3" ]] || [[ "$outFile" -ot "$root4" ]] ; then
        echo ""
        echo "Running comparison script..."
        echo ""
        if [ -e $corrUncFile ] ; then
            corrFile="$corrUncFile"
        else
            corrFile="NONE"
        fi
        $corrCompScript $jetType $outFile $corrFile $isRelative $root1 $root2 $root3 $root4
    else
        echo "Nothing to do - output file is already up to date"
    fi
elif [[ $1 = 3NP ]] ; then
    # Files
    config1="$PWD/../../share/JES_2012/Final/InsituJES2012_3NP_Scenario1.config"
    config2="$PWD/../../share/JES_2012/Final/InsituJES2012_3NP_Scenario2.config"
    config3="$PWD/../../share/JES_2012/Final/InsituJES2012_3NP_Scenario3.config"
    config4="$PWD/../../share/JES_2012/Final/InsituJES2012_3NP_Scenario4.config"
    config5="$PWD/../../share/JES_2012/Final/InsituJES2012_3NP_Scenario5.config"

    plot1="$PWD/3NP1.pdf"
    plot2="$PWD/3NP2.pdf"
    plot3="$PWD/3NP3.pdf"
    plot4="$PWD/3NP4.pdf"
    plot5="$PWD/3NP5.pdf"
    
    root1="$PWD/3NP1.root"
    root2="$PWD/3NP2.root"
    root3="$PWD/3NP3.root"
    root4="$PWD/3NP4.root"
    root5="$PWD/3NP5.root"

    outFile="$PWD/3NP-4D.png"
    #outFile="$PWD/3NP-4D.eps"

    # Ensure root files are newer than config files, otherwise re-produce root files
    if ! [[ -e "$root1" ]] || [[ "$config1" -nt "$root1" ]] ; then
        $corrPlotScript Final2012_Nominal_3NP1 $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        mv CorrelationMatrix-Final2012-Nominal-3NP1.pdf $plot1
        mv CorrelationMatrix-Final2012-Nominal-3NP1.root $root1
    fi
    if ! [[ -e "$root2" ]] || [[ "$config2" -nt "$root2" ]] ; then
        $corrPlotScript Final2012_Nominal_3NP2 $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        mv CorrelationMatrix-Final2012-Nominal-3NP2.pdf $plot2
        mv CorrelationMatrix-Final2012-Nominal-3NP2.root $root2
    fi
    if ! [[ -e "$root3" ]] || [[ "$config3" -nt "$root3" ]] ; then
        $corrPlotScript Final2012_Nominal_3NP3 $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        mv CorrelationMatrix-Final2012-Nominal-3NP3.pdf $plot3
        mv CorrelationMatrix-Final2012-Nominal-3NP3.root $root3
    fi
    if ! [[ -e "$root4" ]] || [[ "$config4" -nt "$root4" ]] ; then
        $corrPlotScript Final2012_Nominal_3NP4 $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        mv CorrelationMatrix-Final2012-Nominal-3NP4.pdf $plot4
        mv CorrelationMatrix-Final2012-Nominal-3NP4.root $root4
    fi
    #if ! [[ -e "$root5" ]] || [[ "$config5" -nt "$root5" ]] ; then
    #    $corrPlotScript Final2012_Nominal_3NP5 $GridType $isRelative
    #    if test $? -ne 0 ; then
    #        exit -1
    #    fi
    #    mv CorrelationMatrix-Final2012-Nominal-3NP5.pdf $plot5
    #    mv CorrelationMatrix-Final2012-Nominal-3NP5.root $root5
    #fi
    
    # Now run comparison if any are newer
    #if ! [[ -e "$outFile" ]] || [[ "$outFile" -ot "$root1" ]] || [[ "$outFile" -ot "$root2" ]] || [[ "$outFile" -ot "$root3" ]] || [[ "$outFile" -ot "$root4" ]] || [[ "$outFile" -ot "$root5" ]] ; then
    if ! [[ -e "$outFile" ]] || [[ "$outFile" -ot "$root1" ]] || [[ "$outFile" -ot "$root2" ]] || [[ "$outFile" -ot "$root3" ]] || [[ "$outFile" -ot "$root4" ]] ; then
        echo ""
        echo "Running comparison script..."
        echo ""
        #$corrCompScript $jetType $outFile $corrUncFile $isRelative $root1 $root2 $root3 $root4 $root5
        $corrCompScript $jetType $outFile $corrUncFile $isRelative $root1 $root2 $root3 $root4
    else
        echo "Nothing to do - output file is already up to date"
    fi
elif [[ $1 = 4NP ]] ; then
    # Files
    config1="$PWD/../../share/JES_2012/Final/InsituJES2012_4NP_Scenario1.config"
    config2="$PWD/../../share/JES_2012/Final/InsituJES2012_4NP_Scenario2.config"
    config3="$PWD/../../share/JES_2012/Final/InsituJES2012_4NP_Scenario3.config"

    plot1="$PWD/4NP1.pdf"
    plot2="$PWD/4NP2.pdf"
    plot3="$PWD/4NP3.pdf"
    
    root1="$PWD/4NP1.root"
    root2="$PWD/4NP2.root"
    root3="$PWD/4NP3.root"

    outFile="$PWD/4NP-4D.pdf"

    # Ensure root files are newer than config files, otherwise re-produce root files
    if ! [[ -e "$root1" ]] || [[ "$config1" -nt "$root1" ]] ; then
        $corrPlotScript Final2012_Nominal_4NP1 $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        mv CorrelationMatrix-Final2012-Nominal-4NP1.pdf $plot1
        mv CorrelationMatrix-Final2012-Nominal-4NP1.root $root1
    fi
    if ! [[ -e "$root2" ]] || [[ "$config2" -nt "$root2" ]] ; then
        $corrPlotScript Final2012_Nominal_4NP2 $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        mv CorrelationMatrix-Final2012-Nominal-4NP2.pdf $plot2
        mv CorrelationMatrix-Final2012-Nominal-4NP2.root $root2
    fi
    if ! [[ -e "$root3" ]] || [[ "$config3" -nt "$root3" ]] ; then
        $corrPlotScript Final2012_Nominal_4NP3 $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        mv CorrelationMatrix-Final2012-Nominal-4NP3.pdf $plot3
        mv CorrelationMatrix-Final2012-Nominal-4NP3.root $root3
    fi
    
    # Now run comparison if any are newer
    if ! [[ -e "$outFile" ]] || [[ "$outFile" -ot "$root1" ]] || [[ "$outFile" -ot "$root2" ]] || [[ "$outFile" -ot "$root3" ]] ; then
        echo ""
        echo "Running comparison script..."
        echo ""
        $corrCompScript $jetType $outFile $corrUncFile $isRelative $root1 $root2 $root3
    else
        echo "Nothing to do - output file is already up to date"
    fi
elif [[ $1 = Nominal ]] || [[ $1 = nominal ]] ; then
    # Files
    config1="$PWD/../../share/JES_2012/Final/InsituJES2012_WeakerCorrelations.config"
    config2="$PWD/../../share/JES_2012/Final/InsituJES2012_StrongerCorrelations.config"

    plot1="$PWD/Weaker.pdf"
    plot2="$PWD/Stronger.pdf"
    
    root1="$PWD/Weaker.root"
    root2="$PWD/Stronger.root"

    outFile="$PWD/Nominal-4D.pdf"
    
    # Ensure root files are newer than config files, otherwise re-produce root files
    if ! [[ -e "$root1" ]] || [[ "$config1" -nt "$root1" ]] ; then
        $corrPlotScript Final2012_Nominal_Weaker $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        mv CorrelationMatrix-Final2012-Nominal-Weaker.pdf $plot1
        mv CorrelationMatrix-Final2012-Nominal-Weaker.root $root1
    fi
    if ! [[ -e "$root2" ]] || [[ "$config2" -nt "$root2" ]] ; then
        $corrPlotScript Final2012_Nominal_Stronger $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        mv CorrelationMatrix-Final2012-Nominal-Stronger.pdf $plot2
        mv CorrelationMatrix-Final2012-Nominal-Stronger.root $root2
    fi
    
    # Now run comparison if any are newer
    if ! [[ -e "$outFile" ]] || [[ "$outFile" -ot "$root1" ]] || [[ "$outFile" -ot "$root2" ]] ; then
        echo ""
        echo "Running comparison script..."
        echo ""
        $corrCompScript $jetType $corrUncFile $outFile $isRelative $root1 $root2
    else
        echo "Nothing to do - output file is already up to date"
    fi
elif [[ $1 = JER ]] ; then

    GridType="JER_grid"

    # Files
    config1="$PWD/../../share/JER_2012/JER_Reduced1.config"
    config2="$PWD/../../share/JER_2012/JER_Reduced2.config"
    config3="$PWD/../../share/JER_2012/JER_Reduced3.config"
    config4="$PWD/../../share/JER_2012/JER_Reduced4.config"

    plot1="$PWD/JER1.pdf"
    plot2="$PWD/JER2.pdf"
    plot3="$PWD/JER3.pdf"
    plot4="$PWD/JER4.pdf"
    
    root1="$PWD/JER1.root"
    root2="$PWD/JER2.root"
    root3="$PWD/JER3.root"
    root4="$PWD/JER4.root"

    #outFile="$PWD/JER-4D.png"
    #outFile="$PWD/JER-4D.eps"
    outFile="$PWD/JER-4D.pdf"

    # Ensure root files are newer than config files, otherwise re-produce root files
    if ! [[ -e "$root1" ]] || [[ "$config1" -nt "$root1" ]] ; then
        if [[ -e "$config1" ]] ; then
            $corrPlotScript JER_R1 $GridType $isRelative
            if test $? -ne 0 ; then
                exit -1
            fi
            mv CorrelationMatrix-JER-R1.pdf $plot1
            mv CorrelationMatrix-JER-R1.root $root1
        fi
    fi
    if ! [[ -e "$root2" ]] || [[ "$config2" -nt "$root2" ]] ; then
        if [[ -e "$config2" ]] ; then
            $corrPlotScript JER_R2 $GridType $isRelative
            if test $? -ne 0 ; then
                exit -1
            fi
            mv CorrelationMatrix-JER-R2.pdf $plot2
            mv CorrelationMatrix-JER-R2.root $root2
        fi
    fi
    if ! [[ -e "$root3" ]] || [[ "$config3" -nt "$root3" ]] ; then
        if [[ -e "$config3" ]] ; then
            $corrPlotScript JER_R3 $GridType $isRelative
            if test $? -ne 0 ; then
                exit -1
            fi
            mv CorrelationMatrix-JER-R3.pdf $plot3
            mv CorrelationMatrix-JER-R3.root $root3
        fi
    fi
    if ! [[ -e "$root4" ]] || [[ "$config4" -nt "$root4" ]] ; then
        if [[ -e "$config4" ]] ; then
            $corrPlotScript JER_R4 $GridType $isRelative
            if test $? -ne 0 ; then
                exit -1
            fi
            mv CorrelationMatrix-JER-R4.pdf $plot4
            mv CorrelationMatrix-JER-R4.root $root4
        fi
    fi
    
    # Now run comparison if any are newer
    #if ! [[ -e "$outFile" ]] || [[ "$outFile" -ot "$root1" ]] || [[ "$outFile" -ot "$root2" ]] || [[ "$outFile" -ot "$root3" ]] || [[ "$outFile" -ot "$root4" ]] || [[ "$outFile" -ot "$root5" ]] ; then
    if ! [[ -e "$outFile" ]] || ( [[ -e "$root1" ]] && [[ "$outFile" -ot "$root1" ]] ) || ( [[ -e "$root2" ]] && [[ "$outFile" -ot "$root2" ]] ) || ( [[ -e "$root3" ]] && [[ "$outFile" -ot "$root3" ]] ) || ( [[ -e "$root4" ]] && [[ "$outFile" -ot "$root4" ]] ) ; then
        echo ""
        echo "Running comparison script..."
        echo ""
        #$corrCompScript $jetType $outFile $corrUncFile $isRelative $root1 $root2 $root3 $root4 $root5
        filesThatExist=""
        if [[ -e "$root1" ]] ; then
            filesThatExist="${filesThatExist} $root1"
        fi
        if [[ -e "$root2" ]] ; then
            filesThatExist="${filesThatExist} $root2"
        fi
        if [[ -e "$root3" ]] ; then
            filesThatExist="${filesThatExist} $root3"
        fi
        if [[ -e "$root4" ]] ; then
            filesThatExist="${filesThatExist} $root4"
        fi
        $corrCompScript $jetType $outFile None $isRelative $filesThatExist
    else
        echo "Nothing to do - output file is already up to date"
    fi
elif [[ $1 = 2015 ]] ; then
    # Files
    config1="$PWD/../../share/JES_2015/Prerec/PrerecJES2015_3NP_Scenario1_${spacing}.config"
    config2="$PWD/../../share/JES_2015/Prerec/PrerecJES2015_3NP_Scenario2_${spacing}.config"
    config3="$PWD/../../share/JES_2015/Prerec/PrerecJES2015_3NP_Scenario3_${spacing}.config"
    config4="$PWD/../../share/JES_2015/Prerec/PrerecJES2015_3NP_Scenario4_${spacing}.config"

    plot1="$PWD/3NP1.pdf"
    plot2="$PWD/3NP2.pdf"
    plot3="$PWD/3NP3.pdf"
    plot4="$PWD/3NP4.pdf"
    plot5="$PWD/3NP5.pdf"
    
    root1="$PWD/3NP1.root"
    root2="$PWD/3NP2.root"
    root3="$PWD/3NP3.root"
    root4="$PWD/3NP4.root"
    root5="$PWD/3NP5.root"

    outFile="$PWD/3NP-4D.png"
    #outFile="$PWD/3NP-4D.eps"

    # Ensure root files are newer than config files, otherwise re-produce root files
    if ! [[ -e "$root1" ]] || [[ "$config1" -nt "$root1" ]] ; then
        $corrPlotScript Prerec2015_Nominal_3NP1_${spacing} $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        mv CorrelationMatrix-Prerec2015-Nominal-3NP1-${spacing}.pdf $plot1
        mv CorrelationMatrix-Prerec2015-Nominal-3NP1-${spacing}.root $root1
    fi
    if ! [[ -e "$root2" ]] || [[ "$config2" -nt "$root2" ]] ; then
        $corrPlotScript Prerec2015_Nominal_3NP2_${spacing} $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        mv CorrelationMatrix-Prerec2015-Nominal-3NP2-${spacing}.pdf $plot2
        mv CorrelationMatrix-Prerec2015-Nominal-3NP2-${spacing}.root $root2
    fi
    if ! [[ -e "$root3" ]] || [[ "$config3" -nt "$root3" ]] ; then
        $corrPlotScript Prerec2015_Nominal_3NP3_${spacing} $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        mv CorrelationMatrix-Prerec2015-Nominal-3NP3-${spacing}.pdf $plot3
        mv CorrelationMatrix-Prerec2015-Nominal-3NP3-${spacing}.root $root3
    fi
    if ! [[ -e "$root4" ]] || [[ "$config4" -nt "$root4" ]] ; then
        $corrPlotScript Prerec2015_Nominal_3NP4_${spacing} $GridType $isRelative
        if test $? -ne 0 ; then
            exit -1
        fi
        mv CorrelationMatrix-Prerec2015-Nominal-3NP4-${spacing}.pdf $plot4
        mv CorrelationMatrix-Prerec2015-Nominal-3NP4-${spacing}.root $root4
    fi
    #if ! [[ -e "$root5" ]] || [[ "$config5" -nt "$root5" ]] ; then
    #    $corrPlotScript Prerec2015_Nominal_3NP5_${spacing} $GridType $isRelative
    #    if test $? -ne 0 ; then
    #        exit -1
    #    fi
    #    mv CorrelationMatrix-Prerec2015-Nominal-3NP5-${spacing}.pdf $plot5
    #    mv CorrelationMatrix-Prerec2015-Nominal-3NP5-${spacing}.root $root5
    #fi
    
    # Now run comparison if any are newer
    #if ! [[ -e "$outFile" ]] || [[ "$outFile" -ot "$root1" ]] || [[ "$outFile" -ot "$root2" ]] || [[ "$outFile" -ot "$root3" ]] || [[ "$outFile" -ot "$root4" ]] || [[ "$outFile" -ot "$root5" ]] ; then
    if ! [[ -e "$outFile" ]] || [[ "$outFile" -ot "$root1" ]] || [[ "$outFile" -ot "$root2" ]] || [[ "$outFile" -ot "$root3" ]] || [[ "$outFile" -ot "$root4" ]] ; then
        echo ""
        echo "Running comparison script..."
        echo ""
        #$corrCompScript $jetType $outFile $corrUncFile $isRelative $root1 $root2 $root3 $root4 $root5
        $corrCompScript $jetType $outFile $corrUncFile $isRelative $root1 $root2 $root3 $root4
    else
        echo "Nothing to do - output file is already up to date"
    fi
else
    echo "Scenario type not recognized: $1"
    exit 2
fi


