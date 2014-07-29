/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia_i/Pythia.h"

/*
  ________________________________________________________________________________________________________________________

  L.M., 
  Pythia parameter settings / TUNE

  Aug. 09

  called by Pythia.cxx
  set ATLAS tune/parameters set determined by m_Tune_Index
  m_Tune_Index 0,-1 and -2 are technical settings to run with 0, recommended or common set of ATLAS Pythia parameters
  in case m_Tune_Index does not correspond to ATLAS tune index use default ATLAS tune (set by m_Default_Tune_Name)
  -------------------------------------------------------------------------------------------------------------------------
  Backw. compat: old tune setting syntax remains supported.
  Mixing of the old and new tune settings is not supported and in this case Atlas default tune settings are used and 

  May 09

  In case only the old syntax is used, the switches have the following effect:
  1. useAtlasPythiaRecomm  : false sets m_Tune_Index=0 [default:true ]
     Event record, error handlig, new PS/MI model, MI structure
  2. useAtlasPythiaCommon  : false sets m_Tune_Index=-1; [default:true ]
     (ATLAS specific) print-out, particle properties, MI structure, pdf
  3. useAtlasPythiaTune09  : sets m_Tune_Index=m_mc09_Tune_Index        [default:false ]
  4. useAtlasPythiaTune08  : sets m_Tune_Index=m_mc08_Tune_Index        [default:false ]
  5. useNoAtlasPythiaParam : sets bool m_useNoAtlasPythiaParam          [default:false ]
     Using Pythia defaults + parameters set by the jO only.
     Please note that the use of ATLAS defaults is recommended.
     Please consider keeping the settings of switch 1.

  ________________________________________________________________________________________________________________________
 */

StatusCode Pythia::atlasTune()

