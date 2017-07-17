/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonManager_H
#define AthenaMonManager_H

#include <map>
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"

class ISvcLocator;
class ITHistSvc;
class ManagedMonitorToolBase;
class TObject;
class IMonitorToolBase;
class LWHist;

/**
 * An Algorithm that manages a set of modules,
 * each inheriting from ManagedMonitorToolBase, by setting common
 * configuration variables and organizing the output data.  It provides
 * all the functionality and behaviour of AthenaMon.
 *
 * $Id: AthenaMonManager.h,v 1.21 2009-05-08 09:50:22 sschaetz Exp $
 *
 * @author Michael Wilson, CERN, January 2007
 */

class AthenaMonManager : public AthAlgorithm {

   public:

      /**
       * An enumeration of the different types of running environment the
       * monitoring application may be in.  The running environment may be
       * used to select which histograms are produced, and where they are
       * located in the output.  For example, the output paths of the
       * histograms are different for the "user", "online" and the various
       * offline flags.  The 'tier0Raw' and 'tier0ESD' are used to select
       * sets of histograms for production either during the RAW --> ESD
       * transform or during the ESD --> AOD transform (but not both!)
       * Strings of the same names may be given as jobOptions.
       */
      enum Environment_t { user = 0, noOutput, online, tier0, tier0Raw, tier0ESD, AOD, altprod };

      /**
       * An enumeration of the different types of data the
       * monitoring application may be running over.  This can be used to
       * select which histograms to produce, e.g., to prevent the production
       * of colliding-beam histograms when running on cosmic-ray data.
       * Strings of the same names may be given as jobOptions.
       */
      enum DataType_t { userDefined = 0, monteCarlo, collisions, cosmics, heavyIonCollisions };

      AthenaMonManager( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~AthenaMonManager();

      /**
       * Converts a string to an Environment_t of the same name.
       * static method (shared by all AthenaMonManager instances during run-time)
       */
      static Environment_t envStringToEnum( const std::string& str );

      /**
       * Converts a string to a DataType_t of the same name.
       */
      static DataType_t dataTypeStringToEnum( const std::string& str );

      /**
       * Returns the running environment of the monitoring application
       * to help ManagedMonitorToolBase objects determine which histograms to produce.
       */
      static Environment_t environment();

      /**
       * Returns the data type that the monitoring application is running over
       * to help ManagedMonitorToolBase objects determine which histograms to produce.
       */
      static DataType_t dataType();

      static unsigned int lumiBlockNumber();
      static unsigned int runNumber();
      static unsigned int fillNumber();

      static unsigned int getLBsLowStat();
      static unsigned int getLBsMedStat();
      static unsigned int getLBsHigStat();

      bool forkedProcess();

      virtual StatusCode initialize();
      virtual StatusCode execute();
      virtual StatusCode stop();
      virtual StatusCode finalize();

      virtual StatusCode beginRun();
      virtual StatusCode endRun();

      virtual std::string fileKey() const;

      /**
       * Pass ownership of a TObject/LWHist to this manager so that it will be deleted appropriately.
       */
      virtual void passOwnership( TObject* h, const std::string& key );
      virtual void passOwnership( LWHist* h, const std::string& key );

      /**
       * If the TObject is owned by this manager, its Write() method is called and it is deleted.
       */
      virtual void writeAndDelete( const std::string& key );
      virtual LWHist *ownedLWHistOfKey(const std::string& key) const;
      // If you know it is an LWHist use writeAndDeleteLWHist. The just deleted LWHist pointer will be returned (0 if nothing deleted)
      virtual LWHist *writeAndDeleteLWHist( const std::string& key, const std::string& streamName );
      virtual LWHist *writeAndResetLWHist( const std::string& key, const std::string& streamName );

      virtual void writeAndDeleteLWHist( LWHist*lwh );

   protected:
      ToolHandleArray<IMonitorToolBase> m_monTools;

      virtual void setEventInfo();

      ITHistSvc*  m_THistSvc;

   private:
      // Use private implementation idiom for more flexible development.
      class Imp;
      Imp *m_d;
};

#endif
