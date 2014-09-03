/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCELLLINK_H
#define CALOEVENT_CALOCELLLINK_H



#include "Navigation/Navigable.h"

#include "CaloEvent/CaloCellContainer.h"

class CaloCellLink : public Navigable<CaloCellContainer,double>
{
 public:

  /*! \brief Constructor */
  CaloCellLink();
  /*! \brief Copy constructor */
  explicit CaloCellLink(const CaloCellLink* pLink);
  explicit CaloCellLink(const CaloCellLink& rLink);  
  /// Assignment.
  CaloCellLink& operator= (const CaloCellLink& rLink);
  /*! \brief Destructor */
  virtual ~CaloCellLink();

  /*! \brief Forward internal iterator type to clients
   * 
   *  Type forwarding hides concrete store interator type and protects 
   *  clients against future changes. 
   */
  typedef Navigable<CaloCellContainer,double>::object_iter  cell_iterator;

 private:

  void copyStore(const CaloCellLink* pLink);

};

/*! \class CaloCellLink
 *
 *  \brief Simple store for \a CaloCell links. This object is fully navigable. 
 */
#endif
 
