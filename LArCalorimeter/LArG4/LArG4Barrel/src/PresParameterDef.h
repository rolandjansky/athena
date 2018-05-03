/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** default units : mm , deg. */

 G4double m_cmm{(1-0.0026)*Athena::Units::mm}; // contraction factor

 /** Electrodes */
 G4double m_smallLength{275.6};
 G4double m_bigLength{277.5};
 G4double m_anode_th{0.330};
 G4double m_cathode_th{0.270};
 G4double m_larheight{13.};

 /** modules para [Length,NAnodes,NCathodes,elec. inclination,Pitch,Pos0]*/
 // mod[#module#][ 0:length, 1:Number of Anodes, 2:number of cathodes,
 // 3:Tilt of cells, 4:Pitch, 5:Position of the first cathode ]
 G4double m_mod[8][6] {
               {286.4,   56,  56, -25., 4.987, 4.2},
               {295.74,  64,  64, -12., 4.621, 0.3},
               {321.1,   72,  72,   0., 4.46,  0.9},
               {356.8,   80,  80,   0., 4.46,  0.9},
               {404.8,   88,  88,   0., 4.6,   0.9},
               {478.4,  104, 104,   0., 4.6,   0.95},
               {563.2,  128, 128,   0., 4.4,   1.05},
               {380.6,   86,  87,   0., 4.4,   0.95}
 };

 /** Prep., MB, ..... */
 G4double m_prep1_th{1.};  // bottom prepreg layer
 G4double m_prep2_th{4.5}; // top prepreg layer
 G4double m_shell_th{0.4}; // Protection Shell Thickness
 G4double m_prot_th{0.5};  // Top protection Shell Thickness

 G4double  m_mech_clear{0.5}; // Bottom Mechanical clearence
 G4double m_rail_th{8.6}; // Support bar Thickness
 G4double m_rail_pos{22.}; // Lateral position from the edge
 G4double m_rail_width{24.}; // Support bar width

 G4double m_mb_th{2.2}; // Mother Board Thickness
 G4double m_mb_width{169.}; // Mother Board Width
 G4double m_mb_length{3100.3}; // Mother Board Length

 /** connectics */
 G4double m_widthFront{2.8}; // width at eta = 0. , width at etamax = MB_width
 G4double m_heightIn{1.5}; // Height at eta = 0.
 G4double m_heightOut{5.}; // Height at etamax

 /** Presampler Mother Volume parameters */
 G4double m_rMinPresamplerMother{1385*Athena::Units::mm};
 G4double m_rMaxPresamplerMother{1447*Athena::Units::mm - 0.001*Athena::Units::mm}; // GU safety
 G4double m_PresamplerMother_length{1549.0*Athena::Units::mm}; // reduced by 1mm to avoid clash - GU 5-Oct-2005

 /** GU add phimin and phi span as parameters */
 G4double m_Phi_min{0.*Athena::Units::deg};
 G4double m_Phi_span{360.*Athena::Units::deg};

 /** GU add phi min and phi span of overall volume */
 G4int m_nsectors{32};  // number of sectors / half barrel
 G4int m_nbsectors{32}; // number of sectors to put in simulation
