// Victoria Rojo, SUNY Albany, June 2006
//
// The purpose of this code is to allow us to look
// at the number of hits in the large (non-ovelapping), 
// small (non-overlaping), and overlap sectors of the 
// barrel.
//

Int_t MooPhiSector(Double_t phi)
{
 
  // The phi angle of each hit in radians in inputted.
  // In MooHists.C this function is called within a loop
  // that requires all entering muons to have an |eta|<1
  // i.e. that it is in the barrel. It is then decided 
  // into which the following three regions the hit falls:
  //      Region 1 = Large Sector (non-overlapping)
  //      Region 2 = Small Sector (non-overlapping)
  //      Region 3 = Overlap region

  Int_t region = 0;

  // Define phi extent (in degrees) of the 
  // large (non-overlapping), small (non-overlaping),
  // and overlap sectors.

  Double_t phiLrg = 23.7;
  Double_t phiSml = 14.1;
  Double_t phiOL = 3.6; 
  
  // Shift origin so that 0 deg coincides
  // with the beginning of the first overlap
  // region; Then, convert radians to degrees.

  Double_t phiDegrees = phi*57.2957;

  if(phiDegrees > 0.){
    phiDegrees = phiDegrees - phiLrg/2;
  }
  if(phiDegrees < 0.){
    phiDegrees = phiDegrees + 360 - phiLrg/2;
  } 

  //printf(" \n");
  //printf(" \n");
  //printf("phi =  %f \n",phi);
  //printf("phiDegrees =  %f \n",phiDegrees);

  // Define phi position of sectors

  Double_t LrgLow [] = {500,500,500,500,500,500,500,500};
  Double_t LrgUp [] = {500,500,500,500,500,500,500,500};
  Double_t SmlLow [] = {500,500,500,500,500,500,500,500};
  Double_t SmlUp [] = {500,500,500,500,500,500,500,500};
 
  for( Int_t i = 0; i < 8; i++)
    {
      LrgLow [i] = (phiSml + 2*phiOL)*(i+1) + phiLrg*(i);
      LrgUp [i] = (phiSml + 2*phiOL)*(i+1) + phiLrg*(i+1);
      SmlLow [i] = phiOL + phiSml*(i) + (phiLrg + 2*phiOL)*(i);
      SmlUp [i] = phiOL + phiSml*(i+1) + (phiLrg + 2*phiOL)*(i);
     
      // printf(" \n");
      // printf(" \n");
      // printf("Large Low %d = %f \n",i,LrgLow[i]);
      // printf("Large Hi %d = %f \n",i,LrgUp[i]);
      // printf("Small Low %d = %f \n",i,SmlLow[i]);
      // printf("Small Hi %d = %f \n",i,SmlUp[i]);
    
    }
 
  // We are only concerned with hits in the barrel
  // This is a little redundant, but this will alow 
  // the code to be called in other places in MooHists.C

       // Large sectors
       for( Int_t j = 0; j < 8; j++)
	 {
	   if(phiDegrees > LrgLow[j] && phiDegrees < LrgUp[j])
	     {
	     region = 1;	  
	     }
	 }

       // Small sectors
       for( Int_t k = 0; k < 8; k++)
	 {
	   if(phiDegrees > SmlLow[k] && phiDegrees < SmlUp[k])
	     {
	       region = 2;
	     }
	 }
     

       // Overlaps

       if(region != 1 && region != 2) 
	   {
	     region = 3;
	   }

       // printf("region = %d \n",region);
      
  return region;
} 
