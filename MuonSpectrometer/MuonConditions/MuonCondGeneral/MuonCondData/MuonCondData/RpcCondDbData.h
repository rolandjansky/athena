/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_RPCCONDDBDATA_H
#define MUONCONDDATA_RPCCONDDBDATA_H

//STL includes
#include <string>
#include <vector>

//Athena includes
#include "Identifier/Identifier.h"
//#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaKernel/CondCont.h" 
#include "AthenaKernel/BaseInfo.h" 


//forward declarations
class Identifier;


class RpcCondDbData {

  friend class RpcCondDbAlg;

public:

    RpcCondDbData();
    virtual ~RpcCondDbData() = default;

    virtual void setDeadPanel       (std::string, Identifier        );
    virtual void setDeadStrip       (Identifier                     );
    virtual void setDeadStripInt    (Identifier, int                );
    virtual void setDeadStripList   (Identifier, std::string        );
    virtual void setEfficiency      (Identifier, double             );
    virtual void setFracClusterSize1(Identifier, double             );
    virtual void setFracClusterSize2(Identifier, double             );
    virtual void setFracClusterSize3(Identifier, double             );
    virtual void setFracDeadStrip   (Identifier, double             );
    virtual void setGapEfficiency   (Identifier, double             );
    virtual void setLowEffPanel     (Identifier                     );
    virtual void setMeanClusterSize (Identifier, double             );
    virtual void setOffPanel        (std::string, Identifier        );
    virtual void setProjectedTrack  (Identifier, int                );
    virtual void setStripTime       (Identifier, std::vector<double>);

    virtual const std::vector<std::string>& getDeadPanels() const;
    virtual const std::vector<std::string>& getOffPanels () const;
    
    virtual const std::vector<Identifier>& getDeadPanelsId() const;
    virtual const std::vector<Identifier>& getDeadStrips  () const;
    virtual const std::vector<Identifier>& getLowEffPanels() const;
    virtual const std::vector<Identifier>& getOffPanelsId () const;

    virtual const std::string&         getDeadStripList   (const Identifier& );
    virtual const int&                 getDeadStripInt    (const Identifier& );
    virtual const double&              getEfficiency      (const Identifier& );
    virtual const double&              getFracClusterSize1(const Identifier& );
    virtual const double&              getFracClusterSize2(const Identifier& );
    virtual const double&              getFracClusterSize3(const Identifier& );
    virtual const double&              getFracDeadStrip   (const Identifier& );
    virtual const double&              getGapEfficiency   (const Identifier& );
    virtual const double&              getMeanClusterSize (const Identifier& );
    virtual const int&                 getProjectedTrack  (const Identifier& );
    virtual const std::vector<double>& getStripTime       (const Identifier& );
 
 
    virtual bool isEffPanel      (const Identifier & Id) const;
    virtual bool isGood          (const Identifier & Id) const;
    virtual bool isGoodPanel     (const Identifier & Id) const;
    virtual bool isGoodStrip     (const Identifier & Id) const;
    virtual bool isOffPanel      (const Identifier & Id) const;


 
private:

    std::vector<std::string> m_cachedDeadPanels;
    std::vector<std::string> m_cachedOffPanels;

    std::vector<Identifier> m_cachedDeadPanelsId;
    std::vector<Identifier> m_cachedDeadStrips;
    std::vector<Identifier> m_cachedLowEffPanels;
    std::vector<Identifier> m_cachedOffPanelsId;

    std::map<Identifier, std::string         > m_cachedDeadStripList;
    std::map<Identifier, int                 > m_cachedDeadStripListInt;
    std::map<Identifier, double              > m_cachedEfficiency;
    std::map<Identifier, double              > m_cachedFracClusterSize1;
    std::map<Identifier, double              > m_cachedFracClusterSize2;
    std::map<Identifier, double              > m_cachedFracClusterSize3;
    std::map<Identifier, double              > m_cachedFracDeadStrip;
    std::map<Identifier, double              > m_cachedGapEfficiency;
    std::map<Identifier, double              > m_cachedMeanClusterSize;
    std::map<Identifier, int                 > m_cachedProjectedTracks;
    std::map<Identifier, std::vector<double> > m_cachedStripTime;

    std::vector<std::string> m_emptyNames;
    std::vector<Identifier> m_emptyIds; 
    std::vector<double> m_emptyDoubles; 
    double m_nill = -1;
    int    m_nillint = -1;
    std::string m_nillstr = "";

public:

    virtual const std::map<Identifier,              double>& getEfficiencyMap      () const { return m_cachedEfficiency      ;}
    virtual const std::map<Identifier,              double>& getEfficiencyGapMap   () const { return m_cachedGapEfficiency   ;}
    virtual const std::map<Identifier,              double>& getMeanClusterSizeMap () const { return m_cachedMeanClusterSize ;}
    virtual const std::map<Identifier,              double>& getFracClusterSize1Map() const { return m_cachedFracClusterSize1;}
    virtual const std::map<Identifier,              double>& getFracClusterSize2Map() const { return m_cachedFracClusterSize2;}
    virtual const std::map<Identifier,              double>& getFracClusterSize3Map() const { return m_cachedFracClusterSize3;}
    virtual const std::map<Identifier,         std::string>& getDeadStripMap       () const { return m_cachedDeadStripList   ;}
    virtual const std::map<Identifier,              double>& getFracDeadStripMap   () const { return m_cachedFracDeadStrip   ;}
    virtual const std::map<Identifier,                 int>& getProjectedTracksMap () const { return m_cachedProjectedTracks ;}
    virtual const std::map<Identifier,                 int>& getDeadStripIntMap    () const { return m_cachedDeadStripListInt;}
    virtual const std::map<Identifier,std::vector<double> >& getStripTimeMap       () const { return m_cachedStripTime       ;}


};

CLASS_DEF( RpcCondDbData, 25128902, 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<RpcCondDbData>, 178635428, 0)

#endif
