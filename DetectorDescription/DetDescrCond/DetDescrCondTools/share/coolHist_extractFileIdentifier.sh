#!/bin/bash
# coolHist_extractFileIdentifier.sh <file>
# print the CoolHist file GUID stored in the fileGUID object in a ROOT file
# also leave the guid in a file coolhist_guid.tmp so can be picked up by tools
# Richard Hawkings 30/1/07
#
# one parameter, ROOT file name
if [ $# -ne 1 ] 
then
  echo 'Syntax: coolHist_extractFileIdentifier.sh <file>'
  exit
fi
RFILE=$1
# create a temporary file with the ROOT commands
MYFILE=/tmp/coolHist_extractFileIdentifier_$$.C
cat > $MYFILE <<EOF
void coolHist_extractFileIdentifier_$$(const char* file) {
  cout << "Get GUID from file " << file << endl;
  TFile* myfile=new TFile(file,"READ");
  TObjString* oguid;
  myfile->GetObject("fileGUID",oguid);
  if (oguid!=0) {
    cout << "GUID is " << oguid->GetString() << endl;
    TString comm="echo "+(oguid->GetString())+" > coolhist_guid.tmp";
    system(comm.Data());
    myfile->Close();
  } else {
    cout << "File has no fileGUID object" << endl;
    system("echo none > coolhist_guid.tmp");
  }
}
EOF
# run ROOT in batch mode to execute this command
root -b -q ${MYFILE}\(\"${RFILE}\"\)
rm $MYFILE
exit
