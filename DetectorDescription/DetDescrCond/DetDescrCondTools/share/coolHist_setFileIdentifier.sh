#!/bin/bash
# coolHist_setFileIdentifier.sh <file>
# generate a CoolHist file GUID and store it in the fileGUID object in the ROOT
# file
# also leave the guid in a file coolhist_guid.tmp so can be picked up by tools
# Richard Hawkings 30/1/07

# one parameter - ROOT file name
if [ $# -ne 1 ] 
then
  echo 'Syntax: coolHist_setFileIdentifier.sh <file>'
  exit
fi
RFILE=$1
# generate a fresh GUID
GUID=`uuidgen|awk '{print toupper($0)}'`
echo Generated GUID is $GUID
# create a temporary file with the ROOT commands
MYFILE=/tmp/coolHist_setFileIdentifier_$$.C
cat > $MYFILE <<EOF
void coolHist_setFileIdentifier_$$(const char* file, const char* guid) {
  cout << "Record GUID " << guid << " in file " << file << endl;
  TFile* myfile=new TFile(file,"UPDATE");
  if (myfile!=0) {
    // check that GUID does not already exist
    TObjString* oguid;
    myfile->GetObject("fileGUID",oguid);
    if (oguid!=0) {
      cout << "ERROR: File already has GUID assigned: " << oguid->GetString() << endl;
    } else {
      oguid=new TObjString(guid);
      myfile->WriteObject(oguid,"fileGUID");
      myfile->ls();
      myfile->Close();
    }
  } else {
    cout << "Problem opening file " << file << endl;
  }
}
EOF
root -b -q ${MYFILE}\(\"${RFILE}\",\"${GUID}\"\)
rm $MYFILE
exit

