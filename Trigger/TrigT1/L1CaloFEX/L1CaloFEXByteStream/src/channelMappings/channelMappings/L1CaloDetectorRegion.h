#ifndef L1CALO_DETECTOR_REGION_H
#define L1CALO_DETECTOR_REGION_H

#include <string>

class CaloDivision;

class L1CaloDetectorRegion {

   public:

      enum ObjectTypeEnum {
        PPM = 0,
        CPM,
        JEM,
        EFEX,
        JFEX,
        GFEX,
        NONE
      };

      enum LayerTypeEnum {
        Electromagnetic = 0,
        Hadronic,
        HadFcal2,
        HadFcal3,
        Presampler,
        Front,
        Middle,
        Back,
        Both,
        None
      };

      // Constructors
      L1CaloDetectorRegion ();
      L1CaloDetectorRegion (ObjectTypeEnum, LayerTypeEnum, bool, int, int, double, double, double, double);
      // Destructor
      ~L1CaloDetectorRegion ();
      
      bool operator==( const L1CaloDetectorRegion& rhs );

      // get methods
      int              getEtaIndex () const {return m_etaIdx;};
      int              getPhiIndex () const {return m_phiIdx;};
      double           getEtaBinWidth () const {return m_etaBinWidth;};
      double           getPhiBinWidth () const {return m_phiBinWidth;};
      double           getEtaCoordinate () const {return m_etaCoord;};
      double           getPhiCoordinate () const {return m_phiCoord;};
      bool             getValidity () const {return m_valid;};
      ObjectTypeEnum   getObjectType() const { return m_object; }
      LayerTypeEnum    getLayer() const { return m_layer; }
      double           getEtaMin() const { return m_etaMin; }
      double           getEtaMax() const { return m_etaMax; }
      double           getPhiMin() const { return m_phiMin; }
      double           getPhiMax() const { return m_phiMax; }

      // methods to convert enums to strings
      std::string      layerToString();
      std::string      objectTypeToString();
      
      // check region against patterns of disabled areas
      CaloDivision     getCaloDivision() const;
      unsigned int     getEtaLetterBit() const;
      unsigned int     getPhiLetterBit() const;
      bool             isDisabled(unsigned int caloDivisionsDisabled,
                                  unsigned int etaLetterBitsDisabled,
                                  unsigned int phiLetterBitsDisabled) const;
      bool             isCaloDisabled(unsigned int caloDivisionsDisabled) const;
      bool             isEtaPhiDisabled(unsigned int etaLetterBitsDisabled,
                                        unsigned int phiLetterBitsDisabled) const;

      void             printInfo ();

   private:

      // data members
      ObjectTypeEnum   m_object;
      LayerTypeEnum    m_layer;
      bool             m_valid; //!< validity of the mapped channel

      // eta-phi coordinates
      int              m_etaIdx; //!< eta index
      int              m_phiIdx; //!< phi index
      double           m_etaBinWidth; //!< eta granularity
      double           m_phiBinWidth; //!< phi granularity
      double           m_etaCoord; //!< eta coordinate of the trigger tower
      double           m_phiCoord; //!< phi coordinate of the trigger tower
      double           m_etaMin; //!< lower eta bounds of tower
      double           m_etaMax; //!< upper eta bounds of tower
      double           m_phiMin; //!< lower phi bounds of tower
      double           m_phiMax; //!< upper phi bounds of tower
};

#endif // L1CALO_DETECTOR_REGION_H
