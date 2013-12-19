/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IIOVRegistrationSvc.h 
 * 
 * @brief This is an interface to a tool used to register conditions
 * objects in the Interval of Validity (IOV) database
 * 
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Antoine Pérus <perus@lal.in2p3.fr>
 * 
 * $Id: IIOVRegistrationSvc.h,v 1.5 2007-02-09 15:28:57 hawkings Exp $
 */

#ifndef REGISTRATIONSERVICES_IIOVREGISTRATIONSVC_H
# define REGISTRATIONSERVICES_IIOVREGISTRATIONSVC_H

//<<<<<< INCLUDES                                                       >>>>>>

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include <stdint.h>

class IOVTime;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/** 
 ** @class IIOVRegistrationSvc
 ** 
 ** @brief This is an interface to a service used to register conditions
 ** objects in the Interval of Validity (IOV) database
 **  
 **    Properties:
 **
 **    - RecreateFolder:<pre>  flag to force the recreation of the requested folder</pre>
 **    - BeginRun:<pre>        Begin run number   (default: IOVTime::MINRUN)</pre>	 
 **    - EndRun:<pre>          End run number     (default: IOVTime::MAXRUN)</pre>	 
 **    - BeginLB:<pre>      Begin Lumiblock number (default: IOVTime::MINEVENT)</pre>	 
 **    - EndLB:<pre>        End Lumiblock number   (default: IOVTime::MAXEVENT)</pre>	 
 **    - BeginTime:<pre>       Begin time	  (default: IOVTime::MINTIMESTAMP)</pre>
 **    - EndTime:<pre>         End time	          (default: IOVTime::MAXTIMESTAMP)</pre>
 **    - IOVDbTag:<pre>        the tag to be used</pre>
 **/



class IIOVRegistrationSvc : virtual public IInterface
{

public:    
  
    /// Retrieve interface ID
    static const InterfaceID& interfaceID();

    /// Register IOV DB for an object given its typeName - run/LB numbers
    ///   interval or times interval  and tag are taken from JobOptions
    ///   Choice between run/LB and timestamp given in JobOptions
    virtual StatusCode registerIOV(const std::string& typeName) const = 0;
    
    /// Register IOV DB for an object given its typeName - run/LB numbers
    ///   interval or times interval taken from JobOptions
    ///   tag is specified
    ///   Choice between run/LB and timestamp given in JobOptions
    virtual StatusCode registerIOV( const std::string& typeName, const std::string& tag ) const = 0;
    
    /// Register IOV DB for an object given its typeName and its key
    ///   run/LB numbers interval or times interval  and tag are taken
    ///   from JobOptions
    ///   Choice between run/LB and timestamp given in JobOptions
    virtual StatusCode registerIOV( const std::string& typeName, const std::string& key,
				    const std::string& tag ) const = 0;
    
    /// Register IOV DB for an object given its typeName, tag and run/LB
    /// numbers interval
    virtual StatusCode registerIOV( const std::string& typeName,
				    const std::string& tag,
				    unsigned int beginRun, 
				    unsigned int endRun, 
				    unsigned int beginLB, 
				    unsigned int endLB ) const = 0;
    
    /// Register IOV DB for an object given its typeName, tag and
    /// times interval
    virtual StatusCode registerIOV( const std::string& typeName, 
				    const std::string& tag,
				    uint64_t beginTime, 
				    uint64_t endTime ) const = 0;

    /// Register IOV DB for an object given its typeName, key, tag and run/LB
    /// numbers interval
    virtual StatusCode registerIOV( const std::string& typeName,
				    const std::string& key,
				    const std::string& tag,
				    unsigned int beginRun, 
				    unsigned int endRun, 
				    unsigned int beginLB, 
				    unsigned int endLB ) const = 0;
    
    /// Register IOV DB for an object given its typeName, key, tag and
    /// times interval
    virtual StatusCode registerIOV( const std::string& typeName,
				    const std::string& key,
				    const std::string& tag,
				    uint64_t beginTime, 
				    uint64_t endTime ) const = 0;

    /// Register IOV DB for an object given its typeName, key, folder, tag 
    ///  and run/LB numbers interval
    virtual StatusCode registerIOV( const std::string& typeName,
				    const std::string& key,
				    const std::string& folder,
				    const std::string& tag,
				    unsigned int beginRun, 
				    unsigned int endRun, 
				    unsigned int beginLB, 
				    unsigned int endLB ) const = 0;

    /// Register IOV DB for an object given its typeName, key, folder, tag and
    /// times interval
    virtual StatusCode registerIOV( const std::string& typeName,
				    const std::string& key,
				    const std::string& folder,
				    const std::string& tag,
				    uint64_t beginTime, 
				    uint64_t endTime ) const = 0;
};

inline const InterfaceID& 
IIOVRegistrationSvc::interfaceID() 
{
    static const InterfaceID IID_IIOVRegistrationSvc("IIOVRegistrationSvc", 1 , 0); 
    return IID_IIOVRegistrationSvc; 
}

 


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // REGISTRATIONSERVICES_IIOVREGISTRATIONSVC_H
