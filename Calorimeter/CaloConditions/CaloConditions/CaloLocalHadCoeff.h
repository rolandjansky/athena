/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDITIONS_CALOLOCALHADCOEFF_H
#define CALOCONDITIONS_CALOLOCALHADCOEFF_H

/**
 * @class CaloLocalHadCoeff
 * @author Gennady Pospelov <guennadi.pospelov@cern.ch>, Pavol Strizenec <pavol@mail.cern.ch>
 * @date 20-July-2009
 * @brief Hold binned correction data for local hadronic calibration procedure
 *
 * This class holds binned correction data in multi-dimensional space for
 * classification, weighting, out-of-cluster or dead material correction tools.
 * Data are holded as vector of floats for each bin defined, thereby
 * that at the end they appear to be vector of vector of float. One data
 * set (one CaloLocalHadCoeff) serve for one correction tool.
 *
 * Binning definition is done via conception of 'correction areas'. One area has
 * fixed number of dimenensions, fixed way how these dimensions have been binned
 * and fixed number of correction parameters for each bin.
 *
 * For example, em/had classification tool requires following setup:
 * 3 correction coefficients (em fraction of bin, number of entries in bin, some
 * error in bin) are binned in 4 dimensions: cluster eta, log10(cluster energy),
 * log10(cluster energy density), log10(cluter depth).
 *
 * There could be more, than one, correction area defined for one correction tool.
 * For example, dead material correction have 8 different areas defined for different
 * kind of dead material energy depositions (before presamplers, between emec and hec,
 * before FCAL, etc)
 */


#include "AthenaKernel/CLASS_DEF.h"
#include <vector>
#include <string>
#include <fstream>

class CaloLocalHadCoeff {
  public:

    /**
     * @brief Class defines binning for user dimension
     */
    class LocalHadDimension {
      public:
        /**
         * @brief Default Constructor.
         */
        LocalHadDimension() : m_type(0), m_nbins(0), m_xmin(0.0), m_xmax(0.0), m_dx(0.0) {};

        /**
         * @brief Constructor to define equidistant binning
         * @param title  Dimension title
         * @param type   Dimension type
         * @param xmin   Minimum value for the first bin
         * @param xmax   Maximum value for the last bin
         */
        LocalHadDimension(const char *title, unsigned int typ, int nbins, float xmin, float xmax) :
          m_title(title), m_type(typ), m_nbins(nbins), m_xmin(xmin), m_xmax(xmax)
          {
            m_xbins.resize(0);
            m_dx = (m_xmax-m_xmin)/float(m_nbins);
          }

        /**
         * @brief Constructor to define arbitrary binning
         * @param title  Dimension title
         * @param type   Dimension type
         * @param xbins  Bins borders, vector of size m_nbins+1
         */
        LocalHadDimension(const char *title, unsigned int typ, std::vector<float > &xbins) :
          m_title(title), m_type(typ), m_dx(0.0)
          {
            m_xbins = xbins;
            m_xmin = xbins.front();
            m_xmax = xbins.back();
            m_nbins = xbins.size() -1;
          }
        /**
         * @brief Copy constructor
         */
        LocalHadDimension(const LocalHadDimension &other);
        LocalHadDimension& operator= (const LocalHadDimension &other);

        // access methods

        /// return dimension type
        inline unsigned int getType() const {return m_type;}

        /// return number of bins
        inline int getNbins() const {return m_nbins;}

        /// return minimum value for the first bin
        inline float getXmin() const {return m_xmin;}

        /// return maximum value for the last bin
        inline float getXmax() const {return m_xmax;}

        /// return size of bin
        inline float getDx() const {return m_dx;}

        /// return bin number
        int getBin(float &x) const;

        int getBinAdjusted(float &x, float &xadj) const;

        /// return dimension name
        std::string getTitle() const {return m_title;}

      private:
        /// dimensions title (e.g. "eta", "energy", "lambda", "blabla", etc)
        std::string m_title;

        /// dimension type
        unsigned int m_type;

        /// number of bins
        int m_nbins;

        /// minimum value for the first bin
        float m_xmin;

        /// maximum value for the last bin
        float m_xmax;

        /// bin size (in the case of equidistant binning)
        float m_dx;

        /// bins borders (if dimension has non-equidistant binning), vector of size m_nbins+1
        std::vector<float > m_xbins;
    };

    /**
    * @brief Definition of correction area
    *
    */
    class LocalHadArea {
      public:
        /**
         * @brief Default Constructor.
         */
        LocalHadArea() : m_type(0), m_npars(0), m_offset(0), m_length(0) {};
        /**
         * @brief Constructor.
         * @param title Name of the area
         * @param typ   Type of area
         * @param npars Number of correction parameters
         */
        LocalHadArea(const char *title, int typ, int npars) : m_title(title), m_type(typ), m_npars(npars), m_offset(0), m_length(0) 
        {
          m_dims.resize(0);
          m_dims_loc.resize(0);
        };
        /**
         * @brief Copy constructor
         */
        LocalHadArea(const LocalHadArea &other);
        LocalHadArea& operator= (const LocalHadArea &other);

