/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIGNVALUESELECTOR_H_
#define SIGNVALUESELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {

/**
 * @brief Many of the tools need a sign (>=) and a value (2).  For example, if
 * we want at least two leptons.  This is an intermediate 'base' class that
 * implements all the functionality these tools need in one place.
 */
class SignValueSelector : public EventSelectorBase {
public:
    /**
     * @brief Many of the tools will be supplied with an equality (or inequality) by
     * the user.  They're decoded from the text input and saved in this 'sign'
     * format.
     */
    enum Sign {
        signNOIDEA,  //the user clearly did something wrong, expect to exit / crash
        signEQ,      // ==
        signLT,      // <
        signGT,      // >
        signLTEQ,    // <=
        signGTEQ     // >=
    };

    /**
     * @brief Constructor for tools that need a sign and a value.  For example
     * selectors based on multiplicity and cuts.
     *
     * @param name The name of the tool to be used in print outs
     * @param params The parameters to setup the tool with.  Note that it's not
     * constant because it's edited by the decodesign function (to remove
     * stuff).
     * @param multiplicityMode Will this take both a cut value and a
     * multiplicity. For example b-tagging.  MV1 0.5 >= 2.  Two mv1 tags of
     * atleast 0.5.  The default is the "normal" cut mode for cuts like MET, HT
     * etc.
     * @param cutValueMode The cut value can also be a string, e.g. FixedCutBEff_77
     * The default behaviour is false (the cut value is not a string). Note that
     * cutValueMode = true only makes sense if multiplicityMode = true
     */
    SignValueSelector(const std::string& name, std::string params, bool multiplicityMode = false, bool cutValueMode = false);

    /**
     * @brief The name is generated in the constructor for this kind of tool.
     */
    virtual std::string name() const override;

protected:
    /**
     * @brief Integers are annoying in C++.
     *
     * Mostly used by the NLargeJet selectors.
     */
    void checkValueIsInteger();

    /**
     * @brief Integers are annoying in C++.
     *
     * Your first thought when reading in an integer is probably just to read it
     * in with cin to an integer variable.  This seems to not work, as it'll
     * just read the integer component and ignore the rest of the string.  The
     * approach here then is to read it in as a floating point number and ask
     * that number if it is equal to itself rounded down (integers are,
     * fractional numbers are not)
     *
     * Since the user probably didn't mean to cut on njets >= 2.3 we should
     * interrupt the job and print a useful error message if it doesn't work.
     */
    void checkMultiplicityIsInteger();

    /**
     * @brief Get the sign setup in the constructor.
     */
    Sign sign() const;

    /**
     * @brief Get the cut value assigned in the constructor.
     */
    double value() const;

    /**
     * @brief Get the cut value assigned in the constructor.
     * This is for when the cut is a string
     */
    std::string valueString() const;

    /**
     * @brief Get the cut multiplicity assigned in the constructor.
     *
     * This is used for cuts that take a value and a multiplicity, for example
     * EL_N 25000 >= 2 means at least two electrons with pT > 25 GeV.  You
     * can chain these together to make non-symmetric cuts.  For example
     * EL_N 25000 >= 2
     * EL_N 40000 >= 1
     *
     * @return The multiplicity is actually integer, but it's stored in a double
     * so that we can check it's an integer when we read it in.
     */
    double multiplicity() const;

    /**
     * @brief Take the string params, look for one of the allowed inequalities
     * (or equality) and return that.  It also removes the text from the string
     * so the string can then be passed on to some code to extract the numerical
     * value.
     *
     * @param params The string that contains something like ">= 2".  It will
     * return the " 2" via the params parameter.
     *
     * @return For the above example, the sign ">=" will be configured.
     */
    Sign decodesign(std::string& params);

    /**
     * @brief Compare a cut supplied by the user with the value calculated in
     * the event.  For floating point numbers.
     *
     * @param value The value from the event.
     * @param cut The cut the user wants to apply.
     *
     * @return True if the cut is passed.
     */
    bool checkFloat(double value, double cut) const;

    /**
     * @brief Compare a cut supplied by the user with the value calculated in
     * the event.  For integer  numbers.
     *
     * @param value The value from the event.
     * @param cut The cut the user wants to apply.
     *
     * @return True if the cut is passed.
     */
    bool checkInt(int value, int cut) const;

    /**
     * @brief Convert the sign enum back to a string for use in the name and
     * print outs.
     *
     * @return A short string version of the name
     */
    const std::string signstring() const;

private:
    ///Hold the name of the tool, filled by the constructor.
    std::string m_name;

    ///The sign of the cut, filled by the constructor.
    Sign m_sign;

    ///The value of the cut, filled by the constructor.
    double m_cutvalue;

    ///The value of the cut in the case it's a string, filled by the constructor.
    std::string m_cutvalueString;

    ///The multiplicity for cuts that take both a value and a multiplicity e.g. EL_N 25000 >= 2
    double m_multiplicity;
};

}

#endif
