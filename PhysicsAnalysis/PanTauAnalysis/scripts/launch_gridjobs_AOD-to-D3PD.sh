echo "INFO    ================================================"
echo "INFO        Script to submit all PanTau jobs to grid    "
echo "INFO    ================================================"
echo "INFO"



SAMPLES[0]=ZTauTau
SAMPLES[1]=ZPrime250TauTau
SAMPLES[2]=ZPrime750TauTau
SAMPLES[3]=WTauNu
SAMPLES[4]=J0
SAMPLES[5]=J1
SAMPLES[6]=J2
SAMPLES[7]=J3
SAMPLES[8]=J4
SAMPLES[9]=J5


if [ "$1" = "submit" ]; then

    echo "INFO    Will submit a whole bunch of jobs!"
    
    for iSample in "${SAMPLES[@]}"
    do
        ToExecute=". ../scripts/submit-grid-job_PanTau_User_TopOptions.sh $iSample $2"
        echo "INFO    ___________________________________________________________________________"
        echo "INFO"
        echo "INFO                               ===> Next Job! <==="
        echo "INFO    ___________________________________________________________________________"
        echo "INFO"
        echo "INFO    Executing: $ToExecute"
        $ToExecute
    done
    
    
else
    echo "INFO    You have to be inside your \$TestArea/PhysicsAnalysis/PanTauAnalysis/run"
    echo "INFO    Otherwise this script will not work and do terrible things".
    echo "INFO    To submit jobs, be sure to be in the directory above and do:"
    echo "INFO       . ../scripts/Launch_WholeBunchOfSamplesToGrid.sh submit xyz"
    echo "INFO    where xyz steers the actual submitting script:"
    echo "INFO       xyz omitted : just print commands, do not execute anything"
    echo "INFO       xyz = local : runs commands locally (not encouraged!)"
    echo "INFO       xyz = grid  : submit jobs to grid"
fi