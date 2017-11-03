/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file tauEvent/TauDetails.h
///
/// Declaration of tau details base class 
///
/// Package: Reconstruction/tauEvent
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created by Lukasz Janyst on 2007-01-15.

#ifndef TAU_DETAILS_ANALYSIS_H
#define TAU_DETAILS_ANALYSIS_H
#include "GaudiKernel/MsgStream.h"
#include "tauEvent/TauJetParameters.h"
#include <string>

namespace Analysis
{
    //Base for classes holding algorithm specific details
    class TauDetails
    {
        public:
            /// Default constructor
            TauDetails();

            /// Virtual destructor
            virtual ~TauDetails();
            
            static const float DEFAULT;
            static const int   DEFAULT_INT;

            /// Printing operator
            /// @param o stream to write to
            /// @param detail detail to print
            friend std::ostream& operator<< (
                    std::ostream& o, const Analysis::TauDetails& detail);

            /// Printing operator for pointer
            /// @param o stream to write to
            /// @param detail detail to print
            friend std::ostream& operator<< (
                    std::ostream& o, const TauDetails* detail);

            /// Printing operator
            /// @param o stream to write to
            /// @param detail detail to print
            friend MsgStream& operator<< (
                    MsgStream& o, const TauDetails& detail);

            /// Printing operator
            /// @param o stream to write to
            /// @param detail detail to print
            friend MsgStream& operator<< (
                    MsgStream& o, const TauDetails* detail);


            ///@name TauDetailParameter Tau Detail Paramter accessors
            ///DEPRECATED DO NOT USE
            ///@{
            /// value of parameter (type double)
            virtual double parameter( TauJetParameters :: Detail detail ) const;

            //! value of parameter (type int)  
            virtual int intParameter( TauJetParameters :: Detail detail ) const;

            //! check if parameter exists
            virtual bool hasParameter( TauJetParameters :: Detail detail ) const;

            //! set parameter of type double
            virtual void setParameter( 
                    TauJetParameters :: Detail detail, double param );

            //! set parameter of type int
            virtual void setIntParameter( 
                    TauJetParameters :: Detail detail, int param );        
            ///@}

            virtual const std::string& className() const = 0;

            ///Uses printOn to make a string of the TauDetails
            ///@return string representation of tau details
            std::string toString() const;
        protected:
            ///Printing function
            ///@param o string to print to
            template <class stream>
                void printOn(stream& o) const; 
    }; //Class TauDetails

    template <class stream>
    void TauDetails::printOn(stream& o) const
    {
        o << "TauDetails;";
    }

    inline std::ostream& operator<< (std::ostream& o, const TauDetails& detail)
    {
        detail.printOn(o);
        return o;
    }   

    inline std::ostream& operator<< (std::ostream& o, const TauDetails* detail)
    {
        detail->printOn(o);
        return o;
    }   

    inline  MsgStream& operator<< (MsgStream& o, const TauDetails& detail)
    {
        detail.printOn(o);
        return o;
    }   

    inline MsgStream& operator<< (MsgStream& o, const TauDetails* detail)
    {
        detail->printOn(o);
        return o;
    }   

} // namespace Analysis

#endif // TAU_DETAILS_ANALYSIS_H
