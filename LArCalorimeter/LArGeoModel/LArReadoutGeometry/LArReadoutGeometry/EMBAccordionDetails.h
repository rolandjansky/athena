/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBAccordionDetails_h
#define EMBAccordionDetails_h
class GeoStraightAccSection;
class EMBAccordionDetails {
  
 public:

  /**
   * @brief Destructor
   **/
  EMBAccordionDetails();
  

  /**
   * @brief Destructor
   **/
  ~EMBAccordionDetails();

  /**
   * @brief Phi of the first absorber
   **/
  double phiFirstAbsorber() const;

  /** 
   * @brief Absorber position details:
   **/
  const GeoStraightAccSection *getAbsorberSections() const;
  
  /** 
   * @brief Electrode position details:
   **/
  const GeoStraightAccSection *getElectrodeSections() const;



 private:
  
  // These methods are illegal:
  EMBAccordionDetails(const EMBAccordionDetails &);
  EMBAccordionDetails & operator = (const EMBAccordionDetails &); 

  class Clockwork;
  Clockwork *m_c;
  
};
#endif
