/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   AthTileTripReader.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on August 21, 2012, 11:32 AM
 */

#ifndef ATHTILETRIPREADER_H
#define	ATHTILETRIPREADER_H

#include "PATCore/IAthSelectorTool.h"
#include "PATCore/IUserDataCalcTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "PATCore/TAccept.h"
#include "PATCore/TResult.h"
#include "TileTripReader/TTileTripReader.h"


class INavigable4Momentum;

static const InterfaceID IID_AthTileTripReader("AthTileTripReader", 1 , 0);

class AthTileTripReader : virtual public IAthSelectorTool,
        virtual public IUserDataCalcTool,
        public AthAlgTool
{
public:
    static const inline InterfaceID& interfaceID() { return IID_AthTileTripReader; }
    AthTileTripReader(const std::string& type, 
            const std::string& name, 
            const IInterface* parent);
    
    
    virtual ~AthTileTripReader();
    
    virtual StatusCode initialize();
    
    virtual StatusCode finalize();
    
    const Root::TAccept& accept(const INavigable4Momentum* part=0);
    
    const Root::TResult& calculate(const INavigable4Momentum* part);
    
    virtual StatusCode calculateElementUserData( const IAthenaBarCode *abc );
    
    virtual StatusCode calculateEventUserData( /*nothing!*/ ){
        return StatusCode::SUCCESS;
    }
    
    bool checkEvent();
    
    inline Root::TTileTripReader* getRootTool(){return m_tripReader;}
    
    inline virtual const Root::TAccept& getTAccept(){
        return getRootTool()->getTAccept();
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
    Root::TAccept m_acceptDummy;
    Root::TResult m_resultDummy;
    double m_dR;
};

#endif	/* ATHTILETRIPREADER_H */