{
  // if tune is left to the dummy starting value, use the default tune:
  if (m_Tune_Index==m_Starting_Tune_Index) m_Tune_Index=m_Default_Tune_Index;

  ATH_MSG_INFO( "\n________________________________________________________________________________\n"
		<< "atlasTune:   [Pythia_i/src/atlasTune.cxx]                                       \n" 
		<< "atlasTune: setting Pythia parameter values:                                     \n"
		<< "--------------------------------------------------------------------------------"  ); 

  // note: m_Tune_Name == "ATLAS_blah" will end here since atoi() returns 0 if it can't ocnvert arg. to int 
  if (m_Tune_Index==0) 
    {
      ATH_MSG_WARNING( "setPythiaTune: m_Tune_Index=0 requested,                                       \n"
		       << "setPythiaTune: skipping par. setting by Pythia_i, running with Pythia defaults. \n"
		       << "setPythiaTune: Using Pythia defaults + parameters set by the USER.              \n"
		       << "setPythiaTune: Please note that the use of ATLAS defaults is recommended.       \n"
		       << "setPythiaTune: Please consider keeping the settings of Tune_Name=-1,-2.         \n"
		       << "--------------------------------------------------------------------------------"  );
      return StatusCode::SUCCESS; 
    }//end of {m_Tune_Index==0} condition
  //---------------------------------------------------------------------------  
  //____________________________________________________________________________________________________________________________
  // when not using an ATLAS tune but want to keep recommended / common settings: 
  else if (m_Tune_Index<0)
    {
      if (m_Tune_Index==-1)
	{
	  //________________________________________________________________________________________________________________________
	  // printout details + 
	  this->pypars().mstp(128)  =1;      // fix junk output for documentary particles 	 
	  this->pydat1().mstu(21)   =1;      // error handling switch 
	  this->pypars().mstp(81)   =21;     // run PYEVNW with PYEVNT 
	  this->pydat1().mstj(22)=2;         // stable particles convention
	  //________________________________________________________________________________________________________________________
	  ATH_MSG_INFO( "mstp(128)  =  "<< this->pypars().mstp(128) << "\n" 
			<< "mstu(21)   =  "<< this->pydat1().mstu(21)  << "\n" 
			<< "mstp(81)   =  "<< this->pypars().mstp(81)  << "\n" 
			<< "mstj(22)   =  "<< this->pydat1().mstj(22) );

	  //________________________________________________________________________________________________________________________ 
        }
      if (m_Tune_Index==-2)
	{
	  //________________________________________________________________________________________________________________________
	  // printout details 
	  this->pypars().mstp(128)  =1;      // fix junk output for documentary particles 	 
	  this->pydat1().mstu(21)   =1;      // error handling switch 
	  this->pypars().mstp(81)   =21;     // run PYEVNW with PYEVNT 
	  this->pydat1().mstj(22)=2;         // stable particles convention
	  //________________________________________________________________________________________________________________________
   	  //physics-related
	  this->pypars().mstp(82)=4;         // MI structure
	  this->pypars().mstp(52)=2;         // (needed for MRST2007lomod (LO* for MC))	 
	  this->pypars().mstp(54)=2;         // (needed for MRST2007lomod (LO* for MC))	 	 
	  this->pypars().mstp(56)=2;         // (needed for MRST2007lomod (LO* for MC))	 	 
	  this->pypars().mstp(51)=20651;     // LO**
	  this->pypars().mstp(53)=20651;     // LO** 	 	 
	  this->pypars().mstp(55)=20651;     // LO** 	
	  this->pydat2().pmas(6,1)=172.5;    // PDG2007 TOP mass
	  this->pydat2().pmas(24,1)=80.399;  // PDG2007 W mass
	  //this->pydat2().pmas(24,2)=2.141;  W-width is calculated dynamically
	  this->pydat2().pmas(23,1)=91.1876; // PDG2007 Z0 mass
	  //this->pydat2().pmas(23,2)=2.4952;  // PDG2007 Z0 width
	  //________________________________________________________________________________________________________________________
	  //printout details ... +
	  ATH_MSG_INFO( "mstp(128)  =  "<< this->pypars().mstp(128) << "\n" 
			<< "mstu(21)   =  "<< this->pydat1().mstu(21)  << "\n" 
			<< "mstp(81)   =  "<< this->pypars().mstp(81)  << "\n" 
			//physics-related
			<< "mstp(82)  = "<<  this->pypars().mstp(82)     << "\n"    
			<< "mstp(52)  = "<<  this->pypars().mstp(52)     << "\n"    
			<< "mstp(54)  = "<<  this->pypars().mstp(54)     << "\n"    
			<< "mstp(56)  = "<<  this->pypars().mstp(56)     << "\n"    
			<< "mstp(51)  = "<<  this->pypars().mstp(51)     << "\n"
			<< "mstp(53)  = "<<  this->pypars().mstp(53)     << "\n"
			<< "mstp(55)  = "<<  this->pypars().mstp(55)     << "\n"
			<< "pmas(6,1) = "<<  this->pydat2().pmas(6,1)    << "\n"  
			<< "pmas(24,1)= "<<  this->pydat2().pmas(24,1)   << "\n"
			//<< "pmas(24,2)= "<<  this->pydat2().pmas(24,2)   << "\n" 
			<< "pmas(23,1)= "<<  this->pydat2().pmas(23,1)   << "\n" );             
	  //<< "pmas(23,2)= "<<  this->pydat2().pmas(23,2)   << "\n" 
	  //________________________________________________________________________________________________________________________
	}
    }
  else if (m_Tune_Index==20090001)
    {
      // ATLAS jargon : MC09
      // author : Markus Warsinsky
      // Pythia : 6.4.20 
      // date: May 2009
      // short description: MC09 production round tune for LO* PDFs
      //________________________________________________________________________________________________________________________
      this->pypars().mstp(128)  =1;      // fix junk output for documentary particles 	 
      this->pydat1().mstu(21)   =1;      // error handling switch 
      //physics - related
      this->pypars().mstp(81)=21;        // run PYEVNW with PYEVNT 
      this->pypars().mstp(82)=4;         // MI structure           
      this->pypars().mstp(52)=2;         // (needed for MRST2007lomod (LO* for MC))    
      this->pypars().mstp(54)=2;         // (needed for MRST2007lomod (LO* for MC))    
      this->pypars().mstp(56)=2;         // (needed for MRST2007lomod (LO* for MC))     
      this->pypars().mstp(51)=20650;     // (needed for MRST2007lomod (LO* for MC)) 	 
      this->pypars().mstp(53)=20650;     // (needed for MRST2007lomod (LO* for MC)) 	 
      this->pypars().mstp(55)=20650;     // (needed for MRST2007lomod (LO* for MC))
      this->pydat2().pmas(6,1)=172.5;    // PDG2007 TOP mass
      this->pydat2().pmas(24,1)=80.399;  // PDG2007 W mass
      //this->pydat2().pmas(24,2)=2.141;  W-width is calculated dynamically
      this->pydat2().pmas(23,1)=91.1876; // PDG2007 Z0 mass
      //this->pydat2().pmas(23,2)=2.4952;  // Z-width is calculated dynamically
      this->pypars().mstp(70)=0;         // (virtuality scale for ISR)
      this->pypars().mstp(72)=1;
      this->pypars().mstp(88)=1;
      this->pypars().mstp(90)=0;
      this->pypars().parp(78)=0.3;
      this->pypars().parp(80)=0.1;
      this->pypars().parp(82)=2.3;       // (cut off scale)
      this->pypars().parp(83)=0.8;
      this->pypars().parp(84)=0.7;       // (matter distribution)
      this->pypars().parp(90)=0.25;      // (cut off scale)
      this->pydat1().parj(81)=0.29;
      this->pypars().mstp(95)=6; 
      this->pydat1().mstj(11)=4;         // select Lund-Bowler for c && b fragmentation
      this->pydat1().mstj(22)=2;         // (Make K_S, Lambda stable)
      this->pydat1().parj(41)=0.3;       // Lund-Bowler a
      this->pydat1().parj(42)=0.58;      // Lund-Bowler b
      this->pydat1().parj(46)=0.75;      // Lund-Bowler rQ
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(89)=1;
      this->pypars().parp(89)=1800.;
      //________________________________________________________________________________________________________________________
      ATH_MSG_INFO( "--------------------------------------------------------------------------------\n" 
		    << "atlasTune: parameter setting to Tune_Index "<<m_Tune_Index<<" tune values :       \n" 
 		    << "mstp(128) = "<< this->pypars().mstp(128)    << "\n" 
		    << "mstu(21)  = "<< this->pydat1().mstu(21)     << "\n"
		    << "mstp(81) = "<<   this->pypars().mstp(81)     << "\n"       
		    << "mstp(82) = "<<   this->pypars().mstp(82)     << "\n"        
		    << "mstp(52) = "<<   this->pypars().mstp(52)     << "\n"        
		    << "mstp(54) = "<<   this->pypars().mstp(54)     << "\n"        
		    << "mstp(56) = "<<   this->pypars().mstp(56)     << "\n"        
		    << "mstp(51) = "<<   this->pypars().mstp(51)     << "\n"    
		    << "mstp(53) = "<<   this->pypars().mstp(53)     << "\n"    
		    << "mstp(55) = "<<   this->pypars().mstp(55)     << "\n"    
		    << "pmas(6,1) = "<<  this->pydat2().pmas(6,1)    << "\n"   
		    << "pmas(24,1) = "<< this->pydat2().pmas(24,1)   << "\n" 
		    //<< "pmas(24,2) = "<< this->pydat2().pmas(24,2)   << "\n"  
		    << "pmas(23,1) = "<< this->pydat2().pmas(23,1)   << "\n"
		    //<< "pmas(23,2) = "<< this->pydat2().pmas(23,2)   << "\n" 
		    << "mstp(70) = "<<   this->pypars().mstp(70)     << "\n"        
		    << "mstp(72) = "<<   this->pypars().mstp(72)     << "\n"
		    << "mstp(88) = "<<   this->pypars().mstp(88)     << "\n"
		    << "mstp(90) = "<<   this->pypars().mstp(90)     << "\n"
		    << "parp(78) = "<<   this->pypars().parp(78)     << "\n"
		    << "parp(80) = "<<   this->pypars().parp(80)     << "\n"
		    << "parp(82) = "<<   this->pypars().parp(82)     << "\n"      
		    << "parp(83) = "<<   this->pypars().parp(83)     << "\n"
		    << "parp(84) = "<<   this->pypars().parp(84)     << "\n"      
		    << "parp(90) = "<<   this->pypars().parp(90)     << "\n"     
		    << "parj(81) = "<<   this->pydat1().parj(81)     << "\n"
		    << "mstp(95) = "<<   this->pypars().mstp(95)     << "\n" 
		    << "mstj(11) = "<<   this->pydat1().mstj(11)     << "\n"        
		    << "mstj(22) = "<<   this->pydat1().mstj(22)     << "\n"        
		    << "parj(41) = "<<   this->pydat1().parj(41)     << "\n"      
		    << "parj(42) = "<<   this->pydat1().parj(42)     << "\n"     
		    << "parj(46) = "<<   this->pydat1().parj(46)     << "\n"     
		    << "mstp(84) = "<<   this->pypars().mstp(84)     << "\n"
		    << "mstp(85) = "<<   this->pypars().mstp(85)     << "\n"
		    << "mstp(86) = "<<   this->pypars().mstp(86)     << "\n"
		    << "mstp(87) = "<<   this->pypars().mstp(87)     << "\n"
		    << "mstp(89) = "<<   this->pypars().mstp(89)     << "\n"
		    << "parp(89) = "<<   this->pypars().parp(89)     << "\n"
		    << "-----------------------------------------------                                 \n"
		    << "atlasTune: end of the list of Tune_Index "<<m_Tune_Index<<" tune values :         \n" 
		    << "--------------------------------------------------------------------------------\n" ); 
      //________________________________________________________________________________________________________________________
    }// end of if (m_Tune_Index==20090001) condition
  else if (m_Tune_Index==20080001)
    {
      // ATLAS jargon : MC08
      // author : Arthur Moraes
      // Pythia : ~ 6.4.18 
      // date: ~ 2008
      // short description: MC08 production round tune 
      //________________________________________________________________________________________________________________________
      this->pypars().mstp(128)  =1;      // fix junk output for documentary particles 	 
      this->pydat1().mstu(21)   =1;      // error handling switch 
      //physics - related
      this->pypars().mstp(81)=21;        // run PYEVNW with PYEVNT 
      this->pypars().mstp(82)=4;         // MI structure
      this->pypars().mstp(52)=2;         // (needed for CTEQ6L) 	 
      this->pypars().mstp(54)=2;         // (needed for CTEQ6L) 	 
      this->pypars().mstp(56)=2;         // (needed for CTEQ6L) 	 
      this->pypars().mstp(51)=10042;     // (needed for CTEQ6L) 	 
      this->pypars().mstp(53)=10042;     // (needed for CTEQ6L) 	 
      this->pypars().mstp(55)=10042;     // (needed for CTEQ6L)
      this->pydat2().pmas(6,1)=172.5;    // PDG2007 TOP mass
      this->pydat2().pmas(24,1)=80.399;  // PDG2007 W mass
      //this->pydat2().pmas(24,2)=2.141;  W-width is calculated dynamically
      this->pydat2().pmas(23,1)=91.1876; // PDG2007 Z0 mass
      //this->pydat2().pmas(23,2)=2.4952;  // Z-width is calculated dynamically
      this->pypars().mstp(70)=0;         // (virtuality scale for ISR)
      this->pypars().mstp(72)=1;
      this->pypars().mstp(88)=1;
      this->pypars().mstp(90)=0;
      this->pypars().parp(78)=0.3;
      this->pypars().parp(80)=0.1;
      this->pypars().parp(82)=2.1;    // (cut off scale)
      this->pypars().parp(83)=0.8;
      this->pypars().parp(84)=0.7;    // (matter distribution)
      this->pypars().parp(90)=0.16;   // (cut off scale)
      this->pydat1().parj(81)=0.29;
      this->pypars().mstp(95)=1; 
      this->pydat1().mstj(11)=3;      // (select peterson for charm fragmentation)
      this->pydat1().mstj(22)=2;      // (Make K_S, Lambda stable)
      this->pydat1().parj(54)=-0.07;  // (c hadronization)
      this->pydat1().parj(55)=-0.006; // (b hadronization)
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(89)=1;
      this->pypars().parp(89)=1800.;
      //________________________________________________________________________________________________________________________
      ATH_MSG_INFO( "--------------------------------------------------------------------------------\n" 
		    << "atlasTune: parameter setting to Tune_Index "<<m_Tune_Index<<" tune values :       \n" 
		    << "mstp(128)  = "<< this->pypars().mstp(128)    << "\n" 
		    << "mstu(21)   = "<< this->pydat1().mstu(21)    << "\n"
		    << "mstp(81)   = "<< this->pypars().mstp(81)  << "\n" 
		    << "mstp(82)   = "<< this->pypars().mstp(82)   << "\n"    
		    << "mstp(52)   = "<< this->pypars().mstp(52)   << "\n"    
		    << "mstp(54)   = "<< this->pypars().mstp(54)   << "\n"    
		    << "mstp(56)   = "<< this->pypars().mstp(56)   << "\n"    
		    << "mstp(51)   = "<< this->pypars().mstp(51) << "\n"
		    << "mstp(53)   = "<< this->pypars().mstp(53) << "\n"
		    << "mstp(55)   = "<< this->pypars().mstp(55) << "\n"
		    << "pmas(6,1)  = "<< this->pydat2().pmas(6,1) << "\n"  
		    << "pmas(24,1) = "<< this->pydat2().pmas(24,1)<< "\n"
		    //<< "pmas(24,2) = "<< this->pydat2().pmas(24,2) << "\n" 
		    << "pmas(23,1) = "<< this->pydat2().pmas(23,1)<< "\n"
		    //<< "pmas(23,2) = "<< this->pydat2().pmas(23,2)<< "\n" 
		    << "mstp(70)   = "<< this->pypars().mstp(70)   << "\n"      
		    << "mstp(72)   = "<< this->pypars().mstp(72)   << "\n"
		    << "mstp(88)   = "<< this->pypars().mstp(88)   << "\n"
		    << "mstp(90)   = "<< this->pypars().mstp(90)   << "\n"
		    << "parp(78)   = "<< this->pypars().parp(78) << "\n"
		    << "parp(80)   = "<< this->pypars().parp(80) << "\n"
		    << "parp(82)   = "<< this->pypars().parp(82)  << "\n"    
		    << "parp(83)   = "<< this->pypars().parp(83) << "\n"
		    << "parp(84)   = "<< this->pypars().parp(84) << "\n"    
		    << "parp(90)   = "<< this->pypars().parp(90)<< "\n"   
		    << "parj(81)   = "<< this->pydat1().parj(81)<< "\n"
		    << "mstp(95)   = "<< this->pypars().mstp(95)   << "\n" 
		    << "mstj(11)   = "<< this->pydat1().mstj(11)   << "\n"      
		    << "mstj(22)   = "<< this->pydat1().mstj(22)   << "\n"      
		    << "parj(54)   = "<< this->pydat1().parj(54) << "\n"  
		    << "parj(55)   = "<< this->pydat1().parj(55) << "\n" 
		    << "mstp(84)   = "<< this->pypars().mstp(84)   << "\n"
		    << "mstp(85)   = "<< this->pypars().mstp(85)   << "\n"
		    << "mstp(86)   = "<< this->pypars().mstp(86)   << "\n"
		    << "mstp(87)   = "<< this->pypars().mstp(87)   << "\n"
		    << "mstp(89)   = "<< this->pypars().mstp(89)   << "\n"
		    << "parp(89)   = "<< this->pypars().parp(89)<< "\n"
		    << "-----------------------------------------------                                 \n"
		    << "atlasTune: end of the list of Tune_Index "<<m_Tune_Index<<" tune values :         \n" 
		    << "--------------------------------------------------------------------------------\n" ); 
      //________________________________________________________________________________________________________________________
    } // end of if (m_Tune_Index==20080001) condition
  else if (m_Tune_Index==20090002)
    {
      // ATLAS jargon : MC09' 
      // author :  Markus Warsinsky 
      // Pythia : 6.4.20
      // date: Jun. 2009
      // short description: MC08 tune update for the MC09 production 
      //________________________________________________________________________________________________________________________
      this->pypars().mstp(128)  =1;      // fix junk output for documentary particles 	 
      this->pydat1().mstu(21)   =1;      // error handling switch 
      //physics - related
      this->pypars().mstp(81)=21;        // run PYEVNW with PYEVNT 
      this->pypars().mstp(82)=4;         // MI structure           
      this->pypars().mstp(52)=2;         // (needed for ext. PDF)    
      this->pypars().mstp(54)=2;         // (needed for ext. PDF)    
      this->pypars().mstp(56)=2;         // (needed for ext. PDF)     
      this->pypars().mstp(51)=10042;     // (needed for CTEQ6L) this is changed for MC08'' (20090002) wrt MC09 20090001
      this->pypars().mstp(53)=10042;     // (needed for CTEQ6L) this is changed for MC08'' (20090002) wrt MC09 20090001
      this->pypars().mstp(55)=10042;     // (needed for CTEQ6L) this is changed for MC08'' (20090002) wrt MC09 20090001
      this->pydat2().pmas(6,1)=172.5;    // PDG2007 TOP mass
      this->pydat2().pmas(24,1)=80.399;  // PDG2007 W mass
      //this->pydat2().pmas(24,2)=2.141;  W-width is calculated dynamically
      this->pydat2().pmas(23,1)=91.1876; // PDG2007 Z0 mass
      //this->pydat2().pmas(23,2)=2.4952;  // Z-width is calculated dynamically
      this->pypars().mstp(70)=0;         // (virtuality scale for ISR)
      this->pypars().mstp(72)=1;
      this->pypars().mstp(88)=1;
      this->pypars().mstp(90)=0;
      this->pypars().parp(78)=0.3;
      this->pypars().parp(80)=0.1;
      this->pypars().parp(82)=2.1;       // this is changed for MC08'' (20090002) wrt MC09 20090001
      this->pypars().parp(83)=0.8;
      this->pypars().parp(84)=0.7;       // (matter distribution)
      this->pypars().parp(90)=0.25;      // (cut off scale)
      this->pydat1().parj(81)=0.29;
      this->pypars().mstp(95)=6; 
      this->pydat1().mstj(11)=4;         // select Lund-Bowler for c && b fragmentation
      this->pydat1().mstj(22)=2;         // (Make K_S, Lambda stable)
      this->pydat1().parj(41)=0.3;       // Lund-Bowler a
      this->pydat1().parj(42)=0.58;      // Lund-Bowler b
      this->pydat1().parj(46)=0.75;      // Lund-Bowler rQ
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(89)=1;
      this->pypars().parp(89)=1800.;
      //________________________________________________________________________________________________________________________
      ATH_MSG_INFO(   "--------------------------------------------------------------------------------\n" 
		     << "atlasTune: parameter setting to Tune_Index "<<m_Tune_Index<<" tune values :       \n" 
		     << "mstp(128) = "<< this->pypars().mstp(128)    << "\n" 
		     << "mstu(21)  = "<< this->pydat1().mstu(21)     << "\n"
		     << "mstp(81) = "<<   this->pypars().mstp(81)     << "\n"       
		     << "mstp(82) = "<<   this->pypars().mstp(82)     << "\n"        
		     << "mstp(52) = "<<   this->pypars().mstp(52)     << "\n"        
		     << "mstp(54) = "<<   this->pypars().mstp(54)     << "\n"        
		     << "mstp(56) = "<<   this->pypars().mstp(56)     << "\n"        
		     << "mstp(51) = "<<   this->pypars().mstp(51)     << "\n"    
		     << "mstp(53) = "<<   this->pypars().mstp(53)     << "\n"    
		     << "mstp(55) = "<<   this->pypars().mstp(55)     << "\n"    
		     << "pmas(6,1) = "<<  this->pydat2().pmas(6,1)    << "\n"   
		     << "pmas(24,1) = "<< this->pydat2().pmas(24,1)   << "\n" 
		     //<< "pmas(24,2) = "<< this->pydat2().pmas(24,2)   << "\n"  
		     << "pmas(23,1) = "<< this->pydat2().pmas(23,1)   << "\n"
		     //<< "pmas(23,2) = "<< this->pydat2().pmas(23,2)   << "\n" 
		     << "mstp(70) = "<<   this->pypars().mstp(70)     << "\n"        
		     << "mstp(72) = "<<   this->pypars().mstp(72)     << "\n"
		     << "mstp(88) = "<<   this->pypars().mstp(88)     << "\n"
		     << "mstp(90) = "<<   this->pypars().mstp(90)     << "\n"
		     << "parp(78) = "<<   this->pypars().parp(78)     << "\n"
		     << "parp(80) = "<<   this->pypars().parp(80)     << "\n"
		     << "parp(82) = "<<   this->pypars().parp(82)     << "\n"      
		     << "parp(83) = "<<   this->pypars().parp(83)     << "\n"
		     << "parp(84) = "<<   this->pypars().parp(84)     << "\n"      
		     << "parp(90) = "<<   this->pypars().parp(90)     << "\n"     
		     << "parj(81) = "<<   this->pydat1().parj(81)     << "\n"
		     << "mstp(95) = "<<   this->pypars().mstp(95)     << "\n" 
		     << "mstj(11) = "<<   this->pydat1().mstj(11)     << "\n"        
		     << "mstj(22) = "<<   this->pydat1().mstj(22)     << "\n"        
		     << "parj(41) = "<<   this->pydat1().parj(41)     << "\n"      
		     << "parj(42) = "<<   this->pydat1().parj(42)     << "\n"     
		     << "parj(46) = "<<   this->pydat1().parj(46)     << "\n"     
		     << "mstp(84) = "<<   this->pypars().mstp(84)     << "\n"
		     << "mstp(85) = "<<   this->pypars().mstp(85)     << "\n"
		     << "mstp(86) = "<<   this->pypars().mstp(86)     << "\n"
		     << "mstp(87) = "<<   this->pypars().mstp(87)     << "\n"
		     << "mstp(89) = "<<   this->pypars().mstp(89)     << "\n"
		     << "parp(89) = "<<   this->pypars().parp(89)     << "\n"
		     << "-----------------------------------------------                                 \n"
		     << "atlasTune: end of the list of Tune_Index "<<m_Tune_Index<<" tune values :         \n" 
		     << "--------------------------------------------------------------------------------\n" ); 
      //________________________________________________________________________________________________________________________
    } 
  else if (m_Tune_Index==20090003)
    {
      // ATLAS jargon     : MC09c
      // references       : ATL-COM-PHYS-2010-040, ATL-PHYS-PUB-2010-002
      // author           : Markus Warsinsky et. al
      // Pythia           : 6.4.21 
      // impl. date       : June  2010
      // tune produced in : 2009
      // short description: simmilar to MC09, but with lowered CR
      // changes wrt. MC09: 
      //         MC09c has: parp(78)=0.224,    parp(82)=2.315,    parp(90)=0.2487 
      //          MC09 has: parp(78)=0.3,      parp(82)=2.3,      parp(90)=0.25
      //________________________________________________________________________________________________________________________
      this->pypars().mstp(128)  =1;      // fix junk output for documentary particles 	 
      this->pydat1().mstu(21)   =1;      // error handling switch 
      //physics - related
      this->pypars().mstp(81)=21;        // run PYEVNW with PYEVNT 
      this->pypars().mstp(82)=4;         // MI structure           
      this->pypars().mstp(52)=2;         // (needed for MRST2007lomod (LO* for MC))    
      this->pypars().mstp(54)=2;         // (needed for MRST2007lomod (LO* for MC))    
      this->pypars().mstp(56)=2;         // (needed for MRST2007lomod (LO* for MC))     
      this->pypars().mstp(51)=20650;     // (needed for MRST2007lomod (LO* for MC)) 	 
      this->pypars().mstp(53)=20650;     // (needed for MRST2007lomod (LO* for MC)) 	 
      this->pypars().mstp(55)=20650;     // (needed for MRST2007lomod (LO* for MC))
      this->pydat2().pmas(6,1)=172.5;    // PDG2007 TOP mass
      this->pydat2().pmas(24,1)=80.399;  // PDG2007 W mass
      this->pydat2().pmas(23,1)=91.1876; // PDG2007 Z0 mass
      this->pypars().mstp(70)=0;         // (virtuality scale for ISR)
      this->pypars().mstp(72)=1;
      this->pypars().mstp(88)=1;
      this->pypars().mstp(90)=0;
      this->pypars().parp(78)=0.224;
      this->pypars().parp(80)=0.1;
      this->pypars().parp(82)=2.315;  // (cut off scale)
      this->pypars().parp(83)=0.8;
      this->pypars().parp(84)=0.7;       // (matter distribution)
      this->pypars().parp(90)=0.2487;  // (cut off scale)
      this->pydat1().parj(81)=0.29;
      this->pypars().mstp(95)=6; 
      this->pydat1().mstj(11)=4;         // select Lund-Bowler for c && b fragmentation
      this->pydat1().mstj(22)=2;         // (Make K_S, Lambda stable)
      this->pydat1().parj(41)=0.3;       // Lund-Bowler a
      this->pydat1().parj(42)=0.58;      // Lund-Bowler b
      this->pydat1().parj(46)=0.75;      // Lund-Bowler rQ
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(89)=1;
      this->pypars().parp(89)=1800.;
      //________________________________________________________________________________________________________________________
      ATH_MSG_INFO(   "--------------------------------------------------------------------------------\n" 
		     << "atlasTune: parameter setting to Tune_Index "<<m_Tune_Index<<" tune values :       \n" 
		     << "mstp(128) = "<< this->pypars().mstp(128)    << "\n" 
		     << "mstu(21)  = "<< this->pydat1().mstu(21)     << "\n"
		     << "mstp(81) = "<<   this->pypars().mstp(81)     << "\n"       
		     << "mstp(82) = "<<   this->pypars().mstp(82)     << "\n"        
		     << "mstp(52) = "<<   this->pypars().mstp(52)     << "\n"        
		     << "mstp(54) = "<<   this->pypars().mstp(54)     << "\n"        
		     << "mstp(56) = "<<   this->pypars().mstp(56)     << "\n"        
		     << "mstp(51) = "<<   this->pypars().mstp(51)     << "\n"    
		     << "mstp(53) = "<<   this->pypars().mstp(53)     << "\n"    
		     << "mstp(55) = "<<   this->pypars().mstp(55)     << "\n"    
		     << "pmas(6,1) = "<<  this->pydat2().pmas(6,1)    << "\n"   
		     << "pmas(24,1) = "<< this->pydat2().pmas(24,1)   << "\n" 
		     << "pmas(23,1) = "<< this->pydat2().pmas(23,1)   << "\n"
		     << "mstp(70) = "<<   this->pypars().mstp(70)     << "\n"        
		     << "mstp(72) = "<<   this->pypars().mstp(72)     << "\n"
		     << "mstp(88) = "<<   this->pypars().mstp(88)     << "\n"
		     << "mstp(90) = "<<   this->pypars().mstp(90)     << "\n"
		     << "parp(78) = "<<   this->pypars().parp(78)     << "\n"
		     << "parp(80) = "<<   this->pypars().parp(80)     << "\n"
		     << "parp(82) = "<<   this->pypars().parp(82)     << "\n"      
		     << "parp(83) = "<<   this->pypars().parp(83)     << "\n"
		     << "parp(84) = "<<   this->pypars().parp(84)     << "\n"      
		     << "parp(90) = "<<   this->pypars().parp(90)     << "\n"     
		     << "parj(81) = "<<   this->pydat1().parj(81)     << "\n"
		     << "mstp(95) = "<<   this->pypars().mstp(95)     << "\n" 
		     << "mstj(11) = "<<   this->pydat1().mstj(11)     << "\n"        
		     << "mstj(22) = "<<   this->pydat1().mstj(22)     << "\n"        
		     << "parj(41) = "<<   this->pydat1().parj(41)     << "\n"      
		     << "parj(42) = "<<   this->pydat1().parj(42)     << "\n"     
		     << "parj(46) = "<<   this->pydat1().parj(46)     << "\n"     
		     << "mstp(84) = "<<   this->pypars().mstp(84)     << "\n"
		     << "mstp(85) = "<<   this->pypars().mstp(85)     << "\n"
		     << "mstp(86) = "<<   this->pypars().mstp(86)     << "\n"
		     << "mstp(87) = "<<   this->pypars().mstp(87)     << "\n"
		     << "mstp(89) = "<<   this->pypars().mstp(89)     << "\n"
		     << "parp(89) = "<<   this->pypars().parp(89)     << "\n"
		     << "-----------------------------------------------                                 \n"
		     << "atlasTune: end of the list of Tune_Index "<<m_Tune_Index<<" tune values :         \n" 
		     << "--------------------------------------------------------------------------------\n" );
      //________________________________________________________________________________________________________________________
    }// end of if (m_Tune_Index==20090003) condition
  else if (m_Tune_Index==20100001)
    {
      // ATLAS jargon     : AMBT1
      // references       : ATLAS-CONF-2010-031, ATL-COM-PHYS-2010-267, ATL-COM-PHYS-2010-268
      // author           : Markus Warsinsky et. al
      // Pythia           : 6.4.21 
      // impl. date       : June 2010
      // tune produced in : 2010
      // short description: the first tune including the ATLAS (MinBias, UE, 0.9TeV and 7TeV) data 
      //________________________________________________________________________________________________________________________
      this->pypars().mstp(128)  =1;      // fix junk output for documentary particles 	 
      this->pydat1().mstu(21)   =1;      // error handling switch 
      //physics - related
      this->pypars().mstp(81)=21;        // run PYEVNW with PYEVNT 
      this->pypars().mstp(82)=4;         // MI structure           
      this->pypars().mstp(52)=2;         // (needed for MRST2007lomod (LO* for MC))    
      this->pypars().mstp(54)=2;         // (needed for MRST2007lomod (LO* for MC))    
      this->pypars().mstp(56)=2;         // (needed for MRST2007lomod (LO* for MC))     
      this->pypars().mstp(51)=20650;     // (needed for MRST2007lomod (LO* for MC)) 	 
      this->pypars().mstp(53)=20650;     // (needed for MRST2007lomod (LO* for MC)) 	 
      this->pypars().mstp(55)=20650;     // (needed for MRST2007lomod (LO* for MC))
      this->pydat2().pmas(6,1)=172.5;    // PDG2007 TOP mass
      this->pydat2().pmas(24,1)=80.399;  // PDG2007 W mass
      this->pydat2().pmas(23,1)=91.1876; // PDG2007 Z0 mass
      this->pypars().parp(62)=1.025;
      this->pypars().mstp(70)=0;         // (virtuality scale for ISR)
      this->pypars().mstp(72)=1;
      this->pypars().mstp(88)=1;
      this->pypars().mstp(90)=0;
      this->pypars().parp(77)=1.016;
      this->pypars().parp(78)=0.538;
      this->pypars().parp(80)=0.1;
      this->pypars().parp(82)=2.292;  // (cut off scale)
      this->pypars().parp(83)=0.356;
      this->pypars().parp(84)=0.651; // (matter distribution)
      this->pypars().parp(90)=0.250;  // (cut off scale)
      this->pypars().parp(93)=10.0;
      this->pydat1().parj(81)=0.29;
      this->pypars().mstp(95)=6; 
      this->pydat1().mstj(11)=4;         // select Lund-Bowler for c && b fragmentation
      this->pydat1().mstj(22)=2;         // (Make K_S, Lambda stable)
      this->pydat1().parj(41)=0.3;       // Lund-Bowler a
      this->pydat1().parj(42)=0.58;      // Lund-Bowler b
      this->pydat1().parj(46)=0.75;      // Lund-Bowler rQ
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(89)=1;
      this->pypars().parp(89)=1800.;
      //________________________________________________________________________________________________________________________
      ATH_MSG_INFO(   "--------------------------------------------------------------------------------\n" 
		     << "atlasTune: parameter setting to Tune_Index "<<m_Tune_Index<<" tune values :       \n" 
		     << "mstp(128) = "<< this->pypars().mstp(128)    << "\n" 
		     << "mstu(21)  = "<< this->pydat1().mstu(21)     << "\n"
		     << "mstp(81) = "<<   this->pypars().mstp(81)     << "\n"       
		     << "mstp(82) = "<<   this->pypars().mstp(82)     << "\n"        
		     << "mstp(52) = "<<   this->pypars().mstp(52)     << "\n"        
		     << "mstp(54) = "<<   this->pypars().mstp(54)     << "\n"        
		     << "mstp(56) = "<<   this->pypars().mstp(56)     << "\n"        
		     << "mstp(51) = "<<   this->pypars().mstp(51)     << "\n"    
		     << "mstp(53) = "<<   this->pypars().mstp(53)     << "\n"    
		     << "mstp(55) = "<<   this->pypars().mstp(55)     << "\n"    
		     << "pmas(6,1) = "<<  this->pydat2().pmas(6,1)    << "\n"   
		     << "pmas(24,1) = "<< this->pydat2().pmas(24,1)   << "\n" 
		     << "pmas(23,1) = "<< this->pydat2().pmas(23,1)   << "\n"
		     << "parp(62) = "<<   this->pypars().parp(62)     << "\n"
		     << "mstp(70) = "<<   this->pypars().mstp(70)     << "\n"        
		     << "mstp(72) = "<<   this->pypars().mstp(72)     << "\n"
		     << "mstp(88) = "<<   this->pypars().mstp(88)     << "\n"
		     << "mstp(90) = "<<   this->pypars().mstp(90)     << "\n"
		     << "parp(77) = "<<   this->pypars().parp(77)     << "\n"
		     << "parp(78) = "<<   this->pypars().parp(78)     << "\n"
		     << "parp(80) = "<<   this->pypars().parp(80)     << "\n"
		     << "parp(82) = "<<   this->pypars().parp(82)     << "\n"      
		     << "parp(83) = "<<   this->pypars().parp(83)     << "\n"
		     << "parp(84) = "<<   this->pypars().parp(84)     << "\n"      
		     << "parp(90) = "<<   this->pypars().parp(90)     << "\n" 
		     << "parp(93) = "<<   this->pypars().parp(93)     << "\n"    
		     << "parj(81) = "<<   this->pydat1().parj(81)     << "\n"
		     << "mstp(95) = "<<   this->pypars().mstp(95)     << "\n" 
		     << "mstj(11) = "<<   this->pydat1().mstj(11)     << "\n"        
		     << "mstj(22) = "<<   this->pydat1().mstj(22)     << "\n"        
		     << "parj(41) = "<<   this->pydat1().parj(41)     << "\n"      
		     << "parj(42) = "<<   this->pydat1().parj(42)     << "\n"     
		     << "parj(46) = "<<   this->pydat1().parj(46)     << "\n"     
		     << "mstp(84) = "<<   this->pypars().mstp(84)     << "\n"
		     << "mstp(85) = "<<   this->pypars().mstp(85)     << "\n"
		     << "mstp(86) = "<<   this->pypars().mstp(86)     << "\n"
		     << "mstp(87) = "<<   this->pypars().mstp(87)     << "\n"
		     << "mstp(89) = "<<   this->pypars().mstp(89)     << "\n"
		     << "parp(89) = "<<   this->pypars().parp(89)     << "\n"
		     << "-----------------------------------------------                                 \n"
		     << "atlasTune: end of the list of Tune_Index "<<m_Tune_Index<<" tune values :         \n" 
		     << "--------------------------------------------------------------------------------\n" );
      //________________________________________________________________________________________________________________________
    }// end of if (m_Tune_Index==20100001) condition
  else if (m_Tune_Index==20110001)
    {
      // ATLAS jargon     : AUET2
      // references       : todo : add
      // author           : A. Buckley, J. Katzy (et. al)
      // Pythia           : 6.4.23 
      // impl. date       : Mar 2011
      // tune produced in : 2011
      // short description: LO** tune incl. ISR/FSR parameters, optimized for UE
      //________________________________________________________________________________________________________________________
      this->pypars().mstp(128)=1;
      this->pydat1().mstu(21)=1;
      this->pypars().mstp(81)=21;
      this->pypars().mstp(82)=4;
      this->pypars().mstp(88)=1;
      this->pypars().mstp(90)=0;
      this->pypars().parp(80)=0.1;
      this->pypars().parp(83)=0.356;
      this->pypars().parp(93)=10.0;
      this->pypars().mstp(95)=6;
      this->pydat1().mstj(22)=2;
      this->pydat1().parj(46)=0.75;
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(89)=1;
      this->pypars().parp(89)=1800.;
      this->pydat2().pmas(6,1)=172.5;
      this->pydat2().pmas(24,1)=80.399;
      this->pydat2().pmas(23,1)=91.1876;
      this->pypars().mstp(51)=20651;
      this->pypars().mstp(53)=20651;
      this->pypars().mstp(55)=20651;
      this->pypars().mstp(52)=2;
      this->pypars().mstp(54)=2;
      this->pypars().mstp(56)=2;
      this->pydat1().mstj(11)=5;
      this->pydat1().parj(1)=7.272809e-02;
      this->pydat1().parj(2)=2.018845e-01;
      this->pydat1().parj(3)=9.498471e-01;
      this->pydat1().parj(4)=3.316182e-02;
      this->pydat1().parj(11)=3.089764e-01;
      this->pydat1().parj(12)=4.015396e-01;
      this->pydat1().parj(13)=5.442874e-01;
      this->pydat1().parj(25)=6.276964e-01;
      this->pydat1().parj(26)=1.292377e-01;
      this->pydat1().parj(21)=3.001463e-01;
      this->pydat1().parj(41)=3.683123e-01;
      this->pydat1().parj(42)=1.003531e+00;
      this->pydat1().parj(47)=8.727703e-01;
      this->pydat1().parj(81)=2.564716e-01;
      this->pydat1().parj(82)=8.296215e-01;
      this->pypars().mstp(3)=1;
      this->pydat1().paru(112)=0.265;
      this->pypars().parp(1)=0.265;
      this->pypars().parp(61)=0.265;
      this->pydat1().mstu(112)=4;
      this->pypars().mstp(70)=0;
      this->pypars().mstp(64)=3;
      this->pypars().mstp(72)=2;
      this->pypars().parp(62)=2.803673e+00;
      this->pypars().parp(64)=2.213751e+00;
      this->pypars().parp(67)=6.635356e-01;
      this->pypars().parp(72)=2.496789e-01;
      this->pypars().parp(91)=1.918441e+00;
      this->pypars().parp(77)=1.127437e+00;
      this->pypars().parp(78)=3.320624e-01;
      this->pypars().parp(82)=2.451288e+00;
      this->pypars().parp(84)=5.296569e-01;
      this->pypars().parp(90)=2.290732e-01;    
      ATH_MSG_INFO( "---------------------------------------------------------------------------------\n"
		    << "atlasTune: parameter setting to Tune_Index "<<m_Tune_Index<<" tune values :      \n"
		    << "-----------------------------------------------                                  \n"
		    << "mstp(128)=" << this->pypars().mstp(128) << "\n"
		    << "mstu(21)=" << this->pydat1().mstu(21) << "\n"
		    << "mstp(81)=" << this->pypars().mstp(81) << "\n"
		    << "mstp(82)=" << this->pypars().mstp(82) << "\n"
		    << "mstp(88)=" << this->pypars().mstp(88) << "\n"
		    << "mstp(90)=" << this->pypars().mstp(90) << "\n"
		    << "parp(80)=" << this->pypars().parp(80) << "\n"
		    << "parp(83)=" << this->pypars().parp(83) << "\n"
		    << "parp(93)=" << this->pypars().parp(93) << "\n"
		    << "mstp(95)=" << this->pypars().mstp(95) << "\n"
		    << "mstj(22)=" << this->pydat1().mstj(22) << "\n"
		    << "parj(46)=" << this->pydat1().parj(46) << "\n"
		    << "mstp(84)=" << this->pypars().mstp(84) << "\n"
		    << "mstp(85)=" << this->pypars().mstp(85) << "\n"
		    << "mstp(86)=" << this->pypars().mstp(86) << "\n"
		    << "mstp(87)=" << this->pypars().mstp(87) << "\n"
		    << "mstp(89)=" << this->pypars().mstp(89) << "\n"
		    << "parp(89)=" << this->pypars().parp(89) << "\n"
		    << "pmas(6,1)=" << this->pydat2().pmas(6,1) << "\n"
		    << "pmas(24,1)=" << this->pydat2().pmas(24,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "mstp(51)=" << this->pypars().mstp(51) << "\n"
		    << "mstp(53)=" << this->pypars().mstp(53) << "\n"
		    << "mstp(55)=" << this->pypars().mstp(55) << "\n"
		    << "mstp(52)=" << this->pypars().mstp(52) << "\n"
		    << "mstp(54)=" << this->pypars().mstp(54) << "\n"
		    << "mstp(56)=" << this->pypars().mstp(56) << "\n"
		    << "mstj(11)=" << this->pydat1().mstj(11) << "\n"
		    << "parj(1)=" << this->pydat1().parj(1) << "\n"
		    << "parj(2)=" << this->pydat1().parj(2) << "\n"
		    << "parj(3)=" << this->pydat1().parj(3) << "\n"
		    << "parj(4)=" << this->pydat1().parj(4) << "\n"
		    << "parj(11)=" << this->pydat1().parj(11) << "\n"
		    << "parj(12)=" << this->pydat1().parj(12) << "\n"
		    << "parj(13)=" << this->pydat1().parj(13) << "\n"
		    << "parj(25)=" << this->pydat1().parj(25) << "\n"
		    << "parj(26)=" << this->pydat1().parj(26) << "\n"
		    << "parj(21)=" << this->pydat1().parj(21) << "\n"
		    << "parj(41)=" << this->pydat1().parj(41) << "\n"
		    << "parj(42)=" << this->pydat1().parj(42) << "\n"
		    << "parj(47)=" << this->pydat1().parj(47) << "\n"
		    << "parj(81)=" << this->pydat1().parj(81) << "\n"
		    << "parj(82)=" << this->pydat1().parj(82) << "\n"
		    << "mstp(3)=" << this->pypars().mstp(3) << "\n"
		    << "paru(112)=" << this->pydat1().paru(112) << "\n"
		    << "parp(1)=" << this->pypars().parp(1) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "mstu(112)=" << this->pydat1().mstu(112) << "\n"
		    << "mstp(70)=" << this->pypars().mstp(70) << "\n"
		    << "mstp(64)=" << this->pypars().mstp(64) << "\n"
		    << "mstp(72)=" << this->pypars().mstp(72) << "\n"
		    << "parp(62)=" << this->pypars().parp(62) << "\n"
		    << "parp(64)=" << this->pypars().parp(64) << "\n"
		    << "parp(67)=" << this->pypars().parp(67) << "\n"
		    << "parp(72)=" << this->pypars().parp(72) << "\n"
		    << "parp(91)=" << this->pypars().parp(91) << "\n"
		    << "parp(77)=" << this->pypars().parp(77) << "\n"
		    << "parp(78)=" << this->pypars().parp(78) << "\n"
		    << "parp(82)=" << this->pypars().parp(82) << "\n"
		    << "parp(84)=" << this->pypars().parp(84) << "\n"
		    << "parp(90)=" << this->pypars().parp(90) << "\n"
		    << "-----------------------------------------------                                 \n"
		    << "atlasTune: end of the list of Tune_Index "<<m_Tune_Index<<" tune values :         \n"
		    << "--------------------------------------------------------------------------------\n" );
    } // end of 20110001 (AUET2) condition
  else if (m_Tune_Index==20110101)
    {
      // ATLAS jargon     : AMBT2
      // references       : todo : add
      // author           : A. Buckley, J. Katzy (et. al)
      // Pythia           : 6.4.23 
      // impl. date       : Mar 2011
      // tune produced in : 2011
      // short description: LO** tune incl. ISR/FSR parameters, optimized for MinBias
      //________________________________________________________________________________________________________________________
      this->pypars().mstp(128)=1;
      this->pydat1().mstu(21)=1;
      this->pypars().mstp(81)=21;
      this->pypars().mstp(82)=4;
      this->pypars().mstp(88)=1;
      this->pypars().mstp(90)=0;
      this->pypars().parp(80)=0.1;
      this->pypars().parp(83)=0.356;
      this->pypars().parp(93)=10.0;
      this->pypars().mstp(95)=6;
      this->pydat1().mstj(22)=2;
      this->pydat1().parj(46)=0.75;
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(89)=1;
      this->pypars().parp(89)=1800.;
      this->pydat2().pmas(6,1)=172.5;
      this->pydat2().pmas(24,1)=80.399;
      this->pydat2().pmas(23,1)=91.1876;
      this->pypars().mstp(51)=20651;
      this->pypars().mstp(53)=20651;
      this->pypars().mstp(55)=20651;
      this->pypars().mstp(52)=2;
      this->pypars().mstp(54)=2;
      this->pypars().mstp(56)=2;
      this->pydat1().mstj(11)=5;
      this->pydat1().parj(1)=7.272809e-02;
      this->pydat1().parj(2)=2.018845e-01;
      this->pydat1().parj(3)=9.498471e-01;
      this->pydat1().parj(4)=3.316182e-02;
      this->pydat1().parj(11)=3.089764e-01;
      this->pydat1().parj(12)=4.015396e-01;
      this->pydat1().parj(13)=5.442874e-01;
      this->pydat1().parj(25)=6.276964e-01;
      this->pydat1().parj(26)=1.292377e-01;
      this->pydat1().parj(21)=3.001463e-01;
      this->pydat1().parj(41)=3.683123e-01;
      this->pydat1().parj(42)=1.003531e+00;
      this->pydat1().parj(47)=8.727703e-01;
      this->pydat1().parj(81)=2.564716e-01;
      this->pydat1().parj(82)=8.296215e-01;
      this->pypars().mstp(3)=1;
      this->pydat1().paru(112)=0.265;
      this->pypars().parp(1)=0.265;
      this->pypars().parp(61)=0.265;
      this->pydat1().mstu(112)=4;
      this->pypars().mstp(70)=0;
      this->pypars().mstp(64)=3;
      this->pypars().mstp(72)=2;
      this->pypars().parp(62)=2.803673e+00;
      this->pypars().parp(64)=2.213751e+00;
      this->pypars().parp(67)=6.635356e-01;
      this->pypars().parp(72)=2.496789e-01;
      this->pypars().parp(91)=1.918441e+00;
      this->pypars().parp(77)=0.88;
      this->pypars().parp(78)=0.18;
      this->pypars().parp(82)=2.49;
      this->pypars().parp(84)=0.61;
      this->pypars().parp(90)=0.244;    
      ATH_MSG_INFO( "---------------------------------------------------------------------------------\n"
		    << "atlasTune: parameter setting to Tune_Index "<<m_Tune_Index<<" tune values :      \n"
		    << "-----------------------------------------------                                  \n"
		    << "mstp(128)=" << this->pypars().mstp(128) << "\n"
		    << "mstu(21)=" << this->pydat1().mstu(21) << "\n"
		    << "mstp(81)=" << this->pypars().mstp(81) << "\n"
		    << "mstp(82)=" << this->pypars().mstp(82) << "\n"
		    << "mstp(88)=" << this->pypars().mstp(88) << "\n"
		    << "mstp(90)=" << this->pypars().mstp(90) << "\n"
		    << "parp(80)=" << this->pypars().parp(80) << "\n"
		    << "parp(83)=" << this->pypars().parp(83) << "\n"
		    << "parp(93)=" << this->pypars().parp(93) << "\n"
		    << "mstp(95)=" << this->pypars().mstp(95) << "\n"
		    << "mstj(22)=" << this->pydat1().mstj(22) << "\n"
		    << "parj(46)=" << this->pydat1().parj(46) << "\n"
		    << "mstp(84)=" << this->pypars().mstp(84) << "\n"
		    << "mstp(85)=" << this->pypars().mstp(85) << "\n"
		    << "mstp(86)=" << this->pypars().mstp(86) << "\n"
		    << "mstp(87)=" << this->pypars().mstp(87) << "\n"
		    << "mstp(89)=" << this->pypars().mstp(89) << "\n"
		    << "parp(89)=" << this->pypars().parp(89) << "\n"
		    << "pmas(6,1)=" << this->pydat2().pmas(6,1) << "\n"
		    << "pmas(24,1)=" << this->pydat2().pmas(24,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "mstp(51)=" << this->pypars().mstp(51) << "\n"
		    << "mstp(53)=" << this->pypars().mstp(53) << "\n"
		    << "mstp(55)=" << this->pypars().mstp(55) << "\n"
		    << "mstp(52)=" << this->pypars().mstp(52) << "\n"
		    << "mstp(54)=" << this->pypars().mstp(54) << "\n"
		    << "mstp(56)=" << this->pypars().mstp(56) << "\n"
		    << "mstj(11)=" << this->pydat1().mstj(11) << "\n"
		    << "parj(1)=" << this->pydat1().parj(1) << "\n"
		    << "parj(2)=" << this->pydat1().parj(2) << "\n"
		    << "parj(3)=" << this->pydat1().parj(3) << "\n"
		    << "parj(4)=" << this->pydat1().parj(4) << "\n"
		    << "parj(11)=" << this->pydat1().parj(11) << "\n"
		    << "parj(12)=" << this->pydat1().parj(12) << "\n"
		    << "parj(13)=" << this->pydat1().parj(13) << "\n"
		    << "parj(25)=" << this->pydat1().parj(25) << "\n"
		    << "parj(26)=" << this->pydat1().parj(26) << "\n"
		    << "parj(21)=" << this->pydat1().parj(21) << "\n"
		    << "parj(41)=" << this->pydat1().parj(41) << "\n"
		    << "parj(42)=" << this->pydat1().parj(42) << "\n"
		    << "parj(47)=" << this->pydat1().parj(47) << "\n"
		    << "parj(81)=" << this->pydat1().parj(81) << "\n"
		    << "parj(82)=" << this->pydat1().parj(82) << "\n"
		    << "mstp(3)=" << this->pypars().mstp(3) << "\n"
		    << "paru(112)=" << this->pydat1().paru(112) << "\n"
		    << "parp(1)=" << this->pypars().parp(1) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "mstu(112)=" << this->pydat1().mstu(112) << "\n"
		    << "mstp(70)=" << this->pypars().mstp(70) << "\n"
		    << "mstp(64)=" << this->pypars().mstp(64) << "\n"
		    << "mstp(72)=" << this->pypars().mstp(72) << "\n"
		    << "parp(62)=" << this->pypars().parp(62) << "\n"
		    << "parp(64)=" << this->pypars().parp(64) << "\n"
		    << "parp(67)=" << this->pypars().parp(67) << "\n"
		    << "parp(72)=" << this->pypars().parp(72) << "\n"
		    << "parp(91)=" << this->pypars().parp(91) << "\n"
		    << "parp(77)=" << this->pypars().parp(77) << "\n"
		    << "parp(78)=" << this->pypars().parp(78) << "\n"
		    << "parp(82)=" << this->pypars().parp(82) << "\n"
		    << "parp(84)=" << this->pypars().parp(84) << "\n"
		    << "parp(90)=" << this->pypars().parp(90) << "\n"
		    << "-----------------------------------------------                                 \n"
		    << "atlasTune: end of the list of Tune_Index "<<m_Tune_Index<<" tune values :         \n"
		    << "--------------------------------------------------------------------------------\n" );
    } // end of 20110101 (AMBT2) condition
  else if (m_Tune_Index==20110002)
    {
      // ATLAS jargon     : AMBT1FF / AUET2b
      // references       : todo : add
      // author           : A. Buckley, J. Katzy (et. al)
      // Pythia           : 6.4.23 
      // impl. date       : Apr 2011
      // tune produced in : 2011
      // short description: LO** tune incl. ISR/FSR parameters, optimized for UE
      //_______________________________________________________________________________________________________________________
      this->pypars().mstp(128)=1;
      this->pydat1().mstu(21)=1;
      this->pypars().mstp(81)=21;
      this->pypars().mstp(82)=4;
      this->pypars().mstp(72)=1;
      this->pypars().mstp(88)=1;
      this->pypars().mstp(90)=0;
      this->pypars().parp(80)=0.1;
      this->pypars().parp(83)=0.356;
      this->pypars().parp(93)=10.0;
      this->pypars().mstp(95)=6;
      this->pydat1().mstj(22)=2;
      this->pydat1().parj(46)=0.75;
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(89)=1;
      this->pypars().parp(89)=1800.;
      this->pydat2().pmas(6,1)=172.5;
      this->pydat2().pmas(24,1)=80.399;
      this->pydat2().pmas(23,1)=91.1876;
      this->pypars().mstp(51)=20651;
      this->pypars().mstp(53)=20651;
      this->pypars().mstp(55)=20651;
      this->pypars().mstp(52)=2;
      this->pypars().mstp(54)=2;
      this->pypars().mstp(56)=2;
      this->pydat1().mstj(11)=5;
      this->pydat1().parj(1)=7.272809e-02;
      this->pydat1().parj(2)=2.018845e-01;
      this->pydat1().parj(3)=9.498471e-01;
      this->pydat1().parj(4)=3.316182e-02;
      this->pydat1().parj(11)=3.089764e-01;
      this->pydat1().parj(12)=4.015396e-01;
      this->pydat1().parj(13)=5.442874e-01;
      this->pydat1().parj(25)=6.276964e-01;
      this->pydat1().parj(26)=1.292377e-01;
      this->pydat1().parj(21)=3.001463e-01;
      this->pydat1().parj(41)=3.683123e-01;
      this->pydat1().parj(42)=1.003531e+00;
      this->pydat1().parj(47)=8.727703e-01;
      this->pydat1().parj(81)=2.564716e-01;
      this->pydat1().parj(82)=8.296215e-01;
      this->pypars().mstp(3)=1;
      this->pydat1().paru(112)=0.192;
      this->pypars().parp(1)=0.192;
      this->pypars().parp(61)=0.192;
      this->pypars().mstp(70)=0;
      this->pypars().parp(67)=1.00;
      this->pypars().parp(91)=2.00;
      this->pypars().parp(62)=2.17;
      this->pypars().parp(64)=0.60;
      this->pypars().parp(72)=0.43;
      this->pypars().parp(77)=9.006820e-01;
      this->pypars().parp(78)=3.092771e-01;
      this->pypars().parp(82)=2.437442e+00;
      this->pypars().parp(84)=5.598934e-01;
      this->pypars().parp(90)=2.414007e-01;
      ATH_MSG_INFO( "---------------------------------------------------------------------------------\n"
		    << "atlasTune: parameter setting to Tune_Index "<<m_Tune_Index<<" tune values :      \n"
		    << "-----------------------------------------------                                  \n"
		    << "mstp(128)=" << this->pypars().mstp(128) << "\n"
		    << "mstu(21)=" << this->pydat1().mstu(21)  << "\n"
		    << "mstp(81)=" << this->pypars().mstp(81)  << "\n"
		    << "mstp(82)=" << this->pypars().mstp(82) << "\n"
		    << "mstp(72)=" << this->pypars().mstp(72) << "\n"
		    << "mstp(88)=" << this->pypars().mstp(88) << "\n"
		    << "mstp(90)=" << this->pypars().mstp(90) << "\n"
		    << "parp(80)=" << this->pypars().parp(80) << "\n"
		    << "parp(83)=" << this->pypars().parp(83) << "\n"
		    << "parp(93)=" << this->pypars().parp(93) << "\n"
		    << "mstp(95)=" << this->pypars().mstp(95) << "\n"
		    << "mstj(22)=" << this->pydat1().mstj(22) << "\n"
		    << "parj(46)=" << this->pydat1().parj(46) << "\n"
		    << "mstp(84)=" << this->pypars().mstp(84) << "\n"
		    << "mstp(85)=" << this->pypars().mstp(85) << "\n"
		    << "mstp(86)=" << this->pypars().mstp(86) << "\n"
		    << "mstp(87)=" << this->pypars().mstp(87) << "\n"
		    << "mstp(89)=" << this->pypars().mstp(89) << "\n"
		    << "parp(89)=" << this->pypars().parp(89) << "\n"
		    << "pmas(6,1)=" << this->pydat2().pmas(6,1) << "\n"
		    << "pmas(24,1)=" << this->pydat2().pmas(24,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "mstp(51)=" << this->pypars().mstp(51) << "\n"
		    << "mstp(53)=" << this->pypars().mstp(53) << "\n"
		    << "mstp(55)=" << this->pypars().mstp(55) << "\n"
		    << "mstp(52)=" << this->pypars().mstp(52) << "\n"
		    << "mstp(54)=" << this->pypars().mstp(54) << "\n"
		    << "mstp(56)=" << this->pypars().mstp(56) << "\n"
		    << "mstj(11)=" << this->pydat1().mstj(11) << "\n"
		    << "parj(1)=" << this->pydat1().parj(1) << "\n"
		    << "parj(2)=" << this->pydat1().parj(2) << "\n"
		    << "parj(3)=" << this->pydat1().parj(3) << "\n"
		    << "parj(4)=" << this->pydat1().parj(4) << "\n"
		    << "parj(11)=" << this->pydat1().parj(11) << "\n"
		    << "parj(12)=" << this->pydat1().parj(12) << "\n"
		    << "parj(13)=" << this->pydat1().parj(13) << "\n"
		    << "parj(25)=" << this->pydat1().parj(25) << "\n"
		    << "parj(26)=" << this->pydat1().parj(26) << "\n"
		    << "parj(21)=" << this->pydat1().parj(21) << "\n"
		    << "parj(41)=" << this->pydat1().parj(41) << "\n"
		    << "parj(42)=" << this->pydat1().parj(42) << "\n"
		    << "parj(47)=" << this->pydat1().parj(47) << "\n"
		    << "parj(81)=" << this->pydat1().parj(81) << "\n"
		    << "parj(82)=" << this->pydat1().parj(82) << "\n"
		    << "mstp(3)=" << this->pypars().mstp(3) << "\n"
		    << "paru(112)=" << this->pydat1().paru(112) << "\n"
		    << "parp(1)=" << this->pypars().parp(1) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "mstp(70)=" << this->pypars().mstp(70) << "\n"
		    << "parp(67)=" << this->pypars().parp(67) << "\n"
		    << "parp(91)=" << this->pypars().parp(91) << "\n"
		    << "parp(62)=" << this->pypars().parp(62) << "\n"
		    << "parp(64)=" << this->pypars().parp(64) << "\n"
		    << "parp(72)=" << this->pypars().parp(72) << "\n"
		    << "parp(77)=" << this->pypars().parp(77) << "\n"
		    << "parp(78)=" << this->pypars().parp(78) << "\n"
		    << "parp(82)=" << this->pypars().parp(82) << "\n"
		    << "parp(84)=" << this->pypars().parp(84) << "\n"
		    << "parp(90)=" << this->pypars().parp(90) << "\n"
		    << "-----------------------------------------------                                 \n"
		    << "atlasTune: end of the list of Tune_Index "<<m_Tune_Index<<" tune values :         \n"
		    << "--------------------------------------------------------------------------------\n" );
    } // end of 20110002 (AMBT1FF/AUET2b) condition
  else if (m_Tune_Index==20110102)
    {
      // ATLAS jargon     : AMBT2b
      // references       : todo : add
      // author           : A. Buckley, J. Katzy (et. al)
      // Pythia           : 6.4.23 
      // impl. date       : Apr 2011
      // tune produced in : 2011
      // short description: LO** tune incl. ISR/FSR parameters, optimized for MinBias
      //_______________________________________________________________________________________________________________________
      this->pypars().mstp(128)=1;
      this->pydat1().mstu(21)=1;
      this->pypars().mstp(81)=21;
      this->pypars().mstp(82)=4;
      this->pypars().mstp(72)=1;
      this->pypars().mstp(88)=1;
      this->pypars().mstp(90)=0;
      this->pypars().parp(80)=0.1;
      this->pypars().parp(83)=0.356;
      this->pypars().parp(93)=10.0;
      this->pypars().mstp(95)=6;
      this->pydat1().mstj(22)=2;
      this->pydat1().parj(46)=0.75;
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(89)=1;
      this->pypars().parp(89)=1800.;
      this->pydat2().pmas(6,1)=172.5;
      this->pydat2().pmas(24,1)=80.399;
      this->pydat2().pmas(23,1)=91.1876;
      this->pypars().mstp(51)=20651;
      this->pypars().mstp(53)=20651;
      this->pypars().mstp(55)=20651;
      this->pypars().mstp(52)=2;
      this->pypars().mstp(54)=2;
      this->pypars().mstp(56)=2;
      this->pydat1().mstj(11)=5;
      this->pydat1().parj(1)=7.272809e-02;
      this->pydat1().parj(2)=2.018845e-01;
      this->pydat1().parj(3)=9.498471e-01;
      this->pydat1().parj(4)=3.316182e-02;
      this->pydat1().parj(11)=3.089764e-01;
      this->pydat1().parj(12)=4.015396e-01;
      this->pydat1().parj(13)=5.442874e-01;
      this->pydat1().parj(25)=6.276964e-01;
      this->pydat1().parj(26)=1.292377e-01;
      this->pydat1().parj(21)=3.001463e-01;
      this->pydat1().parj(41)=3.683123e-01;
      this->pydat1().parj(42)=1.003531e+00;
      this->pydat1().parj(47)=8.727703e-01;
      this->pydat1().parj(81)=2.564716e-01;
      this->pydat1().parj(82)=8.296215e-01;
      this->pypars().mstp(3)=1;
      this->pydat1().paru(112)=0.192;
      this->pypars().parp(1)=0.192;
      this->pypars().parp(61)=0.192;
      this->pypars().mstp(70)=0;
      this->pypars().parp(67)=1.00;
      this->pypars().parp(91)=2.00;
      this->pypars().parp(62)=2.17;
      this->pypars().parp(64)=0.60;
      this->pypars().parp(72)=0.43;
      this->pypars().parp(77)=1.100584e+00;
      this->pypars().parp(78)=2.478809e-01;
      this->pypars().parp(82)=2.592214e+00;
      this->pypars().parp(84)=6.669917e-01;
      this->pypars().parp(90)=2.552765e-01;
      ATH_MSG_INFO( "---------------------------------------------------------------------------------\n"
		    << "atlasTune: parameter setting to Tune_Index "<<m_Tune_Index<<" tune values :      \n"
		    << "-----------------------------------------------                                  \n"
		    << "mstp(128)=" << this->pypars().mstp(128) << "\n"
		    << "mstu(21)=" << this->pydat1().mstu(21)  << "\n"
		    << "mstp(81)=" << this->pypars().mstp(81)  << "\n"
		    << "mstp(82)=" << this->pypars().mstp(82) << "\n"
		    << "mstp(72)=" << this->pypars().mstp(72) << "\n"
		    << "mstp(88)=" << this->pypars().mstp(88) << "\n"
		    << "mstp(90)=" << this->pypars().mstp(90) << "\n"
		    << "parp(80)=" << this->pypars().parp(80) << "\n"
		    << "parp(83)=" << this->pypars().parp(83) << "\n"
		    << "parp(93)=" << this->pypars().parp(93) << "\n"
		    << "mstp(95)=" << this->pypars().mstp(95) << "\n"
		    << "mstj(22)=" << this->pydat1().mstj(22) << "\n"
		    << "parj(46)=" << this->pydat1().parj(46) << "\n"
		    << "mstp(84)=" << this->pypars().mstp(84) << "\n"
		    << "mstp(85)=" << this->pypars().mstp(85) << "\n"
		    << "mstp(86)=" << this->pypars().mstp(86) << "\n"
		    << "mstp(87)=" << this->pypars().mstp(87) << "\n"
		    << "mstp(89)=" << this->pypars().mstp(89) << "\n"
		    << "parp(89)=" << this->pypars().parp(89) << "\n"
		    << "pmas(6,1)=" << this->pydat2().pmas(6,1) << "\n"
		    << "pmas(24,1)=" << this->pydat2().pmas(24,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "mstp(51)=" << this->pypars().mstp(51) << "\n"
		    << "mstp(53)=" << this->pypars().mstp(53) << "\n"
		    << "mstp(55)=" << this->pypars().mstp(55) << "\n"
		    << "mstp(52)=" << this->pypars().mstp(52) << "\n"
		    << "mstp(54)=" << this->pypars().mstp(54) << "\n"
		    << "mstp(56)=" << this->pypars().mstp(56) << "\n"
		    << "mstj(11)=" << this->pydat1().mstj(11) << "\n"
		    << "parj(1)=" << this->pydat1().parj(1) << "\n"
		    << "parj(2)=" << this->pydat1().parj(2) << "\n"
		    << "parj(3)=" << this->pydat1().parj(3) << "\n"
		    << "parj(4)=" << this->pydat1().parj(4) << "\n"
		    << "parj(11)=" << this->pydat1().parj(11) << "\n"
		    << "parj(12)=" << this->pydat1().parj(12) << "\n"
		    << "parj(13)=" << this->pydat1().parj(13) << "\n"
		    << "parj(25)=" << this->pydat1().parj(25) << "\n"
		    << "parj(26)=" << this->pydat1().parj(26) << "\n"
		    << "parj(21)=" << this->pydat1().parj(21) << "\n"
		    << "parj(41)=" << this->pydat1().parj(41) << "\n"
		    << "parj(42)=" << this->pydat1().parj(42) << "\n"
		    << "parj(47)=" << this->pydat1().parj(47) << "\n"
		    << "parj(81)=" << this->pydat1().parj(81) << "\n"
		    << "parj(82)=" << this->pydat1().parj(82) << "\n"
		    << "mstp(3)=" << this->pypars().mstp(3) << "\n"
		    << "paru(112)=" << this->pydat1().paru(112) << "\n"
		    << "parp(1)=" << this->pypars().parp(1) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "mstp(70)=" << this->pypars().mstp(70) << "\n"
		    << "parp(67)=" << this->pypars().parp(67) << "\n"
		    << "parp(91)=" << this->pypars().parp(91) << "\n"
		    << "parp(62)=" << this->pypars().parp(62) << "\n"
		    << "parp(64)=" << this->pypars().parp(64) << "\n"
		    << "parp(72)=" << this->pypars().parp(72) << "\n"
		    << "parp(77)=" << this->pypars().parp(77) << "\n"
		    << "parp(78)=" << this->pypars().parp(78) << "\n"
		    << "parp(82)=" << this->pypars().parp(82) << "\n"
		    << "parp(84)=" << this->pypars().parp(84) << "\n"
		    << "parp(90)=" << this->pypars().parp(90) << "\n"
		    << "-----------------------------------------------                                 \n"
		    << "atlasTune: end of the list of Tune_Index "<<m_Tune_Index<<" tune values :         \n"
		    << "--------------------------------------------------------------------------------\n" );
    } // end of 20110102 (AMBT2b) condition
  else if (m_Tune_Index==20110003)
    {
      // ATLAS jargon     : AUET2b
      // references       : todo : add
      // author           : A. Buckley, J. Katzy (et. al)
      // Pythia           : 6.4.23 
      // impl. date       : Apr 2011
      // tune produced in : 2011
      // short description: CTEQ6L1 tune incl. ISR/FSR parameters, optimized for UE
      //_______________________________________________________________________________________________________________________
      this->pypars().mstp(128)=1;
      this->pydat1().mstu(21)=1;
      this->pypars().mstp(81)=21;
      this->pypars().mstp(82)=4;
      this->pypars().mstp(72)=1;
      this->pypars().mstp(88)=1;
      this->pypars().mstp(90)=0;
      this->pypars().parp(80)=0.1;
      this->pypars().parp(83)=0.356;
      this->pypars().parp(93)=10.0;
      this->pypars().mstp(95)=6;
      this->pydat1().mstj(22)=2;
      this->pydat1().parj(46)=0.75;
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(89)=1;
      this->pypars().parp(89)=1800.;
      this->pydat2().pmas(6,1)=172.5;
      this->pydat2().pmas(24,1)=80.399;
      this->pydat2().pmas(23,1)=91.1876;
      this->pypars().mstp(51)=10042;
      this->pypars().mstp(53)=10042;
      this->pypars().mstp(55)=10042;
      this->pypars().mstp(52)=2;
      this->pypars().mstp(54)=2;
      this->pypars().mstp(56)=2;
      this->pydat1().mstj(11)=5;
      this->pydat1().parj(1)=7.272809e-02;
      this->pydat1().parj(2)=2.018845e-01;
      this->pydat1().parj(3)=9.498471e-01;
      this->pydat1().parj(4)=3.316182e-02;
      this->pydat1().parj(11)=3.089764e-01;
      this->pydat1().parj(12)=4.015396e-01;
      this->pydat1().parj(13)=5.442874e-01;
      this->pydat1().parj(25)=6.276964e-01;
      this->pydat1().parj(26)=1.292377e-01;
      this->pydat1().parj(21)=3.001463e-01;
      this->pydat1().parj(41)=3.683123e-01;
      this->pydat1().parj(42)=1.003531e+00;
      this->pydat1().parj(47)=8.727703e-01;
      this->pydat1().parj(81)=2.564716e-01;
      this->pydat1().parj(82)=8.296215e-01;
      this->pypars().mstp(3)=1;
      this->pydat1().paru(112)=0.192;
      this->pypars().parp(1)=0.192;
      this->pypars().parp(61)=0.192;
      this->pypars().mstp(70)=0;
      this->pypars().parp(67)=1.00;
      this->pypars().parp(91)=2.00;
      this->pypars().parp(62)=1.13;
      this->pypars().parp(64)=0.68;
      this->pypars().parp(72)=0.527;
      this->pypars().parp(77)=4.905171e-01;
      this->pypars().parp(78)=3.105197e-01;
      this->pypars().parp(82)=2.259809e+00;
      this->pypars().parp(84)=4.430423e-01;
      this->pypars().parp(90)=2.490763e-01;
      ATH_MSG_INFO( "---------------------------------------------------------------------------------\n"
		    << "atlasTune: parameter setting to Tune_Index "<<m_Tune_Index<<" tune values :      \n"
		    << "-----------------------------------------------                                  \n"
		    << "mstp(128)=" << this->pypars().mstp(128) << "\n"
		    << "mstu(21)=" << this->pydat1().mstu(21)  << "\n"
		    << "mstp(81)=" << this->pypars().mstp(81)  << "\n"
		    << "mstp(82)=" << this->pypars().mstp(82) << "\n"
		    << "mstp(72)=" << this->pypars().mstp(72) << "\n"
		    << "mstp(88)=" << this->pypars().mstp(88) << "\n"
		    << "mstp(90)=" << this->pypars().mstp(90) << "\n"
		    << "parp(80)=" << this->pypars().parp(80) << "\n"
		    << "parp(83)=" << this->pypars().parp(83) << "\n"
		    << "parp(93)=" << this->pypars().parp(93) << "\n"
		    << "mstp(95)=" << this->pypars().mstp(95) << "\n"
		    << "mstj(22)=" << this->pydat1().mstj(22) << "\n"
		    << "parj(46)=" << this->pydat1().parj(46) << "\n"
		    << "mstp(84)=" << this->pypars().mstp(84) << "\n"
		    << "mstp(85)=" << this->pypars().mstp(85) << "\n"
		    << "mstp(86)=" << this->pypars().mstp(86) << "\n"
		    << "mstp(87)=" << this->pypars().mstp(87) << "\n"
		    << "mstp(89)=" << this->pypars().mstp(89) << "\n"
		    << "parp(89)=" << this->pypars().parp(89) << "\n"
		    << "pmas(6,1)=" << this->pydat2().pmas(6,1) << "\n"
		    << "pmas(24,1)=" << this->pydat2().pmas(24,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "mstp(51)=" << this->pypars().mstp(51) << "\n"
		    << "mstp(53)=" << this->pypars().mstp(53) << "\n"
		    << "mstp(55)=" << this->pypars().mstp(55) << "\n"
		    << "mstp(52)=" << this->pypars().mstp(52) << "\n"
		    << "mstp(54)=" << this->pypars().mstp(54) << "\n"
		    << "mstp(56)=" << this->pypars().mstp(56) << "\n"
		    << "mstj(11)=" << this->pydat1().mstj(11) << "\n"
		    << "parj(1)=" << this->pydat1().parj(1) << "\n"
		    << "parj(2)=" << this->pydat1().parj(2) << "\n"
		    << "parj(3)=" << this->pydat1().parj(3) << "\n"
		    << "parj(4)=" << this->pydat1().parj(4) << "\n"
		    << "parj(11)=" << this->pydat1().parj(11) << "\n"
		    << "parj(12)=" << this->pydat1().parj(12) << "\n"
		    << "parj(13)=" << this->pydat1().parj(13) << "\n"
		    << "parj(25)=" << this->pydat1().parj(25) << "\n"
		    << "parj(26)=" << this->pydat1().parj(26) << "\n"
		    << "parj(21)=" << this->pydat1().parj(21) << "\n"
		    << "parj(41)=" << this->pydat1().parj(41) << "\n"
		    << "parj(42)=" << this->pydat1().parj(42) << "\n"
		    << "parj(47)=" << this->pydat1().parj(47) << "\n"
		    << "parj(81)=" << this->pydat1().parj(81) << "\n"
		    << "parj(82)=" << this->pydat1().parj(82) << "\n"
		    << "mstp(3)=" << this->pypars().mstp(3) << "\n"
		    << "paru(112)=" << this->pydat1().paru(112) << "\n"
		    << "parp(1)=" << this->pypars().parp(1) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "mstp(70)=" << this->pypars().mstp(70) << "\n"
		    << "parp(67)=" << this->pypars().parp(67) << "\n"
		    << "parp(91)=" << this->pypars().parp(91) << "\n"
		    << "parp(62)=" << this->pypars().parp(62) << "\n"
		    << "parp(64)=" << this->pypars().parp(64) << "\n"
		    << "parp(72)=" << this->pypars().parp(72) << "\n"
		    << "parp(77)=" << this->pypars().parp(77) << "\n"
		    << "parp(78)=" << this->pypars().parp(78) << "\n"
		    << "parp(82)=" << this->pypars().parp(82) << "\n"
		    << "parp(84)=" << this->pypars().parp(84) << "\n"
		    << "parp(90)=" << this->pypars().parp(90) << "\n"
		    << "-----------------------------------------------                                 \n"
		    << "atlasTune: end of the list of Tune_Index "<<m_Tune_Index<<" tune values :         \n"
		    << "--------------------------------------------------------------------------------\n" );
    } // end of 20110003 (AUET2b) condition
  //==========================================================
  else if (m_Tune_Index==20110004)
    {
      // ATLAS jargon     : AUET2B-MSTW2008LO
      // references       : todo : add
      // author           : A. Buckley (et. al)
      // Pythia           : 6.4.25 
      // impl. date       : Mar 2012
      // tune produced in : 2011
      // short description: AUET2B variant for MSTW2008LO PDF
      //_______________________________________________________________________________________________________________________
      this->pypars().mstp(128)=1;
      this->pydat1().mstu(21)=1;
      this->pypars().mstp(81)=21;
      this->pypars().mstp(82)=4;
      this->pypars().mstp(72)=1;
      this->pypars().mstp(88)=1;
      this->pypars().mstp(90)=0;
      this->pypars().parp(80)=0.1;
      this->pypars().parp(83)=0.356;
      this->pypars().parp(93)=10.0;
      this->pypars().mstp(95)=6;
      this->pydat1().mstj(22)=2;
      this->pydat1().parj(46)=0.75;
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(89)=1;
      this->pypars().parp(89)=1800.;
      this->pydat2().pmas(6,1)=172.5;
      this->pydat2().pmas(24,1)=80.399;
      this->pydat2().pmas(23,1)=91.1876;
      this->pypars().mstp(51)=21000;
      this->pypars().mstp(53)=21000;
      this->pypars().mstp(55)=21000;
      this->pypars().mstp(52)=2;
      this->pypars().mstp(54)=2;
      this->pypars().mstp(56)=2;
      this->pydat1().mstj(11)=5;
      this->pydat1().parj(1)=7.272809e-02;
      this->pydat1().parj(2)=2.018845e-01;
      this->pydat1().parj(3)=9.498471e-01;
      this->pydat1().parj(4)=3.316182e-02;
      this->pydat1().parj(11)=3.089764e-01;
      this->pydat1().parj(12)=4.015396e-01;
      this->pydat1().parj(13)=5.442874e-01;
      this->pydat1().parj(25)=6.276964e-01;
      this->pydat1().parj(26)=1.292377e-01;
      this->pydat1().parj(21)=3.001463e-01;
      this->pydat1().parj(41)=3.683123e-01;
      this->pydat1().parj(42)=1.003531e+00;
      this->pydat1().parj(47)=8.727703e-01;
      this->pydat1().parj(81)=2.564716e-01;
      this->pydat1().parj(82)=8.296215e-01;
      this->pypars().mstp(3)=1;
      this->pydat1().paru(112)=0.192;
      this->pypars().parp(1)=0.192;
      this->pypars().parp(61)=0.192;
      this->pypars().mstp(70)=0;
      this->pypars().parp(67)=1.00;
      this->pypars().parp(91)=2.00;
      this->pypars().parp(62)=1.26;
      this->pypars().parp(64)=1.11;
      this->pypars().parp(72)=0.49;
      this->pypars().parp(77)=0.597;
      this->pypars().parp(78)=0.371;
      this->pypars().parp(82)=1.99;
      this->pypars().parp(84)=0.499;
      this->pypars().parp(90)=0.266;
      ATH_MSG_INFO( "---------------------------------------------------------------------------------\n"
		    << "atlasTune: parameter setting to Tune_Index "<<m_Tune_Index<<" tune values :      \n"
		    << "-----------------------------------------------                                  \n"
		    << "mstp(128)=" << this->pypars().mstp(128) << "\n"
		    << "mstu(21)=" << this->pydat1().mstu(21)  << "\n"
		    << "mstp(81)=" << this->pypars().mstp(81)  << "\n"
		    << "mstp(82)=" << this->pypars().mstp(82) << "\n"
		    << "mstp(72)=" << this->pypars().mstp(72) << "\n"
		    << "mstp(88)=" << this->pypars().mstp(88) << "\n"
		    << "mstp(90)=" << this->pypars().mstp(90) << "\n"
		    << "parp(80)=" << this->pypars().parp(80) << "\n"
		    << "parp(83)=" << this->pypars().parp(83) << "\n"
		    << "parp(93)=" << this->pypars().parp(93) << "\n"
		    << "mstp(95)=" << this->pypars().mstp(95) << "\n"
		    << "mstj(22)=" << this->pydat1().mstj(22) << "\n"
		    << "parj(46)=" << this->pydat1().parj(46) << "\n"
		    << "mstp(84)=" << this->pypars().mstp(84) << "\n"
		    << "mstp(85)=" << this->pypars().mstp(85) << "\n"
		    << "mstp(86)=" << this->pypars().mstp(86) << "\n"
		    << "mstp(87)=" << this->pypars().mstp(87) << "\n"
		    << "mstp(89)=" << this->pypars().mstp(89) << "\n"
		    << "parp(89)=" << this->pypars().parp(89) << "\n"
		    << "pmas(6,1)=" << this->pydat2().pmas(6,1) << "\n"
		    << "pmas(24,1)=" << this->pydat2().pmas(24,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "mstp(51)=" << this->pypars().mstp(51) << "\n"
		    << "mstp(53)=" << this->pypars().mstp(53) << "\n"
		    << "mstp(55)=" << this->pypars().mstp(55) << "\n"
		    << "mstp(52)=" << this->pypars().mstp(52) << "\n"
		    << "mstp(54)=" << this->pypars().mstp(54) << "\n"
		    << "mstp(56)=" << this->pypars().mstp(56) << "\n"
		    << "mstj(11)=" << this->pydat1().mstj(11) << "\n"
		    << "parj(1)=" << this->pydat1().parj(1) << "\n"
		    << "parj(2)=" << this->pydat1().parj(2) << "\n"
		    << "parj(3)=" << this->pydat1().parj(3) << "\n"
		    << "parj(4)=" << this->pydat1().parj(4) << "\n"
		    << "parj(11)=" << this->pydat1().parj(11) << "\n"
		    << "parj(12)=" << this->pydat1().parj(12) << "\n"
		    << "parj(13)=" << this->pydat1().parj(13) << "\n"
		    << "parj(25)=" << this->pydat1().parj(25) << "\n"
		    << "parj(26)=" << this->pydat1().parj(26) << "\n"
		    << "parj(21)=" << this->pydat1().parj(21) << "\n"
		    << "parj(41)=" << this->pydat1().parj(41) << "\n"
		    << "parj(42)=" << this->pydat1().parj(42) << "\n"
		    << "parj(47)=" << this->pydat1().parj(47) << "\n"
		    << "parj(81)=" << this->pydat1().parj(81) << "\n"
		    << "parj(82)=" << this->pydat1().parj(82) << "\n"
		    << "mstp(3)=" << this->pypars().mstp(3) << "\n"
		    << "paru(112)=" << this->pydat1().paru(112) << "\n"
		    << "parp(1)=" << this->pypars().parp(1) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "mstp(70)=" << this->pypars().mstp(70) << "\n"
		    << "parp(67)=" << this->pypars().parp(67) << "\n"
		    << "parp(91)=" << this->pypars().parp(91) << "\n"
		    << "parp(62)=" << this->pypars().parp(62) << "\n"
		    << "parp(64)=" << this->pypars().parp(64) << "\n"
		    << "parp(72)=" << this->pypars().parp(72) << "\n"
		    << "parp(77)=" << this->pypars().parp(77) << "\n"
		    << "parp(78)=" << this->pypars().parp(78) << "\n"
		    << "parp(82)=" << this->pypars().parp(82) << "\n"
		    << "parp(84)=" << this->pypars().parp(84) << "\n"
		    << "parp(90)=" << this->pypars().parp(90) << "\n"
		    << "-----------------------------------------------                                 \n"
		    << "atlasTune: end of the list of Tune_Index "<<m_Tune_Index<<" tune values :         \n"
		    << "--------------------------------------------------------------------------------\n" );
    } // end of 20110003 (AUET2b) condition
  //==========================================================
  else if (m_Tune_Index==20110005)
    {
      // ATLAS jargon     : AUET2B-CTEQ66 
      // references       : todo : add
      // author           : A. Buckley (et. al)
      // Pythia           : 6.4.25 
      // impl. date       : Mar 2012
      // tune produced in : 2011
      // short description: Underlying Event Tune (CTEQ66 PDF)
      //_______________________________________________________________________________________________________________________
      this->pypars().mstp(128)=1;
      this->pydat1().mstu(21)=1;
      this->pypars().mstp(81)=21;
      this->pypars().mstp(82)=4;
      this->pypars().mstp(72)=1;
      this->pypars().mstp(88)=1;
      this->pypars().mstp(90)=0;
      this->pypars().parp(80)=0.1;
      this->pypars().parp(83)=0.356;
      this->pypars().parp(93)=10.0;
      this->pypars().mstp(95)=6;
      this->pydat1().mstj(22)=2;
      this->pydat1().parj(46)=0.75;
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(89)=1;
      this->pypars().parp(89)=1800.;
      this->pydat2().pmas(6,1)=172.5;
      this->pydat2().pmas(24,1)=80.399;
      this->pydat2().pmas(23,1)=91.1876;
      this->pypars().mstp(51)=10550;
      this->pypars().mstp(53)=10550;
      this->pypars().mstp(55)=10550;
      this->pypars().mstp(52)=2;
      this->pypars().mstp(54)=2;
      this->pypars().mstp(56)=2;
      this->pydat1().mstj(11)=5;
      this->pydat1().parj(1)=7.272809e-02;
      this->pydat1().parj(2)=2.018845e-01;
      this->pydat1().parj(3)=9.498471e-01;
      this->pydat1().parj(4)=3.316182e-02;
      this->pydat1().parj(11)=3.089764e-01;
      this->pydat1().parj(12)=4.015396e-01;
      this->pydat1().parj(13)=5.442874e-01;
      this->pydat1().parj(25)=6.276964e-01;
      this->pydat1().parj(26)=1.292377e-01;
      this->pydat1().parj(21)=3.001463e-01;
      this->pydat1().parj(41)=3.683123e-01;
      this->pydat1().parj(42)=1.003531e+00;
      this->pydat1().parj(47)=8.727703e-01;
      this->pydat1().parj(81)=2.564716e-01;
      this->pydat1().parj(82)=8.296215e-01;
      this->pypars().mstp(3)=1;
      this->pydat1().paru(112)=0.192;
      this->pypars().parp(1)=0.192;
      this->pypars().parp(61)=0.192;
      this->pypars().mstp(70)=0;
      this->pypars().parp(67)=1.00;
      this->pypars().parp(91)=2.00;
      this->pypars().parp(62)=0.95;
      this->pypars().parp(64)=1.03;
      this->pypars().parp(72)=0.40;
      this->pypars().parp(77)=0.428;
      this->pypars().parp(78)=0.281;
      this->pypars().parp(82)=1.892;
      this->pypars().parp(84)=0.388;
      this->pypars().parp(90)=0.211;
      ATH_MSG_INFO( "---------------------------------------------------------------------------------\n"
		    << "atlasTune: parameter setting to Tune_Index "<<m_Tune_Index<<" tune values :      \n"
		    << "-----------------------------------------------                                  \n"
		    << "mstp(128)=" << this->pypars().mstp(128) << "\n"
		    << "mstu(21)=" << this->pydat1().mstu(21)  << "\n"
		    << "mstp(81)=" << this->pypars().mstp(81)  << "\n"
		    << "mstp(82)=" << this->pypars().mstp(82) << "\n"
		    << "mstp(72)=" << this->pypars().mstp(72) << "\n"
		    << "mstp(88)=" << this->pypars().mstp(88) << "\n"
		    << "mstp(90)=" << this->pypars().mstp(90) << "\n"
		    << "parp(80)=" << this->pypars().parp(80) << "\n"
		    << "parp(83)=" << this->pypars().parp(83) << "\n"
		    << "parp(93)=" << this->pypars().parp(93) << "\n"
		    << "mstp(95)=" << this->pypars().mstp(95) << "\n"
		    << "mstj(22)=" << this->pydat1().mstj(22) << "\n"
		    << "parj(46)=" << this->pydat1().parj(46) << "\n"
		    << "mstp(84)=" << this->pypars().mstp(84) << "\n"
		    << "mstp(85)=" << this->pypars().mstp(85) << "\n"
		    << "mstp(86)=" << this->pypars().mstp(86) << "\n"
		    << "mstp(87)=" << this->pypars().mstp(87) << "\n"
		    << "mstp(89)=" << this->pypars().mstp(89) << "\n"
		    << "parp(89)=" << this->pypars().parp(89) << "\n"
		    << "pmas(6,1)=" << this->pydat2().pmas(6,1) << "\n"
		    << "pmas(24,1)=" << this->pydat2().pmas(24,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "mstp(51)=" << this->pypars().mstp(51) << "\n"
		    << "mstp(53)=" << this->pypars().mstp(53) << "\n"
		    << "mstp(55)=" << this->pypars().mstp(55) << "\n"
		    << "mstp(52)=" << this->pypars().mstp(52) << "\n"
		    << "mstp(54)=" << this->pypars().mstp(54) << "\n"
		    << "mstp(56)=" << this->pypars().mstp(56) << "\n"
		    << "mstj(11)=" << this->pydat1().mstj(11) << "\n"
		    << "parj(1)=" << this->pydat1().parj(1) << "\n"
		    << "parj(2)=" << this->pydat1().parj(2) << "\n"
		    << "parj(3)=" << this->pydat1().parj(3) << "\n"
		    << "parj(4)=" << this->pydat1().parj(4) << "\n"
		    << "parj(11)=" << this->pydat1().parj(11) << "\n"
		    << "parj(12)=" << this->pydat1().parj(12) << "\n"
		    << "parj(13)=" << this->pydat1().parj(13) << "\n"
		    << "parj(25)=" << this->pydat1().parj(25) << "\n"
		    << "parj(26)=" << this->pydat1().parj(26) << "\n"
		    << "parj(21)=" << this->pydat1().parj(21) << "\n"
		    << "parj(41)=" << this->pydat1().parj(41) << "\n"
		    << "parj(42)=" << this->pydat1().parj(42) << "\n"
		    << "parj(47)=" << this->pydat1().parj(47) << "\n"
		    << "parj(81)=" << this->pydat1().parj(81) << "\n"
		    << "parj(82)=" << this->pydat1().parj(82) << "\n"
		    << "mstp(3)=" << this->pypars().mstp(3) << "\n"
		    << "paru(112)=" << this->pydat1().paru(112) << "\n"
		    << "parp(1)=" << this->pypars().parp(1) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "mstp(70)=" << this->pypars().mstp(70) << "\n"
		    << "parp(67)=" << this->pypars().parp(67) << "\n"
		    << "parp(91)=" << this->pypars().parp(91) << "\n"
		    << "parp(62)=" << this->pypars().parp(62) << "\n"
		    << "parp(64)=" << this->pypars().parp(64) << "\n"
		    << "parp(72)=" << this->pypars().parp(72) << "\n"
		    << "parp(77)=" << this->pypars().parp(77) << "\n"
		    << "parp(78)=" << this->pypars().parp(78) << "\n"
		    << "parp(82)=" << this->pypars().parp(82) << "\n"
		    << "parp(84)=" << this->pypars().parp(84) << "\n"
		    << "parp(90)=" << this->pypars().parp(90) << "\n"
		    << "-----------------------------------------------                                 \n"
		    << "atlasTune: end of the list of Tune_Index "<<m_Tune_Index<<" tune values :         \n"
		    << "--------------------------------------------------------------------------------\n" );
    } // end of 20110005 (AUET2B-CTEQ66) condition
  else if (m_Tune_Index==20110006)
    {
      // ATLAS jargon     : AUET2B-CT10 
      // references       : todo : add
      // author           : A. Buckley (et. al)
      // Pythia           : 6.4.25 
      // impl. date       : Mar 2012
      // tune produced in : 2011
      // short description: Underlying Event Tune (CT10 PDF)
      //_______________________________________________________________________________________________________________________
      this->pypars().mstp(128)=1;
      this->pydat1().mstu(21)=1;
      this->pypars().mstp(81)=21;
      this->pypars().mstp(82)=4;
      this->pypars().mstp(72)=1;
      this->pypars().mstp(88)=1;
      this->pypars().mstp(90)=0;
      this->pypars().parp(80)=0.1;
      this->pypars().parp(83)=0.356;
      this->pypars().parp(93)=10.0;
      this->pypars().mstp(95)=6;
      this->pydat1().mstj(22)=2;
      this->pydat1().parj(46)=0.75;
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(89)=1;
      this->pypars().parp(89)=1800.;
      this->pydat2().pmas(6,1)=172.5;
      this->pydat2().pmas(24,1)=80.399;
      this->pydat2().pmas(23,1)=91.1876;
      this->pypars().mstp(51)=10800;
      this->pypars().mstp(53)=10800;
      this->pypars().mstp(55)=10800;
      this->pypars().mstp(52)=2;
      this->pypars().mstp(54)=2;
      this->pypars().mstp(56)=2;
      this->pydat1().mstj(11)=5;
      this->pydat1().parj(1)=7.272809e-02;
      this->pydat1().parj(2)=2.018845e-01;
      this->pydat1().parj(3)=9.498471e-01;
      this->pydat1().parj(4)=3.316182e-02;
      this->pydat1().parj(11)=3.089764e-01;
      this->pydat1().parj(12)=4.015396e-01;
      this->pydat1().parj(13)=5.442874e-01;
      this->pydat1().parj(25)=6.276964e-01;
      this->pydat1().parj(26)=1.292377e-01;
      this->pydat1().parj(21)=3.001463e-01;
      this->pydat1().parj(41)=3.683123e-01;
      this->pydat1().parj(42)=1.003531e+00;
      this->pydat1().parj(47)=8.727703e-01;
      this->pydat1().parj(81)=2.564716e-01;
      this->pydat1().parj(82)=8.296215e-01;
      this->pypars().mstp(3)=1;
      this->pydat1().paru(112)=0.192;
      this->pypars().parp(1)=0.192;
      this->pypars().parp(61)=0.192;
      this->pypars().mstp(70)=0;
      this->pypars().parp(67)=1.00;
      this->pypars().parp(91)=2.00;
      this->pypars().parp(62)=0.312;
      this->pypars().parp(64)=0.939;
      this->pypars().parp(72)=0.537;
      this->pypars().parp(77)=0.125;
      this->pypars().parp(78)=0.309;
      this->pypars().parp(82)=1.89;
      this->pypars().parp(84)=0.415;
      this->pypars().parp(90)=0.182;
      ATH_MSG_INFO( "---------------------------------------------------------------------------------\n"
		    << "atlasTune: parameter setting to Tune_Index "<<m_Tune_Index<<" tune values :      \n"
		    << "-----------------------------------------------                                  \n"
		    << "mstp(128)=" << this->pypars().mstp(128) << "\n"
		    << "mstu(21)=" << this->pydat1().mstu(21)  << "\n"
		    << "mstp(81)=" << this->pypars().mstp(81)  << "\n"
		    << "mstp(82)=" << this->pypars().mstp(82) << "\n"
		    << "mstp(72)=" << this->pypars().mstp(72) << "\n"
		    << "mstp(88)=" << this->pypars().mstp(88) << "\n"
		    << "mstp(90)=" << this->pypars().mstp(90) << "\n"
		    << "parp(80)=" << this->pypars().parp(80) << "\n"
		    << "parp(83)=" << this->pypars().parp(83) << "\n"
		    << "parp(93)=" << this->pypars().parp(93) << "\n"
		    << "mstp(95)=" << this->pypars().mstp(95) << "\n"
		    << "mstj(22)=" << this->pydat1().mstj(22) << "\n"
		    << "parj(46)=" << this->pydat1().parj(46) << "\n"
		    << "mstp(84)=" << this->pypars().mstp(84) << "\n"
		    << "mstp(85)=" << this->pypars().mstp(85) << "\n"
		    << "mstp(86)=" << this->pypars().mstp(86) << "\n"
		    << "mstp(87)=" << this->pypars().mstp(87) << "\n"
		    << "mstp(89)=" << this->pypars().mstp(89) << "\n"
		    << "parp(89)=" << this->pypars().parp(89) << "\n"
		    << "pmas(6,1)=" << this->pydat2().pmas(6,1) << "\n"
		    << "pmas(24,1)=" << this->pydat2().pmas(24,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "mstp(51)=" << this->pypars().mstp(51) << "\n"
		    << "mstp(53)=" << this->pypars().mstp(53) << "\n"
		    << "mstp(55)=" << this->pypars().mstp(55) << "\n"
		    << "mstp(52)=" << this->pypars().mstp(52) << "\n"
		    << "mstp(54)=" << this->pypars().mstp(54) << "\n"
		    << "mstp(56)=" << this->pypars().mstp(56) << "\n"
		    << "mstj(11)=" << this->pydat1().mstj(11) << "\n"
		    << "parj(1)=" << this->pydat1().parj(1) << "\n"
		    << "parj(2)=" << this->pydat1().parj(2) << "\n"
		    << "parj(3)=" << this->pydat1().parj(3) << "\n"
		    << "parj(4)=" << this->pydat1().parj(4) << "\n"
		    << "parj(11)=" << this->pydat1().parj(11) << "\n"
		    << "parj(12)=" << this->pydat1().parj(12) << "\n"
		    << "parj(13)=" << this->pydat1().parj(13) << "\n"
		    << "parj(25)=" << this->pydat1().parj(25) << "\n"
		    << "parj(26)=" << this->pydat1().parj(26) << "\n"
		    << "parj(21)=" << this->pydat1().parj(21) << "\n"
		    << "parj(41)=" << this->pydat1().parj(41) << "\n"
		    << "parj(42)=" << this->pydat1().parj(42) << "\n"
		    << "parj(47)=" << this->pydat1().parj(47) << "\n"
		    << "parj(81)=" << this->pydat1().parj(81) << "\n"
		    << "parj(82)=" << this->pydat1().parj(82) << "\n"
		    << "mstp(3)=" << this->pypars().mstp(3) << "\n"
		    << "paru(112)=" << this->pydat1().paru(112) << "\n"
		    << "parp(1)=" << this->pypars().parp(1) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "mstp(70)=" << this->pypars().mstp(70) << "\n"
		    << "parp(67)=" << this->pypars().parp(67) << "\n"
		    << "parp(91)=" << this->pypars().parp(91) << "\n"
		    << "parp(62)=" << this->pypars().parp(62) << "\n"
		    << "parp(64)=" << this->pypars().parp(64) << "\n"
		    << "parp(72)=" << this->pypars().parp(72) << "\n"
		    << "parp(77)=" << this->pypars().parp(77) << "\n"
		    << "parp(78)=" << this->pypars().parp(78) << "\n"
		    << "parp(82)=" << this->pypars().parp(82) << "\n"
		    << "parp(84)=" << this->pypars().parp(84) << "\n"
		    << "parp(90)=" << this->pypars().parp(90) << "\n"
		    << "-----------------------------------------------                                 \n"
		    << "atlasTune: end of the list of Tune_Index "<<m_Tune_Index<<" tune values :         \n"
		    << "--------------------------------------------------------------------------------\n" );
    } // end of 20110006 (AUET2B-CT10) condition
  else if (m_Tune_Index==20110103)
    {
      // ATLAS jargon     : AMBT2b
      // references       : todo : add
      // author           : A. Buckley, J. Katzy (et. al)
      // Pythia           : 6.4.23 
      // impl. date       : Apr 2011
      // tune produced in : 2011
      // short description: CTEQ6L1 tune incl. ISR/FSR parameters, optimized for MinBias
      //_______________________________________________________________________________________________________________________
      this->pypars().mstp(128)=1;
      this->pydat1().mstu(21)=1;
      this->pypars().mstp(81)=21;
      this->pypars().mstp(82)=4;
      this->pypars().mstp(72)=1;
      this->pypars().mstp(88)=1;
      this->pypars().mstp(90)=0;
      this->pypars().parp(80)=0.1;
      this->pypars().parp(83)=0.356;
      this->pypars().parp(93)=10.0;
      this->pypars().mstp(95)=6;
      this->pydat1().mstj(22)=2;
      this->pydat1().parj(46)=0.75;
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(89)=1;
      this->pypars().parp(89)=1800.;
      this->pydat2().pmas(6,1)=172.5;
      this->pydat2().pmas(24,1)=80.399;
      this->pydat2().pmas(23,1)=91.1876;
      this->pypars().mstp(51)=10042;
      this->pypars().mstp(53)=10042;
      this->pypars().mstp(55)=10042;
      this->pypars().mstp(52)=2;
      this->pypars().mstp(54)=2;
      this->pypars().mstp(56)=2;
      this->pydat1().mstj(11)=5;
      this->pydat1().parj(1)=7.272809e-02;
      this->pydat1().parj(2)=2.018845e-01;
      this->pydat1().parj(3)=9.498471e-01;
      this->pydat1().parj(4)=3.316182e-02;
      this->pydat1().parj(11)=3.089764e-01;
      this->pydat1().parj(12)=4.015396e-01;
      this->pydat1().parj(13)=5.442874e-01;
      this->pydat1().parj(25)=6.276964e-01;
      this->pydat1().parj(26)=1.292377e-01;
      this->pydat1().parj(21)=3.001463e-01;
      this->pydat1().parj(41)=3.683123e-01;
      this->pydat1().parj(42)=1.003531e+00;
      this->pydat1().parj(47)=8.727703e-01;
      this->pydat1().parj(81)=2.564716e-01;
      this->pydat1().parj(82)=8.296215e-01;
      this->pypars().mstp(3)=1;
      this->pydat1().paru(112)=0.192;
      this->pypars().parp(1)=0.192;
      this->pypars().parp(61)=0.192;
      this->pypars().mstp(70)=0;
      this->pypars().parp(67)=1.00;
      this->pypars().parp(91)=2.00;
      this->pypars().parp(62)=1.13;
      this->pypars().parp(64)=0.68;
      this->pypars().parp(72)=0.527;
      this->pypars().parp(77)=3.566023e-01;
      this->pypars().parp(78)=2.350777e-01;
      this->pypars().parp(82)=2.342685e+00;
      this->pypars().parp(84)=6.053474e-01;
      this->pypars().parp(90)=2.456730e-01;
      ATH_MSG_INFO( "---------------------------------------------------------------------------------\n"
		    << "atlasTune: parameter setting to Tune_Index "<<m_Tune_Index<<" tune values :      \n"
		    << "-----------------------------------------------                                  \n"
		    << "mstp(128)=" << this->pypars().mstp(128) << "\n"
		    << "mstu(21)=" << this->pydat1().mstu(21)  << "\n"
		    << "mstp(81)=" << this->pypars().mstp(81)  << "\n"
		    << "mstp(82)=" << this->pypars().mstp(82) << "\n"
		    << "mstp(72)=" << this->pypars().mstp(72) << "\n"
		    << "mstp(88)=" << this->pypars().mstp(88) << "\n"
		    << "mstp(90)=" << this->pypars().mstp(90) << "\n"
		    << "parp(80)=" << this->pypars().parp(80) << "\n"
		    << "parp(83)=" << this->pypars().parp(83) << "\n"
		    << "parp(93)=" << this->pypars().parp(93) << "\n"
		    << "mstp(95)=" << this->pypars().mstp(95) << "\n"
		    << "mstj(22)=" << this->pydat1().mstj(22) << "\n"
		    << "parj(46)=" << this->pydat1().parj(46) << "\n"
		    << "mstp(84)=" << this->pypars().mstp(84) << "\n"
		    << "mstp(85)=" << this->pypars().mstp(85) << "\n"
		    << "mstp(86)=" << this->pypars().mstp(86) << "\n"
		    << "mstp(87)=" << this->pypars().mstp(87) << "\n"
		    << "mstp(89)=" << this->pypars().mstp(89) << "\n"
		    << "parp(89)=" << this->pypars().parp(89) << "\n"
		    << "pmas(6,1)=" << this->pydat2().pmas(6,1) << "\n"
		    << "pmas(24,1)=" << this->pydat2().pmas(24,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "pmas(23,1)=" << this->pydat2().pmas(23,1) << "\n"
		    << "mstp(51)=" << this->pypars().mstp(51) << "\n"
		    << "mstp(53)=" << this->pypars().mstp(53) << "\n"
		    << "mstp(55)=" << this->pypars().mstp(55) << "\n"
		    << "mstp(52)=" << this->pypars().mstp(52) << "\n"
		    << "mstp(54)=" << this->pypars().mstp(54) << "\n"
		    << "mstp(56)=" << this->pypars().mstp(56) << "\n"
		    << "mstj(11)=" << this->pydat1().mstj(11) << "\n"
		    << "parj(1)=" << this->pydat1().parj(1) << "\n"
		    << "parj(2)=" << this->pydat1().parj(2) << "\n"
		    << "parj(3)=" << this->pydat1().parj(3) << "\n"
		    << "parj(4)=" << this->pydat1().parj(4) << "\n"
		    << "parj(11)=" << this->pydat1().parj(11) << "\n"
		    << "parj(12)=" << this->pydat1().parj(12) << "\n"
		    << "parj(13)=" << this->pydat1().parj(13) << "\n"
		    << "parj(25)=" << this->pydat1().parj(25) << "\n"
		    << "parj(26)=" << this->pydat1().parj(26) << "\n"
		    << "parj(21)=" << this->pydat1().parj(21) << "\n"
		    << "parj(41)=" << this->pydat1().parj(41) << "\n"
		    << "parj(42)=" << this->pydat1().parj(42) << "\n"
		    << "parj(47)=" << this->pydat1().parj(47) << "\n"
		    << "parj(81)=" << this->pydat1().parj(81) << "\n"
		    << "parj(82)=" << this->pydat1().parj(82) << "\n"
		    << "mstp(3)=" << this->pypars().mstp(3) << "\n"
		    << "paru(112)=" << this->pydat1().paru(112) << "\n"
		    << "parp(1)=" << this->pypars().parp(1) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "parp(61)=" << this->pypars().parp(61) << "\n"
		    << "mstp(70)=" << this->pypars().mstp(70) << "\n"
		    << "parp(67)=" << this->pypars().parp(67) << "\n"
		    << "parp(91)=" << this->pypars().parp(91) << "\n"
		    << "parp(62)=" << this->pypars().parp(62) << "\n"
		    << "parp(64)=" << this->pypars().parp(64) << "\n"
		    << "parp(72)=" << this->pypars().parp(72) << "\n"
		    << "parp(77)=" << this->pypars().parp(77) << "\n"
		    << "parp(78)=" << this->pypars().parp(78) << "\n"
		    << "parp(82)=" << this->pypars().parp(82) << "\n"
		    << "parp(84)=" << this->pypars().parp(84) << "\n"
		    << "parp(90)=" << this->pypars().parp(90) << "\n"
		    << "-----------------------------------------------                                 \n"
		    << "atlasTune: end of the list of Tune_Index "<<m_Tune_Index<<" tune values :         \n"
		    << "--------------------------------------------------------------------------------\n" );
    } // end of 20110103 (AMBT2b) condition
  else
    {
      ATH_MSG_WARNING( "atlasTune: no tune with Tune_Index : "<<m_Tune_Index << "\n"
		       << "run with current default ATLAS tune : "<<m_Default_Tune_Name << "\n" );
      //set Tune_Type to empty to get common undefined tune behavior
      m_Tune_Type=" ";
      StatusCode sc = Pythia::setPythiaTune();
      if ( !sc.isSuccess() ) { 
	ATH_MSG_WARNING( "StatusCode in setPythiaTune: Failed \n");
      }
    }

  ATH_MSG_INFO( "atlasTune: all done                                                             \n" 
		<< "________________________________________________________________________________\n" );
  return StatusCode::SUCCESS;
  
  //end of atlasTune def.
}

