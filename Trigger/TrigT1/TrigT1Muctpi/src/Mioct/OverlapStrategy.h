// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: OverlapStrategy.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_OVERLAPSTRATEGY_H
#define TRIGT1MUCTPI_OVERLAPSTRATEGY_H

// STL include(s):
#include <string>

namespace LVL1MUCTPI {

  // Forward declaration(s):
  class MioctSectorSet;
  class PtMultiplicitySet;
  class MioctID;

  /**
   ************************************************************************
   *
   *    $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $ 
   *
   *    @short Interface for a strategy to resolve overlap between muon candidates.
   *
   *           The   interface to   the  various   strategies  to resolve
   *           overlapping  muon  candidates is  very simple  because the
   *           strategies just  have to calculate  the pt-multiplicities.
   *           Anyway the abstract class  is usefull in order  to support
   *           polymorphism for the strategies.
   *
   *           The OverlapStrategy  class follows the Strategy-Pattern in
   *           E.Gamma, R.Helm, R.Johnson, J.Vlissides "Design Patterns".
   *           (It is very simple though, I admit...)
   *
   *      @see OverlapLogic
   *      @see MioctModule
   *      @see MioctSectorSet
   *      @see PtMultiplicitySet
   *   @author $Author: krasznaa $
   *  @version $Revision: 362102 $
   *
   *
   ************************************************************************
   */
  class OverlapStrategy {

  public:
    /**
     * The constructor of a Strategy takes a string argument which is 
     * the name of the strategy. Since standard strings have a copy
     * constructor and this name string is the only data member of a
     * strategy, all strategies automatically have a copy constructor.
     * (Of course, if in a derived class more data members are needed, 
     * for those also a copy constructor must exist, in order to have 
     * one for the whole strategy.) This fact is important because in
     * the OverlapLogic the strategies are put into a STL map for which
     * a copy constructor is needed.
     * @see OverlapLogic
     */
    OverlapStrategy( std::string theName );
    virtual ~OverlapStrategy();
    /**
     * Here the calculcation of the multiplicity for the six different
     * pt thresholds is performed. 
     * @see MioctSectorSet
     * @see PtMultiplicitySet
     * @see MioctModule
     * @param data  is the words corresponding to the sectors of a MioctModule.
     * @param mioct defines which MIOCT the data is coming from
     * @return PtMultiplicitySet is the result of the calculation. 
     */
    virtual PtMultiplicitySet calculateMultiplicity( const MioctSectorSet& data, const MioctID& mioct ) const = 0;
    /**
     * The OverlapLogic needs to acces the name of the strategy.
     * @see OverlapLogic
     * @return the name of the strategy. 
     */
    std::string getName() const { return m_name; }

  private:

    std::string m_name;

  }; // class OverlapStrategy

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_OVERLAPSTRATEGY_H
