/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGCONFSVC_VERIFYER_H
#define TRIGCONFSVC_VERIFYER_H

class MsgStream;

namespace TrigConf {

  // Forward include(s):
  class Menu;

  /**
   *  $Date: 2006-08-08 08:21:39 $
   *
   *    @short Class used to check the validity of the loaded configuration
   *
   *           As the users will be able to load their own trigger configuration XMLs, this
   *           class is used to check whether the loaded configuration is valid.
   *
   *      @see TrigCongInterface::Menu
   *
   *   @author $Author: haller $
   *  @version $Revision: 1.2 $
   *
   */
  class Verifyer {

  public:
    /// This is the general verification function called from the outside
    static bool verify( const Menu& menu, MsgStream& log );

    //
    // Constants used by the class, and providing them to the outside:
    //
    static const unsigned int MUON_CABLE_MASK;
    static const unsigned int CP1_CABLE_MASK;
    static const unsigned int CP2_CABLE_MASK;
    static const unsigned int JEP1_CABLE_MASK;
    static const unsigned int JEP2_CABLE_MASK;
    static const unsigned int JEP3_CABLE_MASK;

  private:
    // These are the test called by verify():
    static bool verifyThCount( const Menu& menu, MsgStream& log );
    static bool verifyThLayout( const Menu& menu, MsgStream& log );

    // Helper functions:
    static unsigned int createMask( int startbit, int stopbit );
    template< class T > static T max( T i, T j );
    template< class T > static T min( T i, T j );

  }; // class Verifyer

} // namespace TrigConf

#endif // TRIGCONFSVC_VERIFYER_H
