/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   AthTileTripReader.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on August 21, 2012, 11:32 AM
 */

#ifndef ATHTILETRIPREADER_H
#define	ATHTILETRIPREADER_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
#include "TileTripReader/TTileTripReader.h"


class INavigable4Momentum;

static const InterfaceID IID_AthTileTripReader("AthTileTripReader", 1 , 0);

class AthTileTripReader : public AthAlgTool
{
public:
    static const inline InterfaceID& interfaceID() { return IID_AthTileTripReader; }
    AthTileTripReader(const std::string& type, 
            const std::string& name, 
            const IInterface* parent);
    
    
    virtual ~AthTileTripReader();
    
    virtual StatusCode initialize() override;
    
    virtual StatusCode finalize() override;
    
    asg::AcceptData accept(const INavigable4Momentum* part=0);
    
    float calculate(const INavigable4Momentum* part);
    
    bool checkEvent();
    
    inline Root::TTileTripReader* getRootTool(){return m_tripReader;}
    
    inline virtual const asg::AcceptInfo getAcceptInfo(){
        return getRootTool()->getAcceptInfo();
    }
    
    //static const InterfaceID& interfaceID(){return  IID_AthTileTripReader;}
    
//    inline void setReaderVerbosity(int verbose=1){
//        m_tripReader->setVerbosity(verbose);
//    }
//    
//    inline void setTileTripFile(const char* file){
//        m_tripFile=file;
//        m_tripReader->setTripFile(file);
//    }
    
private:
    std::string m_tripFile;
    Root::TTileTripReader* m_tripReader;
    double m_dR;
};

#endif	/* ATHTILETRIPREADER_H */

