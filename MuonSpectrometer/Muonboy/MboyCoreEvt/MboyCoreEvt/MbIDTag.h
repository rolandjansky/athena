/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbIDTag_H
#define MbIDTag_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"



  /**
   @class MbIDTag

   This class is an absract class for taging a data

  @author samusog@cern.ch
  
  */

class MbIDTag {
public:
    MbIDTag();
    virtual ~MbIDTag();
    MbIDTag(const MbIDTag& ToBeCopied);
    MbIDTag& operator=( const MbIDTag& ToBeCopied );

public:
///////////////////////////////////

   /**Print the data using Amdc Ids*/
   virtual void PrintAmdIds(std::ostream* out) const = 0;

   /**Print the data using Athena Ids*/
   virtual void PrintAthIds(std::ostream* out) const = 0;

   /**Set the validity of the carried Amdb Ids*/
   void SetAreAmdbIds (const int& AreAmdbIds );

   /**Get the validity of the carried Amdb Ids*/
   int GetAreAmdbIds () const ;
   
   /**Set Athena Station Nber  */
   void SetSGStationNber(const int& SGStationNber);

   /**Set Athena Station name  */
   void SetSGStationName(const std::string& SGStationName);

   /**Set Athena Eta Id        */
   void SetSGStationEta (const int& SGStationEta );

   /**Set Athena Phi Id        */
   void SetSGStationPhi (const int& SGStationPhi );

   /**Get Athena Station Nber */
   int GetSGStationNber () const ; 

   /**Get Athena Station name */
   std::string GetSGStationName () const ;

   /**Get Athena Eta Id       */
   int GetSGStationEta  () const ;

   /**Get Athena Phi Id       */
   int GetSGStationPhi  () const ;

   /**Set Amdc Station name */
   void SetStationName    (const std::string& StationName );

   /**Set Amdc Station Nber */
   void SetJTYP           (const int& JTYP );

   /**Set Amdc JFF Id       */
   void SetJFF(const int& JFF );

   /**Set Amdc JZZ Id       */
   void SetJZZ(const int& JZZ );

   /**Set Amdc JOB Id       */
   void SetJOB(const int& JOB );
   
   /**Get Amdc Station Nber */
   int GetJTYP() const ; 

   /**Get Amdc Station name */
   std::string GetStationName () const ;

   /**Get Amdc JFF Id       */
   int GetJFF() const ;

   /**Get Amdc JZZ Id       */
   int GetJZZ() const ;

   /**Get Amdc JOB Id       */
   int GetJOB() const ;

   /**Get Station name as characters array */
   void	       GetStationNameChar(char* NameI) const ;

  /**Get Identifier */
  int GetIdentifier () const;

  /**Set Identifier */
  void SetIdentifier (int identifier);

  /**Copy Base Ids */
  void CopyBaseIds(const MbIDTag* pMbIDTag);
  
protected:
///////////////////////////////////

 int         m_AreAmdbIds ; //!< validity of the carried Amdb Ids

 int	     m_SGStationNber ; //!< Athena Station Nber  

 int	     m_SGStationEta  ; //!< Athena Eta Id
 int	     m_SGStationPhi  ; //!< Athena Phi Id

 int         m_JTYP            ; //!< Amdc Station Nber 
 std::string m_StationName     ; //!< Amdc Station name 
 int         m_JFF             ; //!< Amdc JFF Id
 int         m_JZZ             ; //!< Amdc JZZ Id
 int         m_JOB             ; //!< Amdc JOB Id

 char        m_StationNameChar[3] ; //!< Station Name as characters array

 int         m_Identifier      ; //!< Identifier of this digit

};

#endif
