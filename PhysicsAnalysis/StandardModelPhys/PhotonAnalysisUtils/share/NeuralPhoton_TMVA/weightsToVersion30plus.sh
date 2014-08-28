#!/bin/bash

for d in $(ls -p | grep "/" | sed "s/\///"); do

  adapted=`grep "Variable VarIndex" $d/weights/$d.pt40_eta0.60_conv0.weights.xml | wc -l`
  if [ $adapted -eq 0 ]; then 
    echo "These appear to be already adapted for ROOT 5.30 and 5.32.  Exiting..." 
    continue
  fi

  for f in $(ls $d/weights/$d.pt*.xml); do
    echo $f
    total=`wc -l $f | awk '{print $1}'`
    line=`cat -n $f | grep "Transform Name" | awk '{print $1}'`
  
    head -$(($line-1)) $f > temp
  
    echo '    <Transform Name="Normalize">' >> temp
  
    echo '        <Selection>' >> temp
    for put in Input Output; do
      echo "          <$put N$(echo $put)s=\"10\">" >> temp
      for v in fside w1 wtot deltae eratio rphi rhad reta weta2 f1; do
        echo "            <$put Type=\"Variable\" Label=\"$v\" Expression=\"$v\"/>" >> temp
      done
      echo "          </$put>" >> temp
    done
    echo '        </Selection>' >> temp
  
    tail -$(($total - $line)) $f >> temp
  
    sed -i "s/Variables/Ranges/g" temp
    sed -i "s/Variable VarIndex/Range Index/g" temp
  
    mv temp $f
  done
done

