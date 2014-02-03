Int_t MooPhiBin(Double_t phi, Double_t eta)
{
  // Compute phi angle bin# taking the 8-fold symmetry of
  // the ATLAS muon spectrometer into account
  // *** the angle phi is expected to be given in radians      ***
  // *** but this function internally uses angles in degrees...***
  // This phi bin number is 1,2,3,4 and corresponds roughly to
  // bin 1: Large station region
  // bin 2: Small station region (mostly not overlapping barrel toroid)
  // bin 3: Small station region (overlapping barrel toroid)
  // bin 4: Small station region (mostly not overlapping barrel toroid)
  //        this last bin is at larger phi than bin 2
  // Phi bin number is meant to match Fig.5(left) of
  // note ATL-SOFT-2003-007, ATL-COM-MUON-2003-012 (Oct.2003)

  Int_t binNo = -99;
  Double_t phiDegrees = phi*57.29578;
  Double_t phiOffset  = 59.0;
  Double_t minPhiSector = -14.0;

  // For tracks mostly going through the endcap toroids (|eta| > 1.5)
  // one needs to account for the 22.5 degree rotation of the endcap
  // toroids relative to the barrel toroids;
  // this is accounted for by removing 22.5 degrees from the input phi value
  if (fabs(eta) > 1.5) {
    phiDegrees -= 22.5;
  }

  // make sure phi is positive --TBM
  while ( phiDegrees < 0 ) {
    phiDegrees += 360;
  }

  // Take care of the fact that the first sector extends to -14.0 degrees
  // to obtain phi angles between -14.0 and +346.0 degrees
  if (phiDegrees >= 360.0 + minPhiSector){
    phiDegrees -= 360.0;
  }

  // Phi sector numbers are 1,2,...,8
  Double_t phiSector = Int_t(phiDegrees+phiOffset)/45;

  // Compute "reduced" phi angle, i.e. angle phi minus the required
  // integer multiple (should be the phiSector number) to obtain
  // an angle in the range between minPhiSector and maxPhiSector

  Double_t phiReduced = phiDegrees - (phiSector-1)*45.0;

  // Assign phi bin number according to phiReduced value
  // to obtain a bin number that is supposed to match Fig.5(left) of
  // note ATL-SOFT-2003-007, ATL-COM-MUON-2003-012 (Oct.2003)

  if (phiReduced >= -14 && phiReduced < 14.0) { binNo = 1;} // region "A"
  else if (phiReduced < 19.2) { binNo = 2;} // region "B1"
  else if (phiReduced < 25.8) { binNo = 3;} // region "C"
  else if (phiReduced < 31.0) { binNo = 4;} // region "B2"
  else { printf(" Problems with phiReduced: %f out of bounds",phiReduced);}

  return binNo;
} 