        /// return area type
        inline unsigned int getType() const { return m_type; }
        /// return number of parameters
        inline int getNpars() const { return m_npars; }
        /// return area offset
        inline int getOffset() const { return m_offset; }
        /// return area length
        inline int getLength() const { return m_length; }
        /// get number of dimensions
        inline int getNdim() const { return (int) m_dims.size(); }
        /// get dimension locator coefficient
        inline int getDimLoc(int i_dim) const { return m_dims_loc[i_dim]; }
        /// return name
        std::string getTitle() const {return m_title;}

        /// set area offset
        void setOffset(int offset) { m_offset = offset; }
        /// set area length
        void setLength(int length) { m_length = length; }
        /// to add new dimension
        void addDimension(LocalHadDimension &dim);
        /// to get dimension
        const CaloLocalHadCoeff::LocalHadDimension *getDimension(int n_dim) const { return &(m_dims[n_dim]); }

      private:
        /// area title
        std::string m_title;

        /// area type
        unsigned int m_type;

        /// number of correction parameters
        int m_npars;

        /// offset for the data in global vector (if more than one area is defined)
        int m_offset;

        /// length of area data block (product of m_nbins over all dimensions defined)
        int m_length;

        /// vector of defined dimensions
        std::vector<LocalHadDimension > m_dims;

        /// locator index for dimensions
        std::vector<int > m_dims_loc;
    };

    /**
     * @brief Correction parameters for one general bin
     */
    typedef std::vector<float> LocalHadCoeff;

    /**
     * @brief Default constructor.
     */
    CaloLocalHadCoeff();

    /**
     * @brief Copy constructor.
     */
    CaloLocalHadCoeff(const CaloLocalHadCoeff &other);
    CaloLocalHadCoeff& operator= (const CaloLocalHadCoeff &other);

    //
    // general access methods
    /// return name
    std::string getTitle() const {return m_title;}

    /// set name
    void setTitle(const std::string &title) {m_title = title; }

    //
    // access methods for areas

    /// return number of areas defined for this data set
    inline int getSizeAreaSet() const { return (int)m_AreaSet.size(); }

    /// add new area
    void addArea(LocalHadArea & theArea);

    /// replace existing area with another one
    void setArea(const int n_area, const LocalHadArea & theArea);

    /// return area
    const LocalHadArea *getArea(int n_area) const;

    /// return area defined for given general iBin
    const LocalHadArea *getAreaFromBin(int iBin) const;

    /// return area (and area index) defined for given general iBin
    const LocalHadArea *getAreaFromBin(int iBin, int &i_area) const;

    //
    // access methods for parameters

    /// return total number of coefficient sets
    inline int getSizeCoeffSet() const { return (int)m_CoeffSet.size(); }

    /// set new data
    void setCoeff(const int iBin, const LocalHadCoeff & theCoeff);

    /// get data for given general bin number
    const LocalHadCoeff * getCoeff(const int & iBin) const;

    /// get data for given area and list of cluster variables
    const LocalHadCoeff * getCoeff(const int & n_area, std::vector<float> &vars) const;

    //
    // access method for bins

    /// calculate general bin from vector of input cluster variables
    int getBin(const int n_area, std::vector<float> &vars) const;

    /// calculate general bin from known bins in dimensions
    int getBin(const int n_area, std::vector<int> &v_indexes) const;

    /// expand general bin into vector of bins for defined dimensions
    int bin2indexes(const int iBin, std::vector<int> &v_dim_indx) const;

    /// are this bins is neighbour to this indexes vector (any of their indexes differ per one ?)
    bool isNeighbour(const int iBinx, std::vector<int> &v_indx) const;

    /// check if general bin is filled
    bool isFilled(const int iBin) const;

    /// for interpolation, build the vector of relative x, and global bins for interpolated array
    /// for area n_area and list of dimensions (all in case dim is empty)
    bool getInterpArrays(const int n_area, const std::vector<int> &dim, std::vector<float> &x,
                         std::vector<double> &xadj, std::vector<unsigned int> &gbin) const; 

  private:

    /// title of data set
    std::string m_title;

    /// vector of correction coefficients
    std::vector<LocalHadCoeff> m_CoeffSet;

    /// vector of correction areas
    std::vector<LocalHadArea> m_AreaSet;
};
//Class Identifier obtained by 'clid CaloLocalHadCoeff'
CLASS_DEF(CaloLocalHadCoeff, 29079131, 1)

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( CaloLocalHadCoeff, 82862607 );

#endif
