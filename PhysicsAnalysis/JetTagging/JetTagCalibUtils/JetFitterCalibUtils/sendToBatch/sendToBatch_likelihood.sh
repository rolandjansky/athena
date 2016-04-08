#!/bin/sh
for a in $(ls); do
    echo "Processing directory: " $a
    cd $a

    echo "Creating directories if missing"
    if $(test ! -e likelihood);then mkdir likelihood;fi

    echo "Processing b"

    mkdir trainingResultsLikelihood
    mkdir trainingResultsLikelihood/plots

    cd likelihood

    ln -s ../../../likelihoodRoot/createTheHistograms.cxx
    ln -s ../../../likelihoodRoot/createTheHistograms.h
    ln -s ../../../likelihoodRoot/doCreateTheHistograms.C


    actualdir=$(pwd)

    echo "Create batch comb file for " $a
    
    echo '================='
    echo 'LLtrainB' $a '.pbs'
    echo '================='

    filename=$(echo 'LLtrainB'$a'.pbs')

    

    echo "#!/bin/sh" > $filename
    echo "" >> $filename
    echo "unset DISPLAY" >> $filename
    echo "echo Runnin on host \`hostname\`" >> $filename
    echo "cd  "$actualdir" " >> $filename
    echo "root -b \"doCreateTheHistograms.C(\\\""$a"\\\",5)\"|tee "$actualdir"/logTrainingB.txt " >> $filename
    echo "echo \"Finished\" " >> $filename
    chmod a+x $filename

    qsub -N $(echo "LL"$a"B") -l qt=1nw,mem_alloc=2000M,systemtype=4 $HOME/bin/SGE_Start $filename

    cd ..
 
    echo "Processing c"
    cd likelihood

    actualdir=$(pwd)

    echo "Create batch comb file for " $a
    
    echo '================='
    echo 'LLtrainC' $a '.pbs'
    echo '================='

    filename=$(echo 'LLtrainC'$a'.pbs')

    

    echo "#!/bin/sh" > $filename
    echo "" >> $filename
    echo "unset DISPLAY" >> $filename
    echo "echo Runnin on host \`hostname\`" >> $filename
    echo "cd  "$actualdir" " >> $filename
    echo "root -b \"doCreateTheHistograms.C(\\\""$a"\\\",4)\"|tee "$actualdir"/logTrainingC.txt " >> $filename
    echo "echo \"Finished\" " >> $filename
    chmod a+x $filename
    
    qsub -N $(echo "LL"$a"C") -l qt=1nw,mem_alloc=2000M,systemtype=4 $HOME/bin/SGE_Start $filename

    cd ..
    echo "Processing l"
    cd likelihood

    actualdir=$(pwd)

    echo "Create batch comb file for " $a
    
    echo '================='
    echo 'LLtrainL' $a '.pbs'
    echo '================='

    filename=$(echo 'LLtrainL'$a'.pbs')

    

    echo "#!/bin/sh" > $filename
    echo "" >> $filename
    echo "unset DISPLAY" >> $filename
    echo "echo Runnin on host \`hostname\`" >> $filename
    echo "cd  "$actualdir" " >> $filename
    echo "root -b \"doCreateTheHistograms.C(\\\""$a"\\\",1)\"|tee "$actualdir"/logTrainingL.txt " >> $filename
    echo "echo \"Finished\" " >> $filename
    chmod a+x $filename

    qsub -N $(echo "LL"$a"L") -l qt=1nw,mem_alloc=2000M,systemtype=4 $HOME/bin/SGE_Start $filename

    cd ../..;
done

   
    
	
 
    
    
    
    