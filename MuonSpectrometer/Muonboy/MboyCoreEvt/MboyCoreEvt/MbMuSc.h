/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbMuSc_H
#define MbMuSc_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbPOT.h"
  /**
   @class MbMuSc

   This class is for storing data on crossing of MS/Eloss points

  @author samusog@cern.ch
  
  */

class MbMuSc: public MbPOT{
public:
    MbMuSc();
   ~MbMuSc();
    MbMuSc( const MbMuSc& aMbMuSc );

public:
///////////////////////////////////

   /**Print the data */
   virtual void Print(std::ostream* out)  const ;
   virtual void PrintCout()               const ;

   /**Set R at crossing    */
   void SetRRMASC(const double& RRMASC_in) ;

   /**Set LLMASC    */
   void SetLLMASC(const int&    LLMASC_in) ;

   /**Set X0 amount        */
   void SetX0MASC(const double& X0MASC_in) ;

   /**Set angles parameters*/
   void SetS1MASC(const double& S1MASC_in) ;
   void SetS2MASC(const double& S2MASC_in) ;
   void SetTHMASC(const double& THMASC_in) ;

   /**Set A and B  Eloss parameter*/
   void SetALMASC(const double& ALMASC_in) ;
   void SetBLMASC(const double& BLMASC_in) ;

   /**Set position and dir at crossing    */
   void SetPosDirMASC(
                      const double& XposMASC_in ,
                      const double& YposMASC_in ,
                      const double& ZposMASC_in ,
                      const double& XdirMASC_in ,
                      const double& YdirMASC_in ,
                      const double& ZdirMASC_in
                     ) ;
   void SetDir2MASC(
                    const double& XdirMASC_in ,
                    const double& YdirMASC_in ,
                    const double& ZdirMASC_in
                   ) ;
   
   /**Set E at crossing    */
   void SetPmomMASC(const double& PmomMASC_in) ;
   void SetPmom2MASC(const double& PmomMASC_in) ;

   /**Set charge    */
   void SetChgeMASC(const double& ChgeMASC_in) ;

   
   
   /**Get R at crossing    */
   double GetRRMASC() const  ;

   /**Get LLMASC    */
   int    GetLLMASC() const  ;

   /**Get X0 amount        */
   double GetX0MASC() const  ;

   /**Get angles parameters*/
   double GetS1MASC() const  ;
   double GetS2MASC() const  ;
   double GetTHMASC() const  ;

   /**Get A and B Eloss parameter*/
   double GetALMASC() const  ;
   double GetBLMASC() const  ;

   /**Get position and dir at crossing    */
   double GetXposMASC() const  ;
   double GetYposMASC() const  ;
   double GetZposMASC() const  ;
   
   double GetXdirMASC() const  ;
   double GetYdirMASC() const  ;
   double GetZdirMASC() const  ;
   
   double GetXdir2MASC() const  ;
   double GetYdir2MASC() const  ;
   double GetZdir2MASC() const  ;
   
   /**Get momentum at crossing    */
   double GetPmomMASC() const  ;

   double GetPmom2MASC() const  ;

   /**Get charge    */
   double GetChgeMASC() const  ;

private:
///////////////////////////////////

  double m_RRMASC ; //!< R at crossing    

  int    m_LLMASC ; //!< LLMASC      

  double m_X0MASC ; //!< X0 amount        

  double m_S1MASC; //!< angles parameters     
  double m_S2MASC; //!< angles parameters       
  double m_THMASC; //!< angles parameters       

  double m_ALMASC ; //!< A Eloss parameter
  double m_BLMASC ; //!< B Eloss parameter

  double m_XposMASC ; //!< position at crossing
  double m_YposMASC ; //!< position at crossing
  double m_ZposMASC ; //!< position at crossing
  
  double m_XdirMASC ; //!< direction at crossing
  double m_YdirMASC ; //!< direction at crossing
  double m_ZdirMASC ; //!< direction at crossing
  
  double m_Xdir2MASC ; //!< direction at crossing
  double m_Ydir2MASC ; //!< direction at crossing
  double m_Zdir2MASC ; //!< direction at crossing
  
  double m_PmomMASC ; //!< momentum at crossing

  double m_Pmom2MASC ; //!< momentum at crossing

  double m_ChgeMASC ; //!< charge

};

#endif
