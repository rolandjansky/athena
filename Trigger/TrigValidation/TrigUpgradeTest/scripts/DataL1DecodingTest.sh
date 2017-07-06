#!/usr/bin/env sh

get_files -xmls LVL1config_Physics_pp_v5.xml
ln -s /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/atn-test/data15_13TeV.00266904.physics_EnhancedBias.merge.RAW._lb0452._SFO-1._0001.1 input.data

athena.py --threads=1 L1Decoder/decodeBS.py

cat <<EOF > hasStat.C
void hasStat( const char* hname) {

  TH1* h = (TH1*)_file0->Get( hname );
  if ( h == 0 ) {
     cout << "Missing histogram " << hname << "\n";
     exit (EXIT_FAILURE);
  }
  Long_t entries = h->GetEntries();
  if ( entries == 0 ) {
     cout << "Histogram " << hname << " has ) entries \n";
     exit (EXIT_FAILURE);
  }
  cout << "Histogram " << hname << " has " << entries << " entries \n";
}
EOF


root -b -q expert-monitoring.root 'hasStat.C("L1Decoder/TAVItems")'
root -b -q expert-monitoring.root 'hasStat.C("L1Decoder/Chains")'
