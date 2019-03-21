/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         Range.h  -  description
                            -------------------
   begin                : 28/02/2002
   email                : e.moyse@qmul.ac.uk
***************************************************************************/

#ifndef TRIGT1INTERFACES_RANGE_H
#define TRIGT1INTERFACES_RANGE_H

namespace LVL1 {

  /**Range class declaration*/
  class Range {

  public:
    Range() ;
    Range( double min, double max );
    ~Range() = default;

    double min() const;
    double max() const;
    /** sets range */
    void setRange( double min, double max );
    /** returns true if the range contains the number (i.e. if min<=number<=max) */
    bool contains( double number ) const;

  private:
    // This function makes sure m_min really is less than m_max:
    void checkValues();
    double m_min;
    double m_max;

  }; // class Range

} // namespace LVL1

#endif // TRIGT1INTERFACES_RANGE_H
