/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <egammaMVACalib/egammaMVALayerDepth.h>

#include <cmath>


std::array<float, 4> get_MVAradius(float eta)
{
  eta = std::abs(eta);
  std::array<float, 4> radius;
  if (eta / 0.025 < 57)
  {
    static const float
      X0IN = 1.6;
    static const float
      // 1.7 X0 before strips
      X0S1 = 6.0;
    static const float
      //  6  X0 till end of strips
      //            BEND1 = 152.1000,    // first 3 bends radii (cm)
      //            BEND2 = 155.9659,
      //            BEND3 = 159.7202,
      ETASTEP = 0.8;
    static const float
      // where lead changes
      //            DENSHI = 2.137,      // cm/X0 high density
      //            DENSLO = 2.695,      // cm/X0  low density
      X0THI = 23.74;
    static const float
      // total depth high density part (not indep?)
      X0TLO = 19.177;     // total depth  low density part (not indep?)

    float X0T;
    //  float DENS, X0T, RLO, RHI;
    float R12;
    float R23;
    float R0;
    float R1;
    float R2;
    float R3;

    if (eta < ETASTEP) {
      //            DENS = DENSHI;
      X0T = X0THI;
      //            RLO = BEND2;            // break strips betw bends 2 and 3
      //            RHI = BEND3;
    }
    else {
      //            DENS = DENSLO;
      X0T = X0TLO;
      //            RLO = BEND1;            // break strips betw bends 1 and 2
      //            RHI = BEND2;
    }

    R0 = (X0IN) * std::cosh(eta);

    //..   actual radius where S1 should end

    R12 = X0S1;

    //..   total depth of calo at given eta

    float RADLTOT = X0T*std::cosh(eta);


    if (eta < 0.5) {
      R23 = 22 * std::cosh(eta);
    } else {
      R23 = 25;
    }

    R1 = R0  + (R12 - R0)/2;
    R2 = R12 + (R23 - R12)/2;
    R3 = R23 + (RADLTOT - R23)/2;

    radius[0] = R0;
    radius[1] = R1;
    radius[2] = R2;
    radius[3] = R3;
  }
  else
  {
    double x0=0.;
    double x1=0.;
    double x2=0.;
    double x3=0.;

    float abs_ETA = std::abs(eta);

    // TODO: find a better solution from cell 60, 61 (first two)
    if (abs_ETA>1.500 && abs_ETA<=1.525) {x0=0.; x1=8.85762; x2=19.5168; x3=30.0369;}       // COPIED FROM 62 !!!
    else if (abs_ETA>1.525 && abs_ETA<=1.550) {x0=0.; x1=8.85762; x2=19.5168; x3=30.0369;}  // COPIED FROM 62 !!!
    else if (abs_ETA>1.550 && abs_ETA<=1.575) {x0=0.; x1=8.85762; x2=19.5168; x3=30.0369;}
    else if (abs_ETA>1.575 && abs_ETA<=1.600) {x0=0.; x1=7.24613; x2=17.9683; x3=28.5757;}
    else if (abs_ETA>1.600 && abs_ETA<=1.625) {x0=0.; x1=7.11249; x2=17.9466; x3=28.6607;}
    else if (abs_ETA>1.625 && abs_ETA<=1.650) {x0=0.; x1=7.03737; x2=17.7263; x3=28.4004;}
    else if (abs_ETA>1.650 && abs_ETA<=1.675) {x0=0.; x1=7.44517; x2=18.1864; x3=28.9547;}
    else if (abs_ETA>1.675 && abs_ETA<=1.700) {x0=0.; x1=7.74611; x2=18.5419; x3=29.3555;}
    else if (abs_ETA>1.700 && abs_ETA<=1.725) {x0=0.; x1=8.06184; x2=18.8982; x3=29.8214;}
    else if (abs_ETA>1.725 && abs_ETA<=1.750) {x0=0.; x1=6.90503; x2=17.8396; x3=28.7947;}
    else if (abs_ETA>1.750 && abs_ETA<=1.775) {x0=0.; x1=6.53893; x2=17.5996; x3=28.7122;}
    else if (abs_ETA>1.775 && abs_ETA<=1.800) {x0=0.; x1=6.42897; x2=17.1062; x3=28.3578;}
    else if (abs_ETA>1.800 && abs_ETA<=1.825) {x0=0.; x1=6.41139; x2=17.1382; x3=28.4191;}
    else if (abs_ETA>1.825 && abs_ETA<=1.850) {x0=0.; x1=5.72471; x2=16.4543; x3=27.8796;}
    else if (abs_ETA>1.850 && abs_ETA<=1.875) {x0=0.; x1=5.19018; x2=16.1606; x3=27.7097;}
    else if (abs_ETA>1.875 && abs_ETA<=1.900) {x0=0.; x1=5.04008; x2=16.1174; x3=27.7649;}
    else if (abs_ETA>1.900 && abs_ETA<=1.925) {x0=0.; x1=5.01225; x2=16.184;  x3=27.9175;}
    else if (abs_ETA>1.925 && abs_ETA<=1.950) {x0=0.; x1=4.95996; x2=16.2482; x3=28.1358;}
    else if (abs_ETA>1.950 && abs_ETA<=1.975) {x0=0.; x1=4.98242; x2=16.3591; x3=28.3336;}
    else if (abs_ETA>1.975 && abs_ETA<=2.000) {x0=0.; x1=5.04856; x2=16.4194; x3=28.5289;}
    else if (abs_ETA>2.000 && abs_ETA<=2.025) {x0=0.; x1=5.32584; x2=16.8194; x3=29.0619;}
    else if (abs_ETA>2.025 && abs_ETA<=2.050) {x0=0.; x1=5.2971;  x2=16.8203; x3=29.1545;}
    else if (abs_ETA>2.050 && abs_ETA<=2.075) {x0=0.; x1=5.41049; x2=17.0891; x3=29.5856;}
    else if (abs_ETA>2.075 && abs_ETA<=2.100) {x0=0.; x1=5.48954; x2=17.2965; x3=29.9571;}
    else if (abs_ETA>2.100 && abs_ETA<=2.125) {x0=0.; x1=5.50876; x2=16.8573; x3=29.6199;}
    else if (abs_ETA>2.125 && abs_ETA<=2.150) {x0=0.; x1=5.45123; x2=16.885;  x3=29.8321;}
    else if (abs_ETA>2.150 && abs_ETA<=2.175) {x0=0.; x1=5.54906; x2=17.1228; x3=30.2044;}
    else if (abs_ETA>2.175 && abs_ETA<=2.200) {x0=0.; x1=5.53662; x2=17.1833; x3=30.4852;}
    else if (abs_ETA>2.200 && abs_ETA<=2.225) {x0=0.; x1=5.46801; x2=17.1857; x3=30.5842;}
    else if (abs_ETA>2.225 && abs_ETA<=2.250) {x0=0.; x1=5.45581; x2=16.6621; x3=30.2958;}
    else if (abs_ETA>2.250 && abs_ETA<=2.275) {x0=0.; x1=5.46682; x2=16.7899; x3=30.5222;}
    else if (abs_ETA>2.275 && abs_ETA<=2.300) {x0=0.; x1=5.52693; x2=16.9916; x3=30.9557;}
    else if (abs_ETA>2.300 && abs_ETA<=2.325) {x0=0.; x1=5.53211; x2=17.0801; x3=31.1637;}
    else if (abs_ETA>2.325 && abs_ETA<=2.350) {x0=0.; x1=4.79919; x2=16.4387; x3=31.5318;}
    else if (abs_ETA>2.350 && abs_ETA<=2.375) {x0=0.; x1=4.80101; x2=16.5808; x3=31.8533;}
    else if (abs_ETA>2.375 && abs_ETA<=2.400) {x0=0.; x1=4.77726; x2=15.9355; x3=31.4454;}
    else if (abs_ETA>2.400 && abs_ETA<=2.425) {x0=0.; x1=4.76845; x2=16.0665; x3=31.8002;}
    else if (abs_ETA>2.425 && abs_ETA<=2.450) {x0=0.; x1=4.5959;  x2=15.9977; x3=32.1175;}
    else if (abs_ETA>2.450 && abs_ETA<=2.475) {x0=0.; x1=4.68513; x2=16.2385; x3=32.5042;}
    else if (abs_ETA>2.475 && abs_ETA<=2.500) {x0=0.; x1=4.7232;  x2=16.4144; x3=33.028;}

    radius[0] = x0;
    radius[1] = x1;
    radius[2] = x2;
    radius[3] = x3;
  }
  return radius;
}
