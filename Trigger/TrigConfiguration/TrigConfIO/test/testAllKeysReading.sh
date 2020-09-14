smk=1
while ( [[ smk -le 1 ]] )
do
    cmd="TestTriggerMenuAccess --smk ${smk}"
    echo "executing $cmd"
    $cmd 
    #2>1 > /dev/null
    STATUS=$status
    
    let smk++
done

