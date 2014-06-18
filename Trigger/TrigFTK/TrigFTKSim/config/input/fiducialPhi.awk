#!/usr/bin/awk -f 

# Selects single muon events with at least one truth tracks with (phimin < phi < phimax)

# Usage: fiducialPhi.awk -v phimin=-0.6 -v phimax=-0.2 wrapper_muon_file{90..99}.dat
# NOTE: phi should be in the range -Pi..Pi

BEGIN {
  if(phimin=="") phimin=-0.6;
  if(phimax=="") phimax=-0.2;
  split("",data);
  i=0;
  passed=0;
}

{
  data[i++]=$0;
}

$1=="T" {
  phi=atan2($7,$6);
  if(phi>=phimin && phi<=phimax) passed=1;
}

$1=="L" {
  for(z=0;z<i && passed==1;z++) {
    print data[z];
  }
  split("",data);
  i=0;
  passed=0;
}
