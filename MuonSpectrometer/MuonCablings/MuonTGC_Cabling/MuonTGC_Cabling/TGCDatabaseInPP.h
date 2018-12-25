/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCDATABASEINPP_HH
#define MUONTGC_CABLING_TGCDATABASEINPP_HH

#include "MuonTGC_Cabling/TGCDatabase.h"

namespace MuonTGC_Cabling
{
 
class TGCDatabaseInPP : public TGCDatabase
{
public:
  /**
     \code
     share/MuonTGC_Cabling_PP.db
      s t uu x y zz (X Y ZZ)

       s   = output PP ID
       t   = output-connector ID
       uu  = output-channel ID of a connector
       x   = input PP ID
       y   = input-connector ID
       zz  = input-channel ID of a connector
       X, Y, ZZ = OR'ed channel informationin (if exist)
     \endcode
     (s, t, uu) is used as IndexIn in this class.  
   */
  enum INDEXIN {NIndexIn=3, IndexInMin=0, IndexInMax=2};

  /** Constructor */
  TGCDatabaseInPP(std::string filename, std::string blockname);
  /** Copy constructor */
  TGCDatabaseInPP(const TGCDatabaseInPP&);
  /** Destructor */ 
  virtual ~TGCDatabaseInPP(void);

  virtual bool update(const std::vector<int>&);
 
  virtual int find(const std::vector<int>&) const;

  /** Get IndexDBIn (position in the databse between 0 and database.size()-1)
      from indexIn which is NIndexIn-dimension array */
  virtual int getIndexDBIn(int* indexIn);

  /** Get the IndexDBIn table */
  virtual void getindexDBVectorIn(std::vector<int>& tmpindexDBIn) const;
  /** Get the size of the IndexDBIn table */ 
  virtual void getNIndexDBIn(int& tmpNIndexDBIn) const;
  /** Get the maximum values of indexIn with NIndexIn dimensions */ 
  virtual void getmaxIndexIn(int* tmpmaxIndexIn) const;
  /** Get the minimum values of indexIn with NIndexIn dimensions */ 
  virtual void getminIndexIn(int* tmpminIndexIn) const;

private:
  virtual void readDB(void);
  TGCDatabaseInPP(void) {}

  /** Make the IndexDBIn table */
  virtual void makeIndexDBIn(void);
  /** Get the internal number, which is between 0 and NIndexDBIn-1 */ 
  virtual int convertIndexDBIn(int* indexIn) const;

  std::vector<int> m_indexDBIn;
  int m_NIndexDBIn;
  int m_maxIndexIn[NIndexIn];
  int m_minIndexIn[NIndexIn];

  /** hidden assignment operator */
  TGCDatabaseInPP & operator=(const TGCDatabaseInPP &right);
};
  
} // end of namespace
 
#endif
