/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TTileTripReader.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on August 7, 2012, 4:21 PM
 */

#ifndef TTILETRIPREADER_H
#define	TTILETRIPREADER_H

#include <vector>
#include <map>
#include <iostream>
#include <iosfwd>

#ifndef ROOTCORE
#include "PATCore/AcceptData.h"
#include "PATCore/AcceptInfo.h"
#endif /*ROOTCORE*/


class TTree;



struct TripRegion{
    double eta1;
    double eta2;
    double phi1;
    double phi2;
};


namespace Root{

class TTileTripReader
{
public:
    enum Verbosity{
        Default=0,
        Debug=1
    };
    
    TTileTripReader(const char* name="TTileTripReader" );
    virtual ~TTileTripReader();
    
    /** Get the name of the class instance */
    inline const char* getName() const { return m_name.c_str(); };

    const asg::AcceptInfo& getAcceptInfo( ) const
    {
      return m_accept;
    }

#ifndef ROOTCORE
    /**
     * 
     * @param run Run number
     * @param lbn Lumiblock number
     * @param event Event number
     * @param eta Eta position
     * @param phi Phi position
     * @param tileError the tileError value
     * @param tileFlags the tileFlags value
     * @return Constant AcceptData containing the result of the check
     * 
     * Trip result is stored under "InTrip", event check is stored under "BadEvent"
     */
    asg::AcceptData accept(int run,int lbn, unsigned int event=0,double eta=-99.9,double phi=-99.9, int tileError=0, int tileFlags=0);
#endif /*ROOTCORE*/
    
    /**
     * 
     * @param run the run number
     * @param lbn the lumiblock number
     * @param eta the area central eta coordinate
     * @param phi the area central phi coordinate
     * @param dR the area size
     * @return The fraction of the area in eta phi space covered by tripped modules
     */
    double areaTripFraction(int run, int lbn, double eta, double phi, double dR);
    
#ifndef ROOTCORE
    /**
     * 
     * @param run Run number
     * @param lbn Lumiblock number
     * @param eta Eta value of cone center
     * @param phi Phi value of cone center
     * @param dR Radius of the cone in (eta,phi) space
     * @param tileError the tileError value
     * @param tileFlags the tileFlags value
     * @return fraction of the area in eta phi space covered by tripped modules
     */
    float calculate(int run, int lbn, double eta, double phi, double dR, int tileError=0, int tileFlags=0);
#endif /*ROOTCORE*/
    
    
    /**
     * 
     * @param run the run number
     * @param lbn the lumiblock number
     * @param eta eta value to check
     * @param phi phi value to check
     * @return true if there's a tripped module in the region
     */
    bool checkEtaPhi(int run,int lbn,double eta,double phi);
    
    /**
     * 
     * @param run Run number
     * @param lbn Lumiblock number
     * @param event event number
     * @return true if event is good false if bad
     * 
     * Checks against an internal map of 433 bad events
     */
    bool checkEvent(unsigned int run,unsigned int lbn,unsigned int event);
    
    /**
     * 
     * @param run the run number
     * @param lbn the lumiblock number
     * @param eta eta value to check
     * @param phi phi value to check
     * @param delEta extends each trip region checked by delEta on both sides
     * @param delPhi extends each trip region checked by delPhi on both sides
     * @return  true if there's a tripped module near eta,phi
     */
    bool checkExtendedEtaPhi(int run,int lbn,double eta,double phi,double delEta,double delPhi);
    
    inline int finalize() { return 1; }
    
    /**
     * 
     * @param run the run number
     * @param lbn the lumiblock number
     * @param force Forces regenerating the trip list when the run and lbn haven't changed
     * @return a vector containing TripRegions structs for all trip regions
     */
    std::vector<TripRegion> getEtaPhi(int run, int lbn, bool force=false);
    
    int initialize();
    
    /**
     * closes the ability to add more trip files to the chain and loads the current
     * TChain into memory as a TTree.
     */    
    void memLoadTripFile();
    
    /**
     * 
     * @param part tile partition: LBA=0, LBC=1, EBA=2, EBC=3
     * @param mod tile module. Note: starts at 0, not 1
     * @return A TripRegion struct containing the limits of the module
     */
    TripRegion partModToEtaPhi(int part, int mod);
    
    /**
     * 
     * @param run the run number
     * @param lbn the lumiblock number
     * @param tileError the tileError value
     * @param tileFlags the tileFlags value
     * 
     * Uses the tileError and tileFlags values to improve the set of trip regions
     * in the event.
     */
    void readTileFlags(int run, int lbn, int tileError, int tileFlags);
    
    /**
     * 
     * @param file the file path to the trip list root file
     * @return the number of files added
     * 
     * The trip reader uses TChain to read the TTrees inside the root file.  
     * You can load multiple files by using wildcards.
     */
    int setTripFile(const char* file);
    
    void setVerbosity(int v=Debug,std::ostream& stream=std::cout){
        m_verbosity=v;
        m_msglog=&stream;
    }
    
    TripRegion m_LBOffsets;
    TripRegion m_EBOffsets;
private:
    /**
     * Attempts to correct for improperly built trip list files. *DEPRECIATED*
     */
    void buildOffsets();
    
    /**
     * 
     * @param run Run number
     * @return Position in trip TTree where the run starts
     */
    int findStartEntry(int run);
    
    /**
     * Fills the bad event map
     */
    void setBadEventList();


    /** The name of the class instance */
    std::string m_name;

    asg::AcceptInfo m_accept;

    TTree* m_trips;
    TTree* m_runMap;
    int m_mapRun;
    int m_Run;
    int m_FirstEntry;
    int m_currentRun;
    int m_currentLbn;
    int m_startEntry;
    int m_verbosity;
    int m_LastTileFlags;
    std::map<unsigned int,std::map<unsigned short,unsigned int> > m_badEvents;
    std::vector<TripRegion> m_currentTrips;
    std::vector<int> m_Offsets;
    std::vector<char>* m_Partition;
    std::vector<char>* m_Module;
    int m_LumiStart;
    std::vector<int>* m_LumiEnd;
    std::ostream* m_msglog;
    bool m_ChainsLoaded;
    

};
}

#endif	/* TTILETRIPREADER_H */

