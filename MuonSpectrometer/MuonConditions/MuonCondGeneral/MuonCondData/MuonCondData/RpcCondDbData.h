/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_RPCCONDDBDATA_H
#define MUONCONDDATA_RPCCONDDBDATA_H

//STL includes
#include <string>
#include <vector>

//Athena includes
#include "Identifier/Identifier.h"
#include "AthenaKernel/CondCont.h" 
#include "AthenaKernel/BaseInfo.h" 


//forward declarations
class Identifier;


class RpcCondDbData {

  friend class RpcCondDbAlg;

public:

    RpcCondDbData();
    virtual ~RpcCondDbData() = default;

    void setDeadPanel       (const std::string&, Identifier );
    void setDeadStrip       (Identifier                     );
    void setDeadStripInt    (Identifier, int                );
    void setDeadStripList   (Identifier, const std::string& );
    void setEfficiency      (Identifier, double             );
    void setFracClusterSize1(Identifier, double             );
    void setFracClusterSize2(Identifier, double             );
    void setFracClusterSize3(Identifier, double             );
    void setFracDeadStrip   (Identifier, double             );
    void setGapEfficiency   (Identifier, double             );
    void setLowEffPanel     (Identifier                     );
    void setMeanClusterSize (Identifier, double             );
    void setOffPanel        (const std::string&, Identifier );
    void setProjectedTrack  (Identifier, int                );
    void setStripTime       (Identifier, const std::vector<double>&);

    const std::vector<std::string>& getDeadPanels() const;
    const std::vector<std::string>& getOffPanels () const;
    
    const std::vector<Identifier>& getDeadPanelsId() const;
    const std::vector<Identifier>& getDeadStrips  () const;
    const std::vector<Identifier>& getLowEffPanels() const;
    const std::vector<Identifier>& getOffPanelsId () const;

    const std::string&         getDeadStripList   (const Identifier& );
    const int&                 getDeadStripInt    (const Identifier& );
    const double&              getEfficiency      (const Identifier& );
    const double&              getFracClusterSize1(const Identifier& );
    const double&              getFracClusterSize2(const Identifier& );
    const double&              getFracClusterSize3(const Identifier& );
    const double&              getFracDeadStrip   (const Identifier& );
    const double&              getGapEfficiency   (const Identifier& );
    const double&              getMeanClusterSize (const Identifier& );
    const int&                 getProjectedTrack  (const Identifier& );
    const std::vector<double>& getStripTime       (const Identifier& );
 
 
    bool isEffPanel      (const Identifier & Id) const;
    bool isGood          (const Identifier & Id) const;
    bool isGoodPanel     (const Identifier & Id) const;
    bool isGoodStrip     (const Identifier & Id) const;
    bool isOffPanel      (const Identifier & Id) const;


 
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
CLASS_DEF( CondCont<RpcCondDbData>, 178635428, 0)

#endif
