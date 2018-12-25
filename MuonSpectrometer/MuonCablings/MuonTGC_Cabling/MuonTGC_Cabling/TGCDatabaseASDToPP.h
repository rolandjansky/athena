/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCDATABASEASDTOPP_HH
#define MUONTGC_CABLING_TGCDATABASEASDTOPP_HH

#include "MuonTGC_Cabling/TGCDatabase.h"

namespace MuonTGC_Cabling
{
 
class TGCDatabaseASDToPP : public TGCDatabase
{
public:
  /**
     \code
     share/MuonTGC_Cabling_ASD2PP.db
     s t uuu x y zz

      s    = Layer ID
      t    = Chamber ID
      uuu  = Wire-group ID
      x    = PP ID
      y    = PP input-connector ID
      zz   = PP input-channel ID of a connector
     \endcode
     (x, y, zz) is used as IndexOut in this class. 
  */
  enum DATABASESIZE {DATABASESIZE=8}; 
  enum INDEXIN {NIndexIn=3};  
  static const int IndexIn[NIndexIn]; 
  static const int ReverseIndexIn[DATABASESIZE]; 
  enum INDEXOUT {NIndexOut=3}; 
  static const int IndexOut[NIndexOut]; 
  static const int ReverseIndexOut[DATABASESIZE]; 

  /** Constructor */
  TGCDatabaseASDToPP(std::string filename, std::string blockname, bool v_isCommon=true);
  /** Copy constructor */
  TGCDatabaseASDToPP(const TGCDatabaseASDToPP&, bool v_isCommon=true);
  /** Destructor */
  virtual ~TGCDatabaseASDToPP(void);

  virtual bool update(const std::vector<int>&);
 
  virtual int find(const std::vector<int>&) const;

  /** Get IndexDBIn (position in the databse between 0 and database.size()-1) 
      from indexIn which is NIndexIn-dimension array */
  virtual int getIndexDBIn(int* indexIn); 
  /** Get IndexDBOut (position in the databse between 0 and database.size()-1) 
      from indexOut which is NIndexOut-dimension array */
  virtual int getIndexDBOut(int* indexOut);

  /** Get the IndexDBIn table */
  virtual void getindexDBVectorIn(std::vector<int>& tmpindexDBIn) const;
  /** Get the size of the IndexDBIn table */ 
  virtual void getNIndexDBIn(int& tmpNIndexDBIn) const; 
  /** Get the maximum values of indexIn with NIndexIn dimensions */ 
  virtual void getmaxIndexIn(int* tmpmaxIndexIn) const; 
  /** Get the minimum values of indexIn with NIndexIn dimensions */ 
  virtual void getminIndexIn(int* tmpminIndexIn) const; 

  /** Get the IndexDBOut table */
  virtual void getindexDBVectorOut(std::vector<int>& tmpindexDBOut) const;
  /** Get the size of the IndexDBOut table */ 
  virtual void getNIndexDBOut(int& tmpNIndexDBOut) const; 
  /** Get the maximum values of indexOut with NIndexOut dimensions */ 
  virtual void getmaxIndexOut(int* tmpmaxIndexOut) const; 
  /** Get the minimum values of indexOut with NIndexOut dimensions */ 
  virtual void getminIndexOut(int* tmpminIndexOut) const; 
  
  /** This method is used to know the database is common or sector specific */
  bool isCommon() const;

private:
  virtual void readDB(void);
  TGCDatabaseASDToPP(void) {}

  /** Make the IndexDBIn table */
  virtual void makeIndexDBIn(void);
  /** Get the interal number, which is between 0 and NIndexDBIn-1 */ 
  virtual int convertIndexDBIn(int* indexIn) const;

  std::vector<int> m_indexDBIn;
  int m_NIndexDBIn;
  int m_maxIndexIn[NIndexIn];
  int m_minIndexIn[NIndexIn];

  /** Make the IndexDBOut table */
  virtual void makeIndexDBOut(void);
  /** Get the interal number, which is between 0 and NIndexDBOut-1 */ 
  virtual int convertIndexDBOut(int* indexOut) const;

  std::vector<int> m_indexDBOut;
  int m_NIndexDBOut;
  int m_maxIndexOut[NIndexOut];
  int m_minIndexOut[NIndexOut];

  /** This bool variable is used to know the database is common or sector specific */
  bool m_isCommon; 

  /** hidden assignment operator */
  TGCDatabaseASDToPP & operator=(const TGCDatabaseASDToPP &right);
};
  
} // end of namespace
 
#endif
