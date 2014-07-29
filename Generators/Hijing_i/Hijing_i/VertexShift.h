/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

// ---------------------------------------------------------------
// Vertex shifting numbers to make beam gas events.
//
// AuthorList:
//     Alden Stradling: Initial addition to Hijing.cxx Feb 2007


// Default Ranges - Vertex Dispersion (radius, for X and Y, and 1/2 length in Z - symmetric)
// Beam Gas - 1st approximation. In reality, there are variations in the beampipe diameter: 
// (LHC Proj. Report 783)
// 1: z = 0 -> 10200 mm : 58 mm diameter (ID/LAr)
// 2: z = 10200 -> 14300 mm : 80 mm diameter (Toroid/Shielding)
// 3: z = 14300 -> 18900 mm : 120 mm diameter (Shielding section)
// 4: z 18900 -> 23000 mm : 34 mm diameter (TAS Copper Absorber) Perhaps not still copper - Fe?


namespace VertexShift {

  const float Xmin = 17.; //CLHEP::mm
  const float Zmax = 23000.; //CLHEP::mm
  
  // How much wider than the thinnest section is this segment?
  const float Delta1 = 12.; //CLHEP::mm
  const float Delta2 = Delta1 + 11.; //CLHEP::mm
  const float Delta3 = Delta2 + 20.; //CLHEP::mm
  const float Delta4 = 0.; //CLHEP::mm
  
  //What's the segment starting point?
  const float Start0 = 0.; //CLHEP::mm
  const float Start1 = 10200.; //CLHEP::mm
  const float Start2 = 14300.; //CLHEP::mm
  const float Start3 = 18900.; //CLHEP::mm
  const float Envelope = Zmax; //CLHEP::mm

}
