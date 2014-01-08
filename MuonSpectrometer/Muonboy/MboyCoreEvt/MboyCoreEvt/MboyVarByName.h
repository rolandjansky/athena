/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyVarByName_H
#define MboyVarByName_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

  /**
   @class MboyVarByName

   This class is a class to associate name and value 
   of a set of a configuration variables

  @author samusog@cern.ch
  
  */

class MboyVarByName{
public:
    MboyVarByName();
   ~MboyVarByName();

public:
///////////////////////////////////

   /**Reset the set */
   void   Reset();

   /**Define a variable with 2 names and associated comment*/
   void   Define(std::string Name1,std::string Name2,std::string Comment);

   /**Define a variable with 2 names, associated comment and a default value*/
   void   Define(std::string Name1,std::string Name2,std::string Comment, double Value);

   /**Get size of the set*/
   int   Size() const ;

   /**Get first  name attached to variable associated to index Index*/
   std::string GetName1(int Index)   const ;

   /**Get second name attached to variable associated to index Index*/
   std::string GetName2(int Index)   const ;

   /**Get comment     attached to variable associated to index Index*/
   std::string GetComment(int Index) const ;

   /**Get value       attached to variable associated to index Index*/
   double      GetValue(int Index)   const ;

   /**Set value       attached to variable associated to index Index*/
   void   SetValue(int Index, double Value);

private:
///////////////////////////////////

  std::vector<std::string>  m_Name1Set  ; //!< Storage vector for first  name
  std::vector<std::string>  m_Name2Set  ; //!< Storage vector for second name
  std::vector<std::string>  m_CommentSet; //!< Storage vector for comment 
  std::vector<double>       m_ValueSet  ; //!< Storage vector for value

};

#endif
