/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EGPID_H
#define EGAMMAEVENT_EGPID_H
/**
  @class egPID
          Class to hold PID vars egamma information
   @author K. Cranmer, S. Rajagopalan, D. Rousseau, D. Zerwas
*/

/********************************************************************

NAME:     egPID.h
PACKAGE:  offline/Reconstruction/egammaRec

AUTHORS:  K. Cranmer, S. Rajagopalan, D. Rousseau, D. Zerwas
CREATED:  Sept 15. 2006

PURPOSE:  Class to hold PID vars egamma information.  
UPDATED:
          Nov 24, 2009 (FD) use unsigned int
********************************************************************/

#include "egammaEvent/egammaPIDdefs.h"
#include <vector>
#include <cstddef>

class egPID

{
 public:

  /** @brief Default constructor*/
  egPID();
  /** @brief Destructor*/
  ~egPID();
  /** @brief constructor*/
  egPID(const egPID& original);   // copy constructor
  /// Assignment.
  egPID& operator= (const egPID& original);
  /** @brief Metod to define isEM variable */
  unsigned int isEM(const unsigned int mask = egammaPID::ALL, 
		    egammaPID::PID = egammaPID::IsEM, 
		    bool *found = NULL) const;
  /** @brief Method to define isEMse variable */
  unsigned int isEMsofte(const unsigned int mask = egammaPID::ALL, bool *found = NULL) const;
  /** @brief Method to define Object quality variable */
  unsigned int IsGoodOQ(const unsigned int mask = egammaPID::ALLOQ, bool *found = NULL) const;
  
  /** @brief retrieve egamma ID, as double to work for IsEM 
      and all possible weights as likelihood; 
      found, if not NULL, is set to true if found */
  double egammaID(egammaPID::PID, bool *found = NULL) const;
  /** @brief set egamma ID, as double to work double values 
      and all possible weights as likelihood */
  bool set_egammaID(egammaPID::PID, double);

  /** @brief set egamma ID, for unsigned int values */
  bool set_egammaIDint(egammaPID::PID, unsigned int);

 private:
  /** @brief */
  std::vector< std::pair<egammaPID::PID, unsigned int> > m_egammaIDint;
  /** @brief */
  std::vector< std::pair<egammaPID::PID,double> > m_egammaID;

  /** @brief retrieve unsinged int value */
  unsigned int egammaIDint(egammaPID::PID, bool *found) const;

};



#endif
