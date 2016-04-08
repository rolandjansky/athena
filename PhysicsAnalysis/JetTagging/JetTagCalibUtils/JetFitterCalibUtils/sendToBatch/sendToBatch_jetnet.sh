#!/bin/sh
for a in $(ls); do
    echo "Processing directory: " $a
    cd $a

    echo "Creating directories if missing"
    if $(test ! -e comb);then mkdir comb;fi
    if $(test ! -e standalone);then mkdir standalone;fi

    echo "Processing comb"

    cd comb

    mkdir weights

    echo "Linking necessary macros..."
    ln -s ../../../jetnetRoot/libTJetNet.so
    ln -s ../../../jetnetRoot/trainNN_cxx.so
    ln -s ../../../jetnetRoot/doTraining.C

    actualdir=$(pwd)

    echo "Create batch comb file for " $a
    
    echo '================='
    echo 'NNtraincomb' $a '.pbs'
    echo '================='

    filename=$(echo 'NNtraincomb'$a'.pbs')

    

    echo "#!/bin/sh" > $filename
    echo "" >> $filename
    echo "unset DISPLAY" >> $filename
    echo "echo Runnin on host \`hostname\`" >> $filename
    echo "cd  "$actualdir" " >> $filename
    echo "root -b \"doTraining.C(\\\""$a"\\\",true)\"|tee logTraining.txt " >> $filename
    echo "echo \"Finished\" " >> $filename
    chmod a+x $filename

    qsub -N $(echo "NN"$a"comb") -l qt=1nw,mem_alloc=100M,systemtype=4 $HOME/bin/SGE_Start $filename

    cd ..
 
    echo "Processing standalone"
    cd standalone
    
    mkdir weights

    echo "Linking necessary macros..."
    ln -s ../../../jetnetRoot/libTJetNet.so
    ln -s ../../../jetnetRoot/trainNN_cxx.so
    ln -s ../../../jetnetRoot/doTraining.C


    actualdir=$(pwd)

    echo "Create batch standalone file for " $a
    
    echo '================='
    echo 'NNtrainstandalone' $a '.pbs'
    echo '================='

    filename=$(echo 'NNtrainstandalone'$a'.pbs')

    

    echo "#!/bin/sh" > $filename
    echo "" >> $filename
    echo "unset DISPLAY" >> $filename
    echo "echo Runnin on host \`hostname\`" >> $filename
    echo "cd  " $actualdir " " >> $filename
    echo "root -b \"doTraining.C(\\\""$a"\\\",false)\"|tee logTraining.txt " >> $filename
    echo "echo \"Finished\" " >>$filename

    chmod a+x $filename

    qsub -N $(echo "NN"$a"standalone") -l qt=1nw,mem_alloc=100M,systemtype=4 $HOME/bin/SGE_Start $filename

    cd ../..;
done

   
    
	
 
    
    
    
    