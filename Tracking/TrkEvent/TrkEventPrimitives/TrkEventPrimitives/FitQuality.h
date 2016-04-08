/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FitQuality.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_TRKFITQUALITY_H
#define TRKEVENTPRIMITIVES_TRKFITQUALITY_H


#include <iostream>
#include <cmath>

class MsgStream;

namespace Trk {

/** @class FitQuality
    @brief Base class to represent and store fit qualities from track
           reconstruction in terms of @f$\chi^2@f$ and number of
           degrees of freedom.

        Its main use is to describe the trajectory fit quality at
        a measurement and of the overall track. However, it can
        be extended as necessary.

        @author Edward.Moyse@cern.ch, Andreas.Salzburger@cern.ch  
*/

        class FitQuality
    {
    public:

        /** default ctor for POOL*/
        FitQuality();
        /** Constructor with @f$ \chi^2 @f$ and @f$ n_{dof} @f$ */ 
        FitQuality (double chiSquared, int numberDoF); 

        /**Constructor hadling double type of NDF*/
        FitQuality (double chiSquared, double numberDoF); 

        /** Copy constructor */
        FitQuality( const FitQuality& rhs);
        
        ///Assignment
        FitQuality & operator=(const FitQuality& /*rhs*/) = default;

        /** Destructor */
        virtual ~FitQuality();

        /**Virtual constructor */
        virtual FitQuality* clone() const;

        /** returns the @f$ \chi^2 @f$ of the overall track fit*/
        double chiSquared() const;

        /** returns the number of degrees of freedom of the overall track or
        vertex fit as integer */
        int  numberDoF() const;

        /** returns the number of degrees of freedom of the overall track or
        vertex fit as double */
        double  doubleNumberDoF() const;

    private:
        double m_chiSquared;
        double m_numberDoF;

    };//end of class definitions 

/**Overload of << operator for MsgStream for debug output*/
    MsgStream& operator << ( MsgStream& sl, const FitQuality& fq);
/**Overload of << operator for std::ostream for debug output*/
    std::ostream& operator << ( std::ostream& sl, const FitQuality& fq);

    inline FitQuality* FitQuality::clone() const
        { return new FitQuality(*this); }

    inline double FitQuality::chiSquared() const
    {
        return m_chiSquared;
    }

    inline int FitQuality::numberDoF() const
    {
        return static_cast<int>(floor(m_numberDoF+0.5));
    }

    inline double  FitQuality::doubleNumberDoF() const
    {
        return m_numberDoF;
    }

} //end ns

#endif // TRKEVENTPRIMITIVES_FITQUALITY_H
