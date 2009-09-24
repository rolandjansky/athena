/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLEDATA_EXAMPLECLASS_H
#define ATHENAPOOLEXAMPLEDATA_EXAMPLECLASS_H

/** @file ExampleClass.h
 *  @brief This file contains the class definition for the ExampleClass class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ExampleClass.h,v 1.8 2007-10-01 15:28:03 gemmeren Exp $
 **/

#include <string>

/** @class ExampleClass
 *  @brief This class provides an example data object for AthenaPool.
 **/
class ExampleClass {

public: // Constructor and Destructor
   /// Default Constructor
   ExampleClass() : m_run(0), m_event(0), m_text("") {}
   /// Destructor
   virtual ~ExampleClass() {}

public: // Non-static members
   /// @return the run number.
   int getRun() const { return(m_run); }

   /// @return the event number.
   int getEvent() const { return(m_event); }

   /// @return the text string.
   const std::string& getText() const { return(m_text); }

   /// Set the run number.
   /// @param run [IN] run number.
   void setRun(int run) { m_run = run; }

   /// Set the event number.
   /// @param event [IN] event number.
   void setEvent(int event) { m_event = event; }

   /// Set the text string.
   /// @param text [IN] text string.
   void setText(std::string text) { m_text = text; }

private:
   int m_run;
   int m_event;
   std::string m_text;
};
#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(ExampleClass, 67289510, 1)
#endif
