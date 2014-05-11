/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #ifndef TrkVertexWeightCalculator_FuncMeanRMS_H
 #define TrkVertexWeightCalculator_FuncMeanRMS_H

double Norm_st1Mom(double x)
{
  double Mean1= 0.456645 ;
  double  RMS1=1.21201 ;
  return (x-Mean1)/RMS1;
}

double Back_st1Mom(double x)
{
  double Mean1= 0.456645  ;
  double  RMS1= 1.21201  ;
  return (x*RMS1)+ Mean1;
}

double Norm_st2Mom(double x)
{
  
  double Mean2= 0.0806735  ;
  double  RMS2= 5.64695  ;
  return (x-Mean2)/RMS2;
}


double Back_st2Mom(double x)
{
  
  double Mean2= 0.0806735    ;
  double  RMS2= 5.64695  ;
  return (x*RMS2)+ Mean2;
}

double Norm_st3Mom(double x)
{ 
 double Mean3=-0.111183 ;
  double  RMS3=11.0546 ;
  return (x-Mean3)/RMS3;
}


double Back_st3Mom(double x)
{ 
 double Mean3= -0.111183 ;
  double  RMS3= 11.0546  ;
  return (x*RMS3)+ Mean3;
}

double Norm_st4Mom(double x)
{
  double Mean4=1.9536  ;
  double  RMS4= 12.5549 ;
  return (x-Mean4)/RMS4;
} 


double Back_st4Mom(double x)
{
  double Mean4= 1.9536 ;
  double  RMS4= 12.5549 ;
  return (x*RMS4)+ Mean4;
} 

double Norm_st5Mom(double x)
{
 double Mean5=2.56582  ;
  double  RMS5= 16.713 ;
   return (x-Mean5)/RMS5;
}

double Back_st5Mom(double x)
{
 double Mean5= 2.56582 ;
  double  RMS5=16.713  ;
   return (x*RMS5)+ Mean5;
}


double Norm_st6Mom(double x)
{
 double Mean6=4.29469 ;
  double  RMS6= 18.9983 ;
   return (x-Mean6)/RMS6;
}

double Back_st6Mom(double x)
{
 double Mean6= 4.29469 ;
  double  RMS6= 18.9983;
   return (x*RMS6)+ Mean6;
}
////HERE!!!!

double Norm_st7Mom(double x)
{
 double Mean7= 5.2504 ;
  double  RMS7= 22.4894 ;
   return (x-Mean7)/RMS7;
}

double Back_st7Mom(double x)
{
 double Mean7= 5.2504 ;
  double  RMS7= 22.4894  ;
   return (x*RMS7)+ Mean7;
}

double Norm_st8Mom(double x)
{
 double Mean8=6.87065 ;
  double  RMS8= 25.0507  ;
   return (x-Mean8)/RMS8;
}

double Back_st8Mom(double x)
{
 double Mean8= 6.87065 ;
  double  RMS8= 25.0507 ;
   return (x*RMS8)+ Mean8;
}

double Norm_st9Mom(double x)
{
 double Mean9= 7.98152 ;
  double  RMS9= 28.2111 ;
   return (x-Mean9)/RMS9;
}

double Back_st9Mom(double x)
{
 double Mean9=7.98152 ;
  double  RMS9= 28.2111 ;
   return (x*RMS9)+ Mean9;
}

double Norm_st10Mom(double x)
{
 double Mean10=9.53459 ;
  double  RMS10= 30.7888  ;           
  return (x-Mean10)/RMS10;
}

double Back_st10Mom(double x)
{
 double Mean10= 9.53459 ;
  double  RMS10= 30.7888 ;           
  return (x*RMS10)+ Mean10;
}

double Back_NTrks(int x)
{
 double Mean = (double)21.3617   ;
  double  RMS =  (double)23.5872 ;           
  return (x*RMS)+ Mean;
}

double Norm_NTrks(int x)
{
 double Mean = (double)21.3617 ;
  double  RMS =  (double)23.5872 ;           
  return (x - Mean)/RMS;
}

double Back_Sphere(double x)
{
 double Mean = 0.361716  ;
  double  RMS = 0.327354 ;           
  return (x*RMS)+ Mean;
}

double Norm_Sphere(double x)
{
 double Mean =0.361716  ;
  double  RMS =  0.327354  ;           
  return (x - Mean)/RMS;
}


#endif
