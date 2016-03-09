/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia_i/Pythia.h"

//LM remove these
/*#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <math.h>
#include <sstream>*/

/*
  ________________________________________________________________________________________________________________________

  L.M., Aug 09

  Pythia TUNE choice 

  tune is unambigously determined by jO property m_Tune_Name=TUNETYPE_TUNEINDEX
  that is parsed (via separator '_')so that m_Tune_Type=TUNETYPE and m_Tune_Index=TUNEINDEX

  m_Tune_Type options:
  * m_Tune_Type="ATLAS" :  ATLAS tunes (implemented in atlasTune.cxx )
  * m_Tune_Type="PYTUNE" : PYTUNE tunes (apart from MSTP(5) ATLAS defaults are turned off and necessary Pythia params for running
                                        within Athena are set (event record etc.) are set. Handling a non-defined tune in this
                                        case  is up to Pythia.)
  * else: run with the default tune m_Default_Tune_Name

  Apart from tune setting:
  - common printout (for each Pythia run within Athena) is set 
  ________________________________________________________________________________________________________________________
 */

StatusCode Pythia::setPythiaTune()
{
  // count # of function calls for one initialization step (inf. loop protection)
   m_atlasTune_loop_prot++;

   //protect against looping; setPythiaTune calls atlasTune so in principle an infinite loop can occur (in case of core code blunder)
   if (m_atlasTune_loop_prot>100)
     {
       ATH_MSG_FATAL( "Got more than 100 calls of setPythiaTune() -> code errors, exit.");
       return StatusCode::FAILURE;       
     }
       
  //  MsgStream log(messageService(), name());

  //always set printout details
  this->m_initlistlevel=12;
  this->m_eventlistlevel=1;
  this->m_pystatlistlevel.clear();
  this->m_pystatlistlevel.push_back(1);
  this->m_pystatlistlevel.push_back(3);
  this->m_pystatlistlevel.push_back(4);
  this->m_pystatlistlevel.push_back(5);
  this->m_firstlistevent=1;
  this->m_lastlistevent=5;

  // in case of consistent old syntax : handle it, else pring warnings and use ATLAS defaults
  if (m_useAtlasPythiaRecomm != m_Default_useAtlasPythiaRecomm || m_useAtlasPythiaCommon != m_Default_useAtlasPythiaCommon || m_useAtlasPythiaTune09 != m_Default_useAtlasPythiaTune09 || m_useAtlasPythiaTune08 != m_Default_useAtlasPythiaTune08 || m_useNoAtlasPythiaParam != m_Default_useNoAtlasPythiaParam)
    {
      //be backward compatible if the tune has not been set using the new syntax:
      if (m_Tune_Name==m_Starting_Tune_Name)
	{
	  {
	    if (m_useNoAtlasPythiaParam)
	      {
		// using no ATLAS defaults
		m_Tune_Index=0;
	      }
	    // set m_Tune_Index acc. to the switch, overrides taken into acc.
	    if (m_useAtlasPythiaCommon==false) m_Tune_Index=-1;
	    if (m_useAtlasPythiaRecomm==false) m_Tune_Index=0;
	    if (m_useAtlasPythiaTune08) m_Tune_Index=m_mc08_Tune_Index;
	    if (m_useAtlasPythiaTune09) m_Tune_Index=m_mc09_Tune_Index;
	  }
	  // atlasTune is Pythia cmember, so knows about m_Tune_Index
	  StatusCode sc=atlasTune();      
	  if ( sc.isFailure() ) 
	    {
	      ATH_MSG_FATAL( "ERROR returned from atlasTune, exit." );
	      return StatusCode::FAILURE;
	    }
	  else 
	    { 
	      //setPythiaTune done, return
	      return StatusCode::SUCCESS;
	    }
	}
      //print warnings for the mix of the old and new syntax:
      else
	{
	  //mix of the old and new syntax
	  ATH_MSG_WARNING( "Joboptions contain both new-style tune setting Pythia.Tune_Name=\"TUNETYPE_TUNEINDEX\" as well as setting of the old-style switches:" );
	  ATH_MSG_WARNING( "useAtlasPythiaRecomm,useAtlasPythiaCommon,useAtlasPythiaTune09,useAtlasPythiaTune08 or useNoAtlasPythiaParam" );
	  ATH_MSG_WARNING( "Please fix the jo so that only one tune-setting style is used." );
	  ATH_MSG_WARNING( "The old switches are deprecated, the new tune-setting style is preffered." );
	  ATH_MSG_WARNING( "For this run the tune settings are ignored and  ATLAS default tune settings will be used:");
	  m_Tune_Type=m_Default_Tune_Type; m_Tune_Index=m_Default_Tune_Index;
	  ATH_MSG_WARNING( m_Tune_Type <<" , "<< m_Tune_Index );	  
	}      
    }// end of old switches backward compatibility support

  if (m_Tune_Type=="ATLAS")
    {  
      StatusCode sc=atlasTune();      
      if ( sc.isFailure() ) 
	{
	  ATH_MSG_FATAL( "ERROR returned from atlasTune, exit." );
	  return StatusCode::FAILURE;
	}
      else 
	{ 
	  //setPythiaTune done, return
	  return StatusCode::SUCCESS;
	}  
    }
  // call PYTUNE in the original PYTUNE(MSTP(5)) form; 
  // as for Pythia 6.4.21 numbers 1xx - 4xx are in use
  // error checking is as handeled by Pythia in particular too large tune nbr. returns
  // (PYTUNE:) Tune number > max. Using defaults.
  else if (m_Tune_Type=="PYTUNE")
    {
      //setPythiaTune done, return
      this->pypars().mstp(5)=m_Tune_Index;
      //set what is necessary for correct use of Pythia within Athena:
      //-----------------------------------------------------------------------------
      this->pypars().mstp(128)  =1;      // fix junk output for documentary particles 	 
      this->pydat1().mstu(21)   =1;      // error handling switch 
      this->pydat1().mstj(22)=2;         // (Make K_S, Lambda stable)
      //-----------------------------------------------------------------------------
      ATH_MSG_INFO( "setPythiaTune: using Pythia internal tune via PYTUNE(" << m_Tune_Index <<") call.");
      ATH_MSG_INFO( "setting parameters needed for desired functioning of Pythia within Athena        ");
      ATH_MSG_INFO( " mstp(128)  =1;      fix junk output for documentary particles ");
      ATH_MSG_INFO( " mstu(21)   =1;      error handling switch                     ");
      ATH_MSG_INFO( " mstj(22)   =2;      (Make K_S, Lambda stable)                 ");
      ATH_MSG_INFO( " Please note: ");
      ATH_MSG_INFO( " 1. in the jO overrides of the parameters set by PYTUNE # "<< m_Tune_Index <<" will have no effect " );
      ATH_MSG_INFO( " - this correspods to (standalone) Pythia behavior "  );
      ATH_MSG_INFO( " 2. handling of non-existing PYTUNE tune is same as for (default) Pythia standalone." );
     //setPythiaTune done, return
      return StatusCode::SUCCESS;
    }
  //-----------------------------------------------------------------------------------------
  else
    {
      //undefined tune Type (setPythiaTune()) or Index (atlasTune() points here) for Type != PYTUNE 
      //Pythia_i reaction : run with current ATLAS default tune && print WARNINGs:
      ATH_MSG_WARNING("setPythiaTune: got invalid Tune_Name : "<< m_Tune_Name );
      ATH_MSG_WARNING("Pythia will run with current ATLAS default tune : "<<m_Default_Tune_Name ); 
      //string parsing for  m_Default_Tune_Name
      std::string::size_type sep_index_def = m_Default_Tune_Name.find_first_of('_');
      if (sep_index_def != std::string::npos)
	{
	  m_Tune_Type=m_Default_Tune_Name.substr(0,sep_index_def);
	  m_Tune_Index=atoi((m_Default_Tune_Name.substr(sep_index_def+1,m_Default_Tune_Name.length()-1)).c_str());
        }
      else
        {
          // this should never happen
	  ATH_MSG_FATAL ( "setPythiaTune : got invalid m_Default_Tune_Name, Pythia_i code needs to be updated." );
	  return StatusCode::FAILURE;
        }
      //call ATLAS tune
      StatusCode sc=atlasTune();      
      if ( sc.isFailure() ) 
	{
	  ATH_MSG_FATAL ("ERROR returned from atlasTune, exit." );
	  return StatusCode::FAILURE;
	}
      else 
	{ 
	  //setPythiaTune done, return
	  return StatusCode::SUCCESS;
	}        
    }
  //-----------------------------------------------------------------------------------------

}//end of setPythiaTune definition
